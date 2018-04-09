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


 class CompanyFare {
     int fare;
     int fareChild;
     int fareAcademic;
     boolean passflg;        // 弊算割引有無
     CompanyFare() { fare = 0; fareChild = 0; fareAcademic = 0; passflg = false; }
 }

public class FARE_INFO {
    /* result_flag bit */
    final static int BRF_COMAPANY_FIRST = 0;	/* 会社線から開始 */
    final static int BRF_COMAPANY_END = 1;		/* 会社線で終了 */
                                                /* 通常OFF-OFF, ON-ONは会社線のみ */
    final static int BRF_COMPANY_INCORRECT = 2;/* 会社線2社以上通過 */
    final static int BRF_ROUTE_INCOMPLETE = 3;	/* 不完全経路(BSRNOTYET_NA) */
    final static int BRF_ROUTE_EMPTY = 4;       /* empty */
    final static int BRF_FATAL_ERROR = 5;       /* fatal error in aggregate_fare_info() */

    FARE_INFO() {
        reset();
    }

    int sales_km;			//*** 有効日数計算用(会社線含む)

    int base_sales_km;		//*** JR本州3社
    int base_calc_km;		//***

    int kyusyu_sales_km;	//***
    int kyusyu_calc_km;		//***

    int hokkaido_sales_km;
    int hokkaido_calc_km;

    int shikoku_sales_km;
    int shikoku_calc_km;

    boolean local_only;				/* True: 地方交通線のみ (0 < base_sales_km時のみ有効)*/
    boolean local_only_as_hokkaido;	/* True: 北海道路線地方交通線のみ(0 < hokkaidou_sales_km時のみ有効) */
    //幹線のみ
    // (base_sales_km == base_calc_km) && (kyusyu_sales_km == kyusyu_calc_km) &&
    // (hokkaido_sales_km == hokkaido_calc_km) && (shikoku_sales_km == shikoku_calc_km)
    boolean major_only;				/* 幹線のみ */
    int total_jr_sales_km;			//***
    int total_jr_calc_km;			//***

    int company_fare;					/* 会社線料金 */
    int company_fare_ac_discount;		/* 学割用会社線割引額 */
    int company_fare_child;				/* 会社線小児運賃 */
    int result_flag;					/* 結果状態: BRF_xxx */

    int flag;						//***/* IDENT1: 全t_station.sflgの論理積 IDENT2: bit16-22: shinkansen ride mask  */
    int jr_fare;					//***
    int fare_ic;					//*** 0以外で有効
    int avail_days;					//***

    int companymask;

    /* 114 */
    int rule114_fare;
    int rule114_sales_km;
    int rule114_calc_km;

    boolean roundTripDiscount;

    String route_for_disp;
    int beginTerminalId;
    int endTerminalId;

    boolean bEnableTokaiStockSelect;

    /* discount */
    static int fare_discount(int fare, int per) {
        return ((fare) / 10 * (10 - (per)) / 10 * 10);
    }
    /* discount 5円の端数切上 */
    static int fare_discount5(int fare, int per) {
        return ((((fare) / 10 * (10 - (per))) + 5) / 10 * 10);
    }

    //static
    //	集計された営業キロ、計算キロより運賃額を算出(運賃計算最終工程)
    //	(JRのみ)
    //	calc_fare() =>
    //
    //	@retval true Success
    //	@retval false Fatal error(会社線のみJR無し)
    //
    private boolean retr_fare() {
        int fare_tmp;
        this.total_jr_sales_km = this.base_sales_km +
                this.kyusyu_sales_km +
                this.hokkaido_sales_km +
                this.shikoku_sales_km;

        this.total_jr_calc_km =this.base_calc_km +
                this.kyusyu_calc_km +
                this.hokkaido_calc_km +
                this.shikoku_calc_km;

        // 本州3社あり？
        if (0 < (this.base_sales_km + this.base_calc_km)) {
            if (RouteUtil.IS_OSMSP(this.flag)) {
                /* 大阪電車特定区間のみ */
                RouteUtil.ASSERT (this.jr_fare == 0); /* 特別加算区間を通っていないはずやねん */
                RouteUtil.ASSERT (this.company_fare == 0);	// 会社線は通ってへん
                RouteUtil.ASSERT (this.base_sales_km == this.total_jr_sales_km);
                RouteUtil.ASSERT (this.base_sales_km == this.sales_km);
                if (RouteUtil.IS_YAMATE(this.flag)) {
                    System.out.printf("fare(osaka-kan)\n");
                    this.jr_fare = Fare_osakakan(this.total_jr_sales_km);
                } else {
                    System.out.printf("fare(osaka)\n");
                    this.jr_fare = Fare_osaka(this.total_jr_sales_km);
                }
            } else if (RouteUtil.IS_TKMSP(this.flag)) {
                /* 東京電車特定区間のみ */
                RouteUtil.ASSERT (this.jr_fare == 0); /* 特別加算区間を通っていないはずなので */
                RouteUtil.ASSERT (this.company_fare == 0);	// 会社線は通っていない
                RouteUtil.ASSERT (this.base_sales_km == this.total_jr_sales_km);
                RouteUtil.ASSERT (this.base_sales_km == this.sales_km);

                if (RouteUtil.IS_YAMATE(this.flag)) {
                    System.out.printf("fare(yamate)\n");
                    fare_tmp = Fare_yamate_f(this.total_jr_sales_km);
                } else {
                    System.out.printf("fare(tokyo)\n");
                    fare_tmp = Fare_tokyo_f(this.total_jr_sales_km);
                }
                if (RouteDB.getInstance().tax() == 5) {
                    this.jr_fare = RouteUtil.round(fare_tmp);
                } else {
                    /* 新幹線乗車はIC運賃適用外 */
                    if (((1 << RouteUtil.BCBULURB) & this.flag) == 0) {
                        this.fare_ic = fare_tmp;
                    }
                    this.jr_fare = RouteUtil.round_up(fare_tmp);
                }
            } else if (this.local_only || (!this.major_only && (this.total_jr_sales_km <= 100))) {
                /* 本州3社地方交通線のみ or JR東+JR北 */
                /* 幹線+地方交通線でトータル営業キロが10km以下 */
                // (i)<s>
                System.out.printf("fare(sub)\n");

                fare_tmp = Fare_sub_f(this.total_jr_sales_km);

                if ((RouteDB.getInstance().tax() != 5) &&
                        IsIC_area(RouteUtil.URBAN_ID(this.flag)) &&   /* 近郊区間(最短距離で算出可能) */
                                                         /* 新幹線乗車はIC運賃適用外 */
                        (((1 << RouteUtil.BCBULURB) & this.flag) == 0)) {

                    RouteUtil.ASSERT (companymask == (1 << (RouteUtil.JR_EAST - 1)));  /* JR East only  */

                    this.fare_ic = fare_tmp;
                }
                this.jr_fare = RouteUtil.round(fare_tmp);

            } else { /* 幹線のみ／幹線+地方交通線 */
                // (a) + this.calc_kmで算出
                System.out.printf("fare(basic)\n");

                fare_tmp = Fare_basic_f(this.total_jr_calc_km);

                if ((RouteDB.getInstance().tax() != 5) && /* IC運賃導入 */
                        IsIC_area(RouteUtil.URBAN_ID(this.flag)) &&   /* 近郊区間(最短距離で算出可能) */
                                                         /* 新幹線乗車はIC運賃適用外 */
                        (((1 << RouteUtil.BCBULURB) & this.flag) == 0)) {

                    RouteUtil.ASSERT (companymask == (1 << (RouteUtil.JR_EAST - 1)));  /* JR East only  */

                    this.fare_ic = fare_tmp;
                }
                this.jr_fare = RouteUtil.round(fare_tmp);
            }

            // JR北あり?
            if (0 < (this.hokkaido_sales_km + this.hokkaido_calc_km)) {
                /* JR東 + JR北 */
                if (this.local_only_as_hokkaido ||
                        (!this.major_only && (this.total_jr_sales_km <= 100))) {
                    /* JR北は地方交通線のみ */
                    /* or 幹線+地方交通線でトータル営業キロが10km以下 */
                    // (r) sales_km add
                    System.out.printf("fare(hla)\n");		// System.out.printf("fare(r,r)\n");
                    this.jr_fare += Fare_table((RouteDB.getInstance().tax() == 5) ? "hla5p" : "hla", "ha",
                            this.hokkaido_sales_km);

                } else { /* 幹線のみ／幹線+地方交通線で10km越え */
                    // (o) calc_km add
                    System.out.printf("fare(add, ha)\n");	// System.out.printf("fare(opq, o)\n");
                    this.jr_fare += Fare_table((RouteDB.getInstance().tax() == 5) ? "add5p" : "add", "ha",
                            this.hokkaido_calc_km);
                }
            }				// JR九あり？
            if (0 < (this.kyusyu_sales_km + this.kyusyu_calc_km)) {
                /* JR西 + JR九 */
                /* 幹線のみ、幹線+地方交通線 */

                // JR九州側(q)<s><c> 加算
                System.out.printf("fare(add, ka)\n");	// System.out.printf("fare(opq, q)\n");
                this.jr_fare += Fare_table((RouteDB.getInstance().tax() == 5) ? "add5p" : "add", "ka",
                        this.kyusyu_calc_km);
            }				// JR四あり?
            if (0 < (this.shikoku_sales_km + this.shikoku_calc_km)) {
                /* JR西 + JR四 */
                /* 幹線のみ、幹線+地方交通線 */

                // JR四国側(p)<s><c> 加算
                System.out.printf("fare(add, sa)\n");	// System.out.printf("fare(opq, p)\n");
                this.jr_fare += Fare_table((RouteDB.getInstance().tax() == 5) ? "add5p" : "add", "sa",
                        this.shikoku_calc_km);
            }				// JR北
        } else if (0 < (this.hokkaido_sales_km + this.hokkaido_calc_km)) {
            /* JR北海道のみ */
            RouteUtil.ASSERT (this.total_jr_sales_km == this.hokkaido_sales_km);
            RouteUtil.ASSERT (this.total_jr_calc_km == this.hokkaido_calc_km);
            RouteUtil.ASSERT (this.jr_fare == 0);

            if (this.local_only_as_hokkaido) {
                /* JR北海道 地方交通線のみ */
                // (j)<s>
                System.out.printf("fare(hokkaido_sub)\n");
                this.jr_fare = Fare_hokkaido_sub(this.total_jr_sales_km);
            } else {
                /* JR北海道 幹線のみ、幹線+地方交通線 */
                // (f)<c>
                System.out.printf("fare(hokkaido-basic)\n");
                this.jr_fare = Fare_hokkaido_basic(this.total_jr_calc_km);

            }				// JR九
        } else if (0 < (this.kyusyu_sales_km + this.kyusyu_calc_km)) {
            /* JR九州のみ */
            RouteUtil.ASSERT (this.total_jr_sales_km == this.kyusyu_sales_km);
            RouteUtil.ASSERT (this.total_jr_calc_km == this.kyusyu_calc_km);
            RouteUtil.ASSERT (this.jr_fare == 0);

            if (this.local_only) {
                /* JR九州 地方交通線 */
                System.out.printf("fare(ls)'k'\n");
                /* (l) */
                this.jr_fare = Fare_table(this.total_jr_calc_km, this.total_jr_sales_km, 'k');

            }
            if (this.jr_fare == 0) {
                /* JR九州 幹線のみ、幹線＋地方交通線、地方交通線のみ の(l), (n)非適用 */
                // (h)<s><c>
                System.out.printf("fare(kyusyu)\n");			// System.out.printf("fare(m, h)[9]\n");
                this.jr_fare = Fare_kyusyu(this.total_jr_sales_km,
                        this.total_jr_calc_km);
            }

        } else if (0 < (this.shikoku_sales_km + this.shikoku_calc_km)) {
            /* JR四国のみ */
            RouteUtil.ASSERT (this.total_jr_sales_km == this.shikoku_sales_km);
            RouteUtil.ASSERT (this.total_jr_calc_km == this.shikoku_calc_km);
            RouteUtil.ASSERT (this.jr_fare == 0);

            if (this.local_only) {
                /* JR四国 地方交通線 */
                System.out.printf("fare(ls)'s'\n");
                /* (k) */
                this.jr_fare = Fare_table(this.total_jr_calc_km, this.total_jr_sales_km, 's');

            }
            if (this.jr_fare == 0) {
                /* JR四国 幹線のみ、幹線＋地方交通線、地方交通線のみ の(l), (n)非適用 */
                // (g)<s><c>
                System.out.printf("fare(shikoku)[4]\n");		// System.out.printf("fare(m, g)[4]\n");
                this.jr_fare = Fare_shikoku(this.total_jr_sales_km,
                        this.total_jr_calc_km);
            }

        } else {
            /* 会社線のみ */
            //RouteUtil.ASSERT (false);   青森始発からIGRで盛岡方面へ
            return false;
        }
        return true;
    }

