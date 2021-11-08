/*
 * Copyright (c) 2003, 2021, Oracle and/or its affiliates. All rights reserved.
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

import java.lang.management.MemoryUsage;
import java.lang.management.MemoryPoolMXBean;
import javax.management.openmbean.CompositeData;
import javax.management.openmbean.CompositeDataView;
import javax.management.openmbean.CompositeType;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import com.sun.management.internal.GcInfoCompositeData;
import com.sun.management.internal.GcInfoBuilder;

/**
 * Garbage collection information.  It contains the following
 * information for one garbage collection as well as GC-specific
 * attributes:
 * <blockquote>
 * <ul>
 *   <li>Start time</li>
 *   <li>End time</li>
 *   <li>Duration</li>
 *   <li>Memory usage before the collection starts</li>
 *   <li>Memory usage after the collection ends</li>
 * </ul>
 * </blockquote>
 *
 * <p>
 * {@code GcInfo} is a {@link CompositeData CompositeData}
 * The GC-specific attributes can be obtained via the CompositeData
 * interface.  This is a historical relic, and other classes should
 * not copy this pattern.  Use {@link CompositeDataView} instead.
 *
 * <h2>MXBean Mapping</h2>
 * {@code GcInfo} is mapped to a {@link CompositeData CompositeData}
 * with attributes as specified in the {@link #from from} method.
 *
 * @author  Mandy Chung
 * @since   1.5
 */
public class GcInfo implements CompositeData, CompositeDataView {
    private final long index;
    private final long startTime;
    private final long endTime;
    private final Map<String, MemoryUsage> usageBeforeGc;
    private final Map<String, MemoryUsage> usageAfterGc;
    private final String gcCause;
    private final long previousEndTime;
    private final long allocatedSincePrevious;
    private final long allocatedDuringCollection;
    private final long copiedBetweenPools;
    private final long garbageFound;
    private final long garbageCollected;
    private final long liveInPoolsBeforeGc;
    private final long liveInPoolsAfterGc;
    private final PauseInfo[] pauseInfoArray;
    private final ConcurrentInfo[] concurrentInfoArray;
    private final Object[] extAttributes;
    private final boolean valid;
    private final CompositeData cdata;
    private final GcInfoBuilder builder;

    private GcInfo(GcInfoBuilder builder,
                   long index, long startTime, long endTime,
                   MemoryUsage[] muBeforeGc,
                   MemoryUsage[] muAfterGc,
                   String gcCause, 
                   long previousEndTime,
                   long allocatedSincePrevious,
                   long allocatedDuringCollection,
                   long copiedBetweenPools,
                   long garbageFound,
                   long garbageCollected,
                   long liveInPoolsBeforeGc,
                   long liveInPoolsAfterGc,
                   PauseInfo[] pauseInfo,
                   int pauseInfoSize,
                   ConcurrentInfo[] concurrentInfo,
                   int concurrentInfoSize,
                   boolean valid,
                   Object[] extAttributes) {
        this.builder       = builder;
        this.index         = index;
        this.startTime     = startTime;
        this.endTime       = endTime;
        String[] poolNames = builder.getPoolNames();
        this.usageBeforeGc = new HashMap<String, MemoryUsage>(poolNames.length);
        this.usageAfterGc = new HashMap<String, MemoryUsage>(poolNames.length);
        for (int i = 0; i < poolNames.length; i++) {
            this.usageBeforeGc.put(poolNames[i],  muBeforeGc[i]);
            this.usageAfterGc.put(poolNames[i],  muAfterGc[i]);
        }
        this.gcCause = gcCause;
        this.previousEndTime = previousEndTime;
        this.allocatedSincePrevious = allocatedSincePrevious;
        this.allocatedDuringCollection = allocatedDuringCollection;
        this.copiedBetweenPools = copiedBetweenPools;
        this.garbageFound = garbageFound;
        this.garbageCollected = garbageCollected;
        this.liveInPoolsBeforeGc = liveInPoolsBeforeGc;
        this.liveInPoolsAfterGc = liveInPoolsAfterGc;
        this.pauseInfoArray = new PauseInfo[pauseInfoSize];
        System.arraycopy(pauseInfo, 0, this.pauseInfoArray, 0, pauseInfoSize);
        this.concurrentInfoArray = new ConcurrentInfo[concurrentInfoSize];
        System.arraycopy(concurrentInfo, 0, this.concurrentInfoArray, 0, concurrentInfoSize);
        this.valid = valid;
        this.extAttributes = extAttributes;
        try {
            this.cdata = new GcInfoCompositeData(this, builder, extAttributes);
        } catch (Exception e) {
            System.out.println("Exception");
            e.printStackTrace();
            throw new RuntimeException(e);
        }
    }

