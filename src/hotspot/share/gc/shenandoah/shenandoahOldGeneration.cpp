/*
 * Copyright Amazon.com Inc. or its affiliates. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 *
 */


#include "precompiled.hpp"

#include "gc/shared/strongRootsScope.hpp"
#include "gc/shenandoah/shenandoahCollectorPolicy.hpp"
#include "gc/shenandoah/heuristics/shenandoahAdaptiveHeuristics.hpp"
#include "gc/shenandoah/heuristics/shenandoahAggressiveHeuristics.hpp"
#include "gc/shenandoah/heuristics/shenandoahCompactHeuristics.hpp"
#include "gc/shenandoah/heuristics/shenandoahOldHeuristics.hpp"
#include "gc/shenandoah/heuristics/shenandoahStaticHeuristics.hpp"
#include "gc/shenandoah/shenandoahAsserts.hpp"
#include "gc/shenandoah/shenandoahFreeSet.hpp"
#include "gc/shenandoah/shenandoahHeap.hpp"
#include "gc/shenandoah/shenandoahHeap.inline.hpp"
#include "gc/shenandoah/shenandoahHeapRegion.hpp"
#include "gc/shenandoah/shenandoahMarkClosures.hpp"
#include "gc/shenandoah/shenandoahMark.inline.hpp"
#include "gc/shenandoah/shenandoahMonitoringSupport.hpp"
#include "gc/shenandoah/shenandoahOldGeneration.hpp"
#include "gc/shenandoah/shenandoahOopClosures.inline.hpp"
#include "gc/shenandoah/shenandoahReferenceProcessor.hpp"
#include "gc/shenandoah/shenandoahStringDedup.hpp"
#include "gc/shenandoah/shenandoahUtils.hpp"
#include "gc/shenandoah/shenandoahWorkerPolicy.hpp"
#include "gc/shenandoah/shenandoahYoungGeneration.hpp"
#include "prims/jvmtiTagMap.hpp"
#include "utilities/events.hpp"

class ShenandoahFlushAllSATB : public ThreadClosure {
private:
  SATBMarkQueueSet& _satb_qset;
  uintx _claim_token;

public:
  explicit ShenandoahFlushAllSATB(SATBMarkQueueSet& satb_qset) :
    _satb_qset(satb_qset),
    _claim_token(Threads::thread_claim_token()) { }

  void do_thread(Thread* thread) {
    if (thread->claim_threads_do(true, _claim_token)) {
      // Transfer any partial buffer to the qset for completed buffer processing.
      _satb_qset.flush_queue(ShenandoahThreadLocalData::satb_mark_queue(thread));
    }
  }
};

class ShenandoahProcessOldSATB : public SATBBufferClosure {
private:
  ShenandoahObjToScanQueue*       _queue;
  ShenandoahHeap*                 _heap;
  ShenandoahMarkingContext* const _mark_context;
  size_t                          _trashed_oops;

public:
  explicit ShenandoahProcessOldSATB(ShenandoahObjToScanQueue* q) :
    _queue(q),
    _heap(ShenandoahHeap::heap()),
    _mark_context(_heap->marking_context()),
    _trashed_oops(0) {}

  void do_buffer(void** buffer, size_t size) {
    assert(size == 0 || !_heap->has_forwarded_objects() || _heap->is_concurrent_old_mark_in_progress(), "Forwarded objects are not expected here");
    for (size_t i = 0; i < size; ++i) {
      oop *p = (oop *) &buffer[i];
      ShenandoahHeapRegion* region = _heap->heap_region_containing(*p);
      if (region->is_old() && region->is_active()) {
          ShenandoahMark::mark_through_ref<oop, OLD>(p, _queue, NULL, _mark_context, false);
      } else {
        _trashed_oops++;
      }
    }
  }

  size_t trashed_oops() {
    return _trashed_oops;
  }
};

class ShenandoahPurgeSATBTask : public AbstractGangTask {
private:
  ShenandoahObjToScanQueueSet* _mark_queues;
  volatile size_t             _trashed_oops;

public:
  explicit ShenandoahPurgeSATBTask(ShenandoahObjToScanQueueSet* queues) :
    AbstractGangTask("Purge SATB"),
    _mark_queues(queues),
    _trashed_oops(0) {
    Threads::change_thread_claim_token();
  }

