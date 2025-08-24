package org.sutezo.farert.ui.state

import org.sutezo.alps.FareInfo

data class ResultViewUiState(
    val routeEndIndex: Int = -1,
    val alertId: Int = -1,
    val fareInfo: FareInfo? = null,
    val isRoundTrip: Boolean = true,
    
    // Options state
    val optStocktokai: Option = Option.N_A,
    val optNeerest: Option = Option.N_A,
    val optSperule: Option = Option.N_A,
    val optMeihancity: Option = Option.N_A,
    val optLongroute: Option = Option.N_A,
    val optRule115: Option = Option.N_A,
    
    // Menu visibility and text
    val showStocktokaiMenu: Boolean = false,
    val stocktokaiMenuTitle: String = "",
    val showSpecialRuleMenu: Boolean = false,
    val specialRuleMenuTitle: String = "",
    val showMeihanCityMenu: Boolean = false,
    val meihanCityMenuTitle: String = "",
    val showLongRouteMenu: Boolean = false,
    val longRouteMenuTitle: String = "",
    val showRule115Menu: Boolean = false,
    val rule115MenuTitle: String = "",
    val showOsakakanMenu: Boolean = false,
    val osakakanMenuTitle: String = "",
    
    val isLoading: Boolean = false,
    val error: String? = null
) {
    enum class Option {
        N_A,
        FALSE,
        TRUE
    }
    
    enum class OptionItem {
        stocktokai,     // TRUEはJR東海株主優待券を使用する、FALSEは使用しない
        neerest,        // TRUEは適用、FALSEは非適用
        sperule,        // TRUEは非適用、FALSEは特例適用
        meihancity,     // TRUEは着駅を単駅指定、FALSEは発駅を単駅指定
        longroute,      // TRUEは近郊区間遠回り
        rule115,        // TRUEは115条 都区市内発着指定
    }
}