    private GcInfo(CompositeData cd) {
        GcInfoCompositeData.validateCompositeData(cd);

        this.index         = GcInfoCompositeData.getId(cd);
        this.startTime     = GcInfoCompositeData.getStartTimeNanos(cd);
        this.endTime       = GcInfoCompositeData.getEndTimeNanos(cd);
        this.usageBeforeGc = GcInfoCompositeData.getMemoryUsageBeforeGc(cd);
        this.usageAfterGc  = GcInfoCompositeData.getMemoryUsageAfterGc(cd);

        this.gcCause                   = GcInfoCompositeData.getGarbageCollectionCause(cd);
        this.previousEndTime           = GcInfoCompositeData.getPreviousEndTimeNanos(cd);
        this.allocatedSincePrevious    = GcInfoCompositeData.getAllocatedBetweenEndOfPreviousAndStart(cd);
        this.allocatedDuringCollection = GcInfoCompositeData.getAllocatedDuringCollection(cd);
        this.copiedBetweenPools        = GcInfoCompositeData.getCopiedBetweenPools(cd);
        this.garbageFound              = GcInfoCompositeData.getGarbageFound(cd);
        this.garbageCollected          = GcInfoCompositeData.getGarbageCollected(cd);
        this.liveInPoolsBeforeGc       = GcInfoCompositeData.getLiveInPoolsBeforeGc(cd);
        this.liveInPoolsAfterGc        = GcInfoCompositeData.getLiveInPoolsAfterGc(cd);
        this.valid                     = GcInfoCompositeData.isValid(cd);

        this.pauseInfoArray = null;
        this.concurrentInfoArray = null;
        this.extAttributes = null;
        this.builder       = null;
        this.cdata         = cd;
    }

    /**
     * Returns the identifier of this garbage collection, which is
     * the number of collections that this collector has done.
     *
     * @return the identifier of this garbage collection, which is
     * the number of collections that this collector has done.
     */
    public long getId() {
        return index;
    }

    /**
     * Returns the start time of this GC in milliseconds
     * since the Java virtual machine was started.
     *
     * @return the start time of this GC.
     */
    public long getStartTime() {
        return startTime / 1000_000L;
    }

    /**
     * Returns the end time of this GC in milliseconds
     * since the Java virtual machine was started.
     *
     * @return the end time of this GC.
     */
    public long getEndTime() {
        return endTime / 1000_000L;
    }

    /**
     * Returns the elapsed time of this GC in milliseconds.
     *
     * @return the elapsed time of this GC in milliseconds.
     */
    public long getDuration() {
        return (endTime - startTime) / 1000_000L;
    }

    /**
     * Returns the memory usage of all memory pools
     * at the beginning of this GC.
     * This method returns
     * a {@code Map} of the name of a memory pool
     * to the memory usage of the corresponding
     * memory pool before GC starts.
     *
     * @return a {@code Map} of memory pool names to the memory
     * usage of a memory pool before GC starts.
     */
    public Map<String, MemoryUsage> getMemoryUsageBeforeGc() {
        return Collections.unmodifiableMap(usageBeforeGc);
    }

    /**
     * Returns the memory usage of all memory pools
     * at the end of this GC.
     * This method returns
     * a {@code Map} of the name of a memory pool
     * to the memory usage of the corresponding
     * memory pool when GC finishes.
     *
     * @return a {@code Map} of memory pool names to the memory
     * usage of a memory pool when GC finishes.
     */
    public Map<String, MemoryUsage> getMemoryUsageAfterGc() {
        return Collections.unmodifiableMap(usageAfterGc);
    }

    /**
     * Whether the data returned by this {@code GCDetails} object has passed
     * all validity tests.
     *
     * @return {@code true} if the data in this {@code GCDetails} is valid;
     *         otherwise, {@code false}.
     */
    public boolean isValid() {
        return valid;
    }

