package org.sutezo.farert.ui.state

sealed interface MainUiEvent {
    data object SelectTerminal : MainUiEvent
    data object ShowFareDetail : MainUiEvent
    data object GoBack : MainUiEvent
    data object ReverseRoute : MainUiEvent
    data object ArchiveRoute : MainUiEvent
    data class RouteItemClick(val position: Int) : MainUiEvent
    data class AddRoute(val lineId: Int, val stationId: Int) : MainUiEvent
    data class SetupRoute(val routeScript: String) : MainUiEvent
    data class BeginRoute(val stationId: Int) : MainUiEvent
    data class AutoRoute(val stationId: Int, val mode: Int) : MainUiEvent
    data class ToggleOsakaKanDetour(val farMode: Boolean) : MainUiEvent
    data object ClearError : MainUiEvent
    data class RequestRouteChange(val routeScript: String) : MainUiEvent
    data object ConfirmRouteChange : MainUiEvent
    data object CancelRouteChange : MainUiEvent
}