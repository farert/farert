package org.sutezo.alps;

//package Route;

import java.util.*;
import android.database.Cursor;

// import org.sutezo.farert.BuildConfig; // Removed - BuildConfig not available

import static org.sutezo.alps.RouteUtil.*;
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

/*
class PersonOpenHelper extends SQLiteOpenHelper {

	final static private int DB_VERSION = 1;

	public PersonOpenHelper(Context context) {
		super(context, null, null, DB_VERSION);
    }
	private static PersonOpenHelper helper = new PersonOpenHelper(this);
	_db = helper.getReadableDatabase();
}

*/


public class Route extends RouteList {
    JctMask routePassed;

    public Route() {
        routePassed = new JctMask();

    }
    public Route(final RouteList other) {
        routePassed = new JctMask();
        if (other instanceof Route) {
            // If copying from another Route, preserve routePassed data
            assign((Route)other);
        } else {
            // If copying from RouteList, use parent class assign
            super.assign(other);
        }
    }
    /* --------------------------------------- */


    // operator=(final Route& source_route)
    void assign(final Route source_route) {
        assign(source_route, -1);
    }

    void assign(final Route source_route, int count) {
        super.assign(source_route, count);  // Call RouteList.assign()
        routePassed.assign(source_route.routePassed);  // Copy routePassed
    }
    
    // Public method to preserve routePassed during route updates
    public void preserveRoutePassed(final Route sourceRoute) {
        if (sourceRoute != null) {
            routePassed.assign(sourceRoute.routePassed);
        }
    }


    static class JCTSP_DATA {
        int jctSpMainLineId;		// 分岐特例:本線(b)
        int	jctSpStationId;			// 分岐特例:分岐駅(c)
        int	jctSpMainLineId2;		// 分岐特例:本線(b)
        int	jctSpStationId2;		// 分岐特例:分岐駅(c)
        JCTSP_DATA() {
            clear();
        }
        void clear() {
            jctSpMainLineId = 0;		// 分岐特例:本線(b)
            jctSpStationId = 0;			// 分岐特例:分岐駅(c)
            jctSpMainLineId2 = 0;		// 分岐特例:本線(b)
            jctSpStationId2 = 0;		// 分岐特例:分岐駅(c)
        }
    }

    static class JctMask {
        static int JCTMASKSIZE() {
            return ((MAX_JCT + 7) / 8);
        }
        char[] jct_mask = new char [JCTMASKSIZE()]; // about 40 byte

        JctMask() {
            clear();
        }
        void clear() {
            Arrays.fill(jct_mask, (char) 0);
        }
        void on(int jctid) {
            jct_mask[jctid / 8] |= (1 << (jctid % 8));
        }

        void off(int jctid) {
            jct_mask[jctid / 8] &= ~(1 << (jctid % 8));
        }

        boolean check(int jctid) {
            return (jct_mask[jctid / 8] & (1 << (jctid % 8))) != 0;
        }
        char at(int index) {
            return jct_mask[index];
        }
        void or(int index, char mask) {
            jct_mask[index] |= mask;
        }
        void and(int index, char mask) {
            jct_mask[index] &= ~mask;
        }
        void assign(final JctMask jctmask) {
            System.arraycopy(jctmask.jct_mask, 0, jct_mask, 0, jct_mask.length);
        }
    }


    ////////////////////////////////////////////////////////////////////////
    //
    //	Routeクラス インプリメント
    //

    /*!	@brief ルート作成(文字列からRouteオブジェクトの作成)
	 *
	 *	@param [in] route_str	カンマなどのデリミタで区切られた文字列("駅、路線、分岐駅、路線、..."）
	 *	@retval -200 failure(駅名不正)
	 *	@retval -300 failure(線名不正)
	 *	@retval -1   failure(経路重複不正)
	 *	@retval -2   failure(経路不正)
	 *	@retval 1 success
	 *	@retval 0 success
	 */
    public int setup_route(String route_str) {
        final String token = "[, |/\t]+";
        int lineId = 0;
        int stationId1 = 0;
        int stationId2 = 0;
        int rc = 1;
        boolean backup_notsamekokurahakatashinzai = route_flag.notsamekokurahakatashinzai;

        removeAll();

        if (route_str.startsWith(TITLE_NOTSAMEKOKURAHAKATASHINZAI)) {
            route_flag.notsamekokurahakatashinzai = true;
            route_str = route_str.substring(TITLE_NOTSAMEKOKURAHAKATASHINZAI.length());
        }
        String[] tok = route_str.split(token);
        for (String p : tok) {
            if (stationId1 == 0) {
                stationId1 = GetStationId(p);
                if (stationId1 <= 0) {
                    rc = -200;		/* illegal station name */
                    break;
                }
                add(stationId1);
            } else if (lineId == 0) {
                if (p.charAt(0) == 'r') {	/* 大阪環状線 遠回り */
                    route_flag.osakakan_detour = true;
                    lineId = GetLineId(p.substring(1));
                } else {
                    lineId = GetLineId(p);
                }
                if (lineId <= 0) {
                    rc = -300;		/* illegal line name */
                    break;
                }
            } else {
                stationId2 = GetStationId(p);
                if (stationId2 <= 0) {
                    rc = -200;		/* illegal station name */
                    break;
                }
                rc = add(lineId, /*stationId1,*/ stationId2);
				/* -1: route pass error
				 * -2: route error
				 * -100: db error
				 * 0: success(last)
				 * 1: success
                 * 4: company pass finish
                 * -4: wrong company pass
				 */
                //ASSERT((rc == 0) || (rc == 1) || (rc == 10) || (rc == 11) || (rc == 4));
                if (rc <= 0) {
                    break;
                }
                lineId = 0;
                stationId1 = stationId2;
            }
        }
        route_flag.notsamekokurahakatashinzai = backup_notsamekokurahakatashinzai;
        return rc;
    }

// 大阪環状線 通過制御フラグ定義
// ※ ここでいう「内回り」「外回り」は駅1-駅2間の進行方向ではなくDB定義上の
//    順廻り／大回りのことを指す
//                                    from  ex.
// 0) 初期状態
// 1) 近回り1回目 内回り                0 大阪→天王寺
// 2) 近回り1回目 外回り                0 天王寺→西九条
// 3) 近回り2回目 内回り - 内回り       1 今宮→大阪 … 京橋→天王寺
// 4) 近回り2回目 外回り - 内回り       2 西九条→天王寺 … 京橋→大阪
// 5) 近回り2回目 内回り - 外回り       1 大阪→京橋 … 天王寺→西九条
// 6) 近回り2回目 外回り - 外回り       2 N/A(ありえへん)
// 7) 遠回り指定 近回り1回目 内回り     1 大阪→天王寺 (1の時にUI指定した直後の状態)
// 8) 遠回り指定 近回り1回目 外回り     2 天王寺→西九条 (1の時にUI指定した直後の状態)
// 9) 遠回り指定 遠回り1回目 内回り     8 今宮→→京橋 (順廻りが遠回りの例)
// a) 遠回り指定 遠回り1回目 外回り     7 大阪→→天王寺(福島軽油)
// b) 遠回り指定 2回目 内回り - 内回り  9 今宮→→京橋 … 天王寺→京橋
// c) 遠回り指定 2回目 外回り - 内回り  a 西九条→→京橋 … 大阪→西九条
// d) 遠回り指定 2回目 内回り - 外回り  9 今宮→→京橋 … 天王寺→今宮
// e) 遠回り指定 2回目 外回り - 外回り  a ありえへん？
//
// 遠回り指定は、UIから。1,2,9,aのみ許可(7,8はあり得ない)
// 1,2は遠回りへ。 9,aは近回りへ


// bit0-1: 回数 2ビット 0～2
//        00=初期
//        01=1回目
//        10=2回目
//        11=n/a
// bit2: 1回目 内回り/外回り
// bit3: 2回目 内回り/外回り
// bit4: 遠回り指定


//        計算時は、bit 2 osakakan_detour のみ使用し、
//		  計算の過程では、ローカル変数フラグ使用

    private boolean chk_jctsb_b(int kind, int num) {
        boolean ret = false;

        switch (kind) {
            case JCTSP_B_NISHIKOKURA:
			/* 博多-新幹線-小倉*/
                ret = (2 <= num) &&
                        (DbIdOf.INSTANCE.line("山陽新幹線") == route_list_raw.get(num - 1).lineId) &&
                        (DbIdOf.INSTANCE.station("小倉") == route_list_raw.get(num - 1).stationId) &&
                        (DbIdOf.INSTANCE.station("博多") == route_list_raw.get(num - 2).stationId);
                break;
            case JCTSP_B_YOSHIZUKA:
			/* 小倉-新幹線-博多 */
                ret = (2 <= num) &&
                        (DbIdOf.INSTANCE.line("山陽新幹線") == route_list_raw.get(num - 1).lineId) &&
                        (DbIdOf.INSTANCE.station("博多") == route_list_raw.get(num - 1).stationId); //&&
                //(0 < InStation(DbIdOf.INSTANCE.station("小倉"),
                //                      DbIdOf.INSTANCE.line("山陽新幹線"),
                //                      route_list_raw.get(num - 1).stationId,
                //                      route_list_raw.get(num - 2).stationId));
                //return (2 <= num) &&
                //	(DbIdOf.INSTANCE.line("山陽新幹線") == route_list_raw.get(num - 1).lineId) &&
                //	(DbIdOf.INSTANCE.station("博多") == route_list_raw.get(num - 1).stationId) &&
                //	(DbIdOf.INSTANCE.station("小倉") == route_list_raw.get(num - 2).stationId);
                break;
            default:
                break;
        }
        return ret;
    }


    //private:
    static class RoutePass {
        //friend class Route;
        JctMask routePassed = new JctMask();

        JctMask   _source_jct_mask;
        int _line_id;
        int _station_id1;
        int _station_id2;
        int _start_station_id;
        int _num;		        // number of junction
        boolean    _err;
        RouteFlag _routeFlag;	// add() - removeTail() work

        void clear() { routePassed.clear(); _err = false; }
        void update(final RoutePass source) {
            routePassed.assign(source.routePassed);
            if (null != source._source_jct_mask) {
                _source_jct_mask = new JctMask();
                _source_jct_mask.assign(source._source_jct_mask);
            }
            _line_id = source._line_id;
            _station_id1 = source._station_id1;
            _station_id2 = source._station_id2;
            _start_station_id = source._start_station_id;
            _num = source._num;
            _err = source._err;
            _routeFlag = new RouteFlag(source._routeFlag);
        }
        RoutePass() { } // default constructor
        //    public:
        RoutePass(final RoutePass source) {
            update(source);
        }
        RoutePass(final JctMask jct_mask, final RouteFlag route_flag, int line_id, int station_id1, int station_id2) {
            this(jct_mask, route_flag, line_id, station_id1, station_id2, 0);
        }

        //public
        // 経路マークリストコンストラクタ
        //
        //	@param [in]  jct_mask        分岐マーク(used check()) nullはremoveTail()用
        //	@param [in]  route_flag       制御フラグ
        //	@param [in]  line_id         路線
        //	@param [in]  station_id1	 発 or 至
        //	@param [in]  station_id2     至 or 発
        //
        RoutePass(final JctMask jct_mask, final RouteFlag route_flag, int line_id, int station_id1, int station_id2, int start_station_id /* =0 */) {
            routePassed = new JctMask();
            _source_jct_mask = jct_mask;

            _line_id = line_id;
            _station_id1 = station_id1;
            _station_id2 = station_id2;
            _start_station_id = start_station_id;

            _routeFlag = new RouteFlag(route_flag);

            _err = false;

            if (station_id1 == station_id2) {
                _num = 0;
            } else {
                if (line_id == DbIdOf.INSTANCE.line("大阪環状線")) {
                    _num = enum_junctions_of_line_for_osakakan();
                } else if ((line_id == DbIdOf.INSTANCE.line("山陽新幹線"))
                        && route_flag.notsamekokurahakatashinzai) {
    			    if (station_id1 == DbIdOf.INSTANCE.station("博多")) {
    				    if (station_id2 == DbIdOf.INSTANCE.station("小倉")) {
                            /* 山陽新幹線 博多 -> 小倉 */
                            /* 博多の西はもうなかと、小倉の東は本州で閉塞区間じゃけん */
                            routePassed.on(Route.Id2jctId(station_id1));
                            routePassed.on(Route.Id2jctId(station_id2));
                            _num = 2;
    				    }
                        else {
                            /* 博多 -> 小倉、広島方面の場合*/
                            routePassed.on(Route.Id2jctId(station_id1)); // 博多
                            _station_id1 = DbIdOf.INSTANCE.station("小倉");
                            _num = 1 + enum_junctions_of_line(); // 小倉->小倉～新大阪間のどれか
                            _station_id1 = station_id1; // restore(博多)
                        }
                    }
                    else if (station_id2 == DbIdOf.INSTANCE.station("博多")) {
                        /* 山陽新幹線 -> 博多 */
                        if (station_id1 == DbIdOf.INSTANCE.station("小倉")) {
                            routePassed.on(Route.Id2jctId(station_id1));
                            routePassed.on(Route.Id2jctId(station_id2));
                            _num = 2;
                        }
                        else {
                            _station_id2 = DbIdOf.INSTANCE.station("小倉");
                            _num = enum_junctions_of_line(); //新大阪-小倉間のどれかの駅～小倉
                            _station_id2 = station_id2;	// restore(博多)
                            routePassed.on(Route.Id2jctId(station_id2));
                            _num += 1;
                        }
                    }
                    else {
                        _num = enum_junctions_of_line();
                    }
                } else if (IS_COMPANY_LINE(line_id)) {
                    routePassed.on(Route.Id2jctId(station_id1));
                    routePassed.on(Route.Id2jctId(station_id2));
                    _num = 2;
                } else {
                    _num = enum_junctions_of_line();
                }
            }
        }

        //	路線のbegin_station_id駅からto_station_id駅までの分岐駅リストを返す
        //
        //	注： lflg&(1<<17)を含めていないため、新幹線内分岐駅、たとえば、
        //	     東海道新幹線 京都 米原間に草津駅は存在するとして返します.
        //
        //	@return 分岐点数
        //
        private int enum_junctions_of_line() {
            int c;
            final String tsql =
                    "select id from t_lines l join t_jct j on j.station_id=l.station_id where" +
                            "	line_id=?1 and (lflg&((1<<31)|(1<<15)))=(1<<15)" +
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
                            "			and (station_id=?2 or" +
                            "				 station_id=?3))" +
                            " order by" +
                            " case when" +
                            "	(select sales_km from t_lines where line_id=?1 and station_id=?3) <" +
                            "	(select sales_km from t_lines where line_id=?1 and station_id=?2)" +
                            " then sales_km" +
                            " end desc," +
                            " case when" +
                            "	(select sales_km from t_lines where line_id=?1 and station_id=?3) >" +
                            "	(select sales_km from t_lines where line_id=?1 and station_id=?2)" +
                            " then sales_km" +
                            " end asc";

            try (Cursor dbo = RouteDB.db().rawQuery(tsql, new String[]{String.valueOf(_line_id),
                    String.valueOf(_station_id1),
                    String.valueOf(_station_id2)})) {
                c = 0;
                ++c;
                while (dbo.moveToNext()) {
                    routePassed.on(dbo.getInt(0));
                    c++;
                }
            } catch (Exception e) {
                c = -1;
            }
            return c;
        }

        //	路線のbegin_station_id駅からto_station_id駅までの分岐駅リストを返す(大阪環状線DB上の逆回り(今宮経由))
        //
        //	@return 分岐点数
        //
        private int enum_junctions_of_line_for_oskk_rev() {
            int c;
            final String tsql =
                    "select id from t_jct where station_id in ( " +
                            "select station_id from t_lines where line_id=?1 and (lflg&((1<<31)|(1<<15)))=(1<<15) and sales_km<= (select min(sales_km) from t_lines where " +
                            "line_id=?1 and (station_id=?2 or station_id=?3)) union all " +
                            "select station_id from t_lines where line_id=?1 and (lflg&((1<<31)|(1<<15)))=(1<<15) and sales_km>=(select max(sales_km) from t_lines where " +
                            "line_id=?1 and (station_id=?2 or station_id=?3)))";

            ASSERT (_line_id == DbIdOf.INSTANCE.line("大阪環状線"));

            try (Cursor dbo = RouteDB.db().rawQuery(tsql, new String[]{String.valueOf(_line_id),
                    String.valueOf(_station_id1),
                    String.valueOf(_station_id2)})) {
                c = 0;
                while (dbo.moveToNext()) {
                    routePassed.on(dbo.getInt(0));
                    c++;
                }
                return c;
            }
            //return -1;	/* error */
        }


