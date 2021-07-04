package org.sutezo.alps;

//package Route;

import java.nio.charset.StandardCharsets;
import java.util.*;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.database.sqlite.SQLiteOpenHelper;

import static org.sutezo.alps.farertAssert.ASSERT;


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


public class RouteUtil {

    final static boolean C114NOFASTJUNCCHEK = false;
    final static String TITLE_NOTSAMEKOKURAHAKATASHINZAI = "(小倉博多間新幹線在来線別線)";

    static int g_tax;	/* in alps_mfc.cpp(Windows) or main.m(iOS) or main.cpp(unix) */

    // (System->User)

    final static int ADDRC_LAST = 0;   // add() return code
    final static int ADDRC_OK = 1;
    final static int ADDRC_END = 5;
    final static int ADDRC_CEND	= 4;
    // ADDRC_NG -1 to -N

    // length define(UTF-8)
    final static int MAX_STATION_CHR = 64;             // 38

    final static int MAX_JCT = 340;		// 319

    final static int MAX_COMPNPASSSET = 25;       // 会社線 限定的 通過連絡運輸での 有効路線数の最大 */

    final static int CSTART = 1;
    final static int CEND = 2;

    // DB 固定定義(DB変更のない前提のDB値定義)
//	速度考慮しSQLで取得しない
//
    static final int JR_HOKKAIDO	= 1;
    static final int JR_EAST	= 2;
    static final int JR_CENTRAL	= 3;
    static final int JR_WEST	= 4;
    static final int JR_KYUSYU	= 5;
    static final int JR_SHIKOKU	= 6;
    static final int JR_GROUP_MASK  = ((1<<5)|(1<<4)|(1<<3)|(1<<2)|(1<<1)|(1<<0));
    static final boolean IS_JR_MAJOR_COMPANY(int c)	{
        return ((JR_EAST == c) || (JR_CENTRAL == c) || (JR_WEST == c));
    }

    static final int CITYNO_TOKYO = 1;			// 東京都区内[区]
    static final int CITYNO_YOKOHAMA = 2;		// 横浜市内[浜]
    static final int CITYNO_NAGOYA = 3;		// 名古屋市内[名]
    static final int CITYNO_KYOUTO = 4;		// 京都市内[京]
    static final int CITYNO_OOSAKA = 5;		// 大阪市内[阪]
    static final int CITYNO_KOUBE = 6;			// 神戸市内[神]
    static final int CITYNO_HIROSIMA = 7;		// 広島市内[広]
    static final int CITYNO_KITAKYUSYU = 8;	// 北九州市内[九]
    static final int CITYNO_FUKUOKA = 9;		// 福岡市内[福]
    static final int CITYNO_SENDAI = 10;		// 仙台市内[仙]
    static final int CITYNO_SAPPORO = 11;		// 札幌市内[札]
    static final int CITYNO_YAMATE = 12;      	// 山手線内[山]
    static final int CITYNO_SHINOOSAKA	= 13;   // 大阪・新大阪

    static final int STATION_ID_AS_CITYNO = 10000;  // station_id or cityno

    static final int URB_TOKYO = 1;
    static final int URB_NIGATA = 2;
    static final int URB_OSAKA = 3;
    static final int URB_FUKUOKA = 4;
    static final int URB_SENDAI = 5;

    static final int FLAG_FARECALC_INITIAL	= (1<<15);
    public static int MASK_CITYNO(int flg) {
        return ((flg) & 0x0f);
    }
    /* 近郊区間 */
    final static int MASK_URBAN = 0x380;
    static int URBAN_ID(int flg) {
        return (((int)(flg)>>>7)&7);
    }
    static boolean IS_OSMSP(int flg) {
        return (((flg)&(1 << 11))!=0);	/* 大阪電車特定区間 ?*/
    }
    static boolean IS_TKMSP(int flg) {
        return (((flg)&(1 << 10))!=0);	/* 東京電車特定区間 ?*/
    }
    static boolean IS_YAMATE(int flg) {
        return (((flg)&(1 << 5))!=0);	/* 山点線内／大阪環状線内 ?*/
    }

    static final int MASK_FARECALC_INITIAL = 0;

    final static int BCBULURB = 13;     // FARE_INFO.flag: ONの場合大都市近郊区間特例無効(新幹線乗車している)

/*
  RouteItem[] 先頭のlineId
  0x01 発駅が都区市内
  0x02 着駅が都区市内
*/

    final static int MASK_ROUTE_FLAG_LFLG = 0xff000000;
    final static int MASK_ROUTE_FLAG_SFLG = 0x0000ffff;


    // 1レコードめのlineIdの定義
// bit0-9
    final static int B1LID_MARK = 0x8000;		// line_idとの区別で負数になるように
    final static int B1LID_BEGIN_CITY = 0;	// [区][浜][名][京][阪][神][広][九][福][仙][札]
    final static int B1LID_FIN_CITY = 1;
    final static int B1LID_BEGIN_YAMATE = 2;	// [山]
    final static int B1LID_FIN_YAMATE = 3;
    final static int B1LID_BEGIN_2CITY = 4;	// not used
    final static int B1LID_FIN_2CITY = 5;	// not used
    final static int B1LID_BEGIN_CITY_OFF = 6;
    final static int B1LID_FIN_CITY_OFF = 7;
    final static int B1LID_BEGIN_OOSAKA = 8;	// 大阪・新大阪
    final static int B1LID_FIN_OOSAKA = 9;
    final static int M1LID_CITY = 0x3ff;
// bit10-14 reserve


    final static int BCRULE70 = 6;		/* DB:lflag */

