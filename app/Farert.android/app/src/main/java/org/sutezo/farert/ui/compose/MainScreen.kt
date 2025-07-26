package org.sutezo.farert.ui.compose

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
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.dp
import androidx.lifecycle.viewmodel.compose.viewModel
import kotlinx.coroutines.launch
import org.sutezo.alps.RouteList
import org.sutezo.alps.RouteUtil
import org.sutezo.farert.R
import org.sutezo.alps.fareNumStr
import org.sutezo.alps.kmNumStr
import org.sutezo.farert.ui.state.MainUiState
import org.sutezo.farert.ui.state.MainUiEvent
import org.sutezo.farert.ui.state.MainStateHolder
import org.sutezo.farert.ui.components.FarertTopAppBar

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun MainScreen(
    stateHolder: MainStateHolder = viewModel(),
    onNavigateToTerminalSelect: () -> Unit = {},
    onNavigateToFareDetail: () -> Unit = {},
    onNavigateToArchive: (String) -> Unit = {},
    onNavigateToRouteDetail: (Int) -> Unit = {},
    onNavigateToSettings: () -> Unit = {},
    onNavigateToVersion: () -> Unit = {},
    onToggleOsakakanDetour: () -> Unit = {}
) {
    val uiState = stateHolder.uiState
    val drawerState = rememberDrawerState(DrawerValue.Closed)
    val scope = rememberCoroutineScope()
    
    // Handle error display
    uiState.error?.let { error ->
        LaunchedEffect(error) {
            // Show error dialog or snackbar
            stateHolder.handleEvent(MainUiEvent.ClearError)
        }
    }
    
    ModalNavigationDrawer(
        drawerState = drawerState,
        drawerContent = {
            FolderDrawerContent(
                route = uiState.route,
                onRouteSelected = { selectedRoute ->
                    onNavigateToArchive(selectedRoute.route_script())
                    scope.launch {
                        drawerState.close()
                    }
                },
                modifier = Modifier.fillMaxHeight()
            )
        }
    ) {
        Scaffold(
        topBar = {
            var showMenu by remember { mutableStateOf(false) }
            
            FarertTopAppBar(
                title = stringResource(R.string.title_main_view),
                onBackClick = {
                    scope.launch {
                        drawerState.open()
                    }
                },
                showMenu = showMenu,
                onMenuClick = { showMenu = true },
                onDismissMenu = { showMenu = false },
                dropdownContent = {
                    DropdownMenuItem(
                        text = { Text(stringResource(R.string.title_main_menu_settings)) },
                        onClick = {
                            showMenu = false
                            onNavigateToSettings()
                        }
                    )
                    DropdownMenuItem(
                        text = { Text(stringResource(R.string.title_main_menu_version)) },
                        onClick = {
                            showMenu = false
                            onNavigateToVersion()
                        }
                    )
                    DropdownMenuItem(
                        text = { Text(stringResource(R.string.title_specific_calc_option_osakakan)) },
                        onClick = {
                            showMenu = false
                            onToggleOsakakanDetour()
                        }
                    )
                }
            )
        },
        bottomBar = {
            BottomNavigationBar(
                canGoBack = uiState.canGoBack,
                canReverse = uiState.canReverse,
                onBackPressed = { stateHolder.handleEvent(MainUiEvent.GoBack) },
                onReverseRoute = { stateHolder.handleEvent(MainUiEvent.ReverseRoute) },
                onArchiveRoute = {
                    val routeScript = if (uiState.route.count <= 1) "" else uiState.route.route_script()
                    onNavigateToArchive(routeScript)
                }
            )
        }
    ) { paddingValues ->
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(paddingValues)
                .padding(16.dp)
        ) {
            // Terminal selection card with enhanced design
            Card(
                onClick = { onNavigateToTerminalSelect() },
                modifier = Modifier.fillMaxWidth(),
                colors = CardDefaults.cardColors(
                    containerColor = MaterialTheme.colorScheme.primaryContainer
                ),
                elevation = CardDefaults.cardElevation(defaultElevation = 4.dp)
            ) {
                Row(
                    modifier = Modifier
                        .fillMaxWidth()
                        .padding(20.dp),
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Icon(
                        imageVector = Icons.Default.LocationOn,
                        contentDescription = null,
                        modifier = Modifier.size(32.dp),
                        tint = MaterialTheme.colorScheme.onPrimaryContainer
                    )
                    Spacer(modifier = Modifier.width(16.dp))
                    Column(modifier = Modifier.weight(1f)) {
                        Text(
                            text = "駅を選択",
                            style = MaterialTheme.typography.titleMedium,
                            color = MaterialTheme.colorScheme.onPrimaryContainer
                        )
                        Text(
                            text = uiState.terminalButtonText,
                            style = MaterialTheme.typography.bodyMedium,
                            color = MaterialTheme.colorScheme.onPrimaryContainer.copy(alpha = 0.7f)
                        )
                    }
                    Icon(
                        imageVector = Icons.Default.ChevronRight,
                        contentDescription = null,
                        tint = MaterialTheme.colorScheme.onPrimaryContainer.copy(alpha = 0.7f)
                    )
                }
            }
            
            Spacer(modifier = Modifier.height(16.dp))
            
            // Route list
            LazyColumn(
                modifier = Modifier.weight(1f),
                verticalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                itemsIndexed(
                    items = (0 until uiState.route.count).toList(),
                    key = { index, _ -> index }
                ) { index, _ ->
                    RouteListItem(
                        route = uiState.route,
                        position = index,
                        isLast = index == uiState.route.count - 1,
                        statusMessage = if (index == uiState.route.count - 1) uiState.statusMessage else "",
                        onClick = { onNavigateToRouteDetail(index) }
                    )
                }
            }
            
            // Fare information with enhanced design
            uiState.fareInfo?.let { info ->
                Card(
                    modifier = Modifier.fillMaxWidth(),
                    colors = CardDefaults.cardColors(
                        containerColor = MaterialTheme.colorScheme.secondaryContainer
                    ),
                    elevation = CardDefaults.cardElevation(defaultElevation = 6.dp)
                ) {
                    Column(
                        modifier = Modifier.padding(20.dp)
                    ) {
                        // 運賃ヘッダー
                        Row(
                            modifier = Modifier.fillMaxWidth(),
                            verticalAlignment = Alignment.CenterVertically
                        ) {
                            Icon(
                                imageVector = Icons.Default.CurrencyYen,
                                contentDescription = null,
                                modifier = Modifier.size(28.dp),
                                tint = MaterialTheme.colorScheme.onSecondaryContainer
                            )
                            Spacer(modifier = Modifier.width(12.dp))
                            Column(modifier = Modifier.weight(1f)) {
                                Text(
                                    text = "運賃",
                                    style = MaterialTheme.typography.titleMedium,
                                    color = MaterialTheme.colorScheme.onSecondaryContainer
                                )
                                Text(
                                    text = stringResource(R.string.result_yen, fareNumStr(info.fare)),
                                    style = MaterialTheme.typography.headlineMedium,
                                    color = MaterialTheme.colorScheme.onSecondaryContainer
                                )
                            }
                            Button(
                                onClick = { onNavigateToFareDetail() },
                                enabled = uiState.enableFareDetail,
                                colors = ButtonDefaults.buttonColors(
                                    containerColor = MaterialTheme.colorScheme.primary
                                )
                            ) {
                                Text(stringResource(R.string.fare_detail))
                            }
                        }
                        
                        Spacer(modifier = Modifier.height(16.dp))
                        
                        // 詳細情報
                        Row(
                            modifier = Modifier.fillMaxWidth(),
                            horizontalArrangement = Arrangement.SpaceBetween
                        ) {
                            Column {
                                Text(
                                    text = "営業キロ",
                                    style = MaterialTheme.typography.labelMedium,
                                    color = MaterialTheme.colorScheme.onSecondaryContainer.copy(alpha = 0.7f)
                                )
                                Text(
                                    text = stringResource(R.string.result_km, kmNumStr(info.totalSalesKm)),
                                    style = MaterialTheme.typography.bodyLarge,
                                    color = MaterialTheme.colorScheme.onSecondaryContainer
                                )
                            }
                            Column {
                                Text(
                                    text = "有効日数",
                                    style = MaterialTheme.typography.labelMedium,
                                    color = MaterialTheme.colorScheme.onSecondaryContainer.copy(alpha = 0.7f)
                                )
                                Text(
                                    text = stringResource(R.string.result_availdays_fmt, info.ticketAvailDays),
                                    style = MaterialTheme.typography.bodyLarge,
                                    color = MaterialTheme.colorScheme.onSecondaryContainer
                                )
                            }
                        }
                    }
                }
            }
        }
    }
    }
}