    /**
     * The reason for this collection. The most common one is when the Java
     * virtual machine detects that all available Java heap space has been
     * occupied, or that the Java virtual machine estimates that all available
     * space will soon be occupied. A Java virtual machine might use "Max" as
     * the string to identify this cause for starting a collection, or
     * possibly "Alo" for an object allocation failure. The reason is unique
     * across all collectors supported by the Java virtual machine.
     *
     * @return a {@link String} representing the reason for this collection.
     */
    public String getGarbageCollectionCause() {
        return gcCause;
    }

    /**
     * At the start of this collection, the approximate number of bytes occupied
     * by live objects in the memory pools affected by the associated collector.
     * If this operation is not supported, this method returns {@code -1}.
     * <p>
     * Note that "the memory pools affected by the associated collector" may
     * not be the same as "the memory pools affected by this collection".
     *
     * @return the approximate number of bytes occupied by live objects in the
     *         memory pools affected by the associated collector at the start
     *         of this collection; or {@code -1} if this operation is not
     *         supported.
     */
    public long getLiveInPoolsBeforeGc() {
        return liveInPoolsBeforeGc;
    }

    /**
     * At the end of this collection, the approximate number of bytes occupied
     * by live objects in the memory pools affected by the associated collector.
     * If this operation is not supported, this method returns {@code -1}.
     * <p>
     * Note that "the memory pools affected by the associated collector" may
     * not be the same as "the memory pools affected by this collection".
     *
     * @return the approximate number of bytes occupied by live objects in the
     *         memory pools affected by the associated collector at the end of
     *         this collection; or {@code -1} if this operation is not supported.
     */
    public long getLiveInPoolsAfterGc() {
        return liveInPoolsAfterGc;
    }

    /**
     * The aggregate size, in bytes, of all unreachable objects discovered
     * by the associated collector during this collection.
     *
     * @return the aggregate size, in bytes, of all unreachable objects
     *         discovered by the associated collector during this collection.
     */
    public long getGarbageFound() {
        return garbageFound;
    }

    /**
     * The aggregate size, in bytes, of all objects collected by the
     * associated collector during this collection.
     *
     * @return the aggregate size, in bytes, of all objects collected by
     *         the associated collector during this collection.
    */
    public long getGarbageCollected() {
        return garbageCollected;
    }

    /**
     * If objects were copied between the memory pools affected by this
     * collection, their aggregate size in bytes. If this is a young
     * collection in a two-generation system, this value is the total
     * size of objects promoted to the old generation. This value is
     * not the same as the difference in before and after {@link MemoryUsage}
     * occupancy because allocation in the target {@link MemoryPoolMXBean MemoryPool}(s)
     * may have been done by other than the GC threads that ran this
     * collection.
     *
     * @return the aggregate size in bytes of objects copied between
     *         the memory pools affected by this collection.
     */
    public long getCopiedBetweenPools() {
        return copiedBetweenPools;
    }

    /**
     * The approximate start time of this collection in nanoseconds
     * since Java virtual machine launch.
     *
     * @return the start time of this collection.
     */
    public long getStartTimeNanos() {
        return startTime;
    }

    /**
     * The approximate start time of this collection (in seconds
     * as a double) since Java virtual machine launch.
     * <p>
     * This method is a convenience method that returns an
     * approximation of the value returned by
     * {@link #getStartTimeNanos getStartTimeNanos}.
     *
     * @return the start time of this collection.
     */
    private double getStartTimeSeconds() {
        return (double)startTime / 1.0e9;
    }

    /**
     * The approximate end time of this collection in nanoseconds
     * since Java virtual machine launch.
     *
     * @return the end time of this collection.
     */
    public long getEndTimeNanos() {
        return endTime;
    }

    /**
     * The approximate end time of this collection (in seconds
     * as a double) since Java virtual machine launch.
     * <p>
     * This method is a convenience method that returns an
     * approximation of the value returned by
     * {@link #getEndTimeNanos getEndTimeNanos}.
     *
     * @return the end time of this collection.
     */
    private double getEndTimeSeconds() {
        return (double)endTime / 1.0e9;
    }

    /**
     * The approximate elapsed wall clock time in nanoseconds between
     * the start and end of this collection. The returned value is
     * equal to {@link #getEndTimeNanos getEndTimeNanos}() -
     * {@link #getStartTimeNanos getStartTimeNanos}().
     *
     * @return the elapsed time of this collection. 
     */
    public long getDurationNanos() {
        return endTime - startTime;
    }

