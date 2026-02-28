package org.sutezo.farert;

public class DatabaseOpenHelper {
    public static final int DEFAULT_DB_IDX = 0;

    public static class Companion {
        public static int dbIndex() {
            return DEFAULT_DB_IDX;
        }
    }
}
