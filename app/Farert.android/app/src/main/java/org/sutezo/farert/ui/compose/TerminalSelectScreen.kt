package org.sutezo.farert.ui.compose

import androidx.compose.foundation.clickable
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
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.lifecycle.viewmodel.compose.viewModel
import org.sutezo.alps.RouteUtil
import org.sutezo.alps.getDetailStationInfoForSelList
import org.sutezo.farert.R
import org.sutezo.farert.ui.state.TerminalSelectUiState
import org.sutezo.farert.ui.state.TerminalSelectUiEvent
import org.sutezo.farert.ui.state.TerminalSelectStateHolder
import org.sutezo.farert.ui.components.FarertTopAppBar
import org.sutezo.farert.ui.components.farertTopAppBarColors

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun TerminalSelectScreen(
    mode: String,
    stateHolder: TerminalSelectStateHolder = viewModel(),
    onNavigateToLineList: (Int, String, String) -> Unit = { _, _, _ -> },
    onNavigateToMain: (Int, String) -> Unit = { _, _ -> },
    onNavigateUp: () -> Unit = {}
) {
    val context = LocalContext.current
    val uiState = stateHolder.uiState
    
    // Initialize state
    LaunchedEffect(mode) {
        stateHolder.initialize(context, mode)
    }
    
    // Handle error display
    uiState.error?.let { error ->
        LaunchedEffect(error) {
            // Show error dialog or snackbar
            stateHolder.handleEvent(TerminalSelectUiEvent.ClearError)
        }
    }
    
    val title = if (mode == "autoroute") {
        stringResource(R.string.title_terminal_select_arrive)
    } else {
        stringResource(R.string.title_terminal_select_depart)
    }
    
    var searchQuery by remember { mutableStateOf("") }
    var isSearchActive by remember { mutableStateOf(false) }
    
    Scaffold(
        topBar = {
            if (isSearchActive) {
                SearchTopBar(
                    query = searchQuery,
                    onQueryChange = { 
                        searchQuery = it
                        stateHolder.handleEvent(TerminalSelectUiEvent.SearchQueryChanged(it))
                    },
                    onSearchClose = {
                        isSearchActive = false
                        searchQuery = ""
                        stateHolder.handleEvent(TerminalSelectUiEvent.SearchClosed)
                    }
                )
            } else {
                TopAppBar(
                    title = { Text(title) },
                    navigationIcon = {
                        IconButton(onClick = onNavigateUp) {
                            Icon(Icons.AutoMirrored.Filled.ArrowBack, contentDescription = "Back")
                        }
                    },
                    actions = {
                        IconButton(
                            onClick = { 
                                isSearchActive = true
                                stateHolder.handleEvent(TerminalSelectUiEvent.SearchStarted)
                            }
                        ) {
                            Icon(Icons.Default.Search, contentDescription = "Search")
                        }
                        
                        if (uiState.selectedTab == TerminalSelectUiState.TabType.HISTORY.index && uiState.canClearHistory) {
                            IconButton(
                                onClick = { 
                                    stateHolder.handleEvent(TerminalSelectUiEvent.ClearAllHistory)
                                }
                            ) {
                                Icon(Icons.Default.Delete, contentDescription = "Clear All")
                            }
                        }
                    },
                    colors = farertTopAppBarColors()
                )
            }
        }
    ) { paddingValues ->
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(paddingValues)
        ) {
            if (uiState.isSearching && searchQuery.isNotBlank()) {
                // Search results
                SearchResultsList(
                    results = uiState.searchResults,
                    resultCount = uiState.searchResultCount,
                    onItemClick = { stationId ->
                        onNavigateToMain(stationId, mode)
                    }
                )
            } else {
                // Tab layout
                TabRow(selectedTabIndex = uiState.selectedTab) {
                    Tab(
                        selected = uiState.selectedTab == 0,
                        onClick = { stateHolder.handleEvent(TerminalSelectUiEvent.TabSelected(0)) },
                        text = { Text(stringResource(R.string.tab_company)) }
                    )
                    Tab(
                        selected = uiState.selectedTab == 1,
                        onClick = { stateHolder.handleEvent(TerminalSelectUiEvent.TabSelected(1)) },
                        text = { Text(stringResource(R.string.tab_prefect)) }
                    )
                    Tab(
                        selected = uiState.selectedTab == 2,
                        onClick = { stateHolder.handleEvent(TerminalSelectUiEvent.TabSelected(2)) },
                        text = { Text(stringResource(R.string.tab_history)) }
                    )
                }
                
                // Tab content
                when (uiState.selectedTab) {
                    0 -> CompanyList(
                        companies = uiState.companyList,
                        onItemClick = { companyId ->
                            onNavigateToLineList(companyId, "company", mode)
                        }
                    )
                    1 -> PrefectList(
                        prefects = uiState.prefectList,
                        onItemClick = { prefectId ->
                            onNavigateToLineList(prefectId, "prefect", mode)
                        }
                    )
                    2 -> HistoryList(
                        history = uiState.historyList,
                        onItemClick = { stationId ->
                            onNavigateToMain(stationId, mode)
                        },
                        onItemRemove = { position ->
                            stateHolder.handleEvent(TerminalSelectUiEvent.HistoryItemRemoved(position))
                        }
                    )
                }
            }
        }
    }
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
private fun SearchTopBar(
    query: String,
    onQueryChange: (String) -> Unit,
    onSearchClose: () -> Unit
) {
    TopAppBar(
        title = {
            TextField(
                value = query,
                onValueChange = onQueryChange,
                placeholder = { Text(stringResource(R.string.label_search_title)) },
                colors = TextFieldDefaults.colors(
                    focusedContainerColor = Color.Transparent,
                    unfocusedContainerColor = Color.Transparent,
                    focusedIndicatorColor = Color.Transparent,
                    unfocusedIndicatorColor = Color.Transparent
                ),
                singleLine = true,
                modifier = Modifier.fillMaxWidth()
            )
        },
        navigationIcon = {
            IconButton(onClick = onSearchClose) {
                Icon(Icons.Default.Close, contentDescription = "Close Search")
            }
        },
        colors = farertTopAppBarColors()
    )
}

