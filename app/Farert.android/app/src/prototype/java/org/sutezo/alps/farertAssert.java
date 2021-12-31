package org.sutezo.alps;

import java.util.Objects;

public class farertAssert {
    static void ASSERT(boolean condition) {
        if (!condition) {
            Objects.requireNonNull(Thread.currentThread().getUncaughtExceptionHandler()).uncaughtException(
                    Thread.currentThread(), new AssertionError("RouteUtil.ASSERT !!!!!!"));
        }
    }
    static void ASSERT(boolean condition, String message) {
        if (!condition) {
            Objects.requireNonNull(Thread.currentThread().getUncaughtExceptionHandler()).uncaughtException(
                    Thread.currentThread(), new AssertionError("RouteUtil.ASSERT !!!!!!" + message));
        }
    }
}
