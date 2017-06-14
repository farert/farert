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
////////////////////////////////////////////
//	DbidOf
//
internal class DbidOf {

    var StationIdOf_SHINOSAKA: Int = 0        // 新大阪
    var StationIdOf_OSAKA: Int = 0            // 大阪
    var StationIdOf_KOUBE: Int = 0            // 神戸
    var StationIdOf_HIMEJI: Int = 0            // 姫路
    var StationIdOf_NISHIAKASHI: Int = 0    // 西明石
    var LineIdOf_TOKAIDO: Int = 0        // 東海道線
    var LineIdOf_SANYO: Int = 0                // 山陽線
    var LineIdOf_TOKAIDOSHINKANSEN: Int = 0 // 東海道新幹線
    var LineIdOf_SANYOSHINKANSEN: Int = 0    // 山陽新幹線
    var LineIdOf_HAKATAMINAMISEN: Int = 0    // 博多南線
    var LineIdOf_OOSAKAKANJYOUSEN: Int = 0    // 大阪環状線

    var StationIdOf_KITASHINCHI: Int = 0    // 北新地
    var StationIdOf_AMAGASAKI: Int = 0    // 尼崎

    var StationIdOf_KOKURA: Int = 0        // 小倉
    var StationIdOf_NISHIKOKURA: Int = 0    // 西小倉
    var StationIdOf_HAKATA: Int = 0        // 博多
    var StationIdOf_YOSHIZUKA: Int = 0    // 吉塚

    var StationIdOf_MAIBARA: Int = 0     // 米原
    var Cline_align_id: Int = 0            // 会社線路線ID境界(JR線のLast Index)

    init {
        StationIdOf_OSAKA = RouteUtil.GetStationId("大阪")
        StationIdOf_SHINOSAKA = RouteUtil.GetStationId("新大阪")
        StationIdOf_KOUBE = RouteUtil.GetStationId("神戸")
        StationIdOf_HIMEJI = RouteUtil.GetStationId("姫路")
        StationIdOf_NISHIAKASHI = RouteUtil.GetStationId("西明石")
        LineIdOf_TOKAIDO = RouteUtil.GetLineId("東海道線")
        LineIdOf_SANYO = RouteUtil.GetLineId("山陽線")
        LineIdOf_TOKAIDOSHINKANSEN = RouteUtil.GetLineId("東海道新幹線")
        LineIdOf_SANYOSHINKANSEN = RouteUtil.GetLineId("山陽新幹線")
        LineIdOf_HAKATAMINAMISEN = RouteUtil.GetLineId("博多南線")
        LineIdOf_OOSAKAKANJYOUSEN = RouteUtil.GetLineId("大阪環状線")

        StationIdOf_KITASHINCHI = RouteUtil.GetStationId("北新地")
        StationIdOf_AMAGASAKI = RouteUtil.GetStationId("尼崎")

        StationIdOf_KOKURA = RouteUtil.GetStationId("小倉")
        StationIdOf_NISHIKOKURA = RouteUtil.GetStationId("西小倉")
        StationIdOf_HAKATA = RouteUtil.GetStationId("博多")
        StationIdOf_YOSHIZUKA = RouteUtil.GetStationId("吉塚")

        StationIdOf_MAIBARA = RouteUtil.GetStationId("米原")

        /**** global variables  */

        Cline_align_id = 300
        val dbo = RouteDB.db().rawQuery("select cline_align_id from t_global limit(1)", null)
        try {
            if (dbo.moveToNext()) {
                Cline_align_id = dbo.getInt(0)
            }
        } finally {
            dbo.close()
        }

        RouteUtil.ASSERT(0 < StationIdOf_SHINOSAKA)
        RouteUtil.ASSERT(0 < StationIdOf_OSAKA)
        RouteUtil.ASSERT(0 < StationIdOf_KOUBE)
        RouteUtil.ASSERT(0 < StationIdOf_HIMEJI)
        RouteUtil.ASSERT(0 < StationIdOf_NISHIAKASHI)
        RouteUtil.ASSERT(0 < LineIdOf_TOKAIDO)
        RouteUtil.ASSERT(0 < LineIdOf_SANYO)
        RouteUtil.ASSERT(0 < LineIdOf_TOKAIDOSHINKANSEN)
        RouteUtil.ASSERT(0 < LineIdOf_SANYOSHINKANSEN)
        RouteUtil.ASSERT(0 < LineIdOf_HAKATAMINAMISEN)
        RouteUtil.ASSERT(0 < LineIdOf_OOSAKAKANJYOUSEN)

        RouteUtil.ASSERT(0 < StationIdOf_KITASHINCHI)
        RouteUtil.ASSERT(0 < StationIdOf_AMAGASAKI)

        RouteUtil.ASSERT(0 < StationIdOf_KOKURA)
        RouteUtil.ASSERT(0 < StationIdOf_NISHIKOKURA)
        RouteUtil.ASSERT(0 < StationIdOf_HAKATA)
        RouteUtil.ASSERT(0 < StationIdOf_YOSHIZUKA)

        RouteUtil.ASSERT(0 < StationIdOf_MAIBARA)
    }

    companion object {
        //public:
        private val obj = DbidOf()

        fun id(): DbidOf {
            return obj
        }
    }
} // end of class DbidOf
