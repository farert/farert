package org.sutezo.farert.ui.state

sealed interface LineListUiEvent {
    data class LineClicked(val lineId: Int) : LineListUiEvent
    data object SwitchActionClicked : LineListUiEvent
    data object LoadData : LineListUiEvent
    data object ClearError : LineListUiEvent
}