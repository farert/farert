package org.sutezo.alps

import android.content.Context
import org.json.JSONArray
import org.sutezo.farert.ArchiveRouteActivity


const val MAX_HOLDER : Int = 60
const val MAX_HISTORY = 20
const val MAX_ARCHIVE_ROUTE = 100

const val msgPossibleLowcost = "近郊区間内ですので最短経路の運賃で利用可能です(途中下車不可、有効日数当日限り)"
const val msgAppliedLowcost = "近郊区間内ですので最安運賃の経路で計算(途中下車不可、有効日数当日限り)"
const val msgSpecificFareApply = "特定区間割引運賃適用"
const val msgCantMetroTicket = "近郊区間内ですので同一駅発着のきっぷは購入できません."
const val msgCanYouNeerestStation = "「単駅最安」で単駅発着が選択可能です"
const val msgCanYouSpecificTerm = "「特定都区市内発着」で特定都区市内発着が選択可能です"


fun getCompanys() : List<Int> {
    val companys : MutableList<Int> = mutableListOf() // JR group
    val dbo = RouteUtil.Enum_company_prefect()

    dbo.use {
        while (dbo.moveToNext()) {
            val ident = dbo.getInt(1);
            if (ident < 0x10000) {
                companys.add(ident)
            }
        }
    }
    return companys
}

fun getPrefects() : List<Int> {

    val prefects : MutableList<Int> = ArrayList(46) // Prefect
    val dbo = RouteUtil.Enum_company_prefect()

    dbo.use {
        while (dbo.moveToNext()) {
            val ident = dbo.getInt(1);
            if (ident < 0x10000) {
                // ignored
            } else {
                prefects.add(ident)
            }
        }
    }
    return prefects
}

fun fareNumStr(num: Int): String
{
    return RouteUtil.num_str_yen(num)
}

fun kmNumStr(num: Int): String
{
    return RouteUtil.num_str_km(num)
}

// Kana from Stations
//  キーワードに一致した駅名を返す
//  ひらがな昇順ソート
fun keyMatchStations(key: String): List<Int>
{
    val stations: MutableList<Int> = mutableListOf<Int>()
    val dbo = RouteUtil.Enum_station_match(key)
    dbo.use {
        while (dbo.moveToNext()) {
            val station_id = dbo.getInt(1);
            stations.add(station_id)
        }
    }
    return stations;
}


fun getStationId(name: String) : Int
{
    return RouteUtil.GetStationId(name)
}

// StationName
fun stationName(ident: Int):String
{
    return RouteUtil.StationName(ident)
}

// StationNameEx
fun stationNameEx(ident:Int):String
{
    return RouteUtil.StationNameEx(ident)
}

fun lineName(ident:Int) : String
{
    return RouteUtil.LineName(ident)
}

// Company or Prefect Name
fun companyOrPrefectName(ident:Int):String
{
    if (ident < 0x10000) {
        return RouteUtil.CompanyName(ident)
    } else {
        return RouteUtil.PrefectName(ident)
    }
}

// line from prefect or company
//  会社/都道府県idから路線を返す
fun linesFromCompanyOrPrefect(id:Int): List<Int>
{
    val lines : MutableList<Int> = mutableListOf<Int>()
    val dbo = RouteUtil.Enum_lines_from_company_prefect(id)
    dbo.use {
        while (dbo.moveToNext()) {
            lines.add(dbo.getInt(1))
        }
    }
    return lines
}

//	Enum menu_station_select with in company or prefect + line
//	会社or都道府県 + 路線に属する駅を列挙
fun stationsWithInCompanyOrPrefectAnd(id:Int, line_id:Int) : List<Int>
{
    val stations : MutableList<Int> = mutableListOf<Int>()
    val dbo = RouteUtil.Enum_station_located_in_prefect_or_company_and_line(id, line_id)
    dbo.use {
        while (dbo.moveToNext()) {
            stations.add(dbo.getInt(1))
        }
    }
    return stations;
}

