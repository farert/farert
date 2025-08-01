package org.sutezo.farert.ui.state

import android.content.Context
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.launch
import org.sutezo.alps.*
import org.sutezo.farert.FarertApp
import org.sutezo.farert.R

class ArchiveRouteStateHolder : ViewModel() {
    
    var uiState by mutableStateOf(ArchiveRouteUiState())
        private set

    private lateinit var context: Context
    private var curState: ArchiveRouteUiState.CurState = ArchiveRouteUiState.CurState.EMPTY

    companion object {
        const val KEY_ARCHIVE = "archive_route"
        const val KEY_IMPORT_AVAILABLE = "import_guide"
        val delim = arrayOf(" ", ",", ".", "[", "]", "<", ">", "/", "{", "}")
        
        fun countOfRoute(routes: List<String>): Int {
            var count = 0
            for (route in routes) {
                val parts = route.split(*delim).filter { it.isNotEmpty() }
                count += parts.size
            }
            return count
        }
        
        fun countOfRoute(route: String): Int {
            return countOfRoute(route.split("\n"))
        }
    }

    fun initialize(context: Context) {
        this.context = context
        
        // Check import availability
        val importAvailable = readParam(context, KEY_IMPORT_AVAILABLE) == "true"
        uiState = uiState.copy(importAvailable = importAvailable)
        
        handleEvent(ArchiveRouteUiEvent.LoadData)
    }

    fun handleEvent(event: ArchiveRouteUiEvent) {
        when (event) {
            is ArchiveRouteUiEvent.LoadData -> {
                loadData()
            }
            
            is ArchiveRouteUiEvent.SaveRoutes -> {
                saveRoutes()
            }
            
            is ArchiveRouteUiEvent.ClearAllRoutes -> {
                uiState = uiState.copy(
                    message = "全削除しますか？"
                )
            }
            
            is ArchiveRouteUiEvent.ConfirmClearAll -> {
                clearAllRoutes()
            }
            
            is ArchiveRouteUiEvent.RouteClicked -> {
                // Route click handling will be done in the Activity through navigation callback
            }
            
            is ArchiveRouteUiEvent.RemoveRoute -> {
                removeRoute(event.index)
            }
            
            is ArchiveRouteUiEvent.ImportRoutes -> {
                importRoutes(event.clipboardText)
            }
            
            is ArchiveRouteUiEvent.ExportRoutes -> {
                // Export handling will be done in the Activity
            }
            
            is ArchiveRouteUiEvent.ShowImportDialog -> {
                uiState = uiState.copy(
                    message = context.getString(R.string.arch_title_import)
                )
            }
            
            is ArchiveRouteUiEvent.ClearError -> {
                uiState = uiState.copy(error = null)
            }
            
            is ArchiveRouteUiEvent.ClearMessage -> {
                uiState = uiState.copy(message = null)
            }
        }
    }
    
