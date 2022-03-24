/*
 * Copyright (c) 2003, 2020, Oracle and/or its affiliates. All rights reserved.
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

#ifndef SHARE_SERVICES_MEMORYMANAGER_HPP
#define SHARE_SERVICES_MEMORYMANAGER_HPP

#include "gc/shared/gcCause.hpp"
#include "memory/allocation.hpp"
#include "oops/oop.hpp"
#include "oops/oopsHierarchy.hpp"
#include "runtime/handles.hpp"
#include "runtime/timer.hpp"
#include "services/management.hpp"
#include "services/memoryUsage.hpp"

// A memory manager is responsible for managing one or more memory pools.
// The garbage collector is one type of memory managers responsible
// for reclaiming memory occupied by unreachable objects.  A Java virtual
// machine may have one or more memory managers.   It may
// add or remove memory managers during execution.
// A memory pool can be managed by more than one memory managers.

class MemoryPool;
class GCMemoryManager;
class OopClosure;

class MemoryManager : public CHeapObj<mtInternal> {
protected:
  enum {
    max_num_pools = 10
  };

private:
  MemoryPool* _pools[max_num_pools];
  int         _num_pools;

  const char* _name;

protected:
  volatile OopHandle _memory_mgr_obj;

public:
  MemoryManager(const char* name);

  int num_memory_pools() const           { return _num_pools; }
  MemoryPool* get_memory_pool(int index) {
    assert(index >= 0 && index < _num_pools, "Invalid index");
    return _pools[index];
  }

  int add_pool(MemoryPool* pool);

  bool is_manager(instanceHandle mh) const;

  virtual instanceOop get_memory_manager_instance(TRAPS);
  virtual bool is_gc_memory_manager()    { return false; }

  const char* name() const { return _name; }

  // Static factory methods to get a memory manager of a specific type
  static MemoryManager*   get_code_cache_memory_manager();
  static MemoryManager*   get_metaspace_memory_manager();
};

class GCPhaseStatInfo {
private:
  //
  // General properties
  size_t _index;
  jlong  _start_time;
  jlong  _end_time;

  const char* _phase_cause;
  size_t _phase_threads;

public:
  GCPhaseStatInfo();
  size_t get_index()                   { return _index; }
  jlong get_start_time()               { return _start_time; }
  jlong get_end_time()                 { return _end_time; }
  const char* get_phase_cause()        { return _phase_cause; }
  size_t get_phase_threads()           { return _phase_threads; }

  void set_index(size_t index)                  { _index = index; }
  void set_start_time(jlong start_time)         { _start_time = start_time; }
  void set_end_time(jlong end_time)             { _end_time = end_time; }
  void set_phase_cause(const char* phase_cause) { _phase_cause = phase_cause; }
  void set_phase_threads(size_t phase_threads)  { _phase_threads = phase_threads; }
};

class GCPauseStatInfo : public GCPhaseStatInfo {
private:
  jlong _operation_start_time;
  jlong _operation_end_time;

public:
  GCPauseStatInfo();
  jlong get_operation_start_time()     { return _operation_start_time; }
  jlong get_operation_end_time()       { return _operation_end_time; }

  void set_operation_start_time(jlong operation_start_time)  { _operation_start_time = operation_start_time; }
  void set_operation_end_time(jlong operation_end_time)      { _operation_end_time = operation_end_time; }
};

class GCConcurrentStatInfo : public GCPhaseStatInfo {
public:
  GCConcurrentStatInfo();
};

class GCStatInfo : public ResourceObj {
private:
  size_t _index;
  jlong  _start_time;
  jlong  _end_time;

  // We keep memory usage of all memory pools
  MemoryUsage* _before_gc_usage_array;
  MemoryUsage* _after_gc_usage_array;
  int          _usage_array_size;

  //
  // Extended stats
  //
  const char* _gc_cause;

  // Previous information
  jlong _previous_end_time;

  // Allocation information
  jlong _allocated_since_previous;
  jlong _allocated_during_collection;
  jlong _copied_between_pools;
  
  // Garbage stats
  jlong _garbage_collected;
  jlong _garbage_found;

  // Mutator thread info
  jlong _app_thread_count_after_gc;
  jlong _max_app_thread_delay;
  jlong _total_app_thread_delay;
  jint _delayed_app_thread_count;
  jint _gc_thread_count;

  // Live memory
  jlong _live_in_pools_before_gc;
  jlong _live_in_pools_after_gc;

  // Pause phase information
  GCPauseStatInfo* _pause_stat_info_array;
  int              _pause_array_size;
  int              _pause_array_used;

  // Concurrent phase information
  GCConcurrentStatInfo* _concurrent_stat_info_array;
  int                   _concurrent_array_size;
  volatile int          _concurrent_array_used;

  void set_gc_usage(int pool_index, MemoryUsage, bool before_gc);

public:
  GCStatInfo(int num_pools, int max_pauses, int max_concurrent_phases);
  ~GCStatInfo();

  size_t gc_index()               { return _index; }
  jlong  start_time()             { return _start_time; }
  jlong  end_time()               { return _end_time; }
  int    usage_array_size()       { return _usage_array_size; }
  MemoryUsage before_gc_usage_for_pool(int pool_index) {
    assert(pool_index >= 0 && pool_index < _usage_array_size, "Range checking");
    return _before_gc_usage_array[pool_index];
  }
  MemoryUsage after_gc_usage_for_pool(int pool_index) {
    assert(pool_index >= 0 && pool_index < _usage_array_size, "Range checking");
    return _after_gc_usage_array[pool_index];
  }

  MemoryUsage* before_gc_usage_array() { return _before_gc_usage_array; }
  MemoryUsage* after_gc_usage_array()  { return _after_gc_usage_array; }

  const char* get_gc_cause()              { return _gc_cause; }
  jlong get_previous_end_time()           { return _previous_end_time; }
  jlong get_allocated_since_previous()    { return _allocated_since_previous; }
  jlong get_allocated_during_collection() { return _allocated_during_collection; }
  jlong get_copied_between_pools()        { return _copied_between_pools; }
  jlong get_garbage_collected()           { return _garbage_collected; }
  jlong get_garbage_found()               { return _garbage_found; }
  jlong get_app_thread_count_after_gc()   { return _app_thread_count_after_gc; }
  jlong get_live_in_pools_before_gc()     { return _live_in_pools_before_gc; }
  jlong get_live_in_pools_after_gc()      { return _live_in_pools_after_gc; }
  
  jlong get_max_app_thread_delay()        { return _max_app_thread_delay; }
  jlong get_total_app_thread_delay()      { return _total_app_thread_delay; }
  jint get_delay_app_thread_count()       { return _delayed_app_thread_count; }
  jint get_gc_thread_count()              { return _gc_thread_count; }
  
  GCPauseStatInfo* pause_stat_info_array() { return _pause_stat_info_array; }
  int pause_array_size()                   { return _pause_array_size; }
  int pause_array_used()                   { return _pause_array_used; }
  GCPauseStatInfo pause_stat_info_for_index(int pause_index) {
    assert(pause_index >= 0 && pause_index < _pause_array_used, "Range checking");
    return _pause_stat_info_array[pause_index];
  }

  GCConcurrentStatInfo* concurrent_stat_info_array() { return _concurrent_stat_info_array; }
  int concurrent_array_size()                        { return _concurrent_array_size; }
  int concurrent_array_used()                        { return _concurrent_array_used; }
  int new_concurrent_phase()                         { return Atomic::fetch_and_add(&_concurrent_array_used, 1); }
  GCConcurrentStatInfo concurrent_stat_info_for_index(int concurrent_phase_index) {
    assert(concurrent_phase_index >= 0 && concurrent_phase_index < _concurrent_array_used, "Range checking");
    return _concurrent_stat_info_array[concurrent_phase_index];
  }

  void set_index(size_t index)    { _index = index; }
  void set_start_time(jlong time) { _start_time = time; }
  void set_end_time(jlong time)   { _end_time = time; }
  void set_before_gc_usage(int pool_index, MemoryUsage usage) {
    assert(pool_index >= 0 && pool_index < _usage_array_size, "Range checking");
    set_gc_usage(pool_index, usage, true /* before gc */);
  }
  void set_after_gc_usage(int pool_index, MemoryUsage usage) {
    assert(pool_index >= 0 && pool_index < _usage_array_size, "Range checking");
    set_gc_usage(pool_index, usage, false /* after gc */);
  }

  void set_gc_cause(const char* cause)                                    { _gc_cause = cause; }
  void set_previous_end_time(jlong previous_end_time)                     { _previous_end_time = previous_end_time; }
  void set_allocated_since_previous(jlong allocated_since_previous)       { _allocated_since_previous = allocated_since_previous; }
  void set_allocated_during_collection(jlong allocated_during_collection) { _allocated_during_collection = allocated_during_collection; }
  void set_copied_between_pools(jlong copied_between_pools)               { _copied_between_pools = copied_between_pools; }
  void set_garbage_collected(jlong garbage_collected)                     { _garbage_collected = garbage_collected; }
  void set_garbage_found(jlong garbage_found)                             { _garbage_found = garbage_found; }
  void set_app_thread_count_after_gc(jlong app_thread_count_after_gc)     { _app_thread_count_after_gc = app_thread_count_after_gc; }
  void set_max_app_thread_delay(jlong max_app_thread_delay)               { _max_app_thread_delay = max_app_thread_delay; }
  void set_total_app_thread_delay(jlong total_app_thread_delay)           { _total_app_thread_delay = total_app_thread_delay; }
  void set_delay_app_thread_count(jint delayed_app_thread_count)          { _delayed_app_thread_count = delayed_app_thread_count; }
  void set_gc_thread_count(jint gc_thread_count)                          { _gc_thread_count = gc_thread_count; }
  void set_live_in_pools_before_gc(jlong live_in_pools_before_gc)         { _live_in_pools_before_gc = live_in_pools_before_gc; }
  void set_live_in_pools_after_gc(jlong live_in_pools_after_gc)           { _live_in_pools_after_gc = live_in_pools_after_gc; }
  void set_pause_array_used(int pause_array_used)                         { _pause_array_used = pause_array_used; }
  void set_concurrent_array_used(int concurrent_array_used)               { _concurrent_array_used = concurrent_array_used; }

  void clear();
};

