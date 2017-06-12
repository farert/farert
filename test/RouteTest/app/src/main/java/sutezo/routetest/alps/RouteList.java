package sutezo.routetest.alps;

//package Route;

import java.util.*;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.database.sqlite.SQLiteOpenHelper;


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


public class RouteList {

    final static int BSRJCTSP	 = 31;		// [w]分岐特例
    final static int BSRJCTHORD = 31;		// [w]水平型検知フラグ

    final static int BSRNOTYET_NA	= 30;		// [w]不完全ルート
    final static int BSRJCTSP_B	= 29;		// [w]分岐特例B

    final static int BSR69TERM	= 24;    // [r]
    final static int BSR69CONT	= 23;    // [r]
    final static int BSRJCT	= 15;       // [r]
    final static int BSRBORDER	= 16;    // [r]

    final static int JCTSP	= 1;
    final static int JCTSP_2L = 2;
    final static int JCTSP_B_NISHIKOKURA = 3;
    final static int JCTSP_B_YOSHIZUKA = 4;
    final static int JCTSP_B_NAGAOKA = 5;

    /* last_flag */
    final static int LASTFLG_OFF = 0;   // all bit clear at removeAll()

    // bit0-1
    final static int MLF_OSAKAKAN_PASS = 0x03;
    final static int LF_OSAKAKAN_NOPASS = 0;	// 初期状態(大阪環状線未通過)
    final static int LF_OSAKAKAN_1PASS = 1;	// 大阪環状線 1回通過
    final static int LF_OSAKAKAN_2PASS = 2;	// 大阪環状線 2回通過
    final static int BLF_OSAKAKAN_1PASS = 0;	// 大阪環状線 1回通過

    static boolean IS_LF_OSAKAKAN_PASS(int m, int pass) {
        return (pass == ((m) & MLF_OSAKAKAN_PASS));
    }

    // bit 2-3
    final static int BLF_OSAKAKAN_1DIR = 2;	// 大阪環状線 1回目方向
    final static int BLF_OSAKAKAN_2DIR = 3;	// 大阪環状線 2回目方向

    final static int BLF_OSAKAKAN_DETOUR = 4;   // 大阪環状線 1回目遠回り(UI側から指定, 内部はR/O)

    final static int LF_OSAKAKAN_MASK = 0x0f;

    final static int BLF_TRACKMARKCTL = 5;	// 次にremoveTailでlastItemの通過マスクをOffする(typeOでもPでもないので)
    final static int BLF_JCTSP_ROUTE_CHANGE = 6;	// 分岐特例(add内部使用)

    // bit7 - 大高-杉本町とかで、[名][阪]をどっちに適用するか
    final static int BLF_MEIHANCITYFLAG = 7;	// ON: APPLIED_START / OFF:APPLIED_TERMINAL
    final static int BLF_NO_RULE = 8;   // ON: 特例非適用(User->System)
    final static int BLF_RULE_EN = 9;   // ON: 特例適用(System->User)

    // bit 10 JR東日本管内のJR東海
    final static int BLF_JREAST_IN_TOKAI = 10;

    // bit 11-20 発着 都区市内 適用
    final static int BLF_TER_BEGIN_CITY		= 11;		// [区][浜][名][京][阪][神][広][九][福][仙][札]
    final static int BLF_TER_FIN_CITY		= 12;
    final static int BLF_TER_BEGIN_YAMATE	= 13;		// [山]
    final static int BLF_TER_FIN_YAMATE		= 14;
    final static int BLF_TER_BEGIN_2CITY	= 15;		// not used
    final static int BLF_TER_FIN_2CITY		= 16;	  // not used
    final static int BLF_TER_BEGIN_CITY_OFF	= 17;
    final static int BLF_TER_FIN_CITY_OFF	= 18;
    final static int BLF_TER_BEGIN_OOSAKA	= 19;	// 大阪・新大阪
    final static int BLF_TER_FIN_OOSAKA		= 20;
    final static int LF_TER_CITY_MASK  =  (0x3ff << BLF_TER_BEGIN_CITY);	// 11-20bit

    // 会社線
    final static int BLF_COMPNCHECK		= 21;		// 会社線通過チェック有効
    final static int BLF_COMPNPASS		= 22;		// 通過連絡運輸
    final static int BLF_COMPNDA		= 23;		// 通過連絡運輸不正フラグ
    final static int BLF_COMPNBEGIN		= 24;		// 会社線で開始
    final static int BLF_COMPNEND		= 25;		// 会社線で終了

