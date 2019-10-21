package org.sutezo.alps;

//package Route;


import android.app.Application;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;

import org.sutezo.farert.DatabaseOpenHelper;
import org.sutezo.farert.FarertApp;

import static org.sutezo.alps.farertAssert.*;


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

public class RouteDB {
    final static boolean debug = true;	//////////////////////////////////////

    private String name;          // [128] UTF-8 max 42character
    private int tax;
    private String createdate;    // [64] UTF-8 max 42character '2015/03/14 12:43:43'

    private static SQLiteDatabase _db = null;
    private static RouteDB obj = null;

    private RouteDB(SQLiteDatabase db_, int tax_) {
        name = "";
        createdate = "N/A";
        _db = db_;
        Cursor ctx = db_.rawQuery(
                "select name, db_createdate from t_dbsystem limit(1)", null);
        try {
            if (ctx.moveToNext()) {
                name = ctx.getString(0);
                createdate = ctx.getString(1);
            }
        } finally {
            ctx.close();
        }
        tax = tax_;
    }

    private RouteDB() {
        ASSERT(false, "bug");
    }

    public static RouteDB getInstance() {
        if (obj == null) {
            ASSERT(false, "bug");
        }
        return obj;
    }

    // start constructor (Initialize create)
    public static RouteDB createFactory(SQLiteDatabase db_, int tax_) {
        _db = null;
        obj = null;
        obj = new RouteDB(db_, tax_);
        return obj;
    }

    static SQLiteDatabase db() {
        if (_db == null) {
            ASSERT(false, "Must be call createFactory(SQLiteDatabase)");
        }
        return obj._db;
    }

    public String name() {
        return name;
    }
    public int tax() {
        return tax;
    }
    public String dbDate() {
        return createdate;
    }
    public boolean isLatest() {
        return DatabaseOpenHelper.Companion.dbIndex() == DatabaseOpenHelper.Companion.getDEFAULT_DB_IDX();
    }

}
