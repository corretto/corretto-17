/*
 * Copyright (c) 2022, Red Hat, Inc. All rights reserved.
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

#ifndef SHARE_GC_SHENANDOAH_SHENANDOAHOBJECTUTILS_HPP
#define SHARE_GC_SHENANDOAH_SHENANDOAHOBJECTUTILS_HPP

#include "memory/allStatic.hpp"
#include "oops/markWord.hpp"
#include "oops/oopsHierarchy.hpp"

class Klass;

class ShenandoahObjectUtils : public AllStatic {
public:
#ifdef _LP64
  static inline markWord stable_mark(oop obj);
#endif
  static inline Klass* klass(oop obj);
  static inline size_t size(oop obj);
};

#endif // SHARE_GC_SHENANDOAH_SHENANDOAHOBJECTUTILS_HPP