    final static int BLF_END			= 30;	// arrive to end.
    // end of last_flag



    List<RouteItem> route_list_raw = new ArrayList<RouteItem>(0);

    int last_flag;	// add() - removeTail() work

    public RouteList() {}
    public RouteList(RouteList route_list) {

    }
    public void assign(RouteList source_route) {
        this.assign(source_route, -1);
    }
    public void assign(RouteList source_route, int count) {
        route_list_raw = dupRouteItems(source_route.route_list_raw, count);
        last_flag = source_route.last_flag;
    }
    static List<RouteItem> dupRouteItems(final List<RouteItem> source) {
        return dupRouteItems(source, -1);
    }

    static List<RouteItem> dupRouteItems(final List<RouteItem> source, int count) {
        List<RouteItem> dest = new ArrayList<RouteItem>(source.size());
        int n = 0;
        for (RouteItem ri : source) {
            ++n;
            if ((0 < count) && (count < n)) {
                break;
            }
            dest.add(ri.clone());
        }
        return dest;
    }

    static void cpyRouteItems(final List<RouteItem> source, List<RouteItem> dest) {
        cpyRouteItems(source, dest, -1);
    }
    static void cpyRouteItems(final List<RouteItem> source, List<RouteItem> dest, int count) {
        int n = 0;
        dest.clear();
        for (RouteItem ri : source) {
            ++n;
            if ((0 < count) && (count < n)) {
                break;
            }
            dest.add(ri.clone());
        }
    }

    //public:
    public int startStationId() {
        return (route_list_raw.size() <= 0) ? 0 : route_list_raw.get(0).stationId;
    }

    public int endStationId() {
        return (route_list_raw.size() <= 0) ? 0 : route_list_raw.get(route_list_raw.size() - 1).stationId;
    }

    List<RouteItem> routeList() { return route_list_raw; }

    public int getLastFlag() { return last_flag; }
    public int sync_flag(RouteList source_route) {
        last_flag = source_route.getLastFlag();
        return last_flag;
    }

    boolean isModified() {
        return (last_flag & (1 << BLF_JCTSP_ROUTE_CHANGE)) != 0;
    }
    boolean is_end() { return RouteUtil.BIT_CHK(last_flag, BLF_END); }

    /*	ルート表示
	 *	@return ルート文字列
	 *
	 *	@remark showFare()の呼び出し後にのみ有効
	 */
    public String route_script() {
        String result_str;
        boolean oskk_flag;

        if (route_list_raw.size() == 0) {	/* 経路なし(AutoRoute) */
            return "";
        }

        result_str = RouteUtil.StationNameEx(route_list_raw.get(0).stationId);

        oskk_flag = false;
        for (RouteItem ri : route_list_raw) {
            result_str += ",";
            if (!oskk_flag && (ri.lineId == DbidOf.id().LineIdOf_OOSAKAKANJYOUSEN)) {
                if (RouteUtil.BIT_CHK(last_flag, BLF_OSAKAKAN_DETOUR)) {
                    result_str += "r";
                }
                oskk_flag = true;
            }
            result_str += RouteUtil.LineName(ri.lineId);
            result_str += ",";
            result_str += RouteUtil.StationNameEx(ri.stationId);
        }
        //	result_str += "\n\n";
        return result_str;
    }

    //	駅が経路内に含まれているか？
    //
    //	@param [in] stationId   駅ident
    //	@retval true found
    //	@retval false notfound
    //
    boolean checkPassStation(int stationId) {
        int stationIdFrom = 0;

        for (RouteItem route_item : route_list_raw) {
            if (stationIdFrom != 0) {
                if (0 != RouteUtil.InStation(stationId, route_item.lineId, stationIdFrom, route_item.stationId)) {
                    return true;
                }
            } else {
                RouteUtil.ASSERT (route_item == route_list_raw.get(0));
                RouteUtil.ASSERT (startStationId() == route_item.stationId);
            }
            stationIdFrom = route_item.stationId;
        }
        return false;
    }