    //	1経路の営業キロ、計算キロを集計
    //	calc_fare() =>
    //
    //	@retval 0 < Success(特別加算区間割増運賃額.通常は0)
    //	@retval -1 Fatal error
    //
    private int aggregate_fare_info(final LastFlag last_flag, final List<RouteItem> routeList_raw, final List<RouteItem> routeList_cooked) {
        short station_id1;		/* station_id1, (station_id2=ite.stationId) */
        short station_id_0;		/* last station_id1(for Company line) */
        LastFlag osakakan_aggregate;	// 大阪環状線通過フラグ bit0: 通過フラグ
        //
        int fare_add = 0;						// 運賃特別区間加算値
        final List<RouteItem> routeList = (routeList_cooked == null) ? routeList_raw : routeList_cooked;

        this.local_only = true;
        this.local_only_as_hokkaido = true;
        osakakan_aggregate = last_flag.clone();
        osakakan_aggregate.setOsakaKanPass(false);

        station_id1 = 0;

        /* 近郊区間ではない条件となる新幹線乗車があるか */
        for (RouteItem ri : routeList_raw) {
            if (station_id1 != 0) {
                if (IsBulletInUrban(ri.lineId, station_id1, ri.stationId)) {
                    this.flag |= (1 << RouteUtil.BCBULURB); // ONの場合大都市近郊区間特例無効(新幹線乗車している)
                    break;
                }
            }
            station_id1 = ri.stationId;
        }

        station_id_0 = station_id1 = 0;

        for (Iterator ite = routeList.iterator(); ite.hasNext(); ) {
            RouteItem ri = (RouteItem)ite.next();
            //RouteUtil.ASSERT ((ite.flag) == 0);
            if (station_id1 != 0) {
                                /* 会社別営業キロ、計算キロの取得 */

                if (ri.lineId == RouteUtil.ID_L_RULE70) {
                    int sales_km;
                    sales_km = Retrieve70Distance(station_id1, ri.stationId);
                    RouteUtil.ASSERT (0 < sales_km);
                    this.sales_km += sales_km;			// total 営業キロ(会社線含む、有効日数計算用)
                    this.base_sales_km	+= sales_km;
                    this.base_calc_km += sales_km;
                    this.local_only = false;		// 幹線

                } else {
                    int company_id1;
                    int company_id2;
                    int flag;
                    Integer[] dex;

                    if (ri.lineId != DbidOf.id().LineIdOf_OOSAKAKANJYOUSEN) {
                        dex = this.getDistanceEx(ri.lineId, station_id1, ri.stationId);
                    } else {
                        dex = GetDistanceEx(osakakan_aggregate, ri.lineId, station_id1, ri.stationId);
                        osakakan_aggregate.setOsakaKanPass(true);
                    }

                    if (9 != dex.length) {
                        RouteUtil.ASSERT (false);
                        return -1;	/* failure abort end. >>>>>>>>> */
                    }
                    company_id1 = dex[4];
                    company_id2 = dex[5];

                    System.out.printf("multicompany line none detect X: %d, %d, comp1,2=%d, %d, %s:%s-%s\n",
                            dex[2],
                            dex[3],
                            company_id1,
                            company_id2,
                            RouteUtil.LineName(ri.lineId),
                            RouteUtil.StationName(station_id1),
                            RouteUtil.StationName(ri.stationId));

                    this.companymask |= ((1 << (company_id1 - 1)) | ((1 << (company_id2 - 1))));

                    if ((company_id1 == RouteUtil.JR_CENTRAL) || (company_id1 == RouteUtil.JR_WEST)) {
                        company_id1 = RouteUtil.JR_EAST;	/* 本州三社としてJR東海、JR西は、JR東とする */
                    }
                    if ((company_id2 == RouteUtil.JR_CENTRAL) || (company_id2 == RouteUtil.JR_WEST)) {
                        company_id2 = RouteUtil.JR_EAST;	/* 本州三社としてJR東海、JR西は、JR東とする */
                    }

                    this.sales_km += dex[0];			// total 営業キロ(会社線含む、有効日数計算用)
                    if (dex[8] != 0) {	/* 会社線 */

                        CompanyFare comfare = null;

                        if (0 < station_id_0) {
                            /* 2回以上連続で会社線の場合(タンゴ鉄道とか) */
                            /* a+++b+++c : */
                            comfare = Fare_company(station_id_0, ri.stationId);
                            if (comfare == null) {
                                /* 乗継割引なし */
                                comfare = Fare_company(station_id1, ri.stationId);
                                if (comfare == null) {
                                    RouteUtil.ASSERT (false);
                                }
                                station_id_0 = station_id1;
                            } else {
                                CompanyFare comfare_1 = null;	// a+++b

    							// if ex. 氷見-金沢 併算割引非適用
    							if (comfare.passflg && (!last_flag.compnbegin || !last_flag.compnend)) {
    								/* 乗継割引なし */
    								comfare = Fare_company(station_id1, ri.stationId);
                                    if (comfare == null) {
    									RouteUtil.ASSERT(false);
    								}
    							} else {
    								/* normal or 併算割引適用 */
    								comfare_1 = Fare_company(station_id_0, station_id1);
                                    if (comfare_1 == null) {
    									RouteUtil.ASSERT(false);
    								} else {
        								if (0 < comfare_1.fareAcademic) {
        									/* 学割ありの場合は割引額を戻す */
        									RouteUtil.ASSERT(comfare_1.fareAcademic < comfare_1.fare);
        									this.company_fare_ac_discount -= (comfare_1.fare - comfare_1.fareAcademic);
        								}
        								this.company_fare -= comfare_1.fare;	// 戻す(直前加算分を取消)
        								this.company_fare_child -= comfare_1.fareChild;
                                    }
                                }
                            }
                            if (0 < comfare.fareAcademic) {
                                /* 学割ありの場合は割引額を加算 */
                                RouteUtil.ASSERT (comfare.fareAcademic < comfare.fare);
                                this.company_fare_ac_discount += (comfare.fare - comfare.fareAcademic);
                            }
                            this.company_fare += comfare.fare;
                            this.company_fare_child += comfare.fareChild;

                        } else {
                            /* 1回目の会社線 */
                            if (this.sales_km == dex[0]) {
                                result_flag = RouteUtil.BIT_ON(result_flag, BRF_COMAPANY_FIRST);
                            } else if ((0 < this.company_fare) &&
                                    !RouteUtil.BIT_CHK(result_flag, BRF_COMAPANY_END)) {
                                /* 会社線 2回以上通過 */
                                result_flag = RouteUtil.BIT_ON(result_flag, BRF_COMPANY_INCORRECT);
                                RouteUtil.ASSERT(false);
                            }
                            comfare = Fare_company(station_id1, ri.stationId);
                            if (comfare == null) {
                                RouteUtil.ASSERT (false);
                            } else {
                                if (0 < comfare.fareAcademic) {
                                    /* 学割ありの場合は割引額を加算 */
                                    RouteUtil.ASSERT (comfare.fareAcademic < comfare.fare);
                                    this.company_fare_ac_discount += (comfare.fare - comfare.fareAcademic);
                                }
                                this.company_fare += comfare.fare;
                                this.company_fare_child += comfare.fareChild;
                            }
                            station_id_0 = station_id1;

                            result_flag = RouteUtil.BIT_ON(result_flag, BRF_COMAPANY_END);
                        }
                    } else {
                        /* JR線 */
                        station_id_0 = 0;

                        result_flag = RouteUtil.BIT_OFF(result_flag, BRF_COMAPANY_END);

                        /* JR Group */
                        if (RouteUtil.IS_SHINKANSEN_LINE(ri.lineId)) {
                            /* 山陽新幹線 広島-徳山間を通る場合の1経路では、fare(l, i) とされてしまうけんのぉ
                             */
                            this.local_only = false;		// 幹線
                        }

                        if (this.aggregate_fare_jr(company_id1, company_id2, dex) < 0) {
                            return -1;	// error >>>>>>>>>>>>>>>>>>>>>>>>>>>
                        }
                    }
                    if ((this.flag & RouteUtil.FLAG_FARECALC_INITIAL) == 0) { // b15が0の場合最初なので駅1のフラグも反映
                        // ビット13のみ保持(既にflagのビット13のみにはcalc_fare()の最初でセットしているから)
                        this.flag &= (1<<RouteUtil.BCBULURB);
                        // 次回以降から駅1不要、駅1 sflgの下12ビット,
                        // bit12以上はGetDistanceEx()のクエリでOffしているので不要
                        this.flag |= (RouteUtil.FLAG_FARECALC_INITIAL | (/*~(1<<RouteUtil.BCBULURB) & */dex[6]));
                    }
                    flag = (RouteUtil.FLAG_FARECALC_INITIAL | (1<<RouteUtil.BCBULURB) | dex[7]);
                    if ((flag & RouteUtil.MASK_URBAN) != (this.flag & RouteUtil.MASK_URBAN)) {/* 近郊区間(b7-9) の比較 */
                        flag &= ~RouteUtil.MASK_URBAN;				/* 近郊区間 OFF */
                    }
                    this.flag &= flag;	/* b11,10,5(大阪/東京電車特定区間, 山手線／大阪環状線内) */
                                        /* ~(反転）不要 */
                    /* flag(sflg)は、b11,10,5, 7-9 のみ使用で他はFARE_INFOでは使用しない */

                    // 特別加算区間
                    fare_add += CheckSpecficFarePass(ri.lineId, station_id1, ri.stationId);
                }
            }
            station_id1 = ri.stationId;
        }
        return fare_add;
    }

    //
    //	calc_fare() => aggregate_fare_info() -> *
    //	距離値積上集計(JRグループ)
    //
    //	@param [in] company_id1    会社1
    //	@param [in] company_id2    会社2
    //	@param [in] distance       GetDistanceEx()の戻り
    //
    //	@retval 0 success
    //	@retval -1 failure
    //
    private int aggregate_fare_jr(int company_id1, int company_id2, final Integer[] distance) {
        if (company_id1 == company_id2) {		// 同一 1社
                /* 単一 JR Group */
            switch (company_id1) {
                case RouteUtil.JR_HOKKAIDO:
                    this.hokkaido_sales_km 	+= distance[0];
                    if (0 == distance[1]) { 		/* 幹線? */
                        this.hokkaido_calc_km 	+= distance[0];
                        this.local_only_as_hokkaido = false;
                        this.local_only = false;	// 幹線
                    } else {
                        this.hokkaido_calc_km 	+= distance[1];
                    }
                    break;
                case RouteUtil.JR_SHIKOKU:
                    this.shikoku_sales_km 		+= distance[0];
                    if (0 == distance[1]) { 		/* 幹線? */
                        this.shikoku_calc_km 	+= distance[0];
                        this.local_only = false;	// 幹線
                    } else {
                        this.shikoku_calc_km 	+= distance[1];
                    }
                    break;
                case RouteUtil.JR_KYUSYU:
                    this.kyusyu_sales_km 		+= distance[0];
                    if (0 == distance[1]) { 		/* 幹線? */
                        this.kyusyu_calc_km 	+= distance[0];
                        this.local_only = false;	// 幹線
                    } else {
                        this.kyusyu_calc_km 	+= distance[1];
                    }
                    break;
                case RouteUtil.JR_EAST:
                case RouteUtil.JR_CENTRAL:
                case RouteUtil.JR_WEST:
                    this.base_sales_km 		+= distance[0];
                    if (0 == distance[1]) { 		/* 幹線? */
                        this.base_calc_km 		+= distance[0];
                        this.local_only = false;	// 幹線
                    } else {
                        this.base_calc_km 		+= distance[1];
                    }
                    break;
                default:
                    RouteUtil.ASSERT (false);	/* あり得ない */
                    break;
            }
        } else {								// 会社跨り?
            switch (company_id1) {
                case RouteUtil.JR_HOKKAIDO:					// JR北海道->本州3社(JR東)
                    this.hokkaido_sales_km 	+= distance[2];
                    this.base_sales_km 		+= (distance[0] - distance[2]);
                    if (0 == distance[1]) { /* 幹線? */
                        this.hokkaido_calc_km  += distance[2];
                        this.base_calc_km  	+= (distance[0] - distance[2]);
                        this.local_only_as_hokkaido = false;
                    } else {
                        this.hokkaido_calc_km  += distance[3];
                        this.base_calc_km  	+= (distance[1] - distance[3]);
                    }
                    break;
                case RouteUtil.JR_SHIKOKU:					// JR四国->本州3社(JR西)
                    this.shikoku_sales_km 		+= distance[2];
                    this.base_sales_km 		+= (distance[0] - distance[2]);
                    if (0 == distance[1]) { 			/* 幹線? */
                        this.shikoku_calc_km 	+= distance[2];
                        this.base_calc_km  	+= (distance[0] - distance[2]);
                        this.local_only = false;	// 幹線
                    } else {
                        this.shikoku_calc_km 	+= distance[3];
                        this.base_calc_km  	+= (distance[1] - distance[3]);
                    }
                    break;
                case RouteUtil.JR_KYUSYU:					// JR九州->本州3社(JR西)
                    this.kyusyu_sales_km 		+= distance[2];
                    this.base_sales_km 		+= (distance[0] - distance[2]);
                    if (0 == distance[1]) { /* 幹線? */
                        this.kyusyu_calc_km 	+= distance[2];
                        this.base_calc_km 		+= (distance[0] - distance[2]);
                        this.local_only = false;	// 幹線
                    } else {
                        this.kyusyu_calc_km 	+= distance[3];
                        this.base_calc_km 		+= (distance[1] - distance[3]);
                    }
                    break;
                default:
                    this.base_sales_km 		+= distance[2];
                    if (0 == distance[1]) { /* 幹線? */
                        this.base_calc_km 		+= distance[2];
                        this.local_only = false;	// 幹線
                    } else {
                        this.base_calc_km 		+= distance[3];
                    }
                    switch (company_id2) {
                        case RouteUtil.JR_HOKKAIDO:			// 本州3社(JR東)->JR北海道
                            this.hokkaido_sales_km		+= (distance[0] - distance[2]);
                            if (0 == distance[1]) { /* 幹線? */
                                this.hokkaido_calc_km	+= (distance[0] - distance[2]);
                                this.local_only_as_hokkaido = false;
                            } else {
                                this.hokkaido_calc_km	+= (distance[1] - distance[3]);
                            }
                            break;
                        case RouteUtil.JR_SHIKOKU:			// 本州3社(JR西)->JR四国
                            this.shikoku_sales_km		+= (distance[0] - distance[2]);
                            if (0 == distance[1]) { /* 幹線? */
                                this.shikoku_calc_km	+= (distance[0] - distance[2]);
                            } else {
                                this.shikoku_calc_km	+= (distance[1] - distance[3]);
                            }
                            break;
                        case RouteUtil.JR_KYUSYU:				// 本州3社(JR西)->JR九州
                            this.kyusyu_sales_km		+= (distance[0] - distance[2]);
                            if (0 == distance[1]) { /* 幹線? */
                                this.kyusyu_calc_km	+= (distance[0] - distance[2]);
                            } else {
                                this.kyusyu_calc_km	+= (distance[1] - distance[3]);
                            }
                            break;
                        default:
                            RouteUtil.ASSERT (false);
                            return -1;	/* failure abort end. >>>>>>>>> */
                        //break;	/* あり得ない */
                    }
                    break;
            } /* JR 2company */
        } /* JR Group */
        return 0;
    }