fun lineIdsFromStation(id:Int):List<Int>
{
    val lines = mutableListOf<Int>()
    val dbo = RouteUtil.Enum_line_of_stationId(id)
    dbo.use {
        while (dbo.moveToNext()) {
            lines.add(dbo.getInt(1))
        }
    }
    return lines;
}

fun stationsIdsOfLineId(lineId:Int):List<Int>
{
    val stations = mutableListOf<Int>()
    val dbo = RouteUtil.Enum_station_of_lineId(lineId)
    dbo.use {
        while (dbo.moveToNext()) {
            stations.add(dbo.getInt(1))
        }
    }
    return stations;
}

fun junctionIdsOfLineId(lineId:Int, stationId: Int = 0):List<Int>
{
    val stations = mutableListOf<Int>()
    val dbo = RouteUtil.Enum_junction_of_lineId(lineId, stationId)
    dbo.use {
        while (dbo.moveToNext()) {
            stations.add(dbo.getInt (1))
        }
    }
    return stations;
}

fun terminalName(ident:Int):String = CalcRoute.BeginOrEndStationName(ident)

fun isJunction(station_id:Int): Boolean = RouteUtil.STATION_IS_JUNCTION(station_id)

fun isSpecificJunction(lineId:Int, station_id:Int) : Boolean =
    0 != ((RouteUtil.AttrOfStationOnLineLine(lineId, station_id) and (1.shl(31))))

fun PrectNameByStation(stationId:Int):String = RouteUtil.GetPrefectByStationId(stationId)

//	駅のひらがな読み
fun getKanaFromStationId(ident:Int): String = RouteUtil.GetKanaFromStationId(ident)


fun saveParam(context: Context, key:String, value:String) : Unit
{
    val sharedPref = context.getSharedPreferences("settings", Context.MODE_PRIVATE)
    val sharedEdit = sharedPref.edit()
    sharedEdit.putString(key, value)
    sharedEdit.apply()
}

fun saveParam(context: Context, key: String, values: List<String>):Unit {
    val sharedPref = context.getSharedPreferences("settings", Context.MODE_PRIVATE)
    val jsonArray = JSONArray(values)
    val shredEdit = sharedPref.edit()
    shredEdit.putString(key, jsonArray.toString())
    shredEdit.apply()
}

fun readParam(context: Context, key: String) : String {
    val sharedPref = context.getSharedPreferences("settings", Context.MODE_PRIVATE)
    return sharedPref.getString(key, "")?: ""
}

fun readParams(context: Context, key: String) : List<String> {
    val values = mutableListOf<String>()
    var sharedPref  = context.getSharedPreferences("settings", Context.MODE_PRIVATE ) ?:
    return values

    val params = sharedPref.getString(key, "[]")
    val jsonArray = JSONArray(params)
    for (i in 0 until jsonArray.length()) {
        values.add(jsonArray.get(i).toString())
    }
    return values
}

fun saveHistory(context: Context, history : List<String>) {
    saveParam(context,"history", history)
}

fun appendHistory(context: Context, terminal : String) : Unit {
    val history = mutableListOf<String>()
    history.addAll(0, readParams(context, "history"))
    if (history.contains(terminal)) {
        history.remove(terminal)
    }
    history.add(0, terminal)
    saveParam(context,"history", history)
}


// 駅リスト選択でつかう
// line_id : 除外する路線
fun getDetailStationInfoForSelList(line_id : Int, station_id: Int) : String {
    var details : String = "(${RouteUtil.GetKanaFromStationId(station_id)}) "
    var f : Boolean = false

    if (RouteUtil.STATION_IS_JUNCTION(station_id) &&
            !RouteUtil.IsSpecificJunction(line_id, station_id)) {
        for (lid in lineIdsFromStation(station_id)) {
            if ((lid != line_id) &&
                    !RouteUtil.IsSpecificJunction(lid, station_id)) {
                /* junction(lflag.bit12 on) */
                if (f) {
                    details += "/${RouteUtil.LineName(lid)}"
                } else {
                    details += " ${RouteUtil.LineName(lid)}"
                    f = true
                }
            }
        }
    }
    return details
}