    //public:
    //	運賃計算オプションを設定
    //	@param [in]  cooked : bit15=0/1  = 規則適用/非適用
    //						: bit 0
    //							= 1: 発・着が特定都区市内で発-着が100/200km以下のとき、着のみ都区市内有効
    //							= 0: 発・着が特定都区市内で発-着が100/200km以下のとき、発のみ都区市内有効
    //						x : bit 1
    //						x	= 1: 大阪環状線遠回り
    //						x	= 0: 大阪環状線近回り(規定)
    //                      x- shoud use setDetour()
    //
    //  @param [in] availbit : 有効フラグ
    //			    RouteUtil.FAREOPT_AVAIL_OSAKAKAN_DETOUR, RouteUtil.FAREOPT_AVAIL_APPLIED_START_TERMINAL, RouteUtil.FAREOPT_AVAIL_RULE_APPLIED
    //
    void setFareOption(int cooked, int availbit) {
        int opt = getFareOption();

		/* 規則適用 */
        if (0 != (RouteUtil.FAREOPT_AVAIL_RULE_APPLIED & availbit)) {
            if (RouteUtil.FAREOPT_RULE_NO_APPLIED == (RouteUtil.FAREOPT_RULE_NO_APPLIED & cooked)) {
                last_flag = RouteUtil.BIT_ON(last_flag,  BLF_NO_RULE);    /* 非適用 */
            } else {
                last_flag = RouteUtil.BIT_OFF(last_flag,  BLF_NO_RULE);   /* 適用 */
            }
        }

		/* 名古屋市内[名] - 大阪市内[阪] 発駅を単駅とするか着駅を単駅とするか */
        if (0 != (RouteUtil.FAREOPT_AVAIL_APPLIED_START_TERMINAL & availbit)) {
            if (((opt & 3) == 1) || ((opt & 3) == 2)) {
                if (RouteUtil.IS_MAIHAN_CITY_START(cooked)) {
                    last_flag = RouteUtil.BIT_ON(last_flag, BLF_MEIHANCITYFLAG);  /* 着駅=単駅、発駅市内駅 */
                } else {
                    last_flag = RouteUtil.BIT_OFF(last_flag, BLF_MEIHANCITYFLAG);	/* 発駅=単駅、着駅市内駅 */
                }
            } else {
                RouteUtil.ASSERT (false);
                return;
            }
        }
    }

    //public:
    //	運賃計算オプションを得る
    //	@return
    //     & 0xc0 = 0    : usualy
    //     & 0xc0 = 0x80 : empty or non calc.
    //	   & 0xc0 = 0x40 : Start Station only
    //	   & 0xc0 = 0xc0 : Calc error.
    //
    //     & 0x03 = 0 : 無し(通常)(発・着が特定都区市内駅で特定都区市内間が100/200km以下ではない)
    //			 (以下、発・着が特定都区市内駅で特定都区市内間が100/200kmを越える)
    //	   & 0x03 = 0x01 : 結果表示状態は{特定都区市内 -> 単駅} (「発駅を単駅に指定」と表示)
    //	   & 0x03 =	0x02 : 結果表示状態は{単駅 -> 特定都区市内} (「着駅を単駅に指定」と表示)
    //     & 0x0c = 0x04 : 大阪環状線1回通過(近回り)(規定)
    //     & 0x0c = 0x08 : 大阪環状線1回通過(遠回り)
    //     & 0x30 = 0x10 : 特例あり;特例適用
    //     & 0x30 = 0x20 : 特例なし:特例非適用
    //
    int getFareOption()
    {
    	int rc;
    	int c = route_list_raw.size();

    	if (c == 1) {
    		rc = 0x40; 	/* start only*/
    	}
    	else if (c <= 0) {
    		rc = 0x80;	/* empty */
    	}
    	else {
    		rc = 0;
    	}

    	// 大阪環状線 1回通過で近回り時 bit 2-3
    	if (IS_LF_OSAKAKAN_PASS(last_flag, LF_OSAKAKAN_1PASS)) {
    		if (RouteUtil.BIT_CHK(last_flag, BLF_OSAKAKAN_DETOUR)) {
    			rc |= 0x08;
    		}
    		else {
    			rc |= 0x04;
    		}
    	}
    	// 特例有無 bit 4-5
    	if (RouteUtil.BIT_CHK(last_flag, BLF_RULE_EN)) {
    		if (RouteUtil.BIT_CHK(last_flag, BLF_NO_RULE)) {
    			rc |= 0x30;     //
    		}
    		else {
    			rc |= 0x10;     // default: Applied
    		}
    	}
    	return rc;
    }
}
