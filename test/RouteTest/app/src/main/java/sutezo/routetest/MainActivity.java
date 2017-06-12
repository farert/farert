package sutezo.routetest;

import android.database.sqlite.SQLiteDatabase;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import java.io.IOException;     //@
import java.sql.SQLException;   //@

import sutezo.routetest.alps.RouteTest;


public class MainActivity extends AppCompatActivity {

    private DatabaseOpenHelper mDbHelper;   //@
    private SQLiteDatabase db;              //@

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setDatabase();                      //@

        RouteTest.test_execute(this, db);   //@

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }
    private void setDatabase() {
        mDbHelper = new DatabaseOpenHelper(this);
        try {
            mDbHelper.createEmptyDataBase();
            db = mDbHelper.openDataBase();
        } catch (IOException ioe) {
            throw new Error("Unable to create database");
        } catch(SQLException sqle) {
            //throw sqle;
        }
    }
}
