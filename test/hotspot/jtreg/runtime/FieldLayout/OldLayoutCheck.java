/*
 * Copyright (c) 2020, Oracle and/or its affiliates. All rights reserved.
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
 */

/*
 * @test
 * @bug 8239014
 * @summary -XX:-UseEmptySlotsInSupers sometime fails to reproduce the layout of the old code
 * @library /test/lib /
 * @modules java.base/jdk.internal.misc
 *          java.management
 * @requires vm.bits == "64" & vm.opt.final.UseCompressedOops == true & vm.gc != "Z"
 * @build jdk.test.whitebox.WhiteBox
 * @run driver jdk.test.lib.helpers.ClassFileInstaller jdk.test.whitebox.WhiteBox
 * @run main/othervm -Xbootclasspath/a:. -XX:+UnlockDiagnosticVMOptions -XX:+WhiteBoxAPI -XX:+UseCompressedClassPointers -XX:-UseEmptySlotsInSupers OldLayoutCheck
 */

/*
 * @test
 * @requires vm.bits == "32"
 * @library /test/lib /
 * @modules java.base/jdk.internal.misc
 *          java.management
 * @build jdk.test.whitebox.WhiteBox
 * @run driver jdk.test.lib.helpers.ClassFileInstaller jdk.test.whitebox.WhiteBox
 * @run main/othervm -Xbootclasspath/a:. -XX:+UnlockDiagnosticVMOptions -XX:+WhiteBoxAPI -XX:-UseEmptySlotsInSupers OldLayoutCheck
 */

import java.lang.reflect.Field;
import java.util.Arrays;
import java.util.Comparator;
import jdk.internal.misc.Unsafe;

import jdk.test.lib.Asserts;
import jdk.test.lib.Platform;
import jdk.test.whitebox.WhiteBox;

public class OldLayoutCheck {

    static class LIClass {
        public long l;
        public int i;
    }

    private static final long INT_OFFSET;
    private static final long LONG_OFFSET;
    static {
      WhiteBox WB = WhiteBox.getWhiteBox();
      if (!Platform.is64bit() || WB.getBooleanVMFlag("UseCompactObjectHeaders")) {
        INT_OFFSET = 16L;
        LONG_OFFSET = 8L;
      } else {
        INT_OFFSET = 12L;
        LONG_OFFSET = 16L;
      }
    }
    static public void main(String[] args) {
        Unsafe unsafe = Unsafe.getUnsafe();
        Class c = LIClass.class;
        Field[] fields = c.getFields();
        for (int i = 0; i < fields.length; i++) {
            long offset = unsafe.objectFieldOffset(fields[i]);
            if (fields[i].getType() == int.class) {
                Asserts.assertEquals(offset, INT_OFFSET, "Misplaced int field");
            } else if (fields[i].getType() == long.class) {
                Asserts.assertEquals(offset, LONG_OFFSET, "Misplaced long field");
            } else {
                Asserts.fail("Unexpected field type");
            }
        }
    }
}