    final static int FLG_HIDE_LINE	= (1<<19);
    final static int FLG_HIDE_STATION	= (1<<18);
    final static boolean IS_FLG_HIDE_LINE(int lflg)	{
        return (0!=(lflg&FLG_HIDE_LINE));		// 路線非表示
    }
    static boolean IS_FLG_HIDE_STATION(int lflg) {
        return (0!=(lflg&FLG_HIDE_STATION));	// 駅非表示
    }



    /* util */
    static int MASK(int bdef) { return (1 << bdef); }
    static boolean BIT_CHK(int flg, int bdef) {
        return (0 != (flg & MASK(bdef)));
    }
    static boolean BIT_CHK2(int flg, int bdef1, int bdef2) {
        return (0 != (flg & (MASK(bdef1)|MASK(bdef2))));
    }
    static boolean BIT_CHK3(int flg, int bdef1, int bdef2, int bdef3) {
        return (0 != (flg & (MASK(bdef1)|MASK(bdef2)|MASK(bdef3))));
    }
    static int BIT_ON(int flg, int bdef) {
        return flg | MASK(bdef);
    }
    static int BIT_OFF(int flg, int bdef) {
        return flg & ~MASK(bdef);
    }
    static int BIT_OFFN(int flag, int pattern) {
        return flag & ~(pattern);
    }

    static final int HWORD_BIT = 16;		/* Number of bit in half word(unsigned short) */

    /* ---------------------------------------!!!!!!!!!!!!!!! */
    public static boolean IS_SHINKANSEN_LINE(int id) {
        return ((0x1000 < (id)) && ((id) < 0x2000));
    }
    public static boolean IS_COMPANY_LINE(int id) {
        return ((0x2000 < (id)) && ((id) < 0x3000));	/* 会社線id */
    }
    public static boolean IS_BRT_LINE(int id) {
        return ((0x4000 < (id)) && ((id) < 0x5000));	/* BRT id */
    }
    public static int BRTMASK(int id) {
        return (~0x4000 & id);
    }
    //#define MAX_JCT 325
    /* ---------------------------------------!!!!!!!!!!!!!!! */

    // 駅は分岐駅か
    public static boolean STATION_IS_JUNCTION(int sid) {
        return (0 != (AttrOfStationId(sid) & (1<<12)));
    }
//#define STATION_IS_JUNCTION_F(flg)	(0 != (flg & (1<<12)))
// sflg.12は特例乗り換え駅もONなのでlflg.15にした

    static boolean STATION_IS_JUNCTION_F(int flg) {
        return (0 != (flg & (1<<15)));
    }

    // iOS版であって使ってた奴。もーわすれたよ(2018.8.23

    public static boolean IsSpecificJunction(int lineId, int stationId) {
        return 0 != (AttrOfStationOnLineLine(lineId, stationId) & (1 << 31));
    }

        /* ID for line_id on t_lines */
    final static short ID_L_RULE70 = -10;

    static int Limit(int v, int l, int h) {
        return (Math.min(Math.max(v, l), h));
    }


    /* 消費税(四捨五入)加算 */
    static int taxadd(int fare, int tax) {
        return (fare + ((fare * 1000 * tax / 100000) + 5) / 10 * 10);
    }

    static int taxadd_ic(int fare, int tax) {
        return (fare + (fare * 1000 * tax / 100000));
    }

    /* round up on 5 */
    static int round(int d) {
        return (((d) + 5) / 10 * 10);	/* 10円未満四捨五入 */
    }
    static int round_up(int d) {
        return (((d) + 9) / 10 * 10);	/* 10円未満切り上げ */
    }

    static int round_down(int d) {
        return ((d) / 10 * 10);			/* 10円未満切り捨て */
    }


    //////////////////////////////////////////////////////
    // 	文字列は「漢字」か「かな」か？
    //	「かな」ならTrueを返す
    //
    // 	@param [in] szStr    文字列
    // 	@retval true  文字列はひらがなまたはカタカナのみである
    // 	@retval false 文字列はひらがな、カタカナ以外の文字が含まれている
    //
    static boolean isKanaString(String szStr) {

        if (szStr.matches("^[\\u3040-\\u309F\\u30a0-\\u30ff]+$")) {
            return true;
        } else {
            return false;
        }
    }

    //	カナをかなに
    //
    //	@param [in][out] kana_str  変換文字列
    //
    static String conv_to_kana2hira(String kana_str) {
        StringBuffer sb = new StringBuffer(kana_str);
        for (int i = 0; i < sb.length(); i++) {
            char c = sb.charAt(i);
            if (c >= 'ァ' && c <= 'ン') {
                sb.setCharAt(i, (char)(c - 'ァ' + 'ぁ'));
            } else if (c == 'ヵ') {
                sb.setCharAt(i, 'か');
            } else if (c == 'ヶ') {
                sb.setCharAt(i, 'け');
            } else if (c == 'ヴ') {
                sb.setCharAt(i, 'う');
                sb.insert(i + 1, '゛');
                i++;
            }
        }
        return sb.toString();
    }

    //	3桁毎にカンマを付加した数値文字列を作成
    //	(1/10した小数点付き営業キロ表示用)
    //
    //	@param [in] num  数値
    //	@return 変換された文字列(ex. 61000 -> "6,100.0", 25793 -> "2,579.3")
    //
    static String num_str_km(int num) {
        return String.format(Locale.JAPANESE, "%,d.%d", num / 10, num % 10);
    }

    //	3桁毎にカンマを付加した数値文字列を作成
    //	金額表示用
    //
    //	@param [in] num  数値
    //	@return 変換された文字列(ex. 61000 -> "\61,000", 3982003 -> "3,982,003")
    //
    static String num_str_yen(int num) {
        return String.format(Locale.JAPANESE, "%,d", num);
    }


     // static
     //	会社&都道府県の列挙
     //
     //	@return DBクエリ結果オブジェクト(会社(JRのみ)、都道府県一覧)
     //
     static Cursor Enum_company_prefect() {
         final String tsql =
                 "select name, rowid from t_company where name like 'JR%'" +
                         " union" +
                         " select name, rowid*65536 from t_prefect order by rowid";
         return RouteDB.db().rawQuery(tsql, null);	// , false);
     }

