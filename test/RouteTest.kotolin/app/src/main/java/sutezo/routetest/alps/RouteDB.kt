package sutezo.routetest.alps

//package Route;

import java.util.*
import android.database.Cursor
import android.database.sqlite.SQLiteDatabase
import android.database.sqlite.SQLiteOpenHelper


/*!	@file Route core logic implement.
 *	Copyright(c) sutezo9@me.com 2012.
 */

/*
 * 'Farert'
 * Copyright (C) 2014 Sutezo (sutezo666@gmail.com)
 *
 *    'Farert' is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     'Farert' is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with 'Farert'.  If not, see <http://www.gnu.org/licenses/>.
 *
 * このプログラムはフリーソフトウェアです。あなたはこれを、フリーソフトウェ
 * ア財団によって発行された GNU 一般公衆利用許諾契約書(バージョン3か、希
 * 望によってはそれ以降のバージョンのうちどれか)の定める条件の下で再頒布
 * または改変することができます。
 *
 * このプログラムは有用であることを願って頒布されますが、*全くの無保証*
 * です。商業可能性の保証や特定の目的への適合性は、言外に示されたものも含
 * め全く存在しません。詳しくはGNU 一般公衆利用許諾契約書をご覧ください。
 *
 * あなたはこのプログラムと共に、GNU 一般公衆利用許諾契約書の複製物を一部
 * 受け取ったはずです。もし受け取っていなければ、フリーソフトウェア財団ま
 * で請求してください
 */

internal class RouteDB {

    private var name: String? = null          // [128] UTF-8 max 42character
    private var tax: Int = 0
    private var createdate: String? = null    // [64] UTF-8 max 42character '2015/03/14 12:43:43'

    private constructor(db_: SQLiteDatabase) {
        name = ""
        tax = 5
        createdate = "N/A"
        _db = db_
        val ctx = db_.rawQuery(
                "select name, tax, db_createdate from t_dbsystem limit(1)", null)
        try {
            if (ctx.moveToNext()) {
                name = ctx.getString(0)
                tax = ctx.getInt(1)
                createdate = ctx.getString(2)
            }
        } finally {
            ctx.close()
        }
    }

    private constructor() {
        RouteUtil.ASSERT(false, "bug")
    }

    fun name(): String {
        return name
    }

    fun tax(): Int {
        return tax
    }

    fun dbDate(): String {
        return createdate
    }

    companion object {
        val debug = true    //////////////////////////////////////

        private var _db: SQLiteDatabase? = null
        private var obj: RouteDB? = null

        val instance: RouteDB
            get() {
                if (obj == null) {
                    RouteUtil.ASSERT(false, "bug")
                }
                return obj
            }

        // start constructor (Initialize create)
        fun createFactory(db_: SQLiteDatabase): RouteDB {
            if (obj == null) {
                obj = RouteDB(db_)
            }
            return obj
        }

        fun db(): SQLiteDatabase {
            if (_db == null) {
                RouteUtil.ASSERT(false, "Must be call createFactory(SQLiteDatabase)")
            }
            return obj!!._db
        }
    }
}
