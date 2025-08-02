package org.sutezo.farert.ui.compose

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.itemsIndexed
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.ArrowBack
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.lifecycle.viewmodel.compose.viewModel
import org.sutezo.farert.R
import org.sutezo.farert.ui.state.ArchiveRouteUiState
import org.sutezo.farert.ui.state.ArchiveRouteUiEvent
import org.sutezo.farert.ui.state.ArchiveRouteStateHolder
import org.sutezo.farert.ui.components.farertTopAppBarColors

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun ArchiveRouteScreen(
    stateHolder: ArchiveRouteStateHolder = viewModel(),
    onNavigateUp: () -> Unit = {},
    onNavigateToMain: (String) -> Unit = {},
    onImportFromClipboard: () -> String = { "" },
    onExport: (String) -> Unit = {},
    onShowRouteChangeDialog: (String, () -> Unit) -> Unit = { _, callback -> callback() }
) {
    val context = LocalContext.current
    val uiState = stateHolder.uiState
    var showOptionsMenu by remember { mutableStateOf(false) }
    var showClearConfirmDialog by remember { mutableStateOf(false) }
    var showImportDialog by remember { mutableStateOf(false) }
    
    // Initialize state
    LaunchedEffect(Unit) {
        stateHolder.initialize(context)
    }
    
    // Handle error display
    uiState.error?.let { error ->
        LaunchedEffect(error) {
            // Show error dialog or snackbar
            stateHolder.handleEvent(ArchiveRouteUiEvent.ClearError)
        }
    }
    
    // Handle message display
    uiState.message?.let { message ->
        AlertDialog(
            onDismissRequest = { stateHolder.handleEvent(ArchiveRouteUiEvent.ClearMessage) },
            title = { Text(stringResource(R.string.title_main_menu_archive)) },
            text = { Text(message) },
            confirmButton = {
                TextButton(
                    onClick = { 
                        stateHolder.handleEvent(ArchiveRouteUiEvent.ClearMessage)
                        if (message.contains("全削除")) {
                            showClearConfirmDialog = true
                        }
                    }
                ) {
                    Text("OK")
                }
            }
        )
    }
    
    // Clear confirmation dialog
    if (showClearConfirmDialog) {
        AlertDialog(
            onDismissRequest = { showClearConfirmDialog = false },
            title = { Text(stringResource(R.string.main_alert_query_all_clear_title)) },
            text = { Text(stringResource(R.string.main_alert_query_all_clear_mesg)) },
            confirmButton = {
                TextButton(
                    onClick = {
                        showClearConfirmDialog = false
                        stateHolder.handleEvent(ArchiveRouteUiEvent.ConfirmClearAll)
                    }
                ) {
                    Text("Yes")
                }
            },
            dismissButton = {
                TextButton(
                    onClick = { showClearConfirmDialog = false }
                ) {
                    Text("No")
                }
            }
        )
    }
    
    // Import dialog
    if (showImportDialog) {
        AlertDialog(
            onDismissRequest = { showImportDialog = false },
            title = { Text(stringResource(R.string.menu_item_import)) },
            text = { Text(stringResource(R.string.arch_title_import)) },
            confirmButton = {
                TextButton(
                    onClick = { showImportDialog = false }
                ) {
                    Text(stringResource(R.string.agree))
                }
            },
            dismissButton = {
                TextButton(
                    onClick = {
                        showImportDialog = false
                        stateHolder.enableImport()
                        val clipboardText = onImportFromClipboard()
                        stateHolder.handleEvent(ArchiveRouteUiEvent.ImportRoutes(clipboardText))
                    }
                ) {
                    Text(stringResource(R.string.arch_hide_specific_import))
                }
            }
        )
    }
    
    Scaffold(
        topBar = {
            TopAppBar(
                title = { Text(stringResource(R.string.title_main_menu_archive)) },
                navigationIcon = {
                    IconButton(onClick = onNavigateUp) {
                        Icon(Icons.AutoMirrored.Filled.ArrowBack, contentDescription = "Back")
                    }
                },
                colors = farertTopAppBarColors(),
                actions = {
                    // Save button - shown directly when enabled, hidden when disabled (appears in menu)
                    if (uiState.canSave) {
                        IconButton(
                            onClick = { stateHolder.handleEvent(ArchiveRouteUiEvent.SaveRoutes) }
                        ) {
                            Icon(
                                Icons.Default.Save,
                                contentDescription = "保存",
                                tint = MaterialTheme.colorScheme.onSurface
                            )
                        }
                    }
                    
                    Box {
                        IconButton(onClick = { showOptionsMenu = true }) {
                            Icon(Icons.Default.MoreVert, contentDescription = "Options")
                        }
                        
                        DropdownMenu(
                            expanded = showOptionsMenu,
                            onDismissRequest = { showOptionsMenu = false }
                        ) {
                            // Save menu item - only shown when save is disabled
                            if (!uiState.canSave) {
                                DropdownMenuItem(
                                    text = { Text("保存") },
                                    onClick = {
                                        showOptionsMenu = false
                                        stateHolder.handleEvent(ArchiveRouteUiEvent.SaveRoutes)
                                    },
                                    enabled = false,
                                    leadingIcon = {
                                        Icon(
                                            Icons.Default.Save,
                                            contentDescription = null,
                                            tint = MaterialTheme.colorScheme.onSurface.copy(alpha = 0.38f)
                                        )
                                    }
                                )
                            }
                            
                            // Clear all menu item
                            DropdownMenuItem(
                                text = { Text("全削除") },
                                onClick = {
                                    showOptionsMenu = false
                                    showClearConfirmDialog = true
                                },
                                enabled = uiState.canClear,
                                leadingIcon = {
                                    Icon(
                                        Icons.Default.DeleteForever,
                                        contentDescription = null,
                                        tint = if (uiState.canClear) 
                                            MaterialTheme.colorScheme.onSurface 
                                        else 
                                            MaterialTheme.colorScheme.onSurface.copy(alpha = 0.38f)
                                    )
                                }
                            )
                            
                            // Import menu item
                            DropdownMenuItem(
                                text = { Text("インポート") },
                                onClick = {
                                    showOptionsMenu = false
                                    if (uiState.importAvailable) {
                                        val clipboardText = onImportFromClipboard()
                                        stateHolder.handleEvent(ArchiveRouteUiEvent.ImportRoutes(clipboardText))
                                    } else {
                                        showImportDialog = true
                                    }
                                },
                                leadingIcon = {
                                    Icon(Icons.Default.Download, contentDescription = null)
                                }
                            )
                            
                            // Export menu item
                            DropdownMenuItem(
                                text = { Text("エクスポート") },
                                onClick = {
                                    showOptionsMenu = false
                                    onExport(stateHolder.getExportText())
                                },
                                enabled = uiState.canExport,
                                leadingIcon = {
                                    Icon(
                                        Icons.Default.Share,
                                        contentDescription = null,
                                        tint = if (uiState.canExport) 
                                            MaterialTheme.colorScheme.onSurface 
                                        else 
                                            MaterialTheme.colorScheme.onSurface.copy(alpha = 0.38f)
                                    )
                                }
                            )
                        }
                    }
                }
            )
        }
    ) { paddingValues ->
        if (uiState.isLoading) {
            Box(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(paddingValues),
                contentAlignment = Alignment.Center
            ) {
                CircularProgressIndicator()
            }
        } else {
            ArchiveRouteList(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(paddingValues),
                routes = uiState.routes,
                currentRouteScript = uiState.currentRouteScript,
                isSaved = uiState.isSaved,
                stateHolder = stateHolder,
                onRouteClick = { routeScript ->
                    if (!uiState.isSaved) {
                        onShowRouteChangeDialog(routeScript) {
                            onNavigateToMain(routeScript)
                        }
                    } else {
                        onNavigateToMain(routeScript)
                    }
                },
                onRouteRemove = { index ->
                    stateHolder.handleEvent(ArchiveRouteUiEvent.RemoveRoute(index))
                }
            )
        }
    }
}

