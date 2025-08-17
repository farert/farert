package org.sutezo.farert.ui.compose

import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.ArrowBack
import androidx.compose.material.icons.filled.MoreVert
import androidx.compose.material.icons.filled.Share
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.lifecycle.viewmodel.compose.viewModel
import org.sutezo.alps.*
import org.sutezo.farert.R
import org.sutezo.farert.ui.state.ResultViewUiState
import org.sutezo.farert.ui.state.ResultViewUiEvent
import org.sutezo.farert.ui.state.ResultViewStateHolder
import org.sutezo.farert.ui.components.FarertTopAppBar
import org.sutezo.farert.ui.components.farertTopAppBarColors

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun ResultViewScreen(
    routeEndIndex: Int,
    alertId: Int = -1,
    stocktokai: Int = ResultViewUiState.Option.N_A.ordinal,
    neerest: Int = ResultViewUiState.Option.N_A.ordinal,
    sperule: Int = ResultViewUiState.Option.N_A.ordinal,
    meihancity: Int = ResultViewUiState.Option.N_A.ordinal,
    longroute: Int = ResultViewUiState.Option.N_A.ordinal,
    rule115: Int = ResultViewUiState.Option.N_A.ordinal,
    stateHolder: ResultViewStateHolder = viewModel(),
    onNavigateUp: () -> Unit = {},
    onShare: (String, String) -> Unit = { _, _ -> }
) {
    val context = LocalContext.current
    val uiState = stateHolder.uiState
    var showOptionsMenu by remember { mutableStateOf(false) }
    
    // Initialize state
    LaunchedEffect(routeEndIndex, alertId, stocktokai, neerest, sperule, meihancity, longroute, rule115) {
        stateHolder.initialize(
            context = context,
            routeEndIndex = routeEndIndex,
            alertId = alertId,
            stocktokai = stocktokai,
            neerest = neerest,
            sperule = sperule,
            meihancity = meihancity,
            longroute = longroute,
            rule115 = rule115
        )
    }
    
    // Handle error display
    uiState.error?.let { error ->
        LaunchedEffect(error) {
            // Show error dialog or snackbar
            stateHolder.handleEvent(ResultViewUiEvent.ClearError)
        }
    }
    
    Scaffold(
        topBar = {
            TopAppBar(
                title = { Text(stringResource(R.string.title_main_menu_result)) },
                navigationIcon = {
                    IconButton(onClick = onNavigateUp) {
                        Icon(Icons.AutoMirrored.Filled.ArrowBack, contentDescription = "Back")
                    }
                },
                actions = {
                    IconButton(onClick = {
                        stateHolder.handleEvent(ResultViewUiEvent.ShareClicked)
                        stateHolder.getShareData()?.let { (subject, text) ->
                            onShare(subject, text)
                        }
                    }) {
                        Icon(Icons.Default.Share, contentDescription = "Share")
                    }
                    
                    Box {
                        IconButton(onClick = { showOptionsMenu = true }) {
                            Icon(Icons.Default.MoreVert, contentDescription = "Options")
                        }
                        
                        DropdownMenu(
                            expanded = showOptionsMenu,
                            onDismissRequest = { showOptionsMenu = false }
                        ) {
                            if (uiState.showSpecialRuleMenu) {
                                DropdownMenuItem(
                                    text = { Text(uiState.specialRuleMenuTitle) },
                                    onClick = {
                                        showOptionsMenu = false
                                        stateHolder.handleEvent(ResultViewUiEvent.SpecialRuleClicked)
                                    }
                                )
                            }
                            
                            if (uiState.showStocktokaiMenu) {
                                DropdownMenuItem(
                                    text = { Text(uiState.stocktokaiMenuTitle) },
                                    onClick = {
                                        showOptionsMenu = false
                                        stateHolder.handleEvent(ResultViewUiEvent.StocktokaiClicked)
                                    }
                                )
                            }
                            
                            if (uiState.showMeihanCityMenu) {
                                DropdownMenuItem(
                                    text = { Text(uiState.meihanCityMenuTitle) },
                                    onClick = {
                                        showOptionsMenu = false
                                        stateHolder.handleEvent(ResultViewUiEvent.MeihanCityClicked)
                                    }
                                )
                            }
                            
                            if (uiState.showLongRouteMenu) {
                                DropdownMenuItem(
                                    text = { Text(uiState.longRouteMenuTitle) },
                                    onClick = {
                                        showOptionsMenu = false
                                        stateHolder.handleEvent(ResultViewUiEvent.LongRouteClicked)
                                    }
                                )
                            }
                            
                            if (uiState.showRule115Menu) {
                                DropdownMenuItem(
                                    text = { Text(uiState.rule115MenuTitle) },
                                    onClick = {
                                        showOptionsMenu = false
                                        stateHolder.handleEvent(ResultViewUiEvent.Rule115Clicked)
                                    }
                                )
                            }
                            
                            if (uiState.showOsakakanMenu) {
                                DropdownMenuItem(
                                    text = { Text(uiState.osakakanMenuTitle) },
                                    onClick = {
                                        showOptionsMenu = false
                                        stateHolder.handleEvent(ResultViewUiEvent.OsakakanClicked)
                                    }
                                )
                            }
                        }
                    }
                },
                colors = farertTopAppBarColors()
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
            val fareInfo = uiState.fareInfo
            if (fareInfo != null) {
                if (fareInfo.result != 0) {
                    // Error case
                    ErrorContent(
                        modifier = Modifier
                            .fillMaxSize()
                            .padding(paddingValues),
                        fareInfo = fareInfo
                    )
                } else {
                    // Success case
                    ResultContent(
                        modifier = Modifier
                            .fillMaxSize()
                            .padding(paddingValues),
                        fareInfo = fareInfo,
                        isRoundTrip = uiState.isRoundTrip
                    )
                }
            }
        }
    }
}

@Composable
private fun ErrorContent(
    modifier: Modifier = Modifier,
    fareInfo: org.sutezo.alps.FareInfo
) {
    Box(
        modifier = modifier,
        contentAlignment = Alignment.Center
    ) {
        Text(
            text = when (fareInfo.result) {
                1 -> stringResource(R.string.result_err_1)
                else -> stringResource(R.string.result_err_other)
            },
            style = MaterialTheme.typography.headlineSmall,
            color = MaterialTheme.colorScheme.error
        )
    }
}

@Composable
private fun ResultContent(
    modifier: Modifier = Modifier,
    fareInfo: org.sutezo.alps.FareInfo,
    isRoundTrip: Boolean
) {
    Column(
        modifier = modifier
            .verticalScroll(rememberScrollState())
            .padding(16.dp)
    ) {
        // Section header
        Card(
            modifier = Modifier.fillMaxWidth()
        ) {
            Column(
                modifier = Modifier.padding(16.dp)
            ) {
                Text(
                    text = stringResource(
                        R.string.result_value_section,
                        terminalName(fareInfo.beginStationId),
                        terminalName(fareInfo.endStationId)
                    ),
                    style = MaterialTheme.typography.headlineSmall,
                    fontWeight = FontWeight.Bold
                )
            }
        }
        
        Spacer(modifier = Modifier.height(16.dp))
        
        // Distance information
        DistanceSection(fareInfo = fareInfo)
        
        Spacer(modifier = Modifier.height(16.dp))
        
        // Fare information
        FareSection(fareInfo = fareInfo, isRoundTrip = isRoundTrip)
        
        Spacer(modifier = Modifier.height(16.dp))
        
        // Validity information
        ValiditySection(fareInfo = fareInfo)
        
        Spacer(modifier = Modifier.height(16.dp))
        
        // Route information
        RouteSection(fareInfo = fareInfo)
        
        // Notes
        if (fareInfo.resultMessage.isNotBlank()) {
            Spacer(modifier = Modifier.height(16.dp))
            Card(
                modifier = Modifier.fillMaxWidth()
            ) {
                Column(
                    modifier = Modifier.padding(16.dp)
                ) {
                    Text(
                        text = "備考",
                        style = MaterialTheme.typography.titleMedium,
                        fontWeight = FontWeight.Bold
                    )
                    Spacer(modifier = Modifier.height(8.dp))
                    Text(
                        text = fareInfo.resultMessage,
                        style = MaterialTheme.typography.bodyMedium
                    )
                }
            }
        }
    }
}

@Composable
private fun DistanceSection(fareInfo: org.sutezo.alps.FareInfo) {
    Card(
        modifier = Modifier.fillMaxWidth()
    ) {
        Column(
            modifier = Modifier.padding(16.dp)
        ) {
            Text(
                text = "キロ程",
                style = MaterialTheme.typography.titleMedium,
                fontWeight = FontWeight.Bold
            )
            Spacer(modifier = Modifier.height(8.dp))
            
            Text(
                text = stringResource(R.string.result_km, kmNumStr(fareInfo.totalSalesKm)),
                style = MaterialTheme.typography.bodyLarge
            )
            
            if (fareInfo.jrCalcKm != fareInfo.jrSalesKm) {
                Text(
                    text = stringResource(
                        if (fareInfo.companySalesKm != 0) R.string.result_calckm else R.string.result_calckm_wo_jr
                    ),
                    style = MaterialTheme.typography.bodyMedium
                )
                Text(
                    text = stringResource(R.string.result_km, kmNumStr(fareInfo.jrCalcKm)),
                    style = MaterialTheme.typography.bodyMedium
                )
            }
            
            // 規程114条適用時の営業キロ表示
            if (fareInfo.isRule114Applied) {
                Spacer(modifier = Modifier.height(4.dp))
                Text(
                    text = "規程114条適用 営業キロ： ${kmNumStr(fareInfo.rule114_salesKm)} km 計算キロ： ${kmNumStr(fareInfo.rule114_calcKm)} km",
                    style = MaterialTheme.typography.bodyMedium
                )
            }
            
            // JR北海道営業キロ表示
            if (fareInfo.salesKmForHokkaido > 0) {
                Spacer(modifier = Modifier.height(4.dp))
                if (fareInfo.calcKmForHokkaido > 0 && fareInfo.calcKmForHokkaido != fareInfo.salesKmForHokkaido) {
                    Text(
                        text = "JR北海道営業キロ： ${kmNumStr(fareInfo.salesKmForHokkaido)} km 計算キロ： ${kmNumStr(fareInfo.calcKmForHokkaido)} km",
                        style = MaterialTheme.typography.bodyMedium
                    )
                } else {
                    Text(
                        text = "JR北海道営業キロ： ${kmNumStr(fareInfo.salesKmForHokkaido)} km",
                        style = MaterialTheme.typography.bodyMedium
                    )
                }
            }
            
            // JR四国営業キロ表示
            if (fareInfo.salesKmForShikoku > 0) {
                Spacer(modifier = Modifier.height(4.dp))
                if (fareInfo.calcKmForShikoku > 0 && fareInfo.salesKmForShikoku != fareInfo.calcKmForShikoku) {
                    Text(
                        text = "JR四国営業キロ： ${kmNumStr(fareInfo.salesKmForShikoku)} km 計算キロ： ${kmNumStr(fareInfo.calcKmForShikoku)} km",
                        style = MaterialTheme.typography.bodyMedium
                    )
                } else {
                    Text(
                        text = "JR四国営業キロ： ${kmNumStr(fareInfo.salesKmForShikoku)} km",
                        style = MaterialTheme.typography.bodyMedium
                    )
                }
            }
            
            // JR九州営業キロ表示
            if (fareInfo.salesKmForKyusyu > 0) {
                Spacer(modifier = Modifier.height(4.dp))
                if (fareInfo.calcKmForKyusyu > 0 && fareInfo.salesKmForKyusyu != fareInfo.calcKmForKyusyu) {
                    Text(
                        text = "JR九州営業キロ： ${kmNumStr(fareInfo.salesKmForKyusyu)} km  計算キロ： ${kmNumStr(fareInfo.calcKmForKyusyu)} km",
                        style = MaterialTheme.typography.bodyMedium
                    )
                } else {
                    Text(
                        text = "JR九州営業キロ： ${kmNumStr(fareInfo.salesKmForKyusyu)} km",
                        style = MaterialTheme.typography.bodyMedium
                    )
                }
            }
            
            // BRT営業キロ表示
            if (fareInfo.brtSalesKm > 0) {
                Spacer(modifier = Modifier.height(4.dp))
                Text(
                    text = "BRT営業キロ： ${kmNumStr(fareInfo.brtSalesKm)} km",
                    style = MaterialTheme.typography.bodyMedium
                )
            }
        }
    }
}

@Composable
private fun FareSection(fareInfo: org.sutezo.alps.FareInfo, isRoundTrip: Boolean) {
    Card(
        modifier = Modifier.fillMaxWidth()
    ) {
        Column(
            modifier = Modifier.padding(16.dp)
        ) {
            Text(
                text = stringResource(R.string.result_fare),
                style = MaterialTheme.typography.titleMedium,
                fontWeight = FontWeight.Bold
            )
            Spacer(modifier = Modifier.height(8.dp))
            
            // Normal fare
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceBetween
            ) {
                Text(
                    text = "普通運賃",
                    style = MaterialTheme.typography.bodyMedium
                )
                Text(
                    text = stringResource(R.string.result_yen, fareNumStr(fareInfo.fare)),
                    style = MaterialTheme.typography.bodyLarge,
                    fontWeight = FontWeight.Bold
                )
            }
            
            // Company line fare if applicable
            if (fareInfo.fareForCompanyline != 0) {
                Spacer(modifier = Modifier.height(4.dp))
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text(
                        text = stringResource(R.string.result_companyfare),
                        style = MaterialTheme.typography.bodyMedium
                    )
                    Text(
                        text = stringResource(R.string.result_yen, fareNumStr(fareInfo.fareForCompanyline)),
                        style = MaterialTheme.typography.bodyLarge
                    )
                }
            }
            
            // BRT fare if applicable
            if (fareInfo.fareForBRT != 0) {
                Spacer(modifier = Modifier.height(4.dp))
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text(
                        text = "BRT運賃",
                        style = MaterialTheme.typography.bodyMedium
                    )
                    Text(
                        text = stringResource(R.string.result_yen, fareNumStr(fareInfo.fareForBRT)),
                        style = MaterialTheme.typography.bodyLarge
                    )
                }
            }
            
            // IC fare if applicable
            if (fareInfo.fareForIC != 0) {
                Spacer(modifier = Modifier.height(4.dp))
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text(
                        text = stringResource(R.string.result_icfare),
                        style = MaterialTheme.typography.bodyMedium
                    )
                    Text(
                        text = stringResource(R.string.result_yen, fareNumStr(fareInfo.fareForIC)),
                        style = MaterialTheme.typography.bodyLarge
                    )
                }
            }
            
            // Round trip fare if applicable
            if (isRoundTrip) {
                Spacer(modifier = Modifier.height(8.dp))
                HorizontalDivider()
                Spacer(modifier = Modifier.height(8.dp))
                
                val discountText = if (fareInfo.isRoundtripDiscount) {
                    stringResource(R.string.result_value_discount)
                } else {
                    ""
                }
                
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text(
                        text = "往復運賃",
                        style = MaterialTheme.typography.bodyMedium
                    )
                    Text(
                        text = "${stringResource(R.string.result_yen, fareNumStr(fareInfo.roundTripFareWithCompanyLine))}$discountText",
                        style = MaterialTheme.typography.bodyLarge,
                        fontWeight = FontWeight.Bold
                    )
                }
            }
            
            // Child fare
            Spacer(modifier = Modifier.height(8.dp))
            HorizontalDivider()
            Spacer(modifier = Modifier.height(8.dp))
            
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceBetween
            ) {
                Text(
                    text = "小児運賃",
                    style = MaterialTheme.typography.bodyMedium
                )
                Text(
                    text = stringResource(R.string.result_yen, fareNumStr(fareInfo.childFare)),
                    style = MaterialTheme.typography.bodyLarge
                )
            }
            
            if (isRoundTrip) {
                Spacer(modifier = Modifier.height(4.dp))
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text(
                        text = "小児往復運賃",
                        style = MaterialTheme.typography.bodyMedium
                    )
                    Text(
                        text = stringResource(R.string.result_yen, fareNumStr(fareInfo.roundtripChildFare)),
                        style = MaterialTheme.typography.bodyLarge
                    )
                }
            }
            
            // Academic discount if applicable
            if (fareInfo.isAcademicFare) {
                Spacer(modifier = Modifier.height(8.dp))
                HorizontalDivider()
                Spacer(modifier = Modifier.height(8.dp))
                
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text(
                        text = "学割運賃",
                        style = MaterialTheme.typography.bodyMedium
                    )
                    Text(
                        text = stringResource(R.string.result_yen, fareNumStr(fareInfo.academicFare)),
                        style = MaterialTheme.typography.bodyLarge
                    )
                }
                
                if (isRoundTrip) {
                    Spacer(modifier = Modifier.height(4.dp))
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.SpaceBetween
                    ) {
                        Text(
                            text = "学割往復運賃",
                            style = MaterialTheme.typography.bodyMedium
                        )
                        Text(
                            text = stringResource(R.string.result_yen, fareNumStr(fareInfo.roundtripAcademicFare)),
                            style = MaterialTheme.typography.bodyLarge
                        )
                    }
                }
            }
            
            // Stock discounts if applicable
            fareInfo.fareForStockDiscounts?.let { stocks ->
                if (stocks.isNotEmpty()) {
                    Spacer(modifier = Modifier.height(8.dp))
                    HorizontalDivider()
                    Spacer(modifier = Modifier.height(8.dp))
                    
                    stocks.forEach { (companyTitle, normalFare, rule114Fare) ->
                        val fareToShow = if (fareInfo.isRule114Applied && rule114Fare > 0) {
                            rule114Fare
                        } else {
                            normalFare
                        }
                        
                        if (fareToShow > 0) {
                            Row(
                                modifier = Modifier.fillMaxWidth(),
                                horizontalArrangement = Arrangement.SpaceBetween
                            ) {
                                Text(
                                    text = companyTitle,
                                    style = MaterialTheme.typography.bodyMedium
                                )
                                Text(
                                    text = stringResource(R.string.result_yen, fareNumStr(fareToShow)),
                                    style = MaterialTheme.typography.bodyLarge
                                )
                            }
                            Spacer(modifier = Modifier.height(4.dp))
                        }
                    }
                }
            }
        }
    }
}

