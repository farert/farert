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


 internal class CompanyFare {
 var fare:Int = 0
 var fareChild:Int = 0
 var fareAcademic:Int = 0
 var passflg:Boolean = false        // 弊算割引有無
init{
fare = 0
fareChild = 0
fareAcademic = 0
passflg = false
}
}

 class FARE_INFO internal constructor() {

init{
reset()
}

/**	総営業キロを返す

     	 * @retval 営業キロ
 */
    internal var totalSalesKm:Int = 0			//*** 有効日数計算用(会社線含む)

internal var base_sales_km:Int = 0		//*** JR本州3社
internal var base_calc_km:Int = 0		//***

/**	JR九州の営業キロを返す

     	 * @retval	営業キロ
 */
    internal var salesKmForKyusyu:Int = 0	//***
/**	JR九州の計算キロを返す

     	 * @retval	計算キロ
 */
    internal var calcKmForKyusyu:Int = 0		//***

/**	JR北海道の営業キロを返す

     	 * @retval 営業キロ
 */
    internal var salesKmForHokkaido:Int = 0
/**	JR北海道の計算キロを返す

     	 * @retval	計算キロ
 */
    internal var calcKmForHokkaido:Int = 0

/**	JR四国の営業キロを返す

     	 * @retval	営業キロ
 */
    internal var salesKmForShikoku:Int = 0
/**	JR四国の計算キロを返す

     	 * @retval	計算キロ
 */
    internal var calcKmForShikoku:Int = 0

internal var local_only:Boolean = false				/* True: 地方交通線のみ (0 < base_sales_km時のみ有効)*/
internal var local_only_as_hokkaido:Boolean = false	/* True: 北海道路線地方交通線のみ(0 < hokkaidou_sales_km時のみ有効) */
 //幹線のみ
    // (base_sales_km == base_calc_km) && (kyusyu_sales_km == kyusyu_calc_km) &&
    // (hokkaido_sales_km == hokkaido_calc_km) && (shikoku_sales_km == shikoku_calc_km)
    internal var major_only:Boolean = false				/* 幹線のみ */
/**	JR線の営業キロを返す

     	 * @retval 計算キロ
 */
    internal var jrSalesKm:Int = 0			//***
/**	JR線の計算キロを返す

     	 * @retval 計算キロ
 */
    internal var jrCalcKm:Int = 0			//***

/**	会社線の運賃額を返す

     	 * @retval	[円]
 */
    internal var fareForCompanyline:Int = 0					/* 会社線料金 */
internal var company_fare_ac_discount:Int = 0		/* 学割用会社線割引額 */
internal var company_fare_child:Int = 0				/* 会社線小児運賃 */
internal var result_flag:Int = 0					/* 結果状態: BRF_xxx */

internal var flag:Int = 0						//***/* IDENT1: 全t_station.sflgの論理積 IDENT2: bit16-22: shinkansen ride mask  */
/**	JR線の運賃額を返す(114条未適用計算値)

     	 * @retval	[円]
 */
    internal var fareForJR:Int = 0					//***
internal var fare_ic:Int = 0					//*** 0以外で有効
/**	乗車券の有効日数を返す

     	 * @retval	有効日数[日]
 */
    internal var ticketAvailDays:Int = 0					//***

internal var companymask:Int = 0

 /* 114 */
    internal var rule114_fare:Int = 0
internal var rule114SalesKm:Int = 0
internal var rule114CalcKm:Int = 0

internal/* roundTripFareWithCompanyLine() を前に呼んでいること */ var isRoundTripDiscount:Boolean = false

internal var route_string:String
internal var beginTerminalId:Int = 0
internal var endTerminalId:Int = 0

 //static
    //	集計された営業キロ、計算キロより運賃額を算出(運賃計算最終工程)
    //	(JRのみ)
    //	calc_fare() =>
    //
    //	@retval true Success
    //	@retval false Fatal error(会社線のみJR無し)
    //
    private fun retr_fare():Boolean {
val fare_tmp:Int
this.jrSalesKm = this.base_sales_km +
this.salesKmForKyusyu +
this.salesKmForHokkaido +
this.salesKmForShikoku

this.jrCalcKm = this.base_calc_km +
this.calcKmForKyusyu +
this.calcKmForHokkaido +
this.calcKmForShikoku

 // 本州3社あり？
        if (0 < this.base_sales_km + this.base_calc_km)
{
if (RouteUtil.IS_OSMSP(this.flag))
{
 /* 大阪電車特定区間のみ */
                RouteUtil.ASSERT(this.fareForJR == 0) /* 特別加算区間を通っていないはずやねん */
RouteUtil.ASSERT(this.fareForCompanyline == 0)	// 会社線は通ってへん
RouteUtil.ASSERT(this.base_sales_km == this.jrSalesKm)
RouteUtil.ASSERT(this.base_sales_km == this.totalSalesKm)
if (RouteUtil.IS_YAMATE(this.flag))
{
System.out.printf("fare(osaka-kan)\n")
this.fareForJR = Fare_osakakan(this.jrSalesKm)
}
else
{
System.out.printf("fare(osaka)\n")
this.fareForJR = Fare_osaka(this.jrSalesKm)
}
}
else if (RouteUtil.IS_TKMSP(this.flag))
{
 /* 東京電車特定区間のみ */
                RouteUtil.ASSERT(this.fareForJR == 0) /* 特別加算区間を通っていないはずなので */
RouteUtil.ASSERT(this.fareForCompanyline == 0)	// 会社線は通っていない
RouteUtil.ASSERT(this.base_sales_km == this.jrSalesKm)
RouteUtil.ASSERT(this.base_sales_km == this.totalSalesKm)

if (RouteUtil.IS_YAMATE(this.flag))
{
System.out.printf("fare(yamate)\n")
fare_tmp = Fare_yamate_f(this.jrSalesKm)
}
else
{
System.out.printf("fare(tokyo)\n")
fare_tmp = Fare_tokyo_f(this.jrSalesKm)
}
if (RouteDB.instance.tax() == 5)
{
this.fareForJR = RouteUtil.round(fare_tmp)
}
else
{
 /* 新幹線乗車はIC運賃適用外 */
                    if (1 shl RouteUtil.BCBULURB and this.flag == 0)
{
this.fare_ic = fare_tmp
}
this.fareForJR = RouteUtil.round_up(fare_tmp)
}
}
else if (this.local_only || !this.major_only && this.jrSalesKm <= 100)
{
 /* 本州3社地方交通線のみ or JR東+JR北 */
                /* 幹線+地方交通線でトータル営業キロが10km以下 */
                // (i)<s>
                System.out.printf("fare(sub)\n")

fare_tmp = Fare_sub_f(this.jrSalesKm)

if (RouteDB.instance.tax() != 5 &&
IsIC_area(RouteUtil.URBAN_ID(this.flag)) &&   /* 近郊区間(最短距離で算出可能) */
 /* 新幹線乗車はIC運賃適用外 */
                        1 shl RouteUtil.BCBULURB and this.flag == 0)
{

RouteUtil.ASSERT(companymask == 1 shl RouteUtil.JR_EAST - 1)  /* JR East only  */

this.fare_ic = fare_tmp
}
this.fareForJR = RouteUtil.round(fare_tmp)

}
else
{ /* 幹線のみ／幹線+地方交通線 */
 // (a) + this.calc_kmで算出
                System.out.printf("fare(basic)\n")

fare_tmp = Fare_basic_f(this.jrCalcKm)

if (RouteDB.instance.tax() != 5 && /* IC運賃導入 */
IsIC_area(RouteUtil.URBAN_ID(this.flag)) &&   /* 近郊区間(最短距離で算出可能) */
 /* 新幹線乗車はIC運賃適用外 */
                        1 shl RouteUtil.BCBULURB and this.flag == 0)
{

RouteUtil.ASSERT(companymask == 1 shl RouteUtil.JR_EAST - 1)  /* JR East only  */

this.fare_ic = fare_tmp
}
this.fareForJR = RouteUtil.round(fare_tmp)
}

 // JR北あり?
            if (0 < this.salesKmForHokkaido + this.calcKmForHokkaido)
{
 /* JR東 + JR北 */
                if (this.local_only_as_hokkaido || !this.major_only && this.jrSalesKm <= 100)
{
 /* JR北は地方交通線のみ */
                    /* or 幹線+地方交通線でトータル営業キロが10km以下 */
                    // (r) sales_km add
                    System.out.printf("fare(hla)\n")		// System.out.printf("fare(r,r)\n");
this.fareForJR += Fare_table(if (RouteDB.instance.tax() == 5) "hla5p" else "hla", "ha",
this.salesKmForHokkaido)

}
else
{ /* 幹線のみ／幹線+地方交通線で10km越え */
 // (o) calc_km add
                    System.out.printf("fare(add, ha)\n")	// System.out.printf("fare(opq, o)\n");
this.fareForJR += Fare_table(if (RouteDB.instance.tax() == 5) "add5p" else "add", "ha",
this.calcKmForHokkaido)
}
}				// JR九あり？
if (0 < this.salesKmForKyusyu + this.calcKmForKyusyu)
{
 /* JR西 + JR九 */
                /* 幹線のみ、幹線+地方交通線 */

                // JR九州側(q)<s><c> 加算
                System.out.printf("fare(add, ka)\n")	// System.out.printf("fare(opq, q)\n");
this.fareForJR += Fare_table(if (RouteDB.instance.tax() == 5) "add5p" else "add", "ka",
this.calcKmForKyusyu)
}				// JR四あり?
if (0 < this.salesKmForShikoku + this.calcKmForShikoku)
{
 /* JR西 + JR四 */
                /* 幹線のみ、幹線+地方交通線 */

                // JR四国側(p)<s><c> 加算
                System.out.printf("fare(add, sa)\n")	// System.out.printf("fare(opq, p)\n");
this.fareForJR += Fare_table(if (RouteDB.instance.tax() == 5) "add5p" else "add", "sa",
this.calcKmForShikoku)
}				// JR北
}
else if (0 < this.salesKmForHokkaido + this.calcKmForHokkaido)
{
 /* JR北海道のみ */
            RouteUtil.ASSERT(this.jrSalesKm == this.salesKmForHokkaido)
RouteUtil.ASSERT(this.jrCalcKm == this.calcKmForHokkaido)
RouteUtil.ASSERT(this.fareForJR == 0)

if (this.local_only_as_hokkaido)
{
 /* JR北海道 地方交通線のみ */
                // (j)<s>
                System.out.printf("fare(hokkaido_sub)\n")
this.fareForJR = Fare_hokkaido_sub(this.jrSalesKm)
}
else
{
 /* JR北海道 幹線のみ、幹線+地方交通線 */
                // (f)<c>
                System.out.printf("fare(hokkaido-basic)\n")
this.fareForJR = Fare_hokkaido_basic(this.jrCalcKm)

}				// JR九
}
else if (0 < this.salesKmForKyusyu + this.calcKmForKyusyu)
{
 /* JR九州のみ */
            RouteUtil.ASSERT(this.jrSalesKm == this.salesKmForKyusyu)
RouteUtil.ASSERT(this.jrCalcKm == this.calcKmForKyusyu)
RouteUtil.ASSERT(this.fareForJR == 0)

if (this.local_only)
{
 /* JR九州 地方交通線 */
                System.out.printf("fare(ls)'k'\n")
 /* (l) */
                this.fareForJR = Fare_table(this.jrCalcKm, this.jrSalesKm, 'k')

}
if (this.fareForJR == 0)
{
 /* JR九州 幹線のみ、幹線＋地方交通線、地方交通線のみ の(l), (n)非適用 */
                // (h)<s><c>
                System.out.printf("fare(kyusyu)\n")			// System.out.printf("fare(m, h)[9]\n");
this.fareForJR = Fare_kyusyu(this.jrSalesKm,
this.jrCalcKm)
}

}
else if (0 < this.salesKmForShikoku + this.calcKmForShikoku)
{
 /* JR四国のみ */
            RouteUtil.ASSERT(this.jrSalesKm == this.salesKmForShikoku)
RouteUtil.ASSERT(this.jrCalcKm == this.calcKmForShikoku)
RouteUtil.ASSERT(this.fareForJR == 0)

if (this.local_only)
{
 /* JR四国 地方交通線 */
                System.out.printf("fare(ls)'s'\n")
 /* (k) */
                this.fareForJR = Fare_table(this.jrCalcKm, this.jrSalesKm, 's')

}
if (this.fareForJR == 0)
{
 /* JR四国 幹線のみ、幹線＋地方交通線、地方交通線のみ の(l), (n)非適用 */
                // (g)<s><c>
                System.out.printf("fare(shikoku)[4]\n")		// System.out.printf("fare(m, g)[4]\n");
this.fareForJR = Fare_shikoku(this.jrSalesKm,
this.jrCalcKm)
}

}
else
{
 /* 会社線のみ */
            //RouteUtil.ASSERT (false);   青森始発からIGRで盛岡方面へ
            return false
}
return true
}

 //	1経路の営業キロ、計算キロを集計
    //	calc_fare() =>
    //
    //	@retval 0 < Success(特別加算区間割増運賃額.通常は0)
    //	@retval -1 Fatal error
    //
    private fun aggregate_fare_info(last_flag:Int, routeList_raw:List<RouteItem>, routeList_cooked:List<RouteItem>?):Int {
var station_id1:Short		/* station_id1, (station_id2=ite.stationId) */
var station_id_0:Short		/* last station_id1(for Company line) */
var osakakan_aggregate:Int	// 大阪環状線通過フラグ bit0: 通過フラグ
 //
        var fare_add = 0						// 運賃特別区間加算値
val routeList = routeList_cooked ?: routeList_raw

this.local_only = true
this.local_only_as_hokkaido = true
osakakan_aggregate = last_flag
osakakan_aggregate = RouteUtil.BIT_OFF(osakakan_aggregate, RouteList.BLF_OSAKAKAN_1PASS)

station_id1 = 0

 /* 近郊区間ではない条件となる新幹線乗車があるか */
        for (ri in routeList_raw)
{
if (station_id1.toInt() != 0)
{
if (IsBulletInUrban(ri.lineId.toInt(), station_id1.toInt(), ri.stationId.toInt()))
{
this.flag = this.flag or (1 shl RouteUtil.BCBULURB) // ONの場合大都市近郊区間特例無効(新幹線乗車している)
break
}
}
station_id1 = ri.stationId
}

station_id1 = 0
station_id_0 = station_id1

val ite = routeList.iterator()
while (ite.hasNext())
{
val ri = ite.next()
 //RouteUtil.ASSERT ((ite.flag) == 0);
            if (station_id1.toInt() != 0)
{
 /* 会社別営業キロ、計算キロの取得 */

                if (ri.lineId == RouteUtil.ID_L_RULE70)
{
val sales_km:Int
sales_km = Retrieve70Distance(station_id1.toInt(), ri.stationId.toInt())
RouteUtil.ASSERT(0 < sales_km)
this.totalSalesKm += sales_km			// total 営業キロ(会社線含む、有効日数計算用)
this.base_sales_km += sales_km
this.base_calc_km += sales_km
this.local_only = false		// 幹線

}
else
{
var company_id1:Int
var company_id2:Int
var flag:Int
val dex:Array<Int>

if (ri.lineId.toInt() != DbidOf.id().LineIdOf_OOSAKAKANJYOUSEN)
{
dex = this.getDistanceEx(ri.lineId.toInt(), station_id1.toInt(), ri.stationId.toInt())
}
else
{
dex = GetDistanceEx(osakakan_aggregate, ri.lineId.toInt(), station_id1.toInt(), ri.stationId.toInt())
osakakan_aggregate = RouteUtil.BIT_ON(osakakan_aggregate, RouteList.BLF_OSAKAKAN_1PASS)
}

if (9 != dex.size)
{
RouteUtil.ASSERT(false)
return -1	/* failure abort end. >>>>>>>>> */
}
company_id1 = dex[4]
company_id2 = dex[5]

System.out.printf("multicompany line none detect X: %d, %d, comp1,2=%d, %d, %s:%s-%s\n",
dex[2],
dex[3],
company_id1,
company_id2,
RouteUtil.LineName(ri.lineId.toInt()),
RouteUtil.StationName(station_id1.toInt()),
RouteUtil.StationName(ri.stationId.toInt()))

this.companymask = this.companymask or (1 shl company_id1 - 1 or (1 shl company_id2 - 1))

if (company_id1 == RouteUtil.JR_CENTRAL || company_id1 == RouteUtil.JR_WEST)
{
company_id1 = RouteUtil.JR_EAST	/* 本州三社としてJR東海、JR西は、JR東とする */
}
if (company_id2 == RouteUtil.JR_CENTRAL || company_id2 == RouteUtil.JR_WEST)
{
company_id2 = RouteUtil.JR_EAST	/* 本州三社としてJR東海、JR西は、JR東とする */
}

this.totalSalesKm += dex[0]			// total 営業キロ(会社線含む、有効日数計算用)
if (dex[8] !== 0)
{	/* 会社線 */

var comfare:CompanyFare? = null

if (0 < station_id_0)
{
 /* 2回以上連続で会社線の場合(タンゴ鉄道とか) */
                            /* a+++b+++c : */
                            comfare = Fare_company(station_id_0.toInt(), ri.stationId.toInt())
if (comfare == null)
{
 /* 乗継割引なし */
                                comfare = Fare_company(station_id1.toInt(), ri.stationId.toInt())
if (comfare == null)
{
RouteUtil.ASSERT(false)
}
station_id_0 = station_id1
}
else
{
var comfare_1:CompanyFare? = null	// a+++b

 // if ex. 氷見-金沢 併算割引非適用
    							if (comfare.passflg && 1 shl RouteList.BLF_COMPNEND or (1 shl RouteList.BLF_COMPNBEGIN) != last_flag and (1 shl RouteList.BLF_COMPNEND or (1 shl RouteList.BLF_COMPNBEGIN)))
{
 /* 乗継割引なし */
    								comfare = Fare_company(station_id1.toInt(), ri.stationId.toInt())
if (comfare == null)
{
RouteUtil.ASSERT(false)
}
}
else
{
 /* normal or 併算割引適用 */
    								comfare_1 = Fare_company(station_id_0.toInt(), station_id1.toInt())
if (comfare_1 == null)
{
RouteUtil.ASSERT(false)
}
else
{
if (0 < comfare_1.fareAcademic)
{
 /* 学割ありの場合は割引額を戻す */
        									RouteUtil.ASSERT(comfare_1.fareAcademic < comfare_1.fare)
this.company_fare_ac_discount -= comfare_1.fare - comfare_1.fareAcademic
}
this.fareForCompanyline -= comfare_1.fare	// 戻す(直前加算分を取消)
this.company_fare_child -= comfare_1.fareChild
}
}
}
if (0 < comfare.fareAcademic)
{
 /* 学割ありの場合は割引額を加算 */
                                RouteUtil.ASSERT(comfare.fareAcademic < comfare.fare)
this.company_fare_ac_discount += comfare.fare - comfare.fareAcademic
}
this.fareForCompanyline += comfare.fare
this.company_fare_child += comfare.fareChild

}
else
{
 /* 1回目の会社線 */
                            if (this.totalSalesKm == dex[0])
{
result_flag = RouteUtil.BIT_ON(result_flag, BRF_COMAPANY_FIRST)
}
else if (0 < this.fareForCompanyline && !RouteUtil.BIT_CHK(result_flag, BRF_COMAPANY_END))
{
 /* 会社線 2回以上通過 */
                                result_flag = RouteUtil.BIT_ON(result_flag, BRF_COMPANY_INCORRECT)
RouteUtil.ASSERT(false)
}
comfare = Fare_company(station_id1.toInt(), ri.stationId.toInt())
if (comfare == null)
{
RouteUtil.ASSERT(false)
}
else
{
if (0 < comfare.fareAcademic)
{
 /* 学割ありの場合は割引額を加算 */
                                    RouteUtil.ASSERT(comfare.fareAcademic < comfare.fare)
this.company_fare_ac_discount += comfare.fare - comfare.fareAcademic
}
this.fareForCompanyline += comfare.fare
this.company_fare_child += comfare.fareChild
}
station_id_0 = station_id1

result_flag = RouteUtil.BIT_ON(result_flag, BRF_COMAPANY_END)
}
}
else
{
 /* JR線 */
                        station_id_0 = 0

result_flag = RouteUtil.BIT_OFF(result_flag, BRF_COMAPANY_END)

 /* JR Group */
                        if (RouteUtil.IS_SHINKANSEN_LINE(ri.lineId.toInt()))
{
 /* 山陽新幹線 広島-徳山間を通る場合の1経路では、fare(l, i) とされてしまうけんのぉ
                             */
                            this.local_only = false		// 幹線
}

if (this.aggregate_fare_jr(company_id1, company_id2, dex) < 0)
{
return -1	// error >>>>>>>>>>>>>>>>>>>>>>>>>>>
}
}
if (this.flag and RouteUtil.FLAG_FARECALC_INITIAL == 0)
{ // b15が0の場合最初なので駅1のフラグも反映
 // ビット13のみ保持(既にflagのビット13のみにはcalc_fare()の最初でセットしているから)
                        this.flag = this.flag and (1 shl RouteUtil.BCBULURB)
 // 次回以降から駅1不要、駅1 sflgの下12ビット,
                        // bit12以上はGetDistanceEx()のクエリでOffしているので不要
                        this.flag = this.flag or (RouteUtil.FLAG_FARECALC_INITIAL or dex[6])
}
flag = RouteUtil.FLAG_FARECALC_INITIAL or (1 shl RouteUtil.BCBULURB) or dex[7]
if (flag and RouteUtil.MASK_URBAN != this.flag and RouteUtil.MASK_URBAN)
{/* 近郊区間(b7-9) の比較 */
flag = flag and RouteUtil.MASK_URBAN.inv()				/* 近郊区間 OFF */
}
this.flag = this.flag and flag	/* b11,10,5(大阪/東京電車特定区間, 山手線／大阪環状線内) */
 /* ~(反転）不要 */
                    /* flag(sflg)は、b11,10,5, 7-9 のみ使用で他はFARE_INFOでは使用しない */

                    // 特別加算区間
                    fare_add += CheckSpecficFarePass(ri.lineId.toInt(), station_id1.toInt(), ri.stationId.toInt())
}
}
station_id1 = ri.stationId
}
return fare_add
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
    private fun aggregate_fare_jr(company_id1:Int, company_id2:Int, distance:Array<Int>):Int {
if (company_id1 == company_id2)
{		// 同一 1社
 /* 単一 JR Group */
            when (company_id1) {
RouteUtil.JR_HOKKAIDO -> {
this.salesKmForHokkaido += distance[0]
if (0 == distance[1])
{ 		/* 幹線? */
this.calcKmForHokkaido += distance[0]
this.local_only_as_hokkaido = false
this.local_only = false	// 幹線
}
else
{
this.calcKmForHokkaido += distance[1]
}
}
RouteUtil.JR_SHIKOKU -> {
this.salesKmForShikoku += distance[0]
if (0 == distance[1])
{ 		/* 幹線? */
this.calcKmForShikoku += distance[0]
this.local_only = false	// 幹線
}
else
{
this.calcKmForShikoku += distance[1]
}
}
RouteUtil.JR_KYUSYU -> {
this.salesKmForKyusyu += distance[0]
if (0 == distance[1])
{ 		/* 幹線? */
this.calcKmForKyusyu += distance[0]
this.local_only = false	// 幹線
}
else
{
this.calcKmForKyusyu += distance[1]
}
}
RouteUtil.JR_EAST, RouteUtil.JR_CENTRAL, RouteUtil.JR_WEST -> {
this.base_sales_km += distance[0]
if (0 == distance[1])
{ 		/* 幹線? */
this.base_calc_km += distance[0]
this.local_only = false	// 幹線
}
else
{
this.base_calc_km += distance[1]
}
}
else -> RouteUtil.ASSERT(false)	/* あり得ない */
}
}
else
{								// 会社跨り?
when (company_id1) {
RouteUtil.JR_HOKKAIDO					// JR北海道->本州3社(JR東)
 -> {
this.salesKmForHokkaido += distance[2]
this.base_sales_km += distance[0] - distance[2]
if (0 == distance[1])
{ /* 幹線? */
this.calcKmForHokkaido += distance[2]
this.base_calc_km += distance[0] - distance[2]
this.local_only_as_hokkaido = false
}
else
{
this.calcKmForHokkaido += distance[3]
this.base_calc_km += distance[1] - distance[3]
}
}
RouteUtil.JR_SHIKOKU					// JR四国->本州3社(JR西)
 -> {
this.salesKmForShikoku += distance[2]
this.base_sales_km += distance[0] - distance[2]
if (0 == distance[1])
{ 			/* 幹線? */
this.calcKmForShikoku += distance[2]
this.base_calc_km += distance[0] - distance[2]
this.local_only = false	// 幹線
}
else
{
this.calcKmForShikoku += distance[3]
this.base_calc_km += distance[1] - distance[3]
}
}
RouteUtil.JR_KYUSYU					// JR九州->本州3社(JR西)
 -> {
this.salesKmForKyusyu += distance[2]
this.base_sales_km += distance[0] - distance[2]
if (0 == distance[1])
{ /* 幹線? */
this.calcKmForKyusyu += distance[2]
this.base_calc_km += distance[0] - distance[2]
this.local_only = false	// 幹線
}
else
{
this.calcKmForKyusyu += distance[3]
this.base_calc_km += distance[1] - distance[3]
}
}
else -> {
this.base_sales_km += distance[2]
if (0 == distance[1])
{ /* 幹線? */
this.base_calc_km += distance[2]
this.local_only = false	// 幹線
}
else
{
this.base_calc_km += distance[3]
}
when (company_id2) {
RouteUtil.JR_HOKKAIDO			// 本州3社(JR東)->JR北海道
 -> {
this.salesKmForHokkaido += distance[0] - distance[2]
if (0 == distance[1])
{ /* 幹線? */
this.calcKmForHokkaido += distance[0] - distance[2]
this.local_only_as_hokkaido = false
}
else
{
this.calcKmForHokkaido += distance[1] - distance[3]
}
}
RouteUtil.JR_SHIKOKU			// 本州3社(JR西)->JR四国
 -> {
this.salesKmForShikoku += distance[0] - distance[2]
if (0 == distance[1])
{ /* 幹線? */
this.calcKmForShikoku += distance[0] - distance[2]
}
else
{
this.calcKmForShikoku += distance[1] - distance[3]
}
}
RouteUtil.JR_KYUSYU				// 本州3社(JR西)->JR九州
 -> {
this.salesKmForKyusyu += distance[0] - distance[2]
if (0 == distance[1])
{ /* 幹線? */
this.calcKmForKyusyu += distance[0] - distance[2]
}
else
{
this.calcKmForKyusyu += distance[1] - distance[3]
}
}
else -> {
RouteUtil.ASSERT(false)
return -1	/* failure abort end. >>>>>>>>> */
}
}//break;	/* あり得ない */
}
} /* JR 2company */
} /* JR Group */
return 0
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
    internal fun calc_fare(last_flag:Int, routeList_raw:List<RouteItem>, routeList_cooked:List<RouteItem>?):Boolean {
val fare_add:Int		/* 特別加算区間 */
val adjust_km:Int
val routeList = routeList_cooked ?: routeList_raw

reset()

if ((fare_add = aggregate_fare_info(last_flag, routeList_raw, routeList_cooked)) < 0)
{
RouteUtil.ASSERT(false)
reset()
result_flag = RouteUtil.BIT_ON(result_flag, BRF_FATAL_ERROR)
return false
 //goto err;		/* >>>>>>>>>>>>>>>>>>> */
        }

 /* 旅客営業取扱基準規定43条の2（小倉、西小倉廻り） */
        if (routeList_cooked != null)
{
adjust_km = CheckAndApplyRule43_2j(routeList_cooked.toTypedArray<RouteItem>())
this.totalSalesKm -= adjust_km * 2
this.base_sales_km -= adjust_km
this.base_calc_km -= adjust_km
this.salesKmForKyusyu -= adjust_km
this.calcKmForKyusyu -= adjust_km
}
 /* 乗車券の有効日数 */
        this.ticketAvailDays = days_ticket(this.totalSalesKm)

 /* 旅客営業規則89条適用 */
        if (routeList_cooked != null)
{
this.base_calc_km += CheckOfRule89j(routeList_cooked)
}
 /* 幹線のみ ? */
        this.major_only = this.base_sales_km == this.base_calc_km &&
this.salesKmForKyusyu == this.calcKmForKyusyu &&
this.salesKmForHokkaido == this.calcKmForHokkaido &&
this.salesKmForShikoku == this.calcKmForShikoku

 /* 運賃計算 */
    	if (retr_fare())
{
val special_fare:Int

if (RouteUtil.BIT_CHK(last_flag, RouteList.BLF_COMPNCHECK) && this.totalSalesKm < 1500)
{
 /* 大聖寺-和倉温泉は106kmある */
    										/* 大聖寺-米原-岐阜-富山-津端-和倉温泉 を弾く為 */
    	        	System.out.printf("specific fare section replace for IR-ishikawa change continue discount\n")
special_fare = SpecificFareLine(routeList[0].stationId.toInt(), routeList[routeList.size - 1].stationId.toInt(), 2)
if (0 < special_fare)
{
this.fareForJR = special_fare - this.fareForCompanyline	/* IRいしかわ 乗継割引 */
}
}
else if (RouteUtil.MASK_URBAN and this.flag != 0 || this.totalSalesKm < 500)
{
special_fare = SpecificFareLine(routeList[0].stationId.toInt(), routeList[routeList.size - 1].stationId.toInt(), 1)
if (0 < special_fare)
{
System.out.printf("specific fare section replace for Metro or Shikoku-Big-bridge\n")

 // 品川-青森-横浜 なども適用されてはいけないので,近郊区間内なら適用するように。
    				// 品川-横浜などの特別区間は近郊区間内の場合遠回り指定でも特別運賃を表示
    				// 名古屋は近郊区間でないので距離(尾頭橋-岡崎 37.7km 名古屋-岡崎 40.1km)50km以下として条件に含める
    				// またIRいしかわの乗継割引区間も同様50km以下が条件

    		        if (RouteUtil.URB_TOKYO == RouteUtil.URBAN_ID(this.flag))
{      /* 東京、新潟、仙台 近郊区間(最短距離で算出可能) */
 /* 新幹線乗車も特別運賃適用 */

    		            if (companymask and (1 shl RouteUtil.JR_CENTRAL - 1) != 0)
{
 /* 新幹線乗車 はIC運賃適用しないが大都市特例運賃は適用 */
    		                RouteUtil.ASSERT(0 == companymask and (1 shl RouteUtil.JR_EAST - 1 or (1 shl RouteUtil.JR_CENTRAL - 1)).inv())
}
else
{
RouteUtil.ASSERT(companymask == 1 shl RouteUtil.JR_EAST - 1)  /* JR East only  */
this.fare_ic = special_fare
}
this.fareForJR = RouteUtil.round_up(special_fare)	/* 大都市特定区間運賃(東京)(\10単位切り上げ) */
}
else
{
this.fareForJR = special_fare	/* 大都市特定区間運賃(大阪、名古屋) */
}
}
RouteUtil.ASSERT(this.fareForCompanyline == 0)	// 会社線は通っていない
}

 // 特別加算区間分
            this.fareForJR += fare_add

if (isUrbanArea)
{
this.ticketAvailDays = 1	/* 当日限り */
}

if (6000 < jrCalcKm)
{	/* 往復割引 */
this.isRoundTripDiscount = true
}
else
{
this.isRoundTripDiscount = false
}
return true
}
else
{
 /* 会社線のみ */
            //this.reset();
            this.ticketAvailDays = 1
this.isRoundTripDiscount = false
this.companymask = this.companymask and RouteUtil.JR_GROUP_MASK.inv()
return true
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
    internal fun setRoute(begin_station_id:Int, end_station_id:Int,
routeList:List<RouteItem>,
last_flag:Int) {
beginTerminalId = begin_station_id
endTerminalId = end_station_id

route_string = RouteUtil.Show_route(routeList.toTypedArray<RouteItem>(), last_flag)
}


 //public:
     fun reset() {				//***
companymask = 0
totalSalesKm = 0

base_sales_km = 0
base_calc_km = 0

salesKmForKyusyu = 0
calcKmForKyusyu = 0

salesKmForHokkaido = 0
calcKmForHokkaido = 0

salesKmForShikoku = 0
calcKmForShikoku = 0

local_only = false
local_only_as_hokkaido = false

major_only = false
jrSalesKm = 0
jrCalcKm = 0

fareForCompanyline = 0
company_fare_ac_discount = 0
company_fare_child = 0
flag = 0
fareForJR = 0
fare_ic = 0
ticketAvailDays = 0

rule114_fare = 0
rule114SalesKm = 0
rule114CalcKm = 0

isRoundTripDiscount = false

beginTerminalId = 0
endTerminalId = 0

result_flag = 0

route_string = ""
}

internal inner class FareResult {
 var fare:Int = 0
 var isDiscount:Boolean = false
 constructor() {
fare = 0
isDiscount = false
}
 constructor(fare_:Int, isDiscount_:Boolean) {
fare = fare_
isDiscount = isDiscount_
}
}
internal fun setEmpty() {
result_flag = RouteUtil.BIT_ON(result_flag, BRF_ROUTE_EMPTY)
}
internal fun setInComplete() {
result_flag = RouteUtil.BIT_ON(result_flag, BRF_ROUTE_INCOMPLETE)
}
internal val isMultiCompanyLine:Boolean
get() =RouteUtil.BIT_CHK(result_flag, BRF_COMPANY_INCORRECT)
internal val isBeginEndCompanyLine:Boolean
get() =result_flag and (1 shl BRF_COMAPANY_FIRST or (1 shl BRF_COMAPANY_END)) != 0
internal fun resultCode():Int {
if (RouteUtil.BIT_CHK(result_flag, BRF_ROUTE_INCOMPLETE))
{
return -1
}
else if (RouteUtil.BIT_CHK(result_flag, BRF_ROUTE_EMPTY))
{
return -2
}
else if (RouteUtil.BIT_CHK(result_flag, BRF_FATAL_ERROR))
{
return -3
}
else
{
return 0
}
}

/**	往復運賃を返す(会社線含む総額)(JR分は601km以上で1割引)

     	 * @param discount [out]  true=割引あり
      * *
 * @retval [円]
 */
    internal fun roundTripFareWithCompanyLine():FareResult {
val fareW = FareResult()

if (6000 < jrCalcKm)
{	/* 往復割引 */
fareW.fare = fare_discount(fareForJR, 1) * 2 + fareForCompanyline * 2
fareW.isDiscount = true
RouteUtil.ASSERT(this.isRoundTripDiscount == true)
}
else
{
fareW.isDiscount = false
fareW.fare = jrFare() * 2 + fareForCompanyline * 2
RouteUtil.ASSERT(this.isRoundTripDiscount == false)
}
return fareW
}

/**	114条適用前の往復運賃を返す(会社線含む総額)(JR分は601km以上で1割引)

     	 * @retval [円]
 */
    internal fun roundTripFareWithCompanyLinePriorRule114():Int {
val fareW:Int

if (6000 < jrCalcKm)
{	/* 往復割引 */
RouteUtil.ASSERT(false)
}
if (!isRule114)
{
RouteUtil.ASSERT(false)
}
fareW = fareForJR * 2 + fareForCompanyline * 2
return fareW
}

/**	小児往復運賃を返す(会社線含む総額)(JR分は601km以上で1割引)

     	 * @paramm discount [out]  true=割引あり
      * *
 * @retval [円]
 */
    internal fun roundTripChildFareWithCompanyLine():Int {
val fareW:Int

if (6000 < jrCalcKm)
{	/* 往復割引 */
fareW = fare_discount(fare_discount(fareForJR, 5), 1) * 2 + company_fare_child * 2
}
else
{
fareW = fare_discount(jrFare(), 5) * 2 + company_fare_child * 2
}
return fareW
}

/**	近郊区間内かを返す(有効日数を1にする)

     	 * @retval	true 近郊区間
 */
    internal val isUrbanArea:Boolean
get() {
if (RouteUtil.MASK_URBAN and flag != 0 && 1 shl RouteUtil.BCBULURB and flag == 0)
{

RouteUtil.ASSERT(IsIC_area(RouteUtil.URBAN_ID(flag)) && fare_ic != 0 || !IsIC_area(RouteUtil.URBAN_ID(flag)) && fare_ic == 0)

return true
}
else
{
return false
}
}

/**	会社線の営業キロ総数を返す

     	 * @retval 営業キロ
 */
    internal val companySalesKm:Int
get() =totalSalesKm - jrSalesKm

/**	JR線＋会社線の小児運賃額を返す

     	 * @retval	[円]
 */
    internal val childFareForDisplay:Int
get() =company_fare_child + fare_discount(jrFare(), 5)

/**	株主優待割引有効数を返す

     	 * @param index      [in]  0から1 JR東日本のみ 0は2割引、1は4割引を返す
      * *
 * @param idCompany [out]  0:JR東海1割/1:JR西日本5割/2:JR東日本2割/3:JR東日本4割
      * *
 * @retval	[円](無割引、無効は0)
 */
    internal fun countOfFareStockDiscount():Int {
 // 通過連絡運輸も株優は有効らしい

        when (stockDiscountCompany) {
RouteUtil.JR_EAST -> return 2
RouteUtil.JR_CENTRAL, RouteUtil.JR_WEST, RouteUtil.JR_KYUSYU -> return 1
else -> {}
}
return 0
}
@JvmOverloads internal fun getFareStockDiscount(index:Int, applied_r114:Boolean = false):Int {
val cfare:Int
var result = 0

if (applied_r114)
{
if (isRule114)
{
cfare = rule114_fare
}
else
{
RouteUtil.ASSERT(false)
return 0		// >>>>>>>>>
}
}
else
{
cfare = fareForJR
}

val companyno = stockDiscountCompany
when (companyno) {
RouteUtil.JR_EAST -> if (index == 0)
{
result = fare_discount(cfare, 2)
}
else if (index == 1)
{
 /* JR東4割(2枚使用) */
                    result = fare_discount(cfare, 4)
}
else
{
result = 0    // wrong index
}
RouteUtil.JR_WEST, RouteUtil.JR_KYUSYU -> if (index == 0)
{
result = fare_discount(cfare, 5)
}
RouteUtil.JR_CENTRAL -> if (index == 0)
{
result = fare_discount(cfare, 1)
}
else -> {}
}
return result

 // 通過連絡運輸も株優は有効らしい
    }


internal fun getTitleFareStockDiscount(index:Int):String {
val titles = arrayOf("JR東日本 株主優待2割", // 0
    		"JR東日本 株主優待4割", // 1
    		"JR西日本 株主優待5割", // 2
    		"JR東海   株主優待1割", // 3
            "JR九州   株主優待5割")// 4
var sindex = -1
val stockno = stockDiscountCompany
when (stockno) {
RouteUtil.JR_EAST -> if (index == 0)
{
sindex = 0
}
else if (index == 1)
{
 /* JR東4割(2枚使用) */
                    sindex = 1
}
else
{
 //return "";    // wrong index
                    RouteUtil.ASSERT(false)
}
RouteUtil.JR_WEST -> if (index == 0)
{
sindex = 2
}
else
{
 //return "";    // wrong index
                    RouteUtil.ASSERT(false)
}
RouteUtil.JR_KYUSYU -> if (index == 0)
{
sindex = 4
}
else
{
 //return "";    // wrong index
                    RouteUtil.ASSERT(false)
}
RouteUtil.JR_CENTRAL -> if (index == 0)
{
sindex = 3
}
else
{
 //return "";    // wrong index
                    RouteUtil.ASSERT(false)
}
else -> {}
}
if (0 <= sindex)
{
return titles[sindex]
}
else
{
return ""
}
 // 通過連絡運輸も株優は有効らしい
    }

 //  株主優待可否／種別を返す
    //  @retval RouteUtil.JR_EAST = JR東日本
    //  @retval RouteUtil.JR_WEST = JR西日本
    //  @retval RouteUtil.JR_CENTRAL = JR東海
    //  @retval RouteUtil.JR_KYUSYU = JR九州
    //  @retval 0 = 無効
    //
    internal/* 都区内または横浜市内が適用されていたらJR東海ではない*//* CheckOfRule86() */ val stockDiscountCompany:Int
get() {
if (RouteUtil.JR_GROUP_MASK and companymask == 1 shl RouteUtil.JR_EAST - 1)
{
return RouteUtil.JR_EAST
}
if (RouteUtil.JR_GROUP_MASK and companymask == 1 shl RouteUtil.JR_WEST - 1)
{
return RouteUtil.JR_WEST
}
if (RouteUtil.JR_GROUP_MASK and companymask == 1 shl RouteUtil.JR_KYUSYU - 1)
{
return RouteUtil.JR_KYUSYU
}
if (RouteUtil.JR_GROUP_MASK and companymask == 1 shl RouteUtil.JR_CENTRAL - 1)
{
if (RouteUtil.STATION_ID_AS_CITYNO < beginTerminalId && RouteUtil.CITYNO_NAGOYA != beginTerminalId - RouteUtil.STATION_ID_AS_CITYNO || RouteUtil.STATION_ID_AS_CITYNO < endTerminalId && RouteUtil.CITYNO_NAGOYA != endTerminalId - RouteUtil.STATION_ID_AS_CITYNO)
{
return 0
}
else
{
return RouteUtil.JR_CENTRAL
}
}
return 0
}


/**	学割運賃を返す(会社線+JR線=全線)

     	 * @retval	学割運賃[円]
      * *
 * @retval 0 非適用
 */
    internal/* 学割非適用 */ val academicDiscountFare:Int
get() {
val result_fare:Int

if (1000 < jrSalesKm || 0 < company_fare_ac_discount)
{
if (1000 < jrSalesKm)
{
result_fare = fare_discount(jrFare(), 2)
}
else
{
result_fare = jrFare()
}
return result_fare + (fareForCompanyline - company_fare_ac_discount)
}
else
{
return 0
}
}

/**	学割往復運賃を返す(会社線含む総額)(JR分は601km以上で1割引)

     	 * @retval [円]
 */
    internal fun roundTripAcademicFareWithCompanyLine():Int {
var fareW:Int

 // JR

        if (6000 < jrCalcKm)
{	/* 往復割引かつ学割 */
fareW = fare_discount(fare_discount(fareForJR, 1), 2)
RouteUtil.ASSERT(this.isRoundTripDiscount == true)
}
else
{
fareW = jrFare()
if (1000 < jrSalesKm)
{
 // Academic discount
                RouteUtil.ASSERT(this.isRoundTripDiscount == false)
fareW = fare_discount(fareW, 2)
}
}

 // company

        fareW += fareForCompanyline - company_fare_ac_discount
return fareW * 2
}

/**	JR線＋会社線の運賃額を返す

     	 * @retval	[円]
 */
    internal val fareForDisplay:Int
get() =fareForCompanyline + jrFare()

/**	JR線＋会社線の運賃額を返す(114条非適用運賃)

     	 * @retval	[円]
 */
    internal//      RouteUtil.ASSERT (false);
 val fareForDisplayPriorRule114:Int
get() {
if (isRule114)
{
return fareForCompanyline + fareForJR
}
else
{
return 0
}
}

/**	IC運賃を返す

     	 * @retval IC運賃(x10[円])
 */
    internal val fareForIC:Int
get() {
RouteUtil.ASSERT(fare_ic != 0 && companymask == 1 shl RouteUtil.JR_EAST - 1 || fare_ic == 0)
return fare_ic
}

internal class Fare {
 var fare:Int = 0
 var sales_km:Int = 0
 var calc_km:Int = 0
 constructor() {
set(0, 0, 0)
}
 constructor(f:Int, sk:Int, ck:Int) {
set(f, sk, ck)
}
 operator fun set(f:Int, sk:Int, ck:Int) {
fare = f
sales_km = sk
calc_km = ck
}
}

internal fun getRule114(fare:Fare):Boolean {
fare.fare = rule114_fare
fare.sales_km = rule114SalesKm
fare.calc_km = rule114CalcKm
return rule114_fare != 0
}

internal fun setRule114(fare:Fare) {
rule114_fare = fare.fare
rule114SalesKm = fare.sales_km
rule114CalcKm = fare.calc_km
}
internal fun clrRule114() {
rule114_fare = 0
rule114SalesKm = 0
rule114CalcKm = 0
}

internal val isRule114:Boolean
get() =0 != rule114_fare

/**	JR線の運賃額を返す(含114条適用／非適用判定)

     	 * @retval	[円]
 */
    private fun jrFare():Int {
if (isRule114)
{
return rule114_fare
}
else
{
return fareForJR
}
}

 // 営業キロから有効日数を得る
    //	calc_fare() =>
    //
    //	@param [in] km    営業キロ
    //	@return 有効日数
    //
    private fun days_ticket(sales_km:Int):Int {
if (sales_km < 1001)
{
return 1
}
return (sales_km + 1999) / 2000 + 1
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
    private fun getDistanceEx(line_id:Int, station_id1:Int, station_id2:Int):Array<Int> {
val dbid = DbidOf()
val result = arrayOfNulls<Int>(9)

val ctx = RouteDB.db().rawQuery(
"select" +
"	(select max(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))-" +
"	(select min(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))," +	// [0]

"	(select max(calc_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))-" +
"	(select min(calc_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))," +	// [1]

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

"	(select company_id from t_station where rowid=?2)," +                                               // [4]

"   (select company_id from t_station where rowid=?3)," +                                               // [5]

"	(select sflg&4095 from t_station where rowid=?2)," +                                                // [6]

"   (select sflg&4095 from t_station where rowid=?3)," +                                         		// [7]

"	(select (1&(lflg>>18)) from t_lines where line_id=?1 and station_id=?3)"                            // [8]
, arrayOf(line_id.toString(), station_id1.toString(), station_id2.toString()))
try
{
 //	2147483648 = 0x80000000
            if (ctx.moveToNext())
{
result[0] = ctx.getInt(0)	// sales_km
result[1] = ctx.getInt(1)	// calc_km
result[2] = ctx.getInt(2)	// sales_km for in company as station_id1
result[3] = ctx.getInt(3)	// calc_km  for in company as station_id1
result[4] = ctx.getInt(4)	//
result[5] = ctx.getInt(5)	//
result[6] = ctx.getInt(6)	//
result[7] = ctx.getInt(7)	//
result[8] = ctx.getInt(8)	//

if (line_id == dbid.LineIdOf_HAKATAMINAMISEN || line_id == dbid.LineIdOf_SANYOSHINKANSEN)
{ //山陽新幹線、博多南線はJ九州内でもJR西日本
result[4] = RouteUtil.JR_WEST
result[5] = RouteUtil.JR_WEST
}
else if (line_id == dbid.LineIdOf_TOKAIDOSHINKANSEN)
{
if (result[4] === RouteUtil.JR_EAST)
{
result[4] = RouteUtil.JR_CENTRAL
}
if (result[5] === RouteUtil.JR_EAST)
{
result[5] = RouteUtil.JR_CENTRAL
}
if (result[2] === -1)
{
result[4] = result[5]
}
if (result[3] === -1)
{
result[5] = result[4]
}
}
else if (station_id1 == dbid.StationIdOf_MAIBARA && result[5] === RouteUtil.JR_EAST)
{  // 3社は北陸線、東海道線
 // 米原 - JR東日本
                    if (line_id == dbid.LineIdOf_TOKAIDO)
{
result[4] = RouteUtil.JR_CENTRAL
result[5] = RouteUtil.JR_EAST
}
else
{  /* 北陸線 (東海道新幹線は前で引っ掛けてるので有りえない)*/
result[4] = RouteUtil.JR_WEST
result[5] = RouteUtil.JR_EAST
}
}
else if (station_id2 == dbid.StationIdOf_MAIBARA && result[4] === RouteUtil.JR_EAST)
{
 // JR東日本 ｰ 米原
                    if (line_id == dbid.LineIdOf_TOKAIDO)
{
result[4] = RouteUtil.JR_EAST
result[5] = RouteUtil.JR_CENTRAL
}
else
{
result[4] = RouteUtil.JR_EAST  /* 北陸線 */
result[5] = RouteUtil.JR_WEST  /* 北陸線 */
}
}
else if (line_id == RouteUtil.LINE_HOKKAIDO_SINKANSEN)
{
result[4] = RouteUtil.JR_HOKKAIDO
result[5] = RouteUtil.JR_HOKKAIDO
}
else
{
if (result[2] === -1)
{		/* station_id1が境界駅の場合 */
System.out.printf("multicompany line detect 1: %d, %d(com1 <- com2)\n", result[2], result[3])
result[4] = result[5]
}
if (result[3] === -1)
{		/* ite.stationId(station_id2)が境界駅の場合 */
System.out.printf("multicompany line detect 2: %d, %d(com2 <- com1)\n", result[2], result[3])
result[5] = result[4]
}
}
}
}

finally
{
ctx.close()
}
return result
}



internal val resultCode:Int
get() {
if (RouteUtil.BIT_CHK(result_flag, BRF_ROUTE_INCOMPLETE))
{
return -1
}
else
{
return 0
}
}

companion object {
 /* result_flag bit */
    internal val BRF_COMAPANY_FIRST = 0	/* 会社線から開始 */
internal val BRF_COMAPANY_END = 1		/* 会社線で終了 */
 /* 通常OFF-OFF, ON-ONは会社線のみ */
    internal val BRF_COMPANY_INCORRECT = 2/* 会社線2社以上通過 */
internal val BRF_ROUTE_INCOMPLETE = 3	/* 不完全経路(BSRNOTYET_NA) */
internal val BRF_ROUTE_EMPTY = 4       /* empty */
internal val BRF_FATAL_ERROR = 5       /* fatal error in aggregate_fare_info() */

 /* discount */
    internal fun fare_discount(fare:Int, per:Int):Int {
return fare / 10 * (10 - per) / 10 * 10
}
 /* discount 5円の端数切上 */
    internal fun fare_discount5(fare:Int, per:Int):Int {
return (fare / 10 * (10 - per) + 5) / 10 * 10
}
internal fun IsCityId(id:Int):Boolean {
return RouteUtil.STATION_ID_AS_CITYNO <= id
}

 // static
    //		@brief 70条通過の営業キロを得る
    //
    //	@param [in] station_id1		駅1
    //	@param [in] station_id2		駅2
    //
    //	@return station_id1, station_id2間の営業キロ
    //
    internal fun Retrieve70Distance(station_id1:Int, station_id2:Int):Int {
val tsql = "select sales_km from t_rule70" +
" where " +
" ((station_id1=?1 and station_id2=?2) or" +
"  (station_id1=?2 and station_id2=?1))"
var rc = 0
val dbo = RouteDB.db().rawQuery(tsql, arrayOf(station_id1.toString(), station_id2.toString()))
try
{
if (dbo.moveToNext())
{
rc = dbo.getInt(0)
}
}

finally
{
dbo.close()
}
return rc
}


 //	運賃計算(基本)
    //	calc_fare() => retr_fare() =>
    //
    //	@param [in] km    計算キロ
    //	@return 運賃額
    //
    private fun Fare_basic_f(km:Int):Int {
var fare:Int
val c_km:Int

if (km < 31)
{							// 1 to 3km
if (RouteDB.instance.tax() == 5)
{
return 140
}
else
{
return 144
}
}
if (km < 61)
{							// 4 to 6km
if (RouteDB.instance.tax() == 5)
{
return 180
}
else
{
return 185
}
}
if (km < 101)
{							// 7 to 10km
if (RouteDB.instance.tax() == 5)
{
return 190
}
else
{
return 195
}
}
if (6000 < km)
{						// 600km越えは40キロ刻み
c_km = (km - 1) / 400 * 400 + 200
}
else if (1000 < km)
{					// 100.1-600kmは20キロ刻み
c_km = (km - 1) / 200 * 200 + 100
}
else if (500 < km)
{					// 50.1-100kmは10キロ刻み
c_km = (km - 1) / 100 * 100 + 50
}
else if (100 < km)
{					// 10.1-50kmは5キロ刻み
c_km = (km - 1) / 50 * 50 + 30
}
else
{
RouteUtil.ASSERT(false)
c_km = 0
}
if (6000 <= c_km)
{
fare = 1620 * 3000 + 1285 * (6000 - 3000) + 705 * (c_km - 6000)
}
else if (3000 < c_km)
{
fare = 1620 * 3000 + 1285 * (c_km - 3000)
}
else
{
fare = 1620 * c_km
}
if (c_km <= 1000)
{						// 100km以下は切り上げ
 // 1の位を切り上げ
            fare = (fare + 9999) / 10000 * 10
}
else
{								// 100㎞越えは四捨五入
fare = (fare + 50000) / 100000 * 100
}
return RouteUtil.taxadd_ic(fare, RouteDB.instance.tax())	// tax = +5%, 四捨五入
}

 //	sub: 地方交通線
    //	calc_fare() => retr_fare() =>
    //
    //	@param [in] km    営業キロ
    //	@return 運賃額
    //
    private fun Fare_sub_f(km:Int):Int {
var fare:Int
var c_km:Int

if (km < 31)
{							// 1 to 3km
if (RouteDB.instance.tax() == 5)
{
return 140
}
else
{
return 144
}
}
if (km < 61)
{							// 4 to 6km
if (RouteDB.instance.tax() == 5)
{
return 180
}
else
{
return 185
}
}
if (km < 101)
{							// 7 to 10km
if (RouteDB.instance.tax() == 5)
{
return 200
}
else
{
return 206
}
}

if (12000 < km)
{		// 1200km越えは別表第2号イの4にない
RouteUtil.ASSERT(false)
return -1
}

c_km = Fare_table("lspekm", 'e', km)
if (c_km == 0)
{
RouteUtil.ASSERT(false)
return -1
}
if (c_km < 0)
{
return -c_km		/* fare 第77条の5の3*/
}

 /* c_km : 第77条の5の2 */
        c_km *= 10

 /* 第77条の5 */
        if (5460 <= c_km)
{
fare = 1780 * 2730 + 1410 * (5460 - 2730) + 770 * (c_km - 5460)
}
else if (2730 < c_km)
{
fare = 1780 * 2730 + 1410 * (c_km - 2730)
}
else
{
fare = 1780 * c_km
}
if (c_km <= 1000)
{						// 100km以下は切り上げ
 // 1の位を切り上げ
            fare = (fare + 9999) / 10000 * 10
}
else
{				// 100㎞越えは四捨五入
fare = (fare + 50000) / 100000 * 100
}
return RouteUtil.taxadd_ic(fare, RouteDB.instance.tax())
}

 //	b: 電車特定区間(東京)
    //	calc_fare() => retr_fare() =>
    //
    //	@param [in] km    営業キロ
    //	@return 運賃額
    //
    private fun Fare_tokyo_f(km:Int):Int {
var fare:Int
val c_km:Int

if (km < 31)
{							// 1 to 3km
if (RouteDB.instance.tax() == 5)
{
return 130
}
else
{
return 133
}
}
if (km < 61)
{							// 4 to 6km
if (RouteDB.instance.tax() == 5)
{
return 150
}
else
{
return 154
}
}
if (km < 101)
{							// 7 to 10km
if (RouteDB.instance.tax() == 5)
{
return 160
}
else
{
return 165
}
}

if (6000 < km)
{						// 600km越えは40キロ刻み
RouteUtil.ASSERT(false)
return -1
}
else if (1000 < km)
{					// 100.1-600kmは20キロ刻み
c_km = (km - 1) / 200 * 200 + 100
}
else if (500 < km)
{					// 50.1-100kmは10キロ刻み
c_km = (km - 1) / 100 * 100 + 50
}
else if (100 < km)
{					// 10.1-50kmは5キロ刻み
c_km = (km - 1) / 50 * 50 + 30
}
else
{
RouteUtil.ASSERT(false)
c_km = 0
}
if (3000 < c_km)
{
fare = 1530 * 3000 + 1215 * (c_km - 3000)
}
else
{
fare = 1530 * c_km
}
if (c_km <= 1000)
{						// 100km以下は切り上げ
 // 1の位を切り上げ
            fare = (fare + 9999) / 10000 * 10
}
else
{				// 100㎞越えは四捨五入
fare = (fare + 50000) / 100000 * 100
}
return RouteUtil.taxadd_ic(fare, RouteDB.instance.tax())
}

 //	C: 電車特定区間(大阪)
    //	calc_fare() => retr_fare() =>
    //
    //	@param [in] km    営業キロ
    //	@return 運賃額
    //
    private fun Fare_osaka(km:Int):Int {
var fare:Int
val c_km:Int

if (km < 31)
{							// 1 to 3km
if (RouteDB.instance.tax() == 5)
{
return 120
}
else
{
return 120
}
}
if (km < 61)
{							// 4 to 6km
if (RouteDB.instance.tax() == 5)
{
return 160
}
else
{
return 160
}
}
if (km < 101)
{							// 7 to 10km
if (RouteDB.instance.tax() == 5)
{
return 170
}
else
{
return 180
}
}
if (6000 < km)
{						// 600km越えは40キロ刻み
RouteUtil.ASSERT(false)
return -1
}
else if (1000 < km)
{					// 100.1-600kmは20キロ刻み
c_km = (km - 1) / 200 * 200 + 100
}
else if (500 < km)
{					// 50.1-100kmは10キロ刻み
c_km = (km - 1) / 100 * 100 + 50
}
else if (100 < km)
{					// 10.1-50kmは5キロ刻み
c_km = (km - 1) / 50 * 50 + 30
}
else
{
RouteUtil.ASSERT(false)
c_km = 0
}

if (3000 < c_km)
{
fare = 1530 * 3000 + 1215 * (c_km - 3000)
}
else
{
fare = 1530 * c_km
}
if (c_km <= 1000)
{						// 100km以下は切り上げ
 // 1の位を切り上げ
            fare = (fare + 9999) / 10000 * 10
}
else
{								// 100㎞越えは四捨五入
fare = (fare + 50000) / 100000 * 100
}
return RouteUtil.taxadd(fare, RouteDB.instance.tax())	// tax = +5%, 四捨五入
}

 //	d: 電車特定区間(山手線)
    //	calc_fare() => retr_fare() =>
    //
    //	@param [in] km    営業キロ
    //	@return 運賃額
    //
    private fun Fare_yamate_f(km:Int):Int {
var fare:Int
val c_km:Int

if (km < 31)
{							// 1 to 3km
if (RouteDB.instance.tax() == 5)
{
return 130
}
else
{
return 133
}
}
if (km < 61)
{							// 4 to 6km
if (RouteDB.instance.tax() == 5)
{
return 150
}
else
{
return 154
}
}
if (km < 101)
{							// 7 to 10km
if (RouteDB.instance.tax() == 5)
{
return 160
}
else
{
return 165
}
}
if (3000 < km)
{						// 300km越えは未定義
RouteUtil.ASSERT(false)
return -1
}
else if (1000 < km)
{					// 100.1-300kmは20キロ刻み
c_km = (km - 1) / 200 * 200 + 100
}
else if (500 < km)
{					// 50.1-100kmは10キロ刻み
c_km = (km - 1) / 100 * 100 + 50
}
else if (100 < km)
{					// 10.1-50kmは5キロ刻み
c_km = (km - 1) / 50 * 50 + 30
}
else
{
RouteUtil.ASSERT(false)
c_km = 0
}
fare = 1325 * c_km

if (c_km <= 1000)
{	// 100km以下は切り上げ
 // 1の位を切り上げ
            fare = (fare + 9999) / 10000 * 10
}
else
{								// 100㎞越えは四捨五入
fare = (fare + 50000) / 100000 * 100
}
return RouteUtil.taxadd_ic(fare, RouteDB.instance.tax())	// tax = +5%, 四捨五入
}

 //	e: 電車特定区間(大阪環状線)
    //	calc_fare() => retr_fare() =>
    //
    //	@param [in] km    営業キロ
    //	@return 運賃額
    //
    private fun Fare_osakakan(km:Int):Int {
var fare:Int
val c_km:Int

if (km < 31)
{							// 1 to 3km
if (RouteDB.instance.tax() == 5)
{
return 120
}
else
{
return 120
}
}
if (km < 61)
{							// 4 to 6km
if (RouteDB.instance.tax() == 5)
{
return 160
}
else
{
return 160
}
}
if (km < 101)
{							// 7 to 10km
if (RouteDB.instance.tax() == 5)
{
return 170
}
else
{
return 180
}
}
if (3000 < km)
{						// 300km越えは未定義
RouteUtil.ASSERT(false)
return -1
}
else if (1000 < km)
{					// 100.1-300kmは20キロ刻み
c_km = (km - 1) / 200 * 200 + 100
}
else if (500 < km)
{					// 50.1-100kmは10キロ刻み
c_km = (km - 1) / 100 * 100 + 50
}
else if (100 < km)
{					// 10.1-50kmは5キロ刻み
c_km = (km - 1) / 50 * 50 + 30
}
else
{
RouteUtil.ASSERT(false)
c_km = 0
}
fare = 1325 * c_km

if (c_km <= 1000)
{						// 100km以下は切り上げ
 // 1の位を切り上げ
            fare = (fare + 9999) / 10000 * 10
}
else
{								// 100㎞越えは四捨五入
fare = (fare + 50000) / 100000 * 100
}
return RouteUtil.taxadd(fare, RouteDB.instance.tax())	// tax = +5%, 四捨五入
}

 //	f: JR北海道幹線
    //	calc_fare() => retr_fare() =>
    //
    //	@param [in] km    計算キロ
    //	@return 運賃額
    //
    private fun Fare_hokkaido_basic(km:Int):Int {
var fare:Int
val c_km:Int
val tbl:String		// [16]

if (km < 31)
{							// 1 to 3km
if (RouteDB.instance.tax() == 5)
{
return 160
}
else
{
return 170
}
}
if (km < 61)
{							// 4 to 6km
if (RouteDB.instance.tax() == 5)
{
return 200
}
else
{
return 210
}
}
if (km < 101)
{							// 7 to 10km
if (RouteDB.instance.tax() == 5)
{
return 210
}
else
{
return 220
}
}

tbl = String.format("h%d", RouteDB.instance.tax())
fare = Fare_table("bspekm", tbl, km)
if (0 != fare)
{
return fare
}

if (6000 < km)
{						// 600km越えは40キロ刻み
c_km = (km - 1) / 400 * 400 + 200
}
else if (1000 < km)
{					// 100.1-600kmは20キロ刻み
c_km = (km - 1) / 200 * 200 + 100
}
else if (500 < km)
{					// 50.1-100kmは10キロ刻み
c_km = (km - 1) / 100 * 100 + 50
}
else if (100 < km)
{					// 10.1-50kmは5キロ刻み
c_km = (km - 1) / 50 * 50 + 30
}
else
{
RouteUtil.ASSERT(false)
c_km = 0
}

if (6000 < c_km)
{
fare = 1785 * 2000 + 1620 * (3000 - 2000) + 1285 * (6000 - 3000) + 705 * (c_km - 6000)
}
else if (3000 < c_km)
{
fare = 1785 * 2000 + 1620 * (3000 - 2000) + 1285 * (c_km - 3000)
}
else if (2000 < c_km)
{
fare = 1785 * 2000 + 1620 * (c_km - 2000)
}
else
{
fare = 1785 * c_km
}
if (c_km <= 1000)
{						// 100km以下は切り上げ
 // 1の位を切り上げ
            fare = (fare + 9999) / 10000 * 10
}
else
{								// 100㎞越えは四捨五入
fare = (fare + 50000) / 100000 * 100
}
return RouteUtil.taxadd(fare, RouteDB.instance.tax())	// tax = +5%, 四捨五入
}

 //	JR北海道地方交通線
    //	calc_fare() => retr_fare() =>
    //
    //	@param [in] km    営業キロ
    //	@return 運賃額
    //
    private fun Fare_hokkaido_sub(km:Int):Int {
var fare:Int
var c_km:Int

if (km < 31)
{							// 1 to 3km
if (RouteDB.instance.tax() == 5)
{
return 160
}
else
{
return 170
}
}
if (km < 61)
{							// 4 to 6km
if (RouteDB.instance.tax() == 5)
{
return 200
}
else
{
return 210
}
}
if (km < 101)
{							// 7 to 10km
if (RouteDB.instance.tax() == 5)
{
return 220
}
else
{
return 230
}
}

if (12000 < km)
{		// 1200km越えは別表第2号イの4にない
RouteUtil.ASSERT(false)
return -1
}

c_km = Fare_table("lspekm", 'h', km)
if (c_km == 0)
{
RouteUtil.ASSERT(false)
return -1
}
if (c_km < 0)
{
return -c_km		/* fare */
}

c_km *= 10

if (5460 <= c_km)
{
fare = 1960 * 1820 + 1780 * (2730 - 1820) + 1410 * (5460 - 2730) + 770 * (c_km - 5460)
}
else if (2730 < c_km)
{
fare = 1960 * 1820 + 1780 * (2730 - 1820) + 1410 * (c_km - 2730)
}
else if (1820 < c_km)
{
fare = 1960 * 1820 + 1780 * (c_km - 1820)
}
else
{ /* <= 182km */
fare = 1960 * c_km
}
if (c_km <= 1000)
{						// 100km以下は切り上げ
 // 1の位を切り上げ
            fare = (fare + 9999) / 10000 * 10
}
else
{				// 100㎞越えは四捨五入
fare = (fare + 50000) / 100000 * 100
}
return RouteUtil.taxadd(fare, RouteDB.instance.tax())
}

 //	g: JR四国 幹線
    //	calc_fare() => retr_fare() =>
    //
    //	@param [in] skm    営業キロ
    //	@param [in] ckm    計算キロ
    //	@return 運賃額
    //
    private fun Fare_shikoku(skm:Int, ckm:Int):Int {
var fare:Int
val c_km:Int

 /* JTB時刻表 C-3表 */
        if (ckm != skm)
{
if (RouteDB.instance.tax() == 5)
{
 /* JR四国 幹線+地方交通線 */
                /* (m) */
                if (KM.KM(ckm) == 4 && KM.KM(skm) == 3)
{
return 160	/* \ */
}
else if (KM.KM(ckm) == 11 && KM.KM(skm) == 10)
{
return 230	/* \ */
}
}
else
{
 /* JR四国 幹線+地方交通線 */
                    /* (m) */
                if (KM.KM(ckm) == 4 && KM.KM(skm) == 3)
{
return 160	/* \ */
}
else if (KM.KM(ckm) == 11 && KM.KM(skm) == 10)
{
return 230	/* \ */
}
/*** JR四国は消費税8%でもここは据え置きみたい  */
            }
}

if (ckm < 31)
{							// 1 to 3km
if (RouteDB.instance.tax() == 5)
{
return 160
}
else
{
return 160
}
}
if (ckm < 61)
{							// 4 to 6km
if (RouteDB.instance.tax() == 5)
{
return 200
}
else
{
return 210
}
}
if (ckm < 101)
{						// 7 to 10km
if (RouteDB.instance.tax() == 5)
{
return 210
}
else
{
return 220
}
}

fare = Fare_table("bspekm", if (RouteDB.instance.tax() == 5) "s5" else "s8", ckm)
if (0 != fare)
{
return fare
}

if (6000 < ckm)
{						// 600km越えは40キロ刻み
c_km = (ckm - 1) / 400 * 400 + 200
}
else if (1000 < ckm)
{					// 100.1-600kmは20キロ刻み
c_km = (ckm - 1) / 200 * 200 + 100
}
else if (500 < ckm)
{					// 50.1-100kmは10キロ刻み
c_km = (ckm - 1) / 100 * 100 + 50
}
else if (100 < ckm)
{					// 10.1-50kmは5キロ刻み
c_km = (ckm - 1) / 50 * 50 + 30
}
else
{
 /* 0-10kmは上で算出return済み */
            RouteUtil.ASSERT(false)
c_km = 0
}

if (6000 <= c_km)
{
fare = 1821 * 1000 + 1620 * (3000 - 1000) + 1285 * (6000 - 3000) + 705 * (c_km - 6000)
}
else if (3000 < c_km)
{
fare = 1821 * 1000 + 1620 * (3000 - 1000) + 1285 * (c_km - 3000)
}
else if (1000 < c_km)
{
fare = 1821 * 1000 + 1620 * (c_km - 1000)
}
else
{
 /* 10.1km - 100.0 km */
            fare = 1821 * c_km
}

if (c_km <= 1000)
{						// 100km以下は切り上げ
 // 1の位を切り上げ
            fare = (fare + 9999) / 10000 * 10
}
else
{								// 100㎞越えは四捨五入
fare = (fare + 50000) / 100000 * 100
}
return RouteUtil.taxadd(fare, RouteDB.instance.tax())	// tax = +5%, 四捨五入
}

 //	h: JR九州 幹線
    //	calc_fare() => retr_fare() =>
    //
    //	@param [in] skm    営業キロ
    //	@param [in] ckm    計算キロ
    //	@return 運賃額
    //
    private fun Fare_kyusyu(skm:Int, ckm:Int):Int {
var fare:Int
val c_km:Int

 /* JTB時刻表 C-3表 */
        if (ckm != skm)
{
if (RouteDB.instance.tax() == 5)
{
 /* JR九州 幹線+地方交通線 */
                /* (n) */
                if (KM.KM(ckm) == 4 && KM.KM(skm) == 3)
{
return 170	/* \ */
}
else if (KM.KM(ckm) == 11 && KM.KM(skm) == 10)
{
return 240	/* \ */
}
}
else
{
 /* JR九州 幹線+地方交通線 */
                    /* (n) */
                if (KM.KM(ckm) == 4 && KM.KM(skm) == 3)
{
return 180	/* \ */
}
else if (KM.KM(ckm) == 11 && KM.KM(skm) == 10)
{
return 250	/* \ */
}
}
}

if (ckm < 31)
{							// 1 to 3km
if (RouteDB.instance.tax() == 5)
{
return 160
}
else
{
return 160
}
}
if (ckm < 61)
{							// 4 to 6km
if (RouteDB.instance.tax() == 5)
{
return 200
}
else
{
return 220
}
}
if (ckm < 101)
{						// 7 to 10km
if (RouteDB.instance.tax() == 5)
{
return 220
}
else
{
return 230
}
}

fare = Fare_table("bspekm", if (RouteDB.instance.tax() == 5) "k5" else "k8", ckm)
if (0 != fare)
{
return fare
}

if (6000 < ckm)
{						// 600km越えは40キロ刻み
c_km = (ckm - 1) / 400 * 400 + 200
}
else if (1000 < ckm)
{				// 100.1-600kmは20キロ刻み
c_km = (ckm - 1) / 200 * 200 + 100
}
else if (500 < ckm)
{					// 50.1-100kmは10キロ刻み
c_km = (ckm - 1) / 100 * 100 + 50
}
else if (100 < ckm)
{					// 10.1-50kmは5キロ刻み
c_km = (ckm - 1) / 50 * 50 + 30
}
else
{
 /* 0-10kmは上で算出return済み */
            RouteUtil.ASSERT(false)
c_km = 0
}
if (6000 <= c_km)
{
fare = 1775 * 3000 + 1285 * (6000 - 3000) + 705 * (c_km - 6000)
}
else if (3000 < c_km)
{
fare = 1775 * 3000 + 1285 * (c_km - 3000)
}
else if (1000 < c_km)
{
fare = 1775 * c_km
}
else
{
 /* 10.1-100.0kmはDBで定義 */
            RouteUtil.ASSERT(false)
fare = 0
}

if (c_km <= 1000)
{							// 100km以下は切り上げ
 // 1の位を切り上げ
            fare = (fare + 9999) / 10000 * 10
}
else
{									// 100㎞越えは四捨五入
fare = (fare + 50000) / 100000 * 100
}
return RouteUtil.taxadd(fare, RouteDB.instance.tax())	// tax = +5%, 四捨五入
}

 //	会社線の運賃を得る
    //	calc_fare() => aggregate_fare_info() =>
    //
    //	@param [in]     station_id1   駅1
    //	@param [in]     station_id2   駅2
    //	@param [in/out] compantFare   [0]区間運賃 / [1]小児運賃 / [2]学割運賃(非適用は0) / [3]併算割引運賃有無(1=無)
    //	@return true : success / false : failuer
    //
    private fun Fare_company(station_id1:Int, station_id2:Int):CompanyFare {
var companyFare:CompanyFare? = null
val sql:String
val tsql =
 //"select fare, child, academic from %s" +
                "select fare, academic, flg from %s" +
" where " +
" ((station_id1=?1 and station_id2=?2) or" +
"  (station_id1=?2 and station_id2=?1))"

val tbl = arrayOf("t_clinfar", "t_clinfar5p")

sql = String.format(tsql, tbl[if (RouteDB.instance.tax() == 5) 1 else 0])
val dbo = RouteDB.db().rawQuery(sql, arrayOf(station_id1.toString(), station_id2.toString()))	// false));
var rc = false
try
{
val flg:Int
val fare_work:Int
if (dbo.moveToNext())
{
companyFare = CompanyFare()
fare_work = dbo.getInt(0)	// fare
companyFare.fare = fare_work
companyFare.fareAcademic = dbo.getInt(1)	// academic
flg = dbo.getInt(2)	// flg
rc = true
companyFare.passflg = RouteUtil.IS_CONNECT_NON_DISCOUNT_FARE(flg)
 // (0=5円は切り捨て, 1=5円未満切り上げ)
        		if (RouteUtil.IS_ROUND_UP_CHILDREN_FARE(flg))
{
companyFare.fareChild = RouteUtil.round_up(fare_work / 2)
}
else
{
companyFare.fareChild = RouteUtil.round_down(fare_work / 2)
}
}
}

finally
{
dbo.close()
}
return companyFare
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
    private fun Fare_table(tbl:String, field:String, km:Int):Int {
val tsql = "select %s from t_fare%s where km<=? order by km desc limit(1)"
val sql = String.format(tsql, field, tbl)

val dbo = RouteDB.db().rawQuery(sql, arrayOf(KM.KM(km).toString()))
var rc = 0
try
{
if (dbo.moveToNext())
{
rc = dbo.getInt(0)
}
}

finally
{
dbo.close()
}
return rc
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
    private fun Fare_table(tbl:String, c:Char, km:Int):Int {
val ckm:Int
val fare:Int

val sql = String.format(
"select ckm, %c%d from t_fare%s where km<=? order by km desc limit(1)",
c, RouteDB.instance.tax(), tbl)

val dbo = RouteDB.db().rawQuery(sql, arrayOf(KM.KM(km).toString()))
var rc = 0
try
{
if (dbo.moveToNext())
{
ckm = dbo.getInt(0)
fare = dbo.getInt(1)
if (fare == 0)
{
rc = ckm
}
else
{
rc = -fare
}
}
}

finally
{
dbo.close()
}
return rc
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
    private fun Fare_table(dkm:Int, skm:Int, c:Char):Int {
val fare:Int

val sql = String.format(

"select %c%d from t_farels where dkm=?2 and (skm=-1 or skm=?1)",
c, RouteDB.instance.tax())
 /*
    "select k,l from t_farels where "
    "(-1=(select skm from t_farels where skm=(select max(skm) from t_farels where skm<=?1))"
    "	or"
    " skm=(select skm from t_farels where skm=(select max(skm) from t_farels where skm<=?1))"
    ") and dkm="
    "(select dkm from t_farels where dkm=(select max(dkm) from t_farels where dkm<=?2))";
    */
        val dbo = RouteDB.db().rawQuery(sql, arrayOf(KM.KM(skm).toString(), KM.KM(dkm).toString()))
try
{
if (dbo.moveToNext())
{
fare = dbo.getInt(0)
}
else
{
fare = 0
}
}

finally
{
dbo.close()
}
return fare
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
    private fun CheckSpecficFarePass(line_id:Int, station_id1:Int, station_id2:Int):Int {
val sql = String.format(
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
" limit(1)", RouteDB.instance.tax(), RouteDB.instance.tax())

val dbo = RouteDB.db().rawQuery(sql, arrayOf(line_id.toString(), station_id1.toString(), station_id2.toString()))
var fare = 0
try
{
if (dbo.moveToNext())
{
fare = dbo.getInt(2)
System.out.printf("CheckSpecificFarePass found: %s, %s, +%d\n", RouteUtil.StationName(dbo.getInt(0)), RouteUtil.StationName(dbo.getInt(1)), fare)
 /* found, return values is add fare */
            }
}

finally
{
dbo.close()
}
return fare	/* not found */
}

 //	特別運賃区間か判定し該当していたら運賃額を返す
    //	calc_fare() =>
    //
    //	@param [in] station_id1 駅1(startStationId)
    //	@param [in] station_id2 駅2(end_station_id)
    //
    //	@return 特別区間運賃
    //
    private fun SpecificFareLine(station_id1:Int, station_id2:Int, kind:Int):Int {
val sql:String

val tsql = "select fare%dp from t_farespp where kind=?3 and" +
" ((station_id1=?1 and station_id2=?2) or" +
"  (station_id1=?2 and station_id2=?1))"

sql = String.format(tsql, RouteDB.instance.tax())

val dbo = RouteDB.db().rawQuery(sql, arrayOf(station_id1.toString(), station_id2.toString(), kind.toString()))
var fare = 0   /* if not found */
try
{
if (dbo.moveToNext())
{
fare = dbo.getInt(0)
System.out.printf("SpecificFareLine found: %s - %s, +%d\n", RouteUtil.StationName(station_id1), RouteUtil.StationName(station_id2), fare)
}
}

finally
{
dbo.close()
}
return fare
}

/**
         * case 大阪環状線

     	 * calc_fare() => aggregate_fare_info() =>

     	 * @param [in] route flag 	 大阪環状線通過フラグ
      * *
 * @param [in] line_id   	 路線ID
      * *
 * @param [in] station_id1  駅1
      * *
 * @param [in] station_id2  駅2
      * *
      * *
 * @return int[] [0] 営業キロ
      * *
 * @return int[] [1] 計算キロ
      * *
 * @return int[] [2] 駅1の会社区間部の営業キロ(駅1の会社ID≠駅2の会社ID時のみ有効)
      * *                          駅1が境界駅なら-1を返す, 境界駅が駅1～駅2間になければ、Noneを返す
      * *
 * @return int[] [3] 駅1の会社区間部の計算キロ(駅1の会社ID≠駅2の会社ID時のみ有効)
      * *                          駅2が境界駅なら-1を返す, 境界駅が駅1～駅2間になければ、Noneを返す
      * *
 * @return int[] [4] 駅1の会社ID
      * *
 * @return int[] [5] 駅2の会社ID
      * *
 * @return int[] [6] 駅1のsflg
      * *
 * @return int[] [7] 駅2のsflg
      * *
 * @return int[] [8] 1=JR以外の会社線／0=JRグループ社線
 */
    private fun GetDistanceEx(osakakan_aggregate:Int, line_id:Int, station_id1:Int, station_id2:Int):Array<Int> {
val result:MutableList<Int>
var rslt:Long = 0

result = RouteUtil.GetDistance(osakakan_aggregate, line_id, station_id1, station_id2) // [0][1]
result.add(0)	// sales_km for in company as station_id1 [2]
result.add(0)	// calc_km  for in company as station_id1 [3]
result.add(RouteUtil.JR_WEST)	// 駅ID1の会社ID [4]
result.add(RouteUtil.JR_WEST)	// 駅ID2の会社ID [5]
val ctx = RouteDB.db().rawQuery("select" + " (select sflg&4095 from t_station where rowid=?1) + ((select sflg&4095 from t_station where rowid=?2) * 65536)"		// [5]
, arrayOf(station_id1.toString(), station_id2.toString()))
try
{
if (ctx.moveToNext())
{
rslt = ctx.getLong(0)
}
}

finally
{
ctx.close()
}
result.add((0xffff and rslt).toInt())			// 駅1のsflg [6]
result.add((0xffff and rslt.ushr(16)).toInt())	// 駅2のsflg [7]
result.add((0x1 and rslt.ushr(31)).toInt())		// bit31:1=JR以外の会社線／0=JRグループ社線 = 0

return result.toTypedArray<Int>()
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
    private fun IsBulletInUrban(line_id:Int, station_id1:Int, station_id2:Int):Boolean {
val tsql = " select count(*) - sum(sflg>>13&1) from t_station t join t_lines l on l.station_id=t.rowid" +
"	where line_id=?1" +
"	and (lflg&((1<<31)|(1<<17)))=0" +
"	and sales_km>=(select min(sales_km) from t_lines" +
"			where line_id=?1 and (station_id=?2 or station_id=?3))" +
"	and sales_km<=(select max(sales_km) from t_lines" +
"			where line_id=?1 and (station_id=?2 or station_id=?3))"
 // 13:近郊区間、17:新幹線仮想分岐駅

        var rsd = -1

if (!RouteUtil.IS_SHINKANSEN_LINE(line_id))
{
return false
}

val dbo = RouteDB.db().rawQuery(tsql, arrayOf(line_id.toString(), station_id1.toString(), station_id2.toString()))
try
{
if (dbo.moveToNext())
{
rsd = dbo.getInt(0)
}
}

finally
{
dbo.close()
}
return 0 < rsd
}


private fun IsIC_area(urban_id:Int):Boolean {
return urban_id == RouteUtil.URB_TOKYO ||
urban_id == RouteUtil.URB_NIGATA ||
urban_id == RouteUtil.URB_SENDAI
}

 //static:
    //	旅客営業取扱基準規定43条の2（小倉、西小倉廻り）
    //	@param [out] fare_inf  営業キロ(sales_km, kyusyu_sales_km, kyusyu_calc_km)
    //
    //	showFare() => calc_fare() ->
    //	CheckOfRule114j() => calc_fare ->
    //
    private fun CheckAndApplyRule43_2j(route:Array<RouteItem>):Int {
val dbid = DbidOf()
var stage:Int
var c:Int
var rl:Int
var km:Int
val kagoshima_line_id:Int
var ite = 0

stage = 0
c = 0
rl = 0
ite = 0
while (ite < route.size)
{
when (stage) {
0 -> if (0 < route[ite].lineId && route[ite].stationId.toInt() == dbid.StationIdOf_NISHIKOKURA)
{
c = 1
stage = 1
}
else if (route[ite].stationId.toInt() == dbid.StationIdOf_HAKATA)
{
if (route[ite].lineId.toInt() == dbid.LineIdOf_SANYOSHINKANSEN)
{
c = 4			// 山陽新幹線
}
else
{
c = 2
}
stage = 1
}
else if (0 < route[ite].lineId && route[ite].stationId.toInt() == dbid.StationIdOf_YOSHIZUKA)
{
c = 3				// 吉塚
stage = 1
break
}
1 -> {
when (c) {
1 -> if (route[ite].stationId.toInt() == dbid.StationIdOf_KOKURA &&
ite + 1 < route.size &&
route[ite + 1].lineId.toInt() == dbid.LineIdOf_SANYOSHINKANSEN &&
route[ite + 1].stationId.toInt() == dbid.StationIdOf_HAKATA)
{
rl = rl or 1	// 小倉 山陽新幹線 博多
}
2 -> if (route[ite].stationId.toInt() == dbid.StationIdOf_KOKURA &&
route[ite].lineId.toInt() == dbid.LineIdOf_SANYOSHINKANSEN &&
ite + 2 < route.size && // 西小倉止まりまでなら非適用

route[ite + 1].stationId.toInt() == dbid.StationIdOf_NISHIKOKURA)
{
rl = rl or 1
}
3 -> if (route[ite].stationId.toInt() == dbid.StationIdOf_HAKATA &&
ite + 1 < route.size &&
route[ite + 1].lineId.toInt() == dbid.LineIdOf_SANYOSHINKANSEN)
{
rl = rl or 2
}
4 -> if (route[ite].stationId.toInt() == dbid.StationIdOf_YOSHIZUKA && ite + 1 < route.size)
{	// 吉塚止まりまでなら非適用
rl = rl or 2
}
else -> {}
}
c = 0
stage = 0
}
else -> RouteUtil.ASSERT(false)
}
ite++
}
if (rl != 0)
{
km = 0
kagoshima_line_id = RouteUtil.GetLineId("鹿児島線")
if (rl and 1 != 0)
{
km = RouteUtil.GetDistance(kagoshima_line_id, dbid.StationIdOf_KOKURA, dbid.StationIdOf_NISHIKOKURA)[0]
System.out.printf("applied 43-2(西小倉)\n")
}
if (rl and 2 != 0)
{
km += RouteUtil.GetDistance(kagoshima_line_id, dbid.StationIdOf_HAKATA, dbid.StationIdOf_YOSHIZUKA)[0]
System.out.printf("applied 43-2(吉塚)\n")
}
return km
}
return 0
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
    private fun CheckOfRule89j(route:List<RouteItem>):Int {
val dbid = DbidOf()
val lastIndex:Int
 /*static */var distance = 0

lastIndex = route.size.toInt() - 1
if (lastIndex < 2)
{
return 0
}

 // 北新地発やで
        if (route[0].stationId.toInt() == dbid.StationIdOf_KITASHINCHI && route[1].stationId.toInt() == dbid.StationIdOf_AMAGASAKI && (lastIndex <= 1 || route[2].lineId.toInt() != dbid.LineIdOf_TOKAIDO ||
RouteUtil.LINE_DIR.LDIR_ASC == RouteUtil.DirLine(dbid.LineIdOf_TOKAIDO, dbid.StationIdOf_AMAGASAKI, route[2].stationId.toInt())))
{
 /* 北新地-(JR東西線)-尼崎 の場合、発駅（北新地）は大阪や */
            // route.get(0).stationId = dbid.StationIdOf_OSAKA;
            //if (distance == 0) {
            distance = RouteUtil.GetDistance(dbid.LineIdOf_TOKAIDO, dbid.StationIdOf_OSAKA, dbid.StationIdOf_AMAGASAKI)[0] - RouteUtil.GetDistance(route[1].lineId.toInt(), dbid.StationIdOf_AMAGASAKI, dbid.StationIdOf_KITASHINCHI)[0]
 //}
            RouteUtil.ASSERT(distance < 0)
return distance
}			// 北新地終着やねん
else if (route[route.size - 1].stationId.toInt() == dbid.StationIdOf_KITASHINCHI && route[lastIndex - 1].stationId.toInt() == dbid.StationIdOf_AMAGASAKI && (lastIndex <= 1 || route[lastIndex - 1].lineId.toInt() != dbid.LineIdOf_TOKAIDO ||
RouteUtil.LINE_DIR.LDIR_DESC == RouteUtil.DirLine(dbid.LineIdOf_TOKAIDO, route[lastIndex - 2].stationId.toInt(), dbid.StationIdOf_AMAGASAKI)))
{
 //route.get(route.size() - 1).stationId = dbid.StationIdOf_OSAKA;
            //if (distance == 0) {
            distance = RouteUtil.GetDistance(dbid.LineIdOf_TOKAIDO, dbid.StationIdOf_OSAKA, dbid.StationIdOf_AMAGASAKI)[0] - RouteUtil.GetDistance(route[route.size - 1].lineId.toInt(), dbid.StationIdOf_AMAGASAKI, dbid.StationIdOf_KITASHINCHI)[0]
 //}
            RouteUtil.ASSERT(distance < 0)
return distance
}
else
{
return 0
}
}
}



}/**	株主優待割引運賃を返す(会社線運賃は含まない)

     	 * @param index      [in]   0から1 JR東日本のみ 0は2割引、1は4割引を返す
      * *
 * @param title      [out]  項目表示文字列
      * *
 * @param applied_r114  [in]  true = 114条適用 / false = 114条適用前
      * *
 * @retval	[円](無割引、無効は0)
 */ // FARE_INFO
