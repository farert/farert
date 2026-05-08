package org.sutezo.farert

import android.content.Context
import android.database.sqlite.SQLiteDatabase
import android.database.sqlite.SQLiteOpenHelper
import android.database.sqlite.SQLiteException
import java.io.*
import java.sql.SQLException
import java.util.zip.ZipInputStream


class DatabaseOpenHelper(context: Context) : SQLiteOpenHelper(context, DB_NAME, null, DATABASE_VERSION) {

    private val mContext : Context = context
    private val mDatabasePath : File = mContext.getDatabasePath(DB_NAME)

    companion object {
        const val DB_NAME = "jrdb.db"
        //val DB_NAME_ASSET = "routeDB/jrdb2017.db"
        const val DATABASE_VERSION = 38    // 38=2026.5.9  // !!! DB更新したらDATABASE_VERSION を更新
        const val DEFAULT_DB_IDX = 5  // "2025"
        private const val MIN_DB_IDX = 0
        private const val MAX_DB_IDX = 5

        var mDatabaseIndex : Int = DEFAULT_DB_IDX

        fun dbIdx2Name(dbidx : Int) : String {
            return when (dbidx) {
                0, 1 -> { "2014" } // 消費税5%, 2014
                2 -> { "2015" }
                3 -> { "2017" }
                4 -> { "2022" }     // 2019版(消費税8%)
                else -> { "Newest" }  // 2022版(消費税10%)
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
        if (checkDatabaseExists(dbidx)) return

        try {
            // 親ディレクトリを確実に作成する
            mDatabasePath.parentFile?.let {
                if (!it.exists()) it.mkdirs()
            }

            // 直接アセットからコピー（空のDB作成を待たなくて良い）
            this.copyDataBaseFromAsset(dbIdx2Name(dbidx))

            // コピーしたDBを開いてバージョンを設定
            val dbPath = mDatabasePath.absolutePath
            // OPEN_READWRITE に一時的に戻す（versionを書き換えるため）
            SQLiteDatabase.openDatabase(dbPath, null, SQLiteDatabase.OPEN_READWRITE).use { checkDb ->
                checkDb.version = DATABASE_VERSION
                mDatabaseIndex = dbidx
            }
        } catch (e: Exception) {
            // デバッグ用に e.message を含めると原因がより明確になります
            throw Error("Error copying database: ${e.message}", e)
        }
    }
    /**
     * 再コピーを防止するために、すでにデータベースがあるかどうか判定する
     *
     * @return 存在している場合 {@code true}
     */
    private fun checkDatabaseExists(dbid : Int) : Boolean {
        val dbPath = mDatabasePath.absolutePath
        if (!File(dbPath).exists()) return false

        try {
            SQLiteDatabase.openDatabase(dbPath, null, SQLiteDatabase.OPEN_READONLY).use { checkDb ->
                if (checkDb.version == DATABASE_VERSION && dbid == mDatabaseIndex) {
                    return true
                }
            }
        } catch (e: SQLiteException) {
            // 破損やバージョン不一致
        }

        File(dbPath).delete()
        return false
    }
    /**
     * asset に格納したデーだベースをデフォルトのデータベースパスに作成したからのデータベースにコピーする
     */
    private fun copyDataBaseFromAsset(dbvername: String) {
        var bOk = false
        // use を使うことで確実に close される
        mContext.assets.open("routeDB/jrdb.dat").use { ais ->
            ZipInputStream(ais).use { zist ->
                var ent = zist.nextEntry
                while (ent != null) {
                    if (!ent.isDirectory && ent.name == "jrdb${dbvername}.db") {
                        FileOutputStream(mDatabasePath).use { ost ->
                            zist.copyTo(ost, bufferSize = 16384)
                            ost.flush()
                        }
                        zist.closeEntry()
                        bOk = true
                        break
                    }
                    zist.closeEntry()
                    ent = zist.nextEntry
                }
            }
        }
        if (!bOk) throw Exception("Target DB file not found in zip")
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
        return readableDatabase

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

