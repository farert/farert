package org.sutezo.farert.ui.state

sealed interface SettingsUiEvent {
    data object LoadSettings : SettingsUiEvent
    data class DatabaseSelectionChanged(val index: Int) : SettingsUiEvent
    data class KokuraShinzaiToggled(val enabled: Boolean) : SettingsUiEvent
    data object ResetInfoMessages : SettingsUiEvent
    data object BackupToClipboard : SettingsUiEvent
    data class RestoreFromClipboard(val jsonText: String) : SettingsUiEvent
    data object SaveSettings : SettingsUiEvent
    data object ClearError : SettingsUiEvent
    data object ClearMessage : SettingsUiEvent
    data object ClearClipboardText : SettingsUiEvent
}
