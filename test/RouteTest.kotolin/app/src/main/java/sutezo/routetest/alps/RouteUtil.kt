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


class RouteUtil {

    internal val CLEAR_HISTORY = "(clear)"

    internal enum class LINE_DIR {
        LDIR_NONE,
        LDIR_ASC, // 下り
        LDIR_DESC
        // 上り
    }

    companion object {

        internal var g_tax: Int = 0    /* in alps_mfc.cpp(Windows) or main.m(iOS) or main.cpp(unix) */

        // (System->User)

        internal fun IS_MAIHAN_CITY_START_TERMINAL_EN(flg: Int): Boolean {
            return flg and 0x03 != 0x00
        }

        internal fun IS_MAIHAN_CITY_START(flg: Int): Boolean {
            return flg and 0x03 == 0x01
        }

        internal fun IS_MAIHAN_CITY_TERMINAL(flg: Int): Boolean {
            return flg and 0x03 == 0x02
        }

        // bit 2-3
        internal fun IS_OSAKAKAN_DETOUR_EN(flg: Int): Boolean {
            return flg and 0x0c != 0
        }

        // TRUE: Detour / false: Shortcut
        internal fun IS_OSAKAKAN_DETOUR_SHORTCUT(flg: Int): Boolean {
            return flg and 0x0c == 0x08
        }

        // bit 4-5
        internal fun IS_RULE_APPLIED_EN(flg: Int): Boolean {
            return flg and 0x30 != 0
        }

        internal fun IS_RULE_APPLIED(flg: Int): Boolean {
            return flg and 0x30 == 0x10
        }


        internal fun IS_FAREOPT_ENABLED(flg: Int): Boolean {
            return flg and 0x3f != 0
        }


        internal val ADDRC_LAST = 0   // add() return code
        internal val ADDRC_OK = 1
        internal val ADDRC_END = 5
        internal val ADDRC_CEND = 4
        // ADDRC_NG -1 to -N

        /* cooked flag for shoFare(), show_route() */
        // bit 15  (User->System)
        internal val FAREOPT_AVAIL_RULE_APPLIED = 0x8000    // 有効ビットマスク
        internal val FAREOPT_RULE_NO_APPLIED = 0x8000    // 特別規則適用なし
        internal val FAREOPT_RULE_APPLIED = 0        // 通常

        // bit 0-1 (User->System)
        internal val FAREOPT_AVAIL_APPLIED_START_TERMINAL = 1   // 有効ビットマスク
        internal val FAREOPT_APPLIED_START = 1    // 名阪間 発駅を市内駅に適用
        internal val FAREOPT_APPLIED_TERMINAL = 2    // 名阪間 着駅を市内駅に適用

        // length define(UTF-8)
        internal val MAX_STATION_CHR = 64        // 38
        internal val MAX_LINE_CHR = 128
        internal val MAX_COMPANY_CHR = 64        //
        internal val MAX_PREFECT_CHR = 16
        internal val MAX_COREAREA_CHR = 26

        internal val MAX_NUM_JCT_STATION = 103
        internal val MAX_JCT_NUM = 6        // 東京駅乗り入れ路線数が最大で6
        internal val MAX_JCT = 330        // 319

        internal val MAX_COMPNPASSSET = 3       // 会社線 限定的 通過連絡運輸での 有効路線数の最大 （篠ノ井線、信越線(篠ノ井-長野)）*/

        /* t_clinfar */
        internal fun IS_ROUND_UP_CHILDREN_FARE(d: Int): Boolean {
            return d and 0x01 != 0
        }

        internal fun IS_CONNECT_NON_DISCOUNT_FARE(d: Int): Boolean {
            return d and 0x02 != 0
        }

        internal val CSTART = 1
        internal val CEND = 2

        // DB 固定定義(DB変更のない前提のDB値定義)
        //	速度考慮しSQLで取得しない
        //
        internal val JR_HOKKAIDO = 1
        internal val JR_EAST = 2
        internal val JR_CENTRAL = 3
        internal val JR_WEST = 4
        internal val JR_KYUSYU = 5
        internal val JR_SHIKOKU = 6
        internal val JR_GROUP_MASK = 1 shl 5 or (1 shl 4) or (1 shl 3) or (1 shl 2) or (1 shl 1) or (1 shl 0)

        internal val LINE_TOHOKU_SINKANSEN = 1    // 東北新幹線
        internal val LINE_JYOETSU_SINKANSEN = 2    // 上越新幹線
        internal val LINE_HOKURIKU_SINKANSEN = 3    // 北陸長野新幹線
        internal val LINE_TOKAIDO_SINKANSEN = 4    // 東海道新幹線
        internal val LINE_SANYO_SINKANSEN = 5    // 山陽新幹線
        internal val LINE_KYUSYU_SINKANSEN = 6    // 九州新幹線
        internal val LINE_HOKKAIDO_SINKANSEN = 7    // 北海道新幹線
        internal val LINE_YAMAGATA_SINKANSEN = 8    // 山形新幹線
        internal val LINE_NAGASAKI_SINKANSEN = 9    // 長崎新幹線


        internal val CITYNO_TOKYO = 1            // 東京都区内[区]
        internal val CITYNO_YOKOHAMA = 2        // 横浜市内[浜]
        internal val CITYNO_NAGOYA = 3        // 名古屋市内[名]
        internal val CITYNO_KYOUTO = 4        // 京都市内[京]
        internal val CITYNO_OOSAKA = 5        // 大阪市内[阪]
        internal val CITYNO_KOUBE = 6            // 神戸市内[神]
        internal val CITYNO_HIROSIMA = 7        // 広島市内[広]
        internal val CITYNO_KITAKYUSYU = 8    // 北九州市内[九]
        internal val CITYNO_FUKUOKA = 9        // 福岡市内[福]
        internal val CITYNO_SENDAI = 10        // 仙台市内[仙]
        internal val CITYNO_SAPPORO = 11        // 札幌市内[札]
        internal val CITYNO_YAMATE = 12        // 山手線内[山]
        internal val CITYNO_SHINOOSAKA = 13   // 大阪・新大阪

        internal val STATION_ID_AS_CITYNO = 10000  // station_id or cityno

        internal val URB_TOKYO = 1
        internal val URB_NIGATA = 2
        internal val URB_OSAKA = 3
        internal val URB_FUKUOKA = 4
        internal val URB_SENDAI = 5

        internal val FLAG_FARECALC_INITIAL = 1 shl 15
        fun MASK_CITYNO(flg: Int): Int {
            return flg and 0x0f
        }

        /* 近郊区間 */
        internal val BCSUBURB = 7
        internal val MASK_URBAN = 0x380
        internal fun URBAN_ID(flg: Int): Int {
            return flg.toInt().ushr(7) and 7
        }

        internal fun IS_OSMSP(flg: Int): Boolean {
            return flg and (1 shl 11) != 0    /* 大阪電車特定区間 ?*/
        }

        internal fun IS_TKMSP(flg: Int): Boolean {
            return flg and (1 shl 10) != 0    /* 東京電車特定区間 ?*/
        }

        internal fun IS_YAMATE(flg: Int): Boolean {
            return flg and (1 shl 5) != 0    /* 山点線内／大阪環状線内 ?*/
        }

        internal fun IS_COMPANY_LINE(d: Int): Boolean {
            return DbidOf.id().Cline_align_id < d    /* 会社線id */
        }

        // 会社線か？(GetDistanceEx()[5])
        //final static boolean GDIS_COMPANY_LINE(int d) {
        //    //return (((d) & (1<<31)) != 0);
        //}

        internal val BCBULURB = 13    // FARE_INFO.flag: ONの場合大都市近郊区間特例無効(新幹線乗車している)

        /*
  RouteItem[] 先頭のlineId
  0x01 発駅が都区市内
  0x02 着駅が都区市内
*/

        internal val MASK_ROUTE_FLAG_LFLG = 0xff000000.toInt()
        internal val MASK_ROUTE_FLAG_SFLG = 0x0000ffff


        // 1レコードめのlineIdの定義
        // bit0-9
        internal val B1LID_MARK = 0x8000        // line_idとの区別で負数になるように
        internal val B1LID_BEGIN_CITY = 0    // [区][浜][名][京][阪][神][広][九][福][仙][札]
        internal val B1LID_FIN_CITY = 1
        internal val B1LID_BEGIN_YAMATE = 2    // [山]
        internal val B1LID_FIN_YAMATE = 3
        internal val B1LID_BEGIN_2CITY = 4    // not used
        internal val B1LID_FIN_2CITY = 5    // not used
        internal val B1LID_BEGIN_CITY_OFF = 6
        internal val B1LID_FIN_CITY_OFF = 7
        internal val B1LID_BEGIN_OOSAKA = 8    // 大阪・新大阪
        internal val B1LID_FIN_OOSAKA = 9
        internal val M1LID_CITY = 0x3ff
        // bit10-14 reserve


        internal val BCRULE70 = 6        /* DB:lflag */

        internal val FLG_HIDE_LINE = 1 shl 19
        internal val FLG_HIDE_STATION = 1 shl 18
        internal fun IS_FLG_HIDE_LINE(lflg: Int): Boolean {
            return 0 != lflg and FLG_HIDE_LINE        // 路線非表示
        }

        internal fun IS_FLG_HIDE_STATION(lflg: Int): Boolean {
            return 0 != lflg and FLG_HIDE_STATION    // 駅非表示
        }


        /* util */
        internal fun MASK(bdef: Int): Int {
            return 1 shl bdef
        }

        internal fun BIT_CHK(flg: Int, bdef: Int): Boolean {
            return 0 != flg and MASK(bdef)
        }

        internal fun BIT_CHK2(flg: Int, bdef1: Int, bdef2: Int): Boolean {
            return 0 != flg and (MASK(bdef1) or MASK(bdef2))
        }

        internal fun BIT_CHK3(flg: Int, bdef1: Int, bdef2: Int, bdef3: Int): Boolean {
            return 0 != flg and (MASK(bdef1) or MASK(bdef2) or MASK(bdef3))
        }

        internal fun BIT_ON(flg: Int, bdef: Int): Int {
            return flg or MASK(bdef)
        }

        internal fun BIT_OFF(flg: Int, bdef: Int): Int {
            return flg and MASK(bdef).inv()
        }

        internal fun BIT_OFFN(flag: Int, pattern: Int): Int {
            return flag and pattern.inv()
        }

        internal val HWORD_BIT = 16        /* Number of bit in half word(unsigned short) */

        /* ---------------------------------------!!!!!!!!!!!!!!! */
        internal val MAX_STATION = 4590
        internal val MAX_LINE = 223
        internal fun IS_SHINKANSEN_LINE(id: Int): Boolean {
            return 0 < id && id <= 15    /* 新幹線は将来的にも10または15以内 !! */
        }
        //#define IS_COMPANY_LINE_OF(id)     (211<(id))			/* !!!!!!!!!!!!! */
        //#define MAX_JCT 325
        /* ---------------------------------------!!!!!!!!!!!!!!! */

        // 駅は分岐駅か
        internal fun STATION_IS_JUNCTION(sid: Int): Boolean {
            return 0 != AttrOfStationId(sid) and (1 shl 12)
        }
        //#define STATION_IS_JUNCTION_F(flg)	(0 != (flg & (1<<12)))
        // sflg.12は特例乗り換え駅もONなのでlflg.15にした

        internal fun STATION_IS_JUNCTION_F(flg: Int): Boolean {
            return 0 != flg and (1 shl 15)
        }

        /* ID for line_id on t_lines */
        internal val ID_L_RULE70: Short = -10

        internal fun Limit(v: Int, l: Int, h: Int): Int {
            return Math.min(Math.max(v, l), h)
        }


        /* 消費税(四捨五入)加算 */
        internal fun taxadd(fare: Int, tax: Int): Int {
            return fare + (fare * 1000 * tax / 100000 + 5) / 10 * 10
        }

        internal fun taxadd_ic(fare: Int, tax: Int): Int {
            return fare + fare * 1000 * tax / 100000
        }

        /* round up on 5 */
        internal fun round(d: Int): Int {
            return (d + 5) / 10 * 10    /* 10円未満四捨五入 */
        }

        internal fun round_up(d: Int): Int {
            return (d + 9) / 10 * 10    /* 10円未満切り上げ */
        }

        internal fun round_down(d: Int): Int {
            return d / 10 * 10            /* 10円未満切り捨て */
        }


        //////////////////////////////////////////////////////
        // 	文字列は「漢字」か「かな」か？
        //	「かな」ならTrueを返す
        //
        // 	@param [in] szStr    文字列
        // 	@retval true  文字列はひらがなまたはカタカナのみである
        // 	@retval false 文字列はひらがな、カタカナ以外の文字が含まれている
        //
        internal fun isKanaString(szStr: String): Boolean {
            var i: Int
            i = 0
            while (i < szStr.length) {
                val c = szStr[i]
                if ((c < 'ぁ' || 'ん' < c) && (c < 'ァ' || 'ン' < c)) {
                    return false
                }
                i++
            }
            return 0 == i
        }

        //	カナをかなに
        //
        //	@param [in][out] kana_str  変換文字列
        //
        internal fun conv_to_kana2hira(kana_str: String): String {
            val sb = StringBuffer(kana_str)
            var i = 0
            while (i < sb.length()) {
                val c = sb[i]
                if (c >= 'ァ' && c <= 'ン') {
                    sb.setCharAt(i, (c - 'ァ' + 'ぁ').toChar())
                } else if (c == 'ヵ') {
                    sb.setCharAt(i, 'か')
                } else if (c == 'ヶ') {
                    sb.setCharAt(i, 'け')
                } else if (c == 'ヴ') {
                    sb.setCharAt(i, 'う')
                    sb.insert(i + 1, '゛')
                    i++
                }
                i++
            }
            return sb.toString()
        }

        //	3桁毎にカンマを付加した数値文字列を作成
        //	(1/10した小数点付き営業キロ表示用)
        //
        //	@param [in] num  数値
        //	@return 変換された文字列(ex. 61000 -> "6,100.0", 25793 -> "2,579.3")
        //
        internal fun num_str_km(num: Int): String {
            return String.format("%,d.%d", num / 10, num % 10)
        }

        //	3桁毎にカンマを付加した数値文字列を作成
        //	金額表示用
        //
        //	@param [in] num  数値
        //	@return 変換された文字列(ex. 61000 -> "\61,000", 3982003 -> "3,982,003")
        //
        internal fun num_str_yen(num: Int): String {
            return String.format("%,d", num)
        }


        // static
        //	会社&都道府県の列挙
        //
        //	@return DBクエリ結果オブジェクト(会社(JRのみ)、都道府県一覧)
        //
        internal fun Enum_company_prefect(): Cursor {
            val tsql = "select name, rowid from t_company where name like 'JR%'" +
                    " union" +
                    " select name, rowid*65536 from t_prefect order by rowid"
            return RouteDB.db().rawQuery(tsql, null)    // , false);
        }

        // static
        //	駅名のパターンマッチの列挙(ひらがな、カタカナ、ひらがなカタカナ混じり、漢字）
        //  ひらがな昇順ソートで返す / 会社線駅も含む
        //
        //	@param [in] station   えきめい
        //	@return DBクエリ結果オブジェクト(駅名)
        //
        internal fun Enum_station_match(station: String): Cursor {
            val sql: String
            //	final char tsql[] = "/**/select name, rowid, samename from t_station where (sflg&(1<<18))=0 and %s like '%q%%'";
            val tsql = "/**/select name, rowid, samename from t_station where %s like '%s%%'"
            val tsql_s = " and samename='%s'"
            val tsql_e = " order by kana"

            val sameName: String
            var stationName = String(station)        // WIN32 str to C++ string

            if (MAX_STATION_CHR < stationName.length) {
                stationName = stationName.substring(0, MAX_STATION_CHR)
            }

            val pos = stationName.indexOf('(')
            if (0 <= pos) {
                sameName = stationName.substring(pos)
                stationName = stationName.substring(0, pos)
                /* #ifdef _WINDOWS
 			CT2A qsName(stationName, CP_UTF8);
 			CT2A qsSame(sameName, CP_UTF8);

 			sqlite3_snprintf(sizeof(sql), sql, tsql,
 									"name", qsName);
 			sqlite3_snprintf(sizeof(sql) - lstrlenA(sql), sql + lstrlenA(sql), tsql_s,
 									qsSame);
 	#else */
                sql = String.format(tsql, "name", stationName) + String.format(tsql_s, sameName)
                /* #endif */
            } else {
                val bKana: Boolean
                if (isKanaString(stationName)) {
                    stationName = conv_to_kana2hira(stationName)
                    bKana = true
                } else {
                    bKana = false
                }
                /* #ifdef _WINDOWS
 			CT2A qsName(stationName, CP_UTF8);	// C++ string to UTF-8
 			sqlite3_snprintf(sizeof(sql), sql, tsql,
 									bKana ? "kana" : "name", qsName);
 	#else */
                sql = String.format(tsql, if (bKana) "kana" else "name", stationName)
                /* #endif */
            }
            return RouteDB.db().rawQuery(sql + tsql_e, null)    //, false);
        }

        // static
        //	都道府県or会社に属する路線の列挙(JRのみ)
        //
        //	@param [in] id   0x10000 <= 都道府県番号 << 16
        //				     < 0x10000  会社ID
        //	@return DBクエリ結果オブジェクト(路線)
        //
        internal fun Enum_lines_from_company_prefect(id: Int): Cursor {
            val sql: String        // [300];
            val tsql = "select n.name, line_id, lflg from t_line n" +
                    " left join t_lines l on n.rowid=l.line_id" +
                    " left join t_station t on t.rowid=l.station_id" +
                    " where %s=%d" +
                    " and (l.lflg&((1<<18)|(1<<31)|(1<<17)))=0" +
                    " group by l.line_id order by n.name"

            val ident: Int

            if (0x10000 <= id) {
                ident = 0xffff and id.ushr(16)
            } else {
                ident = id
            }
            sql = String.format(tsql,
                    if (0x10000 <= id) "prefect_id" else "company_id", ident)

            return RouteDB.db().rawQuery(sql, null)    // , false);
        }

        // static
        //	会社 or 都道府県 + 路線の駅の列挙
        //
        //	@param [in] prefectOrCompanyId  都道府県 or 会社ID×0x10000
        //	@param [in] lineId              路線
        //	@return DBクエリ結果オブジェクト(都道府県、会社)
        //
        internal fun Enum_station_located_in_prefect_or_company_and_line(prefectOrCompanyId: Int, lineId: Int): Cursor {
            val tsql =
                    //"select t.name, station_id from t_lines l left join t_station t on t.rowid=l.station_id "
                    //" where line_id=? and %s=? order by sales_km";
                    "/**/select t.name, station_id from t_station t left join t_lines l on t.rowid=l.station_id" + " where line_id=? and %s=? and (l.lflg&((1<<18)|(1<<31)|(1<<17)))=0 order by sales_km"
            // 18:Company, 17:virtual junction for Shinkansen

            val ident: Int

            if (0x10000 <= prefectOrCompanyId) {
                ident = 0xffff and prefectOrCompanyId.ushr(16)
            } else {
                ident = prefectOrCompanyId
            }

            val sql = String.format(tsql,
                    if (0x10000 <= prefectOrCompanyId) "prefect_id" else "company_id")

            val dbo = RouteDB.db().rawQuery(sql, arrayOf(lineId.toString(), ident.toString()))

            return dbo
        }

        // static
        //	駅名のよみを得る
        //
        //	@param [in] stationId   駅ident
        //	@return えきめい
        //
        internal fun GetKanaFromStationId(stationId: Int): String {
            val tsql = "select kana from t_station where rowid=?"

            val dbo = RouteDB.db().rawQuery(tsql, arrayOf(stationId.toString()))
            var s = ""
            try {
                if (dbo.moveToNext()) {
                    s = dbo.getString(0)
                } else {
                    /* FAILTHRUE */
                }
            } finally {
                dbo.close()
            }
            return s
        }


        // static
        //	駅の所属路線のイテレータを返す
        //
        //	@param [in] stationId   駅ident
        //	@return DBクエリ結果オブジェクト(路線)
        //	@return field0(text):路線名, field1(int):路線id, field2(int):lflg(bit31のみ)
        //
        fun Enum_line_of_stationId(stationId: Int): Cursor {
            val tsql = "select n.name, line_id, lflg from t_line n left join t_lines l on n.rowid=l.line_id" +
                    //" where station_id=? and (lflg&((1<<31)|(1<<17)))=0 and sales_km>=0";
                    " where station_id=? and (lflg&(1<<17))=0"

            val dbo = RouteDB.db().rawQuery(tsql, arrayOf(stationId.toString()))

            return dbo
        }


        //static
        //	路線の分岐駅一覧イテレータを返す
        //	@param [in] lineId	路線Id
        //	@param [in] stationId 着駅Id(この駅も一覧に含める)
        //	@return DBクエリ結果オブジェクト(分岐駅)
        //
        fun Enum_junction_of_lineId(lineId: Int, stationId: Int): Cursor {
            val tsql =
                    //--#if 1	// 実レコードではなくフラグで分岐駅判定にする（新今宮を載せたくない）
                    //#ifdef __OBJC__
                    //#else
                    //--"select t.name, station_id, lflg&(1<<15)" +
                    //--" from t_lines l left join t_station t on t.rowid=l.station_id" +
                    //--" where line_id=?1 and (lflg&(1<<17))=0 and (sflg&(1<<12))!=0 or (station_id=?2 and line_id=?1)" +
                    //--" order by l.sales_km";
                    //#endif
                    //--#else
                    //--#if 1	// 分岐特例のみの乗換駅(弘前とか)を含む
                    //--"select t.name, station_id, sflg&(1<<12)" +
                    //--" from t_lines l left join t_station t on t.rowid=l.station_id" +
                    //--" where line_id=?1 and (lflg&(1<<17))=0 and" +
                    //--" station_id in (select station_id from t_lines where line_id!=?1 or station_id=?2)" +
                    //--" order by l.sales_km";

                    //"select t.name, station_id, sflg&(1<<12)"
                    //" from t_lines l left join t_station t on t.rowid=l.station_id"
                    //" where line_id=?1 and (lflg&(1<<17))=0 and"
                    //" exists (select * from t_lines where line_id!=?1 and l.station_id=station_id)"
                    //" or (line_id=?1 and (lflg&(1<<17))=0 and station_id=?2) order by l.sales_km";
                    // -- exists を使うとやたら遅い
                    //--#else
                    "select t.name, station_id, sflg&(1<<12)" +
                            " from t_lines l left join t_station t on t.rowid=l.station_id" +
                            " where line_id=?1 and ((sflg & (1<<12))<>0 or (station_id=?2 and line_id=?1))" +
                            //" and (lflg&((1<<31)|(1<<17)))=0 and sales_km>=0"
                            " and (lflg&(1<<17))=0" +
                            " order by l.sales_km"
            //--#endif
            //--#endif

            val dbo = RouteDB.db().rawQuery(tsql, arrayOf(lineId.toString(), stationId.toString()))
            return dbo
        }

        //static
        //	路線内の駅一覧イテレータを返す
        //
        //	@param [in] lineId   駅ident
        //	@return DBクエリ結果オブジェクト(駅)
        //
        fun Enum_station_of_lineId(lineId: Int): Cursor {
            val tsql = "select t.name, station_id, lflg&(1<<15)" +
                    " from t_lines l left join t_station t on t.rowid=l.station_id" +
                    " where line_id=?" +
                    " and (lflg&((1<<31)|(1<<17)))=0" +
                    " order by l.sales_km"

            val dbo = RouteDB.db().rawQuery(tsql, arrayOf(lineId.toString()))
            return dbo
        }

        // 駅名より駅IDを返す(私鉄含)
        //
        // station_id = f("駅名")
        //
        fun GetStationId(station: String): Int {
            val tsql = "select rowid from t_station where (sflg&(1<<18))=0 and name=?1 and samename=?2"

            val sameName: String
            var stationName = String(station)

            val pos = stationName.indexOf('(')
            if (0 <= pos) {
                sameName = stationName.substring(pos)
                stationName = stationName.substring(0, pos)
            } else {
                sameName = ""
            }

            val dbo = RouteDB.db().rawQuery(tsql, arrayOf(stationName, sameName))
            var rc = 0
            try {
                if (dbo.moveToNext()) {
                    rc = dbo.getInt(0)
                }
            } finally {
                dbo.close()
            }
            return rc
        }

        // 線名より線IDを返す
        //
        // station_id = f("線名")
        //
        fun GetLineId(lineName: String): Int {
            val tsql = "select rowid from t_line where name=?"

            val dbo = RouteDB.db().rawQuery(tsql, arrayOf(lineName))
            var rc = 0
            try {
                if (dbo.moveToNext()) {
                    rc = dbo.getInt(0)
                }
            } finally {
                dbo.close()
            }
            return rc
        }

        //static
        //	駅ID→駅名
        //
        fun StationName(id: Int): String {
            var name = ""        //[MAX_STATION_CHR];

            val ctx = RouteDB.db().rawQuery(
                    "select name from t_station where rowid=?", arrayOf(id.toString()))
            try {
                if (ctx.moveToNext()) {
                    name = ctx.getString(0)
                }
            } finally {
                ctx.close()
            }
            return name
        }

        //static
        //	駅ID→駅名(同名駅区別)
        //
        fun StationNameEx(id: Int): String {
            var name = ""    //[MAX_STATION_CHR];

            val ctx = RouteDB.db().rawQuery(
                    "select name,samename from t_station where rowid=?",
                    arrayOf(id.toString()))
            try {
                if (ctx.moveToNext()) {
                    name = ctx.getString(0)
                    name += ctx.getString(1)
                }
            } finally {
                ctx.close()
            }
            return name
        }

        //static
        //	路線ID→路線名
        //
        fun LineName(id: Int): String {
            val ctx = RouteDB.db().rawQuery(
                    "select name from t_line where rowid=?", arrayOf(id.toString()))
            var name = ""   // if error
            try {
                if (ctx.moveToNext()) {
                    name = ctx.getString(0)
                }
            } finally {
                ctx.close()
            }
            return name
        }

        //static
        //	都道府県名
        //
        fun PrefectName(id: Int): String {
            var id = id
            var name = ""    //[MAX_PREFECT_CHR];

            if (0x10000 <= id) {
                id = id.ushr(16)
            }

            val ctx = RouteDB.db().rawQuery("select name from t_prefect where rowid=?",
                    arrayOf(id.toString()))
            try {
                if (ctx.moveToNext()) {
                    name = ctx.getString(0)
                }
            } finally {
                ctx.close()
            }
            return name
        }

        //static
        //	会社線名
        //
        fun CompanyName(id: Int): String {
            var name = ""    //[MAX_PREFECT_CHR];

            val ctx = RouteDB.db().rawQuery("select name from t_company where rowid=?",
                    arrayOf(id.toString()))
            try {
                if (ctx.moveToNext()) {
                    name = ctx.getString(0)
                }
            } finally {
                ctx.close()
            }
            return name
        }

        //static
        //	駅ID→駅名
        //
        fun CoreAreaCenterName(id: Int): String {
            var id = id
            var name = ""        // [MAX_COREAREA_CHR];

            if (STATION_ID_AS_CITYNO <= id) {
                id -= STATION_ID_AS_CITYNO
            }
            val ctx = RouteDB.db().rawQuery(
                    "select name from t_coreareac where rowid=?", arrayOf(id.toString()))
            try {
                if (ctx.moveToNext()) {
                    name = ctx.getString(0)
                }
            } finally {
                ctx.close()
            }
            return name
        }

        //static
        //	駅の属性を得る
        //
        internal fun AttrOfStationId(id: Int): Int {
            val ctx = RouteDB.db().rawQuery(
                    "select sflg from t_station where rowid=?", arrayOf(id.toString()))
            var rc = 1 shl 30
            try {
                if (ctx.moveToNext()) {
                    rc = ctx.getInt(0)
                }
            } finally {
                ctx.close()
            }
            return rc
        }

        //static
        //	路線駅の属性を得る
        //
        internal fun AttrOfStationOnLineLine(line_id: Int, station_id: Int): Int {
            val ctx = RouteDB.db().rawQuery(
                    //  "select lflg, sflg from t_lines where line_id=?1 and station_id=?2", true);
                    "select sflg, lflg from t_station t left join t_lines on t.rowid=station_id where line_id=?1 and station_id=?2", arrayOf(line_id.toString(), station_id.toString()))

            var rc = 1 shl 30
            try {
                if (ctx.moveToNext()) {
                    val s: Long
                    val l: Long

                    s = 0x00007fff and ctx.getLong(0)
                    l = 0xffff8000.toInt() and ctx.getLong(1)    // b15はSTATION_IS_JUNCTION_F(lflg)で必要なので注意
                    rc = (s or l).toInt()
                }
            } finally {
                ctx.close()
            }
            return rc
        }

        // static
        //	駅(Id)の都道府県を得る
        //
        //	@param [in] stationId   駅ident
        //	@return 都道府県名
        //
        fun GetPrefectByStationId(stationId: Int): String {
            val tsql =
                    //"select p.name from t_prefect p left join t_station t on t.prefect_id=p.rowid where t.rowid=?";
                    "select name from t_prefect where rowid=(select prefect_id from t_station where rowid=?)"
            //ここではどーでもいいけど、sqliteは結合遅くてサブクエリの方が早かったのでこうしてみた
            val dbo = RouteDB.db().rawQuery(tsql, arrayOf(stationId.toString()))
            var s = ""
            try {
                if (dbo.moveToNext()) {
                    s = dbo.getString(0)
                }
            } finally {
                dbo.close()
            }
            return s
        }

        // static version
        //	@brief 経由文字列を返す
        //
        //	@param [in] routeList    route
        //	@param [in] last_flag    route flag(LF_OSAKAKAN_MASK:大阪環状線関連フラグのみ).
        //	@retval 文字列
        //
        fun Show_route(routeList: Array<RouteItem>, last_flag: Int): String {
            var last_flag = last_flag
            var lineName: String
            var stationName: String

            //TCHAR buf[MAX_BUF];
            var result_str: String
            var station_id1: Int

            if (routeList.size == 0) {    /* 経路なし(AutoRoute) */
                return ""
            }

            result_str = ""
            station_id1 = routeList[0].stationId.toInt()
            last_flag = last_flag and RouteList.getMLF_OSAKAKAN_PASS().inv()    // BIT_OFF(last_flag, RouteList.BLF_OSAKAKAN_1PASS)

            for (pos in 1..routeList.size - 1) {

                lineName = LineName(routeList[pos].lineId.toInt())

                if (pos != routeList.size - 1) {
                    /* 中間駅 */
                    if (!IS_FLG_HIDE_LINE(routeList[pos].flag)) {
                        if (ID_L_RULE70 != routeList[pos].lineId) {
                            result_str += "["
                            result_str += lineName
                            if (DbidOf.id().LineIdOf_OOSAKAKANJYOUSEN === routeList[pos].lineId) {
                                result_str += RouteOsakaKanDir(station_id1, routeList[pos].stationId.toInt(), last_flag)
                                last_flag = BIT_ON(last_flag, RouteList.getBLF_OSAKAKAN_1PASS())
                            }
                            result_str += "]"
                        } else {
                            result_str += ","
                        }
                    }
                    station_id1 = routeList[pos].stationId.toInt()
                    if (!IS_FLG_HIDE_STATION(routeList[pos].flag)) {
                        stationName = StationName(station_id1)
                        result_str += stationName
                    }
                } else {
                    /* 着駅 */
                    if (!IS_FLG_HIDE_LINE(routeList[pos].flag)) {
                        result_str += "["
                        result_str += lineName
                        if (DbidOf.id().LineIdOf_OOSAKAKANJYOUSEN === routeList[pos].lineId) {
                            result_str += RouteOsakaKanDir(station_id1, routeList[pos].stationId.toInt(), last_flag)
                            last_flag = BIT_ON(last_flag, RouteList.getBLF_OSAKAKAN_1PASS())
                        }
                        result_str += "]"
                    }
                    //result_str += stationName;	// 着駅
                    result_str += "\r\n"
                }
                //result_str += buf;
            }
            return result_str
        }

        //static private
        //	@brief 大阪環状線 方向文字列を返すで
        //
        //	@param [in] station_id1  発駅
        //	@param [in] station_id2  着駅
        //	@param [in] last_flag    route flag.
        //	@retval 文字列
        //
        private fun RouteOsakaKanDir(station_id1: Int, station_id2: Int, last_flag: Int): String {
            val result_str: String
            val result = arrayOf("", "(内回り)", "(外回り)")
            val inner_outer = byteArrayOf(0, 2, 2, 2, 1, 1, 0, 1, 1, 1, 1, 1, 2, 2, 0, 2, 0)//     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
            /*
 	         f e d c b a 9 8 7 6 5 4 3 2 1 0
 	neerdir  1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0  順廻りが近道なら1
 	detour   1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0  遠回り指定で1
 	forward  1 1 1 1 0 0 0 0 1 1 1 1 0 0 0 0  逆回りで1
 	dir      1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0  下り(反時計)廻りで1
 	F/R      O   o O I I I i I   I I O O O   0/1 neer/far
 	         | | | | | | | | | | | | | | | +- 0
 	         | | | | | | | | | | | | | | +--- 0 1西九条 > 大阪    1天王寺 >> 大阪
 	         | | | | | | | | | | | | | +----- 1 1大正 >> 寺田町   1芦原橋 >> 天王寺
 	         | | | | | | | | | | | | +------- 1                   2-大阪 >> 天王寺,,,京橋 > 鶴橋
 	         | | | | | | | | | | | +--------- 0 1大正 > 寺田町
 	         | | | | | | | | | | +----------- 0                   2桃谷 大阪環状線 天王寺 関西線 ... 大阪 大阪環状線 天王寺
 	         | | | | | | | | | +------------- 1
 	         | | | | | | | | +--------------- 1 1西九条 >> 大阪
 	         | | | | | | | +----------------- 0                   2芦原橋・天王寺 … 京橋 > 大正
 	         | | | | | | +------------------- 0 1大阪 > 西九条    2天王寺 > 京橋 1/2天王寺 > 大阪
 	         | | | | | +--------------------- 1 1寺田町 >> 大正
 	         | | | | +----------------------- 1                    2-大阪 >> 天王寺,,,京橋 > 天満
 	         | | | +------------------------- 0 1寺田町 > 大正     2天王寺 > 今宮 2天王寺 > 西九条
 	         | | +--------------------------- 0                    2天王寺 |> 大阪
 	         | +----------------------------- 1
 	         +------------------------------- 1 1大阪 >> 西九条    1天王寺 >> 大阪

 	         > 近回り
 	         >>一回目遠回り
 	         |> 2回目 やむを得ず遠回り
 	         1 1回目
 	         2 2回目

 	遠回り指定、今宮経由が近道、  今宮経由、  上り 1010 x
 	遠回り指定、非今宮経由が近道、非今宮経由、下り 1101 I                   2-大阪 >> 天王寺,,,京橋 > 天満
 	遠回り指定、非今宮経由が近道、非今宮経由、上り 1100 O                   2-大阪 >> 天王寺,,,京橋 > 鶴橋
 	近回り、    非今宮経由が近道、今宮経由、  上り 0110 x
 	近回り、　　非今宮経由が近道、今宮経由、  下り 0111 x
 	近回り、　　今宮経由が近道、  非今宮経由、上り 0000 x
 	遠回り指定、今宮経由が近道、  非今宮経由、上り 1000 O 1西九条 > 大阪    1天王寺 >> 大阪
 	遠回り指定、今宮経由が近道、  今宮経由、  下り 1011 o                   2天王寺 |> 大阪
 	遠回り指定、非今宮経由が近道、今宮経由、  下り 1111 O 1大阪 >> 西九条    1天王寺 >> 大阪
 	近回り、　　今宮経由が近道、  今宮経由、  下り 0011 O 1寺田町 > 大正     2天王寺 > 今宮 2天王寺 > 西九条
 	近回り、　　非今宮経由が近道、非今宮経由、上り 0100 O 1大正 >> 寺田町   1芦原橋 >> 天王寺
 	近回り、    今宮経由が近道、  非今宮経由、下り 0001 i 2-芦原橋 大阪環状線 天王寺 関西線 久宝寺 おおさか東線 放出 片町線 京橋 大阪環状線 大正
 	遠回り指定、今宮経由が近道、  非今宮経由、下り 1001 I 1大阪 > 西九条      2天王寺 > 京橋 1/2天王寺 > 大阪
 	遠回り指定、非今宮経由が近道、今宮経由、  上り 1110 I 1西九条 >> 大阪
 	近回り、　　非今宮経由が近道、非今宮経由、下り 0101 I 1寺田町 >> 大正
 	近回り、　　今宮経由が近道、  今宮経由、  上り 0010 I 1大正 > 寺田町
 	*/
            val pass: Int
            var c: Int

            c = if (DirOsakaKanLine(station_id1, station_id2) == 0) 1 else 0
            c = c or if (BIT_CHK(last_flag, RouteList.getBLF_OSAKAKAN_DETOUR())) 0x02 else 0
            pass = if (BIT_CHK(last_flag, RouteList.getBLF_OSAKAKAN_1PASS())) RouteList.getBLF_OSAKAKAN_2DIR() else RouteList.getBLF_OSAKAKAN_1DIR()
            c = c or if (BIT_CHK(last_flag, pass)) 0x04 else 0
            c = c or if (LINE_DIR.LDIR_ASC == DirLine(DbidOf.id().LineIdOf_OOSAKAKANJYOUSEN, station_id1, station_id2)) 0x08 else 0
            System.out.printf("RouteOsakaKanDir:[%d] %s %s %s: %d %d %d %d\n",
                    if (pass == RouteList.getBLF_OSAKAKAN_2DIR()) 2 else 1,
                    StationName(station_id1),
                    if (0 != c and 0x02 && pass != RouteList.getBLF_OSAKAKAN_2DIR()) ">>" else ">",
                    StationName(station_id2), 0x1 and c, 0x1 and c.ushr(1), 0x01 and c.ushr(2), 0x01 and c.ushr(3))

            if (inner_outer.size <= c) {
                c = inner_outer.size - 1
            }
            return result[inner_outer[c]]
        }


        //static
        //	大阪環状線最短廻り方向を返す
        //
        //	@param [in]  station_id_a   発or至
        //	@param [in]  station_id_b   発or至
        // 	@retval 0 = 内回り(DB定義上の順廻り)が最短
        // 	@retval 1 = 外回りが最短
        //
        //	@note station_id_a, station_id_bは区別はなし
        //
        fun DirOsakaKanLine(station_id_a: Int, station_id_b: Int): Int {
            if (GetDistance(DbidOf.id().LineIdOf_OOSAKAKANJYOUSEN, station_id_a, station_id_b)[0].toInt() <= GetDistanceOfOsakaKanjyouRvrs(DbidOf.id().LineIdOf_OOSAKAKANJYOUSEN, station_id_a, station_id_b)) {
                return 0
            } else {
                return 1
            }
        }

        //static
        //	駅の所属会社IDを得る
        //	(境界駅はあいまい)
        //
        //	@param [in] station_id   駅id
        //
        internal fun CompanyIdFromStation(station_id: Int): Int {
            val ctx = RouteDB.db().rawQuery(
                    "select company_id from t_station where rowid=?",
                    arrayOf(station_id.toString()))
            var rc = 0
            try {
                if (ctx.moveToNext()) {
                    rc = ctx.getInt(0)
                }
            } finally {
                ctx.close()
            }
            return rc
        }

        internal fun CompanyAnotherIdFromStation(station_id: Int): Int {
            val ctx = RouteDB.db().rawQuery(
                    "select sub_company_id from t_station where rowid=?",
                    arrayOf(station_id.toString()))
            var rc = 0
            try {
                if (ctx.moveToNext()) {
                    rc = ctx.getInt(0)
                }
            } finally {
                ctx.close()
            }
            return rc
        }

        //static
        //	運賃計算キロと営業キロを返す
        //	(大阪環状線は新今宮を通らない経路)
        //	@param [in] line_id     路線
        //	@param [in] station_id1 発
        //	@param [in] station_id2 至
        //
        //	@retuen [0]:営業キロ, [1]:計算キロ
        //
        internal fun GetDistance(line_id: Int, station_id1: Int, station_id2: Int): List<Int> {
            val tsql = "select max(sales_km)-min(sales_km), case max(calc_km)-min(calc_km) when 0 then max(sales_km)-min(sales_km) else max(calc_km)-min(calc_km) end" +
                    " from t_lines" +
                    " where line_id=?1 and (lflg&(1<<31))=0 and (station_id=?2 or station_id=?3)"

            /***	// 140416
             * "select max(sales_km)-min(sales_km), case max(calc_km)-min(calc_km) when 0 then max(sales_km)-min(sales_km) else max(calc_km)-min(calc_km) end"
             * " from t_lines"
             * " where line_id=?1"
             * " and	(lflg&(1<<31))=0"
             * " and	sales_km>=(select min(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))"
             * " and	sales_km<=(select max(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))";

             * ///old #if 0
             * "select l1.sales_km - l2.sales_km, l1.calc_km - l2.calc_km"
             * " from t_lines l1"
             * " left join t_lines l2"
             * " where l1.line_id=?1"
             * " and l2.line_id=?1"
             * " and l1.sales_km>l2.sales_km"
             * " and ((l1.station_id=?2 and l2.station_id=?3)"
             * " or (l1.station_id=?3 and l2.station_id=?2))";
             */
            val v = ArrayList<Int>(2)

            val dbo = RouteDB.db().rawQuery(tsql, arrayOf(line_id.toString(), station_id1.toString(), station_id2.toString()))
            try {
                if (dbo.moveToNext()) {
                    v.add(dbo.getInt(0))
                    v.add(dbo.getInt(1))
                }
            } finally {
                dbo.close()
            }
            return v
        }


        //static
        //	運賃計算キロと営業キロを返す(大阪環状線)
        //
        //	@param [in] line_id     路線
        //	@param [in] station_id1 発
        //	@param [in] station_id2 至
        //	@param [in] flag        b1lid flag
        //
        //	@retuen [0]:営業キロ, [1]:計算キロ
        //	@note used aggregate_fare_info()* -> GetDistanceEx(), Get_route_distance()
        //
        internal fun GetDistance(oskkflg: Int, line_id: Int, station_id1: Int, station_id2: Int): List<Int> {
            val d = ArrayList<Int>(2)
            val sales_km: Int

            if (line_id != DbidOf.id().LineIdOf_OOSAKAKANJYOUSEN) {
                RouteUtil.ASSERT(false)
                return GetDistance(line_id, station_id1, station_id2)
            }

            // pass 0 1 0 1 0 1 0 1
            // 1dir 0 0 1 1 0 0 1 1
            // 2dir 0 0 0 0 1 1 1 1
            //      0 0 1 0 0 1 1 1
            // ~pass & 1dir | pass & 2dir

            if (oskkflg and (1 shl RouteList.getBLF_OSAKAKAN_1PASS() or (1 shl RouteList.getBLF_OSAKAKAN_1DIR())) == 1 shl RouteList.getBLF_OSAKAKAN_1DIR() || oskkflg and (1 shl RouteList.getBLF_OSAKAKAN_1PASS() or (1 shl RouteList.getBLF_OSAKAKAN_2DIR())) == 1 shl RouteList.getBLF_OSAKAKAN_1PASS() or (1 shl RouteList.getBLF_OSAKAKAN_2DIR())) {
                sales_km = GetDistanceOfOsakaKanjyouRvrs(line_id, station_id1, station_id2)
                System.out.printf("Osaka-kan reverse\n")
            } else {
                sales_km = GetDistance(line_id, station_id1, station_id2)[0]
                System.out.printf("Osaka-kan forward\n")
            }

            d.add(sales_km)
            d.add(sales_km)

            return d
        }


        //static
        //	営業キロを算出（大阪環状線 新今宮を通る経路)
        //
        //	@param [in] line_id     大阪環状線line id
        //	@param [in] station_id1 発
        //	@param [in] station_id2 至
        //
        //	@retuen 営業キロ
        //
        internal fun GetDistanceOfOsakaKanjyouRvrs(line_id: Int, station_id1: Int, station_id2: Int): Int {
            // 新今宮までの距離(近いほう) + 天王寺までの距離(近いほう) + 新今宮～天王寺

            val tsql = "select" +
                    " (select max(sales_km)-min(sales_km) from t_lines where line_id=?1 and " +
                    "  ((station_id=(select rowid from t_station where name='新今宮')) or" +
                    "   (sales_km=(select max(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3)))))+" +
                    " (select min(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))+" +
                    " (select max(sales_km)-min(sales_km) from t_lines where line_id=" +
                    "  (select rowid from t_line where name='関西線') and station_id in (select rowid from t_station where name='新今宮' or name='天王寺'))"
            val dbo = RouteDB.db().rawQuery(tsql, arrayOf(line_id.toString(), station_id1.toString(), station_id2.toString()))

            var km = 0

            try {
                if (dbo.moveToNext()) {
                    km = dbo.getInt(0)
                    RouteUtil.ASSERT(0 < km)
                    return km
                }
            } finally {
                dbo.close()
            }
            RouteUtil.ASSERT(false)
            return km
        }

        //static
        //	路線が上り／下りかを得る
        //
        //	@param [in] line_id     路線
        //	@param [in] station_id1 発
        //	@param [in] station_id2 至
        //
        //	@retval 1 下り(LDIR_ASC)
        //	@retval 2 上り(LDIR_DESC)
        //
        //  @node 同一駅の場合下り(0)を返す
        //
        //
        internal fun DirLine(line_id: Int, station_id1: Int, station_id2: Int): LINE_DIR {
            val tsql = "select case when" +
                    "   ((select sales_km from t_lines where line_id=?1 and station_id=?2) - " +
                    "    (select sales_km from t_lines where line_id=?1 and station_id=?3)) <= 0" +
                    " then 1 else 2 end"

            val dbo = RouteDB.db().rawQuery(tsql, arrayOf(line_id.toString(), station_id1.toString(), station_id2.toString()))

            var rc = LINE_DIR.LDIR_ASC    /* default is 下り */
            try {
                if (dbo.moveToNext()) {
                    rc = if (1 == dbo.getInt(0)) LINE_DIR.LDIR_ASC else LINE_DIR.LDIR_DESC
                }
            } finally {
                dbo.close()
            }
            return rc
        }

        @JvmOverloads internal fun GetHZLine(line_id: Int, station_id: Int, station_id2: Int = -1 /* =-1 */): Int {
            var i: Int
            var w: Int
            val hzl = EnumHZLine(line_id, station_id, station_id2)

            if (hzl.size < 3) {
                //RouteUtil.ASSERT (false);
                return 0
            }
            i = 0
            while (i < hzl.size.toInt()) {
                if (0x10000 < hzl[i]) {
                    w = 0xffff and hzl[i]
                } else {
                    w = hzl[i]
                }
                if (0 != w) {
                    return w
                }
                i++
                /* 着駅までは関知しない */
            }
            return 0

            // 山陽新幹線 新大阪 姫路 は東海道線と山陽線だが東海道線を返す
            // 山陽新幹線 姫路 新大阪なら山陽線を返す
        }

        internal fun EnumHZLine(line_id: Int, station_id: Int, station_id2: Int): List<Int> {
            // 新幹線-並行在来線取得クエリ
            val tsql_hzl = "select case when(select line_id from t_hzline where rowid=(" +
                    "	select ((lflg>>19)&15) from t_lines where line_id=?1 and station_id=?2)) > 0 then" +
                    "(select line_id from t_hzline where rowid=(" +
                    "	select ((lflg>>19)&15) from t_lines where line_id=?1 and station_id=?2))" +
                    " else 0 end," +
                    "(select count(*) from t_lines where line_id=?1 and station_id=?2 and 0=(lflg&((1<<31)|(1<<17))))" +
                    " union all" +
                    " select distinct line_id, 0 from t_hzline h join (" +
                    "	select (lflg>>19)&15 as x from t_lines" +
                    "	where ((lflg>>19)&15)!=0 and (lflg&((1<<31)|(1<<17)))=0	and line_id=?1 and " +
                    "	case when (select sales_km from t_lines where line_id=?1 and station_id=?2)<" +
                    "	          (select sales_km from t_lines where line_id=?1 and station_id=?3)" +
                    "	then" +
                    "	sales_km>=(select sales_km from t_lines where line_id=?1 and station_id=?2) and" +
                    "	sales_km<=(select sales_km from t_lines where line_id=?1 and station_id=?3) " +
                    "	else" +
                    "	sales_km<=(select sales_km from t_lines where line_id=?1 and station_id=?2) and" +
                    "	sales_km>=(select sales_km from t_lines where line_id=?1 and station_id=?3)" +
                    "	end" +
                    " order by" +
                    " case when" +
                    " (select sales_km from t_lines where line_id=?1 and station_id=?3) <" +
                    " (select sales_km from t_lines where line_id=?1 and station_id=?2) then" +
                    " sales_km" +
                    " end desc," +
                    " case when" +
                    " (select sales_km from t_lines where line_id=?1 and station_id=?3) >" +
                    " (select sales_km from t_lines where line_id=?1 and station_id=?2) then" +
                    " sales_km" +
                    " end asc" +
                    ") as y on y.x=h.rowid " +
                    " union all" +
                    " select case when(select line_id from t_hzline where rowid=(" +
                    "	select ((lflg>>19)&15) from t_lines where line_id=?1 and station_id=?3)) > 0 then" +
                    " (select line_id from t_hzline where rowid=(" +
                    "	select ((lflg>>19)&15) from t_lines where line_id=?1 and station_id=?3))" +
                    " else 0 end,   " +
                    " (select count(*) from t_lines where line_id=?1 and station_id=?3 and 0=(lflg&((1<<31)|(1<<17))))"
            /*
 	 名古屋－＞新横浜の場合
 	 	東海道線	1
 	 	東海道線	0
 	 	0			1

 	 新潟->大宮の場合
 	   信越線	1
 	   信越線	0
 	   信越線/宮内	0	loword(信越線) + hiword(宮内)
 	   上越線	0
 	   0/高崎	0		loword(0) + hiword(高崎)
 	   高崎線	0
 	   高崎線	1

 	   (着駅、発駅がどそっぽの駅の場合(新幹線にない駅）、
 	   0		0
 	   となる
 	*/

            val rslt = ArrayList<Int>()
            var lineId: Int
            var flg: Int

            val dbo = RouteDB.db().rawQuery(tsql_hzl, arrayOf(line_id.toString(), station_id.toString(), station_id2.toString()))

            RouteUtil.ASSERT(IS_SHINKANSEN_LINE(line_id))

            try {
                while (dbo.moveToNext()) {
                    lineId = dbo.getInt(0)
                    flg = dbo.getInt(1)
                    if (flg == 1 && lineId == 0) {
                        lineId = -1    /* 新幹線駅だが在来線接続駅でない */
                    } /* else if ((flg == 0) && (lineId == 0)) 不正(新幹線にない駅) */
                    rslt.add(lineId)
                }
            } finally {
                dbo.close()
            }
            return rslt
        }

        //static
        //	新幹線の駅1から駅2方面の隣の在来線接続駅を返す
        //
        //	@param [in] line_id1  新幹線
        //	@param [in] station_id1  駅1
        //	@param [in] station_id2  駅2(方向)
        //
        //	@return 駅id 0を返した場合、隣駅は駅2またはそれより先の駅
        //
        internal fun NextShinkansenTransferTerm(line_id: Int, station_id1: Int, station_id2: Int): Int {
            val tsql = "select station_id from t_lines where line_id=?1 and" +
                    " case when" +
                    "(select sales_km from t_lines where line_id=?1 and station_id=?3)<" +
                    "(select sales_km from t_lines where line_id=?1 and station_id=?2) then" +
                    " sales_km=(select max(sales_km) from t_lines where line_id=?1 and" +
                    "	((lflg>>19)&15)!=0 and (lflg&((1<<17)|(1<<31)))=0 and" +
                    "	sales_km<(select sales_km from t_lines where line_id=?1 and station_id=?2) and" +
                    "	sales_km>(select sales_km from t_lines where line_id=?1 and station_id=?3))" +
                    " else" +
                    " sales_km=(select min(sales_km) from t_lines where line_id=?1 and" +
                    "	((lflg>>19)&15)!=0 and (lflg&((1<<17)|(1<<31)))=0 and" +
                    "	sales_km>(select sales_km from t_lines where line_id=?1 and station_id=?2) and" +
                    "	sales_km<(select sales_km from t_lines where line_id=?1 and station_id=?3))" +
                    " end"

            val dbo = RouteDB.db().rawQuery(tsql, arrayOf(line_id.toString(), station_id1.toString(), station_id2.toString()))

            ASSERT(IS_SHINKANSEN_LINE(line_id))
            var stid = 0
            try {
                if (dbo.moveToNext()) {
                    stid = dbo.getInt(0)
                }
            } finally {
                dbo.close()
            }
            return stid
        }

        //static
        //	両隣の分岐駅を得る(非分岐駅指定、1つか2つ)
        //	changeNeerest() =>
        //
        //	@param [in] station_id    駅(非分岐駅)
        //	@return int[2] or int[1] 近隣の分岐駅(盲腸線の場合1、以外は2)
        //			IDENT1: station_id, IDENT2: calc_km
        //
        internal fun GetNeerNode(station_id: Int): Array<ShortArray> {
            var idx = 0
            val result = Array(2) { ShortArray(2) }
            for (i in result.indices) {
                for (j in 0..result[0].size - 1) {
                    result[i][j] = 0
                }
            }
            val dbo = Enum_neer_node(station_id)
            try {
                idx = 0
                while (idx < 2) {
                    if (dbo.moveToNext()) {
                        val stationId = dbo.getShort(0)
                        val cost = dbo.getShort(1)
                        result[idx][0] = stationId
                        result[idx][1] = cost
                    }
                    idx++
                }
            } finally {
                dbo.close()
            }
            return result
        }

        //static
        //	駅の隣の分岐駅を返す
        //	(非分岐駅を指定すると正しい結果にならない)
        //
        //	@param [in] stationId   駅ident
        //	@return DBクエリ結果オブジェクト(隣接分岐駅)
        //
        internal fun Enum_neer_node(stationId: Int): Cursor {
            val tsql = "select 	station_id , abs((" +
                    "	select case when calc_km>0 then calc_km else sales_km end " +
                    "	from t_lines " +
                    "	where 0=(lflg&((1<<31)|(1<<17))) " +
                    "	and line_id=(select line_id " +
                    "				 from	t_lines " +
                    "				 where	station_id=?1" +
                    "				 and	0=(lflg&((1<<31)|(1<<17)))) " +
                    "	and station_id=?1)-case when calc_km>0 then calc_km else sales_km end) as cost" +
                    " from 	t_lines " +
                    " where 0=(lflg&((1<<31)|(1<<17)))" +
                    " and	line_id=(select	line_id " +
                    " 				 from	t_lines " +
                    " 				 where	station_id=?1" +
                    " 				 and	0=(lflg&((1<<31)|(1<<17))))" +
                    " and	sales_km in ((select max(y.sales_km)" +
                    "					  from	t_lines x left join t_lines y" +
                    "					  on	x.line_id=y.line_id " +
                    "					  where	0<=x.sales_km and 0=(x.lflg&((1<<31)|(1<<17)))" +
                    "					  and	0<=y.sales_km and (1<<15)=(y.lflg&((1<<31)|(1<<17)|(1<<15)))" +
                    "					  and	x.station_id=?1" +
                    "					  and	x.sales_km>y.sales_km" +
                    "					 )," +
                    "					 (select min(y.sales_km)" +
                    "					  from	t_lines x left join t_lines y" +
                    "					  on	x.line_id=y.line_id " +
                    "					  where 0<=x.sales_km and 0=(x.lflg&((1<<31)|(1<<17)))" +
                    "					  and	0<=y.sales_km and (1<<15)=(y.lflg&((1<<31)|(1<<17)|(1<<15)))" +
                    "					  and	x.station_id=?1" +
                    "					  and	x.sales_km<y.sales_km))"
            val dbo = RouteDB.db().rawQuery(tsql, arrayOf(stationId.toString()))

            return dbo
        }

        //static
        // 駅の隣の分岐駅数を得る
        //	(盲腸線の駅か否かのみを返す)
        //
        //	@param [in] stationId   駅ident
        //	@return 0 to 2 (隣接分岐駅数)
        //
        fun NumOfNeerNode(stationId: Int): Int {
            if (STATION_IS_JUNCTION(stationId)) {
                return 2    // 2以上あることもあるが嘘つき
            }
            val dbo = Enum_neer_node(stationId)
            var c: Int

            c = 0
            while (dbo.moveToNext()) {
                ++c
            }
            return c
        }

        //static
        //	路線の駅間に駅はあるか？
        //	lineIdのb_stationId to e_stationId in stationId ?
        //	注： lflg&(1<<17)を含めていないため、新幹線内分岐駅、たとえば、
        //	     東海道新幹線 京都 米原間に草津駅は存在するとして返します.
        //
        //	@param [in] stationId   検索駅
        //	@param [in] lineId      路線
        //	@param [in] b_stationId 開始駅
        //	@param [in] e_stationId 終了駅
        //	@return 0: not found / not 0: ocunt of found.
        //
        internal fun InStation(stationId: Int, lineId: Int, b_stationId: Int, e_stationId: Int): Int {
            val tsql = "select count(*)" +
                    "	from t_lines" +
                    "	where line_id=?1" +
                    "	and station_id=?4" +
                    "	and (lflg&(1<<31))=0" +
                    "	and sales_km>=" +
                    "			(select min(sales_km)" +
                    "			from t_lines" +
                    "			where line_id=?1" +
                    "			and (station_id=?2 or" +
                    "				 station_id=?3))" +
                    "	and sales_km<=" +
                    "			(select max(sales_km)" +
                    "			from t_lines" +
                    "			where line_id=?1" +
                    "			and (station_id=?2 or " +
                    "				 station_id=?3))"

            val dbo = RouteDB.db().rawQuery(tsql, arrayOf(lineId.toString(), b_stationId.toString(), e_stationId.toString(), stationId.toString()))
            var rc = 0
            try {
                if (dbo.moveToNext()) {
                    rc = dbo.getInt(0)
                }
            } finally {
                dbo.close()
            }
            return rc
        }

        internal fun ASSERT(condition: Boolean) {
            if (!condition) {
                throw AssertionError("ASSERT Error occured!!!!!!")
            }
        }

        internal fun ASSERT(condition: Boolean, message: String) {
            if (!condition) {
                throw AssertionError("RouteUtil.ASSERT !!!!!!" + message)
            }
        }
    }
}//static
//	並行在来線を得る
//
//	@param [in] line_id     路線(新幹線)
//	@param [in] station_id	駅(並行在来線駅(新幹線接続駅)
//	@param [in] (optional)station_id2 至駅(方向)
//	@retval not 0 並行在来線
//	@retval 0xffff 並行在来線は2つあり、その境界駅である(上越新幹線 高崎)
//
//
