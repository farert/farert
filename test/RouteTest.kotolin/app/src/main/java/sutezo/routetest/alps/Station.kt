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

// station_id & line_id
//
internal class Station {
    var lineId: Short = 0
    var stationId: Short = 0

    constructor() {
        stationId = 0
        lineId = stationId
    }

    constructor(lineId_: Short, stationId_: Short) {
        lineId = lineId_
        stationId = stationId_
    }

    constructor(lineId_: Int, stationId_: Int) : this(lineId_.toShort(), stationId_.toShort()) {}

    operator fun set(lineId_: Int, stationId_: Int) {
        lineId = lineId_.toShort()
        stationId = stationId_.toShort()
    }

    fun let(item: Station) {
        lineId = item.lineId
        stationId = item.stationId
    }

    fun set(routeItem: RouteItem) {
        lineId = routeItem.lineId
        stationId = routeItem.stationId
    }

    fun is_equal(item_: Station): Boolean {
        return lineId == item_.lineId && stationId == item_.stationId
    }

    fun is_equal(item_: RouteItem): Boolean {
        return lineId == item_.lineId && stationId == item_.stationId
    }
}
