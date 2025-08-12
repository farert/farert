package org.sutezo.farert.ui.state

import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.launch
import org.sutezo.alps.*
import org.sutezo.farert.R

class MainStateHolder : ViewModel() {
    
    var uiState by mutableStateOf(MainUiState())
        private set
    
    // Callback for notifying MainActivity when route changes
    var onRouteChanged: ((Route) -> Unit)? = null

    fun handleEvent(event: MainUiEvent) {
        when (event) {
            is MainUiEvent.SelectTerminal -> {
                // Handle terminal selection - this will be implemented with navigation
            }
            
            is MainUiEvent.ShowFareDetail -> {
                // Handle fare detail display - this will be implemented with navigation  
            }
            
            is MainUiEvent.GoBack -> {
                handleGoBack()
            }
            
            is MainUiEvent.ReverseRoute -> {
                handleReverseRoute()
            }
            
            is MainUiEvent.ArchiveRoute -> {
                // Handle archive route - this will be implemented with navigation
            }
            
            is MainUiEvent.RouteItemClick -> {
                handleRouteItemClick(event.position)
            }
            
            is MainUiEvent.AddRoute -> {
                handleAddRoute(event.lineId, event.stationId)
            }
            
            is MainUiEvent.SetupRoute -> {
                handleSetupRoute(event.routeScript) 
            }
            
            is MainUiEvent.BeginRoute -> {
                handleBeginRoute(event.stationId)
            }
            
            is MainUiEvent.AutoRoute -> {
                handleAutoRoute(event.stationId, event.mode)
            }
            
            is MainUiEvent.ToggleOsakaKanDetour -> {
                println("DEBUG: MainStateHolder - ToggleOsakaKanDetour event received")
                handleToggleOsakaKanDetour(event.farMode)
            }
            
            is MainUiEvent.ClearError -> {
                uiState = uiState.copy(error = null)
            }
            
            is MainUiEvent.RequestRouteChange -> {
                handleRequestRouteChange(event.routeScript)
            }
            
            is MainUiEvent.ConfirmRouteChange -> {
                handleConfirmRouteChange()
            }
            
            is MainUiEvent.CancelRouteChange -> {
                handleCancelRouteChange()
            }
        }
    }
    
    private fun handleGoBack() = viewModelScope.launch {
        try {
            if (uiState.route.count > 1) {
                uiState.route.removeTail()
            } else {
                uiState.route.removeAll()
            }
            updateFare(1)
        } catch (e: Exception) {
            uiState = uiState.copy(error = e.message)
        }
    }
    
    private fun handleReverseRoute() = viewModelScope.launch {
        try {
            println("DEBUG: handleReverseRoute called - before reverse: route.count=${uiState.route.count}")
            val rc = uiState.route.reverse()
            println("DEBUG: handleReverseRoute - reverse() returned: $rc")
            updateFare(rc)
            println("DEBUG: handleReverseRoute completed - after reverse: route.count=${uiState.route.count}")
        } catch (e: Exception) {
            println("DEBUG: handleReverseRoute error: ${e.message}")
            uiState = uiState.copy(error = e.message)
        }
    }
    
    private fun handleRouteItemClick(position: Int) {
        // This will be handled by navigation in the Activity
        // For now, we just update the state to indicate which item was clicked
    }
    
    private fun handleAddRoute(lineId: Int, stationId: Int) = viewModelScope.launch {
        try {
            val rc = uiState.route.add(lineId, stationId)
            updateFare(rc)
        } catch (e: Exception) {
            uiState = uiState.copy(error = e.message)
        }
    }
    