    /**
     * The approximate elapsed wall clock time (in seconds as a double)
     * between the start and end of this collection. The returned value is
     * equal to {@link #getEndTimeSeconds getEndTimeSeconds}() -
     * {@link #getStartTimeSeconds getStartTimeSeconds}(), which is not
     * the same as (double){@link #getDurationNanos getDurationNanos}().
     *
     * @return the elapsed time of this collection.
     */
    private double getDurationSeconds() {
        return getEndTimeSeconds() - getStartTimeSeconds();
    }

    /** 
     * The approximate amount of time in nanoseconds from the end of
     * the last previous collection run by the associated collector
     * to the start of this collection. The time interval represents
     * the amount of time the collector did not run.
     *
     * @return the amount of time from the end of the last previous
     *         collection run by the associated collector to the start
     *         of this collection.
     */
    public long getTimeFromEndOfPreviousToStartNanos() {
        return startTime - previousEndTime;
    }

    /** 
     * The approximate amount of time (in seconds as a double) from
     * the end of the last previous collection run by the associated
     * collector to the start of this collection. The time interval
     * represents the amount of time the collector did not run.
     * <p>
     * This method is a convenience method that returns an
     * approximation of the value returned by
     * {@link #getTimeFromEndOfPreviousToStartNanos getTimeFromEndOfPreviousToStartNanos}.
     *
     * @return the amount of time from the end of the last previous
     *         collection run by the associated collector to the start
     *         of this collection.
     */
    private double getTimeFromEndOfPreviousToStartSeconds() {
        return (double)getTimeFromEndOfPreviousToStartNanos() / 1.0e9;
    }

    /**
     * The percentage of time that the associated collector ran over the
     * time interval between the end of the last previous collection run
     * by it and the end of this collection.
     *
     * The percentage is calculated by:
     * ( {@link #getDurationSeconds getDurationSeconds}() /
     * ( (double){@link #getTimeFromEndOfPreviousToStartNanos getTimeFromEndOfPreviousToStartNanos}()
     * + {@link #getDurationSeconds getDurationSeconds}() ) ) * 100.
     *
     * @return the percentage of time the associated collector ran
     *         between the end of its last previous collection and
     *         the end of this collection.
     */
    public double getPercentageOfTimeCollectorWasRunning() {
        return (getDurationSeconds() / (getTimeFromEndOfPreviousToStartSeconds() + getDurationSeconds())) * 100;
    }

    /**
     * The approximate object allocation rate in megabytes per second in
     * the memory pools affected by the associated collector during the
     * time interval returned by {@link #getDurationNanos getDurationNanos}.
     * <p>
     * Note that "the memory pools affected by the associated collector" may
     * not be the same as "the memory pools affected by this collection".
     *
     * @return the object allocation rate in megabytes per second during the
     *         time interval between the start and end of this collection.
     */
    public double getAllocRateDuringCollection() {
        return (allocatedDuringCollection / (1024 * 1024)) / getDurationSeconds();
    }

    /**
     * The approximate object allocation rate in megabytes per second in
     * the memory pools affected by the associated collector during the
     * time interval returned by
     * {@link #getTimeFromEndOfPreviousToStartNanos getTimeFromEndOfPreviousToStartNanos}.
     * <p>
     * Note that "the memory pools affected by the associated collector" may
     * not be the same as "the memory pools affected by this collection".
     *
     * @return the object allocation rate in megabytes per second during the
     *         time interval between the end of the last previous collection
     *         and the start of this collection.
     */
    public double getAllocRateBetweenEndOfPreviousAndStart() {
        return (allocatedSincePrevious / (1024 * 1024)) / getTimeFromEndOfPreviousToStartSeconds();
    }

    /**
     * The approximate end time of the last previous collection in nanoseconds
     * since Java virtual machine launch.
     *
     * @return the end time of the last previous collection.
     */
    public long getPreviousEndTimeNanos() {
        return previousEndTime;
    }

    /**
     * The number of bytes allocated in the memory pools affected by the 
     * associated collector during the time interval returned by {@link #getDurationNanos getDurationNanos}.
     * <p>
     * Note that "the memory pools affected by the associated collector" may
     * not be the same as "the memory pools affected by this collection".
     *
     * @return the object allocated bytes during the time interval between 
     *         the start and end of this collection.
     */
    public long getAllocatedDuringCollection() {
        return allocatedDuringCollection;
    }
    