  ~ShenandoahPurgeSATBTask() {
    if (_trashed_oops > 0) {
      log_info(gc)("Purged " SIZE_FORMAT " oops from old generation SATB buffers", _trashed_oops);
    }
  }

  void work(uint worker_id) {
    ShenandoahParallelWorkerSession worker_session(worker_id);
    ShenandoahSATBMarkQueueSet &satb_queues = ShenandoahBarrierSet::satb_mark_queue_set();
    ShenandoahFlushAllSATB flusher(satb_queues);
    Threads::threads_do(&flusher);

    ShenandoahObjToScanQueue* mark_queue = _mark_queues->queue(worker_id);
    ShenandoahProcessOldSATB processor(mark_queue);
    while (satb_queues.apply_closure_to_completed_buffer(&processor)) {}

    Atomic::add(&_trashed_oops, processor.trashed_oops());
  }
};

class ShenandoahConcurrentCoalesceAndFillTask : public AbstractGangTask {
private:
  uint                    _nworkers;
  ShenandoahHeapRegion**  _coalesce_and_fill_region_array;
  uint                    _coalesce_and_fill_region_count;
  volatile bool           _is_preempted;

public:
  ShenandoahConcurrentCoalesceAndFillTask(uint nworkers,
                                          ShenandoahHeapRegion** coalesce_and_fill_region_array,
                                          uint region_count) :
    AbstractGangTask("Shenandoah Concurrent Coalesce and Fill"),
    _nworkers(nworkers),
    _coalesce_and_fill_region_array(coalesce_and_fill_region_array),
    _coalesce_and_fill_region_count(region_count),
    _is_preempted(false) {
  }

  void work(uint worker_id) {
    for (uint region_idx = worker_id; region_idx < _coalesce_and_fill_region_count; region_idx += _nworkers) {
      ShenandoahHeapRegion* r = _coalesce_and_fill_region_array[region_idx];
      if (r->is_humongous()) {
        // There is only one object in this region and it is not garbage,
        // so no need to coalesce or fill.
        continue;
      }

      if (!r->oop_fill_and_coalesce()) {
        // Coalesce and fill has been preempted
        Atomic::store(&_is_preempted, true);
        return;
      }
    }
  }

  // Value returned from is_completed() is only valid after all worker thread have terminated.
  bool is_completed() {
    return !Atomic::load(&_is_preempted);
  }
};

ShenandoahOldGeneration::ShenandoahOldGeneration(uint max_queues, size_t max_capacity, size_t soft_max_capacity)
  : ShenandoahGeneration(OLD, max_queues, max_capacity, soft_max_capacity),
    _coalesce_and_fill_region_array(NEW_C_HEAP_ARRAY(ShenandoahHeapRegion*, ShenandoahHeap::heap()->num_regions(), mtGC)),
    _state(WAITING_FOR_BOOTSTRAP),
    _growth_before_compaction(INITIAL_GROWTH_BEFORE_COMPACTION),
    _min_growth_before_compaction ((ShenandoahMinOldGenGrowthPercent * FRACTIONAL_DENOMINATOR) / 100)
{
  _live_bytes_after_last_mark = ShenandoahHeap::heap()->capacity() * INITIAL_LIVE_FRACTION / FRACTIONAL_DENOMINATOR;
  // Always clear references for old generation
  ref_processor()->set_soft_reference_policy(true);
}

size_t ShenandoahOldGeneration::get_live_bytes_after_last_mark() const {
  return _live_bytes_after_last_mark;
}

void ShenandoahOldGeneration::set_live_bytes_after_last_mark(size_t bytes) {
  _live_bytes_after_last_mark = bytes;
  _growth_before_compaction /= 2;
  if (_growth_before_compaction < _min_growth_before_compaction) {
    _growth_before_compaction = _min_growth_before_compaction;
  }
}

size_t ShenandoahOldGeneration::usage_trigger_threshold() const {
  size_t result = _live_bytes_after_last_mark + (_live_bytes_after_last_mark * _growth_before_compaction) / FRACTIONAL_DENOMINATOR;
  return result;
}

bool ShenandoahOldGeneration::contains(ShenandoahHeapRegion* region) const {
  // TODO: Should this be region->is_old() instead?
  return !region->is_young();
}