    private fun loadData() = viewModelScope.launch {
        try {
            uiState = uiState.copy(isLoading = true)
            
            // Get current route script
            val app = context.applicationContext as? FarertApp
            val currentRouteScript = app?.ds?.let { ds ->
                if (ds.count > 1) ds.route_script() ?: "" else ""
            } ?: ""
            
            // Get saved routes
            val savedRoutes = readParams(context, KEY_ARCHIVE)
            
            // Check if can save current route
            val freeSpace = MAX_ARCHIVE_ROUTE - countOfRoute(savedRoutes)
            val canSaveCurrentRoute = currentRouteScript.isNotEmpty() && freeSpace > 0
            
            // Determine current state and organize routes
            val (finalRoutes, finalCurState) = when {
                currentRouteScript.isEmpty() -> {
                    if (savedRoutes.isEmpty()) {
                        Pair(savedRoutes, ArchiveRouteUiState.CurState.EMPTY)
                    } else {
                        Pair(savedRoutes, ArchiveRouteUiState.CurState.NOTHING_OR_SAVED)
                    }
                }
                else -> {
                    val existIndex = savedRoutes.indexOf(currentRouteScript)
                    when {
                        existIndex >= 1 -> {
                            // Current route exists but not at top - move to top
                            val newRoutes = mutableListOf(currentRouteScript)
                            savedRoutes.filter { it != currentRouteScript }.forEach { newRoutes.add(it) }
                            Pair(newRoutes, ArchiveRouteUiState.CurState.EXIST)
                        }
                        existIndex == -1 -> {
                            // Current route not saved - add to top
                            val newRoutes = mutableListOf(currentRouteScript)
                            newRoutes.addAll(savedRoutes)
                            Pair(newRoutes, ArchiveRouteUiState.CurState.UNSAVE)
                        }
                        existIndex == 0 -> {
                            // Current route is already at top
                            Pair(savedRoutes, ArchiveRouteUiState.CurState.TOP)
                        }
                        else -> Pair(savedRoutes, ArchiveRouteUiState.CurState.NOTHING_OR_SAVED)
                    }
                }
            }
            
            curState = finalCurState
            
            val canClear = when (curState) {
                ArchiveRouteUiState.CurState.EMPTY -> false  
                ArchiveRouteUiState.CurState.UNSAVE -> finalRoutes.size > 1
                else -> finalRoutes.isNotEmpty()
            }
            
            val canSave = curState == ArchiveRouteUiState.CurState.UNSAVE
            val canExport = canClear
            
            uiState = uiState.copy(
                routes = finalRoutes,
                currentRouteScript = currentRouteScript,
                isSaved = !canSave,
                canClear = canClear,
                canSave = canSave,
                canExport = canExport,
                isLoading = false
            )
            
            // Show alert if archive is full
            if (currentRouteScript.isNotEmpty() && !canSaveCurrentRoute) {
                uiState = uiState.copy(
                    message = context.getString(R.string.arch_title_over)
                )
            }
        } catch (e: Exception) {
            uiState = uiState.copy(
                error = e.message,
                isLoading = false
            )
        }
    }
    
    private fun saveRoutes() {
        if (curState == ArchiveRouteUiState.CurState.UNSAVE) {
            val freeSpace = MAX_ARCHIVE_ROUTE - countOfRoute(uiState.routes)
            if (freeSpace > 0) {
                saveParam(context, KEY_ARCHIVE, uiState.routes)
                curState = ArchiveRouteUiState.CurState.NOTHING_OR_SAVED
                uiState = uiState.copy(
                    canSave = false,
                    isSaved = true
                )
            }
        }
    }
    
    private fun clearAllRoutes() {
        saveParam(context, KEY_ARCHIVE, emptyList<String>())
        curState = ArchiveRouteUiState.CurState.EMPTY
        uiState = uiState.copy(
            routes = emptyList(),
            canClear = false,
            canSave = false,
            canExport = false,
            message = null
        )
    }
    
    private fun removeRoute(index: Int) {
        val routes = uiState.routes
        if (index < 0 || index >= routes.size) return
        
        val newRoutes = routes.filterIndexed { i, _ -> i != index }
        
        when (curState) {
            ArchiveRouteUiState.CurState.TOP, ArchiveRouteUiState.CurState.NOTHING_OR_SAVED -> {
                if (index == 0) {
                    curState = ArchiveRouteUiState.CurState.NOTHING_OR_SAVED
                }
                saveParam(context, KEY_ARCHIVE, newRoutes)
            }
            ArchiveRouteUiState.CurState.EXIST -> {
                curState = if (index == 0) {
                    ArchiveRouteUiState.CurState.NOTHING_OR_SAVED
                } else {
                    ArchiveRouteUiState.CurState.TOP
                }
                saveParam(context, KEY_ARCHIVE, newRoutes)
            }
            ArchiveRouteUiState.CurState.UNSAVE -> {
                if (index == 0) {
                    curState = ArchiveRouteUiState.CurState.NOTHING_OR_SAVED
                } else {
                    val saveRoutes = newRoutes.drop(1)
                    saveParam(context, KEY_ARCHIVE, saveRoutes)
                }
            }
            else -> {}
        }
        
        val canClear = when (curState) {
            ArchiveRouteUiState.CurState.EMPTY -> false
            ArchiveRouteUiState.CurState.UNSAVE -> newRoutes.size > 1
            else -> newRoutes.isNotEmpty()
        }
        
        uiState = uiState.copy(
            routes = newRoutes,
            canClear = canClear,
            canSave = curState == ArchiveRouteUiState.CurState.UNSAVE,
            canExport = canClear
        )
    }
    
