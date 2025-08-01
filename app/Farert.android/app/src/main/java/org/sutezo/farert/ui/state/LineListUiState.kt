package org.sutezo.farert.ui.state

data class LineListUiState(
    val mode: String = "",
    val lines: List<Int> = emptyList(),
    val selectedLineId: Int = 0,
    val subtitle: String = "",
    val title: String = "",
    val isTwoPane: Boolean = false,
    val stationSelMode: Int = 0, // 0=none, 1=junction, 2=terminal
    val showSwitchAction: Boolean = false,
    val switchActionTitle: String = "",
    
    // Parameters from intent
    val ident: Int = 0, // company_id or prefect_id
    val idType: Int = 0, // 1=company, 2=prefect, 0=station
    val srcStationId: Int = 0,
    val startStationId: Int = 0,
    val srcLineId: Int = 0,
    
    val isLoading: Boolean = false,
    val error: String? = null
) {
    enum class Mode {
        ROUTE,
        TERMINAL,
        AUTOROUTE
    }
    
    enum class IdType(val value: Int) {
        STATION(0),
        COMPANY(1), 
        PREFECT(2)
    }
}