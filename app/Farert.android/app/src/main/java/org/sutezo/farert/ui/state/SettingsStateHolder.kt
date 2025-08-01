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
            
            is SettingsUiEvent.SaveSettings -> {
                saveSettings()
            }
            
            is SettingsUiEvent.ClearError -> {
                uiState = uiState.copy(error = null)
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