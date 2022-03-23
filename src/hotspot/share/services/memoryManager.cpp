/*
 * Copyright (c) 2003, 2021, Oracle and/or its affiliates. All rights reserved.
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
#include "classfile/javaClasses.hpp"
#include "classfile/vmSymbols.hpp"
#include "memory/allocation.inline.hpp"
#include "memory/universe.hpp"
#include "oops/oop.inline.hpp"
#include "oops/oopHandle.inline.hpp"
#include "runtime/atomic.hpp"
#include "runtime/handles.inline.hpp"
#include "runtime/javaCalls.hpp"
#include "services/lowMemoryDetector.hpp"
#include "services/management.hpp"
#include "services/memoryManager.hpp"
#include "services/memoryPool.hpp"
#include "services/memoryService.hpp"
#include "services/gcNotifier.hpp"
#include "services/threadService.hpp"
#include "utilities/dtrace.hpp"

MemoryManager::MemoryManager(const char* name) :
  _num_pools(0), _name(name) {}

int MemoryManager::add_pool(MemoryPool* pool) {
  int index = _num_pools;
  assert(index < MemoryManager::max_num_pools, "_num_pools exceeds the max");
  if (index < MemoryManager::max_num_pools) {
    _pools[index] = pool;
    _num_pools++;
  }
  pool->add_manager(this);
  return index;
}

bool MemoryManager::is_manager(instanceHandle mh) const {
  return mh() == Atomic::load(&_memory_mgr_obj).resolve();
}

MemoryManager* MemoryManager::get_code_cache_memory_manager() {
  return new MemoryManager("CodeCacheManager");
}

MemoryManager* MemoryManager::get_metaspace_memory_manager() {
  return new MemoryManager("Metaspace Manager");
}

instanceOop MemoryManager::get_memory_manager_instance(TRAPS) {
  // Must do an acquire so as to force ordering of subsequent
  // loads from anything _memory_mgr_obj points to or implies.
  oop mgr_obj = Atomic::load_acquire(&_memory_mgr_obj).resolve();
  if (mgr_obj == NULL) {
    // It's ok for more than one thread to execute the code up to the locked region.
    // Extra manager instances will just be gc'ed.
    Klass* k = Management::sun_management_ManagementFactoryHelper_klass(CHECK_NULL);

    Handle mgr_name = java_lang_String::create_from_str(name(), CHECK_NULL);

    JavaValue result(T_OBJECT);
    JavaCallArguments args;
    args.push_oop(mgr_name);    // Argument 1

    Symbol* method_name = NULL;
    Symbol* signature = NULL;
    if (is_gc_memory_manager()) {
      Klass* extKlass = Management::com_sun_management_internal_GarbageCollectorExtImpl_klass(CHECK_NULL);
      // com.sun.management.GarbageCollectorMXBean is in jdk.management module which may not be present.
      if (extKlass != NULL) {
        k = extKlass;
      }

      method_name = vmSymbols::createGarbageCollector_name();

      signature = vmSymbols::createGarbageCollector_signature();
      args.push_oop(Handle());      // Argument 2 (for future extension)
    } else {
      method_name = vmSymbols::createMemoryManager_name();
      signature = vmSymbols::createMemoryManager_signature();
    }

    InstanceKlass* ik = InstanceKlass::cast(k);

    JavaCalls::call_static(&result,
                           ik,
                           method_name,
                           signature,
                           &args,
                           CHECK_NULL);

    instanceOop m = (instanceOop) result.get_oop();
    instanceHandle mgr(THREAD, m);

    {
      // Get lock before setting _memory_mgr_obj
      // since another thread may have created the instance
      MutexLocker ml(THREAD, Management_lock);

      // Check if another thread has created the management object.  We reload
      // _memory_mgr_obj here because some other thread may have initialized
      // it while we were executing the code before the lock.
      mgr_obj = Atomic::load(&_memory_mgr_obj).resolve();
      if (mgr_obj != NULL) {
         return (instanceOop)mgr_obj;
      }

      // Get the address of the object we created via call_special.
      mgr_obj = mgr();

      // Use store barrier to make sure the memory accesses associated
      // with creating the management object are visible before publishing
      // its address.  The unlock will publish the store to _memory_mgr_obj
      // because it does a release first.
      Atomic::release_store(&_memory_mgr_obj, OopHandle(Universe::vm_global(), mgr_obj));
    }
  }

  return (instanceOop)mgr_obj;
}

GCPhaseStatInfo::GCPhaseStatInfo() :
  _index(0),
  _start_time(0L),
  _end_time(0L),
  _phase_cause("Unknown"),
  _phase_threads(0L)
  {}

GCPauseStatInfo::GCPauseStatInfo() :
  GCPhaseStatInfo(),
  _operation_start_time(0L),
  _operation_end_time(0L)
  {}

GCConcurrentStatInfo::GCConcurrentStatInfo() :
  GCPhaseStatInfo()
  {}

GCStatInfo::GCStatInfo(int num_pools, int max_pauses, int max_concurrent_cycles) {
  // initialize the arrays for memory usage
  _before_gc_usage_array = NEW_C_HEAP_ARRAY(MemoryUsage, num_pools, mtInternal);
  _after_gc_usage_array  = NEW_C_HEAP_ARRAY(MemoryUsage, num_pools, mtInternal);
  _usage_array_size = num_pools;
  _pause_stat_info_array = NEW_C_HEAP_ARRAY(GCPauseStatInfo, max_pauses, mtInternal);
  _pause_array_size = max_pauses;
  _concurrent_stat_info_array = NEW_C_HEAP_ARRAY(GCConcurrentStatInfo, max_concurrent_cycles, mtInternal);
  _concurrent_array_size = max_concurrent_cycles;

  clear();
}

GCStatInfo::~GCStatInfo() {
  FREE_C_HEAP_ARRAY(MemoryUsage*, _before_gc_usage_array);
  FREE_C_HEAP_ARRAY(MemoryUsage*, _after_gc_usage_array);
  FREE_C_HEAP_ARRAY(GCPauseStatInfo*, _pause_stat_info_array);
  FREE_C_HEAP_ARRAY(GCConcurrentStatInfo*, _concurrent_stat_info_array);
}

void GCStatInfo::set_gc_usage(int pool_index, MemoryUsage usage, bool before_gc) {
  MemoryUsage* gc_usage_array;
  if (before_gc) {
    gc_usage_array = _before_gc_usage_array;
  } else {
    gc_usage_array = _after_gc_usage_array;
  }
  gc_usage_array[pool_index] = usage;
}

void GCStatInfo::clear() {
  _index = 0;
  _start_time = 0L;
  _end_time = 0L;
  for (int i = 0; i < _usage_array_size; i++) ::new (&_before_gc_usage_array[i]) MemoryUsage();
  for (int i = 0; i < _usage_array_size; i++) ::new (&_after_gc_usage_array[i]) MemoryUsage();
  
  //
  // Extended stats
  //
  _gc_cause = GCCause::to_string(GCCause::_no_cause_specified);
  _previous_end_time = 0L;
  _allocated_since_previous = 0L;
  _allocated_during_collection = 0L;
  _copied_between_pools = 0L;
  _garbage_collected = 0L;
  _garbage_found = 0L;
  _app_thread_count_after_gc = 0L;
  _max_app_thread_delay = 0L;
  _total_app_thread_delay = 0L;
  _delayed_app_thread_count = 0L;
  _live_in_pools_before_gc = 0L;
  _live_in_pools_after_gc = 0l;
  for (int i = 0; i < _pause_array_size; i++) ::new (&_pause_stat_info_array[i]) GCPauseStatInfo();
  _pause_array_used = 0;
  for (int i = 0; i < _concurrent_array_size; i++) ::new (&_concurrent_stat_info_array[i]) GCConcurrentStatInfo();
  _concurrent_array_used = 0;
}


GCMemoryManager::GCMemoryManager(const char* name, const char* gc_end_message) :
  MemoryManager(name), _gc_end_message(gc_end_message) {
  _num_collections = 0;
  _last_gc_stat = NULL;
  _last_gc_lock = new Mutex(Mutex::leaf, "_last_gc_lock", true,
                            Mutex::_safepoint_check_never);
  _current_gc_stat = NULL;
  _num_gc_threads = 1;
  _notification_enabled = false;
}

GCMemoryManager::~GCMemoryManager() {
  delete _last_gc_stat;
  delete _last_gc_lock;
  delete _current_gc_stat;
}

void GCMemoryManager::add_pool(MemoryPool* pool) {
  add_pool(pool, true);
}

void GCMemoryManager::add_pool(MemoryPool* pool, bool always_affected_by_gc) {
  int index = MemoryManager::add_pool(pool);
  _pool_always_affected_by_gc[index] = always_affected_by_gc;
}

void GCMemoryManager::initialize_gc_stat_info() {
  assert(MemoryService::num_memory_pools() > 0, "should have one or more memory pools");
  _last_gc_stat = new(ResourceObj::C_HEAP, mtGC) GCStatInfo(MemoryService::num_memory_pools(), max_pauses_per_cycle(), max_concurrent_phases_per_cycle());
  _current_gc_stat = new(ResourceObj::C_HEAP, mtGC) GCStatInfo(MemoryService::num_memory_pools(), max_pauses_per_cycle(), max_concurrent_phases_per_cycle());
  // tracking concurrent collections we need two objects: one to update, and one to
  // hold the publicly available "last (completed) gc" information.
}

void GCMemoryManager::gc_begin(bool recordGCBeginTime, bool recordPreGCUsage,
                               bool recordAccumulatedGCTime) {
  assert(_last_gc_stat != NULL && _current_gc_stat != NULL, "Just checking");
  
  bool recordExtendedStats = false;
  #if INCLUDE_SHENANDOAHGC
  recordExtendedStats = true;
  #endif

  if (recordAccumulatedGCTime) {
    _accumulated_timer.start();
  }
  // _num_collections now increases in gc_end, to count completed collections
  if (recordGCBeginTime) {
    _current_gc_stat->set_index(_num_collections+1);
    _current_gc_stat->set_start_time(Management::timestamp());
  }

  if (recordPreGCUsage) {
    // Keep memory usage of all memory pools
    for (int i = 0; i < MemoryService::num_memory_pools(); i++) {
      MemoryPool* pool = MemoryService::get_memory_pool(i);
      MemoryUsage usage = pool->get_memory_usage();
      _current_gc_stat->set_before_gc_usage(i, usage);
      HOTSPOT_MEM_POOL_GC_BEGIN(
        (char *) name(), strlen(name()),
        (char *) pool->name(), strlen(pool->name()),
        usage.init_size(), usage.used(),
        usage.committed(), usage.max_size());
    }
  }
}

// A collector MUST, even if it does not complete for some reason,
// make a TraceMemoryManagerStats object where countCollection is true,
// to ensure the current gc stat is placed in _last_gc_stat.
void GCMemoryManager::gc_end(bool recordPostGCUsage,
                             bool recordAccumulatedGCTime,
                             bool recordGCEndTime, bool countCollection,
                             GCCause::Cause cause,
                             bool allMemoryPoolsAffected) {

  bool recordExtendedStats = false;
  #if INCLUDE_SHENANDOAHGC
  recordExtendedStats = true;
  #endif

  if (recordAccumulatedGCTime) {
    _accumulated_timer.stop();
  }
  if (recordGCEndTime) {
    _current_gc_stat->set_end_time(Management::timestamp());
  }
  if (recordExtendedStats) {
    _current_gc_stat->set_gc_cause(GCCause::to_string(cause));
    _current_gc_stat->set_app_thread_count_after_gc(ThreadService::get_total_thread_count());
  }

  if (recordPostGCUsage) {
    int i;
    // keep the last gc statistics for all memory pools
    for (i = 0; i < MemoryService::num_memory_pools(); i++) {
      MemoryPool* pool = MemoryService::get_memory_pool(i);
      MemoryUsage usage = pool->get_memory_usage();

      HOTSPOT_MEM_POOL_GC_END(
        (char *) name(), strlen(name()),
        (char *) pool->name(), strlen(pool->name()),
        usage.init_size(), usage.used(),
        usage.committed(), usage.max_size());

      _current_gc_stat->set_after_gc_usage(i, usage);
    }

    // Set last collection usage of the memory pools managed by this collector
    for (i = 0; i < num_memory_pools(); i++) {
      MemoryPool* pool = get_memory_pool(i);
      MemoryUsage usage = pool->get_memory_usage();

      if (allMemoryPoolsAffected || pool_always_affected_by_gc(i)) {
        // Compare with GC usage threshold
        pool->set_last_collection_usage(usage);
        LowMemoryDetector::detect_after_gc_memory(pool);
      }
    }
  }

  if (countCollection) {
    _num_collections++;
    // alternately update two objects making one public when complete
    {
      MutexLocker ml(_last_gc_lock, Mutex::_no_safepoint_check_flag);
      GCStatInfo *tmp = _last_gc_stat;
      _last_gc_stat = _current_gc_stat;
      _current_gc_stat = tmp;
      // reset the current stat for diagnosability purposes
      _current_gc_stat->clear();
      if (recordExtendedStats && recordGCEndTime) {
        _current_gc_stat->set_previous_end_time(_last_gc_stat->end_time());
      }
    }

    if (is_notification_enabled()) {
      GCNotifier::pushNotification(this, _gc_end_message, GCCause::to_string(cause));
    }
  }
}

void GCMemoryManager::reset_gc_stat() {
  _num_collections = 0;
  _accumulated_timer.reset();
}

size_t GCMemoryManager::get_last_gc_stat(GCStatInfo* dest) {
  MutexLocker ml(_last_gc_lock, Mutex::_no_safepoint_check_flag);
  if (_last_gc_stat->gc_index() != 0) {
    dest->set_index(_last_gc_stat->gc_index());
    dest->set_start_time(_last_gc_stat->start_time());
    dest->set_end_time(_last_gc_stat->end_time());
    assert(dest->usage_array_size() == _last_gc_stat->usage_array_size(),
           "Must have same array size");
    size_t len = dest->usage_array_size() * sizeof(MemoryUsage);
    memcpy(dest->before_gc_usage_array(), _last_gc_stat->before_gc_usage_array(), len);
    memcpy(dest->after_gc_usage_array(), _last_gc_stat->after_gc_usage_array(), len);

    // Extended stats
    dest->set_gc_cause(_last_gc_stat->get_gc_cause());
    dest->set_previous_end_time(_last_gc_stat->get_previous_end_time());
    dest->set_allocated_since_previous(_last_gc_stat->get_allocated_since_previous());
    dest->set_allocated_during_collection(_last_gc_stat->get_allocated_during_collection());
    dest->set_copied_between_pools(_last_gc_stat->get_copied_between_pools());
    dest->set_garbage_collected(_last_gc_stat->get_garbage_collected());
    dest->set_garbage_found(_last_gc_stat->get_garbage_found());
    dest->set_app_thread_count_after_gc(_last_gc_stat->get_app_thread_count_after_gc());
    dest->set_max_app_thread_delay(_last_gc_stat->get_max_app_thread_delay());
    dest->set_total_app_thread_delay(_last_gc_stat->get_total_app_thread_delay());
    dest->set_delay_app_thread_count(_last_gc_stat->get_delay_app_thread_count());
    dest->set_gc_thread_count(_last_gc_stat->get_gc_thread_count());
    dest->set_live_in_pools_before_gc(_last_gc_stat->get_live_in_pools_before_gc());
    dest->set_live_in_pools_after_gc(_last_gc_stat->get_live_in_pools_after_gc());
    
    assert(dest->pause_array_size() == _last_gc_stat->pause_array_size(),
           "Must have same array size");
    size_t pause_len = _last_gc_stat->pause_array_used() * sizeof(GCPauseStatInfo);
    memcpy(dest->pause_stat_info_array(), _last_gc_stat->pause_stat_info_array(), pause_len);
    dest->set_pause_array_used(_last_gc_stat->pause_array_used());

    assert(dest->concurrent_array_size() == _last_gc_stat->concurrent_array_size(),
           "Must have same array size");
    size_t concurrent_phase_len = _last_gc_stat->concurrent_array_used() * sizeof(GCConcurrentStatInfo);
    memcpy(dest->concurrent_stat_info_array(), _last_gc_stat->concurrent_stat_info_array(), concurrent_phase_len);
    dest->set_concurrent_array_used(_last_gc_stat->concurrent_array_used());
  }
  return _last_gc_stat->gc_index();
}

jint GCMemoryManager::ext_attribute_info(jmmExtAttributeInfo* info, jint count) {
  info[0].name = "GcThreadCount";
  info[0].type = 'I';
  info[0].description = "Number of GC threads";
  return 1;
}

jlong GCMemoryManager::ext_attribute_values(jvalue* ext_attribute_values) {
  ext_attribute_values[0].i = _num_gc_threads;
  return 1;
}

ConcurrentGCMemoryManager::ConcurrentGCMemoryManager(const char* name, const char* gc_end_message) :
  GCMemoryManager(name, gc_end_message) {
  _num_pauses = 0;
  _accumulated_pause_timer.reset();
}

void ConcurrentGCMemoryManager::pause_begin(const char* phase_cause, size_t phase_threads, bool record_accumulated_pause_time,
                     bool count_pauses, bool record_individual_pauses, bool record_duration,
                     bool record_operation_time, bool record_phase_cause, bool record_phase_threads) {
  if (record_accumulated_pause_time) {
    _accumulated_pause_timer.start();
  }
  if (count_pauses) {
    _num_pauses++;
  }
  if (record_individual_pauses && max_pauses_per_cycle() > 0) {
    int pause_index = _current_gc_stat->pause_array_used();
    assert(pause_index < _current_gc_stat->pause_array_size(),
        "Pause happened beyond the max pauses per cycle defined");
    GCPauseStatInfo* stat_info = _current_gc_stat->pause_stat_info_array();
    stat_info[pause_index].set_index(pause_index);

    if (record_duration) {
      stat_info[pause_index].set_start_time(Management::ticks_to_ns(Management::timestamp()));
    }

    if (record_operation_time) {
      stat_info[pause_index].set_operation_start_time(Management::ticks_to_ns(Management::timestamp()));
    }

    if (record_phase_cause) {
      stat_info[pause_index].set_phase_cause(phase_cause);
    }

    if (record_phase_threads) {
      stat_info[pause_index].set_phase_threads(phase_threads);
    }
  }
}

// When recording individualPauses, a call with cycle_pause set to true must be done for each pause
void ConcurrentGCMemoryManager::pause_end(bool record_accumulated_pause_time,
                   bool record_individual_pauses, bool record_duration,
                   bool record_operation_time, bool cycle_pause) {

  if (record_accumulated_pause_time) {
    _accumulated_pause_timer.stop();
  }
  if (record_individual_pauses && max_pauses_per_cycle() > 0) {
      int pause_index = _current_gc_stat->pause_array_used();
      assert(pause_index < _current_gc_stat->pause_array_size(),
        "Pause happened beyond the max pauses per cycle defined");
    GCPauseStatInfo* stat_info = _current_gc_stat->pause_stat_info_array();

    if (record_duration) {
      stat_info[pause_index].set_end_time(Management::ticks_to_ns(Management::timestamp()));
    }

    if (record_operation_time) {
      stat_info[pause_index].set_operation_end_time(Management::ticks_to_ns(Management::timestamp()));
    }

    // When recording individual pauses, at pause_end must be called at least once with cycle_pause true.
    if (cycle_pause) {
      // Roll to the next pause entry
      _current_gc_stat->set_pause_array_used(pause_index + 1);
    }
  }
}

int ConcurrentGCMemoryManager::concurrent_phase_begin(const char* phase_cause, size_t phase_threads,
                                                      bool record_individual_phase, bool record_duration,
                                                      bool record_phase_cause, bool record_phase_threads) {
  if (record_individual_phase && max_concurrent_phases_per_cycle() > 0) {
    int phase_index = _current_gc_stat->new_concurrent_phase();
    assert(phase_index < _current_gc_stat->concurrent_array_size(),
        "Concurrent phase happened beyond the max concurrent phases per cycle defined");
    GCConcurrentStatInfo* stat_info = _current_gc_stat->concurrent_stat_info_array();
    stat_info[phase_index].set_index(phase_index);

    if (record_duration) {
      stat_info[phase_index].set_start_time(Management::ticks_to_ns(Management::timestamp()));
    }

    if (record_phase_cause) {
      stat_info[phase_index].set_phase_cause(phase_cause);
    }

    if (record_phase_threads) {
      stat_info[phase_index].set_phase_threads(phase_threads);
    }
    return phase_index;
  }
  return 0;
}

void ConcurrentGCMemoryManager::concurrent_phase_end(int phase_index, bool record_individual_phase,
                                                     bool record_duration) {
  if (record_individual_phase && max_concurrent_phases_per_cycle() > 0) {
    assert(phase_index < _current_gc_stat->concurrent_array_size(),
        "Concurrent phase happened beyond the max concurrent phases per cycle defined");
    GCConcurrentStatInfo* stat_info = _current_gc_stat->concurrent_stat_info_array();

    if (record_duration) {
      stat_info[phase_index].set_end_time(Management::ticks_to_ns(Management::timestamp()));
    }
  }
}                                                      

void ConcurrentGCMemoryManager::reset_gc_stat() {
  GCMemoryManager::reset_gc_stat();
  _num_pauses = 0;
  _accumulated_pause_timer.reset();
}
