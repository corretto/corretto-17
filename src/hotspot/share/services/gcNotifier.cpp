/*
 * Copyright (c) 2011, 2021, Oracle and/or its affiliates. All rights reserved.
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
#include "classfile/vmClasses.hpp"
#include "classfile/vmSymbols.hpp"
#include "oops/objArrayOop.inline.hpp"
#include "oops/oop.inline.hpp"
#include "runtime/handles.inline.hpp"
#include "runtime/java.hpp"
#include "runtime/javaCalls.hpp"
#include "runtime/mutex.hpp"
#include "runtime/mutexLocker.hpp"
#include "services/gcNotifier.hpp"
#include "services/management.hpp"
#include "services/memoryService.hpp"
#include "memoryManager.hpp"
#include "memory/oopFactory.hpp"
#include "memory/resourceArea.hpp"

GCNotificationRequest *GCNotifier::first_request = NULL;
GCNotificationRequest *GCNotifier::last_request = NULL;

void GCNotifier::pushNotification(GCMemoryManager *mgr, const char *action, const char *cause) {
  // Make a copy of the last GC statistics
  // GC may occur between now and the creation of the notification
  int num_pools = MemoryService::num_memory_pools();
  int max_pauses = mgr->max_pauses_per_cycle();
  int max_concurrent_phases = mgr->max_concurrent_phases_per_cycle();
  // stat is deallocated inside GCNotificationRequest
  GCStatInfo* stat = new(ResourceObj::C_HEAP, mtGC) GCStatInfo(num_pools, max_pauses, max_concurrent_phases);
  mgr->get_last_gc_stat(stat);
  // timestamp is current time in ms
  GCNotificationRequest *request = new GCNotificationRequest(os::javaTimeMillis(),mgr,action,cause,stat);
  addRequest(request);
 }

void GCNotifier::addRequest(GCNotificationRequest *request) {
  MutexLocker ml(Notification_lock, Mutex::_no_safepoint_check_flag);
  if(first_request == NULL) {
    first_request = request;
  } else {
    last_request->next = request;
  }
  last_request = request;
  Notification_lock->notify_all();
}

GCNotificationRequest *GCNotifier::getRequest() {
  MutexLocker ml(Notification_lock, Mutex::_no_safepoint_check_flag);
  GCNotificationRequest *request = first_request;
  if(first_request != NULL) {
    first_request = first_request->next;
  }
  return request;
}

bool GCNotifier::has_event() {
  return first_request != NULL;
}

static Handle getGcInfoBuilder(GCMemoryManager *gcManager,TRAPS) {

  Klass* gcMBeanKlass = Management::com_sun_management_internal_GarbageCollectorExtImpl_klass(CHECK_NH);

  instanceOop i = gcManager->get_memory_manager_instance(THREAD);
  instanceHandle ih(THREAD, i);

  JavaValue result(T_OBJECT);
  JavaCallArguments args(ih);

  JavaCalls::call_virtual(&result,
                          gcMBeanKlass,
                          vmSymbols::getGcInfoBuilder_name(),
                          vmSymbols::getGcInfoBuilder_signature(),
                          &args,
                          CHECK_NH);
  return Handle(THREAD, result.get_oop());
}

static Handle createGcInfo(GCMemoryManager *gcManager, GCStatInfo *gcStatInfo,TRAPS) {

  // Fill the arrays of MemoryUsage objects with before and after GC
  // per pool memory usage

  InstanceKlass* mu_klass = Management::java_lang_management_MemoryUsage_klass(CHECK_NH);

  // The array allocations below should use a handle containing mu_klass
  // as the first allocation could trigger a GC, causing the actual
  // klass oop to move, and leaving mu_klass pointing to the old
  // location.
  objArrayOop bu = oopFactory::new_objArray(mu_klass, MemoryService::num_memory_pools(), CHECK_NH);
  objArrayHandle usage_before_gc_ah(THREAD, bu);
  objArrayOop au = oopFactory::new_objArray(mu_klass, MemoryService::num_memory_pools(), CHECK_NH);
  objArrayHandle usage_after_gc_ah(THREAD, au);

  for (int i = 0; i < MemoryService::num_memory_pools(); i++) {
    Handle before_usage = MemoryService::create_MemoryUsage_obj(gcStatInfo->before_gc_usage_for_pool(i), CHECK_NH);
    Handle after_usage;

    MemoryUsage u = gcStatInfo->after_gc_usage_for_pool(i);
    if (u.max_size() == 0 && u.used() > 0) {
      // If max size == 0, this pool is a survivor space.
      // Set max size = -1 since the pools will be swapped after GC.
      MemoryUsage usage(u.init_size(), u.used(), u.committed(), (size_t)-1);
      after_usage = MemoryService::create_MemoryUsage_obj(usage, CHECK_NH);
    } else {
        after_usage = MemoryService::create_MemoryUsage_obj(u, CHECK_NH);
    }
    usage_before_gc_ah->obj_at_put(i, before_usage());
    usage_after_gc_ah->obj_at_put(i, after_usage());
  }

  // Fill the array of PauseInfo
  InstanceKlass* pi_klass = Management::com_sun_management_PauseInfo_klass(CHECK_NH);

  objArrayOop pi = oopFactory::new_objArray(pi_klass, gcStatInfo->pause_array_used(), CHECK_NH);
  objArrayHandle pause_info_ah(THREAD, pi);

  for (int i = 0; i < gcStatInfo->pause_array_used(); i++) {
    Handle pause_info = MemoryService::create_PauseInfo_obj(gcStatInfo->pause_stat_info_for_index(i), gcManager->phase_prefix(), CHECK_NH);
    pause_info_ah->obj_at_put(i, pause_info());
  }

  // Fill the array of ConcurrentInfo
  InstanceKlass* ci_klass = Management::com_sun_management_ConcurrentInfo_klass(CHECK_NH);

  objArrayOop ci = oopFactory::new_objArray(ci_klass, gcStatInfo->concurrent_array_used(), CHECK_NH);
  objArrayHandle concurrent_info_ah(THREAD, ci);

  for (int i = 0; i < gcStatInfo->concurrent_array_used(); i++) {
    Handle concurrent_info = MemoryService::create_ConcurrentInfo_obj(gcStatInfo->concurrent_stat_info_for_index(i), gcManager->phase_prefix(), CHECK_NH);
    concurrent_info_ah->obj_at_put(i, concurrent_info());
  }

  // Convert the gc cause
  Handle cause = java_lang_String::create_from_str(gcStatInfo->get_gc_cause(), CHECK_NH);

  // Current implementation only has 1 attribute (number of GC threads)
  // The type is 'I'
  objArrayOop extra_args_array = oopFactory::new_objArray(vmClasses::Integer_klass(), 1, CHECK_NH);
  objArrayHandle extra_array (THREAD, extra_args_array);

  JavaCallArguments argsInt;
  argsInt.push_int(gcManager->num_gc_threads());
  Handle extra_arg_val = JavaCalls::construct_new_instance(
                            vmClasses::Integer_klass(),
                            vmSymbols::int_void_signature(),
                            &argsInt,
                            CHECK_NH);

  extra_array->obj_at_put(0,extra_arg_val());

  InstanceKlass* gcInfoklass = Management::com_sun_management_GcInfo_klass(CHECK_NH);

  JavaCallArguments constructor_args(39);
  constructor_args.push_oop(getGcInfoBuilder(gcManager,THREAD));
  constructor_args.push_long(gcStatInfo->gc_index());
  constructor_args.push_long(Management::ticks_to_ns(gcStatInfo->start_time()));
  constructor_args.push_long(Management::ticks_to_ns(gcStatInfo->end_time()));
  constructor_args.push_oop(usage_before_gc_ah);
  constructor_args.push_oop(usage_after_gc_ah);
  constructor_args.push_oop(cause);
  constructor_args.push_long(gcStatInfo->get_previous_end_time());
  constructor_args.push_long(gcStatInfo->get_allocated_since_previous());
  constructor_args.push_long(gcStatInfo->get_allocated_during_collection());
  constructor_args.push_long(gcStatInfo->get_copied_between_pools());
  constructor_args.push_long(gcStatInfo->get_garbage_found());
  constructor_args.push_long(gcStatInfo->get_garbage_collected());
  constructor_args.push_long(gcStatInfo->get_live_in_pools_before_gc());
  constructor_args.push_long(gcStatInfo->get_live_in_pools_after_gc());
  constructor_args.push_oop(pause_info_ah);
  constructor_args.push_int(gcStatInfo->pause_array_used());
  constructor_args.push_oop(concurrent_info_ah);
  constructor_args.push_int(gcStatInfo->concurrent_array_used());
  constructor_args.push_int(JNI_TRUE);
  constructor_args.push_oop(extra_array);

  return JavaCalls::construct_new_instance(
                          gcInfoklass,
                          vmSymbols::com_sun_management_GcInfo_constructor_signature(),
                          &constructor_args,
                          THREAD);
}

void GCNotifier::sendNotification(TRAPS) {
  GCNotifier::sendNotificationInternal(THREAD);
  // Clearing pending exception to avoid premature termination of
  // the service thread
  if (HAS_PENDING_EXCEPTION) {
    CLEAR_PENDING_EXCEPTION;
  }
}

class NotificationMark : public StackObj {
  // This class is used in GCNotifier::sendNotificationInternal to ensure that
  // the GCNotificationRequest object is properly cleaned up, whatever path
  // is used to exit the method.
  GCNotificationRequest* _request;
public:
  NotificationMark(GCNotificationRequest* r) {
    _request = r;
  }
  ~NotificationMark() {
    assert(_request != NULL, "Sanity check");
    delete _request;
  }
};

void GCNotifier::sendNotificationInternal(TRAPS) {
  ResourceMark rm(THREAD);
  HandleMark hm(THREAD);
  GCNotificationRequest *request = getRequest();
  if (request != NULL) {
    NotificationMark nm(request);
    Handle objGcInfo = createGcInfo(request->gcManager, request->gcStatInfo, CHECK);

    Handle objName = java_lang_String::create_from_str(request->gcManager->name(), CHECK);
    Handle objAction = java_lang_String::create_from_str(request->gcAction, CHECK);
    Handle objCause = java_lang_String::create_from_str(request->gcCause, CHECK);
    InstanceKlass* gc_mbean_klass = Management::com_sun_management_internal_GarbageCollectorExtImpl_klass(CHECK);

    instanceOop gc_mbean = request->gcManager->get_memory_manager_instance(THREAD);
    instanceHandle gc_mbean_h(THREAD, gc_mbean);
    if (!gc_mbean_h->is_a(gc_mbean_klass)) {
      THROW_MSG(vmSymbols::java_lang_IllegalArgumentException(),
                "This GCMemoryManager doesn't have a GarbageCollectorMXBean");
    }

    JavaValue result(T_VOID);
    JavaCallArguments args(gc_mbean_h);
    args.push_long(request->timestamp);
    args.push_oop(objName);
    args.push_oop(objAction);
    args.push_oop(objCause);
    args.push_oop(objGcInfo);

    JavaCalls::call_virtual(&result,
                            gc_mbean_klass,
                            vmSymbols::createGCNotification_name(),
                            vmSymbols::createGCNotification_signature(),
                            &args,
                            CHECK);
  }
}

