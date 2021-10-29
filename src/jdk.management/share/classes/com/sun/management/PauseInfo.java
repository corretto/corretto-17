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
import com.sun.management.internal.PauseInfoCompositeData;

import java.lang.String;

/**
 * A {@code PauseInfo} object represents a pause event impacting
 * all threads in a running Java virtual machine. It includes a
 * {@link String} cause describing the reason for the pause which is
 * specific to the implementation of the Java virtual machine, and
 * unique across all pause reasons.
 * <p>
 * {@code PauseInfo} also has additional, optional, details of the time
 * required for individual steps executed during the pause. The times for
 * the individual steps make it easy to identify potential areas for
 * reducing the total pause duration. Generally, the sum of the
 * additional detailed timing information's times should add up to the
 * total pause duration.
 *
 * @author Paul Hohensee
 * @since  18
 */
public class PauseInfo implements PhaseInfo, CompositeDataView {
    private final long index;
    private final long startTime;
    private final long endTime;
    private final String pauseCause;
    private final long threadsSuspensionTime;
    private final long operationTime;
    private final long cleanupTime;
    private final long threadCount;
    private final CompositeData cdata;


    public PauseInfo(long index,
                     long startTime,
                     long endTime,
                     String pauseCause,
                     long operationStartTime,
                     long operationEndTime,
                     long threadCount) {
        this.index = index;
        this.startTime = startTime;
        this.endTime = endTime;
        this.pauseCause = pauseCause;
        if (startTime > 0 && operationStartTime > 0) {
            this.threadsSuspensionTime = operationStartTime - startTime;
        } else {
            this.threadsSuspensionTime = 0;
        }
        if (operationStartTime > 0 && operationEndTime > 0) {
            this.operationTime = operationEndTime - operationStartTime;
        } else {
            this.operationTime = 0;
        }
        if (endTime > 0 && operationEndTime > 0) {
            this.cleanupTime = endTime - operationEndTime;
        } else {
            this.cleanupTime = 0;
        }
        this.threadCount = threadCount;
        this.cdata = new PauseInfoCompositeData(this);
    }

    private PauseInfo(CompositeData cd) {
        PauseInfoCompositeData.validateCompositeData(cd);

        this.index         = PauseInfoCompositeData.getId(cd);
        this.startTime     = PauseInfoCompositeData.getStartTimeNanos(cd);
        this.endTime       = PauseInfoCompositeData.getEndTimeNanos(cd);
        this.pauseCause    = PauseInfoCompositeData.getPhaseCause(cd);

        this.threadsSuspensionTime = PauseInfoCompositeData.getThreadsSuspensionTimeNanos(cd);
        this.operationTime         = PauseInfoCompositeData.getOperationTimeNanos(cd);
        this.cleanupTime           = PauseInfoCompositeData.getCleanupTimeNanos(cd);
        this.threadCount           = PauseInfoCompositeData.getThreadCount(cd);
        this.cdata                 = cd;
    }

    public String getPhaseType() {
        return "Pause";
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
        return pauseCause;
    }

    /**
     * Approximate time in nanoseconds spent waiting for all threads
     * to suspend. The time returned from this method is commonly
     * known as Time To Safepoint. 
     *
     * @return the time spent waiting for all threads to suspend
     */
    public long getThreadsSuspensionTimeNanos() {
        return threadsSuspensionTime;
    }

    /**
     * Approximate time in nanoseconds required to perform the operation
     * for which suspension of all threads is required. The interval
     * begins when all threads have been suspended and ends when the
     * operation has been completed.
     *
     * @return the time spent on the operation associated with the pause.
     */
    public long getOperationTimeNanos() {
 	    return operationTime;
    }

    /**
     * Approximate time in nanoseconds required for cleanup activities
     * just prior to resumption of application thread execution.
     *
     * @return the time required for cleanup activities.
     */
    public long getPostOperationCleanupTimeNanos() {
     	return cleanupTime;
    }

    /**
     * Return the {@code CompositeData} representation of this
     * {@code PauseInfo}, including any pause-specific attributes.  The
     * returned value will have at least all the attributes described
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

    public static PauseInfo from(CompositeData cd) {
        if (cd == null) {
            return null;
        }

        return new PauseInfo(cd);
    }
}
