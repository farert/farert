package org.sutezo.farert.ui.state

import android.content.Context
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.launch
import org.sutezo.alps.*
import org.sutezo.farert.FarertApp
import org.sutezo.farert.R

class ResultViewStateHolder : ViewModel() {
    
    var uiState by mutableStateOf(ResultViewUiState())
        private set

    private lateinit var context: Context
    private var calcRoute: CalcRoute? = null
    private var currentRoute: Route? = null

    fun initialize(
        context: Context,
        routeEndIndex: Int,
        alertId: Int,
        stocktokai: Int = ResultViewUiState.Option.N_A.ordinal,
        neerest: Int = ResultViewUiState.Option.N_A.ordinal,
        sperule: Int = ResultViewUiState.Option.N_A.ordinal,
        meihancity: Int = ResultViewUiState.Option.N_A.ordinal,
        longroute: Int = ResultViewUiState.Option.N_A.ordinal,
        rule115: Int = ResultViewUiState.Option.N_A.ordinal
    ) {
        this.context = context
        
        val opts = ResultViewUiState.Option.values()
        uiState = uiState.copy(
            routeEndIndex = routeEndIndex,
            alertId = alertId,
            optStocktokai = opts[stocktokai],
            optNeerest = opts[neerest],
            optSperule = opts[sperule],
            optMeihancity = opts[meihancity],
            optLongroute = opts[longroute],
            optRule115 = opts[rule115]
        )
        handleEvent(ResultViewUiEvent.LoadData)
    }

    fun handleEvent(event: ResultViewUiEvent) {
        when (event) {
            is ResultViewUiEvent.LoadData -> {
                loadData()
            }
            
            is ResultViewUiEvent.StocktokaiClicked -> {
                handleStocktokaiClick()
            }
            
            is ResultViewUiEvent.SpecialRuleClicked -> {
                handleSpecialRuleClick()
            }
            
            is ResultViewUiEvent.MeihanCityClicked -> {
                handleMeihanCityClick()
            }
            
            is ResultViewUiEvent.LongRouteClicked -> {
                handleLongRouteClick()
            }
            
            is ResultViewUiEvent.Rule115Clicked -> {
                handleRule115Click()
            }
            
            is ResultViewUiEvent.OsakakanClicked -> {
                handleOsakakanClick()
            }
            
            is ResultViewUiEvent.ShareClicked -> {
                // Share handling will be done in the Activity
            }
            
            is ResultViewUiEvent.ClearError -> {
                uiState = uiState.copy(error = null)
            }
        }
    }
    
    private fun loadData() = viewModelScope.launch {
        try {
            uiState = uiState.copy(isLoading = true)
            
            val app = context.applicationContext as? FarertApp
            val appDs = app?.ds ?: return@launch
            
            val ds = Route()
            ds.assign(appDs, uiState.routeEndIndex)
            
            if (ds.count <= 0) {
                uiState = uiState.copy(error = "No route data available", isLoading = false)
                return@launch
            }
            
            val isRoundTrip = ds.isAvailableReverse
            setRouteOption(ds)
            
            currentRoute = ds
            calcRoute = CalcRoute(ds)
            val fareInfo = calcRoute?.calcFareInfo()
            
            // Update UI state first
            uiState = uiState.copy(
                fareInfo = fareInfo,
                isRoundTrip = isRoundTrip,
                isLoading = false
            )
            
            if (fareInfo != null) {
                setOptionFlag(fareInfo)
                updateMenuState()
            }
        } catch (e: Exception) {
            uiState = uiState.copy(
                error = e.message,
                isLoading = false
            )
        }
    }
    