void ShenandoahOldGeneration::parallel_heap_region_iterate(ShenandoahHeapRegionClosure* cl) {
  ShenandoahGenerationRegionClosure<OLD> old_regions(cl);
  ShenandoahHeap::heap()->parallel_heap_region_iterate(&old_regions);
}

void ShenandoahOldGeneration::heap_region_iterate(ShenandoahHeapRegionClosure* cl) {
  ShenandoahGenerationRegionClosure<OLD> old_regions(cl);
  ShenandoahHeap::heap()->heap_region_iterate(&old_regions);
}

void ShenandoahOldGeneration::set_concurrent_mark_in_progress(bool in_progress) {
  ShenandoahHeap::heap()->set_concurrent_old_mark_in_progress(in_progress);
}

bool ShenandoahOldGeneration::is_concurrent_mark_in_progress() {
  return ShenandoahHeap::heap()->is_concurrent_old_mark_in_progress();
}

void ShenandoahOldGeneration::cancel_marking() {
  if (is_concurrent_mark_in_progress()) {
    log_info(gc)("Abandon SATB buffers");
    ShenandoahBarrierSet::satb_mark_queue_set().abandon_partial_marking();
  }

  ShenandoahGeneration::cancel_marking();
}

void ShenandoahOldGeneration::prepare_gc() {

  // Now that we have made the old generation parsable, it is safe to reset the mark bitmap.
  assert(state() != FILLING, "Cannot reset old without making it parsable");

  ShenandoahGeneration::prepare_gc();
}

bool ShenandoahOldGeneration::entry_coalesce_and_fill() {
  ShenandoahHeap* const heap = ShenandoahHeap::heap();

  static const char* msg = "Concurrent Coalesce and Fill (OLD)";
  EventMark em("%s", msg);
  // TODO: I don't think we're using these concurrent collection counters correctly.
  TraceCollectorStats tcs(heap->monitoring_support()->concurrent_collection_counters());
  uint workers_count = ShenandoahWorkerPolicy::calc_workers_for_conc_marking();
  ShenandoahConcurrentPhase gc_phase("Coalescing and filling (OLD)", ShenandoahPhaseTimings::coalesce_and_fill);
  ShenandoahWorkerScope scope(heap->workers(), workers_count, msg);

  return coalesce_and_fill();
}

// Make the old generation regions parsable, so they can be safely
// scanned when looking for objects in memory indicated by dirty cards.
bool ShenandoahOldGeneration::coalesce_and_fill() {
  ShenandoahHeap* const heap = ShenandoahHeap::heap();
  transition_to(FILLING);

  ShenandoahOldHeuristics* old_heuristics = heap->old_heuristics();
  WorkGang* workers = heap->workers();
  uint nworkers = workers->active_workers();

  log_debug(gc)("Starting (or resuming) coalesce-and-fill of old heap regions");

  // This code will see the same set of regions to fill on each resumption as it did
  // on the initial run. That's okay because each region keeps track of its own coalesce
  // and fill state. Regions that were filled on a prior attempt will not try to fill again.
  uint coalesce_and_fill_regions_count = old_heuristics->get_coalesce_and_fill_candidates(_coalesce_and_fill_region_array);
  assert(coalesce_and_fill_regions_count <= heap->num_regions(), "Sanity");
  ShenandoahConcurrentCoalesceAndFillTask task(nworkers, _coalesce_and_fill_region_array, coalesce_and_fill_regions_count);

  workers->run_task(&task);
  if (task.is_completed()) {
    old_heuristics->abandon_collection_candidates();
    return true;
  } else {
    // Coalesce-and-fill has been preempted. We'll finish that effort in the future.  Do not invoke
    // ShenandoahGeneration::prepare_gc() until coalesce-and-fill is done because it resets the mark bitmap
    // and invokes set_mark_incomplete().  Coalesce-and-fill depends on the mark bitmap.
    log_debug(gc)("Suspending coalesce-and-fill of old heap regions");
    return false;
  }
}

void ShenandoahOldGeneration::transfer_pointers_from_satb() {
  ShenandoahHeap* heap = ShenandoahHeap::heap();
  shenandoah_assert_safepoint();
  assert(heap->is_concurrent_old_mark_in_progress(), "Only necessary during old marking.");
  log_info(gc)("Transfer SATB buffers");
  uint nworkers = heap->workers()->active_workers();
  StrongRootsScope scope(nworkers);

  ShenandoahPurgeSATBTask purge_satb_task(task_queues());
  heap->workers()->run_task(&purge_satb_task);
}

