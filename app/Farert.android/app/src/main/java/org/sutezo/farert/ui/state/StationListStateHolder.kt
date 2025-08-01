package org.sutezo.farert.ui.state

import android.content.Context
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.launch
import org.sutezo.alps.*
import org.sutezo.farert.R

class StationListStateHolder : ViewModel() {
    
    var uiState by mutableStateOf(StationListUiState())
        private set

    private lateinit var context: Context

    fun initialize(
        context: Context,
        mode: String,
        lineId: Int,
        srcType: String = "",
        srcCompanyOrPrefectId: Int = 0,
        stationMode: String = "",
        srcStationId: Int = 0,
        startStationId: Int = 0
    ) {
        this.context = context
        uiState = uiState.copy(
            mode = mode,
            lineId = lineId,
            srcType = srcType,
            srcCompanyOrPrefectId = srcCompanyOrPrefectId,
            stationMode = stationMode,
            srcStationId = srcStationId,
            startStationId = startStationId
        )
        handleEvent(StationListUiEvent.LoadData)
    }

    fun handleEvent(event: StationListUiEvent) {
        when (event) {
            is StationListUiEvent.StationClicked -> {
                uiState = uiState.copy(selectedStationId = event.stationId)
                // Station click handling will be done in the Activity through navigation callback
            }
            
            is StationListUiEvent.SwitchActionClicked -> {
                handleSwitchAction()
            }
            
            is StationListUiEvent.LoadData -> {
                loadData()
            }
            
            is StationListUiEvent.ClearError -> {
                uiState = uiState.copy(error = null)
            }
        }
    }
    
    private fun loadData() = viewModelScope.launch {
        try {
            uiState = uiState.copy(isLoading = true)
            
            val stations = if (uiState.mode != "route") {
                // terminal or autoroute
                stationsWithInCompanyOrPrefectAnd(uiState.srcCompanyOrPrefectId, uiState.lineId)
            } else {
                if (uiState.stationMode == "junction") {
                    junctionIdsOfLineId(uiState.lineId, uiState.startStationId)
                } else {
                    stationsIdsOfLineId(uiState.lineId)
                }
            }
            
            val title = when (uiState.mode) {
                "route" -> {
                    if (uiState.stationMode == "junction") {
                        context.getString(R.string.title_station_select_junction)
                    } else {
                        context.getString(R.string.title_station_select_arrive)
                    }
                }
                "autoroute" -> {
                    context.getString(R.string.title_terminal_select_arrive)
                }
                else -> {
                    context.getString(R.string.title_terminal_select_depart)
                }
            }
            
            val subtitle = when (uiState.srcType) {
                "prefect" -> {
                    "${RouteUtil.PrefectName(uiState.srcCompanyOrPrefectId)} - ${RouteUtil.LineName(uiState.lineId)}"
                }
                "company" -> {
                    "${RouteUtil.CompanyName(uiState.srcCompanyOrPrefectId)} - ${RouteUtil.LineName(uiState.lineId)}"
                }
                else -> {
                    RouteUtil.LineName(uiState.lineId)
                }
            }
            
            val (showSwitch, switchTitle) = determineSwitchAction()
            
            uiState = uiState.copy(
                stations = stations,
                title = title,
                subtitle = subtitle,
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
        return when (uiState.title) {
            context.getString(R.string.title_station_select_junction) -> {
                true to context.getString(R.string.title_station_select_arrive)
            }
            context.getString(R.string.title_station_select_arrive) -> {
                true to context.getString(R.string.title_station_select_junction)
            }
            else -> {
                false to ""
            }
        }
    }
    
    private fun handleSwitchAction() {
        // Switch action will be handled in the Activity through navigation callback
        // since it requires recreating the activity with different parameters
    }
    
    fun switchStationMode() = viewModelScope.launch {
        try {
            val newStationMode = if (uiState.stationMode == "junction") "all" else "junction"
            android.util.Log.d("StationListStateHolder", "switchStationMode: ${uiState.stationMode} → $newStationMode")
            
            // 新しいstationModeで駅データを再読み込み
            val stations = if (uiState.mode != "route") {
                // terminal or autoroute
                stationsWithInCompanyOrPrefectAnd(uiState.srcCompanyOrPrefectId, uiState.lineId)
            } else {
                if (newStationMode == "junction") {
                    junctionIdsOfLineId(uiState.lineId, uiState.startStationId)
                } else {
                    stationsIdsOfLineId(uiState.lineId)
                }
            }
            
            val title = when (uiState.mode) {
                "route" -> {
                    if (newStationMode == "junction") {
                        context.getString(R.string.title_station_select_junction)
                    } else {
                        context.getString(R.string.title_station_select_arrive)
                    }
                }
                "autoroute" -> {
                    context.getString(R.string.title_terminal_select_arrive)
                }
                else -> {
                    context.getString(R.string.title_terminal_select_depart)
                }
            }
            
            val (showSwitch, switchTitle) = when (title) {
                context.getString(R.string.title_station_select_junction) -> {
                    true to context.getString(R.string.title_station_select_arrive)
                }
                context.getString(R.string.title_station_select_arrive) -> {
                    true to context.getString(R.string.title_station_select_junction)
                }
                else -> {
                    false to ""
                }
            }
            
            uiState = uiState.copy(
                stationMode = newStationMode,
                stations = stations,
                title = title,
                showSwitchAction = showSwitch,
                switchActionTitle = switchTitle
            )
            android.util.Log.d("StationListStateHolder", "switchStationMode完了: title=$title, stationMode=$newStationMode, stations.size=${stations.size}")
        } catch (e: Exception) {
            uiState = uiState.copy(error = e.message)
        }
    }
}