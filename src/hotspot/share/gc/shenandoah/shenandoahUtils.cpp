/*
 * Copyright (c) 2017, 2021, Red Hat, Inc. All rights reserved.
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

#include "jfr/jfrEvents.hpp"
#include "gc/shared/gcCause.hpp"
#include "gc/shared/gcTrace.hpp"
#include "gc/shared/gcWhen.hpp"
#include "gc/shared/referenceProcessorStats.hpp"
#include "gc/shenandoah/heuristics/shenandoahHeuristics.hpp"
#include "gc/shenandoah/shenandoahCollectorPolicy.hpp"
#include "gc/shenandoah/shenandoahGeneration.hpp"
#include "gc/shenandoah/shenandoahHeap.inline.hpp"
#include "gc/shenandoah/shenandoahReferenceProcessor.hpp"
#include "gc/shenandoah/shenandoahMemoryManager.hpp"
#include "gc/shenandoah/shenandoahUtils.hpp"
#include "gc/shenandoah/shenandoahYoungGeneration.hpp"
#include "utilities/debug.hpp"

ShenandoahPhaseTimings::Phase ShenandoahTimingsTracker::_current_phase = ShenandoahPhaseTimings::_invalid_phase;

ShenandoahGCSession::ShenandoahGCSession(GCCause::Cause cause, ShenandoahGeneration* generation) :
  _heap(ShenandoahHeap::heap()),
  _generation(generation),
  _timer(_heap->gc_timer()),
  _tracer(_heap->tracer()) {
  assert(!ShenandoahGCPhase::is_current_phase_valid(), "No current GC phase");

  _heap->set_gc_cause(cause);
  _heap->set_gc_generation(generation);
  _timer->register_gc_start();
  _tracer->report_gc_start(cause, _timer->gc_start());
  _heap->trace_heap_before_gc(_tracer);

  _heap->shenandoah_policy()->record_cycle_start();
  generation->heuristics()->record_cycle_start();
  _trace_cycle.initialize(_heap->cycle_memory_manager(), cause,
          /* allMemoryPoolsAffected */    true,
          /* recordGCBeginTime = */       true,
          /* recordPreGCUsage = */        true,
          /* recordPeakUsage = */         true,
          /* recordPostGCUsage = */       true,
          /* recordAccumulatedGCTime = */ true,
          /* recordGCEndTime = */         true,
          /* countCollection = */         true
  );
  _trace_memory_manager_stats.initialize(_heap->memory_manager(generation->generation_mode()), cause,
          /* allMemoryPoolsAffected */    true,
          /* recordGCBeginTime = */       true,
          /* recordPreGCUsage = */        true,
          /* recordPeakUsage = */         true,
          /* recordPostGCUsage = */       true,
          /* recordAccumulatedGCTime = */ true,
          /* recordGCEndTime = */         true,
          /* countCollection = */         true
  );
}


ShenandoahGCSession::~ShenandoahGCSession() {

  _generation->heuristics()->record_cycle_end();
  if (_heap->mode()->is_generational() &&
      ((_generation->generation_mode() == GLOBAL) || _heap->upgraded_to_full())) {
    // If we just completed a GLOBAL GC, claim credit for completion of young-gen and old-gen GC as well
    _heap->young_generation()->heuristics()->record_cycle_end();
    _heap->old_generation()->heuristics()->record_cycle_end();
  }
  _timer->register_gc_end();
  _heap->trace_heap_after_gc(_tracer);
  _tracer->report_gc_reference_stats(_generation->ref_processor()->reference_process_stats());
  _tracer->report_gc_end(_timer->gc_end(), _timer->time_partitions());
  assert(!ShenandoahGCPhase::is_current_phase_valid(), "No current GC phase");
  _heap->set_gc_cause(GCCause::_no_gc);
}

ShenandoahGCPauseMark::ShenandoahGCPauseMark(uint gc_id, SvcGCMarker::reason_type type, GenerationMode generation_mode) :
  _heap(ShenandoahHeap::heap()), _gc_id_mark(gc_id), _svc_gc_mark(type), _is_gc_active_mark() {
  _trace_pause.initialize(_heap->stw_memory_manager(), _heap->gc_cause(),
          /* allMemoryPoolsAffected */    true,
          /* recordGCBeginTime = */       true,
          /* recordPreGCUsage = */        false,
          /* recordPeakUsage = */         false,
          /* recordPostGCUsage = */       false,
          /* recordAccumulatedGCTime = */ true,
          /* recordGCEndTime = */         true,
          /* countCollection = */         true
  );
  _trace_gc_pause_stats.initialize(_heap->memory_manager(generation_mode),
          /* phase_cause = */                   NULL,
          /* phase_threads = */                 0L,
          /* record_accumulated_pause_time = */ true,
          /* count_pauses = */                  true,
          /* record_individual_pauses = */      true,
          /* record_duration = */               false,
          /* record_operation_time = */         true,
          /* record_phase_cause = */            false,
          /* record_phase_threads = */          false,
          /* cycle_pause = */                   false);
}

