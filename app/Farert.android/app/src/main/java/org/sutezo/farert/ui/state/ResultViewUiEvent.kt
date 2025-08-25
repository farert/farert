package org.sutezo.farert.ui.state

sealed interface ResultViewUiEvent {
    data object LoadData : ResultViewUiEvent
    data object StocktokaiClicked : ResultViewUiEvent
    data object SpecialRuleClicked : ResultViewUiEvent
    data object MeihanCityClicked : ResultViewUiEvent
    data object LongRouteClicked : ResultViewUiEvent
    data object Rule115Clicked : ResultViewUiEvent
//b#22011101F    data object OsakakanClicked : ResultViewUiEvent
    data object ShareClicked : ResultViewUiEvent
    data object ClearError : ResultViewUiEvent
}