     // static
     //	駅名のパターンマッチの列挙(ひらがな、カタカナ、ひらがなカタカナ混じり、漢字）
     //  ひらがな昇順ソートで返す / 会社線駅も含む
     //
     //	@param [in] menu_station_select   えきめい
     //	@return DBクエリ結果オブジェクト(駅名)
     //
     static Cursor Enum_station_match(String station) {
         String sql;
         //	final char tsql[] = "/**/select name, rowid, samename from t_station where (sflg&(1<<18))=0 and %s like '%q%%'";
         final String tsql = "/**/select name, rowid, samename from t_station where %s like '%s%%'";
         final String tsql_s = " and samename='%s'";
         final String tsql_e = " order by kana";

         String sameName;
         String stationName = new String(station);		// WIN32 str to C++ string

         if (MAX_STATION_CHR < stationName.length()) {
             stationName = stationName.substring(0, MAX_STATION_CHR);
         }

         int pos = stationName.indexOf('(');
         if (0 <= pos) {
             sameName = stationName.substring(pos);
             stationName = stationName.substring(0, pos);
 	/* #ifdef _WINDOWS
 			CT2A qsName(stationName, CP_UTF8);
 			CT2A qsSame(sameName, CP_UTF8);

 			sqlite3_snprintf(sizeof(sql), sql, tsql,
 									"name", qsName);
 			sqlite3_snprintf(sizeof(sql) - lstrlenA(sql), sql + lstrlenA(sql), tsql_s,
 									qsSame);
 	#else */
             sql = String.format(Locale.JAPANESE, tsql, "name", stationName) + String.format(Locale.JAPANESE, tsql_s, sameName);
 	/* #endif */
         } else {
             boolean bKana;
             if (isKanaString(stationName)) {
                 stationName = conv_to_kana2hira(stationName);
                 bKana = true;
             } else {
                 bKana = false;
             }
 	/* #ifdef _WINDOWS
 			CT2A qsName(stationName, CP_UTF8);	// C++ string to UTF-8
 			sqlite3_snprintf(sizeof(sql), sql, tsql,
 									bKana ? "kana" : "name", qsName);
 	#else */
             sql = String.format(Locale.JAPANESE, tsql, bKana ? "kana" : "name", stationName);
 	/* #endif */
         }
         return RouteDB.db().rawQuery(sql + tsql_e, null);	//, false);
     }

     // static
     //	都道府県or会社に属する路線の列挙(JRのみ)
     //
     //	@param [in] id   0x10000 <= 都道府県番号 << 16
     //				     < 0x10000  会社ID
     //	@return DBクエリ結果オブジェクト(路線)
     //
     static Cursor Enum_lines_from_company_prefect(int id) {
         String sql;		// [300];
         final String tsql =
                 "select n.name, line_id, lflg from t_line n" +
                         " left join t_lines l on n.rowid=l.line_id" +
                         " left join t_station t on t.rowid=l.station_id" +
                         " where %s=%d" +
                         " and (l.lflg&((1<<18)|(1<<31)|(1<<17)))=0" +
                         " group by l.line_id order by n.kana";

         int ident;

         if (0x10000 <= id) {
             ident = (0xffff & (id >>> 16));
         } else {
             ident = id;
         }
         sql = String.format(Locale.JAPANESE, tsql,
                 (0x10000 <= id) ? "prefect_id" : "company_id", ident);

         return RouteDB.db().rawQuery(sql, null);	// , false);
     }

     // static
     //	会社 or 都道府県 + 路線の駅の列挙
     //
     //	@param [in] prefectOrCompanyId  都道府県 or 会社ID×0x10000
     //	@param [in] lineId              路線
     //	@return DBクエリ結果オブジェクト(都道府県、会社)
     //
     static Cursor Enum_station_located_in_prefect_or_company_and_line(int prefectOrCompanyId, int lineId) {
         final  String tsql =
                 //"select t.name, station_id from t_lines l left join t_station t on t.rowid=l.station_id "
                 //" where line_id=? and %s=? order by sales_km";
                 "/**/select t.name, station_id from t_station t left join t_lines l on t.rowid=l.station_id" +
                         " where line_id=? and %s=? and (l.lflg&((1<<18)|(1<<31)|(1<<17)))=0 order by sales_km";
         // 18:Company, 17:virtual junction for Shinkansen

         int ident;

         if (0x10000 <= prefectOrCompanyId) {
             ident = (0xffff & (prefectOrCompanyId >>> 16));
         } else {
             ident = prefectOrCompanyId;
         }

         String sql = String.format(Locale.JAPANESE, tsql,
                 (0x10000 <= prefectOrCompanyId) ? "prefect_id" : "company_id");

         Cursor dbo = RouteDB.db().rawQuery(sql, new String[] {String.valueOf(lineId),
                 String.valueOf(ident)});

         return dbo;
     }

     // static
     //	駅名のよみを得る
     //
     //	@param [in] stationId   駅ident
     //	@return えきめい
     //
     public static String  GetKanaFromStationId(int stationId) {
         final String tsql =
                 "select kana from t_station where rowid=?";

         Cursor dbo = RouteDB.db().rawQuery(tsql, new String[] {String.valueOf(stationId)});
         String s = "";
         try {
             if (dbo.moveToNext()) {
                 s = dbo.getString(0);
             } else {
                 /* FAILTHRUE */
             }
         } finally {
             dbo.close();
         }
         return s;
     }


