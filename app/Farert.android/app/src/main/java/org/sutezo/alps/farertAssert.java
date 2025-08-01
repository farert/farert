package org.sutezo.alps;

public class farertAssert {
    public static void ASSERT(boolean condition) {
        if (!condition) {
            throw new AssertionError("Assertion failed");
        }
    }
    
    public static void ASSERT(boolean condition, String message) {
        if (!condition) {
            throw new AssertionError("Assertion failed: " + message);
        }
    }
}