bool ShenandoahOldGeneration::contains(oop obj) const {
  return ShenandoahHeap::heap()->is_in_old(obj);
}

void ShenandoahOldGeneration::prepare_regions_and_collection_set(bool concurrent) {
  ShenandoahHeap* heap = ShenandoahHeap::heap();
  assert(!heap->is_full_gc_in_progress(), "Only for concurrent and degenerated GC");

  {
    ShenandoahGCPhase phase(concurrent ?
        ShenandoahPhaseTimings::final_update_region_states :
        ShenandoahPhaseTimings::degen_gc_final_update_region_states);
    ShenandoahFinalMarkUpdateRegionStateClosure cl(complete_marking_context());

    parallel_heap_region_iterate(&cl);
    heap->assert_pinned_region_status();
  }

  {
    // This doesn't actually choose a collection set, but prepares a list of
    // regions as 'candidates' for inclusion in a mixed collection.
    ShenandoahGCPhase phase(concurrent ?
        ShenandoahPhaseTimings::choose_cset :
        ShenandoahPhaseTimings::degen_gc_choose_cset);
    ShenandoahHeapLocker locker(heap->lock());
    _old_heuristics->prepare_for_old_collections();
  }

  {
    // Though we did not choose a collection set above, we still may have
    // freed up immediate garbage regions so proceed with rebuilding the free set.
    ShenandoahGCPhase phase(concurrent ?
        ShenandoahPhaseTimings::final_rebuild_freeset :
        ShenandoahPhaseTimings::degen_gc_final_rebuild_freeset);
    ShenandoahHeapLocker locker(heap->lock());
    size_t cset_young_regions, cset_old_regions;
    size_t first_old, last_old, num_old;
    heap->free_set()->prepare_to_rebuild(cset_young_regions, cset_old_regions, first_old, last_old, num_old);
    // This is just old-gen completion.  No future budgeting required here.  The only reason to rebuild the freeset here
    // is in case there was any immediate old garbage identified.
    heap->free_set()->rebuild(cset_young_regions, cset_old_regions);
  }
}

const char* ShenandoahOldGeneration::state_name(State state) {
  switch (state) {
    case WAITING_FOR_BOOTSTRAP: return "Waiting for Bootstrap";
    case FILLING:               return "Coalescing";
    case BOOTSTRAPPING:         return "Bootstrapping";
    case MARKING:               return "Marking";
    case EVACUATING:            return "Evacuating";
    default:
      ShouldNotReachHere();
      return "Unknown";
  }
}

void ShenandoahOldGeneration::transition_to(State new_state) {
  if (_state != new_state) {
    log_info(gc)("Old generation transition from %s to %s", state_name(_state), state_name(new_state));
    validate_transition(new_state);
    _state = new_state;
  }
}

