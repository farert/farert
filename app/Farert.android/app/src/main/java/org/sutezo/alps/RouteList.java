package org.sutezo.alps;

//package Route;

import static org.sutezo.alps.farertAssert.ASSERT;

import java.util.ArrayList;
import java.util.List;


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

    final static int BSRNOTYET_NA	= 30;	// [w]不完全ルート
                                            // AttrOfStationOnLineLine()の戻り値(add()時の駅正当チェックに局所的に使用)
                                            // 規43−2や、分岐特例途中の中途半端な経路の時ON
                                            // 会社線通過連絡運輸で発着駅のみのチェックである場合もON（と同時に、COMPNTERMINALもON）
    final static int BSRJCTSP_B	= 29;		// [w]分岐特例B
    final static int BSRSHINZAIREV	= 28;		// [w]新幹線-在来線折り返し

    final static int BSR69TERM	= 24;    // [r] not used
    final static int BSR69CONT	= 23;    // [r]
    final static int BSRSHINKTRSALW	= 19;    // [r]
    final static int BSRJCT	= 15;       // [r]
    final static int BSRBORDER	= 16;    // [r]

    final static int JCTSP	= 1;
    final static int JCTSP_2L = 2;
    final static int JCTSP_B_NISHIKOKURA = 3;
    final static int JCTSP_B_YOSHIZUKA = 4;
    final static int JCTSP_B_NAGAOKA = 5;

    // route_flag

    List<RouteItem> route_list_raw = new ArrayList<>(0);

    RouteFlag route_flag = new RouteFlag();	// add() - removeTail() work

    public RouteList() {}
    public RouteList(RouteList base_route) {
        this.assign(base_route, -1);
    }

    public void assign(RouteList source_route) {
        this.assign(source_route, -1);
    }

    public void assign(RouteList source_route, int count) {
        if (count < 0) {
            route_list_raw = dupRouteItems(source_route.route_list_raw, count);
            route_flag = new RouteFlag(source_route.route_flag);
        } else {
            int row = 0;
            // build
            Route build_route = new Route();
            if (0 < count) {
                for (RouteItem ri : source_route.route_list_raw) {
                    ++row;
                    if (count < row) {
                        break;
                    }
                    if (1 < row) {
                        build_route.add(ri.lineId, ri.stationId);
                    } else {
                        build_route.add(ri.stationId);
                    }
                }
            }
            // copy of route
            route_list_raw = dupRouteItems(build_route.route_list_raw, count);
            // copy of flag
            if (source_route.route_flag.osakakan_detour) {
                build_route.setDetour(true);
            }
            route_flag = build_route.route_flag;
        }
    }

    static List<RouteItem> dupRouteItems(final List<RouteItem> source) {
        return dupRouteItems(source, -1);
    }

    static List<RouteItem> dupRouteItems(final List<RouteItem> source, int count) {
        List<RouteItem> dest = new ArrayList<>(source.size());
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
    public int departureStationId() {
        return (route_list_raw.size() <= 0) ? 0 : route_list_raw.get(0).stationId;
    }

    public int arriveStationId() {
        return (route_list_raw.size() <= 0) ? 0 : route_list_raw.get(route_list_raw.size() - 1).stationId;
    }

    List<RouteItem> routeList() { return route_list_raw; }
    public int getCount() { return route_list_raw.size();   }
    public RouteItem item(int index) { return routeList().get(index); }

    public RouteFlag getRouteFlag() { return route_flag; }

    boolean isModified() {
        return route_flag.jctsp_route_change;
    }
    boolean is_end() { return route_flag.end; }

    /*	ルート表示
	 *	@return ルート文字列
	 *
	 *	@remark showFare()の呼び出し後にのみ有効
	 */
    public String route_script() {
        StringBuilder result_str = new StringBuilder();
        boolean oskk_flag;
        int i;

        if (route_list_raw.size() == 0) {	/* 経路なし(AutoRoute) */
            return "";
        }

        if (route_flag.notsamekokurahakatashinzai) {
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
                    if (route_flag.osakakan_detour) {
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
                ASSERT (departureStationId() == route_item.stationId);
            }
            stationIdFrom = route_item.stationId;
        }
        return false;
    }

    public boolean isAvailableReverse() {
        int c = route_list_raw.size();

        // Available reverse
        return (1 <= c) && (route_flag.isRoundTrip() ||
                (route_list_raw.get(0).stationId == route_list_raw.get(c - 1).stationId));
        // Qの字の大阪環状線ではダメやねん
    }
    boolean isRoundTrip()  { return route_flag.isRoundTrip(); }

    // virual
    int  coreAreaIDByCityId(int startEndFlg) { return 0;}
}