@Composable
fun RouteListItem(
    route: RouteList,
    position: Int,
    isLast: Boolean,
    statusMessage: String,
    onClick: () -> Unit
) {
    Card(
        onClick = onClick,
        modifier = Modifier.fillMaxWidth(),
        colors = CardDefaults.cardColors(
            containerColor = if (isLast) MaterialTheme.colorScheme.tertiaryContainer
                           else MaterialTheme.colorScheme.surfaceVariant
        ),
        elevation = CardDefaults.cardElevation(defaultElevation = 2.dp)
    ) {
        if (isLast) {
            // Last item shows status message or add route prompt
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(16.dp),
                verticalAlignment = Alignment.CenterVertically,
                horizontalArrangement = Arrangement.Center
            ) {
                Icon(
                    imageVector = if (statusMessage.isNotEmpty()) Icons.Default.Warning else Icons.Default.Add,
                    contentDescription = null,
                    modifier = Modifier.size(20.dp),
                    tint = if (statusMessage.isNotEmpty()) MaterialTheme.colorScheme.error 
                          else MaterialTheme.colorScheme.onTertiaryContainer
                )
                Spacer(modifier = Modifier.width(8.dp))
                Text(
                    text = if (statusMessage.isNotEmpty()) statusMessage 
                          else stringResource(R.string.add_route),
                    style = MaterialTheme.typography.bodyMedium,
                    color = if (statusMessage.isNotEmpty()) MaterialTheme.colorScheme.error
                           else MaterialTheme.colorScheme.onTertiaryContainer
                )
            }
        } else {
            // Regular route item with enhanced design
            val current = route.item(position + 1)
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(16.dp),
                verticalAlignment = Alignment.CenterVertically
            ) {
                // 路線アイコン
                Card(
                    modifier = Modifier.size(40.dp),
                    colors = CardDefaults.cardColors(
                        containerColor = MaterialTheme.colorScheme.secondary
                    )
                ) {
                    Box(contentAlignment = Alignment.Center, modifier = Modifier.fillMaxSize()) {
                        Text(
                            text = "${position + 1}",
                            style = MaterialTheme.typography.labelMedium,
                            color = MaterialTheme.colorScheme.onSecondary
                        )
                    }
                }
                
                Spacer(modifier = Modifier.width(12.dp))
                
                Column(modifier = Modifier.weight(1f)) {
                    Text(
                        text = RouteUtil.LineName(current.lineId()),
                        style = MaterialTheme.typography.titleMedium,
                        color = MaterialTheme.colorScheme.onSurfaceVariant
                    )
                    Text(
                        text = RouteUtil.StationName(current.stationId()),
                        style = MaterialTheme.typography.bodyMedium,
                        color = MaterialTheme.colorScheme.onSurfaceVariant.copy(alpha = 0.7f)
                    )
                }
                
                Icon(
                    imageVector = Icons.Default.ChevronRight,
                    contentDescription = null,
                    tint = MaterialTheme.colorScheme.onSurfaceVariant.copy(alpha = 0.5f)
                )
            }
        }
    }
}