    void setTerminal(int begin_station_id, int end_station_id) {
        beginTerminalId = begin_station_id;
        endTerminalId = end_station_id;
    }

    // Private:
    //	showFare() =>
    //	CheckOfRule114j() =>
    //
    //	運賃計算
    //
    //	@param [in] routeList    経路
    //	@param [in] applied_rule ルール適用(デフォルトTrue)
    //	@return 異常の時はfalse(会社線のみでJRなし)
    //
    boolean calc_fare(LastFlag last_flag, final List<RouteItem> routeList_raw, final List<RouteItem> routeList_cooked) {
        int fare_add;		/* 特別加算区間 */
        int adjust_km;
        List<RouteItem> routeList = (routeList_cooked == null) ? routeList_raw : routeList_cooked;

        reset();

        if ((fare_add = aggregate_fare_info(last_flag, routeList_raw, routeList_cooked)) < 0) {
            RouteUtil.ASSERT (false);
            reset();
            result_flag = RouteUtil.BIT_ON(result_flag, BRF_FATAL_ERROR);
            return false;
            //goto err;		/* >>>>>>>>>>>>>>>>>>> */
        }
        //System.out.padb kill-serverrintf("@@@:isNotCityterminalWoTokai()=%d, isCityterminalWoTokai()=%d, %d\n", RouteUtil.isNotCityterminalWoTokai(), RouteUtil.isCityterminalWoTokai(), last_flag.isTerCity());
    	System.out.printf("@@@ en=%s, aply=%s rule=%s norule=%s\n", last_flag.jrtokaistock_enable,
    		last_flag.jrtokaistock_applied,
    		last_flag.no_rule, last_flag.rule_en);

    	bEnableTokaiStockSelect = false;
    	last_flag.jrtokaistock_enable = false;
    	if (CheckIsJRTokai(routeList_raw)) {
    		// JR東海のみ

    		// [名]以外の都区市内・山手線が発着のいずれかにあるか

    		if (isCityterminalWoTokai()) {
    			bEnableTokaiStockSelect = true;	// JR東海株主優待券使用可
    			last_flag.jrtokaistock_enable = true;
    		}
    		else {
    			this.companymask = (1 << (RouteUtil.JR_CENTRAL - 1));
    			if (last_flag.jrtokaistock_applied) {
    				last_flag.jrtokaistock_enable = true;
    			}
    		}
    	}

        /* 旅客営業取扱基準規定43条の2（小倉、西小倉廻り） */
        if (routeList_cooked != null) {
            adjust_km = CheckAndApplyRule43_2j(routeList_cooked.toArray(new RouteItem[0]));
            this.sales_km			-= adjust_km * 2;
            this.base_sales_km		-= adjust_km;
            this.base_calc_km		-= adjust_km;
            this.kyusyu_sales_km	-= adjust_km;
            this.kyusyu_calc_km	-= adjust_km;
        }
        /* 乗車券の有効日数 */
        this.avail_days = days_ticket(this.sales_km);

        /* 旅客営業規則89条適用 */
        if (routeList_cooked != null) {
            this.base_calc_km += CheckOfRule89j(routeList_cooked);
        }
        /* 幹線のみ ? */
        this.major_only = ((this.base_sales_km == this.base_calc_km) &&
                (this.kyusyu_sales_km == this.kyusyu_calc_km) &&
                (this.hokkaido_sales_km == this.hokkaido_calc_km) &&
                (this.shikoku_sales_km == this.shikoku_calc_km));

        /* 運賃計算 */
    	if (retr_fare()) {
    		int special_fare;

    		if (last_flag.compncheck && (this.sales_km < 1500)) {
    										/* 大聖寺-和倉温泉は106kmある */
    										/* 大聖寺-米原-岐阜-富山-津端-和倉温泉 を弾く為 */
    	        	System.out.printf("specific fare section replace for IR-ishikawa change continue discount\n");
    				special_fare = SpecificFareLine(routeList.get(0).stationId, routeList.get(routeList.size() - 1).stationId, 2);
    				if (0 < special_fare) {
    		            this.jr_fare = special_fare - this.company_fare;	/* IRいしかわ 乗継割引 */
    				}
    		} else if (((RouteUtil.MASK_URBAN & this.flag) != 0) || (this.sales_km < 500)) {
    			special_fare = SpecificFareLine(routeList.get(0).stationId, routeList.get(routeList.size() - 1).stationId, 1);
    			if (0 < special_fare) {
    	        	System.out.printf("specific fare section replace for Metro or Shikoku-Big-bridge\n");

    				// 品川-青森-横浜 なども適用されてはいけないので,近郊区間内なら適用するように。
    				// 品川-横浜などの特別区間は近郊区間内の場合遠回り指定でも特別運賃を表示
    				// 名古屋は近郊区間でないので距離(尾頭橋-岡崎 37.7km 名古屋-岡崎 40.1km)50km以下として条件に含める
    				// またIRいしかわの乗継割引区間も同様50km以下が条件

    		        if (RouteUtil.URB_TOKYO == RouteUtil.URBAN_ID(this.flag)) {      /* 東京、新潟、仙台 近郊区間(最短距離で算出可能) */
    		                                                      /* 新幹線乗車も特別運賃適用 */

    		            if ((companymask & (1 << (RouteUtil.JR_CENTRAL - 1))) != 0) {
    		                /* 新幹線乗車 はIC運賃適用しないが大都市特例運賃は適用 */
    		                RouteUtil.ASSERT(0 == (companymask & ~((1 << (RouteUtil.JR_EAST - 1)) | (1 << (RouteUtil.JR_CENTRAL - 1)))));
    		            } else {
    		                RouteUtil.ASSERT(companymask == (1 << (RouteUtil.JR_EAST - 1)));  /* JR East only  */
    		                this.fare_ic = special_fare;
    		            }
    		            this.jr_fare = RouteUtil.round_up(special_fare);	/* 大都市特定区間運賃(東京)(\10単位切り上げ) */
    		        } else {
    		            this.jr_fare = special_fare;	/* 大都市特定区間運賃(大阪、名古屋) */
    		        }
    	        }
    			RouteUtil.ASSERT(this.company_fare == 0);	// 会社線は通っていない
    		}

            // 特別加算区間分
            this.jr_fare += fare_add;

            if (isUrbanArea()) {
                this.avail_days = 1;	/* 当日限り */
            }

            if (6000 < total_jr_sales_km) {	/* 往復割引 */
                this.roundTripDiscount = true;
            } else {
                this.roundTripDiscount = false;
            }
            return true;
        } else {
            /* 会社線のみ */
            //this.reset();
            this.avail_days = 1;
            this.roundTripDiscount = false;
            this.companymask &= ~RouteUtil.JR_GROUP_MASK;
            return true;
            //return false;
        }
    }

    //	経路設定
    //	calcFare() =>
    //
    //	@param [in]	begin_station_id  開始駅
    //	@param [in] end_station_id    終了駅
    //	@param [in] routeList         経路（規則適用時は変換後、規則非適用なら変換前）
    //	@param [in] last_flag         route flag(LF_OSAKAKAN_MASK:大阪環状線関連フラグのみ).
    //
    void setRoute(final List<RouteItem> routeList, final LastFlag last_flag) {
        route_for_disp = RouteUtil.Show_route(routeList.toArray(new RouteItem [0]), last_flag);
    }


    //public:
    public  void reset() {				//***
        companymask = 0;
        sales_km = 0;

        base_sales_km = 0;
        base_calc_km = 0;

        kyusyu_sales_km = 0;
        kyusyu_calc_km = 0;

        hokkaido_sales_km = 0;
        hokkaido_calc_km = 0;

        shikoku_sales_km = 0;
        shikoku_calc_km = 0;

        local_only = false;
        local_only_as_hokkaido = false;

        major_only = false;
        total_jr_sales_km = 0;
        total_jr_calc_km = 0;

        company_fare = 0;
        company_fare_ac_discount = 0;
        company_fare_child = 0;
        flag = 0;
        jr_fare = 0;
        fare_ic = 0;
        avail_days = 0;

        rule114_fare = 0;
        rule114_sales_km = 0;
        rule114_calc_km = 0;

        roundTripDiscount = false;

        //beginTerminalId = 0;
        //endTerminalId = 0;

        result_flag = 0;

        bEnableTokaiStockSelect = false;

        route_for_disp = "";
    }

    class FareResult {
        int fare;
        boolean isDiscount;
        FareResult() { fare = 0; isDiscount = false; }
        FareResult(int fare_, boolean isDiscount_) {
            fare = fare_;
            isDiscount = isDiscount_;
        }
    }
    void setEmpty() {
        result_flag = RouteUtil.BIT_ON(result_flag, BRF_ROUTE_EMPTY);
    }
    void setInComplete() {
        result_flag = RouteUtil.BIT_ON(result_flag, BRF_ROUTE_INCOMPLETE);
    }
    boolean isMultiCompanyLine() {
         return RouteUtil.BIT_CHK(result_flag, BRF_COMPANY_INCORRECT);
    }
    boolean isBeginEndCompanyLine() {
     return (result_flag & ((1 << BRF_COMAPANY_FIRST) | (1 << BRF_COMAPANY_END))) != 0;
    }
    int     resultCode() {
         if (RouteUtil.BIT_CHK(result_flag, BRF_ROUTE_INCOMPLETE)) {
            return -1;
        } else if (RouteUtil.BIT_CHK(result_flag, BRF_ROUTE_EMPTY)) {
             return -2;
         } else if (RouteUtil.BIT_CHK(result_flag, BRF_FATAL_ERROR)) {
             return -3;
         } else {
             return 0;
         }
     }


 	// [名]以外の都区市内・山手線が発着のいずれかにあり
 	boolean isCityterminalWoTokai() {
 		return ((RouteUtil.STATION_ID_AS_CITYNO < beginTerminalId) &&
              (RouteUtil.CITYNO_NAGOYA != (beginTerminalId - RouteUtil.STATION_ID_AS_CITYNO))) ||
             ((RouteUtil.STATION_ID_AS_CITYNO < endTerminalId) &&
              (RouteUtil.CITYNO_NAGOYA != (endTerminalId - RouteUtil.STATION_ID_AS_CITYNO)));
 	}

 	// [名]か単駅のみ発着の場合
     boolean isNotCityterminalWoTokai() {
 		return ((beginTerminalId < RouteUtil.STATION_ID_AS_CITYNO) ||
              (RouteUtil.CITYNO_NAGOYA == (beginTerminalId - RouteUtil.STATION_ID_AS_CITYNO))) &&
             ((endTerminalId < RouteUtil.STATION_ID_AS_CITYNO) ||
              (RouteUtil.CITYNO_NAGOYA == (endTerminalId - RouteUtil.STATION_ID_AS_CITYNO)));
 	}

