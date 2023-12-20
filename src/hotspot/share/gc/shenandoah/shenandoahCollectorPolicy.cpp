/*
 * Copyright (c) 2013, 2021, Red Hat, Inc. All rights reserved.
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

#include "gc/shenandoah/shenandoahCollectorPolicy.hpp"
#include "gc/shenandoah/shenandoahGC.hpp"
#include "gc/shenandoah/shenandoahHeap.inline.hpp"
#include "runtime/os.hpp"

ShenandoahCollectorPolicy::ShenandoahCollectorPolicy() :
  _success_concurrent_gcs(0),
  _mixed_gcs(0),
  _abbreviated_gcs(0),
  _success_old_gcs(0),
  _interrupted_old_gcs(0),
  _success_degenerated_gcs(0),
  _success_full_gcs(0),
  _consecutive_young_gcs(0),
  _consecutive_degenerated_gcs(0),
  _alloc_failure_degenerated(0),
  _alloc_failure_degenerated_upgrade_to_full(0),
  _alloc_failure_full(0),
  _explicit_concurrent(0),
  _explicit_full(0),
  _implicit_concurrent(0),
  _implicit_full(0),
  _cycle_counter(0) {

  Copy::zero_to_bytes(_degen_points, sizeof(size_t) * ShenandoahGC::_DEGENERATED_LIMIT);

  _tracer = new (ResourceObj::C_HEAP, mtGC) ShenandoahTracer();

}

void ShenandoahCollectorPolicy::record_explicit_to_concurrent() {
  _explicit_concurrent++;
}

void ShenandoahCollectorPolicy::record_explicit_to_full() {
  _explicit_full++;
}

void ShenandoahCollectorPolicy::record_implicit_to_concurrent() {
  _implicit_concurrent++;
}

void ShenandoahCollectorPolicy::record_implicit_to_full() {
  _implicit_full++;
}

void ShenandoahCollectorPolicy::record_alloc_failure_to_full() {
  _alloc_failure_full++;
}

void ShenandoahCollectorPolicy::record_alloc_failure_to_degenerated(ShenandoahGC::ShenandoahDegenPoint point) {
  assert(point < ShenandoahGC::_DEGENERATED_LIMIT, "sanity");
  _alloc_failure_degenerated++;
  _degen_points[point]++;
}

void ShenandoahCollectorPolicy::record_degenerated_upgrade_to_full() {
  ShenandoahHeap::heap()->record_upgrade_to_full();
  _consecutive_degenerated_gcs = 0;
  _alloc_failure_degenerated_upgrade_to_full++;
}

void ShenandoahCollectorPolicy::record_success_concurrent(bool is_young) {
  _consecutive_degenerated_gcs = 0;
  if (is_young) {
    _consecutive_young_gcs++;
  } else {
    _consecutive_young_gcs = 0;
  }
  _success_concurrent_gcs++;
}

void ShenandoahCollectorPolicy::record_mixed_cycle() {
  _mixed_gcs++;
}

void ShenandoahCollectorPolicy::record_abbreviated_cycle() {
  _abbreviated_gcs++;
}

void ShenandoahCollectorPolicy::record_success_old() {
  _consecutive_young_gcs = 0;
  _success_old_gcs++;
}

void ShenandoahCollectorPolicy::record_interrupted_old() {
  _consecutive_young_gcs = 0;
  _interrupted_old_gcs++;
}

void ShenandoahCollectorPolicy::record_success_degenerated(bool is_young, bool is_upgraded_to_full) {
  if (!is_upgraded_to_full) {
    _consecutive_degenerated_gcs++;
  }
  if (is_young) {
    _consecutive_young_gcs++;
  } else {
    _consecutive_young_gcs = 0;
  }
  _success_degenerated_gcs++;
}

void ShenandoahCollectorPolicy::record_success_full() {
  _consecutive_degenerated_gcs = 0;
  _consecutive_young_gcs = 0;
  _success_full_gcs++;
}

size_t ShenandoahCollectorPolicy::cycle_counter() const {
  return _cycle_counter;
}

void ShenandoahCollectorPolicy::record_cycle_start() {
  _cycle_counter++;
}

void ShenandoahCollectorPolicy::record_shutdown() {
  _in_shutdown.set();
}

bool ShenandoahCollectorPolicy::is_at_shutdown() {
  return _in_shutdown.is_set();
}

// This may be called by mutator threads.  We declare _success_full_gcs volatile to force the value not to be cached
// in a local register or variable by a mutator thread that is checking this value in a loop.
size_t ShenandoahCollectorPolicy::get_fullgc_count() {
  return _success_full_gcs + _alloc_failure_degenerated_upgrade_to_full;
}
void ShenandoahCollectorPolicy::print_gc_stats(outputStream* out) const {
  out->print_cr("Under allocation pressure, concurrent cycles may cancel, and either continue cycle");
  out->print_cr("under stop-the-world pause or result in stop-the-world Full GC. Increase heap size,");
  out->print_cr("tune GC heuristics, set more aggressive pacing delay, or lower allocation rate");
  out->print_cr("to avoid Degenerated and Full GC cycles. Abbreviated cycles are those which found");
  out->print_cr("enough regions with no live objects to skip evacuation.");
  out->cr();

  out->print_cr(SIZE_FORMAT_W(5) " Successful Concurrent GCs",         _success_concurrent_gcs);
  out->print_cr("  " SIZE_FORMAT_W(5) " invoked explicitly",           _explicit_concurrent);
  out->print_cr("  " SIZE_FORMAT_W(5) " invoked implicitly",           _implicit_concurrent);
  out->cr();

  out->print_cr(SIZE_FORMAT_W(5) " Completed Old GCs",                 _success_old_gcs);
  out->print_cr("  " SIZE_FORMAT_W(5) " mixed",                        _mixed_gcs);
  out->print_cr("  " SIZE_FORMAT_W(5) " interruptions",                _interrupted_old_gcs);
  out->cr();

  out->print_cr(SIZE_FORMAT_W(5) " Degenerated GCs",                   _success_degenerated_gcs);
  out->print_cr("  " SIZE_FORMAT_W(5) " caused by allocation failure", _alloc_failure_degenerated);
  for (int c = 0; c < ShenandoahGC::_DEGENERATED_LIMIT; c++) {
    if (_degen_points[c] > 0) {
      const char* desc = ShenandoahGC::degen_point_to_string((ShenandoahGC::ShenandoahDegenPoint)c);
      out->print_cr("    " SIZE_FORMAT_W(5) " happened at %s",         _degen_points[c], desc);
    }
  }
  out->print_cr("  " SIZE_FORMAT_W(5) " upgraded to Full GC",          _alloc_failure_degenerated_upgrade_to_full);
  out->cr();

  out->print_cr(SIZE_FORMAT_W(5) " Abbreviated GCs",                   _abbreviated_gcs);
  out->cr();

  out->print_cr(SIZE_FORMAT_W(5) " Full GCs",                          _success_full_gcs + _alloc_failure_degenerated_upgrade_to_full);
  out->print_cr("  " SIZE_FORMAT_W(5) " invoked explicitly",           _explicit_full);
  out->print_cr("  " SIZE_FORMAT_W(5) " invoked implicitly",           _implicit_full);
  out->print_cr("  " SIZE_FORMAT_W(5) " caused by allocation failure", _alloc_failure_full);
  out->print_cr("  " SIZE_FORMAT_W(5) " upgraded from Degenerated GC", _alloc_failure_degenerated_upgrade_to_full);
}
