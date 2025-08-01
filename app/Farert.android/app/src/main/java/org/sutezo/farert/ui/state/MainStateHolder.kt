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
            0 -> "経路は終端に達しています"
            1 -> "" // 正常
            4 -> "会社線利用路線はこれ以上追加できません"
            5 -> "経路は終端に達しています"
            40 -> "開始駅へ戻るにはもう少し経路を指定してからにしてください"
            -200 -> "不正な駅名が含まれています"
            -300 -> "不正な路線名が含まれています"
            -2 -> "経路不正"
            -4 -> "許可されていない会社線通過です"
            -10, -11, -1002 -> "経路を算出できません"
            else -> "経路が重複しているため追加できません"
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
                "発駅を選択してください"
            }
        } else {
            "発駅を選択してください"
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