        //	RoutePass Constructor: 大阪環状線の乗車経路の分岐駅リストを返す
        //
        //	@return 分岐点数(plus and 0 or minus)
        //
        private int enum_junctions_of_line_for_osakakan() {
            int i;
            int dir;
            int jnum = -1;
            int  check_result = 0;
            RoutePass farRoutePass = new RoutePass(this);

            ASSERT (_line_id == DbIdOf.INSTANCE.line("大阪環状線"));

            if (_source_jct_mask == null) {
				/* removeTail() */
                if (_routeFlag.is_osakakan_2pass()) {
                    if (_routeFlag.osakakan_2dir) {
                        jnum = enum_junctions_of_line_for_oskk_rev();
                    } else {
                        jnum = enum_junctions_of_line();
                    }
                    _routeFlag.osakakan_2dir = false;
                    _routeFlag.setOsakaKanFlag(RouteFlag.OSAKAKAN_PASS.OSAKAKAN_1PASS);

                } else if (_routeFlag.is_osakakan_1pass()) {
                    if (_routeFlag.osakakan_1dir) {
                        jnum = enum_junctions_of_line_for_oskk_rev();
                    } else {
                        jnum = enum_junctions_of_line();
                    }
                    _routeFlag.setOsakaKanFlag(RouteFlag.OSAKAKAN_PASS.OSAKAKAN_NOPASS);
                    _routeFlag.osakakan_1dir = false;
                    _routeFlag.osakakan_2dir = false;
                    _routeFlag.osakakan_detour = false;

                } else {
					/* 一回も通っていないはあり得ない */
                    System.out.printf("osaka-kan pass flag is not 1 or 2 (%d)\n", _routeFlag.getOsakaKanPassValue());
                    ASSERT (false);
                }
                return jnum;
            }

            dir = DirOsakaKanLine(_station_id1, _station_id2);

            if (_routeFlag.is_osakakan_nopass()) {
                // 始めての大阪環状線
        		//    detour near far
        		// a:   1       o    o  far,1p,
        		// b:   1       -    x  far,  n/a あり得へんけどそのままret
        		// c:   1       x    o  far,2p
        		// d:   1       -    x  far,  n/a あり得へんけどそのままret
        		// e:   0       o    o  near,1p
        		// f:   0       o    x  near,2p
        		// g:   0       x    o  far, 2p
        		// h:   0       x    x  x
                if (_routeFlag.osakakan_detour) {
                    // 大回り指定
                    dir ^= 1;
                    System.out.println("Osaka-kan: 1far");
                } else {
                    // 通常(大回り指定なし)
                    System.out.println("Osaka-kan: 1near");
                }

				/* DB定義上の順廻り（内回り) : 外回り */
                if ((0x01 & dir) == 0) {
                    jnum = enum_junctions_of_line();
                    System.out.println("Osaka-kan: 2fwd");
                } else {
                    jnum = enum_junctions_of_line_for_oskk_rev();
                    System.out.println("Osaka-kan: 2rev");
                }

                // 大阪環状線内駅が始発ポイントか見るため.
                // (始発ポイントなら1回通過でなく2回通過とするため)

                check_result = check() & 0x01;
                if (((0x01 & check_result) == 0) || !_routeFlag.osakakan_detour) {
                    // 近回りがOK または
                    // 近回りNGだが遠回り指定でない場合(大阪環状線内駅が始発ポイントである)
                    // a, c, e, f, g, h
                    if ((0x01 & dir) == 0) {
                        // 順廻り(DB上の. 実際には逆回り)が近回り.
                        // ...のときは遠回りを見てみる
                        i = farRoutePass.enum_junctions_of_line_for_oskk_rev();
                        System.out.println("Osaka-kan: 3rev");
                    } else {
                        // 逆回りが近回り
                        // ...のときは遠回りを見てみる
                        i = farRoutePass.enum_junctions_of_line();
                        System.out.println("Osaka-kan: 3fwd");
                    }
                } else {
                    // b, d
                    // 遠回り指定がNG
                    //ASSERT (false);
                    System.out.println("Osaka-kan Illegal detour flag.");
                    return jnum;
                }
                check_result |= ((farRoutePass.check() & 0x01) << 4);
                if ((0x11 & check_result) == 0) {
                    // a, e
					/* 両方向OK */
                    _routeFlag.setOsakaKanFlag(RouteFlag.OSAKAKAN_PASS.OSAKAKAN_1PASS);
                    if ((0x01 & dir) == 0) {
                        _routeFlag.osakakan_1dir = false;
                        System.out.println("Osaka-kan: 5fwd");
                    } else {
                        _routeFlag.osakakan_1dir = true;
                        System.out.println("Osaka-kan: 5rev");
                    }
                } else if ((0x11 & check_result) != 0x11) {
                    // c, f, g
                    // 近回りか遠回りのどっちかダメ
                    if ((0x11 & check_result) == 0x01) {
                        // 近回りがNG
                        // f
                        dir ^= 1;                        /* 戻す */
                        jnum = i;
                        update(farRoutePass);	// 遠回りを採用
                        // updateは_last_flagを壊す
                        System.out.println("Osaka-kan: 6far");
                    } else {
                        // c, g
                        System.out.println("Osaka-kan: 6near");
                    }
                    _routeFlag.setOsakaKanFlag(RouteFlag.OSAKAKAN_PASS.OSAKAKAN_2PASS); /* 大阪環状線駅始発 */
                    if ((0x01 & dir) == 0) {
                        System.out.println("Osaka-kan:7fwd");
                        _routeFlag.osakakan_1dir = false;
                    } else {                                      // 計算時の経路上では1回目なので、
                        _routeFlag.osakakan_1dir = true;   // osakakan_2dirでなく、osakakan_1dir
                        System.out.println("Osaka-kan:7rev");
                    }
                } else {
                    // どっち廻りもダメ(1回目でそれはない)
                    // h
                    // thru
                    System.out.println("Osaka-kan:8");
                    //ASSERT (false); 大阪 東海道線 草津 草津線 柘植 関西線 今宮 大阪環状線 x鶴橋
                }
            } else if (_routeFlag.is_osakakan_1pass()) {

                // 2回目の大阪環状線
                // 最初近回りで試行しダメなら遠回りで。
                for (i = 0; i < 2; i++) {

                    clear();

                    if ((0x01 & dir) == 0) {
                        jnum = enum_junctions_of_line();
                        System.out.println("Osaka-kan: 9fwd");
                    } else {
                        jnum = enum_junctions_of_line_for_oskk_rev();
                        System.out.println("Osaka-kan: 9rev");
                    }
                    if (((0x01 & check()) == 0) &&
                            (STATION_IS_JUNCTION(_start_station_id) || (_start_station_id == _station_id2) ||
                                    (InStationOnLine(DbIdOf.INSTANCE.line("大阪環状線"), _start_station_id) <= 0) ||
                                    (InStationOnOsakaKanjyou(dir, _start_station_id, _station_id1, _station_id2) <= 0))) {
                        // 開始駅が大阪環状線なら、開始駅が通過範囲内にあるか ?
                        // 野田 大阪環状線 大阪 東海道線 尼崎 JR東西線 京橋 大阪環状線 福島(環)
                        // の2回目の大阪環状線は近回りの内回りは大阪でひっかかるが、外回りは分岐駅のみで判断
                        // すると通ってしまうので */
                        System.out.printf("Osaka-kan: 9(%d, %d, %d)\n", STATION_IS_JUNCTION(_start_station_id) ? 1:0, InStationOnLine(DbIdOf.INSTANCE.line("大阪環状線"), _start_station_id), InStationOnOsakaKanjyou(dir, _start_station_id, _station_id1, _station_id2));
                        System.out.println("Osaka-kan: 9ok");
                        break; /* OK */
                    } else {
                        _err = true;	/* for jct_mask all zero and 0 < InStationOnOsakaKanjyou() */
                    }
                    //System.out.printf("%d %d", STATION_IS_JUNCTION(_start_station_id), _start_station_id);
                    //System.out.printf("Osaka-kan: @(%d, %d, %d)\n", check(), InStationOnLine(DbIdOf.INSTANCE.line("大阪環状線"), _start_station_id), InStationOnOsakaKanjyou(dir, _start_station_id, _station_id1, _station_id2));
                    System.out.println("Osaka-kan: 9x");
                    dir ^= 0x1;	/* 方向を逆にしてみる */
                } /* NG */

                if (i < 2) {
					/* OK */
                    _routeFlag.setOsakaKanFlag(RouteFlag.OSAKAKAN_PASS.OSAKAKAN_2PASS);

                    if ((dir & 0x01) == 0) {
                        _routeFlag.osakakan_2dir = false;
                        System.out.println("Osaka-kan: 10ok_fwd");
                    } else {
                        _routeFlag.osakakan_2dir = true;
                        System.out.println("Osaka-kan: 10ok_rev");
                    }
                } else {
					/* NG */
                    System.out.println("Osaka-kan: 11x");
                }

            } else {
                // 既に2回通っているので無条件で通過済みエラー
                System.out.println("Osaka-kan: 3time");
                jnum = enum_junctions_of_line();
                ASSERT ((check() & 0x01) != 0);
                enum_junctions_of_line_for_oskk_rev(); // safety
                ASSERT ((check() & 0x01) != 0);
            }
            System.out.printf("\ncheck:%d, far=%d, err=%d\n", check(), farRoutePass.check(), _err ? 1:0);
            return jnum;	/* Failure */
        }

        //public
        //	分岐駅リストの指定分岐idの乗車マスクをOff
        //
        //	@param [in]  jid   分岐id
        //
        void off(int jid) {
            routePassed.off(jid);
        }

        //public
        //	分岐駅リストの乗車マスクをOff
        //
        //	@param [in]  jct_mask   分岐mask
        //
        void off(JctMask jct_mask) {
            int i;

            int count = JctMask.JCTMASKSIZE();
            for (i = 0; i < count; i++) {
                jct_mask.and(i, routePassed.at(i));

                if (false) { // BuildConfig.DEBUG removed
                    for (int j = 0; j < 8; j++) {
                        if ((routePassed.at(i) & (1 << j)) != 0) {
                            System.out.printf("removed.  : %s\n", JctName(i * 8 + j));
                        }
                    }
                }
            }
        }

        //public
        //	分岐駅リストの乗車マスクをOn
        //
        //	@param [in]  jct_mask   分岐mask
        //
        void on(JctMask jct_mask) {
            int i;

            int count = JctMask.JCTMASKSIZE();
            for (i = 0; i < count; i++) {
                jct_mask.or(i, routePassed.at(i));
                if (false) { // BuildConfig.DEBUG removed
                    for (int j = 0; j <= 8; j++) {
                        if (((1 << j) & routePassed.at(i)) != 0) {
                            System.out.printf("  add-mask on: %s(%d,%d)\n", JctName((i * 8) + j), Jct2id((i * 8) + j), (i * 8) + j);
                        }
                    }
                }
            }
        }

        //public
        //	分岐駅リストの乗車マスクをcheck
        //
        //	@param [in]  jct_mask   分岐mask
        //	@retval 0 = success
        //	@retval 1 = already passed.
        //	@retval 3 = already passed and last arrive point passed.
        //	@retval 2 = last arrive point.
        //
        int check() { return check(false); }
        int check(boolean is_no_station_id1_first_jct) {
            int i;
            int j;
            int k;
            int jctid;
            int rc;

            if (_source_jct_mask == null) {
                ASSERT (false);
                return -1;	// for removeTail()
            }
            rc = 0;
            int count = JctMask.JCTMASKSIZE();
            for (i = 0; i < count; i++) {
                k = (_source_jct_mask.at(i) & routePassed.at(i));
                if (0 != k) {
				    /* 通過済みポイントあり */
                    for (j = 0; j < 8; j++) {
                        if ((k & (1 << j)) != 0) {
                            jctid = (i * 8) + j; /* 通過済みポイント */
                            if (Jct2id(jctid) == _station_id2) {
                                rc |= 2;   /* 終了駅 */
                            } else if (is_no_station_id1_first_jct || (Jct2id(jctid) != _station_id1)) {
                                // 前回着駅=今回着駅は、通過済みフラグON
                                rc |= 1;	/* 既に通過済み */
                                System.out.printf("  already passed error: %s(%d,%d)\n", JctName(jctid), Jct2id(jctid), jctid);
                                break;
                            }
                        }
                    }
                    if ((rc & 1) != 0) {
                        break;   /* 既に通過済み */
                    }
                }
            }
            if (((rc & 0x01) == 0) && _err) {
                System.out.println("Osaka-kan Pass check error.");
                rc |= 1;
            }
            return rc;
        }

        void update_flag(RouteFlag source_flg) {
            source_flg.setOsakaKanFlag(_routeFlag);
        }
        int num_of_junction() { return _num; }

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
        static int InStationOnOsakaKanjyou(int dir, int start_station_id, int station_id_a, int station_id_b) 	{
            int n = 0;

            if ((dir & 0x01) == 0) {
                n = InStation(start_station_id, DbIdOf.INSTANCE.line("大阪環状線"), station_id_a, station_id_b);
            } else {
                final String tsql =
                        "select count(*) from (" +
                                "select station_id from t_lines where line_id=?1 and (lflg&(1<<31))=0 and sales_km<=(select min(sales_km) from t_lines where " +
                                "line_id=?1 and (station_id=?2 or station_id=?3)) union all " +
                                "select station_id from t_lines where line_id=?1 and (lflg&(1<<31))=0 and sales_km>=(select max(sales_km) from t_lines where " +
                                "line_id=?1 and (station_id=?2 or station_id=?3))" +
                                ") where station_id=?4";

                try (Cursor dbo = RouteDB.db().rawQuery(tsql, new String[]{String.valueOf(DbIdOf.INSTANCE.line("大阪環状線")),
                        String.valueOf(station_id_a),
                        String.valueOf(station_id_b),
                        String.valueOf(start_station_id)})) {
                    try {
                        if (dbo.moveToNext()) {
                            n = dbo.getInt(0);
                        }
                    } finally {
                        dbo.close();
                    }
                }
            }
            return n;
        }
    } // class RoutePass


    //private:
    //	@brief	分岐マークOff
    //
    //	@param [in]  line_id          路線
    //	@param [in]  to_station_id    開始駅(含まない)
    //	@param [in]  begin_station_id 終了駅(含む)
    //
    //	@note add()で分岐特例経路自動変換時に使用大阪環状線はないものとする
    //
    void routePassOff(int line_id, int to_station_id, int begin_station_id)	{
        RoutePass route_pass = new RoutePass(routePassed, route_flag, line_id, to_station_id, begin_station_id);
        route_pass.off(routePassed);
        route_pass.update_flag(route_flag); /* update route_flag LF_OSAKAKAN_MASK */
    }

    //Static
    //	@brief 分岐特例の分岐路線a+乗換駅dから本線bと分岐駅cを得る
    //
    //	@param [in]  jctLineId         a 分岐路線
    //	@param [in]  transferStationId d 乗換駅
    //	@param [out] jctspdt   b 本線, c 分岐駅
    //
    //	@return type 0: usual, 1-3:type B
    //
    static int RetrieveJunctionSpecific(int jctLineId, int transferStationId, JCTSP_DATA jctspdt) {
        final String tsql =
                //"select calc_km>>16, calc_km&65535, (lflg>>16)&32767, lflg&32767 from t_lines where (lflg&(1<<31))!=0 and line_id=?1 and station_id=?2"; +
                //	"select type,jctsp_line_id1, jctsp_station_id1, jctsp_line_id2, jctsp_station_id2" +
                //	" from t_jctspcl where id=(" +
                //	"	select calc_km from t_lines where (lflg&(1<<31))!=0 and line_id=?1 and station_id=?2)"; +
                "select type,jctsp_line_id1, jctsp_station_id1, jctsp_line_id2, jctsp_station_id2" +
                        " from t_jctspcl where id=("  +
                        "	select lflg&255 from t_lines where (lflg&((1<<31)|(1<<29)))!=0 and line_id=?1 and station_id=?2)";
        int type = 0;

        try (Cursor dbo = RouteDB.db().rawQuery(tsql, new String[]{String.valueOf(jctLineId),
                String.valueOf(transferStationId)})) {
            if (dbo.moveToNext()) {
                type = dbo.getInt(0);
                jctspdt.jctSpMainLineId = dbo.getInt(1);
                jctspdt.jctSpStationId = dbo.getInt(2);
                jctspdt.jctSpMainLineId2 = dbo.getInt(3);
                jctspdt.jctSpStationId2 = dbo.getInt(4);
            }
        }
        ASSERT (((jctspdt.jctSpMainLineId2 == 0) && (jctspdt.jctSpStationId2 == 0)) ||
                ((jctspdt.jctSpMainLineId2 != 0) && (jctspdt.jctSpStationId2 != 0)));
        if (jctspdt.jctSpStationId2 == 0) {	// safety
            jctspdt.jctSpMainLineId2 = 0;
        }
        return type;
    }

