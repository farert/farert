package org.sutezo.farert.ui.state

data class ArchiveRouteUiState(
    val routes: List<String> = emptyList(),
    val currentRouteScript: String = "",
    val isSaved: Boolean = true,
    
    // Menu state
    val canClear: Boolean = false,
    val canSave: Boolean = false,
    val canExport: Boolean = false,
    
    // Import/Export state
    val importAvailable: Boolean = false,
    
    val isLoading: Boolean = false,
    val error: String? = null,
    val message: String? = null
) {
    enum class CurState {
        TOP,                // currentScript is exist top
        EXIST,
        EMPTY,              // empty and currentScript nothing
        NOTHING_OR_SAVED,   // currentScript nothing(保存済み or メイン画面経路なし)
        UNSAVE,             // currentScript is exist in values
    }
    
    enum class ItemType {
        NORMAL,
        UNSAVED,
        SAVED,
        EMPTY,
    }
}