package org.sutezo.farert.ui.state

import android.content.Context
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.launch
import org.sutezo.alps.*

class TerminalSelectStateHolder : ViewModel() {
    
    var uiState by mutableStateOf(TerminalSelectUiState())
        private set

    private lateinit var context: Context

    fun initialize(context: Context, mode: String) {
        this.context = context
        uiState = uiState.copy(mode = mode)
        handleEvent(TerminalSelectUiEvent.LoadData)
    }

    fun handleEvent(event: TerminalSelectUiEvent) {
        when (event) {
            is TerminalSelectUiEvent.TabSelected -> {
                uiState = uiState.copy(selectedTab = event.tabIndex)
            }
            
            is TerminalSelectUiEvent.SearchQueryChanged -> {
                uiState = uiState.copy(searchQuery = event.query)
                if (event.query.isNotBlank()) {
                    performSearch(event.query)
                } else {
                    uiState = uiState.copy(isSearching = false, searchResults = emptyList())
                }
            }
            
            is TerminalSelectUiEvent.SearchStarted -> {
                uiState = uiState.copy(isSearching = true)
            }
            
            is TerminalSelectUiEvent.SearchClosed -> {
                uiState = uiState.copy(
                    isSearching = false, 
                    searchQuery = "", 
                    searchResults = emptyList()
                )
            }
            
            is TerminalSelectUiEvent.ItemClicked -> {
                // This will be handled by navigation in the Activity
            }
            
            is TerminalSelectUiEvent.HistoryItemRemoved -> {
                removeHistoryItem(event.position)
            }
            
            is TerminalSelectUiEvent.ClearAllHistory -> {
                clearAllHistory()
            }
            
            is TerminalSelectUiEvent.LoadData -> {
                loadData()
            }
            
            is TerminalSelectUiEvent.ClearError -> {
                uiState = uiState.copy(error = null)
            }
        }
    }
    
    private fun performSearch(query: String) = viewModelScope.launch {
        try {
            uiState = uiState.copy(isLoading = true)
            val results = keyMatchStations(query)
            uiState = uiState.copy(
                searchResults = results,
                searchResultCount = results.size,
                isLoading = false,
                isSearching = true
            )
        } catch (e: Exception) {
            uiState = uiState.copy(
                error = e.message,
                isLoading = false
            )
        }
    }
    
    private fun removeHistoryItem(position: Int) = viewModelScope.launch {
        try {
            val currentHistory = uiState.historyList.toMutableList()
            if (position < currentHistory.size) {
                currentHistory.removeAt(position)
                val historyNames = currentHistory.map { RouteUtil.StationNameEx(it) }
                saveHistory(context, historyNames)
                uiState = uiState.copy(
                    historyList = currentHistory,
                    canClearHistory = currentHistory.isNotEmpty()
                )
            }
        } catch (e: Exception) {
            uiState = uiState.copy(error = e.message)
        }
    }
    
    private fun clearAllHistory() = viewModelScope.launch {
        try {
            saveHistory(context, emptyList())
            uiState = uiState.copy(
                historyList = emptyList(),
                canClearHistory = false
            )
        } catch (e: Exception) {
            uiState = uiState.copy(error = e.message)
        }
    }
    
    private fun loadData() = viewModelScope.launch {
        try {
            uiState = uiState.copy(isLoading = true)
            
            val companies = getCompanys()
            val prefects = getPrefects()
            val history = readParams(context, "history")
                .map { RouteUtil.GetStationId(it) }
                .filter { it > 0 }
            
            uiState = uiState.copy(
                companyList = companies,
                prefectList = prefects,
                historyList = history,
                canClearHistory = history.isNotEmpty(),
                isLoading = false
            )
        } catch (e: Exception) {
            uiState = uiState.copy(
                error = e.message,
                isLoading = false
            )
        }
    }
}