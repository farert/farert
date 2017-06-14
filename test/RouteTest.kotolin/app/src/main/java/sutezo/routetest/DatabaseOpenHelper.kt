package sutezo.routetest

import android.content.Context
import android.database.sqlite.SQLiteDatabase
import android.database.sqlite.SQLiteException
import android.database.sqlite.SQLiteOpenHelper

import java.io.File
import java.io.FileOutputStream
import java.io.IOException
import java.io.InputStream
import java.io.OutputStream
import java.sql.SQLException

/**
 * Created by n.takeda on 2016/03/11.
 */

/*
private DatabaseOpenHelper mDbHelper;
private SQLiteDatabase db;
:
:
private void setDatabase() {
    mDbHelper = new DatabaseOpenHelper(this);
    try {
        mDbHelper.createEmptyDataBase();
        db = mDbHelper.openDataBase();
    } catch (IOException ioe) {
        throw new Error("Unable to create database");
    } catch(SQLException sqle){
        throw sqle;
    }
}
 */

class DatabaseOpenHelper(private val mContext: Context) : SQLiteOpenHelper(mContext, DatabaseOpenHelper.DB_NAME, null, DatabaseOpenHelper.DATABASE_VERSION) {

    private val mDatabase: SQLiteDatabase? = null
    private val mDatabasePath: File

    init {
        mDatabasePath = mContext.getDatabasePath(DB_NAME)
    }

    /**
     * asset に格納したデータベースをコピーするための空のデータベースを作成する
     */
    @Throws(IOException::class)
    fun createEmptyDataBase() {
        val dbExist = checkDatabaseExists()

        if (dbExist) {
            // すでにデータベースは作成されている
        } else {
            // このメソッドを呼ぶことで、空のデータベースがアプリのデフォルトシステムパスに作られる
            readableDatabase

            try {
                // asset に格納したデータベースをコピーする
                copyDataBaseFromAsset()

                val dbPath = mDatabasePath.absolutePath
                var checkDb: SQLiteDatabase? = null
                try {
                    checkDb = SQLiteDatabase.openDatabase(dbPath, null, SQLiteDatabase.OPEN_READWRITE)
                } catch (e: SQLiteException) {
                }

                if (checkDb != null) {
                    checkDb.version = DATABASE_VERSION
                    checkDb.close()
                }

            } catch (e: IOException) {
                throw Error("Error copying database")
            }

        }
    }

    /**
     * 再コピーを防止するために、すでにデータベースがあるかどうか判定する

     * @return 存在している場合 `true`
     */
    private fun checkDatabaseExists(): Boolean {
        val dbPath = mDatabasePath.absolutePath

        var checkDb: SQLiteDatabase? = null
        try {
            checkDb = SQLiteDatabase.openDatabase(dbPath, null, SQLiteDatabase.OPEN_READONLY)
        } catch (e: SQLiteException) {
            // データベースはまだ存在していない
        }

        if (checkDb == null) {
            // データベースはまだ存在していない
            return false
        }

        val oldVersion = checkDb.version
        val newVersion = DATABASE_VERSION

        if (oldVersion == newVersion) {
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
    @Throws(IOException::class)
    private fun copyDataBaseFromAsset() {

        // asset 内のデータベースファイルにアクセス
        val mInput = mContext.assets.open(DB_NAME_ASSET)

        // デフォルトのデータベースパスに作成した空のDB
        val mOutput = FileOutputStream(mDatabasePath)

        // コピー
        val buffer = ByteArray(1024)
        var size: Int
        while ((size = mInput.read(buffer)) > 0) {
            mOutput.write(buffer, 0, size)
        }

        // Close the streams
        mOutput.flush()
        mOutput.close()
        mInput.close()
    }

    @Throws(SQLException::class)
    fun openDataBase(): SQLiteDatabase {
        return readableDatabase
    }

    override fun onCreate(db: SQLiteDatabase) {}

    override fun onUpgrade(db: SQLiteDatabase, oldVersion: Int, newVersion: Int) {}

    @Synchronized override fun close() {
        mDatabase?.close()

        super.close()
    }

    companion object {

        private val DB_NAME = "jrdb"
        private val DB_NAME_ASSET = "routeDB/jrdb2017.db"
        private val DATABASE_VERSION = 1
    }
}