    private fun setRouteOption(ds: Route) {
        // Apply route options based on current state
        if (uiState.optSperule == ResultViewUiState.Option.TRUE) {
            ds.setNoRule(true)
        } else if (uiState.optSperule == ResultViewUiState.Option.FALSE) {
            ds.setNoRule(false)
        }
        
        // stocktokai: TRUE = JR東海株主優待券を使用する、FALSE = 使用しない
        if (uiState.optStocktokai == ResultViewUiState.Option.TRUE) {
            ds.routeFlag.setJrTokaiStockApply(true)
        } else if (uiState.optStocktokai == ResultViewUiState.Option.FALSE) {
            ds.routeFlag.setJrTokaiStockApply(false)
        }
        
        // meihancity: TRUE = 着駅を単駅指定、FALSE = 発駅を単駅指定
        if (uiState.optMeihancity == ResultViewUiState.Option.TRUE) {
            ds.routeFlag.setArriveAsCity()
        } else if (uiState.optMeihancity == ResultViewUiState.Option.FALSE) {
            ds.routeFlag.setStartAsCity()
        }
        
        if (uiState.optLongroute == ResultViewUiState.Option.FALSE) {
            ds.routeFlag.isLongRoute = false
        } else if (uiState.optLongroute == ResultViewUiState.Option.TRUE) {
            ds.routeFlag.isLongRoute = true
        }
        
        if (uiState.optRule115 == ResultViewUiState.Option.FALSE) {
            ds.routeFlag.setSpecificTermRule115(false)
        } else if (uiState.optRule115 == ResultViewUiState.Option.TRUE) {
            ds.routeFlag.setSpecificTermRule115(true)
        }
        
        // 単駅最安オプション処理
        if (uiState.optNeerest == ResultViewUiState.Option.TRUE) {
            ds.routeFlag.setSpecificTermRule115(true)
        } else if (uiState.optNeerest == ResultViewUiState.Option.FALSE) {
            ds.routeFlag.setSpecificTermRule115(false)
        }
    }
    
    private fun setOptionFlag(fareInfo: FareInfo) {
        val optSperule = if (fareInfo.isRuleAppliedEnable) {
            if (fareInfo.isRuleApplied) {
                ResultViewUiState.Option.FALSE
            } else {
                ResultViewUiState.Option.TRUE
            }
        } else {
            ResultViewUiState.Option.N_A
        }
        
        val optStocktokai = if (fareInfo.isJRCentralStockEnable) {
            if (fareInfo.isJRCentralStock) {
                ResultViewUiState.Option.TRUE
            } else {
                ResultViewUiState.Option.FALSE
            }
        } else {
            ResultViewUiState.Option.N_A
        }
        
        val optMeihancity = if (fareInfo.isMeihanCityStartTerminalEnable) {
            if (fareInfo.isMeihanCityStart) {
                ResultViewUiState.Option.FALSE
            } else {
                ResultViewUiState.Option.TRUE
            }
        } else {
            ResultViewUiState.Option.N_A
        }
        
        val optLongroute = if (fareInfo.isEnableLongRoute) {
            if (fareInfo.isLongRoute) {
                ResultViewUiState.Option.TRUE
            } else {
                ResultViewUiState.Option.FALSE
            }
        } else {
            ResultViewUiState.Option.N_A
        }
        
        val optRule115 = if (fareInfo.isEnableRule115) {
            if (fareInfo.isRule115specificTerm) {
                ResultViewUiState.Option.TRUE
            } else {
                ResultViewUiState.Option.FALSE
            }
        } else {
            ResultViewUiState.Option.N_A
        }
        
        // 単駅最安オプション
        val optNeerest = if (fareInfo.isEnableRule115) {
            if (fareInfo.isRule115specificTerm) {
                ResultViewUiState.Option.TRUE
            } else {
                ResultViewUiState.Option.FALSE
            }
        } else {
            ResultViewUiState.Option.N_A
        }
        
        uiState = uiState.copy(
            optSperule = optSperule,
            optStocktokai = optStocktokai,
            optMeihancity = optMeihancity,
            optLongroute = optLongroute,
            optRule115 = optRule115,
            optNeerest = optNeerest
        )
    }
    