    boolean isEnableTokaiStockSelect() {
        return bEnableTokaiStockSelect;
    }

    /**	往復運賃を返す(会社線含む総額)(JR分は601km以上で1割引)
     *
     *	@param discount [out]  true=割引あり
     *	@retval [円]
     */
    FareResult 	roundTripFareWithCompanyLine() {
        FareResult fareW = new FareResult();

        if (6000 < total_jr_sales_km) {	/* 往復割引 */
            fareW.fare = fare_discount(jr_fare, 1) * 2 + company_fare * 2;
            fareW.isDiscount = true;
            RouteUtil.ASSERT (this.roundTripDiscount == true);
        } else {
            fareW.isDiscount = false;
            fareW.fare = jrFare() * 2 + company_fare * 2;
            RouteUtil.ASSERT (this.roundTripDiscount == false);
        }
        return fareW;
    }

    /**	114条適用前の往復運賃を返す(会社線含む総額)(JR分は601km以上で1割引)
     *
     *	@retval [円]
     */
    int 	roundTripFareWithCompanyLinePriorRule114() {
        int fareW;

        if (6000 < total_jr_sales_km) {	/* 往復割引 */
            RouteUtil.ASSERT (false);
        }
        if (!isRule114()) {
            RouteUtil.ASSERT (false);
        }
        fareW = getFareForJR() * 2 + company_fare * 2;
        return fareW;
    }

    /**	小児往復運賃を返す(会社線含む総額)(JR分は601km以上で1割引)
     *
     *	@paramm discount [out]  true=割引あり
     *	@retval [円]
     */
    int 	roundTripChildFareWithCompanyLine()	{
        int fareW;

        if (6000 < total_jr_sales_km) {	/* 往復割引 */
            fareW = fare_discount(fare_discount(jr_fare, 5), 1) * 2 + company_fare_child * 2;
    	} else {
    		fareW = fare_discount(jrFare(), 5) * 2 + company_fare_child * 2;
        }
        return fareW;
    }

    /**	近郊区間内かを返す(有効日数を1にする)
     *
     *	@retval	true 近郊区間
     */
    boolean 	isUrbanArea() {
        if (((RouteUtil.MASK_URBAN & flag) != 0) &&
                (((1 << RouteUtil.BCBULURB) & flag) == 0)) {

            RouteUtil.ASSERT ((IsIC_area(RouteUtil.URBAN_ID(flag)) && (fare_ic != 0)) ||
                    (!IsIC_area(RouteUtil.URBAN_ID(flag)) && (fare_ic == 0)));

            return true;
        } else {
            return false;
        }
    }

    /**	総営業キロを返す
     *
     *	@retval 営業キロ
     */
    int 	getTotalSalesKm() {
        return sales_km;
    }

    int		getRule114SalesKm() { return rule114_sales_km; }
    int		getRule114CalcKm() { return rule114_calc_km;  }

    /**	JR線の営業キロを返す
     *
     *	@retval 計算キロ
     */
    int		getJRSalesKm() {
        return total_jr_sales_km;
    }

    /**	JR線の計算キロを返す
     *
     *	@retval 計算キロ
     */
    int		getJRCalcKm() {
        return total_jr_calc_km;
    }

    /**	会社線の営業キロ総数を返す
     *
     *	@retval 営業キロ
     */
    int		getCompanySalesKm() {
        return sales_km - total_jr_sales_km;
    }

    /**	JR北海道の営業キロを返す
     *
     *	@retval 営業キロ
     */
    int		getSalesKmForHokkaido() {
        return hokkaido_sales_km;
    }

    /**	JR四国の営業キロを返す
     *
     *	@retval	営業キロ
     */
    int		getSalesKmForShikoku() {
        return shikoku_sales_km;
    }

    /**	JR九州の営業キロを返す
     *
     *	@retval	営業キロ
     */
    int		getSalesKmForKyusyu() {
        return kyusyu_sales_km;
    }

    /**	JR北海道の計算キロを返す
     *
     *	@retval	計算キロ
     */
    int		getCalcKmForHokkaido() {
        return hokkaido_calc_km;
    }

    /**	JR四国の計算キロを返す
     *
     *	@retval	計算キロ
     */
    int		getCalcKmForShikoku() {
        return shikoku_calc_km;
    }

    /**	JR九州の計算キロを返す
     *
     *	@retval	計算キロ
     */
    int		getCalcKmForKyusyu() {
        return kyusyu_calc_km;
    }

    /**	乗車券の有効日数を返す
     *
     *	@retval	有効日数[日]
     */
    int		getTicketAvailDays() {
        return avail_days;
    }

    /**	会社線の運賃額を返す
     *
     *	@retval	[円]
     */
    int		getFareForCompanyline() {
        return company_fare;
    }

    /**	JR線＋会社線の小児運賃額を返す
     *
     *	@retval	[円]
     */
    int		getChildFareForDisplay() {
        return company_fare_child + fare_discount(jrFare(), 5);
    }

    /**	JR線の運賃額を返す(114条未適用計算値)
     *
     *	@retval	[円]
     */
    int		getFareForJR() {
        return jr_fare;
    }

    /**	株主優待割引有効数を返す
     *
     *	@param index      [in]  0から1 JR東日本のみ 0は2割引、1は4割引を返す
     *	@param idCompany [out]  0:JR東海1割/1:JR西日本5割/2:JR東日本2割/3:JR東日本4割
     *	@retval	[円](無割引、無効は0)
     */
    int countOfFareStockDiscount() {
        // 通過連絡運輸も株優は有効らしい

        switch (getStockDiscountCompany()) {
            case RouteUtil.JR_EAST:
            case RouteUtil.JR_CENTRAL:
                return 2;
            case RouteUtil.JR_WEST:
            case RouteUtil.JR_KYUSYU:
                return 1;
            default:
                break;
        }
        return 0;
    }


    /**	株主優待割引運賃を返す(会社線運賃は含まない)
     *
     *	@param index      [in]   0から1 JR東日本のみ 0は2割引、1は4割引を返す
     *	@param title      [out]  項目表示文字列
     *	@param applied_r114  [in]  true = 114条適用 / false = 114条適用前
     *	@retval	[円](無割引、無効は0)
     */
    int getFareStockDiscount(int index) {
        return getFareStockDiscount(index, false);
    }
    int getFareStockDiscount(int index, boolean applied_r114)	{
        int cfare;
        int result = 0;

        if (applied_r114) {
            if (isRule114()) {
                cfare = rule114_fare;
            } else {
                RouteUtil.ASSERT (false);
                return 0;		// >>>>>>>>>
            }
        } else {
            cfare = jr_fare;
        }

        int companyno = getStockDiscountCompany();
        switch (companyno) {
            case RouteUtil.JR_EAST:
                if (index == 0) {
                    result = fare_discount(cfare, 2);
                } else if (index == 1) {
                /* JR東4割(2枚使用) */
                    result = fare_discount(cfare, 4);
                } else {
                    result = 0;    // wrong index
                }
                break;
            case RouteUtil.JR_WEST:
            case RouteUtil.JR_KYUSYU:
                if (index == 0) {
                    result = fare_discount(cfare, 5);
                }
                break;
            case RouteUtil.JR_CENTRAL:
                if (index == 0) {
                    result = fare_discount(cfare, 1);
                } else if (index == 1) {
                    result = fare_discount(cfare, 2);
                } else {
                    result = 0;    // wrong index
                }
                break;
            default:
                break;
        }
        return result;

        // 通過連絡運輸も株優は有効らしい
    }


    String getTitleFareStockDiscount(int index)	{
        final String[] titles = {
            "JR東日本 株主優待2割", // 0
    		"JR東日本 株主優待4割", // 1
    		"JR西日本 株主優待5割", // 2
    		"JR東海   株主優待1割", // 3
    		"JR東海   株主優待2割", // 4
            "JR九州   株主優待5割", // 5
        };
        int sindex = -1;
        int stockno = getStockDiscountCompany();
        switch (stockno) {
            case RouteUtil.JR_EAST:
                if (index == 0) {
                    sindex = 0;
                } else if (index == 1) {
                /* JR東4割(2枚使用) */
                    sindex = 1;
                } else {
                    //return "";    // wrong index
                    RouteUtil.ASSERT(false);
                }
                break;
            case RouteUtil.JR_WEST:
                if (index == 0) {
                    sindex = 2;
                } else {
                    //return "";    // wrong index
                    RouteUtil.ASSERT(false);
                }
                break;
            case RouteUtil.JR_KYUSYU:
                if (index == 0) {
                    sindex = 5;
                } else {
                    //return "";    // wrong index
                    RouteUtil.ASSERT(false);
                }
                break;
            case RouteUtil.JR_CENTRAL:
                if (index == 0) {
                    sindex = 3;
                } else if (index == 1) {
                    sindex = 4;
                } else {
                    //return "";    // wrong index
                    RouteUtil.ASSERT(false);
                }
                break;
            default:
                break;
        }
        if (0 <= sindex) {
            return titles[sindex];
        } else {
            return "";
        }
        // 通過連絡運輸も株優は有効らしい
    }

    //  株主優待可否／種別を返す
    //  @retval RouteUtil.JR_EAST = JR東日本
    //  @retval RouteUtil.JR_WEST = JR西日本
    //  @retval RouteUtil.JR_CENTRAL = JR東海
    //  @retval RouteUtil.JR_KYUSYU = JR九州
    //  @retval 0 = 無効
    //	会社線も含んでも良い(計算時は除外されるため)
    //
    int getStockDiscountCompany() {
        if ((RouteUtil.JR_GROUP_MASK & companymask) == (1 << (RouteUtil.JR_EAST - 1))) {
            return RouteUtil.JR_EAST;
        }
        if ((RouteUtil.JR_GROUP_MASK & companymask) == (1 << (RouteUtil.JR_WEST - 1))) {
            return RouteUtil.JR_WEST;
        }
        if ((RouteUtil.JR_GROUP_MASK & companymask) == (1 << (RouteUtil.JR_KYUSYU - 1))) {
            return RouteUtil.JR_KYUSYU;
        }
        if (((RouteUtil.JR_GROUP_MASK & companymask) == (1 << (RouteUtil.JR_CENTRAL - 1)))) {
            return RouteUtil.JR_CENTRAL;
        }
        return 0;
    }


    /**	学割運賃を返す(会社線+JR線=全線)
     *
     *	@retval	学割運賃[円]
     *	@retval 0 非適用
     */
    int		getAcademicDiscountFare() {
        int result_fare;

        if ((1000 < total_jr_sales_km) || (0 < company_fare_ac_discount)) {
            if (1000 < total_jr_sales_km) {
                result_fare = fare_discount(jrFare(), 2);
            } else {
                result_fare = jrFare();
            }
            return result_fare + (company_fare - company_fare_ac_discount);
        } else {
            /* 学割非適用 */
            return 0;
        }
    }

    /**	学割往復運賃を返す(会社線含む総額)(JR分は601km以上で1割引)
     *
     *	@retval [円]
     */
    int 	roundTripAcademicFareWithCompanyLine() {
        int fareW;

        // JR

        if (6000 < total_jr_sales_km) {	/* 往復割引かつ学割 */
            fareW = fare_discount(fare_discount(jr_fare, 1), 2);
            RouteUtil.ASSERT (this.roundTripDiscount == true);
        } else {
            fareW = jrFare();
            if (1000 < total_jr_sales_km) {
                // Academic discount
                RouteUtil.ASSERT (this.roundTripDiscount == false);
                fareW = fare_discount(fareW, 2);
            }
        }

        // company

        fareW += (company_fare - company_fare_ac_discount);
        return fareW * 2;
    }

    /**	JR線＋会社線の運賃額を返す
     *
     *	@retval	[円]
     */
    int		getFareForDisplay() {
        return getFareForCompanyline() + jrFare();
    }

    /**	JR線＋会社線の運賃額を返す(114条非適用運賃)
     *
     *	@retval	[円]
     */
    int		getFareForDisplayPriorRule114() {
        if (isRule114()) {
            return getFareForCompanyline() + jr_fare;
        } else {
            //      RouteUtil.ASSERT (false);
            return 0;
        }
    }

    /**	IC運賃を返す
     *
     *	@retval IC運賃(x10[円])
     */
    int		getFareForIC()
    {
        RouteUtil.ASSERT (((fare_ic != 0) && (companymask == (1 << (RouteUtil.JR_EAST - 1)))) || (fare_ic == 0));
        return fare_ic;
    }

    static class Fare {
        int fare;
        int sales_km;
        int calc_km;
        Fare() { set(0, 0,0); }
        Fare(int f, int sk, int ck) {
            set(f, sk, ck);
        }
        void set(int f, int sk, int ck) {
            fare = f;
            sales_km = sk;
            calc_km = ck;
        }
    }

    boolean     getRule114(Fare fare) {
        fare.fare = rule114_fare;
        fare.sales_km = rule114_sales_km;
        fare.calc_km = rule114_calc_km;
        return rule114_fare != 0;
    }

