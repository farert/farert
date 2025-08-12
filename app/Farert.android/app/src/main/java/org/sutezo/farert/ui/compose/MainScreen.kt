package org.sutezo.farert.ui.compose

import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.itemsIndexed
import androidx.compose.foundation.lazy.rememberLazyListState
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.ArrowBack
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.lifecycle.viewmodel.compose.viewModel
import kotlinx.coroutines.launch
import org.sutezo.alps.RouteList
import org.sutezo.alps.RouteUtil
import org.sutezo.farert.R
import org.sutezo.alps.Route
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
    
    // LazyColumn state for auto-scrolling
    val listState = rememberLazyListState()
    
    // Handle error display
    uiState.error?.let { error ->
        LaunchedEffect(error) {
            // Show error dialog or snackbar
            stateHolder.handleEvent(MainUiEvent.ClearError)
        }
    }
    
    // Force recomposition when route changes
    LaunchedEffect(uiState.route, uiState.route.hashCode()) {
        // This will trigger recomposition when route object changes
        println("DEBUG: MainScreen recomposition triggered - route.count=${uiState.route.count}")
    }
    
    // Auto-scroll to last item when route is added
    LaunchedEffect(uiState.route.count) {
        if (uiState.route.count > 1) {
            // Scroll to the last item (add route button)
            val lastItemIndex = if (uiState.route.count > 1) uiState.route.count - 1 else 0
            listState.animateScrollToItem(lastItemIndex)
        }
    }
    
    ModalNavigationDrawer(
        drawerState = drawerState,
        drawerContent = {
            // Force recomposition when drawer state changes or route changes
            val isDrawerOpen by remember { derivedStateOf { drawerState.isOpen } }
            val currentRoute = uiState.route
            
            // Key ensures recomposition when drawer opens or route changes
            key(isDrawerOpen, currentRoute) {
                FolderDrawerContent(
                    route = currentRoute,
                    onRouteSelected = { selectedRoute ->
                        val routeScript = selectedRoute.route_script()
                        // Use RequestRouteChange to handle confirmation dialog logic
                        stateHolder.handleEvent(MainUiEvent.RequestRouteChange(routeScript))
                        scope.launch {
                            drawerState.close()
                        }
                    },
                    modifier = Modifier.fillMaxHeight()
                )
            }
        }
    ) {
        Scaffold(
        topBar = {
            var showMenu by remember { mutableStateOf(false) }
            
            FarertTopAppBar(
                title = stringResource(R.string.title_main_view),
                onBackClick = {
                    println("DEBUG: Hamburger menu tapped, opening drawer")
                    scope.launch {
                        drawerState.open()
                        println("DEBUG: Drawer state opened")
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
                    // Show Osaka Kanjou Line menu only when available
                    println("DEBUG: MainScreen - osakakanDetour = ${uiState.osakakanDetour}")
                    if (uiState.osakakanDetour != MainUiState.OsakaKanDetour.DISABLE) {
                        println("DEBUG: MainScreen - Showing Osaka Kanjou Line menu")
                        DropdownMenuItem(
                            text = { 
                                Text(stringResource(
                                    if (uiState.osakakanDetour == MainUiState.OsakaKanDetour.FAR) {
                                        R.string.result_menu_osakakan_near
                                    } else {
                                        R.string.result_menu_osakakan_far
                                    }
                                ))
                            },
                            onClick = {
                                showMenu = false
                                onToggleOsakakanDetour()
                            }
                        )
                    }
                }
            )
        },
        bottomBar = {
            BottomNavigationBar(
                canGoBack = uiState.canGoBack,
                canReverse = uiState.canReverse,
                onBackPressed = { stateHolder.handleEvent(MainUiEvent.GoBack) },
                onReverseRoute = { 
                    println("DEBUG: Reverse button tapped")
                    stateHolder.handleEvent(MainUiEvent.ReverseRoute) 
                },
                onArchiveRoute = {
                    val routeScript = if (uiState.route.count <= 1) "" else uiState.route.route_script()
                    onNavigateToArchive(routeScript)
                }
            )
        }
    ) { paddingValues ->
        // Force recomposition when route changes
        val routeKey = remember(uiState.route.hashCode()) { uiState.route.hashCode() }
        
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(paddingValues)
                .padding(16.dp)
        ) {
            // 発駅表示カード
            if (uiState.route.count > 0) {
                Card(
                    onClick = { onNavigateToTerminalSelect() },
                    modifier = Modifier
                        .fillMaxWidth()
                        .then(
                            // Force recomposition when route changes
                            Modifier
                        ),
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
                            imageVector = Icons.Default.Train,
                            contentDescription = null,
                            modifier = Modifier.size(32.dp),
                            tint = MaterialTheme.colorScheme.onPrimaryContainer
                        )
                        Spacer(modifier = Modifier.width(16.dp))
                        Column(modifier = Modifier.weight(1f)) {
                            // Use a derived state that updates when route changes
                            val stationId = remember(uiState.route.hashCode()) {
                                if (uiState.route.count > 0) uiState.route.item(0).stationId() else 0
                            }
                            
                            Text(
                                text = if (stationId > 0) "発駅：${RouteUtil.StationNameEx(stationId)}" else "駅を選択",
                                style = MaterialTheme.typography.titleMedium,
                                color = MaterialTheme.colorScheme.onPrimaryContainer
                            )
                            Text(
                                text = if (stationId > 0) "　　　 ${RouteUtil.GetKanaFromStationId(stationId)}" else "発駅を選択してください",
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
            } else {
                // 駅未選択時の表示
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
                            imageVector = Icons.Default.Train,
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
                                text = "発駅を選択してください",
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
            }
            
            Spacer(modifier = Modifier.height(16.dp))
            
            // 経路リスト（発駅以降）
            LazyColumn(
                state = listState,
                modifier = Modifier.weight(1f),
                verticalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                if (uiState.route.count > 1) {
                    itemsIndexed(
                        items = (1 until uiState.route.count).toList(),
                        key = { _, index -> "${uiState.route.hashCode()}_$index" }
                    ) { _, n ->
                        RouteDetailItem(
                            route = uiState.route,
                            position = n,
                            onClick = { onNavigateToRouteDetail(n) }
                        )
                    }
                }
                
                // 最後の項目（経路追加ボタン）
                if (uiState.route.count > 0) {
                    item {
                        // Check if statusMessage is an error message
                        val isErrorMessage = uiState.statusMessage.isNotEmpty() && (
                            uiState.statusMessage.contains("会社線利用路線はこれ以上追加できません") ||
                            uiState.statusMessage.contains("不正な駅名が含まれています") ||
                            uiState.statusMessage.contains("不正な路線名が含まれています") ||
                            uiState.statusMessage.contains("経路不正") ||
                            uiState.statusMessage.contains("許可されていない会社線通過です") ||
                            uiState.statusMessage.contains("経路を算出できません") ||
                            uiState.statusMessage.contains("経路が重複しているため追加できません") ||
                            uiState.statusMessage.contains("開始駅へ戻るにはもう少し経路を指定してからにしてください")
                        )
                        
                        Card(
                            onClick = { onNavigateToRouteDetail(uiState.route.count - 1) },
                            modifier = Modifier.fillMaxWidth(),
                            colors = CardDefaults.cardColors(
                                containerColor = if (isErrorMessage) MaterialTheme.colorScheme.errorContainer
                                               else MaterialTheme.colorScheme.tertiaryContainer
                            ),
                            elevation = CardDefaults.cardElevation(defaultElevation = 2.dp)
                        ) {
                            Row(
                                modifier = Modifier
                                    .fillMaxWidth()
                                    .padding(16.dp),
                                verticalAlignment = Alignment.CenterVertically,
                                horizontalArrangement = Arrangement.Center
                            ) {
                                Icon(
                                    imageVector = if (uiState.statusMessage.isNotEmpty()) Icons.Default.Warning else Icons.Default.Add,
                                    contentDescription = null,
                                    modifier = Modifier.size(20.dp),
                                    tint = if (isErrorMessage) MaterialTheme.colorScheme.onErrorContainer
                                          else if (uiState.statusMessage.isNotEmpty()) MaterialTheme.colorScheme.error 
                                          else MaterialTheme.colorScheme.onTertiaryContainer
                                )
                                Spacer(modifier = Modifier.width(8.dp))
                                Text(
                                    text = if (uiState.statusMessage.isNotEmpty()) uiState.statusMessage 
                                          else "経路を追加",
                                    style = if (isErrorMessage) MaterialTheme.typography.bodyMedium.copy(fontWeight = FontWeight.Bold)
                                           else MaterialTheme.typography.bodyMedium,
                                    color = if (isErrorMessage) MaterialTheme.colorScheme.onErrorContainer
                                           else if (uiState.statusMessage.isNotEmpty()) MaterialTheme.colorScheme.error
                                           else MaterialTheme.colorScheme.onTertiaryContainer
                                )
                            }
                        }
                    }
                }
            }
            
            // 運賃・営業キロ情報の固定表示（クリックで結果詳細画面へ）
            uiState.fareInfo?.let { info ->
                Card(
                    onClick = { onNavigateToFareDetail() },
                    modifier = Modifier.fillMaxWidth(),
                    colors = CardDefaults.cardColors(
                        containerColor = MaterialTheme.colorScheme.secondaryContainer
                    ),
                    elevation = CardDefaults.cardElevation(defaultElevation = 6.dp)
                ) {
                    Column(
                        modifier = Modifier.padding(16.dp)
                    ) {
                        Row(
                            modifier = Modifier.fillMaxWidth(),
                            horizontalArrangement = Arrangement.SpaceBetween
                        ) {
                            Text(
                                text = "普通運賃　¥${fareNumStr(info.fare)}-",
                                style = MaterialTheme.typography.bodyMedium,
                                color = MaterialTheme.colorScheme.onSecondaryContainer
                            )
                            Text(
                                text = "有効日数　${info.ticketAvailDays}日",
                                style = MaterialTheme.typography.bodyMedium,
                                color = MaterialTheme.colorScheme.onSecondaryContainer
                            )
                        }
                        Row(
                            modifier = Modifier.fillMaxWidth(),
                            horizontalArrangement = Arrangement.SpaceBetween,
                            verticalAlignment = Alignment.CenterVertically
                        ) {
                            Text(
                                text = "営業キロ　${kmNumStr(info.totalSalesKm)} km.",
                                style = MaterialTheme.typography.bodyMedium,
                                color = MaterialTheme.colorScheme.onSecondaryContainer
                            )
                            Row(
                                verticalAlignment = Alignment.CenterVertically
                            ) {
                                Text(
                                    text = "詳細を見る",
                                    style = MaterialTheme.typography.bodySmall,
                                    color = MaterialTheme.colorScheme.onSecondaryContainer.copy(alpha = 0.7f)
                                )
                                Spacer(modifier = Modifier.width(4.dp))
                                Icon(
                                    imageVector = Icons.Default.ChevronRight,
                                    contentDescription = null,
                                    modifier = Modifier.size(16.dp),
                                    tint = MaterialTheme.colorScheme.onSecondaryContainer.copy(alpha = 0.7f)
                                )
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Route selection confirmation dialog
    if (uiState.showRouteConfirmDialog) {
        AlertDialog(
            onDismissRequest = {
                stateHolder.handleEvent(MainUiEvent.CancelRouteChange)
            },
            title = {
                Text(stringResource(R.string.main_alert_query_left_route_overwrite_title))
            },
            text = {
                Text(stringResource(R.string.main_alert_query_left_route_overwrite_mesg))
            },
            confirmButton = {
                TextButton(
                    onClick = {
                        stateHolder.handleEvent(MainUiEvent.ConfirmRouteChange)
                    }
                ) {
                    Text("Yes")
                }
            },
            dismissButton = {
                TextButton(
                    onClick = {
                        stateHolder.handleEvent(MainUiEvent.CancelRouteChange)
                    }
                ) {
                    Text("No")
                }
            }
        )
    }
    }
}

@Composable
fun RouteDetailItem(
    route: RouteList,
    position: Int,
    onClick: () -> Unit
) {
    Card(
        onClick = onClick,
        modifier = Modifier.fillMaxWidth(),
        colors = CardDefaults.cardColors(
            containerColor = MaterialTheme.colorScheme.surfaceVariant
        ),
        elevation = CardDefaults.cardElevation(defaultElevation = 2.dp)
    ) {
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(16.dp),
            verticalAlignment = Alignment.CenterVertically
        ) {
            // 路線番号アイコン
            Card(
                modifier = Modifier.size(40.dp),
                colors = CardDefaults.cardColors(
                    containerColor = MaterialTheme.colorScheme.secondary
                )
            ) {
                Box(contentAlignment = Alignment.Center, modifier = Modifier.fillMaxSize()) {
                    Text(
                        text = "$position",
                        style = MaterialTheme.typography.labelMedium,
                        color = MaterialTheme.colorScheme.onSecondary
                    )
                }
            }
            
            Spacer(modifier = Modifier.width(12.dp))
            
            Column(modifier = Modifier.weight(1f)) {
                Text(
                    text = RouteUtil.LineName(route.item(position).lineId()),
                    style = MaterialTheme.typography.titleMedium,
                    color = MaterialTheme.colorScheme.onSurfaceVariant
                )
                Text(
                    text = RouteUtil.StationName(route.item(position).stationId()),
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
            onClick = {
                println("DEBUG: NavigationBarItem reverse clicked, canReverse=$canReverse")
                onReverseRoute()
            },
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
    route: Route?,
    onRouteSelected: (RouteList) -> Unit,
    modifier: Modifier = Modifier
) {
    val context = LocalContext.current
    // Debug: Add a log to see if this is being called
    println("DEBUG: FolderDrawerContent called with route: ${route?.count ?: "null"}")
    
    ModalDrawerSheet(modifier = modifier) {
        // FolderViewScreenの内容を埋め込み
        FolderViewScreen(
            routefolder = (context.applicationContext as org.sutezo.farert.FarertApp).routefolder,
            route = route,
            onItemClick = onRouteSelected,
            onCloseDrawer = { /* ドロワーを閉じる処理はMainScreenで行う */ }
        )
    }
}

@Preview(showBackground = true)
@Composable
fun RouteListItemPreview() {
    MaterialTheme {
        // Create a mock RouteList for preview
        val mockRoute = RouteList().apply {
            // This would typically be initialized with actual data
        }
        
        RouteListItem(
            route = mockRoute,
            position = 0,
            isLast = false,
            statusMessage = "",
            onClick = {}
        )
    }
}

@Preview(showBackground = true)
@Composable
fun RouteListItemLastPreview() {
    MaterialTheme {
        val mockRoute = RouteList()
        
        RouteListItem(
            route = mockRoute,
            position = 1,
            isLast = true,
            statusMessage = "",
            onClick = {}
        )
    }
}

@Preview(showBackground = true)
@Composable
fun BottomNavigationBarPreview() {
    MaterialTheme {
        BottomNavigationBar(
            canGoBack = true,
            canReverse = true,
            onBackPressed = {},
            onReverseRoute = {},
            onArchiveRoute = {}
        )
    }
}