#ifdef ASSERT
// This diagram depicts the expected state transitions for marking the old generation
// and preparing for old collections. When a young generation cycle executes, the
// remembered set scan must visit objects in old regions. Visiting an object which
// has become dead on previous old cycles will result in crashes. To avoid visiting
// such objects, the remembered set scan will use the old generation mark bitmap when
// possible. It is _not_ possible to use the old generation bitmap when old marking
// is active (bitmap is not complete). For this reason, the old regions are made
// parsable _before_ the old generation bitmap is reset. The diagram does not depict
// cancellation of old collections by global or full collections.
//
// When a global collection supersedes an old collection, the global mark still
// "completes" the old mark bitmap. Subsequent remembered set scans may use the
// old generation mark bitmap, but any uncollected old regions must still be made parsable
// before the next old generation cycle begins. For this reason, a global collection may
// create mixed collection candidates and coalesce and fill candidates and will put
// the old generation in the respective states (EVACUATING or FILLING). After a Full GC,
// the mark bitmaps are all reset, all regions are parsable and the mark context will
// not be "complete". After a Full GC, remembered set scans will _not_ use the mark bitmap
// and we expect the old generation to be waiting for bootstrap.
//
//                              +-----------------+
//               +------------> |     FILLING     | <---+
//               |   +--------> |                 |     |
//               |   |          +-----------------+     |
//               |   |            |                     |
//               |   |            | Filling Complete    | <-> A global collection may
//               |   |            v                     |     may move the old generation
//               |   |          +-----------------+     |     directly from waiting for
//               |   +--------> |     WAITING     |     |     bootstrap to filling or
//               |   |    +---- |  FOR BOOTSTRAP  | ----+     evacuating.
//               |   |    |     +-----------------+
//               |   |    |       |
//               |   |    |       | Reset Bitmap
//               |   |    |       v
//               |   |    |     +-----------------+     +----------------------+
//               |   |    |     |    BOOTSTRAP    | <-> |       YOUNG GC       |
//               |   |    |     |                 |     | (RSet Parses Region) |
//               |   |    |     +-----------------+     +----------------------+
//               |   |    |       |
//               |   |    |       | Old Marking
//               |   |    |       v
//               |   |    |     +-----------------+     +----------------------+
//               |   |    |     |     MARKING     | <-> |       YOUNG GC       |
//               |   +--------- |                 |     | (RSet Parses Region) |
//               |        |     +-----------------+     +----------------------+
//               |        |       |
//               |        |       | Has Evacuation Candidates
//               |        |       v
//               |        |     +-----------------+     +--------------------+
//               |        +---> |    EVACUATING   | <-> |      YOUNG GC      |
//               +------------- |                 |     | (RSet Uses Bitmap) |
//                              +-----------------+     +--------------------+
//
//
//
void ShenandoahOldGeneration::validate_transition(State new_state) {
  ShenandoahHeap* heap = ShenandoahHeap::heap();
  switch (new_state) {
    case FILLING:
      assert(_state != BOOTSTRAPPING, "Cannot beging making old regions parsable after bootstrapping");
      assert(heap->is_old_bitmap_stable(), "Cannot begin filling without first completing marking, state is '%s'", state_name(_state));
      assert(_old_heuristics->has_coalesce_and_fill_candidates(), "Cannot begin filling without something to fill.");
      break;
    case WAITING_FOR_BOOTSTRAP:
      // GC cancellation can send us back here from any state.
      assert(!heap->is_concurrent_old_mark_in_progress(), "Cannot become ready for bootstrap during old mark.");
      assert(_old_heuristics->unprocessed_old_collection_candidates() == 0, "Cannot become ready for bootstrap with collection candidates");
      assert(heap->young_generation()->old_gen_task_queues() == nullptr, "Cannot become ready for bootstrap when still setup for bootstrapping.");
      break;
    case BOOTSTRAPPING:
      assert(_state == WAITING_FOR_BOOTSTRAP, "Cannot reset bitmap without making old regions parsable, state is '%s'", state_name(_state));
      assert(_old_heuristics->unprocessed_old_collection_candidates() == 0, "Cannot bootstrap with mixed collection candidates");
      assert(!heap->is_prepare_for_old_mark_in_progress(), "Cannot still be making old regions parsable.");
      break;
    case MARKING:
      assert(_state == BOOTSTRAPPING, "Must have finished bootstrapping before marking, state is '%s'", state_name(_state));
      assert(heap->young_generation()->old_gen_task_queues() != nullptr, "Young generation needs old mark queues.");
      assert(heap->is_concurrent_old_mark_in_progress(), "Should be marking old now.");
      break;
    case EVACUATING:
      assert(_state == WAITING_FOR_BOOTSTRAP || _state == MARKING, "Cannot have old collection candidates without first marking, state is '%s'", state_name(_state));
      assert(_old_heuristics->unprocessed_old_collection_candidates() > 0, "Must have collection candidates here.");
      break;
    default:
      fatal("Unknown new state");
  }
}
#endif

ShenandoahHeuristics* ShenandoahOldGeneration::initialize_heuristics(ShenandoahMode* gc_mode) {
  _old_heuristics = new ShenandoahOldHeuristics(this);
  _old_heuristics->set_guaranteed_gc_interval(ShenandoahGuaranteedOldGCInterval);
  _heuristics = _old_heuristics;
  return _heuristics;
}

void ShenandoahOldGeneration::record_success_concurrent(bool abbreviated) {
  heuristics()->record_success_concurrent(abbreviated);
  ShenandoahHeap::heap()->shenandoah_policy()->record_success_old();
}