@Composable
private fun SearchResultsList(
    results: List<Int>,
    resultCount: Int,
    onItemClick: (Int) -> Unit
) {
    Column {
        if (resultCount > 5) {
            Text(
                text = stringResource(R.string.match_disp, resultCount),
                modifier = Modifier.padding(16.dp),
                style = MaterialTheme.typography.bodyMedium
            )
        }
        
        LazyColumn {
            itemsIndexed(results) { _, stationId ->
                SearchResultItem(
                    stationId = stationId,
                    onClick = { onItemClick(stationId) }
                )
            }
        }
    }
}

@Composable
private fun SearchResultItem(
    stationId: Int,
    onClick: () -> Unit
) {
    Card(
        onClick = onClick,
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp, vertical = 4.dp)
    ) {
        Column(
            modifier = Modifier.padding(16.dp)
        ) {
            Text(
                text = RouteUtil.StationNameEx(stationId),
                style = MaterialTheme.typography.bodyMedium,
                color = MaterialTheme.colorScheme.onSurfaceVariant
            )
            Text(
                text = "${RouteUtil.GetKanaFromStationId(stationId)} (${RouteUtil.GetPrefectByStationId(stationId)})",
                style = MaterialTheme.typography.bodySmall,
                color = MaterialTheme.colorScheme.primary
            )
        }
    }
}

@Composable
private fun CompanyList(
    companies: List<Int>,
    onItemClick: (Int) -> Unit
) {
    LazyColumn {
        itemsIndexed(companies) { _, companyId ->
            ListItem(
                headlineContent = { Text(RouteUtil.CompanyName(companyId)) },
                modifier = Modifier.clickable { onItemClick(companyId) }
            )
        }
    }
}

@Composable
private fun PrefectList(
    prefects: List<Int>,
    onItemClick: (Int) -> Unit
) {
    LazyColumn {
        itemsIndexed(prefects) { _, prefectId ->
            ListItem(
                headlineContent = { Text(RouteUtil.PrefectName(prefectId)) },
                modifier = Modifier.clickable { onItemClick(prefectId) }
            )
        }
    }
}

@Composable
private fun HistoryList(
    history: List<Int>,
    onItemClick: (Int) -> Unit,
    onItemRemove: (Int) -> Unit
) {
    if (history.isEmpty()) {
        Box(
            modifier = Modifier.fillMaxSize(),
            contentAlignment = Alignment.Center
        ) {
            Text(
                text = stringResource(R.string.no_history),
                style = MaterialTheme.typography.bodyLarge,
                textAlign = TextAlign.Center
            )
        }
    } else {
        LazyColumn {
            itemsIndexed(history) { index, stationId ->
                ListItem(
                    headlineContent = { 
                        Text(
                            text = RouteUtil.StationNameEx(stationId),
                            color = MaterialTheme.colorScheme.primary
                        )
                    },
                    trailingContent = {
                        IconButton(
                            onClick = { onItemRemove(index) }
                        ) {
                            Icon(
                                Icons.Default.Delete,
                                contentDescription = "Delete",
                                tint = MaterialTheme.colorScheme.error
                            )
                        }
                    },
                    modifier = Modifier.clickable { onItemClick(stationId) }
                )
            }
        }
    }
}