    //	@brief 分岐特例の分岐路線a+乗換駅dから本線bと分岐駅cを得る
    //
    //	@param [in]  jctLineId         a 分岐路線
    //	@param [in]  transferStationId d 乗換駅
    //	@param [out] jctSpMainLineId   b 本線
    //	@param [out] jctSpStationId    c 分岐駅
    //
    //	@return type 0: usual, 1-3:type B
    //
    static int GetBsrjctSpType(int line_id, int station_id) {
        final String tsql =
                "select type from t_jctspcl where id=(select lflg&255 from t_lines where line_id=?1 and station_id=?2)";
        int type = -1;
        try (Cursor dbo = RouteDB.db().rawQuery(tsql, new String[]{String.valueOf(line_id),
                String.valueOf(station_id)})) {
            if (dbo.moveToNext()) {
                type = dbo.getInt(0);
            }
        }
        return type;
    }

    //	分岐駅が経路内に含まれているか？
    //
    //	@param [in] stationId   駅ident
    //	@retval true found
    //	@retval false notfound
    //
    int junctionStationExistsInRoute(int stationId) {
        int c;

        c = 0;
        for (RouteItem route_item : route_list_raw) {
            if (stationId == route_item.stationId) {
                c++;
            }
        }
        return c;
    }

    //	経路の種類を得る
    //
    //	@retval bit0 : there is shinkansen.
    //	@retval bit1 : there is company line.
    //
    public int typeOfPassedLine(int offset)
    {
        int c;
        int n;
        c = 0;
        n = 0;
        for (RouteItem route_item : route_list_raw) {
            ++n;
            if (offset < n) {
                if (IS_SHINKANSEN_LINE(route_item.lineId)) {
                    c |= 0x01;
                }
                if (IS_COMPANY_LINE(route_item.lineId)) {
                    c |= 0x02;
                }
                if (c == 0x03) {
                    break;
                }
            }
        }
        return c;
    }

    //private:
    //	大阪環状線経路変更による経路再構成
    //
    //	@return 0 success(is last)
    //	@retval 1 success
    //	@retval otherwise failued
    //
    int reBuild() {
        Route routeWork = new Route();
        int rc = 0;

        if (route_list_raw.size() <= 1) {
            return 0;
        }

        routeWork.add(route_list_raw.get(0).stationId);

        // add() の開始駅追加時removeAll()が呼ばれlast_flagがリセットされるため)
        routeWork.getRouteFlag().osakakan_detour = route_flag.osakakan_detour;
        routeWork.getRouteFlag().notsamekokurahakatashinzai = route_flag.notsamekokurahakatashinzai;

        Iterator<RouteItem> pos = route_list_raw.iterator();
        if (pos.hasNext()) {
            pos.next();
        }
        while (pos.hasNext()) {
            RouteItem ri = (RouteItem)pos.next();
            rc = routeWork.add(ri.lineId, ri.stationId);
            if (rc != ADDRC_OK) {
				/* error */
                break;
            }
        }
        if ((rc < 0) || ((rc != ADDRC_OK) && ((rc == ADDRC_LAST) && (pos.hasNext())))) {
            System.out.printf("Can't reBuild() rc=%d\n", rc);
            route_flag.osakakan_detour = false;
            return -1;	/* error */
        }

        // 経路自体は変わらない。フラグのみ
        // 特例適用、発着駅を単駅指定フラグは保持(大阪環状線廻り)

        route_flag.setAnotherRouteFlag(routeWork.getRouteFlag());
        routePassed.assign(routeWork.routePassed);

        return rc;
    }

    //	遠回り/近回り設定
    //
    //	@return 0 success(is last)
    //	@retval 1 success
    //	@retval otherwise failued
    //
    int setDetour() {
        return setDetour(true);
    }

    public int setDetour(boolean enabled) {
        int rc;

        route_flag.osakakan_detour = enabled;
        rc = reBuild();

        return rc;
    }

    //  特例非適用
    //
    //	@return 0 success(is last)
    //	@retval 1 success
    //	@retval otherwise failued
    //
    public void setNoRule(boolean no_rule) {

        if (!no_rule && route_flag.osakakan_detour) {
            route_flag.osakakan_detour = false;
            reBuild();
        }
        route_flag.no_rule = no_rule;
    }


    //	小倉ー博多 新幹線・在来線別線扱い
    //	@param [in] enabled  : true = 有効／ false = 無効
    //
    public void setNotSameKokuraHakataShinZai(boolean enabled)
    {
        route_flag.notsamekokurahakatashinzai = enabled;
    }

    int brtPassCheck(int stationId2) {
        return /*(stationId2 == DbIdOf.INSTANCE.station("柳津")) ? -1 : */0;
    }

    public boolean isNotSameKokuraHakataShinZai()
    {
        return route_flag.notsamekokurahakatashinzai;
    }


        /*	経路追加
    	 *
    	 *	@param [in] line_id      路線ident
    	 *	@param [in] stationId1   駅1 ident
    	 *	@param [in] stationId2   駅2 ident
    	 *	@param [in] ctlflg       デフォルト0(All Off)
    	 *							 bit8: 新幹線乗換チェックしない
    	 *
         *  @retval 0 = OK(last)         ADDRC_LAST
         *  @retval 1 = OK               ADDRC_OK
         *  @retval 4 = OK(last-company) ADDRC_CEND
         *  //@retval 2 = OK(re-route)
         *  @retval 5 = already finished ADDRC_END)
         *  @retval -1 = overpass(復乗エラー)
         *  @retval -2 = 経路エラー(stationId1 or stationId2はline_id内にない)
         *  @retval -3 = operation error(開始駅未設定)
         *	@retval -4 = 会社線 通過連絡運輸なし
         *  @retval -100 DB error
         *	@retval route_flag bit4-0 : reserve
         *	@retval route_flag bit5=1 : 次にremoveTailでlastItemの通過マスクをOffする(typeOでもPでもないので)
         *	@retval route_flag bit5=0 : 次にremoveTailでlastItemの通過マスクをOffする(typeOでもPでもないので)
         *	@retval route_flag bit6=1 : 分岐特例区間指定による経路変更あり
    	 */
    final static int ADD_BULLET_NC = (1<<8);

    public int add(int stationId) {
        removeAll(true);
        route_list_raw.add(new RouteItem((short)0, (short)stationId));
        System.out.printf("add-begin %s(%d)\n", StationName(stationId), stationId);
        return 1;
    }