class GCMemoryManager : public MemoryManager {
private:
  // TODO: We should unify the GCCounter and GCMemoryManager statistic
  jlong        _num_collections;
  elapsedTimer _accumulated_timer;
  Mutex*       _last_gc_lock;
  int          _num_gc_threads;
  volatile bool _notification_enabled;
  bool         _pool_always_affected_by_gc[MemoryManager::max_num_pools];

protected:
  GCStatInfo*  _last_gc_stat;
  GCStatInfo*  _current_gc_stat;
  const char*  _gc_end_message;

public:
  GCMemoryManager(const char* name, const char* gc_end_message);
  ~GCMemoryManager();

  void add_pool(MemoryPool* pool);
  void add_pool(MemoryPool* pool, bool always_affected_by_gc);

  bool pool_always_affected_by_gc(int index) {
    assert(index >= 0 && index < num_memory_pools(), "Invalid index");
    return _pool_always_affected_by_gc[index];
  }

  void   initialize_gc_stat_info();

  bool   is_gc_memory_manager()            { return true; }
  virtual jlong  gc_time_ms()              { return _accumulated_timer.milliseconds(); }
  virtual jlong  gc_count()                { return _num_collections; }
  virtual jlong  gc_time_ns()              { return _accumulated_timer.nanoseconds(); }
  virtual jlong  gc_running_time_ns()      { return _accumulated_timer.nanoseconds(); }
  virtual jlong gc_pause_count()           { return _num_collections; }
  virtual int max_pauses_per_cycle()       { return 0; }
  virtual int max_concurrent_phases_per_cycle() { return 0;}
  virtual const char* phase_prefix()       { return ""; }
  jlong    num_gc_threads() const          { return _num_gc_threads; }
  void   set_num_gc_threads(int count)  { _num_gc_threads = count; }

