package org.sutezo.farert.ui.state

sealed interface ResultViewUiEvent {
    data object LoadData : ResultViewUiEvent
    data object SpecialRuleClicked : ResultViewUiEvent
    data object MeihanCityClicked : ResultViewUiEvent
    data object LongRouteClicked : ResultViewUiEvent
    data object Rule115Clicked : ResultViewUiEvent
    data object ShareClicked : ResultViewUiEvent
    data object ClearError : ResultViewUiEvent
}