@Composable
fun BottomNavigationBar(
    canGoBack: Boolean,
    canReverse: Boolean,
    onBackPressed: () -> Unit,
    onReverseRoute: () -> Unit,
    onArchiveRoute: () -> Unit
) {
    NavigationBar {
        NavigationBarItem(
            icon = { Icon(Icons.AutoMirrored.Filled.ArrowBack, contentDescription = null) },
            label = { Text(stringResource(R.string.back)) },
            selected = false,
            onClick = onBackPressed,
            enabled = canGoBack
        )
        
        NavigationBarItem(
            icon = { Icon(Icons.Default.SwapHoriz, contentDescription = null) },
            label = { Text(stringResource(R.string.reverse)) },
            selected = false,
            onClick = onReverseRoute,
            enabled = canReverse
        )
        
        NavigationBarItem(
            icon = { Icon(Icons.Default.Archive, contentDescription = null) },
            label = { Text(stringResource(R.string.archive)) },
            selected = false,
            onClick = onArchiveRoute
        )
    }
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun FolderDrawerContent(
    route: RouteList,
    onRouteSelected: (RouteList) -> Unit,
    modifier: Modifier = Modifier
) {
    ModalDrawerSheet(modifier = modifier) {
        Column(
            modifier = Modifier
                .fillMaxWidth()
                .padding(16.dp)
        ) {
            // ドロワーヘッダー
            Card(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(bottom = 16.dp),
                colors = CardDefaults.cardColors(
                    containerColor = MaterialTheme.colorScheme.primaryContainer
                )
            ) {
                Column(
                    modifier = Modifier.padding(16.dp)
                ) {
                    Icon(
                        imageVector = Icons.Default.Train,
                        contentDescription = null,
                        modifier = Modifier.size(32.dp),
                        tint = MaterialTheme.colorScheme.onPrimaryContainer
                    )
                    Spacer(modifier = Modifier.height(8.dp))
                    Text(
                        text = "保存された経路",
                        style = MaterialTheme.typography.headlineSmall,
                        color = MaterialTheme.colorScheme.onPrimaryContainer
                    )
                    Text(
                        text = "タップして経路を選択",
                        style = MaterialTheme.typography.bodyMedium,
                        color = MaterialTheme.colorScheme.onPrimaryContainer.copy(alpha = 0.7f)
                    )
                }
            }
            
            Spacer(modifier = Modifier.height(8.dp))
            
            // FolderViewScreenの内容を埋め込み
            FolderViewScreen(
                routefolder = remember { org.sutezo.farert.Routefolder() },
                route = route,
                onItemClick = onRouteSelected,
                onCloseDrawer = { /* ドロワーを閉じる処理はMainScreenで行う */ }
            )
        }
    }
}