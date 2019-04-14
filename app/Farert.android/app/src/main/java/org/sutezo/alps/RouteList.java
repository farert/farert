package org.sutezo.alps;

//package Route;

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


public class RouteList {

    final static int BSRJCTSP	 = 31;		// [w]分岐特例
    final static int BSRJCTHORD = 31;		// [w]水平型検知フラグ

    final static int BSRNOTYET_NA	= 30;		// [w]不完全ルート
    final static int BSRJCTSP_B	= 29;		// [w]分岐特例B
    final static int BSRSHINZAIREV	= 28;		// [w]新幹線-在来線折り返し

    final static int BSR69TERM	= 24;    // [r]
    final static int BSR69CONT	= 23;    // [r]
    final static int BSRJCT	= 15;       // [r]
    final static int BSRBORDER	= 16;    // [r]

    final static int JCTSP	= 1;
    final static int JCTSP_2L = 2;
    final static int JCTSP_B_NISHIKOKURA = 3;
    final static int JCTSP_B_YOSHIZUKA = 4;
    final static int JCTSP_B_NAGAOKA = 5;

    // last_flag

    List<RouteItem> route_list_raw = new ArrayList<RouteItem>(0);

    LastFlag last_flag = new LastFlag();	// add() - removeTail() work

    public RouteList() {}
    public RouteList(RouteList route_list) {

    }
    public void assign(RouteList source_route) {
        this.assign(source_route, -1);
    }

    public void assign(RouteList source_route, int count) {
        route_list_raw = dupRouteItems(source_route.route_list_raw, count);
        last_flag = source_route.last_flag.clone();
        if ((0 < count) && source_route.route_list_raw.size() != count) {
            last_flag.end = false;
            last_flag.compnda = false;
        }
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
    public int getCount() { return route_list_raw.size();   }
    public RouteItem item(int index) { return routeList().get(index); }

    public LastFlag getLastFlag() { return last_flag; }
    public LastFlag sync_flag(RouteList source_route) {
        last_flag = source_route.getLastFlag().clone();
        return last_flag;
    }

    boolean isModified() {
        return last_flag.jctsp_route_change;
    }
    boolean is_end() { return last_flag.end; }

    /*	ルート表示
	 *	@return ルート文字列
	 *
	 *	@remark showFare()の呼び出し後にのみ有効
	 */
    public String route_script() {
        StringBuffer result_str = new StringBuffer();
        boolean oskk_flag;
        int i;

        if (route_list_raw.size() == 0) {	/* 経路なし(AutoRoute) */
            return "";
        }

        if (last_flag.notsamekokurahakatashinzai) {
    		result_str.append(RouteUtil.TITLE_NOTSAMEKOKURAHAKATASHINZAI);
    	}
        result_str.append(RouteUtil.StationNameEx(route_list_raw.get(0).stationId));

        oskk_flag = false;
        i = 0;
        for (RouteItem ri : route_list_raw) {
            i++;
            if (1 < i) {
                result_str.append(",");
                if (!oskk_flag && (ri.lineId == DbIdOf.INSTANCE.line("大阪環状線"))) {
                    if (last_flag.osakakan_detour) {
                        result_str.append("r");
                    }
                    oskk_flag = true;
                }
                result_str.append(RouteUtil.LineName(ri.lineId));
                result_str.append(",");
                result_str.append(RouteUtil.StationNameEx(ri.stationId));
            }
        }
        //	result_str += "\n\n";
        return result_str.toString();
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
                ASSERT (route_item == route_list_raw.get(0));
                ASSERT (startStationId() == route_item.stationId);
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
    public void setFareOption(int cooked, int availbit) {
        int opt = getFareOption();

		/* 規則適用 */
        if (0 != (RouteUtil.FAREOPT_AVAIL_RULE_APPLIED & availbit)) {
            if (RouteUtil.FAREOPT_RULE_NO_APPLIED == (RouteUtil.FAREOPT_RULE_NO_APPLIED & cooked)) {
                last_flag.no_rule = true;    /* 非適用 */
            } else {
                last_flag.no_rule = false;   /* 適用 */
            }
        }

		/* 名古屋市内[名] - 大阪市内[阪] 発駅を単駅とするか着駅を単駅とするか */
        if (0 != (RouteUtil.FAREOPT_AVAIL_APPLIED_START_TERMINAL & availbit)) {
            if (((opt & 3) == 1) || ((opt & 3) == 2)) {
                if (RouteUtil.IS_MAIHAN_CITY_START(cooked)) {
                    last_flag.meihancityflag = true;  /* 着駅=単駅、発駅市内駅 */
                } else {
                    last_flag.meihancityflag = false;	/* 発駅=単駅、着駅市内駅 */
                }
            } else {
                // ASSERT (false);
                return;
            }
        }
        /* JR東海株主適用有無 */
	    if ((0 != (RouteUtil.FAREOPT_AVAIL_APPLIED_JRTOKAI_STOCK & availbit))) {
		    if (RouteUtil.FAREOPT_JRTOKAI_STOCK_APPLIED == (RouteUtil.FAREOPT_JRTOKAI_STOCK_APPLIED & cooked)) {
			    last_flag.jrtokaistock_applied = true;    /* 適用 */
		    } else {
		  	    last_flag.jrtokaistock_applied = false;   /* 非適用 */
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
    //	   & 0x300= 0x200: JR東海株主優待券使用
    //	   & 0x300= 0x100: JR東海株主優待券使用しない
    //     & 0x400= 0x400: Reverse不可(6の字)
    //     & 0x400= 0x000: 通常(会社線絡みはReverseできなくてもこっち)
    //virtual
    public int getFareOption()
    {
    	int rc;
    	int c = route_list_raw.size();

    	if (c == 1) {
    		rc = (1 << 6);    // 0x40; 	/* start only*/
    	}
    	else if (c <= 0) {
    		rc = (1 << 7);    // 0x80;	/* empty */
    	}
    	else {
    		rc = 0;
    	}

        // Reverse
        if ((c < 1) ||
                (!isRoundTrip() && // finished or company stop
                        //startStationId() == endStationId()
                        (route_list_raw.get(0).stationId != route_list_raw.get(route_list_raw.size() - 1).stationId))) {
            // Qの字の大阪環状線ではダメやねん
            rc |= 0x400;	// Disable reverse
        }

        // 大阪環状線 1回通過で近回り時 bit 2-3
    	if (last_flag.is_osakakan_1pass()) {
    		if (last_flag.osakakan_detour) {
    			rc |= (1 << 3);  // 0x08;
    		}
    		else {
    			rc |= (1 << 2);  // 0x04;
    		}
    	}
    	// 特例有無 bit 4-5
    	if (last_flag.rule_en) {
    		if (last_flag.no_rule) {
    			rc |= 0x30;     //
    		}
    		else {
    			rc |= 0x10;     // default: Applied
    		}
    	}
        // JR東海株主 有無 bit 4-5
    	if (last_flag.jrtokaistock_enable) {
    		if ((rc & 0x30) != 0x20) {	// Disable Tokai stock if applied the No Rule
			    if (last_flag.jrtokaistock_applied) {
				    rc |= (1 << 9);     // 0x200 : Enable
			    }
                else {
				    rc |= (1 << 8);		// default: Disable 0x100
			    }
            }
		}
    	return rc;
    }
    public boolean			isRoundTrip()  {
        return !last_flag.end || last_flag.compnda;
    }
}