    private fun updateMenuState() {
        val showSpecialRule = uiState.optSperule != ResultViewUiState.Option.N_A
        val specialRuleTitle = when (uiState.optSperule) {
            ResultViewUiState.Option.TRUE -> {
                "${context.getString(R.string.result_menu_sperule)}${context.getString(R.string.action)}"
            }
            ResultViewUiState.Option.FALSE -> {
                "${context.getString(R.string.result_menu_sperule)}${context.getString(R.string.nothing)}"
            }
            else -> ""
        }
        
        val showStocktokai = uiState.optStocktokai != ResultViewUiState.Option.N_A
        val stocktokaiTitle = when (uiState.optStocktokai) {
            ResultViewUiState.Option.TRUE -> {
                "JR東海株主優待券を適用"
            }
            ResultViewUiState.Option.FALSE -> {
                "JR東海株主優待券を適用しない"
            }
            else -> ""
        }
        
        val showMeihanCity = uiState.optMeihancity != ResultViewUiState.Option.N_A
        val meihanCityTitle = when (uiState.optMeihancity) {
            ResultViewUiState.Option.TRUE -> {
                "着駅を単駅に"
            }
            ResultViewUiState.Option.FALSE -> {
                "発駅を単駅に"
            }
            else -> ""
        }
        
        val showLongRoute = uiState.optLongroute != ResultViewUiState.Option.N_A
        val longRouteTitle = when (uiState.optLongroute) {
            ResultViewUiState.Option.TRUE -> {
                context.getString(R.string.result_menu_longroute_lowcost)
            }
            ResultViewUiState.Option.FALSE -> {
                context.getString(R.string.result_menu_longroute_long)
            }
            else -> ""
        }
        
        val showRule115 = uiState.optRule115 != ResultViewUiState.Option.N_A
        val rule115Title = when (uiState.optRule115) {
            ResultViewUiState.Option.TRUE -> {
                context.getString(R.string.result_menu_rule115_station)
            }
            ResultViewUiState.Option.FALSE -> {
                context.getString(R.string.result_menu_rule115_city)
            }
            else -> ""
        }
        
        // Osaka Kanjou Line menu setup - use FareInfo instead of direct Route access
        val fareInfo = uiState.fareInfo
        val showOsakakan = fareInfo?.isOsakakanDetourEnable ?: false
        val osakakanTitle = if (showOsakakan) {
            if (fareInfo?.isOsakakanDetour == true) {
                context.getString(R.string.result_menu_osakakan_near)
            } else {
                context.getString(R.string.result_menu_osakakan_far)
            }
        } else ""
        
        
        uiState = uiState.copy(
            showSpecialRuleMenu = showSpecialRule,
            specialRuleMenuTitle = specialRuleTitle,
            showStocktokaiMenu = showStocktokai,
            stocktokaiMenuTitle = stocktokaiTitle,
            showMeihanCityMenu = showMeihanCity,
            meihanCityMenuTitle = meihanCityTitle,
            showLongRouteMenu = showLongRoute,
            longRouteMenuTitle = longRouteTitle,
            showRule115Menu = showRule115,
            rule115MenuTitle = rule115Title,
            showOsakakanMenu = showOsakakan,
            osakakanMenuTitle = osakakanTitle
        )
    }
    
    private fun recalculateWithNewOptions() = viewModelScope.launch {
        try {
            uiState = uiState.copy(isLoading = true)
            
            val app = context.applicationContext as? FarertApp
            val appDs = app?.ds ?: return@launch
            
            val ds = Route()
            ds.assign(appDs, uiState.routeEndIndex)
            
            val isRoundTrip = ds.isAvailableReverse
            setRouteOption(ds)
            
            currentRoute = ds
            calcRoute = CalcRoute(ds)
            val fareInfo = calcRoute?.calcFareInfo()
            
            // Update UI state first
            uiState = uiState.copy(
                fareInfo = fareInfo,
                isRoundTrip = isRoundTrip,
                isLoading = false
            )
            
            if (fareInfo != null) {
                setOptionFlag(fareInfo)
                updateMenuState()
            }
        } catch (e: Exception) {
            uiState = uiState.copy(
                error = e.message,
                isLoading = false
            )
        }
    }
    