    public int add(int line_id, int stationId2) {
        return add(line_id, stationId2, 0);
    }
    public int add(int line_id, int stationId2, int ctlflg) {
        int rc;
        int i;
        int j;
        int num;
        int stationId1;
        int lflg1;
        int lflg2;
        int start_station_id = 0;
        boolean replace_flg = false;	// 経路追加ではなく置換
        int jct_flg_on = 0;   // 水平型検知(D-2) / BSRNOTYET_NA
        int type = 0;
        int is_no_station_id1_first_jct = 0;
        JCTSP_DATA jctspdt = new JCTSP_DATA();

        //if (BuildConfig.DEBUG) {
        int original_line_id = line_id;
        //}
        int first_station_id1 = 0;

        //RouteFlag.   System.out.printf("route_flag=%x\n", route_flag);

        if (route_flag.end) {
            if (route_flag.compnda) {
                System.out.println("route.add(): Can't add for disallow company line.");
                return -4;          // not allow company connect.
            } else {
                System.out.println("route.add(): already terminated.");
                return	ADDRC_END;		/* already terminated. */

            }
        }

        route_flag.trackmarkctl = false;
        route_flag.jctsp_route_change = false;

        num = (int)route_list_raw.size();
        if (num <= 0) {
            ASSERT (false);	// bug. must be call to add(station);
            return -3;
        }
        start_station_id = route_list_raw.get(0).stationId;
        stationId1 = route_list_raw.get(route_list_raw.size() - 1).stationId;
        first_station_id1 = stationId1;

        /* 発駅 */
        lflg1 = AttrOfStationOnLineLine(line_id, stationId1);
        if (BIT_CHK(lflg1, BSRNOTYET_NA)) {
            return -2;		/* 不正経路(line_idにstationId1は存在しない) */
        }

		/* 着駅が発駅～最初の分岐駅間にあるか? */
		/* (着駅未指定, 発駅=着駅は除く) */
        /* Dec.2016: Remove teminal */
        //if ((num == 1) && (0 < end_station_id) && (end_station_id != start_station_id) &&
        //        (end_station_id != stationId2) &&
        //        (0 != InStation(end_station_id, line_id, stationId1, stationId2))) {
        //    return -1;	/* <t> already passed error  */
        //}

        ASSERT (BIT_CHK(lflg1, BSRJCTHORD) || route_list_raw.get(num - 1).stationId == stationId1);

        lflg2 = AttrOfStationOnLineLine(line_id, stationId2);
        if (BIT_CHK(lflg2, BSRNOTYET_NA)) {
            return -2;		/* 不正経路(line_idにstationId2は存在しない) */
        }

        // 直前同一路線指定は受け付けない
        // 直前同一駅は受け付けない
        //if (!BIT_CHK(route_list_raw.back().flag, BSRJCTHORD) && ((1 < num) && (line_id == route_list_raw.back().lineId))) {
        //	System.out.printf("iregal parameter by same line_id.");
        //	return -1;		// E-2, G, G-3, f, j1,j2,j3,j4 >>>>>>>>>>>>>>>>>>
        //}
        if (stationId1 == stationId2) {
            System.out.println("iregal parameter by same station_id.");
            System.out.println("add_abort");
            return -1;		// E-112 >>>>>>>>>>>>>>>>>>
        }

        // 同一路線で折り返した場合
        if ((BRTMASK(route_list_raw.get(num - 1).lineId) == BRTMASK(line_id)) &&
                (2 <= num) &&
                (DirLine(route_list_raw.get(num - 1).lineId, route_list_raw.get(num - 2).stationId, stationId1) !=
                 DirLine(line_id, stationId1, stationId2))) {
            System.out.println("re-route error.");
            System.out.println("add_abort");
            return -1;		//  >>>>>>>>>>>>>>>>>>
        }

        rc = companyPassCheck(line_id, stationId1, stationId2, num);
    	if (rc < 0) {
    		return rc;	/* 通過連絡運輸なし >>>>>>>>>>>> */
    	}

        // 分岐特例B(BSRJCTSP_B)水平型検知
        if (BIT_CHK(lflg2, BSRJCTSP_B) && chk_jctsb_b((type = GetBsrjctSpType(line_id, stationId2)), num)) {
            System.out.println("JCT: h_(B)detect");
            jct_flg_on = BIT_ON(jct_flg_on, BSRNOTYET_NA);	/* 不完全経路フラグ */
        } else {
            /* 新幹線在来線同一視区間の重複経路チェック(lastItemのflagがBSRJCTHORD=ONがD-2ケースである */
            int shinzairev = 0;
            jct_flg_on = BIT_OFF(jct_flg_on, BSRSHINZAIREV);
            if ((0 == (ctlflg & ADD_BULLET_NC))
               && (1 < num)
               && (0 != (shinzairev = Route.CheckTransferShinkansen(route_list_raw.get(num - 1).lineId, line_id,
                                        route_list_raw.get(num - 2).stationId, stationId1, stationId2)))) {
                if ((0 < shinzairev) && checkPassStation(shinzairev)) {
                    // 在来線戻り
                    System.out.printf("assume detect shinkansen-zairaisen too return.%s,%s %s %s\n", LineName(route_list_raw.get(num - 1).lineId), LineName(line_id), StationName(route_list_raw.get(num - 1).stationId), StationName(stationId2));
                    int local_line;
                    int bullet_line;
                    if (IS_SHINKANSEN_LINE(route_list_raw.get(num - 1).lineId)) {
                        local_line = line_id;
                        bullet_line = route_list_raw.get(num - 1).lineId;
                    } else {
                        local_line = route_list_raw.get(num - 1).lineId;
                        bullet_line = line_id;
                    }
                    if ((((AttrOfStationOnLineLine(local_line, stationId2) >>> BSRSHINKTRSALW) & 0x03) != 0) &&
                        (0 < InStationOnLine(bullet_line, stationId2, true))) {
                        System.out.println("      disable");
                        jct_flg_on = BIT_ON(jct_flg_on, BSRSHINZAIREV); // この後着駅が終端だったらエラー
                    }
                    // 第16条の2-2 "〜の各駅を除く。）"
                    // return -1;	// 高崎~長岡 上越新幹線 新潟 "信越線(直江津-新潟)" 長岡 (北長岡までなら良い)
                    // 大宮 上越新幹線 長岡 信越線(直江津-新潟) 直江津
                    // 岡山 山陽新幹線 新大阪 東海道線 大阪 福知山線 谷川
                    // では、許されるので、保留にする
                }
    			if ((shinzairev < 0) && !BIT_CHK2(route_list_raw.get(num - 1).flag, BSRJCTHORD, BSRJCTSP_B)) {
		    		/* 上の2条件で、直江津 信越線(直江津-新潟) 長岡 上越新幹線 大宮 は除外する */
                    System.out.println("JCT: F-3b");
    			    return -1;		// F-3b
                }
            }
    	}
        System.out.printf("add %s(%d)-%s(%d), %s(%d)\n", LineName(line_id), line_id, StationName(stationId1), stationId1, StationName(stationId2), stationId2);

        if (BIT_CHK(route_list_raw.get(num - 1).flag, BSRJCTSP_B)) {
			 /* 信越線上り(宮内・直江津方面) ? (フラグけちってるので
			  * t_jctspcl.type RetrieveJunctionSpecific()で吉塚、小倉廻りと区別しなければならない) */
            if ((LINE_DIR.LDIR_DESC == DirLine(line_id, route_list_raw.get(num - 1).stationId, stationId2)) &&
                    ((num < 2) || ((2 <= num) &&
                            (LINE_DIR.LDIR_ASC  == DirLine(route_list_raw.get(num - 1).lineId,
                                    route_list_raw.get(num - 2).stationId,
                                    route_list_raw.get(num - 1).stationId)))) &&
                    (JCTSP_B_NAGAOKA == RetrieveJunctionSpecific(route_list_raw.get(num - 1).lineId,
                            route_list_raw.get(num - 1).stationId, jctspdt))) {
                if (stationId2 == jctspdt.jctSpStationId2) { /* 宮内止まり？ */
                    System.out.println("JSBH004");
                    System.out.println("add_abort");
                    return -1;
                } else {
                    // 長岡 → 浦佐
                    // 新幹線 長岡-浦佐をOff
                    routePassOff(route_list_raw.get(num - 1).lineId,
                            route_list_raw.get(num - 1).stationId,
                            jctspdt.jctSpStationId);
                    route_list_raw.get(num - 1).let(new RouteItem(route_list_raw.get(num - 1).lineId,
                            (short)jctspdt.jctSpStationId));
                    // 上越線-宮内追加
                    rc = add(jctspdt.jctSpMainLineId, jctspdt.jctSpStationId2, ADD_BULLET_NC);		//****************
                    route_flag.jctsp_route_change = true;	/* route modified */
                    if (rc != ADDRC_OK) {
                        System.out.println("junction special (JSBS001) error.");
                        System.out.println("add_abort");
                        return rc;			// >>>>>>>>>>>>>>>>>>>>>
                    }
                    num++;
                    stationId1 = jctspdt.jctSpStationId2; // 宮内
                    // line_id : 信越線
                    // stationId2 : 宮内～長岡
                }
            } else if ((2 <= num) && (DbIdOf.INSTANCE.line("山陽新幹線") == line_id)) { /* b#14021205 add */
                routePassed.off(Id2jctId(route_list_raw.get(num - 2).stationId));
                System.out.println("b#14021205-1");
            }
        } else if ((2 <= num) && BIT_CHK(lflg2, BSRJCTSP_B) &&	/* b#14021205 add */
                (DbIdOf.INSTANCE.line("山陽新幹線") == route_list_raw.get(num - 1).lineId) &&
                (DirLine(route_list_raw.get(num - 1).lineId,
                        route_list_raw.get(num - 2).stationId,
                        route_list_raw.get(num - 1).stationId)
                        != DirLine(line_id, route_list_raw.get(num - 1).stationId, stationId2))) {
            routePassed.off(Id2jctId(route_list_raw.get(num - 1).stationId));
            System.out.println("b#14021205-2");
            jct_flg_on = BIT_ON(jct_flg_on, BSRNOTYET_NA);	/* 不完全経路フラグ */
        }

        // 水平型検知
        if (BIT_CHK(route_list_raw.get(num - 1).flag, BSRJCTHORD)) {
            System.out.println("JCT: h_detect 2 (J, B, D)");
            if ((DbIdOf.INSTANCE.station("博多") != stationId1) &&
                    IsAbreastShinkansen(route_list_raw.get(num - 1).lineId, line_id, stationId1, stationId2)) {
                // 	line_idは新幹線
                //	route_list_raw.get(num - 1).lineIdは並行在来線
                //
                ASSERT (IS_SHINKANSEN_LINE(line_id));
                if (0 != InStation(route_list_raw.get(num - 2).stationId, line_id, stationId1, stationId2)) {
                    System.out.println("JCT: D-2");
                    j = NextShinkansenTransferTermInRange(line_id, stationId1, stationId2);
                    if (j <= 0) {	// 隣駅がない場合
                        if (false) { // BuildConfig.DEBUG removed
                            ASSERT (original_line_id == line_id);
                        }
                        i = route_list_raw.get(num - 1).lineId;	// 並行在来線
                        // 新幹線の発駅には並行在来線(路線b)に所属しているか?
                        if (0 == InStationOnLine(i, stationId2)) {
                            System.out.println("prev station is not found in shinkansen.");
                            System.out.println("add_abort");	// 恵那-名古屋-東京方面で、
                            // 名古屋-三河安城間に在来線にない駅が存在し、
                            // その駅が着駅(stationId2)の場合
                            // ありえない
                            return -1;			// >>>>>>>>>>>>>>>>>>>
                        } else {
                            System.out.println("JCT: hor.(D-2x)");
                            removeTail();
                            num--;
                            stationId1 = route_list_raw.get(route_list_raw.size() - 1).stationId;
                            line_id = i;
                        }
                    } else {
                        System.out.println("JCT: hor.1(D-2)");
                        i = route_list_raw.get(num - 1).lineId;	// 並行在来線
                        if (InStationOnLine(i, j) <= 0) {
                            System.out.println("junction special (JSBX001) error.");
                            System.out.println("add_abort");
                            return -1;
                        }
                        removeTail();
                        rc = add(i, j, ADD_BULLET_NC);		//****************
                        ASSERT (rc == ADDRC_OK);
                        stationId1 = j;
                    }
                    route_flag.jctsp_route_change = true;	/* route modified */
                } else {
                    System.out.println("JCT: B-2");
                }
            } else {
                System.out.println("JCT: J");
            }
            System.out.println(">");
        }

        // 151 check
        // while is only using it because don't want to use goto.
        while (BIT_CHK(lflg1, BSRJCTSP)) {
            System.out.println(">");
            // 段差型
            if (BIT_CHK(lflg2, BSRJCTSP)) {	// 水平型でもある?
                // retrieve from a, d to b, c
                if (false) { // BuildConfig.DEBUG removed
                    ASSERT (original_line_id == line_id);
                }
                type = RetrieveJunctionSpecific(line_id, stationId2, jctspdt); // update jctSpMainLineId(b), jctSpStation(c)
                ASSERT (0 < type);
                System.out.printf("JCT: detect step-horiz:%d\n", type);
                if (jctspdt.jctSpStationId2 != 0) {
                    lflg1 = BIT_OFF(lflg1, BSRJCTSP);				// 別に要らないけど
                    break;
                }
            }
            if (false) { // BuildConfig.DEBUG removed
                ASSERT (original_line_id == line_id);
            }
            ASSERT (first_station_id1 == stationId1);

            // retrieve from a, d to b, c
            type = RetrieveJunctionSpecific(line_id, stationId1, jctspdt); // update jctSpMainLineId(b), jctSpStation(c)
            ASSERT (0 < type);
            System.out.printf("JCT: detect step:%d\n", type);
            if (stationId2 != jctspdt.jctSpStationId) {
                if (route_list_raw.get(num - 1).lineId == jctspdt.jctSpMainLineId) {
                    ASSERT (stationId1 == route_list_raw.get(num - 1).stationId);
                    if (0 < InStation(jctspdt.jctSpStationId,
                            route_list_raw.get(num - 1).lineId,
                            route_list_raw.get(num - 2).stationId,
                            stationId1)) {
                        System.out.println("JCT: C-1");
                        routePassOff(jctspdt.jctSpMainLineId, jctspdt.jctSpStationId, stationId1);	// C-1
                    } else { // A-1
                        System.out.println("JCT: A-1");
                        is_no_station_id1_first_jct = 1;
                    }
                    if ((2 <= num) && (jctspdt.jctSpStationId == route_list_raw.get(num - 2).stationId)) {
                        removeTail();
                        System.out.println("JCT: A-C");		// 3, 4, 8, 9, g,h
                        --num;
                    } else {
                        route_list_raw.get(num - 1).let(new RouteItem(route_list_raw.get(num - 1).lineId,
                                (short)jctspdt.jctSpStationId));
                        System.out.println("JCT: b#21072801D");
                        is_no_station_id1_first_jct++;
                    }
                    if (jctspdt.jctSpStationId2 != 0) {		// 分岐特例路線2
                        System.out.println("JCT: step_(2)detect");
                        rc = add(jctspdt.jctSpMainLineId2, jctspdt.jctSpStationId2, ADD_BULLET_NC);	//**************
                        ASSERT (rc == ADDRC_OK);
                        num++;
                        if (rc != ADDRC_OK) {			// safety
                            route_flag.jctsp_route_change = true;	/* route modified */
                            System.out.println("A-1/C-1(special junction 2)");
                            System.out.println("add_abort");
                            return -1;
                        }
                        if (stationId2 == jctspdt.jctSpStationId2) {
                            System.out.println("KF1,2");
                            line_id = jctspdt.jctSpMainLineId2;
                            replace_flg = true;
                        }
                        stationId1 = jctspdt.jctSpStationId2;
                    } else {
                        System.out.printf("is_no_station_id1_first_jct is on: is_no_station_id1_first_jct=%d, is_junction %b, %s <- %s\n", is_no_station_id1_first_jct, STATION_IS_JUNCTION_F(lflg1), RouteUtil.StationName(stationId1), RouteUtil.StationName(jctspdt.jctSpStationId));
                        if ((is_no_station_id1_first_jct == 2) && !STATION_IS_JUNCTION_F(lflg1)) {
                            is_no_station_id1_first_jct = 555;
                        }
                        stationId1 = jctspdt.jctSpStationId;
                    }
                } else {
                    ASSERT (first_station_id1 == stationId1);
                    if ((num < 2) ||
                            !IsAbreastShinkansen(jctspdt.jctSpMainLineId,
                                    route_list_raw.get(num - 1).lineId,
                                    stationId1,
                                    route_list_raw.get(num - 2).stationId)
                            || (jctspdt.jctSpStationId == DbIdOf.INSTANCE.station("西小倉")) // KC-2
                            || (jctspdt.jctSpStationId == DbIdOf.INSTANCE.station("吉塚")) // KC-2
                            || (InStation(jctspdt.jctSpStationId,
                            route_list_raw.get(num - 1).lineId,
                            route_list_raw.get(num - 2).stationId,
                            stationId1) <= 0)) {
                        // A-0, I, A-2
                        System.out.println("JCT: A-0, I, A-2");	//***************

                        if ((jctspdt.jctSpStationId == DbIdOf.INSTANCE.station("西小倉")) // KC-2
                                || (jctspdt.jctSpStationId == DbIdOf.INSTANCE.station("吉塚"))) { // KC-2
                            routePassOff(jctspdt.jctSpMainLineId,
                                    stationId1, jctspdt.jctSpStationId);
                            System.out.println("JCT: KC-2");
                        }
                        rc = add(jctspdt.jctSpMainLineId,
								 /*route_list_raw.get(num - 1).stationId,*/ jctspdt.jctSpStationId,
                                ADD_BULLET_NC);
                        ASSERT (rc == ADDRC_OK);
                        num++;
                        if (rc != ADDRC_OK) {				// safety
                            route_flag.jctsp_route_change = true;	/* route modified */
                            System.out.println("A-0, I, A-2");
                            System.out.println("add_abort");
                            return -1;					//>>>>>>>>>>>>>>>>>>>>>>>>>>
                        }
                        if (jctspdt.jctSpStationId2 != 0) {		// 分岐特例路線2
                            rc = add(jctspdt.jctSpMainLineId2, jctspdt.jctSpStationId2, ADD_BULLET_NC);	//**************
                            num++;
                            ASSERT (rc == ADDRC_OK);
                            if (rc != ADDRC_OK) {			// safety
                                route_flag.jctsp_route_change = true;	/* route modified */
                                System.out.println("A-0, I, A-2(special junction 2)");
                                System.out.println("add_abort");
                                return -1;				//>>>>>>>>>>>>>>>>>>>>>>>>>>
                            }
                            if (stationId2 == jctspdt.jctSpStationId2) {
                                System.out.println("KF0,3,4");
                                line_id = jctspdt.jctSpMainLineId2;
                                replace_flg = true;
                            }
                            stationId1 = jctspdt.jctSpStationId2;
                        } else {
                            stationId1 = jctspdt.jctSpStationId;
                        }
                    } else {
                        ASSERT (first_station_id1 == stationId1);

                        // C-2
                        System.out.println("JCT: C-2");
                        ASSERT (IS_SHINKANSEN_LINE(route_list_raw.get(num - 1).lineId));
                        routePassOff(jctspdt.jctSpMainLineId,
                                jctspdt.jctSpStationId, stationId1);
                        i = NextShinkansenTransferTermInRange(route_list_raw.get(num - 1).lineId, stationId1, route_list_raw.get(num - 2).stationId);
                        if (i <= 0) {	// 隣駅がない場合
                            System.out.println("JCT: C-2(none next station on bullet line)");
                            // 新幹線の発駅には並行在来線(路線b)に所属しているか?
                            if (0 == InStationOnLine(jctspdt.jctSpMainLineId,
                                    route_list_raw.get(num - 2).stationId)) {
                                route_flag.jctsp_route_change = true;	/* route modified */
                                System.out.println("next station is not found in shinkansen.");
                                System.out.println("add_abort");
                                return -1;			// >>>>>>>>>>>>>>>>>>>
                            } else {
                                removeTail();
                                rc = add(jctspdt.jctSpMainLineId,
                                        jctspdt.jctSpStationId, ADD_BULLET_NC);	//**************
                                ASSERT (rc == ADDRC_OK);
                                stationId1 = jctspdt.jctSpStationId;
                            }
                        } else {
                            route_list_raw.get(num - 1).let(new RouteItem(route_list_raw.get(num - 1).lineId, (short)i));
                            route_list_raw.add(new RouteItem((short)jctspdt.jctSpMainLineId,
                                    (short)jctspdt.jctSpStationId));
                            stationId1 = jctspdt.jctSpStationId;
                        }
                    }
                }
                route_flag.jctsp_route_change = true;	/* route modified */
            } else {
                // E, G		(stationId2 == jctspdt.jctSpStationId)
                System.out.println("JCT: E, G");
                if (jctspdt.jctSpStationId2 != 0) {
                    System.out.println("JCT: KE0-4");
                    lflg2 = BIT_OFF(lflg2, BSRJCTSP);
                }
                line_id = jctspdt.jctSpMainLineId;
                ASSERT(first_station_id1 == stationId1);

                if ((2 <= num) &&
                        //			!BIT_CHK(AttrOfStationOnLineLine(line_id, stationId2), BSRJCTSP_B) &&
                        (0 < InStation(stationId2, jctspdt.jctSpMainLineId,
                                route_list_raw.get(num - 2).stationId, stationId1))) {
                    System.out.println("E-3:duplicate route error.");
                    System.out.println("add_abort");
                    return -1;	// Duplicate route error >>>>>>>>>>>>>>>>>
                }
                if (BIT_CHK(AttrOfStationOnLineLine(line_id, stationId2), BSRJCTSP_B)) {
                    // 博多-小倉-西小倉
                    //
                    System.out.println("jct-b nisi-kokura-stop/yoshizuka-stop");
                }
                if (route_list_raw.get(num - 1).lineId == jctspdt.jctSpMainLineId) {
                    // E-3 , B-0, 5, 6, b, c, d, e
                    // E-0, E-1, E-1a, 6, b, c, d, e
                    System.out.println("JCT: E-3, B0,5,6,b,c,d,e, E-0,E-1,E-1a,6,b,c,d,e");
                    replace_flg = true;
                } else {
                    // E-2, G, G-3, G-2, G-4
                    System.out.println("JCT: E-2, G, G-3, G-2, G-4");
                }
                route_flag.jctsp_route_change = true;	/* route modified */
            }
            break;
        }
        if (BIT_CHK(lflg2, BSRJCTSP)) {
            // 水平型
            // a(line_id), d(stationId2) -> b(jctSpMainLineId), c(jctSpStationId)
            if (false) { // BuildConfig.DEBUG removed
                ASSERT (original_line_id == line_id);
                //ASSERT (first_station_id1 == stationId2);
            }
            type = RetrieveJunctionSpecific(line_id, stationId2, jctspdt);
            ASSERT (0 < type);
            System.out.printf("JCT:%d\n", type);
            if (stationId1 == jctspdt.jctSpStationId) {
                // E10-, F, H
                System.out.println("JCT: E10-, F, H/KI0-4");
                line_id = jctspdt.jctSpMainLineId;	// a -> b
                if (route_list_raw.get(num - 1).lineId == jctspdt.jctSpMainLineId) {
                    if ((2 <= num) &&
                            (0 < InStation(stationId2, jctspdt.jctSpMainLineId,
                                    route_list_raw.get(num - 2).stationId, stationId1))) {
                        System.out.println("E11:duplicate route error.");
                        System.out.println("add_abort");
                        return -1;	// Duplicate route error >>>>>>>>>>>>>>>>>
                    }
                    replace_flg = true;
                    System.out.println("JCT: F1, H, E11-14");
                } else {
                    // F-3bはエラーとするため. route_flag = BIT_ON(jct_flg_on, BSRJCTHORD);	// F-3b
                }
            } else {
                // J, B, D
                if ((jctspdt.jctSpStationId2 != 0) && (stationId1 == jctspdt.jctSpStationId2)) {	// 分岐特例路線2
                    System.out.println("JCT: KJ0-4(J, B, D)");
                    rc = add(jctspdt.jctSpMainLineId2, jctspdt.jctSpStationId, ADD_BULLET_NC);		//**************
                    num++;
                    if (rc != ADDRC_OK) {
                        route_flag.jctsp_route_change = true;	/* route modified */
                        System.out.println("junction special (KJ) error.");
                        System.out.println("add_abort");
                        return rc;			// >>>>>>>>>>>>>>>>>>>>>
                    }
                } else {
                    if (jctspdt.jctSpStationId2 != 0) {	// 分岐特例路線2
                        System.out.println("JCT: KH0-4(J, B, D) add(日田彦山線, 城野c')");
                        rc = add(line_id, /*stationId1,*/ jctspdt.jctSpStationId2, ADD_BULLET_NC);	//**************
                        num++;
                        if (rc == ADDRC_OK) {
                            System.out.println("JCT: add(日豊線b', 西小倉c)");
                            rc = add(jctspdt.jctSpMainLineId2, jctspdt.jctSpStationId, ADD_BULLET_NC);	//**************
                            num++;
                        }
                        if (rc != ADDRC_OK) {
                            route_flag.jctsp_route_change = true;	/* route modified */
                            System.out.println("junction special horizen type convert error.");
                            System.out.println("add_abort");
                            return rc;			// >>>>>>>>>>>>>>>>>>>>>
                        }
                    } else {
                        System.out.println("JCT: J, B, D");
                        rc = add(line_id, /*stationId1,*/ jctspdt.jctSpStationId, ADD_BULLET_NC);	//**************
                        num++;
                        if (rc != ADDRC_OK) {
                            route_flag.jctsp_route_change = true;	/* route modified */
                            System.out.println("junction special horizen type convert error.");
                            System.out.println("add_abort");
                            return rc;			// >>>>>>>>>>>>>>>>>>>>>
                        }
                        if (stationId2 != DbIdOf.INSTANCE.station("小倉")) {
                            // b#15032701
                            jct_flg_on = BIT_ON(jct_flg_on, BSRJCTHORD);	//b#14021202
                        }
                    }
                }
                // b#14021202 route_flag = BIT_ON(jct_flg_on, BSRJCTHORD);
                line_id = jctspdt.jctSpMainLineId;
                stationId1 = jctspdt.jctSpStationId;
            }
            route_flag.jctsp_route_change = true;	/* route modified */
            is_no_station_id1_first_jct = 0;
        }

        // 長岡周りの段差型
        if ((2 <= num) && BIT_CHK(lflg1, BSRJCTSP_B)) {
            is_no_station_id1_first_jct = 0;
            if (JCTSP_B_NAGAOKA == RetrieveJunctionSpecific(line_id,
                    route_list_raw.get(num - 1).stationId, jctspdt)) {
			 	/* 信越線下り(直江津→長岡方面) && 新幹線|上越線上り(長岡-大宮方面)? */
                if ((LINE_DIR.LDIR_ASC == DirLine(route_list_raw.get(num - 1).lineId,
                        route_list_raw.get(num - 2).stationId,
                        route_list_raw.get(num - 1).stationId)) &&
                        (LINE_DIR.LDIR_DESC == DirLine(line_id,
                                route_list_raw.get(num - 1).stationId,
                                stationId2))) {
					/* 宮内発 */
                    if (route_list_raw.get(num - 2).stationId == jctspdt.jctSpStationId2) {
                        System.out.println("junction special 2(JSBS004) error.");
                        System.out.println("add_abort");
                        return -1;			// >>>>>>>>>>>>>>>>>>>>>
                    }
                    // 長岡Off
                    routePassed.off(Id2jctId(route_list_raw.get(num - 1).stationId));

                    // 長岡->宮内へ置換
                    route_list_raw.get(num - 1).let(new RouteItem(route_list_raw.get(num - 1).lineId,
                            (short)jctspdt.jctSpStationId2));

                    // 上越線 宮内→浦佐
                    rc = add(jctspdt.jctSpMainLineId, jctspdt.jctSpStationId, ADD_BULLET_NC);		//****************
                    route_flag.jctsp_route_change = true;	/* route modified */
                    if (ADDRC_OK != rc) {
                        System.out.println("junction special 2(JSBH001) error.");
                        System.out.println("add_abort");
                        return rc;			// >>>>>>>>>>>>>>>>>>>>>
                    }
                    stationId1 = jctspdt.jctSpStationId;
                    num += 1;
                }
            }
        }

        RoutePass route_pass = new RoutePass(routePassed, route_flag, line_id, stationId1, stationId2, start_station_id);
        if (route_pass.num_of_junction() < 0) {
            System.out.println("DB error(add-junction-enum)");
            System.out.println("add_abort");
            ASSERT (false);
            return -100;		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        }

        // Route passed check
        rc = route_pass.check(is_no_station_id1_first_jct == 555);

        if (line_id == DbIdOf.INSTANCE.line("大阪環状線")) {
            if ((rc & 0x01) != 0) {
                rc = -1;
            } else if ((rc & 0x02) != 0) {
				/* 着駅終了*/
                if (STATION_IS_JUNCTION_F(lflg2)) {
                    rc = 1;		/* trackmarkctl をONにして、次のremoveTail()で削除しない */
                    System.out.println("osaka-kan last point junction");
                } else {
                    System.out.println("osaka-kan last point not junction");
                    rc = 2;
                }
            } else if ((rc & 0x03) == 0) {
                if ((2 <= route_list_raw.size()) && (route_list_raw.get(1).lineId == line_id) &&
                        !STATION_IS_JUNCTION(start_station_id) && (start_station_id != stationId2) &&
                        (0 != InStation(stationId2, line_id, start_station_id, route_list_raw.get(1).stationId))) {
                    System.out.println("osaka-kan passed error");	// 要るか？2015-2-15
                    rc = -1;	/* error */
                    rc = 0;
                } else if (start_station_id == stationId2) {
                    rc = 1;
                } else {
                    rc = 0;	/* OK */
                }
            } else {
                ASSERT (false);	// rc & 0x03 = 0x03
                rc = -1;		// safety
            }
        } else if (rc == 0) {	// 復乗なし
            if (((2 <= route_list_raw.size()) && (start_station_id != stationId2) &&
    			(0 != InStation(start_station_id, line_id, stationId1, stationId2)))
    			/* =Dec.2016:Remove terminal= ||
    			(((0 < end_station_id) && (end_station_id != stationId2) && (2 <= route_list_raw.size())) &&
    			(0 != RouteList::InStation(end_station_id, line_id, stationId1, stationId2)))*/) {
    			rc = -1;	/* <v> <p> <l> <w> */
    		} else if (start_station_id == stationId2) {
    			rc = 2;		/* <f> */
    		} else {
    			rc = 0;		/* <a> <d> <g> */
    		}
        } else {	// 復乗
            if ((rc & 1) == 0) { /* last */
//よくわかんないが、着駅指定UIだった頃の残骸？不要と思われるので消してみる。test_exec は、OK
//                if (
//                    //	(!STATION_IS_JUNCTION(stationId2)) ||
//                    // sflg.12は特例乗り換え駅もONなのでlflg.15にした
//                    (!STATION_IS_JUNCTION_F(lflg2)) ||
//            		((2 <= num) && (start_station_id != stationId2) &&
//            		 (0 != InStation(start_station_id, line_id, stationId1, stationId2)))
//            		 /* =Dec.2016:Reomve terminal=  ||
//            		(((0 < end_station_id) && (end_station_id != stationId2) && (2 <= num)) &&
//            		(0 != RouteList::InStation(end_station_id, line_id, stationId1, stationId2)))
//            		*/
//                        ) {
//                    rc = -1;	/* <k> <e> <r> <x> <u> <m> */
//                } else  {
                    rc = 1;		/* <b> <j> <h> */
//                }
            } else {
                rc = -1;	/* 既に通過済み */
            }
        }

        if (rc < 0) {
            // 不正ルートなのでmaskを反映しないで破棄する
            if (is_no_station_id1_first_jct == 555) {
                // b#21072801D
                // don't necessary     stationId1 = first_station_id1;
                // restore
                route_list_raw.get(num - 1).let(new RouteItem(route_list_raw.get(num - 1).lineId,
                (short)first_station_id1));
                System.out.printf("Detect finish. %d\n", first_station_id1);
            }
            System.out.printf("add_abort(%d)\n", rc);
            route_flag.trackmarkctl = false;
            // E-12, 6, b, c, d, e
            return -1;	/* already passed error >>>>>>>>>>>>>>>>>>>> */

        } else {
            // normality
            // 通過bit ON(maskをマージ)
            route_pass.on(routePassed);

            // 大阪環状線通過フラグを設定
            route_pass.update_flag(route_flag); /* update route_flag LF_OSAKAKAN_MASK */
        }

        if ((stationId1 == DbIdOf.INSTANCE.station("吉塚"))
         && (stationId2 == DbIdOf.INSTANCE.station("博多")) && (rc == 1)) {
            // 上りで、博多南 博多南線 博多 鹿児島線 原田 筑豊線 桂川\(九\) 篠栗線 吉塚 鹿児島線 博多 山陽新幹線 広島
            System.out.println("43-2 博多pre-Off");
            routePassed.off(Id2jctId(stationId2));
            rc = 0;
        }

        if (type == JCTSP_B_YOSHIZUKA) {
            // 広島 山陽新幹線 博多 鹿児島線 吉塚 篠栗線 桂川\(九\) 筑豊線  原田 鹿児島線 博多 博多南線 博多南
            System.out.println("43-2 博多Off");
            ASSERT (route_list_raw.get(num - 1).stationId == stationId1);
            routePassed.off(Id2jctId(route_list_raw.get(num - 1).stationId));
        }

		/* 追加か置換か */
        if (replace_flg) {
            ASSERT (0 < type);	// enable jctspdt
            ASSERT ((line_id == jctspdt.jctSpMainLineId) || (line_id == jctspdt.jctSpMainLineId2));
            ASSERT ((route_list_raw.get(num - 1).lineId == jctspdt.jctSpMainLineId) ||
                    (route_list_raw.get(num - 1).lineId == jctspdt.jctSpMainLineId2));
            route_list_raw.remove(route_list_raw.size() - 1);
            --num;
        }
        lflg1 = AttrOfStationOnLineLine(line_id, stationId1);
        lflg2 = AttrOfStationOnLineLine(line_id, stationId2);

        lflg2 |= (lflg1 & 0xff000000);
        lflg2 &= (0xff00ffff & ~(1<<BSRJCTHORD));	// 水平型検知(D-2);
        lflg2 |= jct_flg_on;	// BSRNOTYET_NA:BSRJCTHORD
        route_list_raw.add(new RouteItem((short)line_id, (short)stationId2, lflg2));
        ++num;

        if (rc == 0) {
            System.out.println("added continue.");
            route_flag.trackmarkctl = false;
        } else if (rc == 1) {
            route_flag.trackmarkctl  = true;
        } else if (rc == 2) {
            route_flag.trackmarkctl = false;	/* 次にremoveTailでlastItemの通過マスクをOffする(typeOでもPでもないので) */
        } else {
            ASSERT (false);
            route_flag.trackmarkctl  = true;
        }

        System.out.printf("added fin.(%d).\n", rc);

        if (rc != 0) {
            if (BIT_CHK(lflg2, BSRNOTYET_NA)) {
                System.out.printf("？？？西小倉・吉塚.rc=%d\n", rc);
                return ADDRC_OK;	/* 西小倉、吉塚 */
            } else if (BIT_CHK(lflg2, BSRSHINZAIREV)) {
                System.out.printf("detect shinkansen-zairaisen too return.arrive %s replace to %s\n", StationName(route_list_raw.get(num - 1).stationId), StationName(route_list_raw.get(num - 2).stationId));
                removeTail();
                return -1;  /* route is duplicate */
            } else {
                route_flag.end  = true;
                System.out.println("detect finish.");
                return ADDRC_LAST;
            }
        } else {
            if (route_flag.compnda) {
    			route_flag.end  = true;
    			return ADDRC_CEND;
    		} else {
                rc = brtPassCheck(stationId2);
                if (rc < 0) {
                    route_flag.end = true;
                    System.out.println("detect BRT finish.");
                    return ADDRC_LAST;
                }
    			return ADDRC_OK;	/* OK - Can you continue */
    		}
    	}
    }


