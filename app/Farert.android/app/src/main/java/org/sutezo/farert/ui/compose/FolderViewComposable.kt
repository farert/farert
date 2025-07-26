package org.sutezo.farert.ui.compose

import android.content.Context
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.itemsIndexed
import androidx.compose.foundation.lazy.rememberLazyListState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.material.icons.automirrored.filled.KeyboardArrowRight
import androidx.compose.material3.*
import androidx.compose.material3.MenuAnchorType
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.colorResource
import androidx.compose.ui.res.stringArrayResource
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.core.app.ShareCompat
import org.sutezo.alps.RouteList
import org.sutezo.alps.fareNumStr
import org.sutezo.alps.kmNumStr
import org.sutezo.alps.terminalName
import org.sutezo.farert.R
import org.sutezo.farert.Routefolder

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun FolderViewScreen(
    routefolder: Routefolder,
    route: RouteList?,
    onItemClick: (RouteList) -> Unit,
    onCloseDrawer: () -> Unit
) {
    val context = LocalContext.current
    var checkedItems by remember { mutableStateOf(setOf<Int>()) }
    var allChecked by remember { mutableStateOf(false) }
    
    // Reload when routefolder changes
    LaunchedEffect(routefolder.count()) {
        checkedItems = setOf()
        allChecked = false
    }
    
    Column(
        modifier = Modifier
            .fillMaxSize()
            .background(colorResource(R.color.colorLeftHeadEnd))
    ) {
        // Header Section
        FolderHeader(
            routefolder = routefolder,
            route = route,
            allChecked = allChecked,
            onAllCheckedChange = { checked ->
                allChecked = checked
                checkedItems = if (checked) {
                    (0 until routefolder.count()).toSet()
                } else {
                    setOf()
                }
            },
            onAddItem = {
                route?.let {
                    val rl = RouteList(it)
                    routefolder.add(context, rl)
                }
            },
            onDeleteItems = {
                if (checkedItems.isNotEmpty()) {
                    // Delete in reverse order to maintain indices
                    checkedItems.sortedDescending().forEach { index ->
                        routefolder.remove(context, index)
                    }
                    checkedItems = setOf()
                    allChecked = false
                }
            },
            onExport = {
                val text = routefolder.makeExportText()
                val shareIntent = ShareCompat.IntentBuilder(context as androidx.fragment.app.FragmentActivity)
                    .setChooserTitle(context.getString(R.string.title_share_text))
                    .setType("text/plain")
                    .setText(text)
                    .setSubject("チケットフォルダ")
                    .intent
                
                if (shareIntent.resolveActivity(context.packageManager) != null) {
                    context.startActivity(shareIntent)
                }
            }
        )
        
        // List Section
        LazyColumn(
            state = rememberLazyListState(),
            modifier = Modifier.fillMaxSize()
        ) {
            itemsIndexed(
                items = (0 until routefolder.count()).map { routefolder.routeItem(it) },
                key = { index, _ -> index }
            ) { index, routeItem ->
                FolderListItem(
                    index = index,
                    routeItem = routeItem,
                    routefolder = routefolder,
                    isChecked = checkedItems.contains(index),
                    onCheckedChange = { checked ->
                        checkedItems = if (checked) {
                            checkedItems + index
                        } else {
                            checkedItems - index
                        }
                        allChecked = checkedItems.size == routefolder.count()
                    },
                    onItemClick = {
                        onItemClick(routeItem)
                        onCloseDrawer()
                    },
                    onAggregateTypeChange = { newType ->
                        routefolder.setAggregateType(context, index, newType)
                    }
                )
            }
        }
    }
}