    private fun handleSetupRoute(routeScript: String) = viewModelScope.launch {
        try {
            // Clear the current route completely
            uiState.route.removeAll()
            
            // Reset UI state first to clear any existing display
            uiState = uiState.copy(
                route = uiState.route,
                fareInfo = null,
                statusMessage = "",
                canGoBack = false,
                canReverse = false,
                enableFareDetail = false,
                terminalButtonText = "発駅を選択してください"
            )
            
            // Setup the new route
            val rc = uiState.route.setup_route(routeScript)
            
            // Create a new route object to ensure state change detection
            val newRoute = Route()
            val routeScript2 = uiState.route.route_script()
            newRoute.setup_route(routeScript2)
            
            // Force UI state update with new route object
            uiState = uiState.copy(
                route = newRoute
            )
            
            // Update the UI with the new route data
            updateFare(rc)
        } catch (e: Exception) {
            uiState = uiState.copy(error = e.message)
        }
    }
    
    private fun handleBeginRoute(stationId: Int) = viewModelScope.launch {
        try {
            uiState.route.removeAll()
            uiState.route.add(stationId)
            updateFare(1)
        } catch (e: Exception) {
            uiState = uiState.copy(error = e.message)
        }
    }
    
    private fun handleAutoRoute(stationId: Int, mode: Int) = viewModelScope.launch {
        try {
            val rc = uiState.route.changeNeerest(mode, stationId)
            updateFare(if (rc == 4) 40 else rc)
        } catch (e: Exception) {
            uiState = uiState.copy(error = e.message)
        }
    }
    
    private fun handleToggleOsakaKanDetour(farMode: Boolean) = viewModelScope.launch {
        try {
            // Toggle between far and near based on current state
            val currentDetour = uiState.route.isOsakakanDetour
            val newDetour = !currentDetour  // Toggle current state
            
            println("DEBUG: toggleOsakakanDetour - current: $currentDetour, new: $newDetour")
            val rc = uiState.route.setDetour(newDetour)
            updateFare(rc)
        } catch (e: Exception) {
            println("DEBUG: toggleOsakakanDetour error: ${e.message}")
            uiState = uiState.copy(error = e.message)
        }
    }
    
    fun initializeRoute(route: Route) {
        try {
            uiState = uiState.copy(route = route)
            updateFare(1)
        } catch (e: Exception) {
            uiState = uiState.copy(
                route = route,
                error = "初期化エラー: ${e.message}",
                terminalButtonText = "発駅を選択してください"
            )
        }
    }
    
    fun updateRoute(route: Route) {
        try {
            uiState = uiState.copy(route = route)
            updateFare(1)
        } catch (e: Exception) {
            uiState = uiState.copy(
                route = route,
                error = "ルート更新エラー: ${e.message}"
            )
        }
    }
    
