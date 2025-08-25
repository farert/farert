package org.sutezo.alps;
import org.sutezo.farert.BuildConfig;

public class farertAssert {
    public static void ASSERT(boolean condition) {
        if (!condition) {
            if (BuildConfig.DEBUG)
                throw new AssertionError("Assertion failed");
        }
    }
    
    public static void ASSERT(boolean condition, String message) {
        if (!condition) {
            if (BuildConfig.DEBUG)
                throw new AssertionError("Assertion failed: " + message);
        }
    }
}