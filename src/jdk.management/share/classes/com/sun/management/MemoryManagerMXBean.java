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
 * Platform-specific management interface for a memory manager of
 * the Java virtual machine
 *
 * <p> This platform extension is only available to a memory
 * manager implementation that supports this extension.
 *
 * @author  Paul Hohensee
 * @since   18
 */
public interface MemoryManagerMXBean
    extends java.lang.management.MemoryManagerMXBean {

    /**
     * Indicate whether or not the manager is currently valid. A memory
     * manager may be removed by a Java virtual machine and become invalid.
     *
     * @return {@code true} if the manager is valid; {@code false} otherwise.
     */
    public default boolean isValid() {
        return false;
    }

    /**
     * Returns, as a {@link String}, the version of the MemoryManagerMXBean
     * API. The format is majorVersion.minorVersion.microVersion. 
     * <p>
     * Memory manager specific attributes, if any, can be obtained via the
     * {@link CompositeData CompositeData} interface.
     * <p>
     * <b>MBeanServer access</b>:<br>
     * The mapped type of {@code MemoryType} is {@code String}.
     *
     * @return a {@code String} object representing the memory manager version;
     *         or {@code null} if no version information is available.
     */
    public default String getVersion() {
        return null;
    }
}
