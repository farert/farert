package org.sutezo.farert.ui.state

data class TerminalSelectUiState(
    val mode: String = "",
    val selectedTab: Int = 0,
    val searchQuery: String = "",
    val isSearching: Boolean = false,
    val companyList: List<Int> = emptyList(),
    val prefectList: List<Int> = emptyList(),
    val historyList: List<Int> = emptyList(),
    val searchResults: List<Int> = emptyList(),
    val searchResultCount: Int = 0,
    val canClearHistory: Boolean = false,
    val isLoading: Boolean = false,
    val error: String? = null
) {
    enum class TabType(val index: Int) {
        COMPANY(0),
        PREFECT(1), 
        HISTORY(2)
    }
    
    enum class ListType {
        COMPANY,
        PREFECT,
        HISTORY,
        SEARCH
    }
}