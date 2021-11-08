/*
 * Copyright (c) 2021, Amazon.com, Inc. or its affiliates. All rights reserved.
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

#ifndef SHARE_GC_SHENANDOAH_SHENANDOAHMEMORYMANAGER_HPP
#define SHARE_GC_SHENANDOAH_SHENANDOAHMEMORYMANAGER_HPP

#include "gc/shenandoah/shenandoahHeap.hpp"
#include "services/memoryManager.hpp"

class ShenandoahMemoryManager : public ConcurrentGCMemoryManager {
protected:
   ShenandoahHeap* _heap;
   ShenandoahGeneration* _generation;
   jlong _allocated_since_previous_start;

public:
  ShenandoahMemoryManager(ShenandoahHeap* heap, ShenandoahGeneration* generation,
                          const char* name, const char* gc_end_message);

  virtual void gc_begin(bool recordGCBeginTime, bool recordPreGCUsage,
              bool recordAccumulatedGCTime) override;

  virtual void gc_end(bool recordPostGCUsage, bool recordAccumulatedGCTime,
              bool recordGCEndTime, bool countCollection, GCCause::Cause cause,
              bool allMemoryPoolsAffected) override;

  virtual void gc_requested();
  virtual void update_copied_between_pools(jlong copied_before_reset);
  virtual void report_garbage(jlong found, jlong collected);
  
  virtual int max_pauses_per_cycle() override { return 4; }
  virtual int max_concurrent_phases_per_cycle() override { return 20; }
  virtual const char* phase_prefix() override { return "Shenandoah "; }
};

class ShenandoahGlobalMemoryManager : public ShenandoahMemoryManager {
public:
  ShenandoahGlobalMemoryManager(ShenandoahHeap* heap);
};

class ShenandoahYoungGenMemoryManager : public ShenandoahMemoryManager {
public:
  ShenandoahYoungGenMemoryManager(ShenandoahHeap* heap);
};

class ShenandoahOldGenMemoryManager : public ShenandoahMemoryManager {
private:
  bool _interrupted;
  jlong _completed_cycles;
public:
  ShenandoahOldGenMemoryManager(ShenandoahHeap* heap);
  virtual void gc_end(bool recordPostGCUsage, bool recordAccumulatedGCTime,
                      bool recordGCEndTime, bool countCollection, GCCause::Cause cause,
                      bool allMemoryPoolsAffected) override;
  virtual size_t ext_attribute_info_size() override;
  virtual size_t ext_attribute_info(jmmExtAttributeInfo* info, jint count) override;
  virtual size_t ext_attribute_values(jvalue* ext_attribute_values) override;
};

#endif // SHARE_GC_SHENANDOAH_SHENANDOAHMEMORYMANAGER_HPP
