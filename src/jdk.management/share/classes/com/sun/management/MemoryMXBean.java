/*
 * Copyright (c) 2021, Amazon.com Inc. or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Oracle designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Oracle in the LICENSE file that accompanied this code.
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
 */

package com.sun.management;

import javax.management.openmbean.CompositeData;
import javax.management.openmbean.CompositeDataView;
import javax.management.openmbean.CompositeType;

import java.lang.String;

/**
 * Platform-specific management interface for a garbage collector
 * which performs collections in cycles.
 * <p>
 * GarbageCollectorMXBean is an interface used by the management system to
 * access garbage collector properties and reset the values of associated
 * counter and accumulator properties. 
 *
 * <p> This platform extension is only available to the garbage
 * collection implementation that supports this extension.
 *
 * @author  Paul Hohensee
 * @since   18
 */
public interface MemoryMXBean
    extends java.lang.management.MemoryMXBean {

    /**
     * Version of the MemoryMXBean application programming interface. 
     * The format for the version is majorVersion.minorVersion.microVersion.
     *
     * @return the version of the MemoryMXBean application programming interface;
     *         the null string if no version information is available.
     */
    public default String getVersion() {
        return "";
    }

    /**
     * Returns an approximation of the total amount of memory, in bytes,
     * allocated in Java object heap memory since the start of JVM execution.
     * The returned value is an approximation because some Java virtual machine
     * implementations may use object allocation mechanisms that result in a
     * delay between the time an object is allocated and the time its size is
     * recorded.
     *
     * @return an approximation of the total memory allocated, in bytes, in
     *         Java object heap memory since the start of JVM execution, if
     *         memory allocation measurement is enabled; {@code -1} otherwise.
     *
     * @throws UnsupportedOperationException if the Java virtual
     *         machine implementation does not support memory allocation
     *         measurement.
     */
    public default long getAllocatedBytes() {
        return -1;
    }
}
