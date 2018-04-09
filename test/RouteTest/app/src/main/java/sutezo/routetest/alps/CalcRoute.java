package sutezo.routetest.alps;

//package Route;

import java.util.*;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.database.sqlite.SQLiteOpenHelper;

import static sutezo.routetest.alps.RouteUtil.JR_CENTRAL;


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


public class CalcRoute extends RouteList {

    List<RouteItem> route_list_cooked = new ArrayList<RouteItem>(0);

//not used    List<RouteItem> cookedRouteList() { return route_list_cooked; }

    private CalcRoute() {

    }

    public CalcRoute(RouteList route) {
        this(route, -1);
    }

    public CalcRoute(RouteList route, int count) {
        sync(route, count);
    }

    public void sync(RouteList route) {
        assign(route);
        last_flag = route.getLastFlag().clone();
        route_list_cooked.clear();
    }

    public void sync(RouteList route, int count) {
        assign(route, count);
        last_flag = route.getLastFlag().clone();
        route_list_cooked.clear();
    }



    //public:
    //	運賃表示
    //	@return 運賃、営業キロ情報 表示文字列
    //
    public String showFare() {
//	final static int MAX_BUF = 1024;
        StringBuffer sqlbuf = new StringBuffer();

        FARE_INFO fare_info = calcFare();
        switch (fare_info.resultCode()) {
        case 0:     // success, company begin/first or too many company
            break;  // OK
        case -1:    /* In completed */
            return new String("この経路の片道乗車券は購入できません.続けて経路を指定してください.");
        case -2:     // empty.
            return new String("");
        default:
            return new String("予期せぬエラーです");
        }

	    /* 発駅→着駅 */
        sqlbuf.append(BeginOrEndStationName(fare_info.getBeginTerminalId()) +
                " -> " +
                BeginOrEndStationName(fare_info.getEndTerminalId()) +
                "\r\n経由：" +
                fare_info.getRoute_string());

        RouteUtil.ASSERT (100<=fare_info.getFareForDisplay());

        if (fare_info.isUrbanArea()) {

			 /* 全経路が近郊区間内で新幹線未利用のとき最短距離で算出可能 */
            sqlbuf.append(String.format(
                            "近郊区間内ですので最短経路の運賃で利用可能です(途中下車不可、有効日数当日限り)\r\n")
            );
            //RouteUtil.ASSERT (fare_info.getTicketAvailDays() <= 2);
        } else {
            // 東京電車特定運賃区間があるので：RouteUtil.ASSERT (fare_info.getFareForIC() == 0);
        }

        if ((0 < fare_info.getJRCalcKm()) &&
                (fare_info.getJRSalesKm() != fare_info.getJRCalcKm())) {
            sqlbuf.append(String.format(
                    (fare_info.getCompanySalesKm() <= 0) ?
                            "営業キロ： %s km     計算キロ： %s km\r\n" :
                            "営業キロ： %s km JR線計算キロ： %s km\r\n",
                    RouteUtil.num_str_km(fare_info.getTotalSalesKm()),
                    RouteUtil.num_str_km(fare_info.getJRCalcKm())));
        } else {
            sqlbuf.append(String.format(
                    "営業キロ： %s km\r\n",
                    RouteUtil.num_str_km(fare_info.getTotalSalesKm())));
        }

		/* 会社線(通過連絡運輸)あり */
        if (fare_info.getJRSalesKm() != fare_info.getTotalSalesKm()) {
            RouteUtil.ASSERT (0 != fare_info.getCompanySalesKm());
            sqlbuf.append(String.format(
                    "(JR線営業キロ： %-6s km   会社線営業キロ： %s km)\r\n",
                    RouteUtil.num_str_km(fare_info.getJRSalesKm()),
                    RouteUtil.num_str_km(fare_info.getCompanySalesKm())));
        } else {
            RouteUtil.ASSERT (0 == fare_info.getCompanySalesKm());
        }

        if (0 < fare_info.getSalesKmForHokkaido()) {
            if ((0 < fare_info.getCalcKmForHokkaido()) &&
                    (fare_info.getCalcKmForHokkaido() != fare_info.getSalesKmForHokkaido())) {
                sqlbuf.append(String.format("JR北海道営業キロ： %-6s km 計算キロ： %s km\r\n",
                        RouteUtil.num_str_km(fare_info.getSalesKmForHokkaido()),
                        RouteUtil.num_str_km(fare_info.getCalcKmForHokkaido())));
            } else {
                sqlbuf.append(String.format("JR北海道営業キロ： %-6s km\r\n",
                        RouteUtil.num_str_km(fare_info.getSalesKmForHokkaido())));
            }
        }
        if (0 < fare_info.getSalesKmForShikoku()) {
            if ((0 < fare_info.getCalcKmForShikoku()) &&
                    (fare_info.getSalesKmForShikoku() != fare_info.getCalcKmForShikoku())) {
                sqlbuf.append(String.format("JR四国営業キロ： %-6s km 計算キロ： %s km\r\n",
                        RouteUtil.num_str_km(fare_info.getSalesKmForShikoku()),
                        RouteUtil.num_str_km(fare_info.getCalcKmForShikoku())));
            } else {
                sqlbuf.append(String.format("JR四国営業キロ： %-6s km \r\n",
                        RouteUtil.num_str_km(fare_info.getSalesKmForShikoku())));
            }
        }
        if (0 < fare_info.getSalesKmForKyusyu()) {
            if ((0 < fare_info.getCalcKmForKyusyu()) &&
                    (fare_info.getSalesKmForKyusyu() != fare_info.getCalcKmForKyusyu())) {
                sqlbuf.append(String.format("JR九州営業キロ： %-6s km  計算キロ： %-6s km\r\n",
                        RouteUtil.num_str_km(fare_info.getSalesKmForKyusyu()),
                        RouteUtil.num_str_km(fare_info.getCalcKmForKyusyu())));
            } else {
                sqlbuf.append(String.format("JR九州営業キロ： %-6s km \r\n",
                        RouteUtil.num_str_km(fare_info.getSalesKmForKyusyu())));
            }
        }

	/*
	運賃(IC)          ： \123,456(\123,456)  小児： \123,456(\123,456) 学割： \123,456
	往復(IC)|(割)     ： \123,456(\123,456)         \123,456(\123,456)        \123,456
	JR東日本 株主優待2： \123,456
	JR東日本 株主優待4： \123,456


	運賃(IC): \123,456(\123,45)   往復: \123,45(\123,45)
	運賃: \123,456   往復: \123,45 (割)
	運賃: \123,456   往復: \123,45
	*/

        int company_fare = fare_info.getFareForCompanyline();
        int normal_fare = fare_info.getFareForDisplay();
        FARE_INFO.FareResult fareW = fare_info.roundTripFareWithCompanyLine();
        int fare_ic = fare_info.getFareForIC();

        if (fare_info.isRule114()) {
            sqlbuf.append(String.format("規程114条適用 営業キロ： %s km 計算キロ： %s km\r\n",
                    RouteUtil.num_str_km(fare_info.getRule114SalesKm()),
                    RouteUtil.num_str_km(fare_info.getRule114CalcKm())));
        }
        String company_option;
        if (0 < company_fare) {
            company_option = String.format(" (うち会社線： ¥%s)", RouteUtil.num_str_yen(company_fare));
        } else {
            company_option = "";
        }
        if (fare_ic == 0) {
            sqlbuf.append(String.format("運賃： ¥%-7s %s     往復： ¥%-5s",
                    RouteUtil.num_str_yen(normal_fare),
                    company_option,
                    RouteUtil.num_str_yen(fareW.fare)));
            if (fareW.isDiscount) {
                sqlbuf.append("(割)\r\n");
            } else if (fare_info.isRule114()) {
                sqlbuf.append(String.format("\r\n規程114条 適用前運賃： {¥%-5s} 往復： {¥%-5s}\r\n",
                        RouteUtil.num_str_yen(fare_info.getFareForDisplayPriorRule114()),
                        RouteUtil.num_str_yen(fare_info.roundTripFareWithCompanyLinePriorRule114())));
            } else {
                sqlbuf.append("\r\n");
            }
        } else {
            RouteUtil.ASSERT (!fareW.isDiscount);
            RouteUtil.ASSERT (company_fare == 0);
            RouteUtil.ASSERT (normal_fare  *  2 == fare_info.roundTripFareWithCompanyLine().fare);
            sqlbuf.append(String.format("運賃(IC)： ¥%s(¥%s)  %s    往復： ¥%s(¥%s)\r\n",
                    RouteUtil.num_str_yen(normal_fare), RouteUtil.num_str_yen(fare_ic),
                    company_option,
                    RouteUtil.num_str_yen(normal_fare * 2), RouteUtil.num_str_yen(fare_ic * 2)));
        }

        for (int i = 0; true; i++) {
            int fareStock = fare_info.getFareStockDiscount(i);

            if (fareStock <= 0) {
                break;
            }
            if (0 < i) {
                sqlbuf.append("\r\n");
            }
            if (fare_info.isRule114()) {
                sqlbuf.append(String.format("%s： ¥%s{¥%s}",
                        fare_info.getTitleFareStockDiscount(i),
                        RouteUtil.num_str_yen(fare_info.getFareStockDiscount(i, true) +
                                company_fare),
                        RouteUtil.num_str_yen(fareStock + company_fare)));
            } else {
                sqlbuf.append(String.format("%s： ¥%s",
                        fare_info.getTitleFareStockDiscount(i),
                        RouteUtil.num_str_yen(fareStock + company_fare)));
            }
        }

		/* child fare */
        sqlbuf.append(String.format("\r\n小児運賃： ¥%-7s 往復： ¥%-5s\r\n",
                RouteUtil.num_str_yen(fare_info.getChildFareForDisplay()),
                RouteUtil.num_str_yen(fare_info.roundTripChildFareWithCompanyLine())));
        normal_fare = fare_info.getAcademicDiscountFare();
        if (0 < normal_fare) {
            sqlbuf.append(String.format("学割運賃： ¥%-7s 往復： ¥%-5s\r\n",
                    RouteUtil.num_str_yen(normal_fare),
                    RouteUtil.num_str_yen(fare_info.roundTripAcademicFareWithCompanyLine())));
        }
        sqlbuf.append(String.format(
                "\r\n有効日数：%4d日\r\n",
                fare_info.getTicketAvailDays()));

        if (fare_info.isMultiCompanyLine()) {
    		sqlbuf.append("\r\n複数の会社線を跨っているため、乗車券は通し発券できません. 運賃額も異なります.");
    	} else if (fare_info.isBeginEndCompanyLine()) {
    		sqlbuf.append("\r\n会社線通過連絡運輸ではないためJR窓口で乗車券は発券されません.");
    	}
        if (fare_info.isEnableTokaiStockSelect()) {
            sqlbuf.append("\r\nJR東海株主優待券使用オプション選択可");
        }
        return sqlbuf.toString();
    }

    public int getFareOption() {
        int rc = super.getFareOption();

    	if ((rc & 0xc0) != 0) {
    		return rc;	/* empty or start only (can't calculate) */
    	}

    	if ((route_list_cooked == null) || (route_list_cooked.size() <= 0)) {
    	//return 0x80;	/* showFare() or calcFare non called */

    		FARE_INFO.Fare fa = checkOfRuleSpecificCoreLine();
            if ((route_list_cooked == null) || (route_list_cooked.size() <= 0)) {
    /* どっちでもOKだが */
    //		FARE_INFO fi = calcFare();
    //		if ((fi.resultCode() != 0) || (route_list_cooked.size() <= 0)) {

    			RouteUtil.ASSERT(false);
    			return 0xc0 | rc;	// calc error
    		}
            rc = super.getFareOption();
    	}
        // JR東海株主 有無 bit 4-5
    	if ((0 != (0x300 & rc)) /*&& last_flag.no_rule)*/) {
    		if (!last_flag.jrtokaistock_enable) {
    			rc &= ~0x300;
    		}
    	}
    	// 名阪 都区市内 - 都区市内 bit 0-1
    	if (last_flag.ter_begin_city_off) {
    		rc |= 1;
    	}
    	else if (last_flag.ter_fin_city_off) {
    		rc |= 2;
    	}
    	return rc;
    }

    //public
    //  開始駅IDを得る
    //  @param [in] applied_agree : True=特例適用 / false=特例非適用
    //  @return  開始駅ID(1000を越えると都区市内ID)
    //
    public int beginStationId(boolean applied_agree) {
        int stid;

        if (!applied_agree) {
            return route_list_raw.get(0).stationId;
        } else {
            stid = coreAreaIDByCityId(RouteUtil.CSTART);
            if (stid == 0) {
                RouteUtil.ASSERT(route_list_cooked.get(0).stationId == route_list_raw.get(0).stationId);
                return route_list_cooked.get(0).stationId;
            } else {
                return stid + RouteUtil.STATION_ID_AS_CITYNO;
            }
        }
    }

    //  public
    //  着（終了）駅IDを得る
    //  @param [in] applied_agree : True=特例適用 / false=特例非適用
    //  @return 開始駅ID(1000を越えると都区市内ID)
    //
    public int endStationId(boolean applied_agree) {
        int stid;

        if (!applied_agree) {
            return route_list_raw.get(route_list_raw.size() - 1).stationId;

        } else {
            stid = coreAreaIDByCityId(RouteUtil.CEND);
            if (stid == 0) {
                 RouteUtil.ASSERT(route_list_cooked.get(route_list_cooked.size() - 1).stationId == route_list_raw.get(route_list_raw.size() - 1).stationId);
                return route_list_cooked.get(route_list_cooked.size() - 1).stationId;
            } else {
                return stid + RouteUtil.STATION_ID_AS_CITYNO;
            }
        }
    }

