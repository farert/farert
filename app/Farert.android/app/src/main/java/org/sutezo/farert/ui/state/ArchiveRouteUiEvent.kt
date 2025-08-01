package org.sutezo.farert.ui.state

sealed interface ArchiveRouteUiEvent {
    data object LoadData : ArchiveRouteUiEvent
    data object SaveRoutes : ArchiveRouteUiEvent
    data object ClearAllRoutes : ArchiveRouteUiEvent
    data object ConfirmClearAll : ArchiveRouteUiEvent
    data class RouteClicked(val routeScript: String) : ArchiveRouteUiEvent
    data class RemoveRoute(val index: Int) : ArchiveRouteUiEvent
    data class ImportRoutes(val clipboardText: String) : ArchiveRouteUiEvent
    data object ExportRoutes : ArchiveRouteUiEvent
    data object ShowImportDialog : ArchiveRouteUiEvent
    data object ClearError : ArchiveRouteUiEvent
    data object ClearMessage : ArchiveRouteUiEvent
}