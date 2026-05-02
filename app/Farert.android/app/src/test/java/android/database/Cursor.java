package android.database;

public interface Cursor extends AutoCloseable {
    boolean moveToNext();
    boolean moveToFirst();
    int getInt(int columnIndex);
    short getShort(int columnIndex);
    long getLong(int columnIndex);
    String getString(int columnIndex);
    @Override
    void close();
}
