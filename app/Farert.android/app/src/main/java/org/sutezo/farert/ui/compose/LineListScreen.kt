package org.sutezo.farert.ui.compose

import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.itemsIndexed
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.ArrowBack
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.colorResource
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.lifecycle.viewmodel.compose.viewModel
import org.sutezo.alps.RouteUtil
import org.sutezo.farert.R
import org.sutezo.farert.ui.state.LineListUiState
import org.sutezo.farert.ui.state.LineListUiEvent
import org.sutezo.farert.ui.state.LineListStateHolder
import org.sutezo.farert.ui.components.FarertTopAppBar

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun LineListScreen(
    mode: String,
    ident: Int = 0,
    idType: Int = 0,
    srcStationId: Int = 0,
    startStationId: Int = 0,
    srcLineId: Int = 0,
    isTwoPane: Boolean = false,
    stateHolder: LineListStateHolder = viewModel(),
    onNavigateToStationList: (LineListNavigationData) -> Unit = {},
    onNavigateToTerminalSelect: () -> Unit = {},
    onNavigateUp: () -> Unit = {},
    onStationFragment: (LineListNavigationData) -> Unit = {}
) {
    val context = LocalContext.current
    val uiState = stateHolder.uiState
    
    // Initialize state
    LaunchedEffect(mode, ident, idType, srcStationId, startStationId, srcLineId, isTwoPane) {
        stateHolder.initialize(
            context = context,
            mode = mode,
            ident = ident, 
            idType = idType,
            srcStationId = srcStationId,
            startStationId = startStationId,
            srcLineId = srcLineId,
            isTwoPane = isTwoPane
        )
    }
    
    // Handle error display
    uiState.error?.let { error ->
        LaunchedEffect(error) {
            // Show error dialog or snackbar
            stateHolder.handleEvent(LineListUiEvent.ClearError)
        }
    }
    
    if (isTwoPane) {
        TwoPaneLineListLayout(
            uiState = uiState,
            stateHolder = stateHolder,
            onNavigateUp = onNavigateUp,
            onNavigateToTerminalSelect = onNavigateToTerminalSelect,
            onStationFragment = onStationFragment
        )
    } else {
        SinglePaneLineListLayout(
            uiState = uiState,
            stateHolder = stateHolder,
            onNavigateUp = onNavigateUp,
            onNavigateToTerminalSelect = onNavigateToTerminalSelect,
            onNavigateToStationList = onNavigateToStationList
        )
    }
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
private fun SinglePaneLineListLayout(
    uiState: LineListUiState,
    stateHolder: LineListStateHolder,
    onNavigateUp: () -> Unit,
    onNavigateToTerminalSelect: () -> Unit,
    onNavigateToStationList: (LineListNavigationData) -> Unit
) {
    Scaffold(
        topBar = {
            FarertTopAppBar(
                title = if (uiState.subtitle.isNotEmpty()) {
                    "${uiState.title} - ${uiState.subtitle}"
                } else {
                    uiState.title
                },
                onBackClick = onNavigateUp,
                onMenuClick = {
                    if (uiState.showSwitchAction) {
                        if (uiState.stationSelMode == 0) {
                            onNavigateToTerminalSelect()
                        } else {
                            stateHolder.handleEvent(LineListUiEvent.SwitchActionClicked)
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
                contentAlignment = androidx.compose.ui.Alignment.Center
            ) {
                CircularProgressIndicator()
            }
        } else {
            LazyColumn(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(paddingValues),
                verticalArrangement = Arrangement.spacedBy(1.dp)
            ) {
                itemsIndexed(uiState.lines) { _, lineId ->
                    LineListItem(
                        lineId = lineId,
                        isSelected = lineId == uiState.srcLineId,
                        onClick = {
                            if (lineId != uiState.srcLineId) {
                                stateHolder.handleEvent(LineListUiEvent.LineClicked(lineId))
                                
                                val navigationData = LineListNavigationData(
                                    mode = uiState.mode,
                                    lineId = lineId,
                                    lineToType = when (uiState.idType) {
                                        1 -> "company"
                                        2 -> "prefect"
                                        else -> ""
                                    },
                                    srcStationId = uiState.srcStationId,
                                    startStationId = uiState.startStationId,
                                    lineToId = uiState.ident,
                                    stationMode = if (uiState.mode == "route") "junction" else ""
                                )
                                onNavigateToStationList(navigationData)
                            }
                        }
                    )
                }
            }
        }
    }
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
private fun TwoPaneLineListLayout(
    uiState: LineListUiState,
    stateHolder: LineListStateHolder,
    onNavigateUp: () -> Unit,
    onNavigateToTerminalSelect: () -> Unit,
    onStationFragment: (LineListNavigationData) -> Unit
) {
    // Two-pane layout for tablets - simplified for now
    Row(modifier = Modifier.fillMaxSize()) {
        // Left pane - Line list
        Column(
            modifier = Modifier
                .weight(0.4f)
                .fillMaxHeight()
        ) {
            FarertTopAppBar(
                title = if (uiState.subtitle.isNotEmpty()) {
                    "${uiState.title} - ${uiState.subtitle}"
                } else {
                    uiState.title
                },
                onBackClick = onNavigateUp,
                onMenuClick = { /* No menu action for two-pane */ }
            )
            
            LazyColumn {
                itemsIndexed(uiState.lines) { _, lineId ->
                    LineListItem(
                        lineId = lineId,
                        isSelected = lineId == uiState.srcLineId,
                        onClick = {
                            if (lineId != uiState.srcLineId) {
                                stateHolder.handleEvent(LineListUiEvent.LineClicked(lineId))
                                
                                val navigationData = LineListNavigationData(
                                    mode = uiState.mode,
                                    lineId = lineId,
                                    lineToType = when (uiState.idType) {
                                        1 -> "company"
                                        2 -> "prefect" 
                                        else -> ""
                                    },
                                    srcStationId = uiState.srcStationId,
                                    startStationId = uiState.startStationId,
                                    lineToId = uiState.ident,
                                    stationMode = if (uiState.mode == "route") "junction" else ""
                                )
                                onStationFragment(navigationData)
                                
                                if (uiState.mode == "route") {
                                    stateHolder.updateStationSelMode(1) // junction
                                }
                            }
                        }
                    )
                }
            }
        }
        
        // Right pane - Station list (placeholder)
        Box(
            modifier = Modifier
                .weight(0.6f)
                .fillMaxHeight()
                .background(MaterialTheme.colorScheme.surfaceVariant),
            contentAlignment = androidx.compose.ui.Alignment.Center
        ) {
            Text(
                text = stringResource(R.string.title_stationlistactivity),
                style = MaterialTheme.typography.bodyLarge,
                color = MaterialTheme.colorScheme.onSurfaceVariant
            )
        }
    }
}

@Composable
private fun LineListItem(
    lineId: Int,
    isSelected: Boolean,
    onClick: () -> Unit
) {
    Card(
        onClick = if (!isSelected) onClick else { {} },
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp, vertical = 4.dp),
        colors = if (isSelected) {
            CardDefaults.cardColors(
                containerColor = colorResource(R.color.colorBackOriginTerm)
            )
        } else {
            CardDefaults.cardColors()
        }
    ) {
        Text(
            text = RouteUtil.LineName(lineId),
            modifier = Modifier.padding(16.dp),
            style = if (isSelected) {
                MaterialTheme.typography.bodyLarge.copy(fontWeight = FontWeight.Bold)
            } else {
                MaterialTheme.typography.bodyLarge
            }
        )
    }
}

data class LineListNavigationData(
    val mode: String,
    val lineId: Int,
    val lineToType: String,
    val srcStationId: Int = 0,
    val startStationId: Int = 0,
    val lineToId: Int = 0,
    val stationMode: String = ""
)