    //	経路の末尾を除去
    //
    //	@param [in] begin_off    最終ノードの始点もOffするか(デフォルト:Offしない)
    //
    public void removeTail() {
        removeTail(false);
    }

    public void removeTail(boolean begin_off/* = false*/) {
        int line_id;
        int begin_station_id;
        int to_station_id;
        int i;
        int list_num;

        System.out.println("Enter removeTail");

        list_num = (int)route_list_raw.size();
        if (list_num < 2) {
            route_flag.trackmarkctl = false;
            return;
        }

		/* 発駅～最初の乗換駅時 */
        if (list_num <= 2) {
            removeAll(false, false);
            return;
        }

        line_id = route_list_raw.get(list_num - 1).lineId;

        to_station_id = route_list_raw.get(list_num - 1).stationId;	// tail
        begin_station_id = route_list_raw.get(list_num - 2).stationId;	// tail - 1

        RoutePass route_pass = new RoutePass(null, route_flag, line_id, to_station_id, begin_station_id);

        if (!begin_off) {
			/* 開始駅はOffしない(前路線の着駅なので) */
            i = Id2jctId(begin_station_id);
            if (0 < i) {
                route_pass.off(i);
            }
        }

        i = Id2jctId(to_station_id);
        if ((0 < i) && route_flag.trackmarkctl) {
			/* 最近分岐駅でO型経路、P型経路の着駅の場合は除外 */
            route_pass.off(i);
        }

        route_pass.off(routePassed);

        route_pass.update_flag(route_flag); /* update route_flag LF_OSAKAKAN_MASK */
        route_flag.trackmarkctl = false;
        route_flag.end = false;

        if (IS_COMPANY_LINE(route_list_raw.get(route_list_raw.size() - 1).lineId)) {
    		if (!IS_COMPANY_LINE(route_list_raw.get(list_num - 2).lineId)) {
    			route_flag.compncheck = false;
    		}
    	}

        route_list_raw.remove(route_list_raw.size() - 1);	// route_list_raw.pop_back();

    	/* 後半リストチェック */
    	if (IS_COMPANY_LINE(route_list_raw.get(route_list_raw.size() - 1).lineId)) {
    		route_flag.compnend  = true;
    		route_flag.compnpass = false;
    	} else {
    		route_flag.compnend = false;
    	}
    	route_flag.compnda = false;
    }

    //	経路設定中 経路重複発生時
    //	経路設定キャンセル
    //
    //	@param [in]  bWithStart  開始駅もクリアするか(デフォルトクリア)
    //	@note
    //	x stop_jctId > 0 : 指定分岐駅前までクリア(指定分岐駅含まず)
    //	x stop_jctId = 0 : (default)指定分岐駅指定なし
    //	x stop_jctId < 0 : 開始地点も削除
    //
    public void removeAll(boolean bWithStart /* =true */) {
        removeAll(bWithStart, true);
    }

    public void removeAll() {
        removeAll(true, true);
    }

    public void removeAll(boolean bWithStart /* =true */, boolean bWithEnd /* =true */) {
        int begin_station_id = 0;

        routePassed.clear();

        if (!bWithStart) {
            begin_station_id = departureStationId();
        }

        route_list_raw.clear();

        boolean shinzaikyusyu = route_flag.notsamekokurahakatashinzai; // backup
        route_flag.clear();
        route_flag.notsamekokurahakatashinzai = shinzaikyusyu; // restore

        System.out.println("clear-all mask.");

        if (!bWithStart) {
            add(begin_station_id);
        }
    }

    //public:
    //	経路を逆転
    //
    //	@retval 1   sucess
    //	@retval 0   sucess(end)
    //	@retval -1	failure(6の字を逆転すると9になり経路重複となるため)
    //
    public int reverse() {
        int station_id;
        int line_id;
        int rc = -1;

        List<RouteItem> route_list_rev = new ArrayList<>(route_list_raw.size());

        if (route_list_raw.size() <= 1) {
            return rc;  // -1
        }

        ListIterator<RouteItem> revIterator = route_list_raw.listIterator(route_list_raw.size());
        while (revIterator.hasPrevious()) {
            RouteItem ri = revIterator.previous();
            route_list_rev.add(ri);
        }

        removeAll();	/* clear route_list_raw */

        ListIterator<RouteItem> ite = route_list_rev.listIterator();
        if (ite.hasNext()) {
            RouteItem ri = ite.next();
            station_id = ri.stationId;
            rc = add(station_id);
            line_id = ri.lineId;
            while (ite.hasNext()) {
                ri = ite.next();
                rc = add(line_id, /*station_id,*/ ri.stationId);
                if (rc < 0) {
					/* error */
					/* restore */

                    removeAll();	/* clear route_list_raw */

                    ite = route_list_rev.listIterator(route_list_rev.size());
                    if (ite.hasPrevious()) {
                        ri = ite.previous();
                        station_id = ri.stationId;
                        add(station_id);
                        while (ite.hasPrevious()) {
                            ri = ite.previous();
                            int r = add(ri.lineId, /*station_id,*/ ri.stationId, 1<<8);
                            ASSERT (0 <= r);
                        }
                    } else {
                        ASSERT(false);
                    }
                    System.out.println("cancel reverse route");
                    return rc;
                }
                station_id = ri.stationId;
                line_id = ri.lineId;
            }
        } else {
            ASSERT(false);
        }
        System.out.println("reverse route");
        return rc;
    }