// fare calc Extend method
fun CalcRoute.calcFareInfo() : FareInfo
{
    val result = FareInfo()
    val fi = FARE_INFO()

    this.calcFare(fi)
    when (fi.resultCode()) {
        0 -> {     // success, company begin/first or too many company
            result.result = 0;
        }
        -1 -> {    /* In completed (吉塚、西小倉における不完全ルート) */
            result.result = 1;     //"この経路の片道乗車券は購入できません.続けて経路を指定してください."
        }
        else -> {
            result.result = -2 /* -2:empty or -3:fail */
            return result
        }
    }


    result.isResultCompanyBeginEnd = fi.isBeginEndCompanyLine()
    result.isResultCompanyMultipassed = fi.isMultiCompanyLine()

    result.isEnableTokaiStockSelect = fi.isEnableTokaiStockSelect()

    /* begin/end terminal */
    result.beginStationId = fi.getBeginTerminalId()
    result.endStationId = fi.getEndTerminalId()

    result.isBeginInCity = FARE_INFO.IsCityId(fi.getBeginTerminalId());
    result.isEndInCity = FARE_INFO.IsCityId(fi.getEndTerminalId())

    /* route */
    result.routeList = fi.getRoute_string()

    /* stock discount (114 no applied) */
    val stks = mutableListOf<Triple<String, Int, Int>>()
    for (i in 0..1) {
        val stk = fi.getFareStockDiscount(i)
        if (0 < stk.fare) {
            stks.add(Triple(stk.title, stk.fare + fi.fareForCompanyline, 0))
        }
    }

    /* rule 114 */
    val rule114Fare = fi.getRule114();
    if (rule114Fare.fare == 0) {
        result.rule114_salesKm = 0;
        result.rule114_calcKm = 0;
        result.isRule114Applied = false;
        result.fareForStockDiscounts = stks
    } else {
        result.isRule114Applied = true;
        result.rule114_salesKm = rule114Fare.sales_km;
        result.rule114_calcKm = rule114Fare.calc_km;

        /* stock discount (114 applied) */
        for (i in 0..1) {
            val stk = fi.getFareStockDiscount(i, true)
            if (0 < stk.fare) {
                stks[i] = Triple(stks[i].first, stks[i].second + fi.fareForCompanyline, stk.fare + fi.fareForCompanyline)
            }
        }
        result.fareForStockDiscounts = stks
    }

    result.isArbanArea = fi.isUrbanArea();


    result.totalSalesKm = fi.getTotalSalesKm();
    result.jrCalcKm = fi.getJRCalcKm();
    result.jrSalesKm = fi.getJRSalesKm();

    /* 会社線有無はtotalSalesKm != jrSalesKm */

    result.companySalesKm = fi.getCompanySalesKm();
    result.salesKmForHokkaido = fi.getSalesKmForHokkaido();
    result.calcKmForHokkaido = fi.getCalcKmForHokkaido();

    result.salesKmForShikoku = fi.getSalesKmForShikoku();
    result.calcKmForShikoku = fi.getCalcKmForShikoku();

    // BRT営業キロ
    result.brtSalesKm = fi.brtSalesKm

    result.salesKmForKyusyu = fi.getSalesKmForKyusyu();
    result.calcKmForKyusyu = fi.getCalcKmForKyusyu();


    // 往復割引有無
    result.isRoundtripDiscount = fi.isRoundTripDiscount();

    // 会社線部分の運賃
    result.fareForCompanyline = fi.getFareForCompanyline();

    // BRT運賃
    result.fareForBRT = fi.fareForBRT

    result.isBRT_discount = fi.isBRT_discount

    // 普通運賃
    result.fare = fi.getFareForDisplay();
    result.farePriorRule114 = fi.getFareForDisplayPriorRule114();

    // 往復
    val fareResult = fi.roundTripFareWithCompanyLine()
    result.roundTripFareWithCompanyLine = fareResult.fare
    if (fi.isRule114()) {
        result.roundTripFareWithCompanyLinePriorRule114 = fi.roundTripFareWithCompanyLinePriorRule114();
    }

    // IC運賃
    result.fareForIC = fi.getFareForIC();

    // 小児運賃
    result.childFare = fi.getChildFareForDisplay();

    // 学割運賃
    result.academicFare = fi.getAcademicDiscountFare()
    if (0 < result.academicFare) {
        result.isAcademicFare = true
        result.roundtripAcademicFare = fi.roundTripAcademicFareWithCompanyLine()
    } else {
        result.isAcademicFare = false
        result.roundtripAcademicFare = 0
    }
    // 有効日数
    result.ticketAvailDays = fi.getTicketAvailDays();

    result.routeListForTOICA = fi.calc_route_for_disp

    result.isMeihanCityStartTerminalEnable = this.route_flag.isMeihanCityEnable
    result.isMeihanCityStart = this.route_flag.isStartAsCity
    result.isMeihanCityTerminal = this.route_flag.isArriveAsCity;

    result.isRuleAppliedEnable = this.route_flag.rule_en()
    result.isRuleApplied = !this.route_flag.no_rule;

    result.isJRCentralStockEnable = this.route_flag.jrtokaistock_enable;
    result.isJRCentralStock = this.route_flag.jrtokaistock_applied;

    result.isEnableLongRoute = this.route_flag.isEnableLongRoute();
    result.isLongRoute = this.route_flag.isLongRoute();
    result.isDisableSpecificTermRule115 = this.route_flag.isDisableSpecificTermRule115();
    result.isEnableRule115 = this.route_flag.isEnableRule115();

    // make message
    if (result.isRuleApplied &&
            fi.isUrbanArea() && !this.route_flag.isUseBullet()) {
        if (fi.getBeginTerminalId() == fi.getEndTerminalId()) {
            result.resultMessage = msgCantMetroTicket
        } else if (!this.route_flag.isEnableRule115() ||
            !this.route_flag.isDisableSpecificTermRule115()) {
            if (this.route_flag.isLongRoute()) {
                result.resultMessage = msgPossibleLowcost
            } else {
                result.resultMessage = msgAppliedLowcost
            }
        } else {
            result.resultMessage = "";
        }

        // 大回り指定では115適用はみない
        if (this.route_flag.isEnableRule115() && !this.route_flag.isEnableLongRoute()) {
            if (this.route_flag.isDisableSpecificTermRule115()) {
                result.resultMessage += msgCanYouNeerestStation
            } else {
                result.resultMessage += msgCanYouSpecificTerm
            }
        }
    }

    result.isSpecificFare = this.route_flag.special_fare_enable // 私鉄競合特例運賃(大都市近郊区間)
    if (result.isSpecificFare) {
        result.resultMessage += "\r\n" + msgSpecificFareApply  // "特定区間割引運賃適用"
    }

    // UI結果オプションメニュー
    result.isFareOptEnabled = result.isRuleAppliedEnable ||
            route_flag.is_osakakan_1pass() ||
            result.isJRCentralStockEnable ||
            result.isEnableRule115 ||
            result.isLongRoute ||
            result.isSpecificFare

    return result
}

// 経路は不揮発メモリに保存されたものか？
fun isStrageInRoute(context: Context, routeScript : String) : Boolean {
    var listItems = readParams(context, ArchiveRouteActivity.KEY_ARCHIVE)
    return listItems.contains(routeScript)
}

fun CalcRoute.showFare() : String {
    val fi = FARE_INFO()
    this.calcFare(fi)
    return fi.showFare(route_flag)
}