    void     setRule114(final Fare fare) {
        rule114_fare = fare.fare;
        rule114_sales_km = fare.sales_km;
        rule114_calc_km = fare.calc_km;
    }
    void     clrRule114() {
        rule114_fare = 0;
        rule114_sales_km = 0;
        rule114_calc_km = 0;
    }

    boolean	isRule114() { return 0 != rule114_fare; }
    boolean	isRoundTripDiscount() { /* roundTripFareWithCompanyLine() を前に呼んでいること */ return roundTripDiscount; }
    int getBeginTerminalId() { return beginTerminalId;}
    int getEndTerminalId() { return endTerminalId; }
    String getRoute_string() { return route_for_disp; }
    static boolean IsCityId(int id) { return RouteUtil.STATION_ID_AS_CITYNO <= id; }

    // static
    //		@brief 70条通過の営業キロを得る
    //
    //	@param [in] station_id1		駅1
    //	@param [in] station_id2		駅2
    //
    //	@return station_id1, station_id2間の営業キロ
    //
    static int Retrieve70Distance(int station_id1, int station_id2)	{
        final String tsql =
                "select sales_km from t_rule70" +
                        " where " +
                        " ((station_id1=?1 and station_id2=?2) or" +
                        "  (station_id1=?2 and station_id2=?1))";
        int rc = 0;
        Cursor dbo = RouteDB.db().rawQuery(tsql, new String[] {String.valueOf(station_id1), String.valueOf(station_id2)});
        try {
            if (dbo.moveToNext()) {
                rc = dbo.getInt(0);
            }
        } finally {
            dbo.close();
        }
        return rc;
    }

    /**	JR線の運賃額を返す(含114条適用／非適用判定)
     *
     *	@retval	[円]
     */
    private int		jrFare() {
        if (isRule114()) {
            return rule114_fare;
        } else {
            return jr_fare;
        }
    }


    //	運賃計算(基本)
    //	calc_fare() => retr_fare() =>
    //
    //	@param [in] km    計算キロ
    //	@return 運賃額
    //
    private static int Fare_basic_f(int km) {
        int fare;
        int c_km;

        if (km < 31) {							// 1 to 3km
            if (RouteDB.getInstance().tax() == 5) {
                return 140;
            } else {
                return 144;
            }
        }
        if (km < 61) {							// 4 to 6km
            if (RouteDB.getInstance().tax() == 5) {
                return 180;
            } else {
                return 185;
            }
        }
        if (km < 101) {							// 7 to 10km
            if (RouteDB.getInstance().tax() == 5) {
                return 190;
            } else {
                return 195;
            }
        }
        if (6000 < km) {						// 600km越えは40キロ刻み
            c_km = (km - 1) / 400 * 400 + 200;
        } else if (1000 < km) {					// 100.1-600kmは20キロ刻み
            c_km = (km - 1) / 200 * 200 + 100;
        } else if (500 < km) {					// 50.1-100kmは10キロ刻み
            c_km = (km - 1) / 100 * 100 + 50;
        } else if (100 < km) {					// 10.1-50kmは5キロ刻み
            c_km = (km - 1) / 50 * 50 + 30;
        } else {
            RouteUtil.ASSERT (false);
            c_km = 0;
        }
        if (6000 <= c_km) {
            fare = 1620 * 3000 + 1285 * (6000 - 3000) + 705 * (c_km - 6000);
        } else if (3000 < c_km) {
            fare = 1620 * 3000 + 1285 * (c_km - 3000);
        } else {
            fare = 1620 * c_km;
        }
        if (c_km <= 1000) {						// 100km以下は切り上げ
            // 1の位を切り上げ
            fare = (fare + 9999) / 10000 * 10;
        } else {								// 100㎞越えは四捨五入
            fare = (fare + 50000) / 100000 * 100;
        }
        return RouteUtil.taxadd_ic(fare, RouteDB.getInstance().tax());	// tax = +5%, 四捨五入
    }

    //	sub: 地方交通線
    //	calc_fare() => retr_fare() =>
    //
    //	@param [in] km    営業キロ
    //	@return 運賃額
    //
    private static int Fare_sub_f(int km) {
        int fare;
        int c_km;

        if (km < 31) {							// 1 to 3km
            if (RouteDB.getInstance().tax() == 5) {
                return 140;
            } else {
                return 144;
            }
        }
        if (km < 61) {							// 4 to 6km
            if (RouteDB.getInstance().tax() == 5) {
                return 180;
            } else {
                return 185;
            }
        }
        if (km < 101) {							// 7 to 10km
            if (RouteDB.getInstance().tax() == 5) {
                return 200;
            } else {
                return 206;
            }
        }

        if (12000 < km) {		// 1200km越えは別表第2号イの4にない
            RouteUtil.ASSERT (false);
            return -1;
        }

        c_km = Fare_table("lspekm", 'e', km);
        if (c_km == 0) {
            RouteUtil.ASSERT (false);
            return -1;
        }
        if (c_km < 0) {
            return -c_km;		/* fare 第77条の5の3*/
        }

        /* c_km : 第77条の5の2 */
        c_km *= 10;

        /* 第77条の5 */
        if (5460 <= c_km) {
            fare = 1780 * 2730 + 1410 * (5460 - 2730) + 770 * (c_km - 5460);
        } else if (2730 < c_km) {
            fare = 1780 * 2730 + 1410 * (c_km - 2730);
        } else {
            fare = 1780 * c_km;
        }
        if (c_km <= 1000) {						// 100km以下は切り上げ
            // 1の位を切り上げ
            fare = (fare + 9999) / 10000 * 10;
        } else {				// 100㎞越えは四捨五入
            fare = (fare + 50000) / 100000 * 100;
        }
        return RouteUtil.taxadd_ic(fare, RouteDB.getInstance().tax());
    }

    //	b: 電車特定区間(東京)
    //	calc_fare() => retr_fare() =>
    //
    //	@param [in] km    営業キロ
    //	@return 運賃額
    //
    private static int Fare_tokyo_f(int km) {
        int fare;
        int c_km;

        if (km < 31) {							// 1 to 3km
            if (RouteDB.getInstance().tax() == 5) {
                return 130;
            } else {
                return 133;
            }
        }
        if (km < 61) {							// 4 to 6km
            if (RouteDB.getInstance().tax() == 5) {
                return 150;
            } else {
                return 154;
            }
        }
        if (km < 101) {							// 7 to 10km
            if (RouteDB.getInstance().tax() == 5) {
                return 160;
            } else {
                return 165;
            }
        }

        if (6000 < km) {						// 600km越えは40キロ刻み
            RouteUtil.ASSERT (false);
            return -1;
        } else if (1000 < km) {					// 100.1-600kmは20キロ刻み
            c_km = (km - 1) / 200 * 200 + 100;
        } else if (500 < km) {					// 50.1-100kmは10キロ刻み
            c_km = (km - 1) / 100 * 100 + 50;
        } else if (100 < km) {					// 10.1-50kmは5キロ刻み
            c_km = (km - 1) / 50 * 50 + 30;
        } else {
            RouteUtil.ASSERT (false);
            c_km = 0;
        }
        if (3000 < c_km) {
            fare = 1530 * 3000 + 1215 * (c_km - 3000);
        } else {
            fare = 1530 * c_km;
        }
        if (c_km <= 1000) {						// 100km以下は切り上げ
            // 1の位を切り上げ
            fare = (fare + 9999) / 10000 * 10;
        } else {				// 100㎞越えは四捨五入
            fare = (fare + 50000) / 100000 * 100;
        }
        return RouteUtil.taxadd_ic(fare, RouteDB.getInstance().tax());
    }

    //	C: 電車特定区間(大阪)
    //	calc_fare() => retr_fare() =>
    //
    //	@param [in] km    営業キロ
    //	@return 運賃額
    //
    private static int Fare_osaka(int km) {
        int fare;
        int c_km;

        if (km < 31) {							// 1 to 3km
            if (RouteDB.getInstance().tax() == 5) {
                return 120;
            } else {
                return 120;
            }
        }
        if (km < 61) {							// 4 to 6km
            if (RouteDB.getInstance().tax() == 5) {
                return 160;
            } else {
                return 160;
            }
        }
        if (km < 101) {							// 7 to 10km
            if (RouteDB.getInstance().tax() == 5) {
                return 170;
            } else {
                return 180;
            }
        }
        if (6000 < km) {						// 600km越えは40キロ刻み
            RouteUtil.ASSERT (false);
            return -1;
        } else if (1000 < km) {					// 100.1-600kmは20キロ刻み
            c_km = (km - 1) / 200 * 200 + 100;
        } else if (500 < km) {					// 50.1-100kmは10キロ刻み
            c_km = (km - 1) / 100 * 100 + 50;
        } else if (100 < km) {					// 10.1-50kmは5キロ刻み
            c_km = (km - 1) / 50 * 50 + 30;
        } else {
            RouteUtil.ASSERT (false);
            c_km = 0;
        }

        if (3000 < c_km) {
            fare = 1530 * 3000 + 1215 * (c_km - 3000);
        } else {
            fare = 1530 * c_km;
        }
        if (c_km <= 1000) {						// 100km以下は切り上げ
            // 1の位を切り上げ
            fare = (fare + 9999) / 10000 * 10;
        } else {								// 100㎞越えは四捨五入
            fare = (fare + 50000) / 100000 * 100;
        }
        return RouteUtil.taxadd(fare, RouteDB.getInstance().tax());	// tax = +5%, 四捨五入
    }

    //	d: 電車特定区間(山手線)
    //	calc_fare() => retr_fare() =>
    //
    //	@param [in] km    営業キロ
    //	@return 運賃額
    //
    private static int Fare_yamate_f(int km) {
        int fare;
        int c_km;

        if (km < 31) {							// 1 to 3km
            if (RouteDB.getInstance().tax() == 5) {
                return 130;
            } else {
                return 133;
            }
        }
        if (km < 61) {							// 4 to 6km
            if (RouteDB.getInstance().tax() == 5) {
                return 150;
            } else {
                return 154;
            }
        }
        if (km < 101) {							// 7 to 10km
            if (RouteDB.getInstance().tax() == 5) {
                return 160;
            } else {
                return 165;
            }
        }
        if (3000 < km) {						// 300km越えは未定義
            RouteUtil.ASSERT (false);
            return -1;
        } else if (1000 < km) {					// 100.1-300kmは20キロ刻み
            c_km = (km - 1) / 200 * 200 + 100;
        } else if (500 < km) {					// 50.1-100kmは10キロ刻み
            c_km = (km - 1) / 100 * 100 + 50;
        } else if (100 < km) {					// 10.1-50kmは5キロ刻み
            c_km = (km - 1) / 50 * 50 + 30;
        } else {
            RouteUtil.ASSERT (false);
            c_km = 0;
        }
        fare = 1325 * c_km;

        if (c_km <= 1000) {	// 100km以下は切り上げ
            // 1の位を切り上げ
            fare = (fare + 9999) / 10000 * 10;
        } else {								// 100㎞越えは四捨五入
            fare = (fare + 50000) / 100000 * 100;
        }
        return RouteUtil.taxadd_ic(fare, RouteDB.getInstance().tax());	// tax = +5%, 四捨五入
    }

    //	e: 電車特定区間(大阪環状線)
    //	calc_fare() => retr_fare() =>
    //
    //	@param [in] km    営業キロ
    //	@return 運賃額
    //
    private static int Fare_osakakan(int km) {
        int fare;
        int c_km;

        if (km < 31) {							// 1 to 3km
            if (RouteDB.getInstance().tax() == 5) {
                return 120;
            } else {
                return 120;
            }
        }
        if (km < 61) {							// 4 to 6km
            if (RouteDB.getInstance().tax() == 5) {
                return 160;
            } else {
                return 160;
            }
        }
        if (km < 101) {							// 7 to 10km
            if (RouteDB.getInstance().tax() == 5) {
                return 170;
            } else {
                return 180;
            }
        }
        if (3000 < km) {						// 300km越えは未定義
            RouteUtil.ASSERT (false);
            return -1;
        } else if (1000 < km) {					// 100.1-300kmは20キロ刻み
            c_km = (km - 1) / 200 * 200 + 100;
        } else if (500 < km) {					// 50.1-100kmは10キロ刻み
            c_km = (km - 1) / 100 * 100 + 50;
        } else if (100 < km) {					// 10.1-50kmは5キロ刻み
            c_km = (km - 1) / 50 * 50 + 30;
        } else {
            RouteUtil.ASSERT (false);
            c_km = 0;
        }
        fare = 1325 * c_km;

        if (c_km <= 1000) {						// 100km以下は切り上げ
            // 1の位を切り上げ
            fare = (fare + 9999) / 10000 * 10;
        } else {								// 100㎞越えは四捨五入
            fare = (fare + 50000) / 100000 * 100;
        }
        return RouteUtil.taxadd(fare, RouteDB.getInstance().tax());	// tax = +5%, 四捨五入
    }