    //public:
    //
    //	最短経路に変更(raw immidiate)
    //
    //	@param [in] useBulletTrain 0 在来線のみ
    //                             1 新幹線を利用
    //                             2 会社線を利用
    //                             3 新幹線も会社線も利用
    //                          100  地方交通線を除く(在来線のみ)
    //	@retval true success
    //	@retval 1 : success
    //	@retval 0 : loop end.
    //	@retval 4 : already routed
    //	@retval 5 : already finished
    //	@retval -n: add() error(re-track)
    //	@retval -32767 unknown error(DB error or BUG)
    //
    public int changeNeerest(int useBulletTrain, int end_station_id) {
        class Dijkstra	{
        	class NODE_JCT {
        		int minCost;
        		short fromNode;
        		boolean done_flg;
        		short line_id;
                NODE_JCT() {
                    minCost = -1;
                    fromNode = 0;
                    done_flg = false;
                    line_id = 0;
                }
        	}
            final NODE_JCT [] d;
        	Dijkstra() {
        		int i;
        		d = new NODE_JCT [MAX_JCT];
        		/* ダイクストラ変数初期化 */
        		for (i = 0; i < MAX_JCT; i++) {
                    d[i] = new NODE_JCT();
        		}
        	}
        	void setMinCost(int index, int value) { d[index].minCost = value; }
        	void setFromNode(int index, int value) { d[index].fromNode = (short)value; }
        	void setDoneFlag(int index, boolean value) { d[index].done_flg = value; }
        	void setLineId(int index, int value) { d[index].line_id = (short)value; }

        	int minCost(int index) { return d[index].minCost; }
        	short fromNode(int index) { return (short)d[index].fromNode; }
        	boolean doneFlag(int index) { return d[index].done_flg; }
        	short lineId(int index) { return (short)d[index].line_id; }
        }
        Dijkstra dijkstra = new Dijkstra();

        ASSERT (0 < departureStationId());
        //ASSERT (startStationId() != end_station_id);

        short startNode;
        short lastNode = 0;
        short excNode1 = 0;
        short excNode2 = 0;
        short lastNode1 = 0;
        int lastNode1_distance = 0;
        short lastNode2 = 0;
        short lastNode2_distance = 0;
        int i;
        boolean loopRoute;
        //int km;
        int a = 0;
        int b = 0;
        short doneNode;
        int cost;
        short id;
        short lid;
        short stationId;
        short nLastNode;
        short[][] neer_node;

        boolean except_local;

        if (useBulletTrain == 100) {
            useBulletTrain = 0;
            except_local = true;
        } else {
            except_local = false;
        }

		/* 途中追加か、最初からか */
        if (1 < route_list_raw.size()) {
            do {
                stationId = route_list_raw.get(route_list_raw.size() - 1).stationId;
                if (0 == Id2jctId(stationId)) {
                    removeTail();
                    route_flag.jctsp_route_change = true;	/* route modified */
                } else {
                    break;
                }
            } while (1 < route_list_raw.size());

            stationId = route_list_raw.get(route_list_raw.size() - 1).stationId;

        } else {
            stationId = (short)departureStationId();
        }

        if ((stationId == end_station_id) || (end_station_id <= 0)) {
            return 4;			/* already routed */
        }

        if (route_flag.end) {
            return 5;           // already finished
        }
		/* ダイクストラ変数初期化 */

        startNode = Id2jctId(stationId);
        lastNode = Id2jctId(end_station_id);
        if (startNode == 0) { /* 開始駅は非分岐駅 */
            // 駅の両隣の最寄分岐駅IDとその駅までの計算キロを得る
            neer_node = GetNeerNode(stationId);
            lid = LineIdFromStationId(stationId);
            // 発駅～最初の分岐駅までの計算キロを最初の分岐駅までの初期コストとして初期化
            a = Id2jctId(neer_node[0][0]);
            if (!routePassed.check(a)) {
                dijkstra.setMinCost(a - 1, neer_node[0][1]);
    			dijkstra.setFromNode(a - 1, -1);	// from駅を-1(分岐駅でないので存在しない分岐駅)として初期化
    			dijkstra.setLineId(a - 1, lid);
            }
            if (neer_node[1][0] != 0) {	// 両端あり?
                b = Id2jctId(neer_node[1][0]);
                if (!routePassed.check(b)) {
                    dijkstra.setMinCost(b - 1, neer_node[1][1]);
    				dijkstra.setFromNode(b - 1, -1);
    				dijkstra.setLineId(b - 1, lid);
                } else if (routePassed.check(a)) {
                    System.out.println("Autoroute:発駅の両隣の分岐駅は既に通過済み");
                    return -10;								// >>>>>>>>>>>>>>>>>>>>>>>
                }
            } else {
				/* 盲腸線 */
                if (routePassed.check(a)) {
                    System.out.println("Autoroute:盲腸線で通過済み.");
                    return -11;								// >>>>>>>>>>>>>>>>>>>>>>>>>>
                }
                b = 0;
            }
        } else {
            dijkstra.setMinCost(startNode - 1, 0);
        }

        loopRoute = false;

        if (lastNode == 0) { /* 終了駅は非分岐駅 ? */
            // 駅の両隣の最寄分岐駅IDとその駅までの計算キロを得る
            neer_node = GetNeerNode(end_station_id);

            // dijkstraのあとで使用のために変数に格納
            // 終了駅の両隣の分岐駅についてはadd()でエラーとなるので不要(かどうか？）

            lastNode1 = Id2jctId(neer_node[0][0]);
            lastNode1_distance = neer_node[0][1];
            if (neer_node[0][0] != 0) {
                if (neer_node[1][0] != 0) {
                    nLastNode = 2;
                } else {
                    nLastNode = 1;
                }
            } else {
                nLastNode = 0;
            }
            //nLastNode = (int)neer_node.size();
            ASSERT ((nLastNode == 1) || (nLastNode == 2));  /* 野辺地の大湊線はASSERT */
            if (2 <= nLastNode) {
                lastNode2 = Id2jctId(neer_node[1][0]);
                lastNode2_distance = neer_node[1][1];
                nLastNode = 2;
            } else {
				/* 着駅=閉塞線 */
                lastNode2 = 0;
                lastNode2_distance = (short)0xffff;
                nLastNode = 1;
            }
            System.out.printf("Last target=%s, %s\n", StationName(Jct2id(lastNode1)), StationName(Jct2id(lastNode2)));
        } else {
            nLastNode = 0;
            lastNode1 = lastNode2 = 0;
        }

        if ((2 == route_list_raw.size()) &&
                IsSameNode(route_list_raw.get(route_list_raw.size() - 1).lineId,
                        route_list_raw.get(0).stationId,
                        route_list_raw.get(route_list_raw.size() - 1).stationId)) {
            id = Id2jctId(departureStationId());
            if (id == 0) {
                neer_node = GetNeerNode(departureStationId());
                if (neer_node[1][0] != 0) {	// neer_node.size() == 2
                    excNode1 = Id2jctId(neer_node[0][0]);		/* 渋谷 品川 代々木 */
                    excNode2 = Id2jctId(neer_node[1][0]);		/* 渋谷 品川 新宿 */
                    System.out.printf("******** loopRouteX **%s, %s******\n", StationName(Jct2id(excNode1)), StationName(Jct2id(excNode2)));
                    loopRoute = true;
                } else {
					/* 逗子 大船 磯子 */
                }
            } else {
                excNode1 = id;
                excNode2 = Id2jctId(route_list_raw.get(route_list_raw.size() - 1).stationId);
                if (excNode2 == 0) {
                    neer_node = GetNeerNode(route_list_raw.get(route_list_raw.size() - 1).stationId);
                    if (neer_node[1][0] != 0) {	// neer_node.size() == 2
                        excNode1 = Id2jctId(neer_node[0][0]);
                        excNode2 = Id2jctId(neer_node[1][0]);
                        ASSERT ((id == excNode1) || (id == excNode2));
                        loopRoute = true;		/* 代々木 品川 代々木 */
                    } else {
                        ASSERT (false);	/* 先頭で途中追加の最終ノードは分岐駅のみとしているのであり得ない */
						                /* 大船 鎌倉 横須賀などは、鎌倉がremoveTail() され大船 横須賀となる*/
                    }
                } else {
                    loopRoute = true;
                }
                System.out.printf("******** loopRouteY **%s, %s******\n", StationName(Jct2id(excNode1)), StationName(Jct2id(excNode2)));
            }
        }

		/* dijkstra */
        for (;;) {
            doneNode = -1;
            for (i = 0; i < MAX_JCT; i++) {
                // ノードiが確定しているとき
                // ノードiまでの現時点での最小コストが不明の時
                if (dijkstra.doneFlag(i) || (dijkstra.minCost(i) < 0)) {
                    continue;
                }
				/*  確定したノード番号が-1かノードiの現時点の最小コストが小さいとき
				 *  確定ノード番号更新する
				 */
                 if ((doneNode < 0) || (!routePassed.check(i + 1) && (dijkstra.minCost(i) < dijkstra.minCost(doneNode)))) {
                    doneNode = (short)i;
                }
            }
            if (doneNode == -1) {
                break;	/* すべてのノードが確定したら終了 */
            }
            dijkstra.setDoneFlag(doneNode, true);	// Enter start node

            System.out.printf("[%s]", StationName(Jct2id(doneNode + 1)));
            if (nLastNode == 0) {
                if ((doneNode + 1) == lastNode) {
                    break;	/* 着ノードが完了しても終了可 */
                }
            } else if (nLastNode == 1) {
                if ((doneNode + 1) == lastNode1) {
                    break;	/* 着ノードが完了しても終了可 */
                }
            } else if (nLastNode == 2) {
                if (dijkstra.doneFlag(lastNode1 - 1) &&
    				dijkstra.doneFlag(lastNode2 - 1)) {
                    break;	/* 着ノードが完了しても終了可 */
                }
            }

            List<Integer[]> nodes = Node_next(doneNode + 1, except_local);

            for (Integer[] node : nodes) {

                a = node[0] - 1;	// jctId

                if ((!routePassed.check(a + 1) /**/|| ((nLastNode == 0) && (lastNode == (a + 1))) ||
                        ((0 < nLastNode) && (lastNode1 == (a + 1))) ||
                        ((1 < nLastNode) && (lastNode2 == (a + 1)))) /**/ &&
                        ((((0x01 & useBulletTrain) != 0) || !IS_SHINKANSEN_LINE(node[2])) &&
                         (((0x02 & useBulletTrain) != 0) || !IS_COMPANY_LINE(node[2])))) {
                                 /* コメント化しても同じだが少し対象が減るので無駄な比較がなくなる */
					/* 新幹線でない */
                    cost = dijkstra.minCost(doneNode) + node[1]; // cost

                    // ノードtoはまだ訪れていないノード
                    // またはノードtoへより小さいコストの経路だったら
                    // ノードtoの最小コストを更新
                    if ((((dijkstra.minCost(a) < 0) || (cost <= dijkstra.minCost(a))) &&
    					((cost != dijkstra.minCost(a)) || IS_SHINKANSEN_LINE(node[2])))
                            &&
                            (!loopRoute ||
                                    ((((doneNode + 1) != excNode1) && ((doneNode + 1) != excNode2)) ||
                                            ((excNode1 != (a + 1)) && (excNode2 != (a + 1)))))) {
						/* ↑ 同一距離に2線ある場合新幹線を採用 */
                        dijkstra.setMinCost(a, cost);
    					dijkstra.setFromNode(a, doneNode + 1);
    					dijkstra.setLineId(a, node[2]);
                        System.out.printf( "+<%s(%s)>", StationName(Jct2id(a + 1)), LineName(dijkstra.lineId(a)));
                    } else {
                        System.out.printf("-<%s>", StationName(Jct2id(a + 1)));
                    }
                } else {
                    System.out.printf("x(%s)", StationName(Jct2id(a + 1)));
                }
            }
            System.out.println();
        }

        List<Integer> route = new ArrayList<>();
        short lineid = 0;
        short idb;

        if (lastNode == 0) { /* 終了駅は非分岐駅 ? */
            // 最後の分岐駅の決定：
            // 2つの最後の分岐駅候補(終了駅(非分岐駅）の両隣の分岐駅)～終了駅(非分岐駅)までの
            // 計算キロ＋2つの最後の分岐駅候補までの計算キロは、
            // どちらが短いか？
            if ((2 == nLastNode) &&
                    (!routePassed.check(lastNode2) && ((dijkstra.minCost(lastNode2 - 1) + lastNode2_distance) <
                      (dijkstra.minCost(lastNode1 - 1) + lastNode1_distance)))) {
                id = lastNode2;		// 短い方を最後の分岐駅とする
            } else {
                id = lastNode1;
            }
        } else {
            id = lastNode;
        }

        System.out.printf("Last target=%s, <-- %s(%d), (%d, %d, %d)\n", StationName(Jct2id(id)), StationName(Jct2id(dijkstra.fromNode(id - 1))), dijkstra.fromNode(id - 1), (int)lastNode, (int)lastNode1, (int)lastNode2);

        //fromNodeが全0で下のwhileループで永久ループに陥る
        if (dijkstra.fromNode(id - 1) == 0) {
            if ((lastNode == 0) &&
                    (((nLastNode == 2) && (lastNode2 == startNode)) ||
                            (lastNode1 == startNode))) {
	      		/* 品川―大森 */
                lid = LineIdFromStationId(end_station_id);
                if (0 < InStationOnLine(lid, stationId)) {
                    System.out.println( "short-cut route.###");
                    a = add(lid, /*stationId,*/ end_station_id);
//                    if (0 <= a) {
//                        route_list_cooked.clear();
//                    }
                    route_flag.jctsp_route_change = true;	/* route modified */
                    return a;	//>>>>>>>>>>>>>>>>>>>>>>>>>
                }
            }
            System.out.println( "can't lowcost route.-1002");
            return -1002;
        }
        //------------------------------------------
        // 発駅(=分岐駅)でなく最初の分岐駅(-1+1=0)でない間
        // 最後の分岐駅からfromをトレース >> route[]
        while ((id != startNode) && (0 < id)) {
            System.out.printf( "  %s, %s, %s.", LineName(lineid), LineName(dijkstra.lineId(id - 1)), StationName(Jct2id(id)));
            if (lineid != dijkstra.lineId(id - 1)) {
                if (IS_SHINKANSEN_LINE(lineid)) {
                    //System.out.printf("@@@@->%d\n", lineid);
	                /* 新幹線→並行在来線 */
                    int zline = GetHZLine(lineid, Jct2id(id));
                    for (idb = id; (idb != startNode) && (dijkstra.lineId(idb - 1) == zline);
                         idb = dijkstra.fromNode(idb - 1)) {
                        System.out.printf( "    ? %s %s/",  LineName(dijkstra.lineId(idb - 1)), StationName(Jct2id(idb)));
                    }
                    if (dijkstra.lineId(idb - 1) == lineid) { /* もとの新幹線に戻った ? */
                        //System.out.printf(".-.-.-");
                        id = idb;
                        continue;
                    } else if (idb == startNode) { /* 発駅？ */
                        if (zline == GetHZLine(lineid, Jct2id(idb))) {
                            id = idb;
                            continue;
                        }
						/* thru */
                        //System.out.printf("*-*-*-");
                    } else if (idb != id) { /* 他路線の乗り換えた ? */
                        System.out.printf( "%sはそうだが、%sにも新幹線停車するか?", StationName(Jct2id(id)), StationName(Jct2id(idb)));
                        if (zline == GetHZLine(lineid, Jct2id(idb))) {
                            id = idb;
                            continue;
                        }
						/* thru */
                        System.out.printf( "+-+-+-: %s(%s) : ", LineName(dijkstra.lineId(idb - 1)), LineName(lineid));
                    } else {
                        //System.out.printf("&");
                    }
                } else { /* 前回新幹線でないなら */
					/* thru */
                    //System.out.printf("-=-=-=");
                }
                // 次の新幹線でも並行在来線でもない路線への分岐駅に新幹線分岐駅でない場合(金山)最初の在来線切り替えを有効にする（三河安城）
                // 新幹線に戻ってきている場合(花巻→盛岡）、花巻まで（北上から）無効化にする
                //
                route.add(id - 1);
                lineid = dijkstra.lineId(id - 1);
                System.out.println( "  o");
            } else {
                System.out.println( "  x");
            }
            id = dijkstra.fromNode(id - 1);
        }

        //// 発駅=分岐駅

        System.out.printf( "----------[%s]------\n", StationName(Jct2id(id)));

        List<Integer> route_rev = new ArrayList<>(route.size());
        int bid = -1;
        for (int ritr = route.size() - 1; 0 <= ritr; ritr--) {
            System.out.printf("> %s %s\n", LineName(dijkstra.lineId(route.get(ritr))), StationName(Jct2id(route.get(ritr)+ 1)));
            if (0 < bid && IS_SHINKANSEN_LINE(dijkstra.lineId(bid))) {
                if (GetHZLine(dijkstra.lineId(bid), Jct2id(route.get(ritr) + 1)) == dijkstra.lineId(route.get(ritr))) {
                    dijkstra.setLineId(route.get(ritr), dijkstra.lineId(bid));	/* local line -> bullet train */
                    route_rev.remove(route_rev.size() - 1);
                }
            }
            route_rev.add(route.get(ritr));
            bid = route.get(ritr);
        }
		/* reverse(route_rev->route) */
        route = new ArrayList<>(route_rev);
        route_rev.clear();	/* release */

        if (lastNode == 0) {	// 着駅は非分岐駅?
            System.out.printf("last: %s\n", LineName(dijkstra.lineId(route.get(route.size() - 1))));
            lid = LineIdFromStationId(end_station_id); // 着駅所属路線ID
            // 最終分岐駅～着駅までの営業キロ、運賃計算キロを取得
            //km = Get_node_distance(lid, end_station_id, Jct2id(a));
            //km += minCost[route.get(route.size() - 1)];	// 最後の分岐駅までの累積計算キロを更新
            boolean isAbreastShinkansen = IsAbreastShinkansen(lid, dijkstra.lineId(route.get(route.size() - 1)),
                                                              Jct2id(route.get(route.size() - 1) + 1),
                                                              end_station_id);
            if ((lid == dijkstra.lineId(route.get(route.size() - 1))) ||
                ((0 < InStationOnLine(dijkstra.lineId(route.get(route.size() - 1)), end_station_id)) &&
    		     isAbreastShinkansen) ||
                (isAbreastShinkansen &&  (0 < InStation(end_station_id, lid, Jct2id(route.get(route.size() - 1) + 1), Jct2id(id))))) {
                route.remove(route.size() -1 );
                // if   着駅の最寄分岐駅の路線=最後の分岐駅?
                //      (渋谷-新宿-西国分寺-国立)
                // or   平行在来線の新幹線 #141002001
                // else 渋谷-新宿-三鷹
            }
        } else {
            lid = 0;
        }

        if ((1 < route_list_raw.size()) && (1 < route.size()) && (route_list_raw.get(route_list_raw.size() - 1).lineId == dijkstra.lineId(route.get(0)))) {
            System.out.printf("###return return!!!!!!!! back!!!!!! %s:%s#####\n", LineName(route_list_raw.get(route_list_raw.size() - 1).lineId), StationName(route_list_raw.get(route_list_raw.size() - 1).stationId));
            removeTail();
            ASSERT (0 < route_list_raw.size()); /* route_list_raw.size() は0か2以上 */
            //stationId = route_list_raw.get(route_list_raw.size() - 1).stationId;
        }

        a = 1;
        for (i = 0; i < (int)route.size(); i++) {
            System.out.printf("route[] add: %s\n", StationName(Jct2id(route.get(i) + 1)));
            a = add(dijkstra.lineId(route.get(i)), /*stationId,*/ Jct2id(route.get(i) + 1));
            route_flag.jctsp_route_change = true;	/* route modified */
            if ((a <= 0) || (a == 5)) {
                //ASSERT (false);
                System.out.printf("####%d##%d, %d##\n", a, i, route.size());
                if ((a < 0) || ((i + 1) < (int)route.size())) {
//                    route_list_cooked.clear();
                    return a;	/* error */
                } else {
                    break;
                }
            }
            //stationId = Jct2id(route[i] + 1);
        }

//        route_list_cooked.clear();

        if (lastNode == 0) {
            System.out.printf("fin last:%s\n", LineName(lid));
            ASSERT (0 < lid);
            if (a == 0) {
                // 立川 八王子 拝島 西国立
                return -1000;
            }
            a = add(lid, /*stationId,*/ end_station_id);
            route_flag.jctsp_route_change = true;	/* route modified */
            if ((a <= 0) || (a == 5)) {
                //ASSERT (0 == a);
                return a;
            }
        }
        return a;
    }


