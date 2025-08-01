package org.sutezo.farert.ui.state

import android.content.Context
import android.content.Intent
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.launch
import org.sutezo.alps.*
import org.sutezo.farert.R

class LineListStateHolder : ViewModel() {
    
    var uiState by mutableStateOf(LineListUiState())
        private set

    private lateinit var context: Context

    fun initialize(
        context: Context,
        mode: String,
        ident: Int = 0,
        idType: Int = 0,
        srcStationId: Int = 0,
        startStationId: Int = 0,
        srcLineId: Int = 0,
        isTwoPane: Boolean = false
    ) {
        this.context = context
        uiState = uiState.copy(
            mode = mode,
            ident = ident,
            idType = idType,
            srcStationId = srcStationId,
            startStationId = startStationId,
            srcLineId = srcLineId,
            isTwoPane = isTwoPane
        )
        handleEvent(LineListUiEvent.LoadData)
    }

    fun handleEvent(event: LineListUiEvent) {
        when (event) {
            is LineListUiEvent.LineClicked -> {
                uiState = uiState.copy(selectedLineId = event.lineId)
                // Line click handling will be done in the Activity through navigation callback
            }
            
            is LineListUiEvent.SwitchActionClicked -> {
                handleSwitchAction()
            }
            
            is LineListUiEvent.LoadData -> {
                loadData()
            }
            
            is LineListUiEvent.ClearError -> {
                uiState = uiState.copy(error = null)
            }
        }
    }
    
    private fun loadData() = viewModelScope.launch {
        try {
            uiState = uiState.copy(isLoading = true)
            
            val (lines, subtitle) = when (uiState.idType) {
                1 -> { // company
                    val companyName = RouteUtil.CompanyName(uiState.ident)
                    val linesList = linesFromCompanyOrPrefect(uiState.ident)
                    linesList to companyName
                }
                2 -> { // prefect
                    val prefectName = RouteUtil.PrefectName(uiState.ident)
                    val linesList = linesFromCompanyOrPrefect(uiState.ident)
                    linesList to prefectName
                }
                0 -> { // station
                    val stationName = RouteUtil.StationName(uiState.srcStationId)
                    val linesList = lineIdsFromStation(uiState.srcStationId)
                    linesList to stationName
                }
                else -> {
                    emptyList<Int>() to ""
                }
            }
            
            val title = context.getString(R.string.title_linelistactivity)
            
            val (showSwitch, switchTitle) = determineSwitchAction()
            
            uiState = uiState.copy(
                lines = lines,
                subtitle = subtitle,
                title = title,
                showSwitchAction = showSwitch,
                switchActionTitle = switchTitle,
                isLoading = false
            )
        } catch (e: Exception) {
            uiState = uiState.copy(
                error = e.message,
                isLoading = false
            )
        }
    }
    
    private fun determineSwitchAction(): Pair<Boolean, String> {
        return when {
            uiState.stationSelMode == 1 -> {
                // junction mode
                true to context.getString(R.string.title_station_select_arrive)
            }
            uiState.stationSelMode == 2 -> {
                // terminal mode  
                true to context.getString(R.string.title_station_select_junction)
            }
            uiState.mode == "autoroute" || uiState.mode == "terminal" -> {
                false to ""
            }
            else -> {
                false to ""
            }
        }
    }
    
    private fun handleSwitchAction() {
        when (uiState.stationSelMode) {
            0 -> {
                // Switch to autoroute mode - will be handled by navigation callback
            }
            else -> {
                // Switch between junction and terminal modes
                uiState = uiState.copy(
                    stationSelMode = if (uiState.stationSelMode == 1) 2 else 1
                )
                val (showSwitch, switchTitle) = determineSwitchAction()
                uiState = uiState.copy(
                    showSwitchAction = showSwitch,
                    switchActionTitle = switchTitle
                )
            }
        }
    }
    
    fun updateStationSelMode(mode: Int) {
        uiState = uiState.copy(stationSelMode = mode)
        val (showSwitch, switchTitle) = determineSwitchAction()
        uiState = uiState.copy(
            showSwitchAction = showSwitch,
            switchActionTitle = switchTitle
        )
    }
}