/*
 * Copyright (c) 2021, Red Hat, Inc. All rights reserved.
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

#include "gc/shenandoah/shenandoahBarrierSet.hpp"
#include "gc/shenandoah/shenandoahClosures.inline.hpp"
#include "gc/shenandoah/shenandoahGeneration.hpp"
#include "gc/shenandoah/shenandoahGenerationType.hpp"
#include "gc/shenandoah/shenandoahMark.inline.hpp"
#include "gc/shenandoah/shenandoahOopClosures.inline.hpp"
#include "gc/shenandoah/shenandoahReferenceProcessor.hpp"
#include "gc/shenandoah/shenandoahTaskqueue.inline.hpp"
#include "gc/shenandoah/shenandoahUtils.hpp"
#include "gc/shenandoah/shenandoahVerifier.hpp"

ShenandoahMarkRefsSuperClosure::ShenandoahMarkRefsSuperClosure(ShenandoahObjToScanQueue* q,  ShenandoahReferenceProcessor* rp, ShenandoahObjToScanQueue* old_q) :
  MetadataVisitingOopIterateClosure(rp),
  _stringDedup_requests(),
  _queue(q),
  _old_queue(old_q),
  _mark_context(ShenandoahHeap::heap()->marking_context()),
  _weak(false)
{ }

ShenandoahMark::ShenandoahMark(ShenandoahGeneration* generation) :
  _generation(generation),
  _task_queues(generation->task_queues()),
  _old_gen_task_queues(generation->old_gen_task_queues()) {
}

template <ShenandoahGenerationType GENERATION, bool CANCELLABLE, StringDedupMode STRING_DEDUP>
void ShenandoahMark::mark_loop_prework(uint w, TaskTerminator *t, ShenandoahReferenceProcessor *rp, bool update_refs) {
  ShenandoahObjToScanQueue* q = get_queue(w);
  ShenandoahObjToScanQueue* old_q = get_old_queue(w);

  ShenandoahHeap* const heap = ShenandoahHeap::heap();
  ShenandoahLiveData* ld = heap->get_liveness_cache(w);

  // TODO: We can clean up this if we figure out how to do templated oop closures that
  // play nice with specialized_oop_iterators.
  if (heap->unload_classes()) {
    if (update_refs) {
      using Closure = ShenandoahMarkUpdateRefsMetadataClosure<GENERATION, STRING_DEDUP>;
      Closure cl(q, rp, old_q);
      mark_loop_work<Closure, GENERATION, CANCELLABLE>(&cl, ld, w, t);
    } else {
      using Closure = ShenandoahMarkRefsMetadataClosure<GENERATION, STRING_DEDUP>;
      Closure cl(q, rp, old_q);
      mark_loop_work<Closure, GENERATION, CANCELLABLE>(&cl, ld, w, t);
    }
  } else {
    if (update_refs) {
      using Closure = ShenandoahMarkUpdateRefsClosure<GENERATION, STRING_DEDUP>;
      Closure cl(q, rp, old_q);
      mark_loop_work<Closure, GENERATION, CANCELLABLE>(&cl, ld, w, t);
    } else {
      using Closure = ShenandoahMarkRefsClosure<GENERATION, STRING_DEDUP>;
      Closure cl(q, rp, old_q);
      mark_loop_work<Closure, GENERATION, CANCELLABLE>(&cl, ld, w, t);
    }
  }

  heap->flush_liveness_cache(w);
}

template<bool CANCELLABLE, StringDedupMode STRING_DEDUP>
void ShenandoahMark::mark_loop(ShenandoahGenerationType generation, uint worker_id, TaskTerminator* terminator,
                               ShenandoahReferenceProcessor *rp) {
  bool update_refs = ShenandoahHeap::heap()->has_forwarded_objects();
  switch (generation) {
    case YOUNG:
      mark_loop_prework<YOUNG, CANCELLABLE, STRING_DEDUP>(worker_id, terminator, rp, update_refs);
      break;
    case OLD:
      // Old generation collection only performs marking, it should not update references.
      mark_loop_prework<OLD, CANCELLABLE, STRING_DEDUP>(worker_id, terminator, rp, false);
      break;
    case GLOBAL_GEN:
      mark_loop_prework<GLOBAL_GEN, CANCELLABLE, STRING_DEDUP>(worker_id, terminator, rp, update_refs);
      break;
    case GLOBAL_NON_GEN:
      mark_loop_prework<GLOBAL_NON_GEN, CANCELLABLE, STRING_DEDUP>(worker_id, terminator, rp, update_refs);
      break;
    default:
      ShouldNotReachHere();
      break;
  }
}

void ShenandoahMark::mark_loop(ShenandoahGenerationType generation, uint worker_id, TaskTerminator* terminator,
                               ShenandoahReferenceProcessor *rp, bool cancellable, StringDedupMode dedup_mode) {
  if (cancellable) {
    switch(dedup_mode) {
      case NO_DEDUP:
        mark_loop<true, NO_DEDUP>(generation, worker_id, terminator, rp);
        break;
      case ENQUEUE_DEDUP:
        mark_loop<true, ENQUEUE_DEDUP>(generation, worker_id, terminator, rp);
        break;
      case ALWAYS_DEDUP:
        mark_loop<true, ALWAYS_DEDUP>(generation, worker_id, terminator, rp);
        break;
    }
  } else {
    switch(dedup_mode) {
      case NO_DEDUP:
        mark_loop<false, NO_DEDUP>(generation, worker_id, terminator, rp);
        break;
      case ENQUEUE_DEDUP:
        mark_loop<false, ENQUEUE_DEDUP>(generation, worker_id, terminator, rp);
        break;
      case ALWAYS_DEDUP:
        mark_loop<false, ALWAYS_DEDUP>(generation, worker_id, terminator, rp);
        break;
    }
  }
}

template <class T, ShenandoahGenerationType GENERATION, bool CANCELLABLE>
void ShenandoahMark::mark_loop_work(T* cl, ShenandoahLiveData* live_data, uint worker_id, TaskTerminator *terminator) {
  uintx stride = ShenandoahMarkLoopStride;

  ShenandoahHeap* heap = ShenandoahHeap::heap();
  ShenandoahObjToScanQueueSet* queues = task_queues();
  ShenandoahObjToScanQueue* q;
  ShenandoahMarkTask t;

  assert(heap->active_generation()->type() == GENERATION,
         "Expected %s, but got: %s", shenandoah_generation_name(heap->active_generation()->type()), shenandoah_generation_name(GENERATION));
  heap->active_generation()->ref_processor()->set_mark_closure(worker_id, cl);

  /*
   * Process outstanding queues, if any.
   *
   * There can be more queues than workers. To deal with the imbalance, we claim
   * extra queues first. Since marking can push new tasks into the queue associated
   * with this worker id, we come back to process this queue in the normal loop.
   */
  assert(queues->get_reserved() == heap->workers()->active_workers(),
         "Need to reserve proper number of queues: reserved: %u, active: %u", queues->get_reserved(), heap->workers()->active_workers());

  q = queues->claim_next();
  while (q != NULL) {
    if (CANCELLABLE && heap->check_cancelled_gc_and_yield()) {
      return;
    }

    for (uint i = 0; i < stride; i++) {
      if (q->pop(t)) {
        do_task<T>(q, cl, live_data, &t);
      } else {
        assert(q->is_empty(), "Must be empty");
        q = queues->claim_next();
        break;
      }
    }
  }
  q = get_queue(worker_id);
  ShenandoahObjToScanQueue* old_q = get_old_queue(worker_id);

  ShenandoahSATBBufferClosure<GENERATION> drain_satb(q, old_q);
  SATBMarkQueueSet& satb_mq_set = ShenandoahBarrierSet::satb_mark_queue_set();

  /*
   * Normal marking loop:
   */
  while (true) {
    if (CANCELLABLE && heap->check_cancelled_gc_and_yield()) {
      return;
    }
    while (satb_mq_set.completed_buffers_num() > 0) {
      satb_mq_set.apply_closure_to_completed_buffer(&drain_satb);
    }

    uint work = 0;
    for (uint i = 0; i < stride; i++) {
      if (q->pop(t) ||
          queues->steal(worker_id, t)) {
        do_task<T>(q, cl, live_data, &t);
        work++;
      } else {
        break;
      }
    }

    if (work == 0) {
      // No work encountered in current stride, try to terminate.
      // Need to leave the STS here otherwise it might block safepoints.
      ShenandoahSuspendibleThreadSetLeaver stsl(CANCELLABLE && ShenandoahSuspendibleWorkers);
      ShenandoahTerminatorTerminator tt(heap);
      if (terminator->offer_termination(&tt)) return;
    }
  }
}