  virtual void   gc_begin(bool recordGCBeginTime, bool recordPreGCUsage,
                bool recordAccumulatedGCTime);
  virtual void   gc_end(bool recordPostGCUsage, bool recordAccumulatedGCTime,
                bool recordGCEndTime, bool countCollection, GCCause::Cause cause,
                bool allMemoryPoolsAffected);

  virtual void   reset_gc_stat();

  // Copy out _last_gc_stat to the given destination, returning
  // the collection count. Zero signifies no gc has taken place.
  size_t get_last_gc_stat(GCStatInfo* dest);

  virtual jlong ext_attribute_info_size() { return 1; }
  virtual jint  ext_attribute_info(jmmExtAttributeInfo* info, jint count);
  virtual jlong ext_attribute_values(jvalue* ext_attribute_values);

  void set_notification_enabled(bool enabled) { _notification_enabled = enabled; }
  bool is_notification_enabled() { return _notification_enabled; }
};

class ConcurrentGCMemoryManager : public GCMemoryManager {
private:
  size_t       _num_pauses;
  elapsedTimer _accumulated_pause_timer;
public:
  ConcurrentGCMemoryManager(const char* name, const char* gc_end_message);

  jlong gc_time_ms() override         { return _accumulated_pause_timer.milliseconds(); }
  jlong gc_time_ns() override         { return _accumulated_pause_timer.nanoseconds(); }
  jlong gc_pause_count() override    { return _num_pauses; }

  void   pause_begin(const char* phase_cause, size_t phase_threads, bool record_accumulated_pause_time,
                     bool count_pauses, bool record_individual_pauses, bool record_duration,
                     bool record_operation_time, bool record_phase_cause, bool record_phase_threads);

  void   pause_end(bool record_accumulated_pause_time,
                   bool record_individual_pauses, bool record_duration,
                   bool record_operation_time, bool cycle_pause);

  int concurrent_phase_begin(const char* phase_cause, size_t phase_threads, bool record_individual_phase,
                            bool record_duration, bool record_phase_cause, bool record_phase_threads);

  void concurrent_phase_end(int phase_index, bool record_individual_phase, bool record_duration);

  virtual void reset_gc_stat() override;
};

#endif // SHARE_SERVICES_MEMORYMANAGER_HPP
