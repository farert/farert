package org.sutezo.farert.ui.state

sealed interface TerminalSelectUiEvent {
    data class TabSelected(val tabIndex: Int) : TerminalSelectUiEvent
    data class SearchQueryChanged(val query: String) : TerminalSelectUiEvent
    data object SearchStarted : TerminalSelectUiEvent
    data object SearchClosed : TerminalSelectUiEvent
    data class ItemClicked(val itemId: Int, val listType: TerminalSelectUiState.ListType) : TerminalSelectUiEvent
    data class HistoryItemRemoved(val position: Int) : TerminalSelectUiEvent
    data object ClearAllHistory : TerminalSelectUiEvent
    data object LoadData : TerminalSelectUiEvent
    data object ClearError : TerminalSelectUiEvent
}