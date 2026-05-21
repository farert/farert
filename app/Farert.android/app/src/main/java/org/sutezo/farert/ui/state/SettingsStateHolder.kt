package org.sutezo.farert.ui.state

import android.content.Context
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.launch
import org.sutezo.alps.readParam
import org.sutezo.alps.saveParam
import org.sutezo.farert.BackupRestoreManager
import org.sutezo.farert.DatabaseOpenHelper
import org.sutezo.farert.FarertApp

class SettingsStateHolder : ViewModel() {
    
    var uiState by mutableStateOf(SettingsUiState())
        private set

    private lateinit var context: Context
    private var originalDatabaseIndex: Int = -1
    private var originalKokuraShinzai: Boolean = false

    fun initialize(context: Context) {
        this.context = context
        handleEvent(SettingsUiEvent.LoadSettings)
    }

    fun handleEvent(event: SettingsUiEvent) {
        when (event) {
            is SettingsUiEvent.LoadSettings -> {
                loadSettings()
            }
            
            is SettingsUiEvent.DatabaseSelectionChanged -> {
                uiState = uiState.copy(selectedDatabaseIndex = event.index)
            }
            
            is SettingsUiEvent.KokuraShinzaiToggled -> {
                uiState = uiState.copy(kokuraShinzaiEnabled = event.enabled)
            }
            
            is SettingsUiEvent.ResetInfoMessages -> {
                resetInfoMessages()
            }

            is SettingsUiEvent.BackupToClipboard -> {
                backupToClipboard()
            }

            is SettingsUiEvent.RestoreFromClipboard -> {
                restoreFromClipboard(event.jsonText)
            }
            
            is SettingsUiEvent.SaveSettings -> {
                saveSettings()
            }
            
            is SettingsUiEvent.ClearError -> {
                uiState = uiState.copy(error = null)
            }

            is SettingsUiEvent.ClearMessage -> {
                uiState = uiState.copy(message = null)
            }

            is SettingsUiEvent.ClearClipboardText -> {
                uiState = uiState.copy(clipboardText = null)
            }
        }
    }
    
    private fun loadSettings() = viewModelScope.launch {
        try {
            uiState = uiState.copy(isLoading = true)
            
            val app = context.applicationContext as? FarertApp
            val kokuraShinzai = app?.bKokuraHakataShinZaiFlag ?: false
            
            val selDatabase = readParam(context, "datasource")
            val dbIndex = try {
                Integer.parseInt(selDatabase)
            } catch (e: NumberFormatException) {
                -1
            }
            
            // Create database options (this would need to be populated from actual data)
            val databaseOptions = listOf("データベース 1", "データベース 2", "データベース 3") // Placeholder
            
            val finalDbIndex = if (dbIndex < 0) databaseOptions.size - 1 else dbIndex
            
            // Check if info messages can be reset
            val infoKeys = arrayOf(
                "setting_key_hide_osakakan_detour_info",
                "setting_key_hide_no_rule_info", 
                "import_guide"
            )
            val canReset = infoKeys.any { readParam(context, it) == "true" }
            
            // Store original values
            originalDatabaseIndex = finalDbIndex
            originalKokuraShinzai = kokuraShinzai
            
            uiState = uiState.copy(
                selectedDatabaseIndex = finalDbIndex,
                kokuraShinzaiEnabled = kokuraShinzai,
                databaseOptions = databaseOptions,
                canResetInfoMessages = canReset,
                isLoading = false
            )
        } catch (e: Exception) {
            uiState = uiState.copy(
                error = e.message,
                isLoading = false
            )
        }
    }
    
    private fun resetInfoMessages() {
        val keys = arrayOf(
            "setting_key_hide_osakakan_detour_info",
            "setting_key_hide_no_rule_info",
            "import_guide"
        )
        
        keys.forEach { key ->
            saveParam(context, key, "")
        }
        
        uiState = uiState.copy(canResetInfoMessages = false)
    }

    private fun backupToClipboard() {
        try {
            val json = BackupRestoreManager.exportJson(context)
            uiState = uiState.copy(
                clipboardText = json,
                message = "バックアップ JSON をクリップボードへコピーしました",
                error = null
            )
        } catch (e: Exception) {
            uiState = uiState.copy(error = e.message)
        }
    }

    private fun restoreFromClipboard(jsonText: String) {
        try {
            val result = BackupRestoreManager.restoreJson(context, jsonText)
            uiState = uiState.copy(
                message = "リストアしました: 保存経路 ${result.savedRoutes}件、きっぷホルダ ${result.ticketHolderRoutes}件、履歴 ${result.stationHistory}件",
                error = null
            )
        } catch (e: Exception) {
            uiState = uiState.copy(error = "リストアに失敗しました: ${e.message}")
        }
    }
    
    fun saveSettings(): Boolean {
        var hasChanges = false
        
        try {
            // Save database selection if changed
            val validDbIndex = DatabaseOpenHelper.validDBidx(uiState.selectedDatabaseIndex)
            if (originalDatabaseIndex != validDbIndex) {
                saveParam(context, "datasource", validDbIndex.toString())
                val app = context.applicationContext as? FarertApp
                app?.changeDatabase(validDbIndex)
                hasChanges = true
            }
            
            // Save Kokura-Shinzai setting if changed
            if (originalKokuraShinzai != uiState.kokuraShinzaiEnabled) {
                val value = if (uiState.kokuraShinzaiEnabled) "true" else "false"
                saveParam(context, "kokura_hakata_shinzai", value)
                val app = context.applicationContext as? FarertApp
                app?.bKokuraHakataShinZaiFlag = uiState.kokuraShinzaiEnabled
                hasChanges = true
            }
            
            return hasChanges
        } catch (e: Exception) {
            uiState = uiState.copy(error = e.message)
            return false
        }
    }
    
    fun hasUnsavedChanges(): Boolean {
        return uiState.hasChanges(originalDatabaseIndex, originalKokuraShinzai)
    }
}