    //public
    //	86、87条、69条判定＆経路算出
    //	showFare() => calcFare() =>
    //	route_list_raw => route_list_cooked
    //
    //	後半でter_xxx(route[0].lineId)を設定します
    //
    //	@param [in]  last_flag   meihancityflag = 発駅:着駅 無効(0)/有効(1)
    //	@param [out] rule114	 [0] = 運賃, [1] = 営業キロ, [2] = 計算キロ
    //	@return false : rule 114 no applied. true: rule 114 applied(available for rule114[] )
    //	@remark ルール未適用時はroute_list_cooked = route_list_rawである
    //
    public FARE_INFO.Fare checkOfRuleSpecificCoreLine() {

        int[] cityId = new int [2];
        int sales_km;
        int skm;
        List<RouteItem> route_list_tmp = new ArrayList<RouteItem>();
        List<RouteItem> route_list_tmp2 = new ArrayList<RouteItem>();
        List<RouteItem> route_list_tmp3 = new ArrayList<RouteItem>();
        List<RouteItem> route_list_tmp4 = new ArrayList<RouteItem>();
        Station enter = new Station();
        Station exit = new Station();
        int n;
        int sk;         /* 114 check 90km or 190km */
        int sk2 = 0;	/* begin and arrive point as city, 101km or 201km */
        int chk;        /* 86 applied flag */
        int rtky;       /* 87 applied flag */
        int flg;
        int aply88;

        last_flag.rule_en = false;    // initialize

        // 69を適用したものをroute_list_tmp2へ
        n = ReRouteRule69j(route_list_raw, route_list_tmp);	/* 69条適用(route_list_raw.route_list_tmp) */
        System.out.printf("Rule 69 applied %dtimes.\n", n);
        if (0 < n) {
            last_flag.rule_en = true;    // applied rule
        }
        // route_list_tmp2 = route_list_tmp
        // 70を適用したものをroute_list_tmp2へ
        n = ReRouteRule70j(route_list_tmp, /*out*/route_list_tmp2);
        System.out.printf(0 == n ? "Rule70 applied.\n" : "Rule70 not applied.\n");
        if (0 == n) {
            last_flag.rule_en = true;    // applied rule
        }

        // 88を適用したものをroute_list_tmpへ
        aply88 = CheckOfRule88j(route_list_tmp2);
        if (0 != aply88) {
            if ((aply88 & 1) != 0) {
                System.out.printf("Apply to rule88 for start.\n");
                last_flag.terCityReset();
    			last_flag.ter_begin_oosaka = true;
            } else if ((aply88 & 2) != 0) {
                System.out.printf("Apply to rule88 for arrive.\n");
                last_flag.terCityReset();
    			last_flag.ter_fin_oosaka = true;
            }
            last_flag.rule_en = true;    // applied rule
        }

		/* 特定都区市内発着可否判定 */
        chk = CheckOfRule86(route_list_tmp2.toArray(new RouteItem[0]), last_flag, exit, enter, cityId);
        System.out.printf("RuleSpecific:chk 0x%x, %d -> %d\n", chk, cityId[0], cityId[1]);
        if (RouteUtil.BIT_CHK(chk, 31)) {
    		last_flag.jrtokaistock_enable = true; // for UI
    	}
    	else {
    		last_flag.jrtokaistock_enable = false; // for UI
    	}
    	chk &= ~(1 << 31);

		/* 86, 87適用可能性None？ */
        if ((chk == 4) || (chk == 0)) {  /* 全駅特定都区市内駅 or 発着とも特定都区市内駅でない場合 */
			/* 未変換 */
            System.out.printf("no applied for rule86/87\n");
            cpyRouteItems(route_list_tmp2, route_list_cooked);
            return new FARE_INFO.Fare();			// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        }

		/* (発駅=都区市内 or 着駅=都区市内)
		 */
		/* route_list_tmp = route_list_tmp2 */
        cpyRouteItems(route_list_tmp2, route_list_tmp);

		/* 変換 -> route_list_tmp:86適用(仮)
		   88変換したものは対象外(=山陽新幹線 新大阪着時、非表示フラグが消えてしまうのを避ける効果あり) */
        ReRouteRule86j87j(cityId, chk & ~aply88, exit, enter, /*out*/route_list_tmp);

        // 88を適用
        aply88 = CheckOfRule88j(route_list_tmp);
        if (0 != aply88) {
            if ((aply88 & 1) != 0) {
                System.out.printf("Apply to rule88(2) for start.\n");
                last_flag.terCityReset();
    			last_flag.ter_begin_oosaka = true;
            } else if ((aply88 & 2) != 0) {
                System.out.printf("Apply to rule88(2) for arrive.\n");
                last_flag.terCityReset();
    			last_flag.ter_fin_oosaka = true;
            }
            last_flag.rule_en = true;    // applied rule
        }

        // 69を適用したものをroute_list_tmp3へ
        n = ReRouteRule69j(route_list_tmp, route_list_tmp3);	/* 69条適用(route_list_tmp->route_list_tmp3) */
        System.out.printf("Rule 69(2) applied %dtimes.\n", n);

		/* route_list_tmp	70-88-69-86適用
		 * route_list_tmp2	70-88-69適用
		 * route_list_tmp3	70-88-69-86-69適用
		 */
		/* compute of sales_km by route_list_cooked */
        sales_km = Get_route_distance(last_flag, route_list_tmp3).sales_km;

        if ((2000 < sales_km) && ((InRouteUrban(route_list_raw) != RouteUtil.URB_TOKYO) || (2000 < Get_route_distance(last_flag, route_list_raw).sales_km))) {
			/* <<<都区市内適用>>> */
			/* 201km <= sales_km */
			/* enable */
            last_flag.terCityReset();
            switch (chk & 0x03) {
                case 0:
                    break;
                case 1:
                    last_flag.ter_begin_city = true;
                    break;
                case 2:
                    last_flag.ter_fin_city = true;
                    break;
                case 3:
                default:
                    last_flag.ter_begin_city = true;
                    last_flag.ter_fin_city = true;
                    break;
            }
            System.out.printf("applied for rule86(%d)\n", chk & 0x03);

            // route_list_cooked = route_list_tmp3
            cpyRouteItems(route_list_tmp3, route_list_cooked);

            last_flag.rule_en = true;    // applied rule

            return new FARE_INFO.Fare();			// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        }

		/* 101km - 200km : 山手線発着 or 200km以下の都区市内間(名古屋-大阪など)チェック */
        rtky = CheckOfRule87(route_list_tmp2.toArray(new RouteItem[0]));
        if ((3 & rtky) != 0) {
			/* apply to 87 */  /* 都区内に限り最短が100km以下は非適用(基115-2) */
            if ((1000 < sales_km) && ((InRouteUrban(route_list_raw) != RouteUtil.URB_TOKYO) || (1000 < Get_route_distance(last_flag, route_list_raw).sales_km))) {
				/* 山手線内発着 enable */
                if (!last_flag.jrtokaistock_enable || !last_flag.jrtokaistock_applied) {

    				last_flag.terCityReset();
                    switch (chk & 0x03) {
                        case 0:
                            break;
                        case 1:
                            last_flag.ter_begin_yamate = true;
                            break;
                        case 2:
                            last_flag.ter_fin_yamate = true;
                            break;
                        case 3:
                        default:
                            last_flag.ter_begin_yamate = true;
                            last_flag.ter_fin_yamate = true;
                            break;
                    }
    				System.out.printf("applied for rule87\n");
    			}
                // route_list_cooked = route_list_tmp3
                cpyRouteItems(route_list_tmp3, route_list_cooked);

                last_flag.rule_en = true;    // applied rule

                return new FARE_INFO.Fare();			// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
            }
            sk = 900;	/* 90km */
        } else {
			/* can't apply to 87 */
            sk = 1900;	/* 190km */
        }

        // route_list_tmp4 = route_list_tmp3
        cpyRouteItems(route_list_tmp3, route_list_tmp4);

		/* 発着とも都区市内? */
        if ((0x03 & (rtky | chk)) == 3) { /* 名古屋市内-大阪市内など([名]-[阪]、[九]-[福]、[区]-[浜]) */
								/*  [区]-[区], [名]-[名], [山]-[区], ... */
            for (sk2 = 2000; true; sk2 = 1000) {
                flg = 0;

				/* route_list_tmp = route_list_tmp2 */
                cpyRouteItems(route_list_tmp2, route_list_tmp);
				/* 発駅のみ特定都区市内着経路に変換 */
                ReRouteRule86j87j(cityId, 1, exit, enter, /*out*/route_list_tmp);

                // 69を適用したものをroute_list_tmp3へ
                n = ReRouteRule69j(route_list_tmp, route_list_tmp3);	/* 69条適用(route_list_tmp->route_list_tmp3) */
                System.out.printf("Rule 69(3) applied %dtimes.\n", n);

				/* 発駅のみ都区市内にしても201/101km以上か？ */
                skm = Get_route_distance(last_flag, route_list_tmp3).sales_km;
                if (sk2 < skm) {
                    // 発 都区市内有効
                    flg = 0x01;
                }

				/* route_list_tmp = route_list_tmp2 */
                cpyRouteItems(route_list_tmp2, route_list_tmp);
				/* 着駅のみ特定都区市内着経路に変換仮適用 */
                ReRouteRule86j87j(cityId, 2, exit, enter, /*out*/route_list_tmp);

                // 69を適用したものをroute_list_tmp3へ
                n = ReRouteRule69j(route_list_tmp, route_list_tmp3);	/* 69条適用(route_list_tmp->route_list_tmp3) */
                System.out.printf("Rule 69(4) applied %dtimes.\n", n);

				/* 着駅のみ都区市内にしても201/101km以上か？ */
                skm = Get_route_distance(last_flag, route_list_tmp3).sales_km;
                if (sk2 < skm) {
                    // 着 都区市内有効
                    flg |= 0x02;
                }
                if (flg == 0x03) {	/* 発・着とも200km越えだが、都区市内間は200km以下 */
                    if (last_flag.meihancityflag) {
						/* 発のみ都区市内適用 */
						/* route_list_tmp = route_list_tmp2 */
                        cpyRouteItems(route_list_tmp2, route_list_tmp);
						/* 発駅のみ特定都区市内着経路に変換 */
                        ReRouteRule86j87j(cityId, 1, exit, enter, /*out*/route_list_tmp);

                        // 69を適用したものをroute_list_tmp3へ
                        n = ReRouteRule69j(route_list_tmp, route_list_tmp3);	/* 69条適用(route_list_tmp->route_list_tmp3) */
                        System.out.printf("Rule 69(5) applied %dtimes.\n", n);

						/* 発駅・着駅特定都区市内だが発駅のみ都区市内適用 */
                        if (sk == 900) {
                            System.out.printf("applied for rule87(start)\n");
                            last_flag.terCityReset();
            				last_flag.ter_begin_yamate = true;
            				last_flag.ter_begin_city_off = true;
                        } else {
                            System.out.printf("applied for rule86(start)\n");
                            last_flag.terCityReset();
            				last_flag.ter_begin_city = true;
            				last_flag.ter_begin_city_off = true;
                        }
                        // route_list_cooked = route_list_tmp3
                        cpyRouteItems(route_list_tmp3, route_list_cooked);
                        last_flag.rule_en = true;    // applied rule
                        return new FARE_INFO.Fare();			// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
                    } else {
						/* 着のみ都区市内適用 */
						/* 発駅・着駅特定都区市内だが着駅のみ都区市内適用 */
                        if (sk == 900) {
                            System.out.printf("applied for rule87(end)\n");
                            last_flag.terCityReset();
        					last_flag.ter_fin_yamate = true;
        					last_flag.ter_fin_city_off = true;
                        } else {
                            System.out.printf("applied for rule86(end)\n");
                            last_flag.terCityReset();
        					last_flag.ter_fin_city = true;
        					last_flag.ter_fin_city_off = true;
                        }
                        // route_list_cooked = route_list_tmp3
                        cpyRouteItems(route_list_tmp3, route_list_cooked);
                        last_flag.rule_en = true;    // applied rule
                        return new FARE_INFO.Fare();			// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
                    }
                } else if (flg == 0x01) {
					/* route_list_tmp = route_list_tmp2 */
                    cpyRouteItems(route_list_tmp2, route_list_tmp);
					/* 発駅のみ特定都区市内着経路に変換 */
                    ReRouteRule86j87j(cityId, 1, exit, enter, /*out*/route_list_tmp);

                    // 69を適用したものをroute_list_tmp3へ
                    n = ReRouteRule69j(route_list_tmp, route_list_tmp3);	/* 69条適用(route_list_tmp->route_list_tmp3) */
                    System.out.printf("Rule 69(6) applied %dtimes.\n", n);

					/* 発駅・着駅特定都区市内だが発駅のみ都区市内適用 */
                    if (sk == 900) {
                        System.out.printf("applied for rule87(start)\n");
                        last_flag.terCityReset();
    					last_flag.ter_begin_yamate = true;;
                    } else {
                        System.out.printf("applied for rule86(start)\n");
                        last_flag.terCityReset();
    					last_flag.ter_begin_city = true;
                    }
                    // route_list_cooked = route_list_tmp3
                    cpyRouteItems(route_list_tmp3, route_list_cooked);
                    last_flag.rule_en = true;    // applied rule
                    return new FARE_INFO.Fare();			// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
                } else if (flg == 0x02) {
					/* 発駅・着駅特定都区市内だが着駅のみ都区市内適用 */
                    if (sk == 900) {
                        System.out.printf("applied for rule87(end)\n");
                        last_flag.terCityReset();
    					last_flag.ter_fin_yamate = true;
                    } else {
                        System.out.printf("applied for rule86(end)\n");
                        last_flag.terCityReset();
    					last_flag.ter_fin_city = true;
                    }
                    // route_list_cooked = route_list_tmp3
                    cpyRouteItems(route_list_tmp3, route_list_cooked);
                    last_flag.rule_en = true;    // applied rule
                    return new FARE_INFO.Fare();			// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
                }
				/* flg == 0 */
                if ((sk != 900) || (sk2 == 1000)) {
					/* 87無効 or 営業キロ200km越え判定は済 */
                    break;
                }
            } /* sk2= 2000, 1000 */
			/* passthru */
        } /* ((0x03 & (rtky | chk)) == 3)  名古屋市内-大阪市内など([名]-[阪]、[九]-[福]、[区]-[浜]) */

		/* route_list_tmp	x
		 * route_list_tmp2	70-88-69適用
		 * route_list_tmp3	x
		 * route_list_tmp4	70-88-69-86-69適用
		 * route_list_cooked 空
		 */

		/* 未変換 */
        System.out.printf("no applied for rule86/87(sales_km=%d)\n", sales_km);

        FARE_INFO.Fare r114;

        if (sk <= sales_km) {
				/* 114条適用かチェック */

            cpyRouteItems(route_list_tmp2, route_list_tmp);
            cpyRouteItems(route_list_tmp4, route_list_tmp3);

            route_list_tmp = ConvertShinkansen2ZairaiFor114Judge(route_list_tmp);
            route_list_tmp3 = ConvertShinkansen2ZairaiFor114Judge(route_list_tmp3);
            if ((0x03 & chk) == 3) {
							/* 86,87適用前,   86,87適用後 */
                r114 = CheckOfRule114j(last_flag, route_list_tmp, route_list_tmp3,
                        0x01 | ((sk2 == 2000) ? 0 : 0x8000));
                if (r114 == null) {
                    r114 = CheckOfRule114j(last_flag, route_list_tmp, route_list_tmp3,
                            0x02 | ((sk2 == 2000) ? 0 : 0x8000));
                }
            } else {
                RouteUtil.ASSERT (((0x03 & chk) == 1) || ((0x03 & chk) == 2));
                r114 = CheckOfRule114j(last_flag, route_list_tmp, route_list_tmp3,
                                (chk & 0x03) | ((sk == 1900) ? 0 : 0x8000));
            }
        } else {
            // do nothing(not 114)
            r114 = new FARE_INFO.Fare();
        }
		/* 86-87非適用 */
        // route_list_cooked = route_list_tmp2
        cpyRouteItems(route_list_tmp2, route_list_cooked);
        if (r114 == null) {
            r114 = new FARE_INFO.Fare();
        }
        return r114;
    }

    /*  public
	 *  運賃計算
	 *  result_flag 0 : empty root
	 *   -2 : 吉塚、西小倉における不完全ルート：この経路の片道乗車券は購入できません.
	 *   1 : normal(Success)
	 */
    public FARE_INFO calcFare() {
        FARE_INFO fare_info = new FARE_INFO();
    	FARE_INFO.Fare rule114;	// [0] = 運賃, [1] = 営業キロ, [2] = 計算キロ

    	if (route_list_raw.size() <= 1) {
    		fare_info.setEmpty();

    /* fare_info.result_flag は privateなので でもASSERTは正当 */
    //ASSERT((BIT_CHK(fare_info.result_flag, BRF_COMAPANY_FIRST) && last_flag.compnbegin)) ||
    //	(!BIT_CHK(fare_info.result_flag, BRF_COMAPANY_FIRST) && !last_flag.compnbegin)));
    //ASSERT((BIT_CHK(fare_info.result_flag, BRF_COMAPANY_END) && last_flag.compnend)) ||
    //	(!BIT_CHK(fare_info.result_flag, BRF_COMAPANY_END) && !last_flag.compnend)));

    		return fare_info;
    	}

    	if (RouteUtil.BIT_CHK(route_list_raw.get(route_list_raw.size() - 1).flag, RouteList.BSRNOTYET_NA)) {
    					// BIT_ON(result_flag, BRF_ROUTE_INCOMPLETE)
    		fare_info.setInComplete();	// この経路の片道乗車券は購入できません."));
         /* fare_info.result_flag は privateなので でもASSERTは正当 */
    	//	ASSERT((BIT_CHK(fare_info.result_flag, BRF_COMAPANY_FIRST) && last_flag.compnbegin)) ||
    	//		(!BIT_CHK(fare_info.result_flag, BRF_COMAPANY_FIRST) && !last_flag.compnbegin)));
    	//	ASSERT((BIT_CHK(fare_info.result_flag, BRF_COMAPANY_END) && last_flag.compnend)) ||
    	//		(!BIT_CHK(fare_info.result_flag, BRF_COMAPANY_END) && !last_flag.compnend)));

    		return fare_info;
    	}
    	if (!last_flag.no_rule) {
    		/* 規則適用 */

    		/* 86, 87, 69, 70条 114条適用かチェック */
    		rule114 = checkOfRuleSpecificCoreLine();	// route_list_raw -> route_list_cooked
    		// 仮↑

    		if ((route_list_cooked == null) || (route_list_cooked.size() <= 1)) {
    RouteUtil.ASSERT(false);
    			// Don't come here
    			///////////////////////////////////////////////////
    			// calc fare

                fare_info.calc_fare(last_flag, route_list_raw, null);

    		} else {
    			///////////////////////////////////////////////////
    			// calc fare
                fare_info.setTerminal(this.beginStationId(true),
                        this.endStationId(true));
    			if (fare_info.calc_fare(last_flag, route_list_raw, route_list_cooked)) {
                    fare_info.setRoute(this.route_list_cooked, last_flag);
    				RouteUtil.ASSERT(fare_info.getBeginTerminalId() == this.beginStationId(true));
                    RouteUtil.ASSERT(fare_info.getEndTerminalId() == this.endStationId(true));
    			                                // rule 114 applied
                    fare_info.setRule114(rule114);
                } else {
				    fare_info.reset();
			    }
    		}
    	} else {
    		/* 規則非適用 */
    		/* 単駅 */
    		///////////////////////////////////////////////////
    		// calc fare
            fare_info.setTerminal(this.beginStationId(false),
    		 					  this.endStationId(false));
    		if (fare_info.calc_fare(last_flag, route_list_raw, null)) {
                fare_info.setRoute(this.route_list_raw, last_flag);
    			RouteUtil.ASSERT(fare_info.getBeginTerminalId() == this.beginStationId(false));
    			RouteUtil.ASSERT(fare_info.getEndTerminalId() == this.endStationId(false));
            }
    	}
    	// success
        /* fare_info.result_flag は privateなので でもASSERTは正当 */
    	// というわけで BRF_COMAPANY_xxx と、comnxxx は同じ
    	//TRACE("%d, %d - %d, %d\n", BIT_CHK(fare_info.result_flag, BRF_COMAPANY_FIRST), last_flag.compnbegin), BIT_CHK(fare_info.result_flag, BRF_COMAPANY_END), last_flag.compnend));
    	//ASSERT((BIT_CHK(fare_info.result_flag, BRF_COMAPANY_FIRST) && last_flag.compnbegin)) ||
    	//	(!BIT_CHK(fare_info.result_flag, BRF_COMAPANY_FIRST) && !last_flag.compnbegin)));
    	//ASSERT((BIT_CHK(fare_info.result_flag, BRF_COMAPANY_END) && last_flag.compnend)) ||
    	//	(!BIT_CHK(fare_info.result_flag, BRF_COMAPANY_END) && !last_flag.compnend)));

    	return fare_info;
    }

    public FARE_INFO calcFare(int count) {
        if (route_list_raw.size() < count) {
            return new FARE_INFO();
        }
        CalcRoute aRoute = new CalcRoute();
        aRoute.route_list_raw = aRoute.dupRouteItems(route_list_raw, count);
        return aRoute.calcFare();
    }

    //  public
    //  着（終了）駅を得る
    //  @param [in] applied_agree : True=特例適用 / false=特例非適用
    //  @return 終了駅名文字列を返す(String)
    //
    public static String BeginOrEndStationName(int ident) {
        if (ident < RouteUtil.STATION_ID_AS_CITYNO) {
            return RouteUtil.StationNameEx(ident);
        } else {
            return RouteUtil.CoreAreaCenterName(ident);
        }
    }

    //static private
    //	経路フラグから発着都区市内IDを得る
    //
    //	@param [in] startEndFlg     CSTART(発駅) / RouteUtil.CEND(着駅)
    //
    //	@return 特定都区市内ID(0だと都区市内ではない単駅)
    //
    private int coreAreaIDByCityId(int startEndFlg)	{
        int cityno;

        if (last_flag.rule_en) {
    		if (((startEndFlg == RouteUtil.CSTART) && last_flag.ter_begin_yamate) ||
    		    ((startEndFlg == RouteUtil.CEND)   && last_flag.ter_fin_yamate)) {
    			return RouteUtil.CITYNO_YAMATE;	/* 山手線内*/
    		} else if (((startEndFlg == RouteUtil.CSTART) && last_flag.ter_begin_city) ||
    				   ((startEndFlg == RouteUtil.CEND)   && last_flag.ter_fin_city)) {
    			int flags;
    			if (startEndFlg == RouteUtil.CSTART) {
    				flags = route_list_cooked.get(0).flag;
    			} else {
    				flags = route_list_cooked.get(route_list_cooked.size() - 1).flag;
    			}
    			cityno = RouteUtil.MASK_CITYNO(flags);
    			if ((0 < cityno) && (cityno < RouteUtil.CITYNO_YAMATE)) {
    				return cityno;
    			} else {
    				RouteUtil.ASSERT(false);
    			}
    		} else if (((startEndFlg == RouteUtil.CSTART) && last_flag.ter_begin_oosaka) ||
    				   ((startEndFlg == RouteUtil.CEND)   && last_flag.ter_fin_oosaka)) {
    			return RouteUtil.CITYNO_SHINOOSAKA;	/* 大阪・新大阪 */
    		}
            // else
            // thru
        }
        return 0;
    }


    //static:
    //	88条のチェックと変換
    //	新大阪発（着）-[東海道線]-神戸-[山陽線]-姫路以遠なら、新大阪→大阪置換
    //	大阪-[東海道線]-新大阪-[山陽新幹線]-
    //	の場合、大阪-[東海道線]-新大阪の区間を外す（201km以上なら外さない)
    //	(この関数へは201km以上の86条適用時には呼ばれることはない)
    //	ter_xxx(route[0].lineId)の設定前であることを前提(0を設定)とします
    //
    //	@param [in/out] route    route
    //
    //	@retval 0: no-convert
    //	@retval 1: change start
    //	@retval 2: change arrived
    //
    static int CheckOfRule88j(List<RouteItem> route) {
        DbidOf	dbid = new DbidOf();
        int lastIndex;
		/*static*/ int chk_distance1 = 0;
		/*static*/ int chk_distance2 = 0;

        lastIndex = (int)route.size() - 1;

        //if (!chk_distance1) {	/* chk_distance: 山陽線 神戸-姫路間営業キロ, 新幹線 新大阪-姫路 */
        chk_distance1 = RouteUtil.GetDistance(dbid.LineIdOf_SANYO, dbid.StationIdOf_KOUBE, dbid.StationIdOf_HIMEJI).get(0);
        chk_distance2 = RouteUtil.GetDistance(dbid.LineIdOf_SANYOSHINKANSEN, dbid.StationIdOf_SHINOSAKA, dbid.StationIdOf_HIMEJI).get(0);
        //}

        if (2 <= lastIndex) {
            // 新大阪 発 東海道線 - 山陽線
            if ((route.get(0).stationId == dbid.StationIdOf_SHINOSAKA) &&
                    (route.get(1).lineId == dbid.LineIdOf_TOKAIDO) &&
                    (route.get(2).lineId == dbid.LineIdOf_SANYO) &&
                    (chk_distance1 <= RouteUtil.GetDistance(dbid.LineIdOf_SANYO, dbid.StationIdOf_KOUBE, route.get(2).stationId).get(0))) {

                RouteUtil.ASSERT (route.get(1).stationId == dbid.StationIdOf_KOUBE);
				/*	新大阪発東海道線-山陽線-姫路以遠なら発駅を新大阪->大阪へ */
                route.get(0).let(new RouteItem(0, dbid.StationIdOf_OSAKA));	// 新大阪->大阪

                return 1;
            }	// 新大阪 着 山陽線 - 東海道線
            else if ((route.get(route.size() - 1).stationId == dbid.StationIdOf_SHINOSAKA) &&
                    (route.get(route.size() - 1).lineId == dbid.LineIdOf_TOKAIDO) &&
                    (route.get(lastIndex - 1).lineId == dbid.LineIdOf_SANYO) &&
                    (chk_distance1 <= RouteUtil.GetDistance(dbid.LineIdOf_SANYO, dbid.StationIdOf_KOUBE, route.get(lastIndex - 2).stationId).get(0))) {

                RouteUtil.ASSERT (route.get(lastIndex - 1).stationId == dbid.StationIdOf_KOUBE);
				/*	新大阪着東海道線-山陽線-姫路以遠なら着駅を新大阪->大阪へ */
                route.get(route.size() - 1).let(new RouteItem(dbid.LineIdOf_TOKAIDO, dbid.StationIdOf_OSAKA));	// 新大阪->大阪

                return 2;
            }
            // 大阪 発 新大阪 経由 山陽新幹線
            if ((route.get(0).stationId == dbid.StationIdOf_OSAKA) &&
                    (route.get(2).lineId == dbid.LineIdOf_SANYOSHINKANSEN) &&
                    (route.get(1).stationId == dbid.StationIdOf_SHINOSAKA) &&
                    (chk_distance2 <= RouteUtil.GetDistance(dbid.LineIdOf_SANYOSHINKANSEN, dbid.StationIdOf_SHINOSAKA, route.get(2).stationId).get(0))) {

                RouteUtil.ASSERT (route.get(1).lineId == dbid.LineIdOf_TOKAIDO);

				/* 大阪発-東海道線上り-新大阪-山陽新幹線 姫路以遠の場合、大阪発-東海道線-山陽線 西明石経由に付け替える */

                route.get(1).let(new RouteItem(dbid.LineIdOf_TOKAIDO, dbid.StationIdOf_KOUBE));
                route.get(1).flag |= (RouteUtil.FLG_HIDE_LINE | RouteUtil.FLG_HIDE_STATION);

                // at(2)						// 山陽線-西明石
                route.add(2, new RouteItem(dbid.LineIdOf_SANYO, dbid.StationIdOf_NISHIAKASHI));
                route.get(2).flag |= (RouteUtil.FLG_HIDE_LINE | RouteUtil.FLG_HIDE_STATION);

                return 1;
            }	// 山陽新幹線 新大阪 経由 大阪 着
            else if ((route.get(route.size() - 1).stationId == dbid.StationIdOf_OSAKA) &&
                    (route.get(lastIndex - 1).stationId == dbid.StationIdOf_SHINOSAKA) &&
                    (route.get(lastIndex - 1).lineId == dbid.LineIdOf_SANYOSHINKANSEN) &&
                    (chk_distance2 <= RouteUtil.GetDistance(dbid.LineIdOf_SANYOSHINKANSEN, dbid.StationIdOf_SHINOSAKA, route.get(lastIndex - 2).stationId).get(0))) {

                RouteUtil.ASSERT ((route.get(route.size() - 1).lineId == dbid.LineIdOf_TOKAIDO));

				/* 山陽新幹線 姫路以遠～新大阪乗換東海道線-大阪着の場合、最後の東海道線-大阪 を西明石 山陽線、東海道線に付け替える */

                route.get(lastIndex - 1).let(new RouteItem(dbid.LineIdOf_SANYOSHINKANSEN, dbid.StationIdOf_NISHIAKASHI));	// 新大阪->西明石
                route.get(lastIndex - 1).flag |= RouteUtil.FLG_HIDE_STATION;
                route.get(lastIndex).flag |= RouteUtil.FLG_HIDE_LINE;	// 東海道線 非表示
                route.add(lastIndex, new RouteItem(dbid.LineIdOf_SANYO, dbid.StationIdOf_KOUBE));
                route.get(lastIndex).flag |= (RouteUtil.FLG_HIDE_LINE | RouteUtil.FLG_HIDE_STATION);

                return 2;
            }
        }
        if (1 <= lastIndex) {
            // 新大阪 発 山陽新幹線
            if ((route.get(0).stationId == dbid.StationIdOf_SHINOSAKA) &&
                    (route.get(1).lineId == dbid.LineIdOf_SANYOSHINKANSEN) &&
                    (chk_distance2 <= RouteUtil.GetDistance(dbid.LineIdOf_SANYOSHINKANSEN, dbid.StationIdOf_SHINOSAKA, route.get(1).stationId).get(0))) {

				/* 大阪発-東海道線上り-新大阪-山陽新幹線 姫路以遠の場合、大阪発-東海道線-山陽線 西明石経由に付け替える */

                route.get(0).let(new RouteItem(0, dbid.StationIdOf_OSAKA));
                route.add(1, new RouteItem(dbid.LineIdOf_SANYO, dbid.StationIdOf_NISHIAKASHI));
                route.get(1).flag |= (RouteUtil.FLG_HIDE_LINE | RouteUtil.FLG_HIDE_STATION);

                route.add(1, new RouteItem(dbid.LineIdOf_TOKAIDO, dbid.StationIdOf_KOUBE));
                route.get(1).flag |= (RouteUtil.FLG_HIDE_LINE | RouteUtil.FLG_HIDE_STATION);

                return 1;

            }	// 山陽新幹線 大阪 着
            else if ((route.get(route.size() - 1).stationId == dbid.StationIdOf_SHINOSAKA) &&
                    (route.get(route.size() - 1).lineId == dbid.LineIdOf_SANYOSHINKANSEN) &&
                    (chk_distance2 <= RouteUtil.GetDistance(dbid.LineIdOf_SANYOSHINKANSEN, dbid.StationIdOf_SHINOSAKA, route.get(lastIndex - 1).stationId).get(0))) {

				/* 山陽新幹線 姫路以遠～新大阪乗換東海道線-大阪着の場合、最後の東海道線-大阪 を西明石 山陽線、東海道線に付け替える */

                route.get(route.size() - 1).let(new RouteItem(dbid.LineIdOf_SANYOSHINKANSEN, dbid.StationIdOf_NISHIAKASHI));	// 新大阪->西明石
                route.get(route.size() - 1).flag |= RouteUtil.FLG_HIDE_STATION;

                route.add(new RouteItem(dbid.LineIdOf_SANYO, dbid.StationIdOf_KOUBE));	// add 山陽線-神戸
                route.get(route.size() - 1).flag |= (RouteUtil.FLG_HIDE_LINE | RouteUtil.FLG_HIDE_STATION);

                route.add(new RouteItem(dbid.LineIdOf_TOKAIDO, dbid.StationIdOf_OSAKA));	// add 東海道線-大阪
                route.get(route.size() - 1).flag |= RouteUtil.FLG_HIDE_LINE;

                return 2;
            }
        }
        return 0;
    }

    //private
    //static
    //	114条のチェック
    //
    //	@param [in] route         経路(86, 87変換前)
    //	@param [in] routeSpecial  経路(86, 87変換後)
    //	@param [in] kind       種別
    //							bit0:発駅が特定都区市内または山手線内
    //							bit1:着駅が特定都区市内または山手線内
    //							bit15:OFF=特定都区市内
    //							      ON =山手線内
    //	@param [out] rule114   (戻り値True時のみ有効) [0]適用運賃(未適用時、0), [1]営業キロ, [2]計算キロ)
    //
    //	@note 86/87適用後の営業キロが200km/100km以下であること.
    //
    private static FARE_INFO.Fare CheckOfRule114j(final LastFlag last_flag, final List<RouteItem> route, final List<RouteItem> routeSpecial, int kind) {

        int dkm;
        int km;				// 100km or 200km
        int aSales_km;		// 86/87 applied
        short line_id;
        short station_id1;
        short station_id2;
        short station_id3;
        //if (RoudeDB.debug) {
        KM km_raw ;		// 86 or 87 適用前 [0]:営業キロ、[1]計算キロ
        //}
        KM km_spe;		// 86 or 87 適用後 [0]:営業キロ、[1]計算キロ
        FARE_INFO fi = new FARE_INFO();
        int fare_normal;			/* 200(100)km以下により, 86 or 87 非適用の通常計算運賃 */
        int fare_applied;			/* より遠い駅までの都区市内発着の仮適用運賃 */

		/* 経路は乗換なしの単一路線 */
        if (route.size() <= 2) {
            return null;
        }

        if (RouteDB.debug) {
            km_raw = Get_route_distance(last_flag, route); 			/* 経路距離 */
        }
        km_spe = Get_route_distance(last_flag, routeSpecial); 	/* 経路距離(86,87適用後) */

        aSales_km = km_spe.sales_km;			// 営業キロ

		/* 中心駅～目的地は、180(90) - 200(100)km未満であるのが前提 */
        if ((0x8000 & kind) == 0) {
            // 200km
            km = 2000;
        } else {
            // 100km
            km = 1000;
        }
		/* 距離があと86、87条適用距離-10kmの範囲内ではない */
        if ((aSales_km < (km * 9 / 10)) || (km < aSales_km)) {
            return null;
        }

        if ((kind & 1) != 0) {		/* 発駅が特定都区市内 */
            line_id = route.get(route.size() - 1).lineId;				// 着 路線 発-着
            station_id1 = route.get(route.size() - 2).stationId;
            station_id2 = route.get(route.size() - 1).stationId;
        } else if ((kind & 2) != 0) { /* 着駅が特定都区市内 */
            line_id = route.get(1).lineId;								// 発 路線.発-着
            station_id1 = route.get(1).stationId;
            station_id2 = route.get(0).stationId;
        } else {
            RouteUtil.ASSERT (false);
            return null;					// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        }
        // ex. 国母-横浜-長津田の場合、身延線.富士-国母 間の距離を引く
        dkm = RouteUtil.GetDistance(line_id, station_id1, station_id2).get(0);
        aSales_km -= dkm;	/* 発駅から初回乗換駅までの営業キロを除いた営業キロ */

        RouteUtil.ASSERT (0 <= aSales_km);
        RouteUtil.ASSERT (0 < dkm);

        if (RouteUtil.LINE_DIR.LDIR_ASC != RouteUtil.DirLine(line_id, station_id1, station_id2)) {
			/* 上り */
            km = -km;
        }
		/* 中心駅から目的地方向に最初に200(100)kmに到達する駅を得る */
		/* 富士から身延線で甲府方向に */
        station_id3 = Retreive_SpecificCoreAvailablePoint(km, aSales_km, line_id, station_id1);

        //#2013.6.28:modified>> RouteUtil.ASSERT (0 < station_id3);
        if (station_id3 <= 0) {
            return null;
        }
        //<<

		/* (86, 87条適用前されなかった)運賃その駅までの運賃より高ければ、その駅までの運賃とする */

        // <- routeSpecial
        List<RouteItem> route_work = dupRouteItems(routeSpecial);

        if ((kind & 1) != 0) {			/* 発駅が特定都区市内 */
            route_work.get(route_work.size() - 1).stationId = station_id3;	/* 最終着駅を置き換える */
        } else if ((kind & 2) != 0) {	/* 着駅が特定都区市内 */
            route_work.get(0).stationId = station_id3;	/* 始発駅を置き換える */
        } else {
            RouteUtil.ASSERT (false);
            return null;					// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        }

		/* 通常運賃を得る */
        if (!fi.calc_fare(last_flag, route, route)) {
            RouteUtil.ASSERT (false);
            return null;					// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        }
        fare_normal = fi.getFareForJR();			/* 200(100)km以下により, 86 or 87 非適用の通常計算運賃(長津田-横浜-国母) */

        if (RouteDB.debug) {
            RouteUtil.ASSERT (km_raw.sales_km == fi.getJRSalesKm());
            RouteUtil.ASSERT (km_raw.calc_km == fi.getJRCalcKm());
        }
		/* 86,87適用した最短駅の運賃を得る(上例では甲斐住吉-横浜間) */
        if (!fi.calc_fare(last_flag, route_work, route_work)) {
            RouteUtil.ASSERT (false);
            return null;					// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        }
        fare_applied = fi.getFareForJR();			/* より遠い駅までの都区市内発着の仮適用運賃(横浜-甲斐住吉) */

        if (fare_applied < fare_normal) {
			/* 114条適用 */
            System.out.printf("Rule 114 Applied(%d->%d)\n", fare_normal, fare_applied);
			/* 先の駅の86,87適用運賃 */
            return new FARE_INFO.Fare(fare_applied, fi.getJRSalesKm(), fi.getJRCalcKm());
        }
        System.out.printf("Rule 114 no applied\n");
        return null;
    }

    //	Route 営業キロを計算
    //	showFare() => calcFare() => checkOfRuleSpecificCoreLine() =>
    //	CheckOfRule114j() =>
    //
    //	@param [in]  last_flag 大阪環状線通過方向(osakakan_1dir, osakakan_2dir, osakakan_1pass)
    //                         * osakakan_1pass はwork用に使用可
    //	@param [in]  route     計算ルート
    //	@retuen 営業キロ[0] ／ 計算キロ[1]
    //
    private static KM Get_route_distance(final LastFlag last_flag, final List<RouteItem> route) {
        int total_sales_km;
        int total_calc_km;
        int stationId;
        LastFlag oskk_flag;

        total_sales_km = 0;
        total_calc_km = 0;
        stationId = 0;
		/* 大阪環状線flag */

        oskk_flag = last_flag.clone();
        oskk_flag.setOsakaKanPass(false);

        for (RouteItem it : route) {
            if (stationId != 0) {
                List<Integer> vkms;
                if (it.lineId == DbidOf.id().LineIdOf_OOSAKAKANJYOUSEN) {
                    vkms = RouteUtil.GetDistance(oskk_flag, it.lineId, stationId, it.stationId);
                    oskk_flag.setOsakaKanPass(true);
                } else if (it.lineId == RouteUtil.ID_L_RULE70) {
                    vkms = new ArrayList<Integer>(2);
                    vkms.add(FARE_INFO.Retrieve70Distance(stationId, it.stationId));
                    vkms.add(vkms.get(0));
                } else {
                    vkms = RouteUtil.GetDistance(it.lineId, stationId, it.stationId);
                }
                RouteUtil.ASSERT (vkms.size() == 2);
                total_sales_km += vkms.get(0);
                total_calc_km  += vkms.get(1);
            }
            stationId = it.stationId;
        }
        return new KM(total_sales_km, total_calc_km);
    }


    //static
    //	指定路線・駅からの100/200km到達地点の駅を得る
    //	CheckOfRule114j() =>
    //
    //	@param [in] km  下り: 100=1000, 200=2000(上り: -1000 / -2000)
    //	@param [in] line_id   路線
    //	@param [in] station_id   起点駅
    //	@retval 到達駅id. 0はなし
    //
    static short Retreive_SpecificCoreAvailablePoint(int km, int km_offset, int line_id, int station_id) {
        // 上り方向で200km到達地点
        final String tsql_desc =
                "select l2.sales_km - l1.sales_km+%d as sales_km, l1.station_id" +
                        " from  t_lines l1 left join t_lines l2" +
                        " where l1.line_id=?1 and l2.line_id=?1" +
                        " and   l2.sales_km>l1.sales_km" +
                        " and   l2.station_id=?2" +
                        " and	(l1.lflg&(1<<31))=0" +
                        " and   (l2.sales_km-l1.sales_km)>%d order by l1.sales_km desc limit(1)";

        // 下り方向で200km到達地点
        final String tsql_asc =
                "select l1.sales_km-l2.sales_km+%d as sales_km, l1.station_id" +
                        " from  t_lines l1 left join t_lines l2" +
                        " where l1.line_id=?1 and l2.line_id=?1" +
                        " and   l1.sales_km>l2.sales_km" +
                        " and   l2.station_id=?2" +
                        " and	(l1.lflg&(1<<31))=0" +
                        " and   (l1.sales_km-l2.sales_km)>%d order by l1.sales_km limit(1)";

        String sql_buf;	//[512];

        if (km < 0) {
            km = -km;	/* 上り */
            sql_buf = String.format(tsql_desc, km_offset, km - km_offset);
        } else {		/* 下り */
            sql_buf = String.format(tsql_asc, km_offset, km - km_offset);
        }
        //int akm;
        short aStationId = 0;
        Cursor dbo = RouteDB.db().rawQuery(sql_buf, new String[]{String.valueOf(line_id),
                String.valueOf(station_id)});

        try {
            if (dbo.moveToNext()) {
                //akm = dbo.getInt(0);		// 未使用(営業キロじゃ貰っても要らん)
                aStationId = dbo.getShort(1);
            }
        } finally {
            dbo.close();
        }
        return aStationId;
    }





    //static
    //	69条適用
    //	in_route_listを適用してroute_list_cookedに設定する
    //	out_route_listは表示用ではなく計算用
    //	showFare() => calcFare() => checkOfRuleSpecificCoreLine() =>
    //
    //	@param [in] in_route_list  変換前のチェック経路
    //	@param [out] out_route_list 変換後のチェック経路
    //	@retval 0: N/A
    //	@retval 0<: replace for number of route
    //
    static int ReRouteRule69j(final List<RouteItem> in_route_list, List<RouteItem> out_route_list) {
        int change;
        boolean continue_flag;
        int station_id1;
        int a69_line_id;
        List<Integer[][]> dbrecord = new ArrayList<Integer[][]>(0);	// LO:stid1, HI:flg
        List<Integer[]> a69list = new ArrayList<Integer[]>(0);		// LO:stid1, HI:stid2
        List<Integer[]> trule69list = new ArrayList<Integer[]>(0);	//

        change = 0;

		/* dup copy in_route_list -> out_route_list */
        out_route_list.clear();
        cpyRouteItems(in_route_list, out_route_list);

        continue_flag = false;
        station_id1 = 0;

        for (int i = 0; i < out_route_list.size(); i++) {
            if (station_id1 != 0) {
                a69_line_id = out_route_list.get(i).lineId;
                continue_flag = Query_a69list(a69_line_id, station_id1, out_route_list.get(i).stationId, /*out*/dbrecord, continue_flag);
                int dbrec_ite = 0;
                for (Integer[][] dbrec : dbrecord) {
                    // 最後のアイテムでない or !continue_flag
                    dbrec_ite++;
                    if ((dbrec_ite != dbrecord.size()) || !continue_flag) {
                        a69list.add(new Integer[] {dbrec[0][0], dbrec[1][0]});
                        // 置換処理
                        Query_rule69t(in_route_list, out_route_list.get(i), dbrec[0][1], trule69list);
                        if (1 == trule69list.size()) {
                            if (0 == a69list.size()) {
                                RouteUtil.ASSERT (false);
                            } else if (1 == a69list.size()) {
								/* 置換処理1 */
                                if (out_route_list.get(i).stationId == a69list.get(0)[1]) { /* a69list.station2 ? */
                                    out_route_list.get(i).lineId = trule69list.get(0)[0].shortValue();
                                    if (station_id1 != a69list.get(0)[0]) {       /* a69list.station1 ? */
										/* 乗車路線, 駅2を挿入 */
                                        out_route_list.add(i, new RouteItem(a69_line_id, a69list.get(0)[0]));
                                        i++;
                                    }
                                } else {
                                    if (station_id1 != a69list.get(0)[0]) { /* a69list.station1 ? */
										/* 乗車路線, 駅1を挿入 */
                                        out_route_list.add(i, new RouteItem(a69_line_id, a69list.get(0)[0]));
                                        i++;
                                    }
									/* 計算路線、駅2を挿入 */
                                    out_route_list.add(i, new RouteItem(trule69list.get(0)[0], a69list.get(0)[1]));
                                    i++;
                                }
                            } else if (2 <= a69list.size()) {
								/* 置換処理3 */
                                if (out_route_list.get(i).stationId == a69list.get(1)[1].shortValue()) {
                                    out_route_list.get(i).lineId = trule69list.get(0)[0].shortValue();
                                    //out_route_list.get(i).stationId = a69list.get(1)[1];
                                    if (out_route_list.get(i - a69list.size()).stationId == a69list.get(0)[0]) {
                                        // c) in-in
                                        int j = a69list.size() - 1;
                                        i -= j;
                                        for ( ; 0 < j; j--) {
                                            out_route_list.remove(i);
                                        }
                                    } else { // d) out-in
                                        out_route_list.get(i - 1).stationId = a69list.get(0)[0].shortValue();
                                    }
                                } else if (out_route_list.get(i - a69list.size()).stationId == a69list.get(0)[0]) {
									/* b) in -> out */
                                    out_route_list.get(i - 1).lineId = trule69list.get(0)[0].shortValue();
                                    out_route_list.get(i - 1).stationId = a69list.get(a69list.size() - 1)[1].shortValue();
                                    if (2 < a69list.size()) {
                                        int j = a69list.size() - 2;
                                        i -= j;
                                        for ( ; 0 < j; j--) {
                                            out_route_list.remove(i);
                                        }
                                        // 3経路以上は現状なし
                                    }
                                } else {
									/* a) out . out */
                                    out_route_list.add(i, new RouteItem(trule69list.get(0)[0], a69list.get(a69list.size() - 1)[1]));
                                    i++;
                                    out_route_list.get(i - a69list.size()).stationId = a69list.get(0)[0].shortValue();
                                    if (2 < a69list.size()) {
                                        int j = (int)a69list.size() - 1;
                                        i -= j;
                                        j--;
                                        for ( ; 0 < j; j--) {
                                            out_route_list.remove(i);
                                        }
                                        // 3経路以上は現状なし
                                    }
                                }
                            } else {
                                RouteUtil.ASSERT (false);	/* not implement for 3 <= a69list.size() */
                            }
                        } else if (0 == trule69list.size()) {
                            // RouteUtil.ASSERT (false);
                            // 69条 「…かつこ内の両線路にまたがる場合を除いて」の場合
                        } else if (2 == trule69list.size()) {
							/* 置換処理2 */
                            if (a69list.get(0)[0].intValue() == trule69list.get(0)[1].intValue()) {
                                // 下り線
                                out_route_list.get(i).lineId = trule69list.get(1)[0].shortValue();
                                out_route_list.add(i, new RouteItem(trule69list.get(0)[0], trule69list.get(0)[2]));
                                i++;
                            } else {
                                // 上り線
                                out_route_list.get(i).lineId = trule69list.get(0)[0].shortValue();
                                out_route_list.add(i, new RouteItem(trule69list.get(1)[0], trule69list.get(1)[1]));
                                i++;
                            }
                        } else {
                            RouteUtil.ASSERT (false);	/* 0 or <=2 then failured */
                        }
                        a69list.clear();
                        change++;
                    }
                    if (continue_flag) {
                        //RouteUtil.ASSERT ((2 <= dbrecord.size()) && ((size_t)i == (dbrecord.size() - 2)));
                        a69list.add(new Integer[] {dbrec[0][0], dbrec[1][0]});
                    }
                    else {	// continue_flag
                        a69list.clear();
                    }
                }//for dbrec
            }
            station_id1 = out_route_list.get(i).stationId;
        } // for
        RemoveDupRoute(out_route_list);
        System.out.printf(change == 0 ? "noapplid rule69(%d)" : "applied rule69 count=%d\n", change);
        return change;
    }



    //static
    //	70条適用
    //	route_listを適用してout_route_listに設定する
    //	showFare() => calcFare() => checkOfRuleSpecificCoreLine() =>
    //
    //	out_route_listは表示用ではなく計算用(路線が70条専用がある)
    //
    //	@param [in]   in_route_list  経路
    //	@param [out]  out_route_list 経路
    //	@retval 0: done
    //	@retval -1: N/A
    //
    static int ReRouteRule70j(final List<RouteItem> in_route_list, List<RouteItem> out_route_list) {
        int stage;
        int stationId_o70 = 0;
        int flag = 0;

        stage = 0;

        for (RouteItem route_item : in_route_list) {
            boolean skip = false;
            RouteItem ri = new RouteItem(route_item);

            if (stage == 0) {
                if ((route_item.flag & (1 << RouteUtil.BCRULE70)) == 0) {
                    stage = 1;					/* 1: off */
                } else {
                    break;		/* 70条適用エリア内発は非適用(86、87条) */
                }
            } else if (stage == 1) {
                if ((route_item.flag & (1 << RouteUtil.BCRULE70)) != 0) {
                    stage = 2;					/* 2: on */ /* 外から進入した */
									/* 路線より最外側の大環状線内(70条適用)駅を得る */
                    stationId_o70 = RetrieveOut70Station(route_item.lineId);
                    RouteUtil.ASSERT (0 < stationId_o70);
                    ri.stationId = (short)stationId_o70;
                    flag = route_item.flag;
                } else {	// 外のまま
					/* do nothing */
                }
            } else if (stage == 2) {
                if ((route_item.flag & (1 << RouteUtil.BCRULE70)) == 0) {
                    int stationId_tmp;
                    stage = 3;					/* 3: off: !70 -> 70 -> !70 (applied) */
									/* 進入して脱出した */
									/* 路線より最外側の大環状線内(70条適用)駅を得る */
                    stationId_tmp = RetrieveOut70Station(route_item.lineId);
                    if (stationId_tmp <= 0)
                    RouteUtil.ASSERT (0 < stationId_tmp);
                    if (stationId_tmp != stationId_o70) {
                        out_route_list.add(new RouteItem(RouteUtil.ID_L_RULE70, (short)stationId_tmp, flag));
                    }
                } else {	// 中のまま
                    skip = true;
                    flag = route_item.flag;
                }
            } else if (stage == 3) {
				/* 4 */
                if ((route_item.flag & (1 << RouteUtil.BCRULE70)) != 0) {
                    stage = 4;				// 進入して脱出して(通過して)また進入したら70条非適用
											/* !70 -> 70 -> !70 -> 70 (exclude) */
                    break;
                } else {
					/* 70条通過(通過して外に出ている */
					/* do nothing */
                }
            } else {	/* -1: 発駅から70条適用駅 */
				/* do nothing */
            }
            if (!skip) {
                out_route_list.add(ri);
            }
        }

        switch (stage) {
            case 0:
                // 非適合
                // 0: 70条適用駅発
                out_route_list.clear();
                cpyRouteItems(in_route_list, out_route_list);
                break;
            case 1:		// 1: 70条適用駅なし(都内を通らない多くのルート)
                return -1;
            //break;
            case 3:		/* 70条適用駅を通過 */
                return 0;	// done
            //break;
            case 2:		// 2: 70条適用駅着(発駅は非適用駅)
            case 4:		// 4: 70条適用駅区間を通過して再度進入した場合
            default:
			/* cancel */
                out_route_list.clear();
                cpyRouteItems(in_route_list, out_route_list);
                break;
        }
        return -1;
    }


    //static
    //	69条適用駅を返す
    //	端駅1～端駅2のペア×Nをかえす.
    //	results[]配列は必ずn*2を返す
    //
    //	showFare() => calcFare() => checkOfRuleSpecificCoreLine() => ReRouteRule69j()=>
    //
    //	coninue_flag: false 続き無し
    //                true  続き
    //	return: false 続き無し
    //          true  続き
    static boolean Query_a69list(int line_id, int station_id1, int station_id2, List<Integer[][]> results, boolean continue_flag) {
        final String tsql =
                //" select station_id, (lflg>>0)&15, (lflg>>24)&1,  (lflg>>23)&1"
                " select station_id, lflg" +
                        " from t_lines" +
                        " where line_id=?1" +
                        " and (lflg&((1<<31)|(1<<24)))=(1<<24)" +
                        " and station_id" +
                        " in (select station_id" +
                        "      from t_lines" +
                        "      where line_id=?1" +
                        "      and ((sales_km>=(select sales_km" +
                        "                   from t_lines" +
                        "                   where line_id=?1" +
                        "                   and station_id=?2)" +
                        "      and  (sales_km<=(select sales_km" +
                        "                   from t_lines" +
                        "                   where line_id=?1" +
                        "                   and station_id=?3)))" +
                        "      or  (sales_km<=(select sales_km" +
                        "                  from t_lines" +
                        "                  where line_id=?1" +
                        "                  and station_id=?2)" +
                        "      and (sales_km>=(select sales_km" +
                        "                  from t_lines" +
                        "                  where line_id=?1" +
                        "                  and station_id=?3)))))" +
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
                        " end asc";

        int prev_flag = 0;
        int prev_stid = 0;
        int cur_flag;
        int cur_stid;
        boolean next_continue = false;

        List<Integer[]> pre_list = new ArrayList<Integer[]>();

        results.clear();

        System.out.printf("c69 line_id=%d, station_id1=%d, station_id2=%d\n", line_id, station_id1, station_id2);

        Cursor ctx = RouteDB.db().rawQuery(tsql, new String[] {String.valueOf(line_id),
                String.valueOf(station_id1),
                String.valueOf(station_id2)});
        try {
            while (ctx.moveToNext()) {
                cur_stid = ctx.getInt(0);
                cur_flag = ctx.getInt(1);
                // bit23 -> bit15 | bit3-0
                cur_flag = ((cur_flag >>> (BSR69CONT - 15)) & (1 << 15)) | (cur_flag & 0x0f);
                pre_list.add(new Integer[] {cur_stid, cur_flag});
            }
            cur_stid = 0;
            cur_flag = 0;
            for (Integer[] it : pre_list) {
                cur_stid = it[0];
                cur_flag = it[1];

                if ((prev_flag & 0x0f) == (cur_flag & 0x0f)) {
                    // OK
                    if (((cur_flag & (1 << 15)) != 0) && //((it + 1) == pre_list.cend()) &&
                            (station_id2 == cur_stid)) { /* 最後でBSR69CONT=1 */
                        next_continue = true;
                    } else if ((cur_flag & (1 << 15)) != 0) {
                        // NG
                        prev_flag = 0;
                        continue;
                    }
                    results.add(new Integer[][] {{prev_stid, (prev_flag & 0x0f)},
                            {cur_stid, (cur_flag & 0x0f)}});
                    prev_flag = 0;

                    System.out.printf("c69             station_id1=%s(%d), station_id2=%s(%d)\n", RouteUtil.StationName(prev_stid), prev_stid, RouteUtil.StationName(cur_stid), cur_stid);

                    if (((cur_flag & (1 << 15)) != 0) &&
                            // ((it + 1) == pre_list.cend()) &&
                            (station_id2 == cur_stid)) { /* 最後でBSR69CONT=1 */
                        next_continue = true;
                    } else {

                    }
                } else {
					/* 継続以外で継続フラグ付BSR69TERMは無効*/
                    if ((continue_flag && ((cur_flag & (1 << 15)) != 0) && (station_id1 == cur_stid)) ||
                            (!continue_flag && ((cur_flag & (1 << 15)) == 0))) {
                        // OK
                        prev_flag = cur_flag;
                        prev_stid = cur_stid;
                    } else {
                        // NG skip
                        prev_flag = 0;
                        prev_stid = 0;
                    }
                    continue_flag = false;	/* 1回目以外はOff */
                }
            }
        } finally {
            ctx.close();
        }
        return next_continue;
    }

    //static
    //	69条置換路線、駅を返す
    //
    static boolean Query_rule69t(final List<RouteItem> in_route_list, final RouteItem cur, int ident, List<Integer[]> results) {
        int lineId;
        int stationId1;
        int stationId2;
        int in;
        int in1;
        int s1;
        int s2;
        final String tsql =
                " select line_id, station_id1, station_id2 from t_rule69 where id=? order by ord";

        results.clear();

        Cursor ctx = RouteDB.db().rawQuery(tsql, new String[] {String.valueOf(ident)});
        try {
            while (ctx.moveToNext()) {
                results.add(new Integer[] {ctx.getInt(0), ctx.getInt(1), ctx.getInt(2)});
            }

            s1 = 0;
            in1 = 0;
            int leng = results.size();
            RouteUtil.ASSERT (0 < leng);
            leave: for (RouteItem route_item : in_route_list) {
                s2 = route_item.stationId;
                if (s1 != 0) { /* 2回目以降 ? */
                    if (!cur.is_equal(route_item)) {
                        for (int i = 0; i < leng; i++) {
                            lineId = results.get(i)[0];
                            stationId1 = results.get(i)[1];
                            stationId2 = results.get(i)[2];
                            in = RouteUtil.InStation(s2, lineId, stationId1, stationId2);
                            in1 = RouteUtil.InStation(s1, lineId, stationId1, stationId2);
							/* 2駅とも置換路線上にあれば置換しない
							 * 1駅のみ置換路線上にある場合、その駅が端駅でなければ置換しない
							 */
                            if (leng == 1) {
                                if (((in != 0) && (in1 != 0)) ||
                                        ((in  != 0) && ((s2 != stationId1) && (s2 != stationId2))) ||
                                        ((in1 != 0) && ((s1 != stationId1) && (s1 != stationId2)))) {
                                    results.clear();	/* 置換対象外とする */
                                    break leave;
                                }
                            } else if (i == 0) {
                                if (((in != 0) && (in1 != 0)) ||
                                        ((in  != 0) && (s2 != stationId1)) ||
                                        ((in1 != 0) && (s1 != stationId1))) {
                                    results.clear();	/* 置換対象外とする */
                                    break leave;
                                }
                            } else if (i  == (leng - 1)) {
								/* last */
                                if (((in != 0) && (in1 != 0)) ||
                                        ((in  != 0) && (s2 != stationId2)) ||
                                        ((in1 != 0) && (s1 != stationId2))) {
                                    results.clear();	/* 置換対象外とする */
                                    break leave;
                                }
                            } else {
                                if ((in != 0) || (in1 != 0)) {
                                    results.clear();	/* 置換対象外とする */
                                    break leave;
                                }
                            }
                        }
                    }
                }
                s1 = s2;
            }
        } finally {
            ctx.close();
        }
        return 0 < results.size();
    }


    //static
    //	86条適用されるかのチェック
    //	showFare() => calcFare() => checkOfRuleSpecificCoreLine() =>
    //
    //	@param [in]  in_route_list 経路
    //  @param [in]  last_flag     フラグ(jrtokaistock_applied)
    //	@param [out] exit          脱出路線・駅
    //	@param [out] entr          進入路線・駅
    //	@param [out] cityId_pair   IDENT1(発駅特定都区市内Id), IDENT2(着駅特定都区市内Id)
    //
    //	@return	0: 非適応(両端単駅)              -
    //			1: 発駅が特定都区市内            exit有効
    //			2: 着駅が特定都区市内            entr有効
    //			3: 発駅≠着駅とも特定都区市内    entr, exit 有効
    //		 0x83: 発駅=着駅=特定都区市内        entr, exit 有効
    //			4: 全駅同一特定都区市内          -
    //		0x8000_0000 : jrtokaistock_enable JR東海株主優待券使用メニュー有効可否フラグ
    //
    //	 [九] 山陽新幹線
    //	 [福] 鹿児島線
    //	 [神] 山陽新幹線
    //	 [札] 函館線
    //	 [仙] 東北新幹線、東北線
    //	 [広] 山陽新幹線、山陽線
    //	 [阪] 東海道線
    //	 [京] 東海道線
    //	 [名] 東海道線
    //	 [浜] 東海道線
    //	 と、都区内以外はみんなあり得る
    // 同一線で都区市内駅(o)のパターン
    //	--o-- 上記一覧
    //	ooooo あり(問題なし)
    //	o---- 通常
    //	----o 通常
    //	o---o なし(乗車駅または分岐駅～分岐駅または降車駅が都区市内だが間に非都区市内が含まれる例はなし。
    //
    static int CheckOfRule86(final RouteItem[] in_route_list, final LastFlag last_flag, Station exit, Station entr, int[] cityId_pair) {
        DbidOf dbid = new DbidOf();
        int city_no_s;
        int city_no_e;
        int c;
        int r;
        Station in_line = new Station();
        Station out_line = new Station();
        int stationId = 0;
        int lineId = 0;

        r = 0;
        if (0 == in_route_list.length) {
            return 0;	/* empty */
        }
        city_no_s = RouteUtil.MASK_CITYNO(in_route_list[0].flag);
        // 発駅が尼崎の場合大阪市内発ではない　基153-2
        if ((city_no_s == RouteUtil.CITYNO_OOSAKA) && (dbid.StationIdOf_AMAGASAKI == in_route_list[0].stationId)) {
            city_no_s = 0;
        } else if ((city_no_s != 0) && (city_no_s != RouteUtil.CITYNO_NAGOYA)) {
    		/* "JR東海株主優待券使用"指定のときは適用条件可否適用 */
    		r |= 0x80000000; // BIT_ON(last_flag, jrtokaistock_enable); // for UI
    		if (last_flag.jrtokaistock_applied) { /* by user */
    			city_no_s = 0;
    		}
    	}

        city_no_e = RouteUtil.MASK_CITYNO(in_route_list[in_route_list.length - 1].flag);
        // 着駅が尼崎の場合大阪市内着ではない　基153-2
        if ((city_no_e == RouteUtil.CITYNO_OOSAKA) &&
                (dbid.StationIdOf_AMAGASAKI == in_route_list[in_route_list.length - 1].stationId)) {
            city_no_e = 0;
        }
    	else if ((city_no_e != 0) && (city_no_e != RouteUtil.CITYNO_NAGOYA)) {
    		/* "JR東海株主優待券使用"指定のときは適用条件可否適用 */
    		r |= 0x80000000; // BIT_ON(last_flag, jrtokaistock_enable); // for UI
    		if (last_flag.jrtokaistock_applied) {
    			city_no_e = 0;
    		}
    	}

        cityId_pair[0] = city_no_s;
        cityId_pair[1] = city_no_e;

        if (city_no_s != 0) {
            // 発駅=特定都区市内
            c = 0;
            stationId = in_route_list[0].stationId;	// 発
            for (int fite = 1; fite < in_route_list.length; fite++) {
                int cno = RouteUtil.MASK_CITYNO(in_route_list[fite].flag);
                if (c == 0) {
                    if (cno != city_no_s) {
                        c = 1;			// 抜けた
                        out_line.set(in_route_list[fite]);
                    }
                } else {
                    if (cno == city_no_s) {
                        if (c == 1) {
                            c = 2;		// 戻ってきた
                            in_line.set(in_route_list[fite]);
                        } else {
                            RouteUtil.ASSERT (c == 2);
                        }
                    } else {
                        if ((c == 2) || (0 < InCityStation(city_no_s,
                                in_route_list[fite].lineId,
                                stationId,
                                in_route_list[fite].stationId))) {
                            c = 3;		// 戻ってまた抜けた
                            break;
                        }
                        RouteUtil.ASSERT (c == 1);
                    }
                }
                stationId = in_route_list[fite].stationId;	// 発
            }
            switch (c) {
                case 0:
                    return 4;	/* 全駅特定都区市内 */
                //break;
                case 1:
                    r &= ~0xffff;
        			r |= 1;		/* 発駅特定都区市内 */
                    exit.let(out_line);
                    break;
                case 2:
                    exit.let(out_line);
                    entr.let(in_line);
                    return 0x83;	/* 発駅=着駅=特定都区市内 */
                //break;
                case 3:
                    // 8の字(いったん離れて戻ってきてまた離れた) (単駅(特定都区市内無効))
                    break;
                default:
                    RouteUtil.ASSERT (false);
                    break;
            }
        }

        if (city_no_e != 0) {
            // 着駅=特定都区市内
            c = 0;
            in_line.set(in_route_list[in_route_list.length - 1]);
            for (int rite = in_route_list.length - 2; 0 <= rite; rite--) {
                int cno = RouteUtil.MASK_CITYNO(in_route_list[rite].flag);
                if (c == 0) {
                    if (cno != city_no_e) {
                        c = 1;			//
                    } else {
                        in_line.set(in_route_list[rite]);
                    }
                } else {
                    if (cno == city_no_e) {
                        if (c == 1) {
                            c = 2;
                        } else {
                            RouteUtil.ASSERT (c == 2);
                        }
                    } else {
                        if ((c == 2) || (0 < InCityStation(city_no_e, lineId, stationId, in_route_list[rite].stationId))) {
                            c = 3;
                            break;
                        }
                        RouteUtil.ASSERT (c == 1);
                    }
                }
                stationId = in_route_list[rite].stationId;	// 発
                lineId = in_route_list[rite].lineId;
            }
            switch (c) {
                case 1:
                    entr.let(in_line);
                    r |= 0x02;	/* 着駅特定都区市内(発駅≠着駅) */
                    break;
                case 3:
                    // 8の字(いったん離れて戻ってきてまた離れた) (単駅(特定都区市内無効))
                    break;
                default:
                    RouteUtil.ASSERT (false);
                    break;
            }
        }
        return r;
    }

    //static
    // 山手線内発着かをチェック
    //	showFare() => calcFare() => checkOfRuleSpecificCoreLine() =>
    //
    //	@param [in] in_route_list  route
    //	@return 0  : 発着ではない
    //			1  : 発駅は山手線内
    //			2  : 着駅は山手線内
    //			3  : (返すことはない)
    //		 0x83  : 発着駅山手線内
    //		    4  : 全駅駅山手線内
    //
    static int CheckOfRule87(final RouteItem[] in_route_list) {
        int c;
        int r;

        r = 0;

        if ((0 != in_route_list.length) && /* 発駅適用 ？ */
                (((in_route_list[0].flag & ((1 << 10) | (1 << 5))) == ((1 << 10)|(1 << 5))))) {
            // start=山手線
            c = 0;
            for (int fite = 1; fite < in_route_list.length; fite++) {
                if (c == 0) {
                    if (((in_route_list[fite].flag & ((1 << 10) | (1 << 5))) != ((1 << 10)|(1 << 5)))) {
                        c = 1;	// 抜けた
                    }
                } else {
                    if (((in_route_list[fite].flag & ((1 << 10) | (1 << 5))) == ((1 << 10)|(1 << 5)))) {
                        // 山手線内
                        if (c == 1) {
                            c = 2;
                        } else {
                            RouteUtil.ASSERT (c == 2);
                        }
                    } else {
                        if (c == 2) {
                            c = 3;
                            break;
                        }
                    }
                }
            }
            switch (c) {
                case 0:
                    return 4;		/* 全駅山手線内 */
//				break;
                case 1:
                    r = 1;			/* 発駅山手線内 */
                    break;
                case 2:
                    return 0x83;	/* 発駅=着駅=山手線内 */
//				break;
                case 3:
                    // 8の字(いったん離れて戻ってきてまた離れた) (単駅(特定都区市内無効))
                    return 0;		/* 単駅とする */
//				break;
                default:
                    RouteUtil.ASSERT (false);
                    break;
            }
        }
        if ((0 != in_route_list.length) && /* 着駅適用 ? */
                (((in_route_list[in_route_list.length - 1].flag & ((1 << 10) | (1 << 5))) == ((1 << 10)|(1 << 5))))) {
            // 着駅=山手線内
            c = 0;
            for (int rite = in_route_list.length - 2; 0 <= rite; rite--) {
                if (((in_route_list[rite].flag & ((1 << 10) | (1 << 5))) == ((1 << 10)|(1 << 5)))) {
							/* 山手線内 */
                    if (c == 0) {
                        // continue (c == 0)
                    } else {
                        c = 3;
                        break;
                    }
                } else {	/* 山手線外 */
                    if (c == 0) {
                        c = 1;		/* take */
                    } else {	// c == 1
                        // continue
                    }
                }
            }
            switch (c) {
                case 1:
                    RouteUtil.ASSERT (r == 0);		/* 上で0x83で返しているはずなので */
                    r |= 0x02;			/* 着駅山手線内 */
                    break;
                case 3:
                    // 8の字(いったん離れて戻ってきてまた離れた) (単駅(特定都区市内無効))
                    break;				/* 単駅 */
                default:
                    RouteUtil.ASSERT (false);
                    break;
            }
        }
        return r;
    }

    //static
    //	86, 87条適用変換
    //	showFare() => calcFare() => checkOfRuleSpecificCoreLine() =>
    //	route_list_raw => route_list_cooked
    //
    //	@param [in]     cityId      core id
    //
    //	@param [in]     mode
    //						0: 非適応(両端単駅)              steady
    //						1: 発駅が特定都区市内            modified
    //						2: 着駅が特定都区市内            modified
    //						3: 発駅≠着駅とも特定都区市内    modified
    //					 0x83: 発駅=着駅=特定都区市内        modified
    //						4: 全駅同一特定都区市内          steady
    //
    //	@param [in]     exit            脱出路線・駅
    //	@param [in]     entr            進入路線・駅
    //	@param [in/out] out_route_list  変換元・変換先経路
    //
    //	@remark このあと69条適用が必要(69条適用後でも可)
    //	@remark ルール未適用時はroute_list_cooked = route_list_rawである
    //
    static void  ReRouteRule86j87j(int[] cityId, int mode, final Station exit, final Station enter, /*out*/List<RouteItem> out_route_list) {
        int coreStationId;
        boolean skip;
        int lineId;
        List<RouteItem> work_route_list = new ArrayList<RouteItem>();
        List<Station> firstTransferStation = new ArrayList<Station>();
        int c;   // work counter
        int n;   // work counter

        if ((mode & 1) != 0) {
			/* 発駅-脱出: exit 有効*/		/* ex) [[東北線、日暮里]] = (常磐線, [区]) */
            firstTransferStation = SpecificCoreAreaFirstTransferStationBy(exit.lineId, cityId[0]);
            if (firstTransferStation.size() <= 0) {
		        /* テーブル未定義 */
                cpyRouteItems(out_route_list, work_route_list);
            } else {
                if (exit.lineId == firstTransferStation.get(firstTransferStation.size() - 1).lineId) {
                    work_route_list.add(new RouteItem(0, firstTransferStation.get(firstTransferStation.size() - 1).stationId));	/* 発駅:都区市内代表駅 */
                    // RouteUtil.ASSERT (firstTransferStation[firstTransferStation.length - 1].stationId == Retrieve_SpecificCoreStation(short(cityId))); (新横浜とか新神戸があるので)
                } else {
                    coreStationId = Retrieve_SpecificCoreStation(cityId[0]);
                    RouteUtil.ASSERT (0 < coreStationId);
                    work_route_list.add(new RouteItem(0, coreStationId));			/* 発駅:都区市内代表駅 */
                    ListIterator rite = firstTransferStation.listIterator(firstTransferStation.size());
                    while (rite.hasPrevious()) {
                        Station st = (Station)rite.previous();
                        work_route_list.add(new RouteItem(st.lineId, st.stationId));
                    }
                }
            }
            skip = true;
            for (RouteItem ri : out_route_list) {
                if (skip) {
                    if (exit.is_equal(ri)) {
                        skip = false;
                        work_route_list.add(ri);
                    }/* else  脱出 路線:駅の前の経路をスキップ*/
                } else {
                    work_route_list.add(ri);
                }
            }
            System.out.printf("start station is re-route rule86/87\n");
        } else {
            cpyRouteItems(out_route_list, work_route_list);
        }

        firstTransferStation.clear();
        out_route_list.clear();

        if ((mode & 2) != 0) {
			/* 着駅-進入: entr 有効 */
            n = 0;
            c = 1;
            for (RouteItem ri : work_route_list) {
                if (enter.is_equal(ri)) {
                    // 京都 東海道線 山科 湖西線 近江塩津 北陸線 富山 高山線 岐阜 東海道線 山科で着駅が[京]にならない不具合
                    n = c;
                }
                ++c;
            }
            c = 1;
            for (RouteItem ri : work_route_list) {
                if ((n == 0) || (c < n)) {
                    out_route_list.add(ri);
                }
                ++c;
            }
            firstTransferStation = SpecificCoreAreaFirstTransferStationBy(enter.lineId, cityId[1]);
            if (firstTransferStation.size() <= 0) {
                // 博多南線で引っかかる RouteUtil.ASSERT (false);
                cpyRouteItems(work_route_list, out_route_list);
            } else {
                if (enter.lineId == firstTransferStation.get(0).lineId) {
                    out_route_list.add(new RouteItem(firstTransferStation.get(0).lineId, firstTransferStation.get(0).stationId));
                    // RouteUtil.ASSERT (firstTransferStation[0].stationId == Retrieve_SpecificCoreStation(cityId[1])); 新横浜とかあるので
                } else {
                    out_route_list.add(new RouteItem(enter.lineId, firstTransferStation.get(0).stationId));
                    lineId = firstTransferStation.get(0).lineId;
                    int firstTransferStationLen = firstTransferStation.size();
                    for (int sta_ite = 1; sta_ite < firstTransferStationLen; sta_ite++) {
                        out_route_list.add(new RouteItem(lineId, firstTransferStation.get(sta_ite).stationId));
                        lineId = firstTransferStation.get(sta_ite).lineId;
                    }
                    coreStationId = Retrieve_SpecificCoreStation(cityId[1]);
                    RouteUtil.ASSERT (0 < coreStationId);
                    out_route_list.add(new RouteItem(lineId, coreStationId));
                }
                System.out.printf("end station is re-route rule86/87\n");
            }
        } else {
            cpyRouteItems(work_route_list, out_route_list);
        }
    }

    //static
    //	経路は近郊区間内にあるか(115条2項check)
    //
    static int InRouteUrban(final List<RouteItem> route_list) {
        short urban = 0;

        Iterator<RouteItem> ite = route_list.iterator();
        if (ite.hasNext()) {
            RouteItem ri = ite.next();
            urban = (short)(ri.flag & RouteUtil.MASK_URBAN);
            while (ite.hasNext()) {
                ri = ite.next();
                if (urban != (RouteUtil.MASK_URBAN & ri.flag)) {
                    urban = 0;
                    break;
                }
            }
        }
        return RouteUtil.URBAN_ID(urban);
    }

    //static
    //	70条進入路線、脱出路線から進入、脱出境界駅と営業キロ、路線IDを返す
    //
    //	@param [in] line_id    大環状線進入／脱出路線
    //	@return 一番外側の大環状線内(70条適用)駅
    //
    //	@note 東京都区内なので、営業キロMAX固定(下り即ち東京から一番遠い70条適用駅)とする
    //
    static int RetrieveOut70Station(int line_id) {
        final String tsql =
                "select station_id from t_lines where line_id=?1 and " +
                        " sales_km=(select max(sales_km) from t_lines where line_id=?1 and (lflg&(1<<31))=0 and" +
                        " exists (select * from t_station where rowid=station_id and (sflg&(1<<6))!=0))";
	/*
	"select	t1.line_id," +
	"	65535&t1.station_id," +
	"	65535&t2.station_id" +
	//"	65535&t2.station_id," +
	//"	(t1.lflg&65535)," +
	//"	(t2.lflg&65535)," +
	//"	t2.sales_km + t1.sales_km" +
	" from	t_lines t1 left join t_lines t2" +
	" on t1.line_id=t2.line_id and" +
	"	(t1.lflg&(1<<24))!=0 and" +
	"	(t2.lflg&(1<<24))!=0 and" +
	"	(t1.lflg&65535)<>(t2.lflg & 65535)" +
	" where" +
	"	(t1.lflg&65535)=?1 and" +
	"	(t2.lflg&65535)=?2";
	*/
        Cursor dbo = RouteDB.db().rawQuery(tsql, new String[] {String.valueOf(line_id)});
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
    //	路線の駅間に都区市内駅はあるか？
    //	     東海道新幹線 京都 米原間に草津駅は存在するとして返します.
    //
    //	@param [in] cityno     検索する都区市内No
    //	@param [in] lineId     路線
    //	@param [in] stationId1 開始駅
    //	@param [in] stationId2 終了駅
    //	@return 0: not found / not 0: ocunt of found.
    //
    static int InCityStation(int cityno, int lineId, int stationId1, int stationId2) {
        final String tsql =
                "select count(*)" +
                        "	from t_lines" +
                        "	where line_id=?1" +
                        "	and (lflg&(1<<31))=0" +
                        "	and exists (select * from t_station where rowid=station_id and (sflg&15)=?4)" +
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
        String sql = tsql.replace("?4", String.valueOf(cityno & 0x0f)); // Android Sqlite bug
        Cursor dbo = RouteDB.db().rawQuery(sql, new String[] {String.valueOf(lineId),
                String.valueOf(stationId1),
                String.valueOf(stationId2)/* Android Sqlite bug,
            String.valueOf(cityno & 0x0f)*/});
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
    //	進入/脱出路線から86条中心駅から最初の乗換駅までの路線と乗換駅を返す
    //	showFare() => calcFare() => checkOfRuleSpecificCoreLine() => ReRouteRule86j87j() =>
    //
    //  @param [in] lineId : 進入／脱出路線
    //	@param [in] cityId : 都区市内Id
    //
    //  @retval vector<Station> : 返却配列(lineId, stationId)
    //
    //	例：lineId:常磐線, cityId:[区] -> 東北線、日暮里
    //
    static List<Station> SpecificCoreAreaFirstTransferStationBy(int lineId, int cityId) {
        final String tsql =
                "select station_id, line_id2 from t_rule86 where" +
                        " line_id1=?1 and (city_id & 255)=?2 order by city_id";

        int station_id;
        int line_id;
        List<Station> firstTransferStation = new ArrayList<Station>();
        String p1 = String.valueOf(lineId);
        String p2 = String.valueOf(cityId);
        String sql = tsql.replace("?2", p2);    // Android Sqlite bug.
        Cursor dbo = RouteDB.db().rawQuery(sql, new String[]{p1}); //, p2});
        try {
            while (dbo.moveToNext()) {
                station_id = dbo.getInt(0);
                line_id = dbo.getInt(1);
                firstTransferStation.add(new Station(line_id, station_id));
            }
        } finally {
            dbo.close();
        }
        return firstTransferStation;
    }

    //static
    //	86条中心駅を返す
    //	showFare() => calcFare() => checkOfRuleSpecificCoreLine() => ReRouteRule86j87j() =>
    //
    //  @param [in]  cityId : 都区市内Id
    //
    //  @return 中心駅
    //
    private static int Retrieve_SpecificCoreStation(int cityId) {
        int stationId = 0;

        final String tsql =
                "select rowid from t_station where" +
                        " (sflg & (1 << 4))!=0 and (sflg & 15)=?1";
        String sql = tsql.replace("?1", String.valueOf(cityId));
        Cursor dbo = RouteDB.db().rawQuery(sql, null); // Android Sqlite Bug // new String[] {String.valueOf(cityId)});
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
    //	重複ルート除去
    //
    //	       勝浦
    //  外房線 蘇我 <- remove
    //  外房線 千葉
    //	総武線 東京
    //
    static int RemoveDupRoute(List<RouteItem> routeList) {
        int affects = 0;
//	#if 1	/* erase(iterator)しかないので */
//		RouteItem[].iterator pos;
//
//		for (pos = routeList.begin(); pos != routeList.end() && (pos + 1) != routeList.end(); ) {
//			if (pos.lineId == (pos + 1).lineId) {
//				pos = routeList.erase(pos);
//				affects++;
//			} else {
//				pos++;
//			}
//		}
//
//	#else	/* erase(index)があればこっちでも可 */
        int last = routeList.size() - 2;
        int idx = 0;

        while (idx <= last) {
            if (routeList.get(idx).lineId == routeList.get(idx + 1).lineId) {
                routeList.remove(idx);
                last--;
                affects++;
            } else {
                idx++;
            }
        }
//	#endif
        return affects;
    }


    //static
    //	並行在来線へ変換
    //	114条判定用
    //  都区市内駅から200kmを超える場合は結果が正しくない場合がある
    //	(東北新幹線 仙台 八戸 など)この場合は前段でチェックされるので
    //  ここにくることはない(114条チェック候補から外れるため)。
    //
    protected  void testConvertShinkansen2ZairaiFor114Judge() {
        List<RouteItem> r = ConvertShinkansen2ZairaiFor114Judge(route_list_raw);
        cpyRouteItems(r, route_list_raw);
    }
    static List<RouteItem> ConvertShinkansen2ZairaiFor114Judge(List<RouteItem> route) {
        int station_id1 = 0;
        int station_id1n = 0;
        int station_id2 = 0;
        int j_station_id = 0;
        int z_station_id = 0;
        int cline_id = 0;
        int bline_id = 0;
        int zline_id = 0;
        int i;
        List<Integer> zroute = new ArrayList<Integer>();
        List<RouteItem> result_route = dupRouteItems(route);
        int replace = 0;

        for (int ite = 0; ite < result_route.size(); ) {
            station_id1n = result_route.get(ite).stationId;
            n1:
            if ((station_id1 != 0) && RouteUtil.IS_SHINKANSEN_LINE(result_route.get(ite).lineId)) {
                List<Integer> zline = RouteUtil.EnumHZLine(result_route.get(ite).lineId, station_id1, station_id1n);
                //System.out.printf("?%d?%d %d %d", zline.size(), zline[0], zline[1], zline[2]);
                if (3 <= zline.size()) {
                    // 並行在来線
                    cline_id = 0;
                    zline_id = 0;
                    j_station_id = 0;
                    zroute.clear();
                    for (i = 1; i < (zline.size() - 1); i++) {
                        zline_id = (0xffff & zline.get(i));
                        station_id2 = (zline.get(i) >>> 16);

                        //    if (i == 0) {
                        //    System.out.printf("++%15s(%d)\t%s(%d)\n", RouteUtil.LineName(zline_id), zline_id, RouteUtil.StationName(station_i    //d2), station_id2);
                        //    } else if (i == (zline.size() - 1)) {
                        //    System.out.printf("--%15s(%d)\t%s(%d)\n", RouteUtil.LineName(zline_id), zline_id, RouteUtil.StationName(station_i    //d2), station_id2);
                        //    } else {
                        //    System.out.printf(".%15s(%d)\t%s(%d)\n", RouteUtil.LineName(zline_id), zline_id, RouteUtil.StationName(station_i    //d2), station_id2);
                        //    }
                        if ((0 < zline_id) && (cline_id != zline_id)) {
                            cline_id = zline_id;
                        } else {
                            zline_id = 0;
                        }

                        if ((zroute.size() % 2) == 0) {
                            // 高崎着
                            if (zline_id != 0) {
                                zroute.add(zline_id);
                                if (station_id2 != 0) {
                                    // 境界駅通過
                                    zroute.add(station_id2);
                                }
                            } else {
                                // 高崎 - 新潟 /
                                j_station_id = station_id2;
                            }
                        } else {
                            if (station_id2 != 0) {
                                // 境界駅通過
                                zroute.add(station_id2);
                                if (zline_id != 0) {
                                    zroute.add(zline_id);
                                } else {
                                    // 大宮－高崎 / 0 - 高崎
                                }
                            } else {
                                // 大宮-新潟 / 信越線 - 0
                                RouteUtil.ASSERT (zroute.get(zroute.size() - 1) == (0xffff & zline.get(i)));
                            }
                        }
                    }
                    // 上毛高原-高崎-xxや、本庄早稲田-高崎-xxはj_station_id=高崎 else j_station_id=0
                    // (xxは高崎かその上毛高原か本庄早稲田)
                    if (j_station_id == 0) {
                        if (0 < zroute.size()) {
                            bline_id = result_route.get(ite).lineId;
                            if (0xffffffff == zline.get(0)) {
                                z_station_id = RouteUtil.NextShinkansenTransferTerm(bline_id, station_id1, station_id1n);
                                if (0 < z_station_id) {
                                    result_route.get(ite).stationId = (short)z_station_id;
                                    result_route.get(ite).refresh();
                                    ite++;
                                    result_route.add(ite, new RouteItem(zroute.get(0), station_id1n));
                                } else {
                                    // 新横浜→品川、本庄早稲田→熊谷
                                    break n1;
                                }
                            } else {
                                z_station_id = 0;
                                result_route.get(ite).lineId = zroute.get(0).shortValue();
                            }

                            if (1 < zroute.size()) {
                                result_route.get(ite).stationId = zroute.get(1).shortValue();
                                result_route.get(ite).refresh();
                                for (i = 2; i < zroute.size() - 1; i += 2) {
                                    ite++;
                                    result_route.add(ite, new RouteItem(zroute.get(i), zroute.get(i + 1)));
                                }
                                if (i < zroute.size()) {
                                    ite++;
                                    result_route.add(ite, new RouteItem(zroute.get(i), station_id1n));
                                }
                            }
                            if (0xffffffff == zline.get(zline.size() - 1)) {
                                station_id2 = RouteUtil.NextShinkansenTransferTerm(bline_id, station_id1n, station_id1);
                                if (0 < station_id2) {
                                    if (z_station_id == station_id2) {
                                        // いわて沼宮内 - 新花巻
                                        result_route.remove(ite - 1);
                                        result_route.get(ite - 1).let(new RouteItem(bline_id, station_id1n));
                                        --replace;
                                    } else {
                                        result_route.get(ite).stationId = (short)station_id2;
                                        result_route.get(ite).refresh();
                                        ite++;
                                        result_route.add(ite, new RouteItem(bline_id, station_id1n));
                                    }
                                } else {
                                    // 品川-新横浜
                                    result_route.get(ite).let(new RouteItem(bline_id, station_id1n));
                                    result_route.get(ite).refresh();
                                }
                            } else {
                                result_route.get(ite).stationId = (short)station_id1n;
                                result_route.get(ite).refresh();
                            }
                            ++replace;
                        } else {
                            RouteUtil.ASSERT (false);
                        }
                    } else {
                        if (0 < zroute.size()) {
                            if (station_id1 == j_station_id) {
                                // 高崎発
                                // k
                                result_route.get(ite).lineId = zroute.get(0).shortValue();
                                if (1 < zroute.size()) {
                                    result_route.get(ite).stationId = zroute.get(1).shortValue();
                                }
                                i = 2;
                            } else {
                                // 高崎の隣の非在来線駅発高崎通過
                                //  x, y, ag
                                result_route.get(ite).stationId = (short)j_station_id;
                                i = 0;
                            }
                            result_route.get(ite).refresh();
                            for (; i < zroute.size() - 1; i += 2) {
                                ite++;
                                result_route.add(ite, new RouteItem(zroute.get(i), zroute.get(i + 1)));
                            }
                            if (i < zroute.size()) {
                                ite++;
                                result_route.add(ite, new RouteItem(zroute.get(i), station_id1n));
                            }
                            ++replace;
                            // n, o
                            // DO NOTHING
                            // l, m,
                            // DO NOTHING
                        }
	/*
						if (station_id1 == j_station_id) {
							// 高崎発
							if (0 < zroute.size()) {
								// k
								ite.lineId = zroute.get(0);
								if (1 < zroute.size()) {
									ite.stationId = zroute.get(1);
									ite.refresh();
									for (i = 2; i < zroute.size() - 1; i += 2) {
										ite++;
										ite = result_route.insert(ite, RouteItem(zroute.get(i), zroute.get(i + 1)));
									}
									if (i < zroute.size()) {
										ite++;
										ite = result_route.insert(ite, RouteItem(zroute.get(i), station_id1n));
									}
									++replace;
								} else {
									// n, o
									// DO NOTHING
								}
							} else {
								// l, m,
								// DO NOTHING
							}
						} else {
							// 高崎の隣の非在来線駅発高崎通過
							if (0 < zroute.size()) {
								//  x, y, ag
								ite.stationId = j_station_id;
								ite.refresh();
								for (i = 0; i < zroute.size() - 1; i += 2) {
									ite++;
									ite = result_route.insert(ite, RouteItem(zroute.get(i), zroute.get(i + 1)));
								}
								if (i < zroute.size()) {
									ite++;
									ite = result_route.insert(ite, RouteItem(zroute.get(i), station_id1n));
								}
								++replace;
							} // else // r, t, v, w
						}
	*/
                    }
	/*
	System.out.printf("\n;;%15s(%d)\t%s(%d)\n", RouteUtil.LineName(result_route.get(result_route.size() - 1).lineId),
	                                    result_route.get(result_route.size() - 1).lineId,
	                                    RouteUtil.StationName(result_route.get(result_route.size() - 1).stationId),
	                                    result_route.get(result_route.size() - 1).stationId);
	 System.out.printf("@");
	 for (i = 0; i < zroute.size(); i++) {
	 System.out.printf("%s, ", ((i % 2) == 0) ? RouteUtil.LineName(zroute.get(i)) : RouteUtil.StationName(zroute.get(i)));
	 }
	 System.out.printf("\n");
	 if (j_station_id != 0) { System.out.printf("/%s/\n", RouteUtil.StationName(j_station_id)); }
	*/
                } else {
                    // 整備新幹線
                    // DO NOTHING
                }
                //System.out.printf("---------------------------------\n");
            }
            //n1:
            station_id1 = station_id1n;
            ite++;
        }
        ////return 0 < replace;

        return result_route;
    }






	/*
		// 大阪環状線 近回り(規定)／遠回り
		if (0 != (RouteUtil.FAREOPT_AVAIL_OSAKAKAN_DETOUR & availbit)) {
			if (IS_LF_OSAKAKAN_PASS(last_flag, LF_OSAKAKAN_1PASS)) {
				if (0 != (cooked & RouteUtil.FAREOPT_OSAKAKAN_DETOUR)) {
					last_flag.osakakan_detour = true;
				} else {
					last_flag = RouteUtil.BIT_OFF(last_flag, osakakan_detour);
				}
			} else {
				RouteUtil.ASSERT (false);
				return;
			}
		}
	*/

	/*	// 大阪環状線方向実装したしlast_flagどうするか困るし未使用だし
	//
	// static
	static String Route_script(final RouteItem[] routeList) {
		String result_str;
		boolean oskk_flag;

		if (routeList.size() == 0) {	// 経路なし(AutoRoute)
			return "";
		}

		RouteItem[].const_iterator pos = routeList.cbegin();

		result_str = RouteUtil.StationNameEx(pos.stationId);

		oskk_flag = false;
		for (pos++; pos != routeList.cend() ; pos++) {
			result_str += ",";
			result_str += RouteUtil.LineName(pos.lineId);
			result_str += ",";
			result_str += RouteUtil.StationNameEx(pos.stationId);
		}
		result_str += "\n\n";
		return result_str;
	}
	*/











/*
// assign
// begin()
//crend()
//cbegin()
// もともとなし printf
//trace
//insert
//erase

//substr
string.find differeced by java


string += stringbuffer

//IDENT1
//IDENT2
//MAKEPAIR
sqlite3_snprintf
sprintf_s
_tcscat_s
NumOf
sizeof
strlen

---- original bug modified ------
original_line_id のassertはバグだぞオリジナルの。
			if (fare_info.isRule114() != 0) {
			if (fare_info.isRule114()) {

FARE_INFO.FareResult
FARE_INFO.Fare
にする。

// 大阪環状線 1回通過で近回り時 bit 2-3
if (IS_LF_OSAKAKAN_PASS(last_flag, LF_OSAKAKAN_1PASS) == LF_OSAKAKAN_1PASS) {
の部分おかしい。Booleanなのだから
#define IS_LF_OSAKAKAN_PASS(m, pass) (pass == ((m) & MLF_OSAKAKAN_PASS))
LF_OSAKAKAN_1PASS=1だから助かっているバグ
pass == で、1が返されて、その1で比較しているからTrueとなる。
if (IS_LF_OSAKAKAN_PASS(last_flag, LF_OSAKAKAN_1PASS)) {
にすべき。

if (RouteUtil.IS_MAIHAN_CITY_START(cooked) == RouteUtil.FAREOPT_APPLIED_START) {
も同様

 ((c & 0x02) && (pass != osakakan_2dir)) ? ">>" : ">",
 v
 ((0 != (c & 0x02)) && (pass != osakakan_2dir)) ? ">>" : ">",

CheckOfRule114j() 戻り値真偽でないのがある

InRouteUrban() 変数未初期化で終わる恐れあり

*/

}
