package org.sutezo.farert

import android.app.Application
import android.content.Context
import android.content.SharedPreferences
import android.database.sqlite.SQLiteDatabase
import org.json.JSONArray
import org.sutezo.alps.NullPrintStream
import org.sutezo.alps.Route
import org.sutezo.alps.RouteDB
import org.sutezo.alps.readParam
import java.io.OutputStream
import java.io.PrintStream

class FarertApp : Application() {
    val ds = Route()

    companion object {
        lateinit var mDbHelper : DatabaseOpenHelper
    }

    override fun onCreate() {
        super.onCreate()
        //System.setOut(PrintStream("C:\\debug.log") )
        System.setOut(NullPrintStream())
        setDatabase()
    }

    private fun setDatabase() {
        val dbname = readParam(this, "datasource").toString()
        var dbidx : Int
        try {
            dbidx = Integer.parseInt(dbname)
        } catch (e: NumberFormatException) {
            dbidx = -1
        }
        dbidx = DatabaseOpenHelper.validDBidx(dbidx)
        mDbHelper = DatabaseOpenHelper(this, dbidx)
        mDbHelper.createEmptyDataBase(dbidx)
        RouteDB.createFactory(mDbHelper.openDataBase(), if (dbidx == 0) 5 else 8)
    }


    fun changeDatabase(dbidx: Int) {
        val idx = DatabaseOpenHelper.validDBidx(dbidx)
        mDbHelper.closeDatabase()
        mDbHelper.createEmptyDataBase(idx)
        RouteDB.createFactory(mDbHelper.openDataBase(), if (idx == 0) 5 else 8);
    }
}