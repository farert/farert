package org.sutezo.alps

import org.sutezo.alps.farertAssert.*

class FareInfo {
    // 株主割引
    var fareForStockDiscounts : List<Triple<String, Int, Int>>? = null
    var result : Int = 0

    var resultState : Int = 0
// 1 会社線で始まっている
// 2 会社線で終わっている
// 3 会社線のみ
// 4 会社線２回以上通過
// 8 不完全経路（未使用というか前で弾く）
    var isResultCompanyBeginEnd : Boolean = false
    var isResultCompanyMultipassed: Boolean = false

    var isEnableTokaiStockSelect : Boolean = false

    var beginStationId: Int = 0
    var endStationId : Int = 0

    var isBeginInCity : Boolean = false
    var isEndInCity : Boolean = false

    var rule114_salesKm : Int = 0
    var rule114_calcKm : Int = 0
    var isRule114Applied : Boolean = false

    var isSpecificFare : Boolean = false

    var totalSalesKm : Int = 0
    var jrCalcKm : Int = 0
    var jrSalesKm : Int = 0
/* 会社線有無はtotalSalesKm != jrSalesKm */

    var companySalesKm : Int = 0
    var salesKmForHokkaido : Int = 0
    var calcKmForHokkaido : Int = 0

    var brtSalesKm : Int = 0

    var salesKmForShikoku : Int = 0
    var calcKmForShikoku : Int = 0


    var salesKmForKyusyu : Int = 0
    var calcKmForKyusyu : Int = 0


// 往復割引有無
    var isRoundtripDiscount : Boolean = false

// 会社線部分の運賃
    var fareForCompanyline: Int = 0

// BRT運賃
    var fareForBRT : Int = 0
    var isBRT_discount : Boolean = false

// 普通運賃
    var fare : Int = 0
    var farePriorRule114 : Int = 0

// 普通運賃は, fareForJR + fareForCompanyline;
// 往復
    var roundTripFareWithCompanyLine : Int = 0
    var roundTripFareWithCompanyLinePriorRule114 : Int = 0

// IC運賃
    var fareForIC : Int = 0

    // 子供運賃
    var childFare: Int = 0
    var roundtripChildFare : Int = 0

// 学割運賃
    var isAcademicFare: Boolean = false
    var academicFare : Int = 0
    var roundtripAcademicFare : Int = 0

// 有効日数
    var ticketAvailDays: Int = 0

// 経路
    var routeList : String = ""
    var routeListForTOICA : String = "" // TOICA IC運賃計算経路

    var isEnableLongRoute : Boolean = false
    var isLongRoute : Boolean = false
    var isRule115specificTerm : Boolean = false
    var isEnableRule115 : Boolean = false

    var resultMessage : String = ""

    var isMeihanCityStartTerminalEnable : Boolean = false
    var isMeihanCityStart : Boolean = false
    var isMeihanCityTerminal : Boolean = false

    var isRuleAppliedEnable: Boolean = false
    var isRuleApplied: Boolean = false
    var isFareOptEnabled : Boolean = false

    var isOsakakanDetour : Boolean = false
    var isOsakakanDetourEnable : Boolean = false

// bit 8-9
    //fun isJRCentralStockEnable(): Boolean = (calcResultFlag and 0x300) != 0x000
    var isJRCentralStockEnable: Boolean = false
    var isJRCentralStock : Boolean = false
}