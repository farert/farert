package android.database;

import java.util.List;

public class Cursor implements AutoCloseable {
    private final List<String[]> rows;
    private int index = -1;
    private boolean closed = false;

    public Cursor(List<String[]> rows) {
        this.rows = rows;
    }

    public boolean moveToNext() {
        if (closed) {
            return false;
        }
        int next = index + 1;
        if (next < rows.size()) {
            index = next;
            return true;
        }
        return false;
    }

    public boolean moveToFirst() {
        if (closed) {
            return false;
        }
        if (rows.isEmpty()) {
            return false;
        }
        index = 0;
        return true;
    }

    public int getInt(int columnIndex) {
        String v = value(columnIndex);
        if (v == null || v.isEmpty()) {
            return 0;
        }
        try {
            return Integer.parseInt(v);
        } catch (NumberFormatException ex) {
            return (int) Long.parseLong(v);
        }
    }

    public short getShort(int columnIndex) {
        String v = value(columnIndex);
        if (v == null || v.isEmpty()) {
            return 0;
        }
        try {
            return Short.parseShort(v);
        } catch (NumberFormatException ex) {
            return (short) Integer.parseInt(v);
        }
    }

    public long getLong(int columnIndex) {
        String v = value(columnIndex);
        if (v == null || v.isEmpty()) {
            return 0L;
        }
        return Long.parseLong(v);
    }

    public String getString(int columnIndex) {
        return value(columnIndex);
    }

    private String value(int columnIndex) {
        if (index < 0 || index >= rows.size()) {
            return null;
        }
        String[] row = rows.get(index);
        if (columnIndex < 0 || columnIndex >= row.length) {
            return null;
        }
        String v = row[columnIndex];
        if ("\\N".equals(v)) {
            return null;
        }
        return v;
    }

    @Override
    public void close() {
        closed = true;
    }
}
