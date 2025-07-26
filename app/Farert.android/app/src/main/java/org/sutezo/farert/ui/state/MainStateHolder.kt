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
                handleToggleOsakaKanDetour(event.farMode)
            }
            
            is MainUiEvent.ClearError -> {
                uiState = uiState.copy(error = null)
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
            val rc = uiState.route.reverse()
            updateFare(rc)
        } catch (e: Exception) {
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
            val rc = uiState.route.setup_route(routeScript)
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
            val rc = uiState.route.setDetour(farMode)
            updateFare(rc)
        } catch (e: Exception) {
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
        
        uiState = uiState.copy(
            fareInfo = fareInfo,
            statusMessage = statusMsg,
            canGoBack = uiState.route.count > 0,
            canReverse = uiState.route.count > 1 && (fareInfo?.let { 
                CalcRoute(uiState.route).isAvailableReverse 
            } ?: false),
            enableFareDetail = uiState.route.count > 1,
            terminalButtonText = terminalText,
            osakakanDetour = osakakanDetour
        )
    }
}