    //	f: JR北海道幹線
    //	calc_fare() => retr_fare() =>
    //
    //	@param [in] km    計算キロ
    //	@return 運賃額
    //
    private static int Fare_hokkaido_basic(int km) {
        int fare;
        int c_km;
        String tbl;		// [16]

        if (km < 31) {							// 1 to 3km
            if (RouteDB.getInstance().tax() == 5) {
                return 160;
            } else {
                return 170;
            }
        }
        if (km < 61) {							// 4 to 6km
            if (RouteDB.getInstance().tax() == 5) {
                return 200;
            } else {
                return 210;
            }
        }
        if (km < 101) {							// 7 to 10km
            if (RouteDB.getInstance().tax() == 5) {
                return 210;
            } else {
                return 220;
            }
        }

        tbl = String.format("h%d", RouteDB.getInstance().tax());
        fare = Fare_table("bspekm", tbl, km);
        if (0 != fare) {
            return fare;
        }

        if (6000 < km) {						// 600km越えは40キロ刻み
            c_km = (km - 1) / 400 * 400 + 200;
        } else if (1000 < km) {					// 100.1-600kmは20キロ刻み
            c_km = (km - 1) / 200 * 200 + 100;
        } else if (500 < km) {					// 50.1-100kmは10キロ刻み
            c_km = (km - 1) / 100 * 100 + 50;
        } else if (100 < km) {					// 10.1-50kmは5キロ刻み
            c_km = (km - 1) / 50 * 50 + 30;
        } else {
            RouteUtil.ASSERT (false);
            c_km = 0;
        }

        if (6000 < c_km) {
            fare = 1785 * 2000 + 1620 * (3000 - 2000) + 1285 * (6000 - 3000) + 705 * (c_km - 6000);
        } else if (3000 < c_km) {
            fare = 1785 * 2000 + 1620 * (3000 - 2000) + 1285 * (c_km - 3000);
        } else if (2000 < c_km) {
            fare = 1785 * 2000 + 1620 * (c_km - 2000);
        } else {
            fare = 1785 * c_km;
        }
        if (c_km <= 1000) {						// 100km以下は切り上げ
            // 1の位を切り上げ
            fare = (fare + 9999) / 10000 * 10;
        } else {								// 100㎞越えは四捨五入
            fare = (fare + 50000) / 100000 * 100;
        }
        return RouteUtil.taxadd(fare, RouteDB.getInstance().tax());	// tax = +5%, 四捨五入
    }

    //	JR北海道地方交通線
    //	calc_fare() => retr_fare() =>
    //
    //	@param [in] km    営業キロ
    //	@return 運賃額
    //
    private static int Fare_hokkaido_sub(int km) {
        int fare;
        int c_km;

        if (km < 31) {							// 1 to 3km
            if (RouteDB.getInstance().tax() == 5) {
                return 160;
            } else {
                return 170;
            }
        }
        if (km < 61) {							// 4 to 6km
            if (RouteDB.getInstance().tax() == 5) {
                return 200;
            } else {
                return 210;
            }
        }
        if (km < 101) {							// 7 to 10km
            if (RouteDB.getInstance().tax() == 5) {
                return 220;
            } else {
                return 230;
            }
        }

        if (12000 < km) {		// 1200km越えは別表第2号イの4にない
            RouteUtil.ASSERT (false);
            return -1;
        }

        c_km = Fare_table("lspekm", 'h', km);
        if (c_km == 0) {
            RouteUtil.ASSERT (false);
            return -1;
        }
        if (c_km < 0) {
            return -c_km;		/* fare */
        }

        c_km *= 10;

        if (5460 <= c_km) {
            fare = 1960 * 1820 + 1780 * (2730 - 1820) + 1410 * (5460 - 2730) + 770 * (c_km - 5460);
        } else if (2730 < c_km) {
            fare = 1960 * 1820 + 1780 * (2730 - 1820) + 1410 * (c_km - 2730);
        } else if (1820 < c_km) {
            fare = 1960 * 1820 + 1780 * (c_km - 1820);
        } else { /* <= 182km */
            fare = 1960 * c_km;
        }
        if (c_km <= 1000) {						// 100km以下は切り上げ
            // 1の位を切り上げ
            fare = (fare + 9999) / 10000 * 10;
        } else {				// 100㎞越えは四捨五入
            fare = (fare + 50000) / 100000 * 100;
        }
        return RouteUtil.taxadd(fare, RouteDB.getInstance().tax());
    }

    //	g: JR四国 幹線
    //	calc_fare() => retr_fare() =>
    //
    //	@param [in] skm    営業キロ
    //	@param [in] ckm    計算キロ
    //	@return 運賃額
    //
    private static int Fare_shikoku(int skm, int ckm) {
        int fare;
        int c_km;

        /* JTB時刻表 C-3表 */
        if (ckm != skm) {
            if (RouteDB.getInstance().tax() == 5) {
                /* JR四国 幹線+地方交通線 */
                /* (m) */
                if ((KM.KM(ckm) == 4) && (KM.KM(skm) == 3)) {
                    return 160;	/* \ */
                } else if ((KM.KM(ckm) == 11) && (KM.KM(skm) == 10)) {
                    return 230;	/* \ */
                }
            } else {
                    /* JR四国 幹線+地方交通線 */
                    /* (m) */
                if ((KM.KM(ckm) == 4) && (KM.KM(skm) == 3)) {
                    return 160;	/* \ */
                } else if ((KM.KM(ckm) == 11) && (KM.KM(skm) == 10)) {
                    return 230;	/* \ */
                }
                /*** JR四国は消費税8%でもここは据え置きみたい ***/
            }
        }

        if (ckm < 31) {							// 1 to 3km
            if (RouteDB.getInstance().tax() == 5) {
                return 160;
            } else {
                return 160;
            }
        }
        if (ckm < 61) {							// 4 to 6km
            if (RouteDB.getInstance().tax() == 5) {
                return 200;
            } else {
                return 210;
            }
        }
        if (ckm < 101) {						// 7 to 10km
            if (RouteDB.getInstance().tax() == 5) {
                return 210;
            } else {
                return 220;
            }
        }

        fare = Fare_table("bspekm", (RouteDB.getInstance().tax() == 5) ? "s5" : "s8", ckm);
        if (0 != fare) {
            return fare;
        }

        if (6000 < ckm) {						// 600km越えは40キロ刻み
            c_km = (ckm - 1) / 400 * 400 + 200;
        } else if (1000 < ckm) {					// 100.1-600kmは20キロ刻み
            c_km = (ckm - 1) / 200 * 200 + 100;
        } else if (500 < ckm) {					// 50.1-100kmは10キロ刻み
            c_km = (ckm - 1) / 100 * 100 + 50;
        } else if (100 < ckm) {					// 10.1-50kmは5キロ刻み
            c_km = (ckm - 1) / 50 * 50 + 30;
        } else {
            /* 0-10kmは上で算出return済み */
            RouteUtil.ASSERT (false);
            c_km = 0;
        }

        if (6000 <= c_km) {
            fare = 1821 * 1000 + 1620 * (3000 - 1000) + 1285 * (6000 - 3000) + 705 * (c_km - 6000);
        } else if (3000 < c_km) {
            fare = 1821 * 1000 + 1620 * (3000 - 1000) + 1285 * (c_km - 3000);
        } else if (1000 < c_km) {
            fare = 1821 * 1000 + 1620 * (c_km - 1000);
        } else {
            /* 10.1km - 100.0 km */
            fare = 1821 * c_km;
        }

        if (c_km <= 1000) {						// 100km以下は切り上げ
            // 1の位を切り上げ
            fare = (fare + 9999) / 10000 * 10;
        } else {								// 100㎞越えは四捨五入
            fare = (fare + 50000) / 100000 * 100;
        }
        return RouteUtil.taxadd(fare, RouteDB.getInstance().tax());	// tax = +5%, 四捨五入
    }

    //	h: JR九州 幹線
    //	calc_fare() => retr_fare() =>
    //
    //	@param [in] skm    営業キロ
    //	@param [in] ckm    計算キロ
    //	@return 運賃額
    //
    private static int Fare_kyusyu(int skm, int ckm) {
        int fare;
        int c_km;

        /* JTB時刻表 C-3表 */
        if (ckm != skm) {
            if (RouteDB.getInstance().tax() == 5) {
                /* JR九州 幹線+地方交通線 */
                /* (n) */
                if ((KM.KM(ckm) == 4) && (KM.KM(skm) == 3)) {
                    return 170;	/* \ */
                } else if ((KM.KM(ckm) == 11) && (KM.KM(skm) == 10)) {
                    return 240;	/* \ */
                }
            } else {
                    /* JR九州 幹線+地方交通線 */
                    /* (n) */
                if ((KM.KM(ckm) == 4) && (KM.KM(skm) == 3)) {
                    return 180;	/* \ */
                } else if ((KM.KM(ckm) == 11) && (KM.KM(skm) == 10)) {
                    return 250;	/* \ */
                }
            }
        }

        if (ckm < 31) {							// 1 to 3km
            if (RouteDB.getInstance().tax() == 5) {
                return 160;
            } else {
                return 160;
            }
        }
        if (ckm < 61) {							// 4 to 6km
            if (RouteDB.getInstance().tax() == 5) {
                return 200;
            } else {
                return 220;
            }
        }
        if (ckm < 101) {						// 7 to 10km
            if (RouteDB.getInstance().tax() == 5) {
                return 220;
            } else {
                return 230;
            }
        }

        fare = Fare_table("bspekm", (RouteDB.getInstance().tax() == 5) ? "k5" : "k8", ckm);
        if (0 != fare) {
            return fare;
        }

        if (6000 < ckm) {						// 600km越えは40キロ刻み
            c_km = (ckm - 1) / 400 * 400 + 200;
        } else if (1000 < ckm) {				// 100.1-600kmは20キロ刻み
            c_km = (ckm - 1) / 200 * 200 + 100;
        } else if (500 < ckm) {					// 50.1-100kmは10キロ刻み
            c_km = (ckm - 1) / 100 * 100 + 50;
        } else if (100 < ckm) {					// 10.1-50kmは5キロ刻み
            c_km = (ckm - 1) / 50 * 50 + 30;
        } else {
            /* 0-10kmは上で算出return済み */
            RouteUtil.ASSERT (false);
            c_km = 0;
        }
        if (6000 <= c_km) {
            fare = 1775 * 3000 + 1285 * (6000 - 3000) + 705 * (c_km - 6000);
        } else if (3000 < c_km) {
            fare = 1775 * 3000 + 1285 * (c_km - 3000);
        } else if (1000 < c_km) {
            fare = 1775 * c_km;
        } else {
            /* 10.1-100.0kmはDBで定義 */
            RouteUtil.ASSERT (false);
            fare = 0;
        }

        if (c_km <= 1000) {							// 100km以下は切り上げ
            // 1の位を切り上げ
            fare = (fare + 9999) / 10000 * 10;
        } else {									// 100㎞越えは四捨五入
            fare = (fare + 50000) / 100000 * 100;
        }
        return RouteUtil.taxadd(fare, RouteDB.getInstance().tax());	// tax = +5%, 四捨五入
    }

    // 営業キロから有効日数を得る
    //	calc_fare() =>
    //
    //	@param [in] km    営業キロ
    //	@return 有効日数
    //
    private int days_ticket(int sales_km) {
        if (sales_km < 1001) {
            return 1;
        }
        return (sales_km + 1999) / 2000 + 1;
    }

    //	会社線の運賃を得る
    //	calc_fare() => aggregate_fare_info() =>
    //
    //	@param [in]     station_id1   駅1
    //	@param [in]     station_id2   駅2
    //	@param [in/out] compantFare   [0]区間運賃 / [1]小児運賃 / [2]学割運賃(非適用は0) / [3]併算割引運賃有無(1=無)
    //	@return true : success / false : failuer
    //
    private static CompanyFare Fare_company(int station_id1, int station_id2) {
        CompanyFare companyFare = null;
        String sql;
        final String tsql =
                //"select fare, child, academic from %s" +
                "select fare, academic, flg from %s" +
                        " where " +
                        " ((station_id1=?1 and station_id2=?2) or" +
                        "  (station_id1=?2 and station_id2=?1))";

        final String tbl[] = { "t_clinfar", "t_clinfar5p"};

        sql = String.format(tsql, tbl[(RouteDB.getInstance().tax() == 5) ? 1 : 0]);
        Cursor dbo = RouteDB.db().rawQuery(sql, new String[] {String.valueOf(station_id1), String.valueOf(station_id2)});	// false));
        boolean rc = false;
        try {
            int flg;
            int fare_work;
            if (dbo.moveToNext()) {
                companyFare = new CompanyFare();
                fare_work = dbo.getInt(0);	// fare
                companyFare.fare = fare_work;
        		companyFare.fareAcademic = dbo.getInt(1);	// academic
        		flg = dbo.getInt(2);	// flg
                rc = true;
                companyFare.passflg = RouteUtil.IS_CONNECT_NON_DISCOUNT_FARE(flg);
        		// (0=5円は切り捨て, 1=5円未満切り上げ)
        		if (RouteUtil.IS_ROUND_UP_CHILDREN_FARE(flg)) {
        			companyFare.fareChild = RouteUtil.round_up(fare_work / 2);
        		} else {
        			companyFare.fareChild = RouteUtil.round_down(fare_work / 2);
        		}
            }
        } finally {
            dbo.close();
        }
        return companyFare;
    }