    /*  通過連絡運輸チェック
    *  param [in] line_id  路線id
    *
    *  @param [in] line_id  路線
    *  @param [in] 駅1
    *  @param [in] 駅2
    *  @param [in] num route_list.size()=経路数
    *  @retval 0 = continue
    *	@retval -4 = 会社線 通過連絡運輸なし
    */
    private int companyPassCheck(int line_id, int stationId1, int stationId2, int num) {

    	int rc;

        if (route_flag.compnda ||
        (IS_COMPANY_LINE(line_id) && route_flag.compnpass)) {
            return -4;      /* error x a c */
        }
        if (IS_COMPANY_LINE(line_id) 
        && (!route_flag.compncheck && !route_flag.compnpass)) {

    		route_flag.compnend  = true;	// if company_line

            /* pre block check d */
    		route_flag.compncheck  = true;
    		return preCompanyPassCheck(line_id, stationId1, stationId2, num);

        } else if (!IS_COMPANY_LINE(line_id) && route_flag.compncheck) {

            /* after block check e f */
            route_flag.compnterm = false;	// initialize
    		rc = postCompanyPassCheck(line_id, stationId1, stationId2, num);
    		if (0 <= rc) {
                route_flag.compnpass = true;
                route_flag.compnend = false;	// if company_line
                if (rc == 1) {
                    route_flag.tokai_shinkansen = true;
                } else if (rc == 3) {
                    if (STATION_IS_JUNCTION(stationId2)) {
                        route_flag.compnterm = true;
                    } else {
                        rc = -4;	/* 分岐駅ではない場合、”~続けて経路を指定してください"ができない*/
                    }
                } else {
                    rc = 0;
                }
    		}
    		return rc;

        } else if (IS_COMPANY_LINE(line_id)) {
    		/* b */
    		ASSERT(!route_flag.compnda);
    		ASSERT(route_flag.compncheck);
    		ASSERT(!route_flag.compnpass);
    		ASSERT(route_list_raw.get(route_list_raw.size() - 1).lineId != line_id);
    		//ASSERT(IS_COMPANY_LINE(route_list_raw.get(route_list_raw.size() - 1).lineId));

    		route_flag.compnend  = true;	// if company_line

    		if (route_flag.compnbegin) {
    			return 0;	/* 会社線始発はとりあえず許す！ */
    		}

    		/* 会社線乗継可否チェック(市振、目時、妙高高原、倶利伽羅) */
    		rc = CompanyConnectCheck(stationId1);
            if (rc == 0) {
                // route_list_raw.at(num - 2).stationId is 会社線
                // term1 jr_line1 term2 company_line2 term3 company_line3 term4 だから num - 2.stationIdはterm2
                return preCompanyPassCheck(line_id, route_list_raw.get(num - 2).stationId, stationId2, num);
            }
    		return rc;
    	} else {
            /* g h */
    		route_flag.compnend = false;	// if company_line
            return 0;
        }
    }


    /*!
     *	前段チェック 通過連絡運輸
     *	@param [in] station_id2  add(),追加予定駅
     *	@retval 0 : エラーなし(継続)
     *	@retval -4 : 通過連絡運輸禁止
     */
    static int CompanyConnectCheck(int station_id) {
    	final String tsql =
    	"select pass from t_compnconc where station_id=?";
    	Cursor dbo = RouteDB.db().rawQuery(tsql, new String[]{String.valueOf(station_id)});
    	int r = 0;	/* if disallow */
        try {
    		if (dbo.moveToNext()) {
    			r = dbo.getInt(0);
    		}
        } finally {
            dbo.close();
        }
        System.out.printf("CompanyConnectCheck: %s(%d)\n", StationName(station_id), r);
    	return r == 0 ? -4 : 0;
    }

    /*!
     *	前段チェック 通過連絡運輸
     *	@param [in] line_id      add(),追加予定路線
     *	@param [in] station_id1  add(),最後に追加した駅
     *	@param [in] station_id2  add(),追加予定駅
     *	@retval 0 : エラーなし(継続)
     *	@retval -4 : 通過連絡運輸禁止
     */
    int preCompanyPassCheck(int line_id, int station_id1, int station_id2, int num) {
    	CompnpassSet cs = new CompnpassSet();
    	int i;
    	int rc;

        ASSERT(IS_COMPANY_LINE(line_id));
    	System.out.printf("Enter preCompanyPassCheck(%s, %s %s %d)\n", LineName(line_id), StationName(station_id1), StationName(station_id2), num);
        System.out.printf("  key1=%s, key2=%s\n", StationName(station_id1), StationName(station_id2));
    	if (num <= 1) {
    		/* 会社線で始まる */
    		route_flag.compnbegin  = true;
    		return 0;
    	}
    	rc = cs.open(station_id1, station_id2);
    	if (rc <= 0) {
    		return 0;		/* Error or Non-record(always pass) as continue */
    	}
    	rc = 0;
    	for (i = 1; i < num; i++) {
            if (IS_COMPANY_LINE(route_list_raw.get(i).lineId)) {
                continue;
            }
            rc = cs.check((i == 1) ? -1 : 0,
                          route_list_raw.get(i).lineId,
    					  route_list_raw.get(i - 1).stationId,
    					  route_list_raw.get(i).stationId);
            System.out.printf("preCompanyPassCheck %d/%d->%d(%s:%s-%s)\n", i, num, rc, 
                LineName(route_list_raw.get(i).lineId), StationName(route_list_raw.get(i - 1).stationId), StationName(route_list_raw.get(i).stationId));
            if (rc < 0) {
    			break;	/* disallow */
            }
            if (rc == 1) {
                route_flag.tokai_shinkansen = true;
                rc = 0;
            } else if (rc == 3) {
                rc = -4;	// disallow pending through pre.
            } else if (rc == 2) {
                break;	// OK
            } else {
                ASSERT(rc == 0);
            }
    	}
        if (0 <= rc) {
            System.out.printf("preCompanyPassCheck always pass(%d)\n", rc);
            return 0;		/* Error or Non-record(always pass) as continue */
        } else {
            System.out.println("preCompanyPassCheck will fail");
    		route_flag.compnda  = true; /* 通過連絡運輸不正 */
    		return 0;	/* -4 受け入れて（追加して）から弾く */
    	}
    }

    /*	@brief 後段 通過連絡運輸チェック
     *	@param [in] line_id      add(),追加予定路線
     *	@param [in] station_id1  add(),最後に追加した駅
     *	@param [in] station_id2  add(),追加予定駅
     *	@retval 0 : エラーなし(継続)
     *  @retval 1 : エラーなし(継続)(JR東海新幹線)
     *	@retval 3 : エラー保留（発着駅指定）
     *	@retval -4 : 通過連絡運輸禁止
     */
    int postCompanyPassCheck(int line_id, int station_id1, int station_id2, int num) {
    	CompnpassSet cs = new CompnpassSet();
    	int rc;
    	int i;
    	int key1 = 0;
    	int key2 = 0;

    	System.out.printf("Enter postCompanyPassCheck(%s, %s %s %d)\n", LineName(line_id), StationName(station_id1), StationName(station_id2), num);

        do {
    // 1st後段チェック	ASSERT(IS_COMPANY_LINE(route_list_raw.back().lineId));
            ASSERT(!IS_COMPANY_LINE(line_id));

            for (i = num - 1; 0 < i; i--) {
                if ((key1 == 0) && IS_COMPANY_LINE(route_list_raw.get(i).lineId)) {
                    key1 = route_list_raw.get(i).stationId;
                } else if ((key1 != 0) && !IS_COMPANY_LINE(route_list_raw.get(i).lineId)) {
                    key2 = route_list_raw.get(i).stationId;
                    break;
                }
            }
            System.out.printf("  key1=%s, key2=%s\n", StationName(key1), StationName(key2));
            if (i <= 0) {
                route_flag.compnda  = true; /* 通過連絡運輸不正 */
                if (route_flag.compnbegin) {
                    rc = 0;	/* 会社線始発なら終了 */
                }
                else {
                    ASSERT(false);
                    rc = -4;    // return -4;
                }
                break;  // return 0 or -4
            }
            rc = cs.open(key1, key2);
            if (rc <= 0) {
                System.out.println("postCompanyPassCheck not found in db(pass)");
                rc = 0;
                break;  // return 0;		/* Error or Non-record(always pass) as continue */
            }
            rc = cs.check(1, line_id, station_id1, station_id2);
            if (rc < 0) {
    			// route_flag.compnda = true; /* 通過連絡運輸不正 */
                if (cs.is_terminal()) {
                    rc = 4;
                }
            }
        } while (false);
        System.out.printf("Leave postCompanyPassCheck(%d)\n", rc);
        return rc;	/* 0 / -4 */
    }

    static class CompnpassSet {
		static class recordset
		{
			int line_id;
			int stationId1;
			int stationId2;
		}
        recordset[] results;
		int max_record;
        int num_of_record;
        boolean terminal;

		CompnpassSet() {
            max_record = MAX_COMPNPASSSET;
			results = new recordset[max_record];
            for (int i = 0; i < results.length; i++) {
                results[i] = new recordset();
                // ウヘェ使いづれー言語
            }
			num_of_record = 0;
		}
        /*	会社線通過連連絡運輸テーブル取得
         *	@param [in] key1   駅１
         *	@param [in] key2   駅2
         *	@return  結果数を返す（0~N, -1 Error：レコード数がオーバー(あり得ないバグ)）
         */
		int open(int key1, int key2) {
            final String tsql =
        "select en_line_id, en_station_id1, en_station_id2, option" +
        " from t_compnpass" +
        " where station_id1=? and station_id2=?";
        	int i;
            int stationId1;
            int stationId2;
        	Cursor dbo = RouteDB.db().rawQuery(tsql, new String[] {String.valueOf(key1), String.valueOf(key2)});
            int rc = -1;
            try {
        		for (i = 0; dbo.moveToNext(); i++) {
        			if (max_record <= i) {
        				ASSERT(false);
        				return -1;		/* too many record */
        			}
        			results[i].line_id = dbo.getInt(0);
        			stationId1 = dbo.getInt(1);
        			stationId2 = dbo.getInt(2);
                    if (!terminal) {
                        terminal = 0 < dbo.getInt(3);
                    }                    results[i].stationId1 = stationId1;
                    results[i].stationId2 = stationId2;
                }
                num_of_record = i;
        		rc = i;	/* num of receord */
            } finally {
                dbo.close();
            }
    		return rc;	/* db error */
        }

        /*!
         *	@brief 通過連絡運輸チェック
         *
         *  @param [in] postcheck_flag plus is postCompnayPasscheck, minus is preCompanyPasscheck and leave, otherwise zero
         *	@param [in] line_id      add(),追加予定路線
         *	@param [in] station_id1  add(),最後に追加した駅
         *	@param [in] station_id2  add(),追加予定駅
         *	@retval 2 : 許可駅発着駅OK
         *	@retval 3 : 着駅有効(pending)
         *	@retval 0 : エラーなし(継続)
         *	@retval -4 : 通過連絡運輸禁止
         */
		int check(int postcheck_flag, int line_id, int station_id1, int station_id2) {
            int i;
            int rc = -4;
            int terminal_id;

        	if (num_of_record <= 0) {
        		return 0;
        	}
            if (0 < postcheck_flag) {
                // post
                terminal_id = station_id2;
            } else if (postcheck_flag < 0) {
                // pre
                terminal_id = station_id1;
            } else {
                // pre and throgh
                terminal_id = 0;
            }
            for (i = 0; i < num_of_record; i++) {
                if ((terminal_id != 0) && terminal
                        && (results[i].stationId1 != 0)) {
                    ASSERT(terminal_id != 0 && postcheck_flag != 0);
                    if (0 < RouteUtil.InStation(terminal_id, results[i].line_id, results[i].stationId1, results[i].stationId2)) {
                        System.out.printf("Company check OK(terminal) %s %s\n", 0 < postcheck_flag ? "arrive":"leave", StationName(terminal_id));
                        System.out.printf("    (%d/%d %s,%s-%s def= %s:%s-%s)\n", i, num_of_record,
                                LineName(line_id), StationName(station_id1), StationName(station_id2),
                                LineName(results[i].line_id), StationName(results[i].stationId1), StationName(results[i].stationId2));
                        return 2; 	// OK possible to pass
                    } else {
                        System.out.printf("Check company terminal notfound(%s)(%s:%s-%s) in %s:%s-%s)\n", StationName(terminal_id), LineName(line_id), StationName(station_id1), StationName(station_id2), LineName(results[i].line_id), StationName(results[i].stationId1), StationName(results[i].stationId2));
                    }
                }
        		if ((results[i].line_id & 0x80000000) != 0) {
        			/* company */
                    int company_mask = results[i].line_id;
                    int[] cid1 = CompanyIdFromStation(station_id1);
                    int[] cid2 = CompanyIdFromStation(station_id2);

                    if ((0 != (company_mask & (1 << cid1[0] | 1 << cid1[1])))
                     && (0 != (company_mask & (1 << cid2[0] | 1 << cid2[1])))) {
                        return 0;	/* OK possible pass */
                    }
                    /* JR東海なら新幹線を許す */
                    if (((company_mask & ((1 << JR_CENTRAL) | (1 << JR_EAST))) == (1 << JR_CENTRAL))
                      && (line_id == DbIdOf.INSTANCE.line("東海道新幹線"))) {
                        // 都区市内適用しないように (CheckOfRule86())
                        // b#20090901 tokai_shinkansen = true;
                        System.out.println("JR-tolai Company line");
                        return 1;       /* OK possible pass */
                    }
                } else if (results[i].line_id == line_id) {
                    System.out.printf("Company check begin(%d/%d %s,%s-%s def= %s:%s-%s)\n", i, num_of_record,
                            LineName(line_id), StationName(station_id1), StationName(station_id2),
                            LineName(results[i].line_id), StationName(results[i].stationId1), StationName(results[i].stationId2));
                    if ((results[i].stationId1 == 0) || (
                            (0 < RouteUtil.InStation(station_id1, line_id, results[i].stationId1, results[i].stationId2)) &&
                    (0 < RouteUtil.InStation(station_id2, line_id, results[i].stationId1, results[i].stationId2)))) {
                        System.out.printf("Company check OK(%s,%s in %s:%s-%s)\n", StationName(station_id1), StationName(station_id2), LineName(line_id), StationName(results[i].stationId1), StationName(results[i].stationId2));
                        return 0;	/* OK possible to pass */
                    }
                } else if (results[i].line_id == 0) {
                    System.out.printf("Company check NG(%s,%s-%s = %s:%s-%s)\n", LineName(line_id), StationName(station_id1), StationName(station_id2), LineName(results[i].line_id), StationName(results[i].stationId1), StationName(results[i].stationId2));
                    break;	/* can't possoble */
                }
            }
            if (terminal) {
                System.out.printf("Company check pending to terminal\n");
                return 3;	/* 会社線契約着駅まで保留 */
            }
            return rc;
        }
        boolean is_terminal() { return terminal; }