    private fun updateFare(rc: Int) {
        val statusMsg = when (rc) {
            0 -> "\u7d4c\u8def\u306f\u7d42\u7aef\u306b\u9054\u3057\u3066\u3044\u307e\u3059"
            1 -> "" // \u6b63\u5e38
            4 -> "\u4f1a\u793e\u7dda\u5229\u7528\u8def\u7dda\u306f\u3053\u308c\u4ee5\u4e0a\u8ffd\u52a0\u3067\u304d\u307e\u305b\u3093"
            5 -> "\u7d4c\u8def\u306f\u7d42\u7aef\u306b\u9054\u3057\u3066\u3044\u307e\u3059"
            40 -> "\u958b\u59cb\u99c5\u3078\u623b\u308b\u306b\u306f\u3082\u3046\u5c11\u3057\u7d4c\u8def\u3092\u6307\u5b9a\u3057\u3066\u304b\u3089\u306b\u3057\u3066\u304f\u3060\u3055\u3044"
            -200 -> "\u4e0d\u6b63\u306a\u99c5\u540d\u304c\u542b\u307e\u308c\u3066\u3044\u307e\u3059"
            -300 -> "\u4e0d\u6b63\u306a\u8def\u7dda\u540d\u304c\u542b\u307e\u308c\u3066\u3044\u307e\u3059"
            -2 -> "\u7d4c\u8def\u4e0d\u6b63"
            -4 -> "\u8a31\u53ef\u3055\u308c\u3066\u3044\u306a\u3044\u4f1a\u793e\u7dda\u901a\u904e\u3067\u3059"
            -10, -11, -1002 -> "\u7d4c\u8def\u3092\u7b97\u51fa\u3067\u304d\u307e\u305b\u3093"
            else -> "\u7d4c\u8def\u304c\u91cd\u8907\u3057\u3066\u3044\u308b\u305f\u3081\u8ffd\u52a0\u3067\u304d\u307e\u305b\u3093"
        }
        
        val fareInfo = if (uiState.route.count > 1) {
            try {
                val cr = CalcRoute(uiState.route)
                cr.calcFareInfo()
            } catch (e: Exception) {
                null
            }
        } else {
            null
        }
        
        val terminalText = if (uiState.route.count > 0) {
            try {
                val stationId = uiState.route.item(0).stationId()
                "${RouteUtil.StationNameEx(stationId)} ${RouteUtil.GetKanaFromStationId(stationId)}"
            } catch (e: Exception) {
                "\u767a\u99c5\u3092\u9078\u629e\u3057\u3066\u304f\u3060\u3055\u3044"
            }
        } else {
            "\u767a\u99c5\u3092\u9078\u629e\u3057\u3066\u304f\u3060\u3055\u3044"
        }
        
        val osakakanDetour = if (uiState.route.count > 1 && uiState.route.isOsakakanDetourEnable) {
            if (uiState.route.isOsakakanDetour) {
                MainUiState.OsakaKanDetour.FAR
            } else {
                MainUiState.OsakaKanDetour.NEAR
            }
        } else {
            MainUiState.OsakaKanDetour.DISABLE
        }
        
        println("DEBUG: MainStateHolder.updateFare - route.count=${uiState.route.count}")
        println("DEBUG: MainStateHolder.updateFare - isOsakakanDetourEnable=${uiState.route.isOsakakanDetourEnable}")
        println("DEBUG: MainStateHolder.updateFare - isOsakakanDetour=${uiState.route.isOsakakanDetour}")
        println("DEBUG: MainStateHolder.updateFare - osakakanDetour=$osakakanDetour")
        println("DEBUG: MainStateHolder.updateFare - fareInfo=${fareInfo != null}")
        
        uiState = uiState.copy(
            route = Route(uiState.route), // Create new Route object to trigger recomposition
            fareInfo = fareInfo,
            statusMessage = statusMsg,
            canGoBack = uiState.route.count > 0,
            canReverse = uiState.route.count > 1 && (fareInfo?.let { 
                val result = CalcRoute(uiState.route).isAvailableReverse
                println("DEBUG: canReverse - route.count=${uiState.route.count}, fareInfo!=null=${fareInfo != null}, isAvailableReverse=$result")
                result
            } ?: run {
                println("DEBUG: canReverse - route.count=${uiState.route.count}, fareInfo=null")
                false
            }),
            enableFareDetail = uiState.route.count > 1,
            terminalButtonText = terminalText,
            osakakanDetour = osakakanDetour
        )
        
        // Notify MainActivity of route changes (for legacy compatibility)
        onRouteChanged?.invoke(uiState.route)
    }
    
    fun updateStatusMessage(message: String) {
        uiState = uiState.copy(statusMessage = message)
    }
    
    private fun handleRequestRouteChange(routeScript: String) {
        // Check if current route exists and is different from requested route
        if (uiState.route.count > 1 && uiState.route.route_script() != routeScript) {
            // Show confirmation dialog
            uiState = uiState.copy(
                showRouteConfirmDialog = true,
                pendingRouteScript = routeScript
            )
        } else {
            // Directly apply the route
            handleSetupRoute(routeScript)
        }
    }
    
    private fun handleConfirmRouteChange() {
        uiState.pendingRouteScript?.let { routeScript ->
            handleSetupRoute(routeScript)
        }
        uiState = uiState.copy(
            showRouteConfirmDialog = false,
            pendingRouteScript = null
        )
    }
    
    private fun handleCancelRouteChange() {
        uiState = uiState.copy(
            showRouteConfirmDialog = false,
            pendingRouteScript = null
        )
    }
}