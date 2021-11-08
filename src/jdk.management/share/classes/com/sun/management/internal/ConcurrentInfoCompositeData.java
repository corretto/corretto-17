/*
 * Copyright (c) 2021, Amazon.com, Inc. or its affiliates. All rights reserved.
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

package com.sun.management.internal;

import java.lang.management.MemoryUsage;
import java.lang.reflect.Method;
import java.lang.reflect.Field;
import java.util.Map;
import java.io.InvalidObjectException;
import javax.management.openmbean.CompositeType;
import javax.management.openmbean.CompositeData;
import javax.management.openmbean.CompositeDataSupport;
import javax.management.openmbean.TabularData;
import javax.management.openmbean.SimpleType;
import javax.management.openmbean.OpenType;
import javax.management.openmbean.OpenDataException;
import com.sun.management.ConcurrentInfo;
import java.security.AccessController;
import java.security.PrivilegedAction;
import sun.management.LazyCompositeData;
import static sun.management.LazyCompositeData.getLong;
import static sun.management.LazyCompositeData.getString;
import sun.management.MappedMXBeanType;
import sun.management.Util;

/**
 * A CompositeData for ConcurrentInfo for the local management support.
 * This class avoids the performance penalty paid to the
 * construction of a CompositeData use in the local case.
 */
public class ConcurrentInfoCompositeData extends LazyCompositeData implements java.io.Serializable {
    private final ConcurrentInfo concurrentInfo;

    public ConcurrentInfoCompositeData(ConcurrentInfo concurrentInfo) {
        this.concurrentInfo = concurrentInfo;
    }

    public ConcurrentInfo getConcurrentInfo() {
        return this.concurrentInfo;
    }

    public static CompositeData toCompositeData(final ConcurrentInfo info) {
        ConcurrentInfoCompositeData cicd =
            new ConcurrentInfoCompositeData(info);
        return cicd.getCompositeData();
    }

    protected CompositeData getCompositeData() {
        // CONTENTS OF THIS ARRAY MUST BE SYNCHRONIZED WITH
        // baseConcurrentInfoItemNames!
        final Object[] concurrentInfoItemValues;

        concurrentInfoItemValues = new Object[] {
            concurrentInfo.getId(),
            concurrentInfo.getStartTimeNanos(),
            concurrentInfo.getEndTimeNanos(),
            concurrentInfo.getDurationNanos(),
            concurrentInfo.getPhaseCause(),
            concurrentInfo.getPhaseGarbageCollectorThreadCount()
        };

        try {
            return new CompositeDataSupport(concurrentInfoCompositeType,
                                            CONCURRENT_INFO_ITEM_NAMES,
                                            concurrentInfoItemValues);
        } catch (OpenDataException e) {
            // Should never reach here
            throw new AssertionError(e);
        }
    }

    private static final String ID                     = "id";
    private static final String START_TIME_NANOS        = "startTimeNanos";
    private static final String END_TIME_NANOS          = "endTimeNanos";
    private static final String DURATION_NANOS          = "durationNanos";
    private static final String PHASE_CAUSE             = "phaseCause";
    private static final String THREAD_COUNT            = "phaseGarbageCollectorThreadCount";

    private static final String[] CONCURRENT_INFO_ITEM_NAMES = {
        ID,
        START_TIME_NANOS,
        END_TIME_NANOS,
        DURATION_NANOS,
        PHASE_CAUSE,
        THREAD_COUNT        
    };

    private static final OpenType<?>[] CONCURRENT_INFO_ITEM_TYPES = {
        SimpleType.LONG,    // ID,
        SimpleType.LONG,    // START_TIME_NANOS,
        SimpleType.LONG,    // END_TIME_NANOS,
        SimpleType.LONG,    // DURATION_NANOS,
        SimpleType.STRING,  // PHASE_CAUSE,
        SimpleType.LONG     // THREAD_COUNT
    };

    public static long getId(CompositeData cd) {
        return getLong(cd, ID);
    }

    public static long getStartTimeNanos(CompositeData cd) {
        return getLong(cd, START_TIME_NANOS);
    }

    public static long getEndTimeNanos(CompositeData cd) {
        return getLong(cd, END_TIME_NANOS);
    }

    public static String getPhaseCause(CompositeData cd) {
        return getString(cd, PHASE_CAUSE);
    }

    public static long getThreadCount(CompositeData cd) {
        return getLong(cd, THREAD_COUNT);
    }

    private static final CompositeType concurrentInfoCompositeType;
    static {
        try {
            concurrentInfoCompositeType = new CompositeType("sun.management.ConcurrentInfoCompositeType",
            "CompositeType for ConcurrentInfo ",
            CONCURRENT_INFO_ITEM_NAMES,
            CONCURRENT_INFO_ITEM_NAMES,
            CONCURRENT_INFO_ITEM_TYPES);
        } catch (OpenDataException e) {
            // Should never reach here
            throw new AssertionError(e);
        }
    }
    
    /**
     * Returns true if the input CompositeData has the expected
     * CompositeType (i.e. contain all attributes with expected
     * names and types).  Otherwise, return false.
     */
    public static void validateCompositeData(CompositeData cd) {
        if (cd == null) {
            throw new NullPointerException("Null CompositeData");
        }

        if (!isTypeMatched(getConcurrentInfoCompositeType(),
                           cd.getCompositeType())) {
           throw new IllegalArgumentException(
                "Unexpected composite type for ConcurrentInfo");
        }
    }
    
    public static CompositeType getConcurrentInfoCompositeType() {
        return concurrentInfoCompositeType;
    }

    private static final long serialVersionUID = -265357581522892189L;
}