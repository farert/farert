package org.sutezo.farert

import android.app.Application
import android.content.Context
import android.content.SharedPreferences
import android.database.sqlite.SQLiteDatabase
import org.json.JSONArray
import org.sutezo.alps.*
import java.io.OutputStream
import java.io.PrintStream

class FarertApp : Application() {
    val ds = Route()

    companion object {
        lateinit var instance: Application private set
    }

    lateinit var mDbHelper : DatabaseOpenHelper

    override fun onCreate() {
        super.onCreate()
        instance = this
        //System.setOut(PrintStream("C:\\debug.log") )
        System.setOut(NullPrintStream())
        setDatabase()

        TestRoute.exec(this)    // test function
    }

    private fun setDatabase() {
        val dbno = readParam(this, "datasource").toString()
        var dbidx : Int
        try {
            dbidx = Integer.parseInt(dbno)
        } catch (e: Throwable) {
            dbidx = -1
        }
        dbidx = DatabaseOpenHelper.validDBidx(dbidx)
        DatabaseOpenHelper.mDatabaseIndex = dbidx // これがないと毎回DB展開する
        mDbHelper = DatabaseOpenHelper(this)
        mDbHelper.createEmptyDataBase(dbidx)
        RouteDB.createFactory(mDbHelper.openDataBase(), if (dbidx == 0) 5 else if (dbidx <= 4) 8 else 10)
    }

    fun changeDatabase(dbidx: Int) {
        val idx = DatabaseOpenHelper.validDBidx(dbidx) // retrieve the available index of database.
        //不要！これがあるとDB切り替わりません. DatabaseOpenHelper.mDatabaseIndex = idx
        mDbHelper.closeDatabase()
        mDbHelper.createEmptyDataBase(idx)
        RouteDB.createFactory(mDbHelper.openDataBase(), if (idx == 0) 5 else if (idx <= 4) 8 else 10)
    }
}