		private int en_line_id(int index) {
			return results[Limit(index, 0, MAX_COMPNPASSSET - 1)].line_id;
		}
		private int en_station_id1(int index) {
			return results[Limit(index, 0, MAX_COMPNPASSSET - 1)].stationId1;
		}
		private int en_station_id2(int index) {
			return results[Limit(index, 0, MAX_COMPNPASSSET - 1)].stationId2;
		}

	} // CompnpassSet



    //static
    // 駅ID(分岐駅)の最寄りの分岐駅を得る(全路線）
    // [jct_id, calc_km, line_id][N] = f(jct_id)
    //
    //	@param [in] jctId   分岐駅
    //  @param except_local True: 地方交通線を除外
    //	@return 分岐駅[0]、計算キロ[1]、路線[2]
    //
    private static List<Integer[]> Node_next(int jctId, boolean except_local) {
        final String tsql =
                "select case jct_id when ?1 then neer_id else jct_id end as node, cost, line_id, attr" +
                        " from t_node" +
                        " where jct_id=?1 or neer_id=?1 order by node";

        List<Integer[]> result = new ArrayList<Integer[]>();

        Cursor dbo = RouteDB.db().rawQuery(tsql, new String[] {String.valueOf(jctId)});
        try {
            while (dbo.moveToNext()) {
                if (!except_local || (dbo.getInt(3) != 2)) {
                    Integer[] r1 = new Integer[3];
                    r1[0] = dbo.getInt(0);    // jct_id
                    r1[1] = dbo.getInt(1);    // cost(calc_km)
                    r1[2] = dbo.getInt(2);    // line_id
                    result.add(r1);
                }
            }
        } finally {
            dbo.close();
        }
        return result;
    }

    // static
    //	駅ID→分岐ID
    //
    private static short Id2jctId(int stationId) {
        Cursor ctx = RouteDB.db().rawQuery(
                "select id from t_jct where station_id=?", new String[] {String.valueOf(stationId)});
        short rc = 0; // if error
        try {
            if (ctx.moveToNext()) {
                rc = ctx.getShort(0);
            }
        } finally {
            ctx.close();
        }
        return rc;
    }
    // static
    //	分岐ID→駅ID
    //
    private static short Jct2id(int jctId)	{
        Cursor ctx = RouteDB.db().rawQuery(
                "select station_id from t_jct where rowid=?", new String[] { String.valueOf(jctId)});
        short rc = 0;   // if error
        try {
            if (ctx.moveToNext()) {
                rc = ctx.getShort(0);
            }
        } finally {
            ctx.close();
        }
        return rc;
    }



    //static
    //	分岐ID→駅名
    //
    private static String JctName(int jctId)	{
        String name = "";		//[MAX_STATION_CHR];

        Cursor ctx = RouteDB.db().rawQuery(
                "select name from t_jct j left join t_station t on j.station_id=t.rowid where id=?",
                new String[] {String.valueOf(jctId)});
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
    //	駅は路線内にあるか否か？
    //  @param [in] line_id    路線ID
    //  @param [in] station_id 駅ID
    //  @param [in] flag　     true: set follow flag / false: no-flag(default)
    //  注： lflg&(1<<17)を含めていないため、新幹線内分岐駅、たとえば、
    //       東海道新幹線 京都 米原間に草津駅は存在するとして返します.    //
    private static int InStationOnLine(int line_id, int station_id)	{
        return InStationOnLine(line_id, station_id, false);
    }
    private static int InStationOnLine(int line_id, int station_id, boolean flag)	{
        Cursor ctx = RouteDB.db().rawQuery(
                //		"select count(*) from t_lines where line_id=?1 and station_id=?2");
                flag ? "select count(*) from t_lines where (lflg&(1<<31))=0 and line_id=?1 and station_id=?2" :
                       "select count(*) from t_lines where (lflg&((1<<31)|(1<<17)))=0 and line_id=?1 and station_id=?2",
                new String[] {String.valueOf(line_id), String.valueOf(station_id)});
        int rc = 0;
        try {
            if (ctx.moveToNext()) {
                rc = ctx.getInt(0);
            }
        } finally {
            ctx.close();
        }
        return rc;
    }


    // 駅(station_id)の所属する路線IDを得る.
    // 駅は非分岐駅で, 路線は1つしかないと解っていることを前提とする.
    //
    // line_id = f(station_id)
    //
    private static short LineIdFromStationId(int station_id) {
        Cursor ctx = RouteDB.db().rawQuery(
                "select line_id" +
                        " from t_lines" +
                        " where station_id=?" +
                        " and 0=(lflg&((1<<31)|(1<<17)))", new String[] {String.valueOf(station_id)});
        short rc = 0;
        try {
            if (ctx.moveToNext()) {
                rc = ctx.getShort(0);
            }
        } finally {
            ctx.close();
        }
        return rc;
    }


    // 駅(station_id)の所属する路線IDを得る.
    // 駅は非分岐駅で, 路線は1つしかないと解っていることを前提とする.
    //
    // line_id = f(station_id)
    //
    private static short LineIdFromStationId2(int station_id1, int station_id2) {
        Cursor ctx = RouteDB.db().rawQuery(
                "select line_id from t_lines where station_id=?1 and 0=(lflg&((1<<31)|(1<<17))) and line_id in " +
                        "(select line_id from t_lines where station_id=?2 and 0=(lflg&((1<<31)|(1<<17))))",
                new String[] {String.valueOf(station_id1), String.valueOf(station_id2)});
        short rc = 0;
        try {
            if (ctx.moveToNext()) {
                rc =  ctx.getShort(0);
            }
        } finally {
            ctx.close();
        }
        return rc;
    }


    //static
    //	二つの駅は、同一ノード内にあるか
    //
    //	@param [in] line_id    line
    //	@param [in] station_id1  station_id 1
    //	@param [in] station_id2  station_id 2
    //	@return true : 同一ノード
    //	@return fase : 同一ノードではない
    //
    private static boolean IsSameNode(int line_id, int station_id1, int station_id2) {
        final String tsql =
                " select count(*)" +
                        " from t_lines" +
                        " where line_id=?1" +
                        " and (lflg&((1<<31)|(1<<17)|(1<<15)))=(1<<15)" +
                        " and sales_km>" +
                        " 		(select min(sales_km)" +
                        " 		from t_lines" +
                        " 		where line_id=?1" +
                        " 		and (station_id=?2 or" +
                        " 			 station_id=?3))" +
                        " and sales_km<" +
                        " 		(select max(sales_km)" +
                        " 		from t_lines" +
                        " 		where line_id=?1" +
                        " 		and (station_id=?2 or" +
                        " 			 station_id=?3))";

        Cursor dbo = RouteDB.db().rawQuery(tsql, new String[] {String.valueOf(line_id),
                String.valueOf(station_id1),
                String.valueOf(station_id2)});
        boolean same = false;
        try {
            if (dbo.moveToNext()) {
                same = dbo.getInt(0) <= 0;
            }
        } finally {
            dbo.close();
        }
        return same;
    }

    //static
    //	路線の駅1から駅2方向の最初の分岐駅
    //  (neerestで使おうと思ったが不要になった-残念)
    //	@param [in] line_id    line
    //	@param [in] station_id1  station_id 1(from)
    //	@param [in] station_id2  station_id 2(to)
    //	@return station_id : first function(contains to station_id1 or station_id2)
    //
    private static int NeerJunction(int line_id, int station_id1, int station_id2) {
        final String tsql =
                "select station_id from t_lines where line_id=?1 and" +
                        " case when" +
                        " (select sales_km from t_lines where line_id=?1 and station_id=?3)<" +
                        " (select sales_km from t_lines where line_id=?1 and station_id=?2) then" +
                        " sales_km=(select max(sales_km) from t_lines where line_id=?1 and (lflg&((1<<17)|(1<<31)))=0 and" +
                        " sales_km<=(select sales_km from t_lines where line_id=?1 and station_id=?2) and (lflg&((1<<17)|(1<<31)|(1<<15)))=(1<<15))" +
                        " else" +
                        " sales_km=(select min(sales_km) from t_lines where line_id=?1 and (lflg&((1<<17)|(1<<31)))=0 and " +
                        " sales_km>=(select sales_km from t_lines where line_id=?1 and station_id=?3) and (lflg&((1<<17)|(1<<31)|(1<<15)))=(1<<15))" +
                        " end;";

        Cursor dbo = RouteDB.db().rawQuery(tsql, new String[] {String.valueOf(line_id),
                String.valueOf(station_id1),
                String.valueOf(station_id2)});
        int stationId = 0;
        try {
            if (dbo.moveToNext()) {
                stationId = dbo.getInt(0);
            }
        } finally {
            dbo.close();
        }
        return stationId;
    }

    //static
    //	運賃計算キロを返す(最短距離算出用)
    //
    //	@param [in] line_id     路線
    //	@param [in] station_id1 発
    //	@param [in] station_id2 至
    //
    //	@retuen 営業キロ or 計算キロ
    //
    private static int Get_node_distance(int line_id, int station_id1, int station_id2)	{
        final String tsql =
                "select case when sum(calc_km)=0 then max(sales_km)-min(sales_km) else max(calc_km)-min(calc_km) end" +
                        " from	t_lines" +
                        " where line_id=?1 and (lflg&(1<<31))=0 and	(station_id=?2 or station_id=?3)";

        /*	// 140416
         "select case when sum(calc_km)=0 then max(sales_km)-min(sales_km) else max(calc_km)-min(calc_km) end"
         " from	t_lines"
         " where line_id=?1"
         " and	(lflg&(1<<31))=0"
         " and	sales_km>=(select min(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))"
         " and	sales_km<=(select max(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))";

         /* old // if 0
         "select case when (l1.calc_km-l2.calc_km)=0 then l1.sales_km - l2.sales_km else  l1.calc_km - l2.calc_km end"
         " from t_lines l1"
         " left join t_lines l2"
         " where l1.line_id=?1"
         " and l2.line_id=?1"
         " and l1.sales_km>l2.sales_km"
         " and ((l1.station_id=?2 and l2.station_id=?3)"
         " or (l1.station_id=?3 and l2.station_id=?2))";
         ****/
        Cursor dbo = RouteDB.db().rawQuery(tsql, new String[] {String.valueOf(line_id),
                String.valueOf(station_id1),
                String.valueOf(station_id2)});
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
    //	路線は新幹線の在来線か?
    //
    //	@param [in] line_id1		比較元路線(在来線)
    //	@param [in] line_id2		路線(新幹線)
    //	@param [in] station_id1		駅1 (発) 在来線接続駅であること
    //	@param [in] station_id2		駅2 (着) 在来線接続駅でなくても可
    //
    //	@return	true 並行在来線
    //
    static boolean IsAbreastShinkansen(int line_id1, int line_id2, int station_id1, int station_id2) {
        int i;
        int w;

        if (!IS_SHINKANSEN_LINE(line_id2)) {
            return false;
        }
        List<Integer> hzl = EnumHZLine(line_id2, station_id1, station_id2);
        if (hzl.size() < 3) {
            if (hzl.size() < 1) {
                ASSERT (false);
            } else {
                if (hzl.get(0) == (int)line_id1) {
                    return 0 < InStationOnLine(line_id1, station_id2);
                }
            }
            return false;
        }
        for (i = 0; i < (int)hzl.size(); i++) {
            if (0x10000 < hzl.get(i)) {
                w = 0xffff & hzl.get(i);
            } else {
                w = hzl.get(i);
            }
            if (w == line_id1) {
                return true;
            } else if (0 != w) {
                return false;
            }
        }
        return false;
    }




    //static
    //	新幹線乗換可否のチェック
    //	add() =>
    //
    //	@param [in] line_id1  前路線
    //	@param [in] line_id2  今路線
    //	@param [in] station_id1  前回接続(発)駅
    //	@param [in] station_id2  接続駅
    //	@param [in] station_id3  着駅
    //
    // @retval 0: N/A(OK)
    // @retval -1: NG
    // @retval 1<: 新幹線接続駅の（乗ってきた、乗る予定の）次の在来線接続駅
    //
    //                 国府津 s1                東京
    // l1 東海道線     小田原 s2 東海道新幹線   静岡
    // l2 東海道新幹線 名古屋 s3 東海道線       草薙
    //
    private static int CheckTransferShinkansen(int line_id1, int line_id2, int station_id1, int station_id2, int station_id3) {
        int bullet_line;
        int local_line;
        LINE_DIR dir;
        int hzl;
        int flgbit;
        int opposite_bullet_station;

        if (IS_SHINKANSEN_LINE(line_id2)) {
            bullet_line = line_id2;		// 在来線->新幹線乗換
            local_line = line_id1;
            opposite_bullet_station = station_id3;
            hzl = GetHZLine(bullet_line, station_id2, station_id3);

        } else if (IS_SHINKANSEN_LINE(line_id1)) {
            bullet_line = line_id1;		// 新幹線->在来線乗換
            local_line = line_id2;
            opposite_bullet_station = station_id1;
            hzl = GetHZLine(bullet_line, station_id2, station_id1);

        } else {
            return 0;				// それ以外は対象外
        }
        if (local_line != hzl) {
            return 0;
        }

        // table.A
        dir = DirLine(line_id1, station_id1, station_id2);
        if (dir == DirLine(line_id2, station_id2, station_id3)) {
            return 0;		// 上り→上り or 下り→下り
        }
        if (dir == LINE_DIR.LDIR_ASC) {	// 下り→上り
            flgbit = 0x01;
        } else {        		                    // 上り→下り
            flgbit = 0x02;
        }

        if (((AttrOfStationOnLineLine(local_line, station_id2) >>> BSRSHINKTRSALW) & flgbit) != 0) {
            int chk_station = RouteUtil.NextShinkansenTransferTerm(bullet_line, station_id2, opposite_bullet_station);
            System.out.printf("shinzai: %s -> %s, %s(%d)\n", RouteUtil.StationName(station_id2), RouteUtil.StationName(opposite_bullet_station), RouteUtil.StationName(chk_station), chk_station);
            ASSERT(0 < chk_station);
            return chk_station;
        } else {
            return -1;
        }
    }

    public boolean isOsakakanDetourEnable() {
        return route_flag.is_osakakan_1pass();
    }

    public boolean isOsakakanDetour() {
        return route_flag.osakakan_detour;
    }


/*
//	経路内を着駅で終了する(以降の経路はキャンセル)
//
//	@param [in] stationId   駅ident
//
void terminate(int stationId) {
    int i;
    int stationIdFrom = 0;
    int newLastIndex = 0x7fffffff;
    int line_id = -1;
    //int stationIdTo;

    for (i = 0; i < route_list_raw.size(); i++) {
        if (stationIdFrom != 0) {
            // stationIdはroute_list_raw[i].lineId内のstationIdFromから
            //              route_list_raw[i].stationIdの間にあるか?
            if (0 != InStation(stationId, route_list_raw.get(i).lineId, stationIdFrom, route_list_raw.get(i).stationId)) {
                newLastIndex = i;
                line_id = route_list_raw.get(i).lineId;
                //stationIdTo = route_list_raw[i].stationId;
                break;
            }
        } else {
            ASSERT (i == 0);
        }
        stationIdFrom = route_list_raw.get(i).stationId;
    }
    if (newLastIndex < route_list_raw.size()) {
        while (newLastIndex < route_list_raw.size()) {
            removeTail();
        }
        ASSERT ((newLastIndex<=1) || route_list_raw.get(newLastIndex - 1).stationId == stationIdFrom);
        add(line_id, stationId);
        end_station_id = (short)stationId;
    }
}


*/
} // Route