    private fun importRoutes(clipboardText: String) {
        viewModelScope.launch {
            try {
                var existCount = 0
                var newCount = 0
                var errorCount = 0
                var noAddCount = 0
                var isTopImport = false
                
                var freeSpace = MAX_ARCHIVE_ROUTE - countOfRoute(uiState.routes)
                
                if (freeSpace <= 0) {
                    uiState = uiState.copy(
                        message = "これ以上経路は追加できません. 既存の経路をいくつか削除してください"
                    )
                    return@launch
                }
                
                val routeScriptList = clipboardText.split("\n")
                val routeList = uiState.routes.toMutableList()
                
                for (scriptRoute in routeScriptList) {
                    if (scriptRoute.trim().isEmpty()) continue
                    
                    val scrRoute = scriptRoute.split(*delim).filter { it.isNotEmpty() }.joinToString(",")
                    
                    if (freeSpace <= 0) {
                        noAddCount++
                    } else {
                        val route = Route()
                        val rc = route.setup_route(scrRoute)
                        if (rc < 0) {
                            errorCount++
                        } else {
                            val inIndex = routeList.indexOf(scrRoute)
                            if (inIndex >= 0) {
                                if (inIndex == 0) {
                                    isTopImport = true
                                }
                                existCount++
                            } else {
                                newCount++
                                freeSpace -= countOfRoute(scrRoute)
                                routeList.add(scrRoute)
                            }
                        }
                    }
                }
                
                var message = when {
                    newCount <= 0 -> {
                        when {
                            errorCount > 0 -> "追加経路はありません(すべて不正書式でした）"
                            existCount <= 0 -> "追加経路はありません."
                            else -> "追加経路はありません(既に全経路あります)."
                        }
                    }
                    existCount > 0 -> "${newCount}経路を追加しました(${existCount}経路は既にあります.)"
                    else -> "${newCount}経路を追加しました"
                }
                
                if (errorCount > 0 && newCount > 0) {
                    message += "\n${errorCount}経路が不正書式でした."
                }
                if (noAddCount > 0) {
                    message += "\n上限を越えたため追加できなかった経路が${noAddCount}経路あります"
                }
                
                if (newCount > 0) {
                    // Update routes and save
                    if (curState == ArchiveRouteUiState.CurState.UNSAVE && !isTopImport) {
                        val saveRoutes = routeList.drop(1)
                        saveParam(context, KEY_ARCHIVE, saveRoutes)
                    } else {
                        saveParam(context, KEY_ARCHIVE, routeList)
                    }
                    
                    // Update state
                    when (curState) {
                        ArchiveRouteUiState.CurState.EXIST, ArchiveRouteUiState.CurState.EMPTY -> {
                            curState = ArchiveRouteUiState.CurState.NOTHING_OR_SAVED
                        }
                        ArchiveRouteUiState.CurState.UNSAVE -> {
                            if (isTopImport) {
                                curState = ArchiveRouteUiState.CurState.TOP
                            }
                        }
                        else -> {}
                    }
                    
                    val canClear = when (curState) {
                        ArchiveRouteUiState.CurState.UNSAVE -> routeList.size > 1
                        else -> routeList.isNotEmpty()
                    }
                    
                    uiState = uiState.copy(
                        routes = routeList,
                        canClear = canClear,
                        canSave = curState == ArchiveRouteUiState.CurState.UNSAVE,
                        canExport = canClear
                    )
                }
                
                uiState = uiState.copy(message = message)
                
            } catch (e: Exception) {
                uiState = uiState.copy(error = e.message)
            }
        }
    }
    
    fun enableImport() {
        saveParam(context, KEY_IMPORT_AVAILABLE, "true")
        uiState = uiState.copy(importAvailable = true)
    }
    
    fun getExportText(): String {
        return uiState.routes.joinToString("\n")
    }
    
    fun getItemType(index: Int): ArchiveRouteUiState.ItemType {
        if (curState == ArchiveRouteUiState.CurState.EMPTY) {
            return ArchiveRouteUiState.ItemType.EMPTY
        }
        
        val route = uiState.routes.getOrNull(index) ?: return ArchiveRouteUiState.ItemType.NORMAL
        
        return if (route == uiState.currentRouteScript) {
            if (curState == ArchiveRouteUiState.CurState.UNSAVE) {
                ArchiveRouteUiState.ItemType.UNSAVED
            } else {
                ArchiveRouteUiState.ItemType.SAVED
            }
        } else {
            ArchiveRouteUiState.ItemType.NORMAL
        }
    }
}