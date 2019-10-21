package org.sutezo.farert

import android.content.Context
import android.database.sqlite.SQLiteDatabase
import android.database.sqlite.SQLiteOpenHelper
import android.database.sqlite.SQLiteException
import java.io.*
import java.sql.SQLException
import java.util.zip.ZipInputStream


class DatabaseOpenHelper(context: Context) : SQLiteOpenHelper(context, DB_NAME, null, DATABASE_VERSION) {

    val mContext : Context
    val mDatabasePath : File

    init  {
        mContext = context
        mDatabasePath = mContext.getDatabasePath(DB_NAME)
    }

    companion object {
        val DB_NAME = "jrdb.db"
        //val DB_NAME_ASSET = "routeDB/jrdb2017.db"
        val DATABASE_VERSION = 8    // 7=2019.7.23(BRT,路線かな他)
        val DEFAULT_DB_IDX = 5  // "2019"     // !!! DB更新してされたらDATABASE_VERSION, DEFAULT_DB_VERを更新
        val MIN_DB_IDX = 0
        val MAX_DB_IDX = 5

        var mDatabaseIndex : Int = DEFAULT_DB_IDX

        fun dbIdx2Name(dbidx : Int) : String {
            return when (dbidx) {
                0, 1 -> { "2014" } // 消費税5%, 2014
                2 -> { "2015" }
                3 -> { "2017" }
                4 -> { "2019" }     // 2019版(消費税8%)
                else -> { "2019" }  // 2019版(消費税10%)
            }
        }
        fun dbIdx2NameWithTax(dbidx : Int) : String {
            return when (dbidx) {
                0 -> { "2014(5%tax)" }
                1 -> { "2014" }
                2 -> { "2015" }
                3 -> { "2017" }
                4 -> { "2019版(消費税8%)" }
                else -> { "2019版(消費税10%)" }
            }
        }
        fun dbIndex() : Int = mDatabaseIndex

        fun validDBidx(dbidx: Int) : Int {
            return if (dbidx < MIN_DB_IDX || MAX_DB_IDX < dbidx) {
                DEFAULT_DB_IDX
            } else {
                dbidx
            }
        }
    }

    /**
     * asset に格納したデータベースをコピーするための空のデータベースを作成する
     */
     fun createEmptyDataBase(dbidx : Int)  {

        if (checkDatabaseExists(dbidx)) {
            // すでにデータベースは作成されている

        } else {
            // このメソッドを呼ぶことで、空のデータベースがアプリのデフォルトシステムパスに作られる
            getReadableDatabase()
            close()

             try {
                // asset に格納したデータベースをコピーする
                this.copyDataBaseFromAsset(dbIdx2Name(dbidx))

                val dbPath = mDatabasePath.absolutePath
                val checkDb = SQLiteDatabase.openDatabase(dbPath, null, SQLiteDatabase.OPEN_READWRITE)

                checkDb.version = DATABASE_VERSION
                DatabaseOpenHelper.mDatabaseIndex = dbidx
                checkDb.close()

            } catch (e: Exception) {
                throw Error("Error copying database")
            }
        }
    }

    /**
     * 再コピーを防止するために、すでにデータベースがあるかどうか判定する
     *
     * @return 存在している場合 {@code true}
     */
     private fun checkDatabaseExists(dbid : Int) : Boolean {

        val dbPath = mDatabasePath.absolutePath

        var checkDb :SQLiteDatabase? = null
        try {
            checkDb = SQLiteDatabase.openDatabase(dbPath, null, SQLiteDatabase.OPEN_READONLY)
        } catch (e: SQLiteException) {
            // データベースはまだ存在していない
        }

        if (checkDb == null) {
            // データベースはまだ存在していない
            return false
        }

        if ((checkDb.version == DATABASE_VERSION) && (dbid == mDatabaseIndex)) {
            // データベースは存在していて最新
            checkDb.close()
            return true
        }

        // データベースが存在していて最新ではないので削除
        val f = File(dbPath)
        f.delete()
        return false
    }

    /**
     * asset に格納したデーだベースをデフォルトのデータベースパスに作成したからのデータベースにコピーする
     */
    private fun copyDataBaseFromAsset(dbvername: String) {
        if (true) {
            var bOk = false
            val zist = ZipInputStream(mContext.assets.open("routeDB/jrdb.dat"))
            var ent = zist.nextEntry
            while (ent != null) {
                if (!ent.isDirectory && ent.name == "jrdb${dbvername}.db") {
                    val ost = FileOutputStream(mDatabasePath)
                    zist.copyTo(ost)
                    zist.closeEntry()
                    bOk = true
                    break
                }
                zist.closeEntry()
                ent = zist.nextEntry
            }
            if (!bOk) {
                throw Exception()
            }

        } else {
            // Open your local db as the input stream
            val ist = mContext.assets.open("routeDB/${dbvername}.db")
            // transfer bytes from the inputfile to the
            // outputfile
            val ost = FileOutputStream(mDatabasePath)
            ist.copyTo(ost)
            ost.close()
            ost.flush()
            ist.close()
        }
    }

    @Throws(SQLException::class)
    fun openDataBase() : SQLiteDatabase {

/*
val dbPath = mDatabasePath.absolutePath

var checkDb :SQLiteDatabase? = null
try {
checkDb = SQLiteDatabase.openDatabase(dbPath, null, SQLiteDatabase.OPEN_READONLY)
} catch (e: SQLiteException) {
// データベースはまだ存在していない
}

checkDb?.let {
return it
}

/*** original code is unnecessary ***/

*/
        return getReadableDatabase();

    }

    override fun onCreate(db: SQLiteDatabase) {
    }

    override fun onUpgrade(db: SQLiteDatabase, oldVersion:Int, newVersion: Int) {
    }

    @Synchronized @Throws(SQLException::class)
    fun closeDatabase() {
        super.close()
    }
}

