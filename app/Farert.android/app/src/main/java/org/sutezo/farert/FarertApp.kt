package org.sutezo.farert

import android.app.Application
import android.content.Context
import android.content.SharedPreferences
import android.database.sqlite.SQLiteDatabase
import org.json.JSONArray
import org.sutezo.alps.Route
import org.sutezo.alps.RouteDB
import org.sutezo.alps.readParam

class FarertApp : Application() {
    val ds = Route()

    companion object {
        //const val CONSTANT = 12
        private lateinit var db: SQLiteDatabase
        lateinit var mDbHelper : DatabaseOpenHelper
    }

    override fun onCreate() {
        super.onCreate()
        setDatabase()
    }

    private fun setDatabase() {
        val dbname = readParam(this, "datasource").toString()
        var dbidx : Int = 0
        try {
            dbidx = Integer.parseInt(dbname)
        } catch (e: NumberFormatException) {
            dbidx = -1
        }
        dbidx = DatabaseOpenHelper.validDBidx(dbidx)
        mDbHelper = DatabaseOpenHelper(this, dbidx)
        mDbHelper.createEmptyDataBase(dbidx)
        db = mDbHelper.openDataBase()
        RouteDB.createFactory(db, if (dbidx == 0) 5 else 8)
    }


    fun changeDatabase(dbidx: Int) {
        mDbHelper.closeDatabase()
        mDbHelper.createEmptyDataBase(dbidx)
        db = mDbHelper.openDataBase()
        RouteDB.createFactory(db, if (dbidx == 0) 5 else 8);
    }
}