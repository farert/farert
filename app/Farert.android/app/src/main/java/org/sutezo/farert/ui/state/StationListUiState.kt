package org.sutezo.farert.ui.state

data class StationListUiState(
    val mode: String = "",
    val lineId: Int = 0,
    val srcType: String = "", // prefect|company
    val srcCompanyOrPrefectId: Int = 0,
    val stationMode: String = "", // junction|all
    val srcStationId: Int = 0,
    val startStationId: Int = 0,
    
    val stations: List<Int> = emptyList(),
    val selectedStationId: Int = 0,
    
    val title: String = "",
    val subtitle: String = "",
    val showSwitchAction: Boolean = false,
    val switchActionTitle: String = "",
    
    val isLoading: Boolean = false,
    val error: String? = null
) {
    enum class Mode {
        TERMINAL,
        AUTOROUTE,
        ROUTE
    }
    
    enum class StationMode {
        JUNCTION,
        ALL
    }
    
    enum class SrcType {
        PREFECT,
        COMPANY
    }
}