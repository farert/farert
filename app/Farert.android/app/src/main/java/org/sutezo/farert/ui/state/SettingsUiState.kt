package org.sutezo.farert.ui.state

data class SettingsUiState(
    val selectedDatabaseIndex: Int = -1,
    val kokuraShinzaiEnabled: Boolean = false,
    val databaseOptions: List<String> = emptyList(),
    val canResetInfoMessages: Boolean = false,
    
    val isLoading: Boolean = false,
    val error: String? = null
) {
    // Helper to check if settings have changed
    fun hasChanges(originalDbIndex: Int, originalKokuraShinzai: Boolean): Boolean {
        return selectedDatabaseIndex != originalDbIndex || kokuraShinzaiEnabled != originalKokuraShinzai
    }
}