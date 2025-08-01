package org.sutezo.farert.ui.state

sealed interface StationListUiEvent {
    data class StationClicked(val stationId: Int) : StationListUiEvent
    data object SwitchActionClicked : StationListUiEvent
    data object LoadData : StationListUiEvent
    data object ClearError : StationListUiEvent
}