@Composable
private fun ArchiveRouteList(
    modifier: Modifier = Modifier,
    routes: List<String>,
    currentRouteScript: String,
    isSaved: Boolean,
    stateHolder: ArchiveRouteStateHolder,
    onRouteClick: (String) -> Unit,
    onRouteRemove: (Int) -> Unit
) {
    LazyColumn(
        modifier = modifier,
        contentPadding = PaddingValues(16.dp),
        verticalArrangement = Arrangement.spacedBy(8.dp)
    ) {
        if (routes.isEmpty()) {
            item {
                ArchiveRouteItem(
                    route = stringResource(R.string.no_archive_route),
                    itemType = ArchiveRouteUiState.ItemType.EMPTY,
                    onClick = {},
                    onRemove = {}
                )
            }
        } else {
            itemsIndexed(routes) { index, route ->
                val itemType = stateHolder.getItemType(index)
                ArchiveRouteItem(
                    route = route,
                    itemType = itemType,
                    onClick = { onRouteClick(route) },
                    onRemove = { onRouteRemove(index) }
                )
            }
        }
    }
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
private fun ArchiveRouteItem(
    route: String,
    itemType: ArchiveRouteUiState.ItemType,
    onClick: () -> Unit,
    onRemove: () -> Unit
) {
    val backgroundColor = when (itemType) {
        ArchiveRouteUiState.ItemType.EMPTY -> MaterialTheme.colorScheme.surfaceVariant
        else -> MaterialTheme.colorScheme.surface
    }
    
    val textColor = when (itemType) {
        ArchiveRouteUiState.ItemType.UNSAVED -> Color.Red
        ArchiveRouteUiState.ItemType.SAVED, ArchiveRouteUiState.ItemType.EMPTY -> Color.Gray
        else -> MaterialTheme.colorScheme.onSurface
    }
    
    val fontWeight = when (itemType) {
        ArchiveRouteUiState.ItemType.UNSAVED, ArchiveRouteUiState.ItemType.SAVED, ArchiveRouteUiState.ItemType.EMPTY -> FontWeight.Bold
        else -> FontWeight.Normal
    }
    
    Card(
        onClick = if (itemType != ArchiveRouteUiState.ItemType.EMPTY) onClick else { {} },
        modifier = Modifier.fillMaxWidth(),
        colors = CardDefaults.cardColors(containerColor = backgroundColor)
    ) {
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(16.dp),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Text(
                text = route,
                modifier = Modifier.weight(1f),
                color = textColor,
                fontWeight = fontWeight,
                style = MaterialTheme.typography.bodyLarge
            )
            
            if (itemType != ArchiveRouteUiState.ItemType.EMPTY) {
                IconButton(
                    onClick = onRemove,
                    modifier = Modifier.size(24.dp)
                ) {
                    Icon(
                        Icons.Default.Delete,
                        contentDescription = "Delete",
                        tint = MaterialTheme.colorScheme.error
                    )
                }
            }
        }
    }
}