ShenandoahPausePhase::ShenandoahPausePhase(const char* title, ShenandoahPhaseTimings::Phase phase, GenerationMode generation_mode, size_t num_workers, bool log_heap_usage) :
  ShenandoahTimingsTracker(phase),
  _tracer(title, NULL, GCCause::_no_gc, log_heap_usage),
  _timer(ShenandoahHeap::heap()->gc_timer()) {
  _timer->register_gc_pause_start(title);
  ShenandoahHeap::heap()->memory_manager(generation_mode)->pause_begin(
          /* phase_cause = */                   NULL,
          /* phase_threads = */                 num_workers,
          /* record_accumulated_pause_time = */ false,
          /* count_pauses = */                  false,
          /* record_individual_pauses = */      true,
          /* record_duration = */               false,
          /* record_operation_time = */         false,
          /* record_phase_cause = */            false,
          /* record_phase_threads = */          true);
}

ShenandoahPausePhase::~ShenandoahPausePhase() {
  _timer->register_gc_pause_end();
}

ShenandoahConcurrentPhase::ShenandoahConcurrentPhase(const char* title, ShenandoahPhaseTimings::Phase phase, GenerationMode generation_mode, size_t num_workers, bool log_heap_usage) :
  ShenandoahTimingsTracker(phase),
  _tracer(title, NULL, GCCause::_no_gc, log_heap_usage),
  _timer(ShenandoahHeap::heap()->gc_timer()) {
  ShenandoahHeap* const heap = ShenandoahHeap::heap();
  _timer->register_gc_concurrent_start(title);
  // Concurrent uncommit is excluded as it happens outside any GC cycle
  if (phase != ShenandoahPhaseTimings::conc_uncommit) {
    _trace_gc_concurrent_stats.initialize(heap->memory_manager(generation_mode),
            /* phase_cause = */               ShenandoahPhaseTimings::phase_name(phase), 
            /* phase_threads = */             num_workers,
            /* record_individual_phases = */  true,
            /* record_duration = */           true,
            /* record_phase_cause */          true,
            /* record_phase_threads = */      true);
  }
}

ShenandoahConcurrentPhase::~ShenandoahConcurrentPhase() {
  _timer->register_gc_concurrent_end();
}

ShenandoahTimingsTracker::ShenandoahTimingsTracker(ShenandoahPhaseTimings::Phase phase) :
  _timings(ShenandoahHeap::heap()->phase_timings()), _phase(phase) {
  assert(!Thread::current()->is_Worker_thread() &&
              (Thread::current()->is_VM_thread() ||
               Thread::current()->is_ConcurrentGC_thread()),
          "Must be set by these threads");
  _parent_phase = _current_phase;
  _current_phase = phase;
  _start = os::elapsedTime();
}

ShenandoahTimingsTracker::~ShenandoahTimingsTracker() {
  _timings->record_phase_time(_phase, os::elapsedTime() - _start);
  _current_phase = _parent_phase;
}

bool ShenandoahTimingsTracker::is_current_phase_valid() {
  return _current_phase < ShenandoahPhaseTimings::_num_phases;
}

ShenandoahGCPhase::ShenandoahGCPhase(ShenandoahPhaseTimings::Phase phase) :
  ShenandoahTimingsTracker(phase),
  _timer(ShenandoahHeap::heap()->gc_timer()) {
  _timer->register_gc_phase_start(ShenandoahPhaseTimings::phase_name(phase), Ticks::now());
}

ShenandoahGCPhase::~ShenandoahGCPhase() {
  _timer->register_gc_phase_end(Ticks::now());
}

ShenandoahGCWorkerPhase::ShenandoahGCWorkerPhase(const ShenandoahPhaseTimings::Phase phase) :
    _timings(ShenandoahHeap::heap()->phase_timings()), _phase(phase) {
  _timings->record_workers_start(_phase);
}

ShenandoahGCWorkerPhase::~ShenandoahGCWorkerPhase() {
  _timings->record_workers_end(_phase);
}

ShenandoahWorkerSession::ShenandoahWorkerSession(uint worker_id) : _worker_id(worker_id) {
  Thread* thr = Thread::current();
  assert(ShenandoahThreadLocalData::worker_id(thr) == ShenandoahThreadLocalData::INVALID_WORKER_ID, "Already set");
  ShenandoahThreadLocalData::set_worker_id(thr, worker_id);
}

ShenandoahConcurrentWorkerSession::~ShenandoahConcurrentWorkerSession() {
  _event.commit(GCId::current(), ShenandoahPhaseTimings::phase_name(ShenandoahGCPhase::current_phase()));
}

ShenandoahParallelWorkerSession::~ShenandoahParallelWorkerSession() {
  _event.commit(GCId::current(), _worker_id, ShenandoahPhaseTimings::phase_name(ShenandoahGCPhase::current_phase()));
}
ShenandoahWorkerSession::~ShenandoahWorkerSession() {
#ifdef ASSERT
  Thread* thr = Thread::current();
  assert(ShenandoahThreadLocalData::worker_id(thr) != ShenandoahThreadLocalData::INVALID_WORKER_ID, "Must be set");
  ShenandoahThreadLocalData::set_worker_id(thr, ShenandoahThreadLocalData::INVALID_WORKER_ID);
#endif
}
