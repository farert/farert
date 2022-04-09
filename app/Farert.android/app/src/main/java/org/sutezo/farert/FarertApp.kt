package org.sutezo.farert

import android.app.Application
import android.content.pm.PackageManager
import android.os.Build
import android.support.annotation.RequiresApi
import org.sutezo.alps.*


class FarertApp : Application() {
    val ds = Route()
    var bKokuraHakataShinZaiFlag : Boolean = false

    companion object {
        lateinit var instance: Application private set
    }

    private lateinit var mDbHelper : DatabaseOpenHelper

    override fun onCreate() {
        super.onCreate()
        instance = this
        //System.setOut(PrintStream("C:\\debug.log") )
        System.setOut(NullPrintStream())

        /* database index reset */
        try {
            val num = readParam(this, "hasLaunched").toInt()
            // 消費税10%になった最初のバージョン(17)より前から使ってた人は
            // バージョンアップで、消費税10%版DBに強制的に設定
            if (num < 17) {  // 16=19.10 17=19.10.01
                // set current default database index
                saveParam(this, "datasource", DatabaseOpenHelper.validDBidx(-1).toString())
            }
        } catch (e : NumberFormatException) {
            saveParam(this, "datasource", DatabaseOpenHelper.validDBidx(-1).toString())
        } finally {
            setDatabase()
        }

        TestRoute.exec(this)    // test function

        bKokuraHakataShinZaiFlag = (readParam(this, "kokura_hakata_shinzai") == "true")
    }

    private fun setDatabase() {
        val dbno = readParam(this, "datasource")
        var dbidx : Int = try {
            Integer.parseInt(dbno)
        } catch (e: Throwable) {
            -1
        }
        // 有効なDBIndexに変換
        dbidx = DatabaseOpenHelper.validDBidx(dbidx)
        DatabaseOpenHelper.mDatabaseIndex = dbidx // これがないと毎回DB展開する
        mDbHelper = DatabaseOpenHelper(this)
        mDbHelper.createEmptyDataBase(dbidx)
        // 消費税は、DbId=DbIndexにより決まる
        RouteDB.createFactory(mDbHelper.openDataBase(), if (dbidx == 0) 5 else if (dbidx <= 4) 8 else 10)
    }

    fun changeDatabase(dbidx: Int) {
        val idx = DatabaseOpenHelper.validDBidx(dbidx) // retrieve the available index of database.
        //不要！これがあるとDB切り替わりません. DatabaseOpenHelper.mDatabaseIndex = idx
        mDbHelper.closeDatabase()
        mDbHelper.createEmptyDataBase(idx)
        RouteDB.createFactory(mDbHelper.openDataBase(), if (idx == 0) 5 else if (idx <= 4) 8 else 10)
    }

    @RequiresApi(Build.VERSION_CODES.P)
    fun getVersionCode(): Long {
        val pm = this.packageManager
        var versionCode : Long = 0
        try {
            val packageInfo = pm.getPackageInfo(this.packageName, 0)
            // versionCode:通算バージョン(数値)
            // versionName: "18.11" とか
            //if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
                versionCode = packageInfo.longVersionCode
            //} else {
            //    versionCode = packageInfo.versionCode.toLong()
            //}
        } catch (e: PackageManager.NameNotFoundException) {
            e.printStackTrace()
        }
        return versionCode
    }
}