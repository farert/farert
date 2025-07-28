package org.sutezo.farert.ui.compose

import android.content.Context
import android.content.Intent
import androidx.compose.foundation.background
import androidx.compose.ui.graphics.Brush
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.itemsIndexed
import androidx.compose.foundation.lazy.rememberLazyListState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.gestures.detectDragGestures
import androidx.compose.foundation.interaction.MutableInteractionSource
import androidx.compose.foundation.interaction.collectIsDraggedAsState
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.material.icons.automirrored.filled.KeyboardArrowRight
import androidx.compose.material3.*
import androidx.compose.material3.MenuAnchorType
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.draw.scale
import androidx.compose.ui.draw.shadow
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.colorResource
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.res.stringArrayResource
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.geometry.Offset
import kotlinx.coroutines.delay
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
    var refreshTrigger by remember { mutableStateOf(0) }
    var itemCount by remember { mutableStateOf(routefolder.count()) }
    var draggedItem by remember { mutableStateOf<Int?>(null) }
    
    // Reload when routefolder changes
    LaunchedEffect(refreshTrigger) {
        itemCount = routefolder.count()
    }
    
    Column(
        modifier = Modifier
            .fillMaxSize()
            .background(
                Brush.horizontalGradient(
                    colors = listOf(
                        Color(0xFF2D1B69), // Rich purple
                        Color(0xFF11998e), // Teal  
                        Color(0xFF38ef7d)  // Light green
                    )
                )
            )
    ) {
        // Header Section
        FolderHeader(
            routefolder = routefolder,
            route = route,
            itemCount = itemCount,
            onAddItem = {
                route?.let {
                    val rl = RouteList(it)
                    routefolder.add(context, rl)
                    refreshTrigger++
                }
            },
            onExport = {
                val text = routefolder.makeExportText()
                val shareIntent = Intent().apply {
                    action = Intent.ACTION_SEND
                    type = "text/plain"
                    putExtra(Intent.EXTRA_TEXT, text)
                    putExtra(Intent.EXTRA_SUBJECT, "チケットフォルダ")
                }
                
                val chooserIntent = Intent.createChooser(
                    shareIntent, 
                    context.getString(R.string.title_share_text)
                )
                
                if (chooserIntent.resolveActivity(context.packageManager) != null) {
                    context.startActivity(chooserIntent)
                }
            }
        )
        
        // List Section
        LazyColumn(
            state = rememberLazyListState(),
            modifier = Modifier.fillMaxSize()
        ) {
            itemsIndexed(
                items = (0 until itemCount).map { routefolder.routeItem(it) },
                key = { index, _ -> index }
            ) { index, routeItem ->
                FolderListItem(
                    index = index,
                    routeItem = routeItem,
                    routefolder = routefolder,
                    onItemClick = {
                        onItemClick(routeItem)
                        onCloseDrawer()
                    },
                    onRemove = {
                        routefolder.remove(context, index)
                        refreshTrigger++
                    },
                    onAggregateTypeChange = { newType ->
                        routefolder.setAggregateType(context, index, newType)
                    },
                    onDragStart = { draggedItem = index },
                    onDragEnd = { draggedItem = null },
                    onSwap = { fromIndex, toIndex ->
                        if (routefolder.swap(context, fromIndex, toIndex)) {
                            refreshTrigger++
                        }
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
    itemCount: Int,
    onAddItem: () -> Unit,
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
                painter = painterResource(R.drawable.tickfolder3),
                contentDescription = stringResource(R.string.nav_header_title),
                modifier = Modifier.size(32.dp),
                tint = Color.Unspecified
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
            // Left side - Export button
            IconButton(
                onClick = onExport,
                enabled = itemCount > 0
            ) {
                Icon(
                    imageVector = Icons.Default.Share,
                    contentDescription = "Export",
                    tint = colorResource(R.color.colorTicketTypeText)
                )
            }

            // Right side - Add button
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

@OptIn(ExperimentalMaterial3Api::class)
@Composable
private fun FolderListItem(
    index: Int,
    routeItem: RouteList,
    routefolder: Routefolder,
    onItemClick: () -> Unit,
    onRemove: () -> Unit,
    onAggregateTypeChange: (Routefolder.Aggregate) -> Unit,
    onDragStart: () -> Unit = {},
    onDragEnd: () -> Unit = {},
    onSwap: (Int, Int) -> Unit = { _, _ -> }
) {
    val fareTypes = stringArrayResource(R.array.list_ticket_type)
    var expanded by remember { mutableStateOf(false) }
    var selectedFareType by remember { 
        mutableIntStateOf(routefolder.aggregateType(index).ordinal)
    }
    var isDragging by remember { mutableStateOf(false) }
    var dragOffset by remember { mutableStateOf(Offset.Zero) }
    
    Card(
        onClick = onItemClick,
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 8.dp, vertical = 4.dp)
            .scale(if (isDragging) 1.05f else 1f)
            .shadow(
                elevation = if (isDragging) 8.dp else 4.dp,
                shape = RoundedCornerShape(12.dp)
            )
            .background(
                Brush.verticalGradient(
                    colors = if (isDragging) listOf(
                        Color(0xFF2A2A3E), // Lighter when dragging
                        Color(0xFF26314E), 
                        Color(0xFF1F4470), 
                        Color(0xFF634493)  
                    ) else listOf(
                        Color(0xFF1A1A2E), // Deep dark blue
                        Color(0xFF16213E), // Dark navy
                        Color(0xFF0F3460), // Rich blue
                        Color(0xFF533483)  // Deep purple
                    )
                ),
                shape = RoundedCornerShape(12.dp)
            ),
        colors = CardDefaults.cardColors(
            containerColor = Color.Transparent
        ),
        shape = RoundedCornerShape(12.dp)
    ) {
        Column(
            modifier = Modifier.padding(12.dp)
        ) {
            // Route title
            Row(
                modifier = Modifier.fillMaxWidth(),
                verticalAlignment = Alignment.CenterVertically
            ) {
                // Drag handle
                Icon(
                    imageVector = Icons.Default.DragHandle,
                    contentDescription = "Drag handle",
                    tint = if (isDragging) Color.Yellow else Color.White.copy(alpha = 0.7f),
                    modifier = Modifier
                        .size(24.dp)
                        .padding(end = 8.dp)
                        .pointerInput(Unit) {
                            detectDragGestures(
                                onDragStart = { 
                                    isDragging = true
                                    onDragStart() 
                                },
                                onDragEnd = { 
                                    isDragging = false
                                    dragOffset = Offset.Zero
                                    onDragEnd() 
                                }
                            ) { change, dragAmount ->
                                dragOffset += dragAmount
                                // Simple drag threshold for swapping
                                if (dragOffset.y > 100f) {
                                    // Drag down - swap with next item
                                    onSwap(index, index + 1)
                                    dragOffset = Offset.Zero
                                } else if (dragOffset.y < -100f) {
                                    // Drag up - swap with previous item
                                    onSwap(index, index - 1)
                                    dragOffset = Offset.Zero
                                }
                            }
                        }
                )
                
                IconButton(
                    onClick = { onRemove() },
                    modifier = Modifier.size(32.dp)
                ) {
                    Icon(
                        imageVector = Icons.Default.Delete,
                        contentDescription = "Delete",
                        tint = Color.White,
                        modifier = Modifier.size(20.dp)
                    )
                }
                
                Text(
                    text = "${terminalName(routeItem.departureStationId())} - ${terminalName(routeItem.arriveStationId())}",
                    fontWeight = FontWeight.Bold,
                    fontSize = 14.sp,
                    color = Color.White,
                    modifier = Modifier.weight(1f)
                )
                
                Row(
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Icon(
                        imageVector = Icons.Default.DirectionsTransit,
                        contentDescription = null,
                        modifier = Modifier.size(16.dp),
                        tint = Color.White.copy(alpha = 0.8f)
                    )
                    Spacer(modifier = Modifier.width(8.dp))
                    Icon(
                        imageVector = Icons.AutoMirrored.Filled.KeyboardArrowRight,
                        contentDescription = null,
                        modifier = Modifier.size(16.dp),
                        tint = Color.White.copy(alpha = 0.8f)
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
                        textStyle = LocalTextStyle.current.copy(
                            fontSize = 12.sp,
                            color = Color.White
                        ),
                        colors = OutlinedTextFieldDefaults.colors(
                            focusedTextColor = Color.White,
                            unfocusedTextColor = Color.White,
                            focusedBorderColor = Color.White.copy(alpha = 0.7f),
                            unfocusedBorderColor = Color.White.copy(alpha = 0.5f)
                        )
                    )
                    
                    ExposedDropdownMenu(
                        expanded = expanded,
                        onDismissRequest = { expanded = false }
                    ) {
                        fareTypes.forEachIndexed { typeIndex, fareType ->
                            DropdownMenuItem(
                                text = { Text(fareType, fontSize = 12.sp, color = Color.Black) },
                                onClick = {
                                    selectedFareType = typeIndex
                                    expanded = false
                                    val aggregateType = Routefolder.Aggregate.entries[typeIndex]
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
                        fontSize = 14.sp,
                        color = Color.White
                    )
                    Text(
                        text = "${kmNumStr(fareValue.second)}km",
                        fontSize = 12.sp,
                        color = Color.White.copy(alpha = 0.8f)
                    )
                }
            }
        }
    }
}

@Preview(showBackground = true)
@Composable
fun FolderHeaderPreview() {
    MaterialTheme {
        FolderHeader(
            routefolder = Routefolder().apply {
                // Mock data for preview
            },
            route = null,
            itemCount = 0,
            onAddItem = {},
            onExport = {}
        )
    }
}

@Preview(showBackground = true)
@Composable
fun FolderListItemPreview() {
    MaterialTheme {
        val mockRoute = RouteList().apply {
            // Mock route data for preview
        }
        
        FolderListItem(
            index = 0,
            routeItem = mockRoute,
            routefolder = Routefolder(),
            onItemClick = {},
            onRemove = {},
            onAggregateTypeChange = {},
            onDragStart = {},
            onDragEnd = {},
            onSwap = { _, _ -> }
        )
    }
}

@Preview(showBackground = true)
@Composable
fun FolderViewScreenPreview() {
    MaterialTheme {
        FolderViewScreen(
            routefolder = Routefolder(),
            route = null,
            onItemClick = {},
            onCloseDrawer = {}
        )
    }
}