     // static
     //	駅の所属路線のイテレータを返す
     //
     //	@param [in] stationId   駅ident
     //	@return DBクエリ結果オブジェクト(路線)
     //	@return field0(text):路線名, field1(int):路線id, field2(int):lflg(bit31のみ)
     //
     public static Cursor Enum_line_of_stationId(int stationId) {
         final  String tsql =
                 "select n.name, line_id, lflg from t_line n left join t_lines l on n.rowid=l.line_id" +
                         //" where station_id=? and (lflg&((1<<31)|(1<<17)))=0 and sales_km>=0";
                         " where station_id=? and (lflg&(1<<17))=0 order by n.kana";

         Cursor dbo = RouteDB.db().rawQuery(tsql, new String[] {String.valueOf(stationId)});

         return dbo;
     }


     //static
     //	路線の分岐駅一覧イテレータを返す
     //	@param [in] lineId	路線Id
     //	@param [in] stationId 着駅Id(この駅も一覧に含める)
     //	@return DBクエリ結果オブジェクト(分岐駅)
     //
     public static Cursor Enum_junction_of_lineId(int lineId, int stationId) {
         final  String tsql =
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
                         " order by l.sales_km";
         //--#endif
         //--#endif

         Cursor dbo = RouteDB.db().rawQuery(tsql, new String[] {String.valueOf(lineId),
                 String.valueOf(stationId)});
         return dbo;
     }

     //static
     //	路線内の駅一覧イテレータを返す
     //
     //	@param [in] lineId   駅ident
     //	@return DBクエリ結果オブジェクト(駅)
     //
     public static Cursor Enum_station_of_lineId(int lineId) {
         final  String tsql =
                 "select t.name, station_id, lflg&(1<<15)" +
                         " from t_lines l left join t_station t on t.rowid=l.station_id" +
                         " where line_id=?" +
                         " and (lflg&((1<<31)|(1<<17)))=0" +
                         " order by l.sales_km";

         Cursor dbo = RouteDB.db().rawQuery(tsql, new String[] {String.valueOf(lineId)});
         return dbo;
     }

     // 駅名より駅IDを返す(私鉄含)
     //
     // station_id = f("駅名")
     //
     public static int GetStationId(String station)	{
         final String tsql = "select rowid from t_station where (sflg&(1<<18))=0 and name=?1 and samename=?2";

         String sameName;
         String stationName = new String(station);

         int pos = stationName.indexOf('(');
         if (0 <= pos) {
             sameName = stationName.substring(pos);
             stationName = stationName.substring(0, pos);
         } else {
             sameName = "";
         }

         Cursor dbo = RouteDB.db().rawQuery(tsql, new String[] {stationName, sameName});
         int rc = 0;
         try {
             if (dbo.moveToNext()) {
                 rc = dbo.getInt(0);
             }
         } finally {
             dbo.close();
         }
         return rc;
     }

     // 線名より線IDを返す
     //
     // station_id = f("線名")
     //
     public static int GetLineId(String lineName) {
         final String tsql = "select rowid from t_line where name=?";

         Cursor dbo = RouteDB.db().rawQuery(tsql, new String[] {lineName});
         int rc = 0;
         try {
             if (dbo.moveToNext()) {
                 rc = dbo.getInt(0);
             }
         } finally {
             dbo.close();
         }
         return rc;
     }

     //static
     //	駅ID→駅名
     //
     public static String StationName(int id)	{
         String name = "";		//[MAX_STATION_CHR];

         Cursor ctx = RouteDB.db().rawQuery(
                 "select name from t_station where rowid=?", new String[] {String.valueOf(id)});
         try {
             if (ctx.moveToNext()) {
                 name = ctx.getString(0);
             }
         } finally {
             ctx.close();
         }
         return name;
     }

     //static
     //	駅ID→駅名(同名駅区別)
     //
     public static String StationNameEx(int id) {
         String name = "";	//[MAX_STATION_CHR];

         Cursor ctx = RouteDB.db().rawQuery(
                 "select name,samename from t_station where rowid=?",
                 new String[] {String.valueOf(id)});
         try {
             if (ctx.moveToNext()) {
                 name = ctx.getString(0);
                 name += ctx.getString(1);
             }
         } finally {
             ctx.close();
         }
         return name;
     }

     //static
     //	路線ID→路線名
     //
     public static String LineName(int id) {
         Cursor ctx = RouteDB.db().rawQuery(
                 "select name from t_line where rowid=?", new String[] {String.valueOf(id)});
         String name = "";   // if error
         try {
             if (ctx.moveToNext()) {
                 name = ctx.getString(0);
             }
         } finally {
             ctx.close();
         }
         return name;
     }

     //static
     //	都道府県名
     //
     public static String PrefectName(int id) {
         String name = "";	//[MAX_PREFECT_CHR];

         if (0x10000 <= id) {
             id = id >>> 16;
         }

         Cursor ctx = RouteDB.db().rawQuery("select name from t_prefect where rowid=?",
                 new String[] {String.valueOf(id)});
         try {
             if (ctx.moveToNext()) {
                 name = ctx.getString(0);
             }
         } finally {
             ctx.close();
         }
         return name;
     }

     //static
     //	会社線名
     //
     public static String CompanyName(int id) {
         String name = "";	//[MAX_PREFECT_CHR];

         Cursor ctx = RouteDB.db().rawQuery("select name from t_company where rowid=?",
                 new String[] {String.valueOf(id)});
         try {
             if (ctx.moveToNext()) {
                 name = ctx.getString(0);
             }
         } finally {
             ctx.close();
         }
         return name;
     }

     //static
     //	駅ID→駅名
     //
     public static String CoreAreaCenterName(int id) {
         String name = "";		// [MAX_COREAREA_CHR];

         if (STATION_ID_AS_CITYNO <= id) {
             id -= STATION_ID_AS_CITYNO;
         }
         Cursor ctx = RouteDB.db().rawQuery(
                 "select name from t_coreareac where rowid=?", new String[] {String.valueOf(id)});
         try {
             if (ctx.moveToNext()) {
                 name = ctx.getString(0);
             }
         } finally {
             ctx.close();
         }
         return name;
     }

