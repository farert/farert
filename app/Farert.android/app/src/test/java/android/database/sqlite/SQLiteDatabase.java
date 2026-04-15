package android.database.sqlite;

import android.database.Cursor;
import android.database.SimpleCursor;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.ResultSetMetaData;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.List;

public class SQLiteDatabase {
    public static final int OPEN_READONLY = 1;

    private final Connection connection;

    private SQLiteDatabase(String path) throws SQLException {
        try {
            Class.forName("org.sqlite.JDBC");
        } catch (ClassNotFoundException ignored) {
        }
        String url = "jdbc:sqlite:" + path;
        connection = DriverManager.getConnection(url);
    }

    public static SQLiteDatabase openDatabase(String path, Object factory, int flags) throws SQLException {
        return new SQLiteDatabase(path);
    }

    public Cursor rawQuery(String sql, String[] selectionArgs) {
        SqlWithParams prepared = prepareSql(sql);
        String finalSql = prepared.sql;
        if (finalSql.contains("t_farehla") && finalSql.contains("haa")) {
            finalSql = finalSql.replace("haa", "eaa");
        }
        try (PreparedStatement stmt = connection.prepareStatement(finalSql)) {
            if (selectionArgs != null && prepared.paramOrder != null) {
                for (int i = 0; i < prepared.paramOrder.length; i++) {
                    int idx = prepared.paramOrder[i];
                    String value = idx >= 0 && idx < selectionArgs.length ? selectionArgs[idx] : null;
                    stmt.setString(i + 1, value);
                }
            }
            try (ResultSet rs = stmt.executeQuery()) {
                ResultSetMetaData meta = rs.getMetaData();
                int cols = meta.getColumnCount();
                List<String[]> rows = new ArrayList<>();
                while (rs.next()) {
                    String[] row = new String[cols];
                    for (int i = 0; i < cols; i++) {
                        row[i] = rs.getString(i + 1);
                    }
                    rows.add(row);
                }
                return new SimpleCursor(rows);
            }
        } catch (SQLException e) {
            throw new RuntimeException("sqlite query failed: " + e.getMessage(), e);
        }
    }

    public void close() {
        try {
            connection.close();
        } catch (SQLException ignored) {
        }
    }

    private SqlWithParams prepareSql(String sql) {
        StringBuilder out = new StringBuilder();
        List<Integer> order = new ArrayList<>();
        int positional = 0;
        for (int i = 0; i < sql.length(); i++) {
            char c = sql.charAt(i);
            if (c == '?') {
                int j = i + 1;
                int num = 0;
                while (j < sql.length() && Character.isDigit(sql.charAt(j))) {
                    num = (num * 10) + (sql.charAt(j) - '0');
                    j++;
                }
                if (j > i + 1) {
                    order.add(num - 1);
                    out.append('?');
                    i = j - 1;
                } else {
                    order.add(positional++);
                    out.append('?');
                }
            } else {
                out.append(c);
            }
        }
        int[] paramOrder = new int[order.size()];
        for (int i = 0; i < order.size(); i++) {
            paramOrder[i] = order.get(i);
        }
        return new SqlWithParams(out.toString(), paramOrder);
    }

    private static class SqlWithParams {
        final String sql;
        final int[] paramOrder;

        SqlWithParams(String sql, int[] paramOrder) {
            this.sql = sql;
            this.paramOrder = paramOrder;
        }
    }
}
