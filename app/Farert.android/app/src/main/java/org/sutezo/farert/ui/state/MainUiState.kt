package org.sutezo.farert.ui.state

import org.sutezo.alps.FareInfo
import org.sutezo.alps.Route

data class MainUiState(
    val route: Route = Route(),
    val fareInfo: FareInfo? = null,
    val statusMessage: String = "",
    val isLoading: Boolean = false,
    val error: String? = null,
    val canGoBack: Boolean = false,
    val canReverse: Boolean = false,
    val enableFareDetail: Boolean = false,
    val terminalButtonText: String = "",
    val osakakanDetour: OsakaKanDetour = OsakaKanDetour.DISABLE
) {
    enum class OsakaKanDetour {
        DISABLE,
        FAR,
        NEAR
    }
}