     //static
     //	駅の属性を得る
     //
     static int AttrOfStationId(int id) {
         Cursor ctx = RouteDB.db().rawQuery(
                 "select sflg from t_station where rowid=?", new String[] {String.valueOf(id)});
         int rc = (1 << 30);
         try {
             if (ctx.moveToNext()) {
                 rc = ctx.getInt(0);
             }
         } finally {
             ctx.close();
         }
         return rc;
     }

     //static
     //	路線駅の属性を得る
     //
     public static int AttrOfStationOnLineLine(int line_id, int station_id) {
         Cursor ctx = RouteDB.db().rawQuery(
                 //  "select lflg, sflg from t_lines where line_id=?1 and station_id=?2", true);
                 "select sflg, lflg from t_station t left join t_lines on t.rowid=station_id where line_id=?1 and station_id=?2", new String[] {String.valueOf(line_id), String.valueOf(station_id)});

         int rc = (1 << 30);
         try {
             if (ctx.moveToNext()) {
                 long s;
                 long l;

                 s = 0x00007fff & ctx.getLong(0);
                 l = 0xffff8000 & ctx.getLong(1);	// b15はSTATION_IS_JUNCTION_F(lflg)で必要なので注意
                 rc = (int)(s | l);
             }
         } finally {
             ctx.close();
         }
         return rc;
     }

     // static
     //	駅(Id)の都道府県を得る
     //
     //	@param [in] stationId   駅ident
     //	@return 都道府県名
     //
     public static String GetPrefectByStationId(int stationId) {
         final String tsql =
                 //"select p.name from t_prefect p left join t_station t on t.prefect_id=p.rowid where t.rowid=?";
                 "select name from t_prefect where rowid=(select prefect_id from t_station where rowid=?)";
         //ここではどーでもいいけど、sqliteは結合遅くてサブクエリの方が早かったのでこうしてみた
         Cursor dbo = RouteDB.db().rawQuery(tsql, new String[] {String.valueOf(stationId)});
         String s = "";
         try {
             if (dbo.moveToNext()) {
                 s = dbo.getString(0);
             }
         } finally {
             dbo.close();
         }
         return s;
     }

     // static version
     //	@brief 経由文字列を返す
     //
     //	@param [in] routeList    route
     //	@param [in] last_flag    route flag(LF_OSAKAKAN_MASK:大阪環状線関連フラグのみ).
     //	@retval 文字列
     //
     public static String Show_route(final RouteItem[] routeList, final RouteFlag routeFlag) {
         String lineName;
         String stationName;
         RouteFlag _route_flag = routeFlag;
         //TCHAR buf[MAX_BUF];
         String result_str;
         int station_id1;

         if (routeList.length == 0) {	/* 経路なし(AutoRoute) */
             return "";
         }

         result_str = "";
         station_id1 = routeList[0].stationId;
         _route_flag.setOsakaKanFlag(RouteFlag.OSAKAKAN_PASS.OSAKAKAN_NOPASS);	// BIT_OFF(last_flag, osakakan_1pass)

         for (int pos = 1; pos < routeList.length ; pos++) {

             lineName = LineName(routeList[pos].lineId);

             if (pos != (routeList.length - 1)) {
 				/* 中間駅 */
                 if (!IS_FLG_HIDE_LINE(routeList[pos].flag)) {
                     if (ID_L_RULE70 != routeList[pos].lineId) {
                         result_str += "[";
                         result_str += lineName;
                         if (DbIdOf.INSTANCE.line("大阪環状線") == routeList[pos].lineId) {
                             result_str += RouteOsakaKanDir(station_id1, routeList[pos].stationId, _route_flag);
                             _route_flag.setOsakaKanPass(true);
                         }
                         result_str += "]";
                     } else {
                         result_str += ",";
                     }
                 }
                 station_id1 = routeList[pos].stationId;
                 if (!IS_FLG_HIDE_STATION(routeList[pos].flag)) {
                     stationName = StationName(station_id1);
                     result_str += stationName;
                 }
             } else {
 				/* 着駅 */
                 if (!IS_FLG_HIDE_LINE(routeList[pos].flag)) {
                     result_str += "[";
                     result_str += lineName;
                     if (DbIdOf.INSTANCE.line("大阪環状線") == routeList[pos].lineId) {
                         result_str += RouteOsakaKanDir(station_id1, routeList[pos].stationId, _route_flag);
                         _route_flag.setOsakaKanPass(true);
                     }
                     result_str += "]";
                 }
                 //result_str += stationName;	// 着駅
             }
             //result_str += buf;
         }
         return result_str;
     }

    // static version
    //	@brief 完全な経由文字列を返す
    //	(for Debug only use)
    //	@param [in] routeList    route
    //	@param [in] route_flag    route flag(LF_OSAKAKAN_MASK:大阪環状線関連フラグのみ).
    //	@retval 文字列
    //
    public String Show_route_full(final RouteItem[] routeList, final RouteFlag routeFlag)
    {
        if (routeList.length == 0) {	/* 経路なし(AutoRoute) */
            return "";
        }
        String startStationName = RouteUtil.StationName(routeList[0].stationId);
        String route_str = RouteUtil.Show_route(routeList, routeFlag);
        String arriveStationName = RouteUtil.StationName(routeList[routeList.length - 1].stationId);
        return startStationName + route_str + arriveStationName;
    }