    private fun handleSpecialRuleClick() {
        val newOpt = if (uiState.specialRuleMenuTitle.contains(context.getString(R.string.nothing))) {
            ResultViewUiState.Option.TRUE
        } else {
            ResultViewUiState.Option.FALSE
        }
        
        uiState = uiState.copy(optSperule = newOpt)
        // Trigger recalculation
        recalculateWithNewOptions()
    }

    
    private fun handleStocktokaiClick() {
        val newOpt = if (uiState.stocktokaiMenuTitle == "JR東海株主優待券を適用しない") {
            ResultViewUiState.Option.TRUE
        } else {
            ResultViewUiState.Option.FALSE
        }
        
        uiState = uiState.copy(optStocktokai = newOpt)
        recalculateWithNewOptions()
    }

    
    
    
    private fun handleMeihanCityClick() {
        val newOpt = if (uiState.meihanCityMenuTitle == "発駅を単駅に") {
            ResultViewUiState.Option.TRUE
        } else {
            ResultViewUiState.Option.FALSE
        }
        
        uiState = uiState.copy(optMeihancity = newOpt)
        recalculateWithNewOptions()
    }
    
    private fun handleLongRouteClick() {
        val newOpt = if (uiState.longRouteMenuTitle == context.getString(R.string.result_menu_longroute_long)) {
            ResultViewUiState.Option.TRUE
        } else {
            ResultViewUiState.Option.FALSE
        }
        
        uiState = uiState.copy(optLongroute = newOpt)
        recalculateWithNewOptions()
    }
    
    private fun handleRule115Click() {
        val newOpt = if (uiState.rule115MenuTitle == context.getString(R.string.result_menu_rule115_city)) {
            ResultViewUiState.Option.TRUE
        } else {
            ResultViewUiState.Option.FALSE
        }
        
        uiState = uiState.copy(optRule115 = newOpt)
        recalculateWithNewOptions()
    }
    
    private fun handleOsakakanClick() = viewModelScope.launch {
        try {
            // Toggle Osaka Kanjou Line detour setting using FareInfo state
            val fareInfo = uiState.fareInfo
            val currentDetour = fareInfo?.isOsakakanDetour ?: false
            val newDetour = !currentDetour
            
            uiState = uiState.copy(isLoading = true)
            
            // Apply detour setting to current route directly
            currentRoute?.setDetour(newDetour)
            
            // Also update the global route in FarertApp for consistency
            val app = context.applicationContext as? org.sutezo.farert.FarertApp
            app?.ds?.setDetour(newDetour)
            
            // Recalculate with the updated route
            calcRoute = CalcRoute(currentRoute ?: return@launch)
            val newFareInfo = calcRoute?.calcFareInfo()
            
            // Update UI state first
            uiState = uiState.copy(
                fareInfo = newFareInfo,
                isLoading = false
            )
            
            if (newFareInfo != null) {
                setOptionFlag(newFareInfo)
                updateMenuState()
            }
        } catch (e: Exception) {
            uiState = uiState.copy(
                error = e.message,
                isLoading = false
            )
        }
    }
    
    fun getShareData(): Pair<String, String>? {
        val fareInfo = uiState.fareInfo ?: return null
        val calcRoute = this.calcRoute ?: return null
        
        val subject = "${context.getString(R.string.result_mailtitle_faredetail)}(${terminalName(fareInfo.beginStationId)} - ${terminalName(fareInfo.endStationId)})"
        
        var body = "$subject\n\n${calcRoute.showFare()}\n"
        body = body.replace('\\', '¥')
        
        if (fareInfo.isRuleAppliedEnable) {
            body += if (fareInfo.isRuleApplied) {
                "(特例適用)\n"
            } else {
                "(特例未適用)\n"
            }
        }
        body += "\n[指定経路]\n"
        body += calcRoute.route_script()
        
        return Pair(subject, body)
    }
}