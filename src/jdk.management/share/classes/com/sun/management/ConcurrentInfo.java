/*
 * Copyright (c) Amazon.com Inc. or its affiliates. All rights reserved.
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
import com.sun.management.internal.ConcurrentInfoCompositeData;

import java.lang.String;

/**
 * A {@code ConcurrentInfo} object represents a concurrent phase
 * within a garbage collection. It includes a timestamp specifying
 * the time since the launch of the Java virtual machine process
 * at which the phase started, the wall clock time duration of the
 * phase, plus additional, optional information.
 *
 * @author Paul Hohensee
 * @since  18
 */
public class ConcurrentInfo implements PhaseInfo, CompositeDataView {
    private final long index;
    private final long startTime;
    private final long endTime;
    private final long threadCount;
    private final String phaseCause;
    private final CompositeData cdata;

    public ConcurrentInfo(long index, long startTime, long endTime, long threadCount, String phaseCause) {
        this.index = index;
        this.startTime = startTime;
        this.endTime = endTime;
        this.threadCount = threadCount;
        this.phaseCause = phaseCause;
        this.cdata = new ConcurrentInfoCompositeData(this);
    }

    public ConcurrentInfo(CompositeData cd) {
        ConcurrentInfoCompositeData.validateCompositeData(cd);

        this.index         = ConcurrentInfoCompositeData.getId(cd);
        this.startTime     = ConcurrentInfoCompositeData.getStartTimeNanos(cd);
        this.endTime       = ConcurrentInfoCompositeData.getEndTimeNanos(cd);
        this.threadCount   = ConcurrentInfoCompositeData.getThreadCount(cd);
        this.phaseCause     = ConcurrentInfoCompositeData.getPhaseCause(cd);
        this.cdata         = cd;
    }

    public String getPhaseType() {
        return "Concurrent";
    }

    public long getId() {
        return index;
    }

    public long getStartTimeNanos() {
        return startTime;
    }

    public long getEndTimeNanos() {
        return endTime;
    }

    public long getPhaseGarbageCollectorThreadCount() {
        return threadCount;
    }

    public String getPhaseCause() {
        return phaseCause;
    }

    /**
     * Return the {@code CompositeData} representation of this
     * {@code ConcurrentInfo}, including any phase-specific attributes. 
     * The returned value will have at least all the attributes described
     * in the {@link #from(CompositeData) from} method.
     *
     * @param ct the {@code CompositeType} that the caller expects.
     * This parameter is ignored and can be null.
     *
     * @return the {@code CompositeData} representation.
     */
    public CompositeData toCompositeData(CompositeType ct) {
        return cdata;
    }

    public static ConcurrentInfo from(CompositeData cd) {
        if (cd == null) {
            return null;
        }

        return new ConcurrentInfo(cd);
    }
}
