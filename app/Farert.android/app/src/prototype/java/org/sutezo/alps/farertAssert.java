package org.sutezo.alps;

public class farertAssert {
    static void ASSERT(boolean condition) {
        if (!condition) {
            Thread.currentThread().getUncaughtExceptionHandler().uncaughtException(
                    Thread.currentThread(), new AssertionError("RouteUtil.ASSERT !!!!!!"));
        }
    }
    static void ASSERT(boolean condition, String message) {
        if (!condition) {
            Thread.currentThread().getUncaughtExceptionHandler().uncaughtException(
                    Thread.currentThread(), new AssertionError("RouteUtil.ASSERT !!!!!!" + message));
        }
    }
}
