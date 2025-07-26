package org.sutezo.farert.ui.compose

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.itemsIndexed
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.ArrowBack
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
// import androidx.compose.ui.graphics.Typeface // Not used in Compose version
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.colorResource
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.lifecycle.viewmodel.compose.viewModel
import org.sutezo.alps.RouteUtil
import org.sutezo.alps.getDetailStationInfoForSelList
import org.sutezo.farert.R
import org.sutezo.farert.ui.state.StationListUiState
import org.sutezo.farert.ui.state.StationListUiEvent
import org.sutezo.farert.ui.state.StationListStateHolder
import org.sutezo.farert.ui.components.FarertTopAppBar

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun StationListScreen(
    mode: String,
    lineId: Int,
    srcType: String = "",
    srcCompanyOrPrefectId: Int = 0,
    stationMode: String = "",
    srcStationId: Int = 0,
    startStationId: Int = 0,
    stateHolder: StationListStateHolder = viewModel(),
    onNavigateToMain: (Int) -> Unit = {},
    onNavigateUp: () -> Unit = {},
    onSwitchAction: () -> Unit = {}
) {
    val context = LocalContext.current
    val uiState = stateHolder.uiState
    
    // Initialize state
    LaunchedEffect(mode, lineId, srcType, srcCompanyOrPrefectId, stationMode, srcStationId, startStationId) {
        stateHolder.initialize(
            context = context,
            mode = mode,
            lineId = lineId,
            srcType = srcType,
            srcCompanyOrPrefectId = srcCompanyOrPrefectId,
            stationMode = stationMode,
            srcStationId = srcStationId,
            startStationId = startStationId
        )
    }
    
    // Handle error display
    uiState.error?.let { error ->
        LaunchedEffect(error) {
            // Show error dialog or snackbar
            stateHolder.handleEvent(StationListUiEvent.ClearError)
        }
    }
    
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
                        stateHolder.handleEvent(StationListUiEvent.SwitchActionClicked)
                        onSwitchAction()
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
            LazyColumn(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(paddingValues),
                verticalArrangement = Arrangement.spacedBy(1.dp)
            ) {
                itemsIndexed(uiState.stations) { _, stationId ->
                    StationListItem(
                        stationId = stationId,
                        lineId = uiState.lineId,
                        srcStationId = uiState.srcStationId,
                        startStationId = uiState.startStationId,
                        onClick = {
                            if (stationId != uiState.srcStationId) {
                                stateHolder.handleEvent(StationListUiEvent.StationClicked(stationId))
                                onNavigateToMain(stationId)
                            }
                        }
                    )
                }
            }
        }
    }
}

@Composable
private fun StationListItem(
    stationId: Int,
    lineId: Int,
    srcStationId: Int,
    startStationId: Int,
    onClick: () -> Unit
) {
    val itemViewType = when (stationId) {
        srcStationId -> 1 // Selected station
        startStationId -> 2 // Start station
        else -> 0 // Normal station
    }
    
    Card(
        onClick = if (itemViewType != 1) onClick else { {} },
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp, vertical = 4.dp),
        colors = if (itemViewType == 1) {
            CardDefaults.cardColors(
                containerColor = colorResource(R.color.colorBackOriginTerm)
            )
        } else {
            CardDefaults.cardColors()
        }
    ) {
        Column(
            modifier = Modifier.padding(16.dp)
        ) {
            // Station name
            val stationName = if (itemViewType == 2) {
                val label = stringResource(R.string.route_start_station_mark)
                "$label${RouteUtil.StationNameEx(stationId)}"
            } else {
                RouteUtil.StationNameEx(stationId)
            }
            
            Text(
                text = stationName,
                style = if (itemViewType == 1 || itemViewType == 2) {
                    MaterialTheme.typography.bodyLarge.copy(fontWeight = FontWeight.Bold)
                } else {
                    MaterialTheme.typography.bodyLarge
                }
            )
            
            // Station lines info
            val linesInfo = getDetailStationInfoForSelList(lineId, stationId)
            if (linesInfo.isNotBlank()) {
                Text(
                    text = linesInfo,
                    style = if (itemViewType == 1 || itemViewType == 2) {
                        MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Bold)
                    } else {
                        MaterialTheme.typography.bodyMedium
                    },
                    color = MaterialTheme.colorScheme.onSurfaceVariant
                )
            }
        }
    }
}

data class StationListNavigationData(
    val destStationId: Int,
    val mode: String,
    val lineId: Int
)