@Composable
private fun FolderHeader(
    routefolder: Routefolder,
    route: RouteList?,
    allChecked: Boolean,
    onAllCheckedChange: (Boolean) -> Unit,
    onAddItem: () -> Unit,
    onDeleteItems: () -> Unit,
    onExport: () -> Unit
) {
    Column(
        modifier = Modifier
            .fillMaxWidth()
            .background(colorResource(R.color.colorLeftHeadEnd))
            .padding(16.dp)
    ) {
        // Header with icon and title
        Row(
            verticalAlignment = Alignment.CenterVertically,
            modifier = Modifier.fillMaxWidth()
        ) {
            Icon(
                imageVector = Icons.Default.FolderOpen,
                contentDescription = null,
                tint = Color.White,
                modifier = Modifier.size(32.dp)
            )
            
            Spacer(modifier = Modifier.width(8.dp))
            
            Text(
                text = stringResource(R.string.nav_header_title),
                color = Color.White,
                fontSize = 18.sp,
                fontWeight = FontWeight.Bold
            )
        }
        
        Spacer(modifier = Modifier.height(8.dp))
        
        // Total fare and distance
        Row(
            modifier = Modifier.fillMaxWidth()
        ) {
            Text(
                text = stringResource(R.string.nav_header_total_fare_title),
                color = Color.White,
                fontSize = 14.sp
            )
            Spacer(modifier = Modifier.width(8.dp))
            Text(
                text = routefolder.totalFare(),
                color = Color.White,
                fontSize = 14.sp,
                fontWeight = FontWeight.Bold
            )
        }
        
        Row(
            modifier = Modifier.fillMaxWidth()
        ) {
            Text(
                text = stringResource(R.string.nav_header_total_km_title),
                color = Color.White,
                fontSize = 14.sp
            )
            Spacer(modifier = Modifier.width(8.dp))
            Text(
                text = routefolder.totalSalesKm(),
                color = Color.White,
                fontSize = 14.sp,
                fontWeight = FontWeight.Bold
            )
        }
        
        Spacer(modifier = Modifier.height(16.dp))
        
        // Action buttons
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Row(verticalAlignment = Alignment.CenterVertically) {
                Checkbox(
                    checked = allChecked,
                    onCheckedChange = onAllCheckedChange,
                    colors = CheckboxDefaults.colors(
                        checkmarkColor = Color.White,
                        checkedColor = colorResource(R.color.colorChkBoxFolder)
                    )
                )
                
                TextButton(
                    onClick = onDeleteItems,
                    enabled = routefolder.count() > 0
                ) {
                    Text(
                        text = stringResource(R.string.btn_del),
                        color = colorResource(R.color.colorTicketTypeText)
                    )
                }
            }
            
            Row {
                IconButton(
                    onClick = onExport,
                    enabled = routefolder.count() > 0
                ) {
                    Icon(
                        imageVector = Icons.Default.Share,
                        contentDescription = "Export",
                        tint = colorResource(R.color.colorTicketTypeText)
                    )
                }
                
                TextButton(
                    onClick = onAddItem,
                    enabled = route != null
                ) {
                    Text(
                        text = stringResource(R.string.btn_add),
                        color = colorResource(R.color.colorTicketTypeText)
                    )
                }
            }
        }
    }
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
private fun FolderListItem(
    index: Int,
    routeItem: RouteList,
    routefolder: Routefolder,
    isChecked: Boolean,
    onCheckedChange: (Boolean) -> Unit,
    onItemClick: () -> Unit,
    onAggregateTypeChange: (Routefolder.Aggregate) -> Unit
) {
    val context = LocalContext.current
    val fareTypes = stringArrayResource(R.array.list_ticket_type)
    var expanded by remember { mutableStateOf(false) }
    var selectedFareType by remember { 
        mutableStateOf(routefolder.aggregateType(index).ordinal)
    }
    
    Card(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 8.dp, vertical = 4.dp)
            .clickable { onItemClick() },
        colors = CardDefaults.cardColors(
            containerColor = colorResource(R.color.colorLeftViewBackground)
        )
    ) {
        Column(
            modifier = Modifier.padding(12.dp)
        ) {
            // Route title
            Row(
                modifier = Modifier.fillMaxWidth(),
                verticalAlignment = Alignment.CenterVertically
            ) {
                Checkbox(
                    checked = isChecked,
                    onCheckedChange = onCheckedChange,
                    modifier = Modifier.padding(end = 8.dp)
                )
                
                Text(
                    text = "${terminalName(routeItem.departureStationId())} - ${terminalName(routeItem.arriveStationId())}",
                    fontWeight = FontWeight.Bold,
                    fontSize = 14.sp,
                    modifier = Modifier.weight(1f)
                )
                
                Row(
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Icon(
                        imageVector = Icons.Default.DirectionsTransit,
                        contentDescription = null,
                        modifier = Modifier.size(16.dp),
                        tint = Color.Gray
                    )
                    Spacer(modifier = Modifier.width(8.dp))
                    Icon(
                        imageVector = Icons.AutoMirrored.Filled.KeyboardArrowRight,
                        contentDescription = null,
                        modifier = Modifier.size(16.dp),
                        tint = Color.Gray
                    )
                }
            }
            
            Spacer(modifier = Modifier.height(8.dp))
            
            // Fare type dropdown and fare info
            Row(
                modifier = Modifier.fillMaxWidth(),
                verticalAlignment = Alignment.CenterVertically,
                horizontalArrangement = Arrangement.SpaceBetween
            ) {
                // Fare type dropdown
                ExposedDropdownMenuBox(
                    expanded = expanded,
                    onExpandedChange = { expanded = !expanded },
                    modifier = Modifier.width(120.dp)
                ) {
                    OutlinedTextField(
                        value = fareTypes.getOrNull(selectedFareType) ?: "",
                        onValueChange = {},
                        readOnly = true,
                        trailingIcon = {
                            ExposedDropdownMenuDefaults.TrailingIcon(expanded = expanded)
                        },
                        modifier = Modifier.menuAnchor(MenuAnchorType.PrimaryNotEditable, true),
                        textStyle = LocalTextStyle.current.copy(fontSize = 12.sp)
                    )
                    
                    ExposedDropdownMenu(
                        expanded = expanded,
                        onDismissRequest = { expanded = false }
                    ) {
                        fareTypes.forEachIndexed { typeIndex, fareType ->
                            DropdownMenuItem(
                                text = { Text(fareType, fontSize = 12.sp) },
                                onClick = {
                                    selectedFareType = typeIndex
                                    expanded = false
                                    val aggregateType = Routefolder.Aggregate.values()[typeIndex]
                                    onAggregateTypeChange(aggregateType)
                                }
                            )
                        }
                    }
                }
                
                // Fare and distance info
                Column(
                    horizontalAlignment = Alignment.End
                ) {
                    val fareValue = routefolder.routeItemFareKm(index)
                    Text(
                        text = "¥${fareNumStr(fareValue.first)}",
                        fontWeight = FontWeight.Bold,
                        fontSize = 14.sp
                    )
                    Text(
                        text = "${kmNumStr(fareValue.second)}km",
                        fontSize = 12.sp,
                        color = Color.Gray
                    )
                }
            }
        }
    }
}