package sutezo.routetest

import android.database.sqlite.SQLiteDatabase
import android.support.v7.app.AppCompatActivity
import android.os.Bundle

import java.io.IOException     //@
import java.sql.SQLException   //@

import sutezo.routetest.alps.RouteTest


class MainActivity : AppCompatActivity() {

    private var mDbHelper: DatabaseOpenHelper? = null   //@
    private var db: SQLiteDatabase? = null              //@

    override fun onCreate(savedInstanceState: Bundle?) {
        setDatabase()                      //@

        RouteTest.test_execute(this, db!!)   //@

        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
    }

    private fun setDatabase() {
        mDbHelper = DatabaseOpenHelper(this)
        try {
            mDbHelper!!.createEmptyDataBase()
            db = mDbHelper!!.openDataBase()
        } catch (ioe: IOException) {
            throw Error("Unable to create database")
        } catch (sqle: SQLException) {
            //throw sqle;
        }

    }
}