    /**
     * The number of bytes allocated in the memory pools affected by the 
     * associated collector during the time interval returned by 
     * {@link #getTimeFromEndOfPreviousToStartNanos getTimeFromEndOfPreviousToStartNanos}.
     * <p>
     * Note that "the memory pools affected by the associated collector" may
     * not be the same as "the memory pools affected by this collection".
     *
     * @return the object allocated bytes during the time interval between 
     *         the end of the last previous collection and the start of this 
     *         collection.
     */
    public long getAllocatedBetweenEndOfPreviousAndStart() {
        return allocatedSincePrevious;
    }

    /**
     * A {@link List} of {@link PauseInfo} for the pauses required during
     * this collection.
     *
     * @return a List of PauseInfo for the pauses during this collection.
     */
    public List<PauseInfo> getPauseInfo() {
        return java.util.Collections.unmodifiableList(Arrays.asList(pauseInfoArray));
    }

    /**
     * A {@link List} of {@link ConcurrentInfo} for the concurrent
     * phases of this collection.
     *
     * @return a List of {@link ConcurrentInfo} for the concurrent
     *         phases of this collection.
     */
    public List<ConcurrentInfo> getConcurrentInfo() {
        return java.util.Collections.unmodifiableList(Arrays.asList(concurrentInfoArray));
    }

   /**
     * Returns a {@code GcInfo} object represented by the
     * given {@code CompositeData}. The given
     * {@code CompositeData} must contain
     * all the following attributes:
     *
     * <blockquote>
     * <table class="striped"><caption style="display:none">description</caption>
     * <thead>
     * <tr>
     *   <th scope="col" style="text-align:left">Attribute Name</th>
     *   <th scope="col" style="text-align:left">Type</th>
     * </tr>
     * </thead>
     * <tbody>
     * <tr>
     *   <th scope="row">index</th>
     *   <td>{@code java.lang.Long}</td>
     * </tr>
     * <tr>
     *   <th scope="row">startTime</th>
     *   <td>{@code java.lang.Long}</td>
     * </tr>
     * <tr>
     *   <th scope="row">endTime</th>
     *   <td>{@code java.lang.Long}</td>
     * </tr>
     * <tr>
     *   <th scope="row">memoryUsageBeforeGc</th>
     *   <td>{@code javax.management.openmbean.TabularData}</td>
     * </tr>
     * <tr>
     *   <th scope="row">memoryUsageAfterGc</th>
     *   <td>{@code javax.management.openmbean.TabularData}</td>
     * </tr>
     * </tbody>
     * </table>
     * </blockquote>
     *
     * @throws IllegalArgumentException if {@code cd} does not
     *   represent a {@code GcInfo} object with the attributes
     *   described above.
     *
     * @return a {@code GcInfo} object represented by {@code cd}
     * if {@code cd} is not {@code null}; {@code null} otherwise.
     */
    public static GcInfo from(CompositeData cd) {
        if (cd == null) {
            return null;
        }

        if (cd instanceof GcInfoCompositeData) {
            return ((GcInfoCompositeData) cd).getGcInfo();
        } else {
            return new GcInfo(cd);
        }

    }

    // Implementation of the CompositeData interface
    public boolean containsKey(String key) {
        return cdata.containsKey(key);
    }

    public boolean containsValue(Object value) {
        return cdata.containsValue(value);
    }

    public boolean equals(Object obj) {
        return cdata.equals(obj);
    }

    public Object get(String key) {
        return cdata.get(key);
    }

    public Object[] getAll(String[] keys) {
        return cdata.getAll(keys);
    }

    public CompositeType getCompositeType() {
        return cdata.getCompositeType();
    }

    public int hashCode() {
        return cdata.hashCode();
    }

    public String toString() {
        return cdata.toString();
    }

    public Collection<?> values() {
        return cdata.values();
    }

    /**
     * Return the {@code CompositeData} representation of this
     * {@code GcInfo}, including any GC-specific attributes.  The
     * returned value will have at least all the attributes described
     * in the {@link #from(CompositeData) from} method, plus optionally
     * other attributes.
     *
     * @param ct the {@code CompositeType} that the caller expects.
     * This parameter is ignored and can be null.
     *
     * @return the {@code CompositeData} representation.
     */
    public CompositeData toCompositeData(CompositeType ct) {
        return cdata;
    }
}