@Composable
private fun ValiditySection(fareInfo: org.sutezo.alps.FareInfo) {
    Card(
        modifier = Modifier.fillMaxWidth()
    ) {
        Column(
            modifier = Modifier.padding(16.dp)
        ) {
            Text(
                text = "有効日数",
                style = MaterialTheme.typography.titleMedium,
                fontWeight = FontWeight.Bold
            )
            Spacer(modifier = Modifier.height(8.dp))
            
            Text(
                text = stringResource(R.string.result_availdays_fmt, fareInfo.ticketAvailDays),
                style = MaterialTheme.typography.bodyLarge
            )
            
            val availabilityText = if (fareInfo.ticketAvailDays > 1) {
                val exceptText = when {
                    fareInfo.isBeginInCity && fareInfo.isEndInCity -> {
                        stringResource(R.string.result_availday_except_begin_end)
                    }
                    fareInfo.isBeginInCity -> {
                        stringResource(R.string.result_availday_except_begin)
                    }
                    fareInfo.isEndInCity -> {
                        stringResource(R.string.result_availday_except_end)
                    }
                    else -> {
                        stringResource(R.string.result_availday_empty)
                    }
                }
                stringResource(R.string.result_availday_stopover, exceptText)
            } else {
                stringResource(R.string.result_availday_dontstopover)
            }
            
            Text(
                text = availabilityText,
                style = MaterialTheme.typography.bodyMedium,
                color = MaterialTheme.colorScheme.onSurfaceVariant
            )
        }
    }
}

@Composable
private fun RouteSection(fareInfo: org.sutezo.alps.FareInfo) {
    Card(
        modifier = Modifier.fillMaxWidth()
    ) {
        Column(
            modifier = Modifier.padding(16.dp)
        ) {
            Text(
                text = "経由",
                style = MaterialTheme.typography.titleMedium,
                fontWeight = FontWeight.Bold
            )
            Spacer(modifier = Modifier.height(8.dp))
            
            var routeText = fareInfo.routeList
            if (fareInfo.routeListForTOICA.isNotEmpty()) {
                routeText += "\n${stringResource(R.string.result_toica_calc_route)}\n${fareInfo.routeListForTOICA}"
            }
            
            Text(
                text = routeText,
                style = MaterialTheme.typography.bodyMedium
            )
        }
    }
}