    //static private
     //	@brief 大阪環状線 方向文字列を返すで
     //
     //	@param [in] station_id1  発駅
     //	@param [in] station_id2  着駅
     //	@param [in] routeFlag    route flag.
     //	@retval 文字列
     //
     private static String  RouteOsakaKanDir(int station_id1, int station_id2, final RouteFlag routeFlag) {
         String result_str;
         final String result[] = {
                 "",
                 "(内回り)",
                 "(外回り)",
         };
         byte inner_outer[] = {
                 0, 2, 2, 2, 1, 1, 0, 1, 1, 1, 1, 1, 2, 2, 0, 2, 0,
                 //     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
         };
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
         int  pass;
         int  c;

         c = DirOsakaKanLine(station_id1, station_id2) == 0 ? 1 : 0;
         c |= routeFlag.osakakan_detour ? 0x02 : 0;
         if (routeFlag.getOsakaKanPass()) {
             c |= routeFlag.osakakan_2dir ? 0x04 : 0;
             pass = 2;
         } else {
             c |= routeFlag.osakakan_1dir ? 0x04 : 0;
             pass = 1;
         }
         c |= LINE_DIR.LDIR_ASC == DirLine(DbIdOf.INSTANCE.line("大阪環状線"), station_id1, station_id2) ? 0x08 : 0;
         System.out.printf("RouteOsakaKanDir:[%d] %s %s %s: %d %d %d %d\n",
                     pass,
                     StationName(station_id1),
                     ((0 != (c & 0x02)) && (pass != 2)) ? ">>" : ">",
                     StationName(station_id2), 0x1 & c, 0x1 & (c >>> 1), 0x01 & (c >>> 2), 0x01 & (c >>> 3));

         if (inner_outer.length <= c) {
             c = inner_outer.length - 1;
         }
         return result[inner_outer[c]];
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
     public static int DirOsakaKanLine(int station_id_a, int station_id_b) {
         if (GetDistance(DbIdOf.INSTANCE.line("大阪環状線"), station_id_a, station_id_b).get(0).intValue() <=
                 GetDistanceOfOsakaKanjyouRvrs(DbIdOf.INSTANCE.line("大阪環状線"), station_id_a, station_id_b)) {
             return 0;
         } else {
             return 1;
         }
     }

     //static
     //	駅の所属会社IDを得る
     //	(境界駅はあいまい)
     //
     //	@param [in] station_id   駅id
     // @return int[0]: 会社1 / int[1]: 会社2
     //
     static int[]  CompanyIdFromStation(int station_id) {
         int results[] = {0, 0};
         Cursor ctx = RouteDB.db().rawQuery(
                 "select company_id, sub_company_id from t_station where rowid=?",
                 new String[] {String.valueOf(station_id)});

         try {
             if (ctx.moveToNext()) {
                 results[0] = ctx.getInt(0);
                 results[1] = ctx.getInt(1);
             }
         } finally {
             ctx.close();
         }
         return results;
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
     static List<Integer> GetDistance(int line_id, int station_id1, int station_id2) {
         final String tsql =
                 "select max(sales_km)-min(sales_km), case max(calc_km)-min(calc_km) when 0 then max(sales_km)-min(sales_km) else max(calc_km)-min(calc_km) end" +
                         " from t_lines" +
                         " where line_id=?1 and (lflg&(1<<31))=0 and (station_id=?2 or station_id=?3)";

         /***	// 140416
          "select max(sales_km)-min(sales_km), case max(calc_km)-min(calc_km) when 0 then max(sales_km)-min(sales_km) else max(calc_km)-min(calc_km) end"
          " from t_lines"
          " where line_id=?1"
          " and	(lflg&(1<<31))=0"
          " and	sales_km>=(select min(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))"
          " and	sales_km<=(select max(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))";

          ///old #if 0
          "select l1.sales_km - l2.sales_km, l1.calc_km - l2.calc_km"
          " from t_lines l1"
          " left join t_lines l2"
          " where l1.line_id=?1"
          " and l2.line_id=?1"
          " and l1.sales_km>l2.sales_km"
          " and ((l1.station_id=?2 and l2.station_id=?3)"
          " or (l1.station_id=?3 and l2.station_id=?2))";
          ****/
         List<Integer> v = new ArrayList<Integer>(2);

         Cursor dbo = RouteDB.db().rawQuery(tsql, new String[] {String.valueOf(line_id),
                 String.valueOf(station_id1),
                 String.valueOf(station_id2)});
         try {
             if (dbo.moveToNext()) {
                 v.add(dbo.getInt(0));
                 v.add(dbo.getInt(1));
             }
         } finally {
             dbo.close();
         }
         return v;
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
     static List<Integer> GetDistance(final RouteFlag oskkflg, int line_id, int station_id1, int station_id2) {
         List<Integer> d = new ArrayList<Integer>(2);
         int sales_km;

         if (line_id != DbIdOf.INSTANCE.line("大阪環状線"))  {
             ASSERT (false);
             return GetDistance(line_id, station_id1, station_id2);
         }

         // pass 0 1 0 1 0 1 0 1
         // 1dir 0 0 1 1 0 0 1 1
         // 2dir 0 0 0 0 1 1 1 1
         //      0 0 1 0 0 1 1 1
         // ~pass & 1dir | pass & 2dir

         if ((!oskkflg.getOsakaKanPass() && oskkflg.osakakan_1dir) ||
             (oskkflg.getOsakaKanPass() && oskkflg.osakakan_2dir)) {
             sales_km = GetDistanceOfOsakaKanjyouRvrs(line_id, station_id1, station_id2);
             System.out.printf("Osaka-kan reverse\n");
         } else {
             sales_km = GetDistance(line_id, station_id1, station_id2).get(0);
             System.out.printf("Osaka-kan forward\n");
         }

         d.add(sales_km);
         d.add(sales_km);

         return d;
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
     static int GetDistanceOfOsakaKanjyouRvrs(int line_id, int station_id1, int station_id2) {
         // 新今宮までの距離(近いほう) + 天王寺までの距離(近いほう) + 新今宮～天王寺

         final String tsql =
                 "select" +
                         " (select max(sales_km)-min(sales_km) from t_lines where line_id=?1 and " +
                         "  ((station_id=(select rowid from t_station where name='新今宮')) or" +
                         "   (sales_km=(select max(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3)))))+" +
                         " (select min(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))+" +
                         " (select max(sales_km)-min(sales_km) from t_lines where line_id=" +
                         "  (select rowid from t_line where name='関西線') and station_id in (select rowid from t_station where name='新今宮' or name='天王寺'))";
         Cursor dbo = RouteDB.db().rawQuery(tsql, new String[] {String.valueOf(line_id),
                 String.valueOf(station_id1),
                 String.valueOf(station_id2)});

         int km = 0;

         try {
             if (dbo.moveToNext()) {
                 km = dbo.getInt(0);
                 ASSERT (0 < km);
                 return km;
             }
         } finally {
             dbo.close();
         }
         ASSERT (false);
         return km;
     }

     enum LINE_DIR {
         LDIR_NONE,
         LDIR_ASC,		// 下り
         LDIR_DESC,		// 上り
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
     static LINE_DIR DirLine(int line_id, int station_id1, int station_id2) {
         final String tsql =
                 "select case when" +
                         "   ((select sales_km from t_lines where line_id=?1 and station_id=?2) - " +
                         "    (select sales_km from t_lines where line_id=?1 and station_id=?3)) <= 0" +
                         " then 1 else 2 end";

         Cursor dbo = RouteDB.db().rawQuery(tsql, new String[] {String.valueOf(line_id),
                 String.valueOf(station_id1),
                 String.valueOf(station_id2)});

         LINE_DIR rc = LINE_DIR.LDIR_ASC;    /* default is 下り */
         try {
             if (dbo.moveToNext()) {
                 rc = (1 == dbo.getInt(0)) ? LINE_DIR.LDIR_ASC : LINE_DIR.LDIR_DESC;
             }
         } finally {
             dbo.close();
         }
         return rc;
     }

     //static
     //	並行在来線を得る
     //
     //	@param [in] line_id     路線(新幹線)
     //	@param [in] station_id	駅(並行在来線駅(新幹線接続駅)
     //	@param [in] (optional)station_id2 至駅(方向)
     //	@retval not 0 並行在来線
     //	@retval 0xffff 並行在来線は2つあり、その境界駅である(上越新幹線 高崎)
     //
     //
     static int GetHZLine(int line_id, int station_id) {
         return GetHZLine(line_id, station_id, -1);
     }
     static int GetHZLine(int line_id, int station_id, int station_id2 /* =-1 */) {
         int i;
         int w;
         List<Integer> hzl = EnumHZLine(line_id, station_id, station_id2);

         if (hzl.size() < 3) {
             //ASSERT (false);
             return 0;
         }
         for (i = 0; i < (int)hzl.size(); i++) {
             if (0x10000 < hzl.get(i)) {
                 w = 0xffff & hzl.get(i);
             } else {
                 w = hzl.get(i);
             }
             if (0 != w) {
                 return w;
             }
 			/* 着駅までは関知しない */
         }
         return 0;

         // 山陽新幹線 新大阪 姫路 は東海道線と山陽線だが東海道線を返す
         // 山陽新幹線 姫路 新大阪なら山陽線を返す
     }

     static List<Integer> EnumHZLine(int line_id, int station_id, int station_id2) {
         // 新幹線-並行在来線取得クエリ
         final String tsql_hzl =
                 "select case when(select line_id from t_hzline where rowid=(" +
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
                         " (select count(*) from t_lines where line_id=?1 and station_id=?3 and 0=(lflg&((1<<31)|(1<<17))))";
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

         List<Integer> rslt = new ArrayList<Integer>();
         int lineId;
         int flg;

         Cursor dbo = RouteDB.db().rawQuery(tsql_hzl, new String[] {String.valueOf(line_id),
                 String.valueOf(station_id),
                 String.valueOf(station_id2)});

         ASSERT (IS_SHINKANSEN_LINE(line_id));

         try {
             while (dbo.moveToNext()) {
                 lineId = dbo.getInt(0);
                 flg = dbo.getInt(1);
                 if ((flg == 1) && (lineId == 0)) {
                     lineId = -1;	/* 新幹線駅だが在来線接続駅でない */
                 } /* else if ((flg == 0) && (lineId == 0)) 不正(新幹線にない駅) */
                 rslt.add(lineId);
             }
         } finally {
             dbo.close();
         }
         return rslt;
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
     static int NextShinkansenTransferTerm(int line_id, int station_id1, int station_id2) {
         final String tsql =
                 "select station_id from t_lines where line_id=?1 and" +
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
                         " end";

         Cursor dbo = RouteDB.db().rawQuery(tsql, new String[]{String.valueOf(line_id),
                 String.valueOf(station_id1),
                 String.valueOf(station_id2)});

         ASSERT (IS_SHINKANSEN_LINE(line_id));
         int stid = 0;
         try {
             if (dbo.moveToNext()) {
                 stid = dbo.getInt(0);
             }
         } finally {
             dbo.close();
         }
         return stid;
     }

     //static
     //	両隣の分岐駅を得る(非分岐駅指定、1つか2つ)
     //	changeNeerest() =>
     //
     //	@param [in] station_id    駅(非分岐駅)
     //	@return int[2] or int[1] 近隣の分岐駅(盲腸線の場合1、以外は2)
     //			IDENT1: station_id, IDENT2: calc_km
     //
     static short[][] GetNeerNode(int station_id) {
         int idx = 0;
         short[][] result = new short [2][2];
         for (int i = 0; i < result.length; i++) {
             for (int j = 0; j < result[0].length; j++) {
                 result[i][j] = 0;
             }
         }
         Cursor dbo = Enum_neer_node(station_id);
         try {
             for (idx = 0; idx < 2; idx++) {
                 if (dbo.moveToNext()) {
                     short stationId = dbo.getShort(0);
                     short cost = dbo.getShort(1);
                     result[idx][0] = stationId;
                     result[idx][1] = cost;
                 }
             }
         } finally {
             dbo.close();
         }
         return result;
     }

     //static
     //	駅の隣の分岐駅を返す
     //	(非分岐駅を指定すると正しい結果にならない)
     //
     //	@param [in] stationId   駅ident
     //	@return DBクエリ結果オブジェクト(隣接分岐駅)
     //
     static Cursor Enum_neer_node(int stationId) {
         final  String tsql =
                 "select 	station_id , abs((" +
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
                         "					  and	x.sales_km<y.sales_km))";
         Cursor dbo = RouteDB.db().rawQuery(tsql, new String[] {String.valueOf(stationId)});

         return dbo;
     }

     //static
     // 駅の隣の分岐駅数を得る
     //	(盲腸線の駅か否かのみを返す)
     //
     //	@param [in] stationId   駅ident
     //	@return 0 to 2 (隣接分岐駅数)
     //
     public static int NumOfNeerNode(int stationId) {
         if (STATION_IS_JUNCTION(stationId)) {
             return 2;	// 2以上あることもあるが嘘つき
         }
         Cursor dbo =  Enum_neer_node(stationId);
         int c;

         c = 0;
         while (dbo.moveToNext()) {
             ++c;
         }
         return c;
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
     static int InStation(int stationId, int lineId, int b_stationId, int e_stationId) {
         final String tsql =
                 "select count(*)" +
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
                         "				 station_id=?3))";

         Cursor dbo = RouteDB.db().rawQuery(tsql, new String[] {String.valueOf(lineId),
                 String.valueOf(b_stationId),
                 String.valueOf(e_stationId),
                 String.valueOf(stationId)});
         int rc = 0;
         try {
             if (dbo.moveToNext()) {
                 rc = dbo.getInt(0);
             }
         } finally {
             dbo.close();
         }
         return rc;
     }


    //static
    //
    // 路線内の駅1〜駅2内に、駅3〜駅4が含まれるか？重なる部分の営業キロ、計算キロを返す
    // ---:駅1〜駅2, ===:駅3-駅4
    // -----
    //   == 　　==を返す
    // --
    //    == 　0を返す
    // ----
    //   ====  ==の距離を返す
    // ----
    // ====    ====の距離を返す
    // @param [in] line_id     路線
    // @param [in] station_id1 検査する駅1
    // @param [in] station_id2 検査する駅2 (sales_kmは駅１<駅2であることつまり下り)
    // @param [in] station_id3 検査する駅3
    // @param [in] station_id4 検査する駅4 (上り下り関係なし)
    //
    //	@retuen [0]:営業キロ, [1]:計算キロ
    //
    static KM  getIntersectOnLine(int line_id, int station_id1, int station_id2, int station_id3, int station_id4) {
        final String tsql =
            "select ifnull(max(sales_km) - min(sales_km), 0), ifnull(max(calc_km) - min(calc_km), 0)" +
        " from t_lines" +
        " where line_id=?1 and sales_km>=(select sales_km from t_lines where line_id=?1 and station_id=?2)" +
        "                  and sales_km<=(select sales_km from t_lines where line_id=?1 and station_id=?3)" +
        "                  and sales_km>=(select min(sales_km)" +
        "                                  from t_lines" +
        "                                  where line_id=?1 and (station_id=?4 or station_id=?5))" +
        "                  and sales_km<=(select max(sales_km)" +
        "                                  from t_lines" +
        "                                  where line_id=?1 and (station_id=?4 or station_id=?5));";
        List<Integer> sales_calc_km = new ArrayList<Integer>();

        int sales_km = 0;
        int calc_km = 0;

        Cursor dbo = RouteDB.db().rawQuery(tsql, new String[] {
                String.valueOf(line_id),
                String.valueOf(station_id1),
                String.valueOf(station_id2),
                String.valueOf(station_id3),
                String.valueOf(station_id4)});
        try {
            if (dbo.moveToNext()) {
                sales_km = dbo.getInt(0);
                calc_km = dbo.getInt(1);
            }
        } finally {
            dbo.close();
        }
        return new KM(sales_km, calc_km);
    }

    //static
    //
    // 路線内の駅1〜駅2内に、駅3〜駅4が完全に含まれるか？
    // @param [in] line_id     路線
    // @param [in] station_id1 検査する駅1
    // @param [in] station_id2 検査する駅2 (sales_kmは駅１<駅2であることつまり下り)
    // @param [in] station_id3 検査する駅3
    // @param [in] station_id4 検査する駅4 (上り下り関係なし)
    //
    //	@retuen [0]:営業キロ, [1]:計算キロ
    //
    static boolean  inlineOnline(int line_id, int station_id1, int station_id2, int station_id3, int station_id4) {
        final String sql =
            "select case when 2=count(*) then 1 else 0 end" +
        " from t_lines" +
        " where line_id=?1 and sales_km>=(select sales_km from t_lines where line_id=?1 and station_id=?2)" +
        "                and sales_km<=(select sales_km from t_lines where line_id=?1 and station_id=?3)" +
        "                and (station_id=?4 or station_id=?5)";

        boolean rc = false;

        Cursor dbo = RouteDB.db().rawQuery(sql, new String[] {
                String.valueOf(line_id),
                String.valueOf(station_id1),
                String.valueOf(station_id2),
                String.valueOf(station_id3),
                String.valueOf(station_id4)});
        try {
            if (dbo.moveToNext()) {
                int res = dbo.getInt(0);
                if (res == 1) {
                    rc = true;
                }
            }
        } finally {
            dbo.close();
        }
        return rc;
    }
}