    //static
    //	運賃テーブル参照
    //	calc_fare() => retr_fare() =>
    //
    //	@param [in] tbl  table name postfix"
    //	@param [in] field name of column for retrieve fare in table.
    //	@param [in] km
    //	@return fare [yen]
    //
    private static int	Fare_table(final String tbl, final String field, int km) {
        final String tsql =
                "select %s from t_fare%s where km<=? order by km desc limit(1)";
        String sql = String.format(tsql, field, tbl);

        Cursor dbo = RouteDB.db().rawQuery(sql, new String[] {String.valueOf(KM.KM(km))});
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
    //	運賃テーブル参照
    //	calc_fare() => retr_fare() =>
    //
    //	@param [in] c   'h'=hokkaido, 'e'= east or central or west
    //	@param [in] km
    //	@retval 0 db error
    //	@retval Number of negative ; -fare
    //	@retval Positive of negative ; c_km
    //
    private static int	Fare_table(final String tbl, char c, int km) {
        int ckm;
        int fare;

        String sql = String.format(
                "select ckm, %c%d from t_fare%s where km<=? order by km desc limit(1)",
                c, RouteDB.getInstance().tax(), tbl);

        Cursor dbo = RouteDB.db().rawQuery(sql, new String[] {String.valueOf(KM.KM(km))});
        int rc = 0;
        try {
            if (dbo.moveToNext()) {
                ckm = dbo.getInt(0);
                fare = dbo.getInt(1);
                if (fare == 0) {
                    rc = ckm;
                } else {
                    rc = -fare;
                }
            }
        } finally {
            dbo.close();
        }
        return rc;
    }

    //static
    //	運賃テーブル参照(ls)
    //	calc_fare() => retr_fare() =>
    //
    //	@param [in] dkm   擬制キロ
    //	@param [in] skm	  営業キロ
    //	@param [in] c     's': 四国 / 'k': 九州
    //	@return value
    //
    private static int Fare_table(int dkm, int skm, char c)	{
        int fare;

        String sql = String.format(

                "select %c%d from t_farels where dkm=?2 and (skm=-1 or skm=?1)",
                c, RouteDB.getInstance().tax());
    /*
    "select k,l from t_farels where "
    "(-1=(select skm from t_farels where skm=(select max(skm) from t_farels where skm<=?1))"
    "	or"
    " skm=(select skm from t_farels where skm=(select max(skm) from t_farels where skm<=?1))"
    ") and dkm="
    "(select dkm from t_farels where dkm=(select max(dkm) from t_farels where dkm<=?2))";
    */
        Cursor dbo = RouteDB.db().rawQuery(sql, new String[] {String.valueOf(KM.KM(skm)), String.valueOf(KM.KM(dkm))});
        try {
            if (dbo.moveToNext()) {
                fare = dbo.getInt(0);
            } else {
                fare = 0;
            }
        } finally {
            dbo.close();
        }
        return fare;
    }


    //	特別通過運賃区間か判定し通過していたら加算運賃額を返す
    //	calc_fare() => aggregate_fare_info() =>
    //
    //	@param [in] line_id     路線
    //	@param [in] station_id1 駅1
    //	@param [in] station_id2 駅2
    //
    //	@return 駅1～駅2に運賃と区別区間が含まれている場合その加算額を返す
    //
    private static int CheckSpecficFarePass(int line_id, int station_id1, int station_id2) {
        String sql = String.format(
                "select station_id1, station_id2, fare%dp from t_farespp f where kind=0 and exists (" +
                        "select *" +
                        "	from t_lines" +
                        "	where line_id=?1" +
                        "	and station_id=f.station_id1" +
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
                        "				 station_id=?3)))" +
                        " and exists (" +
                        " select *" +
                        "	from t_lines" +
                        "	where line_id=?1" +
                        "	and station_id=f.station_id2" +
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
                        "				 station_id=?3)))" +
                        " order by fare%dp desc" +
                        " limit(1)", RouteDB.getInstance().tax(), RouteDB.getInstance().tax());

        Cursor dbo = RouteDB.db().rawQuery(sql, new String[] {String.valueOf(line_id),
                String.valueOf(station_id1),
                String.valueOf(station_id2)});
        int fare = 0;
        try {
            if (dbo.moveToNext()) {
                fare = dbo.getInt(2);
                System.out.printf("CheckSpecificFarePass found: %s, %s, +%d\n", RouteUtil.StationName(dbo.getInt(0)), RouteUtil.StationName(dbo.getInt(1)), fare);
                /* found, return values is add fare */
            }
        } finally {
            dbo.close();
        }
        return fare;	/* not found */
    }

    //	特別運賃区間か判定し該当していたら運賃額を返す
    //	calc_fare() =>
    //
    //	@param [in] station_id1 駅1(startStationId)
    //	@param [in] station_id2 駅2(end_station_id)
    //
    //	@return 特別区間運賃
    //
    private static int SpecificFareLine(int station_id1, int station_id2, int kind) {
        String sql;

        final String tsql =
                "select fare%dp from t_farespp where kind=?3 and" +
                        " ((station_id1=?1 and station_id2=?2) or" +
                        "  (station_id1=?2 and station_id2=?1))";

        sql = String.format(tsql, RouteDB.getInstance().tax());

        Cursor dbo = RouteDB.db().rawQuery(sql, new String[] {
                String.valueOf(station_id1),
                String.valueOf(station_id2),
                String.valueOf(kind)});
        int fare = 0;   /* if not found */
        try {
            if (dbo.moveToNext()) {
                fare = dbo.getInt(0);
                System.out.printf("SpecificFareLine found: %s - %s, +%d\n", RouteUtil.StationName(station_id1), RouteUtil.StationName(station_id2), fare);
            }
        } finally {
            dbo.close();
        }
        return fare;
    }

    /*	路線の2点間営業キロ、計算キロ、会社(JR-Gr.)境界を得る
     *	calc_fare() => aggregate_fare_info() =>
     *
     *	@param [in] line_id   	 路線ID
     *	@param [in] station_id1  駅1
     *	@param [in] station_id2  駅2
     *
     *	@return int[] [0] 営業キロ
     *	@return int[] [1] 計算キロ
     *	@return int[] [2] 駅1の会社区間部の営業キロ(駅1の会社ID≠駅2の会社ID時のみ有効)
     *                          駅1が境界駅なら-1を返す, 境界駅が駅1～駅2間になければ、Noneを返す
     *	@return int[] [3] 駅1の会社区間部の計算キロ(駅1の会社ID≠駅2の会社ID時のみ有効)
     *                          駅2が境界駅なら-1を返す, 境界駅が駅1～駅2間になければ、Noneを返す
     *	@return int[] [4] 駅1の会社ID
     *	@return int[] [5] 駅2の会社ID
     *	@return int[] [6] 駅1のsflg
     *	@return int[] [7] 駅2のsflg
     *	@return int[] [8] 1=JR以外の会社線／0=JRグループ社線
    */
    private Integer[] getDistanceEx(int line_id, int station_id1, int station_id2) {
        DbidOf dbid = new DbidOf();
        Integer[] result = new Integer[9];

        Cursor ctx = RouteDB.db().rawQuery(
                "select" +
                        "	(select max(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))-" +
                        "	(select min(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3)),"	+	// [0]
                        "	(select max(calc_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))-" +
                        "	(select min(calc_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3)),"	+	// [1]
                        "	case when exists (select * from t_lines	where line_id=?1 and (lflg&((1<<16)|(1<<31)))=(1<<16) and station_id=?2)" +
                        "	then -1 else" +
                        "	abs((select sales_km from t_lines where line_id=?1 and (lflg&((1<<16)|(1<<31)))=(1<<16)" +
                        "	and	sales_km>(select min(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))" +
                        "	and	sales_km<(select max(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3)))-" +
                        "	(select sales_km from t_lines where line_id=?1 and station_id=?2)) end," + 						// [2]
                        "	case when exists (select * from t_lines" +
                        "	where line_id=?1 and (lflg&((1<<16)|(1<<31)))=(1<<16) and station_id=?3)" +
                        "	then -1 else" +
                        "	abs((select calc_km from t_lines where line_id=?1 and (lflg&((1<<16)|(1<<31)))=(1<<16)" +
                        "	and	sales_km>(select min(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))" +
                        "	and	sales_km<(select max(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3)))-" +
                        "	(select calc_km from t_lines where line_id=?1 and station_id=?2)) end," +							// [3]
                        "	(select company_id from t_station where rowid=?2),"		+		// [4](4)
                        "   (select company_id from t_station where rowid=?3),"	    +       // [4](5)
                        "	(select sub_company_id from t_station where rowid=?2),"	+		// [5](6)
                        "	(select sub_company_id from t_station where rowid=?3)," +       // [5](7)
                        "	(select sflg&4095 from t_station where rowid=?2)," +                        // [6](8)
                        "   (select sflg&4095 from t_station where rowid=?3)," +	                    // [7](9)
                        "	(select (1&(lflg>>18)) from t_lines where line_id=?1 and station_id=?3)" // [8](10)
                , new String[] {String.valueOf(line_id), String.valueOf(station_id1), String.valueOf(station_id2)});
        int company_id1;
    	int company_id2;
    	int sub_company_id1;
    	int sub_company_id2;
        try {
            //	2147483648 = 0x80000000
            if (ctx.moveToNext()) {
                result[0] = ctx.getInt(0);	// sales_km
                result[1] = ctx.getInt(1);	// calc_km
                result[2] = ctx.getInt(2);	// sales_km for in company as station_id1
                result[3] = ctx.getInt(3);	// calc_km  for in company as station_id1
                company_id1 = ctx.getInt(4);     // [4]駅ID1の会社ID
    			company_id2 = ctx.getInt(5);     // [5]駅ID2の会社ID
    			sub_company_id1 = ctx.getInt(6);
    			sub_company_id2 = ctx.getInt(7);
    			result[6] = (ctx.getInt(8));	// 駅1のsflg
    			result[7] = (ctx.getInt(9));	// 駅2のsflg
    			result[8] = (ctx.getInt(10));	// 1=JR以外の会社線／0=JRグループ社線

    			if ((line_id == dbid.LineIdOf_HAKATAMINAMISEN) ||
    				(line_id == dbid.LineIdOf_SANYOSHINKANSEN)) { //山陽新幹線、博多南線はJ九州内でもJR西日本
    				result[4] = RouteUtil.JR_WEST;
    				result[5] = RouteUtil.JR_WEST;
    			}
    			else if (line_id == RouteUtil.LINE_HOKKAIDO_SINKANSEN) {
    				result[4] = RouteUtil.JR_HOKKAIDO;
    				result[5] = RouteUtil.JR_HOKKAIDO;
    			}
    			else if ((line_id == dbid.LineIdOf_TOKAIDOSHINKANSEN) &&
    				     ((company_id1 == company_id2) && (company_id1 != RouteUtil.JR_CENTRAL))) {
    				/*
    					東 西 -> あとで救われる
    					東 海
    					海 東
    					海 海
    					東 東 -> 東 海
    					西 西 -> 西 海 にする　上り、下りの情報は不要
    					西 東
    					西 海
    					海 西
    				*/
    				result[4] = company_id1;
    				result[5] = RouteUtil.JR_CENTRAL;
    			}
    			else {
    // 猪谷(4)(4) 富山(4)
    // 猪谷(4)(3) 名古屋(3)
    // 神戸(4)　門司(3)(4)
    // 品川(2)-新大阪(4)
    // 長野(2)-金沢(4)
     // 同一路線で会社A-会社B-会社Aなどというパターンはあり得ない
     // A-B-Cはあり得る。熱海 新幹線 京都　の場合、JR海-西であるべきだが　東-西となる(現在は本州3社は同一と見て問題ない)
    				if (RouteUtil.IS_JR_MAJOR_COMPANY(company_id1) && RouteUtil.IS_JR_MAJOR_COMPANY(company_id2) && (company_id1 != company_id2)) {
    					if (sub_company_id1 == company_id2) {
    						company_id1 = sub_company_id1;
    					}
    					else if (company_id1 == sub_company_id2) {
    						company_id2 = sub_company_id2;
    					}
    				}
    				result[4] = company_id1;
    				result[5] = company_id2;
    			}
            }
        } finally {
            ctx.close();
        }
        return result;
    }

    /**
     *   case 大阪環状線
     *
     *	calc_fare() => aggregate_fare_info() =>
     *
     *	@param [in] route flag 	 大阪環状線通過フラグ
     *	@param [in] line_id   	 路線ID
     *	@param [in] station_id1  駅1
     *	@param [in] station_id2  駅2
     *
     *	@return int[] [0] 営業キロ
     *	@return int[] [1] 計算キロ
     *	@return int[] [2] 駅1の会社区間部の営業キロ(駅1の会社ID≠駅2の会社ID時のみ有効)
     *                          駅1が境界駅なら-1を返す, 境界駅が駅1～駅2間になければ、Noneを返す
     *	@return int[] [3] 駅1の会社区間部の計算キロ(駅1の会社ID≠駅2の会社ID時のみ有効)
     *                          駅2が境界駅なら-1を返す, 境界駅が駅1～駅2間になければ、Noneを返す
     *	@return int[] [4] 駅1の会社ID
     *	@return int[] [5] 駅2の会社ID
     *	@return int[] [6] 駅1のsflg
     *	@return int[] [7] 駅2のsflg
     *	@return int[] [8] 1=JR以外の会社線／0=JRグループ社線
     *
     */
    private static Integer[] GetDistanceEx(final LastFlag osakakan_aggregate, int line_id, int station_id1, int station_id2) {
        List<Integer> result;
        long rslt = 0;

        result = RouteUtil.GetDistance(osakakan_aggregate, line_id, station_id1, station_id2); // [0][1]
        result.add(0);	// sales_km for in company as station_id1 [2]
        result.add(0);	// calc_km  for in company as station_id1 [3]
        result.add(RouteUtil.JR_WEST);	// 駅ID1の会社ID [4]
        result.add(RouteUtil.JR_WEST);	// 駅ID2の会社ID [5]
        Cursor ctx = RouteDB.db().rawQuery("select" +
                " (select sflg&4095 from t_station where rowid=?1) + ((select sflg&4095 from t_station where rowid=?2) * 65536)"		// [5]
                , new String[] {String.valueOf(station_id1), String.valueOf(station_id2)});
        try {
            if (ctx.moveToNext()) {
                rslt = ctx.getLong(0);
            }
        } finally {
            ctx.close();
        }
        result.add((int)(0xffff & rslt));			// 駅1のsflg [6]
        result.add((int)(0xffff & (rslt >>> 16)));	// 駅2のsflg [7]
        result.add((int)(0x1 & (rslt >>> 31)));		// bit31:1=JR以外の会社線／0=JRグループ社線 = 0

        return result.toArray(new Integer[0]);
    }


    //static
    //	@brief 近郊区間でない条件となる新幹線乗車があるか？
    //
    //	@param [in] line_id
    //	@param [in] station_id1		駅1
    //	@param [in] station_id2		駅2
    //
    //	@return true if 近郊区間でない条件
    //                flag    N  N-flag return
    //  姫路-新大阪    3      4   1      t
    //  姫路 新神戸    2      3   1      t
    //  姫路 相生      2      2   0
    //  米原 新大阪    3      3   0
    //  新富士 静岡    0      2   2      t
    //  静岡 米原      1      8   7      t
    //  静岡 名古屋    0      6   6      t
    //  品川 東京      0      2   2      t
    private static boolean IsBulletInUrban(int line_id, int station_id1, int station_id2) {
        final String tsql =
                " select count(*) - sum(sflg>>13&1) from t_station t join t_lines l on l.station_id=t.rowid" +
                        "	where line_id=?1" +
                        "	and (lflg&((1<<31)|(1<<17)))=0" +
                        "	and sales_km>=(select min(sales_km) from t_lines" +
                        "			where line_id=?1 and (station_id=?2 or station_id=?3))" +
                        "	and sales_km<=(select max(sales_km) from t_lines" +
                        "			where line_id=?1 and (station_id=?2 or station_id=?3))";
        // 13:近郊区間、17:新幹線仮想分岐駅

        int rsd = -1;

        if (!RouteUtil.IS_SHINKANSEN_LINE(line_id)) {
            return false;
        }

        Cursor dbo = RouteDB.db().rawQuery(tsql, new String[] {String.valueOf(line_id),
                String.valueOf(station_id1),
                String.valueOf(station_id2)});
        try {
            if (dbo.moveToNext()) {
                rsd = dbo.getInt(0);
            }
        } finally {
            dbo.close();
        }
        return 0 < rsd;
    }


    private static boolean IsIC_area(int urban_id) {
        return ((urban_id == RouteUtil.URB_TOKYO) ||
                (urban_id == RouteUtil.URB_NIGATA) ||
                (urban_id == RouteUtil.URB_SENDAI));
    }

    //static:
    //	旅客営業取扱基準規定43条の2（小倉、西小倉廻り）
    //	@param [out] fare_inf  営業キロ(sales_km, kyusyu_sales_km, kyusyu_calc_km)
    //
    //	showFare() => calc_fare() ->
    //	CheckOfRule114j() => calc_fare ->
    //
    private static int CheckAndApplyRule43_2j(final RouteItem[] route) {
        DbidOf dbid = new DbidOf();
        int stage;
        int c;
        int rl;
        int km;
        int kagoshima_line_id;
        int ite = 0;

        stage = 0;
        c = 0;
        rl = 0;
        for (ite = 0; ite < route.length; ite++) {
            switch (stage) {
                case 0:
                    if ((0 < route[ite].lineId) && (route[ite].stationId == dbid.StationIdOf_NISHIKOKURA)) {
                        c = 1;
                        stage = 1;
                    } else if (route[ite].stationId == dbid.StationIdOf_HAKATA) {
                        if (route[ite].lineId == dbid.LineIdOf_SANYOSHINKANSEN) {
                            c = 4;			// 山陽新幹線
                        } else {
                            c = 2;
                        }
                        stage = 1;
                    } else if ((0 < route[ite].lineId) && (route[ite].stationId == dbid.StationIdOf_YOSHIZUKA)) {
                        c = 3;				// 吉塚
                        stage = 1;
                        break;
                    }
                    break;
                case 1:
                    switch (c) {
                        case 1:
                            if ((route[ite].stationId == dbid.StationIdOf_KOKURA) &&
                                    ((ite + 1) < route.length) &&
                                    (route[ite + 1].lineId == dbid.LineIdOf_SANYOSHINKANSEN) &&
                                    (route[ite + 1].stationId == dbid.StationIdOf_HAKATA)) {
                                rl |= 1;	// 小倉 山陽新幹線 博多
                            }
                            break;
                        case 2:
                            if ((route[ite].stationId == dbid.StationIdOf_KOKURA) &&
                                    (route[ite].lineId == dbid.LineIdOf_SANYOSHINKANSEN) &&
                                    ((ite + 2) < route.length) && // 西小倉止まりまでなら非適用
                                    (route[ite + 1].stationId == dbid.StationIdOf_NISHIKOKURA)) {
                                rl |= 1;
                            }
                            break;
                        case 3:
                            if ((route[ite].stationId == dbid.StationIdOf_HAKATA) &&
                                    ((ite + 1) < route.length) &&
                                    (route[ite + 1].lineId == dbid.LineIdOf_SANYOSHINKANSEN)) {
                                rl |= 2;
                            }
                            break;
                        case 4:
                            if ((route[ite].stationId == dbid.StationIdOf_YOSHIZUKA) &&
                                    ((ite + 1) < route.length)) {	// 吉塚止まりまでなら非適用
                                rl |= 2;
                            }
                            break;
                        default:
                            break;
                    }
                    c = 0;
                    stage = 0;
                    break;
                default:
                    RouteUtil.ASSERT (false);
                    break;
            }
        }
        if (rl != 0) {
            km = 0;
            kagoshima_line_id = RouteUtil.GetLineId("鹿児島線");
            if ((rl & 1) != 0) {
                km = RouteUtil.GetDistance(kagoshima_line_id, dbid.StationIdOf_KOKURA, dbid.StationIdOf_NISHIKOKURA).get(0);
                System.out.printf("applied 43-2(西小倉)\n");
            }
            if ((rl & 2) != 0) {
                km += RouteUtil.GetDistance(kagoshima_line_id, dbid.StationIdOf_HAKATA, dbid.StationIdOf_YOSHIZUKA).get(0);
                System.out.printf("applied 43-2(吉塚)\n");
            }
            return km;
        }
        return 0;
    }


    //static:
    //	showFare() => calc_fare() =>
    //	CheckOfRule114j() => calc_fare() =>
    //
    //	89条のチェックと変換
    //	北新地発（着）-[JR東西線]-尼崎なら、北新地→大阪置換
    //
    //	@param [in] route    route
    //
    //	@retval 0: no-convert
    //	@retval <0: 大阪-尼崎の営業キロ - 北新地-尼崎の営業キロ(負数)
    //
    private static int CheckOfRule89j(final List<RouteItem> route) {
        DbidOf dbid = new DbidOf();
        int lastIndex;
		/*static */int distance = 0;

        lastIndex = (int)route.size() - 1;
        if (lastIndex < 2) {
            return 0;
        }

        // 北新地発やで
        if (((route.get(0).stationId == dbid.StationIdOf_KITASHINCHI) &&
                (route.get(1).stationId == dbid.StationIdOf_AMAGASAKI)) &&
                ((lastIndex <= 1) || (route.get(2).lineId != dbid.LineIdOf_TOKAIDO) ||
                        (RouteUtil.LINE_DIR.LDIR_ASC == RouteUtil.DirLine(dbid.LineIdOf_TOKAIDO, dbid.StationIdOf_AMAGASAKI, route.get(2).stationId)))) {
			/* 北新地-(JR東西線)-尼崎 の場合、発駅（北新地）は大阪や */
            // route.get(0).stationId = dbid.StationIdOf_OSAKA;
            //if (distance == 0) {
            distance = RouteUtil.GetDistance(dbid.LineIdOf_TOKAIDO,   dbid.StationIdOf_OSAKA, dbid.StationIdOf_AMAGASAKI).get(0) -
                    RouteUtil.GetDistance(route.get(1).lineId, dbid.StationIdOf_AMAGASAKI, dbid.StationIdOf_KITASHINCHI).get(0);
            //}
            RouteUtil.ASSERT (distance < 0);
            return distance;
        }			// 北新地終着やねん
        else if (((route.get(route.size() - 1).stationId == dbid.StationIdOf_KITASHINCHI) &&
                (route.get(lastIndex - 1).stationId == dbid.StationIdOf_AMAGASAKI)) &&
                ((lastIndex <= 1) || (route.get(lastIndex - 1).lineId != dbid.LineIdOf_TOKAIDO) ||
                        (RouteUtil.LINE_DIR.LDIR_DESC == RouteUtil.DirLine(dbid.LineIdOf_TOKAIDO, route.get(lastIndex - 2).stationId, dbid.StationIdOf_AMAGASAKI)))) {
            //route.get(route.size() - 1).stationId = dbid.StationIdOf_OSAKA;
            //if (distance == 0) {
            distance = RouteUtil.GetDistance(dbid.LineIdOf_TOKAIDO,   dbid.StationIdOf_OSAKA, dbid.StationIdOf_AMAGASAKI).get(0) -
                    RouteUtil.GetDistance(route.get(route.size() - 1).lineId, dbid.StationIdOf_AMAGASAKI, dbid.StationIdOf_KITASHINCHI).get(0);
            //}
            RouteUtil.ASSERT (distance < 0);
            return distance;
        } else {
            return 0;
        }
    }

    //static
    //
    //  経路はJR東海管内のみか？
    //  @retval true: JR東海管内のみ / false=以外
    //  @note 新幹線 東京ー熱海間はJR東日本エリアだけどJR東海エリアなのでその判定をやる
    //
    static boolean CheckIsJRTokai(List<RouteItem> route)
    {
    /*
        b:発駅が境界駅ならtrue
        f:着駅が境界駅ならtrue
        -:true
        n:新幹線ならtrue
        x:false
    */
        int station_id1 = 0;
        int [] cid1 = {0, 0};
        int cid_s1;
        int cid_e1;
        int cid_s2;
        int cid_e2;

        /* JR東海以外 and 東海道新幹線でない場合false */
        for (RouteItem ite : route) {
            int [] cid = RouteUtil.CompanyIdFromStation(ite.stationId);
            if (station_id1 != 0) {
                if (ite.lineId != DbidOf.id().LineIdOf_TOKAIDOSHINKANSEN) {
                    cid_e1 = cid[0];
                    cid_s1 = cid1[0];
                    cid_e2 = cid[1];
                    cid_s2 = cid1[1];
                    if (((cid_s1 == cid_e1) && (RouteUtil.JR_CENTRAL != cid_e1)) ||   /* 塩尻-甲府 */
                            ((cid_s1 != RouteUtil.JR_CENTRAL) && (cid_s2 != RouteUtil.JR_CENTRAL)) ||
                            ((cid_e1 != RouteUtil.JR_CENTRAL) && (cid_e2 != RouteUtil.JR_CENTRAL))) {
                        return false;
                    }
                }
            }
            station_id1 = ite.stationId;
            System.arraycopy(cid, 0, cid1, 0, cid.length);
        }
        System.out.printf("isJrTokai true\n");
        return true;
    }


    int		getResultCode() {
        if (RouteUtil.BIT_CHK(result_flag, BRF_ROUTE_INCOMPLETE)) {
            return -1;
        } else {
            return 0;
        }
    }



} // FARE_INFO
