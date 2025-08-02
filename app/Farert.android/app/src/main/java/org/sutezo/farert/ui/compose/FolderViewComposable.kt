package org.sutezo.farert.ui.compose

import android.content.Context
import android.content.Intent
import androidx.compose.foundation.background
import androidx.compose.ui.graphics.Brush
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.itemsIndexed
import androidx.compose.foundation.lazy.rememberLazyListState
import androidx.compose.foundation.lazy.LazyListState
import kotlinx.coroutines.CoroutineScope
import androidx.compose.foundation.shape.RoundedCornerShape
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
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.foundation.gestures.detectDragGesturesAfterLongPress
import androidx.compose.foundation.layout.offset
import androidx.compose.ui.hapticfeedback.HapticFeedbackType
import androidx.compose.ui.platform.LocalHapticFeedback
import androidx.compose.ui.zIndex
import kotlinx.coroutines.launch
import androidx.compose.runtime.rememberCoroutineScope
import org.sutezo.alps.Route
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
    route: Route?,
    onItemClick: (RouteList) -> Unit,
    onCloseDrawer: () -> Unit
) {
    // Debug: Add a log to see if this is being called
    println("DEBUG: FolderViewScreen called with route: ${route?.count ?: "null"}")
    
    val context = LocalContext.current
    var refreshTrigger by remember { mutableIntStateOf(0) }
    var dragOffset by remember { mutableFloatStateOf(0f) }
    var originalDragIndex by remember { mutableStateOf<Int?>(null) }
    val lazyListState = rememberLazyListState()
    val scope = rememberCoroutineScope()
    
    // Create observable item list that forces recomposition
    // Force recomposition by recreating the items list every time refreshTrigger changes
    val items = remember(refreshTrigger) {
        val count = routefolder.count()
        (0 until count).map { index ->
            routefolder.routeItem(index)
        }
    }
    
    val itemCount = items.size
    
    Column(
        modifier = Modifier
            .fillMaxSize()
            .background(colorResource(R.color.colorTicketTypeDropDownBackground))
    ) {
        // Header Section
        FolderHeader(
            routefolder = routefolder,
            route = route,
            itemCount = itemCount,
            refreshTrigger = refreshTrigger,
            onAddItem = {
                route?.let { routeObj ->
                    val rl = RouteList()
                    rl.assign(routeObj, -1)  // Copy all route items
                    routefolder.add(context, rl)
                    refreshTrigger = (refreshTrigger + 1) % 100000
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
            state = lazyListState,
            modifier = Modifier.fillMaxSize()
        ) {
            itemsIndexed(
                items = items,
                key = { index, routeItem -> "route_${index}_${routeItem.departureStationId()}_${routeItem.arriveStationId()}_$refreshTrigger" }
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
                        refreshTrigger = (refreshTrigger + 1) % 100000
                    },
                    onAggregateTypeChange = { newType ->
                        routefolder.setAggregateType(context, index, newType)
                        refreshTrigger = (refreshTrigger + 1) % 100000
                    },
                    onDragStart = { 
                        originalDragIndex = index
                        dragOffset = 0f
                    },
                    onDragEnd = { targetIndex ->
                        // Perform final drop operation
                        originalDragIndex?.let { fromIndex ->
                            if (fromIndex != targetIndex && targetIndex in 0 until itemCount) {
                                // Move item by performing multiple swaps
                                var currentIndex = fromIndex
                                val direction = if (targetIndex > fromIndex) 1 else -1
                                
                                while (currentIndex != targetIndex) {
                                    val nextIndex = currentIndex + direction
                                    if (nextIndex in 0 until itemCount && 
                                        routefolder.swap(context, currentIndex, nextIndex)) {
                                        currentIndex = nextIndex
                                    } else {
                                        break
                                    }
                                }
                                
                                // Force immediate UI refresh with unique value
                                refreshTrigger = (refreshTrigger + 1) % 100000
                            }
                        }
                        // Clear drag state
                        originalDragIndex = null
                        dragOffset = 0f
                    },
                    onCalculateTargetIndex = { dragY ->
                        // Calculate target index based on drag position
                        val itemHeight = 80f // Approximate item height in dp  
                        val positionsToMove = (dragY / itemHeight).toInt()
                        val targetIndex = (originalDragIndex ?: index) + positionsToMove
                        targetIndex.coerceIn(0, itemCount - 1)
                    },
                    isDraggedItem = originalDragIndex == index,
                    dragOffset = if (originalDragIndex == index) dragOffset else 0f,
                    onUpdateDragOffset = { offset -> 
                        if (originalDragIndex == index) {
                            dragOffset = offset
                        }
                    },
                    lazyListState = lazyListState,
                    scope = scope
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
    refreshTrigger: Int,
    onAddItem: () -> Unit,
    onExport: () -> Unit
) {
    // Cache total fare and km values that update when refreshTrigger changes
    val totalFareText = remember(refreshTrigger) { routefolder.totalFare() }
    val totalKmText = remember(refreshTrigger) { routefolder.totalSalesKm() }
    
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
                text = totalFareText,
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
                text = totalKmText,
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
            // Left side - Export button (show only when items exist)
            if (itemCount > 0) {
                IconButton(
                    onClick = onExport
                ) {
                    Icon(
                        imageVector = Icons.Default.Share,
                        contentDescription = "Export",
                        tint = colorResource(R.color.colorTicketTypeText)
                    )
                }
            } else {
                // Spacer to maintain layout when Share button is hidden
                Spacer(modifier = Modifier.size(48.dp))
            }

            // Right side - Add button (show only when route is available)
            if (route != null && route.count > 0) {
                TextButton(
                    onClick = onAddItem
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
    onItemClick: () -> Unit,
    onRemove: () -> Unit,
    onAggregateTypeChange: (Routefolder.Aggregate) -> Unit,
    onDragStart: () -> Unit = {},
    onDragEnd: (Int) -> Unit = {},
    onCalculateTargetIndex: (Float) -> Int = { 0 },
    isDraggedItem: Boolean = false,
    dragOffset: Float = 0f,
    onUpdateDragOffset: (Float) -> Unit = {},
    lazyListState: LazyListState,
    scope: CoroutineScope
) {
    val fareTypes = stringArrayResource(R.array.list_ticket_type)
    var expanded by remember { mutableStateOf(false) }
    var selectedFareType by remember { 
        mutableIntStateOf(routefolder.aggregateType(index).ordinal)
    }
    val haptic = LocalHapticFeedback.current
    var cumulativeDragY by remember { mutableFloatStateOf(0f) }
    
    Card(
        onClick = if (isDraggedItem) { {} } else onItemClick,
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp, vertical = 0.dp)
            .zIndex(if (isDraggedItem) 1f else 0f)
            .offset(y = if (isDraggedItem) dragOffset.dp else 0.dp)
            .scale(if (isDraggedItem) 1.05f else 1f)
            .shadow(
                elevation = if (isDraggedItem) 8.dp else 4.dp,
                shape = RoundedCornerShape(12.dp)
            )
            .background(
                Brush.verticalGradient(
                    colors = if (isDraggedItem) listOf(
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
                    tint = if (isDraggedItem) Color.Yellow else Color.White.copy(alpha = 0.7f),
                    modifier = Modifier
                        .size(24.dp)
                        .padding(end = 8.dp)
                        .pointerInput(index) {
                            detectDragGesturesAfterLongPress(
                                onDragStart = { offset ->
                                    haptic.performHapticFeedback(HapticFeedbackType.LongPress)
                                    onDragStart()
                                    cumulativeDragY = 0f
                                },
                                onDragEnd = {
                                    // Calculate final target index and call onDragEnd
                                    val targetIndex = onCalculateTargetIndex(cumulativeDragY)
                                    onDragEnd(targetIndex)
                                    cumulativeDragY = 0f
                                }
                            ) { change, dragAmount ->
                                // Simple cumulative drag tracking
                                cumulativeDragY += dragAmount.y
                                onUpdateDragOffset(cumulativeDragY)
                                
                                // Auto-scroll logic
                                val layoutInfo = lazyListState.layoutInfo
                                
                                // Check if we need to scroll
                                val currentItemTop = layoutInfo.visibleItemsInfo.find { it.index == index }?.offset ?: 0
                                val draggedY = currentItemTop + cumulativeDragY
                                
                                // Auto-scroll when near edges
                                when {
                                    draggedY < layoutInfo.viewportStartOffset + 100 -> {
                                        // Scroll up
                                        scope.launch {
                                            if (index > 0) {
                                                lazyListState.animateScrollToItem(maxOf(0, index - 1))
                                            }
                                        }
                                    }
                                    draggedY > layoutInfo.viewportEndOffset - 100 -> {
                                        // Scroll down
                                        scope.launch {
                                            val totalItems = lazyListState.layoutInfo.totalItemsCount
                                            if (index < totalItems - 1) {
                                                lazyListState.animateScrollToItem(minOf(totalItems - 1, index + 1))
                                            }
                                        }
                                    }
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
                    modifier = Modifier.width(140.dp)
                ) {
                    OutlinedTextField(
                        value = fareTypes.getOrNull(selectedFareType) ?: "",
                        onValueChange = {},
                        readOnly = true,
                        trailingIcon = {
                            ExposedDropdownMenuDefaults.TrailingIcon(expanded = expanded)
                        },
                        modifier = Modifier
                            .menuAnchor(MenuAnchorType.PrimaryNotEditable, true)
                            .clip(RoundedCornerShape(12.dp)),
                        textStyle = LocalTextStyle.current.copy(
                            fontSize = 12.sp,
                            color = Color.White
                        ),
                        colors = OutlinedTextFieldDefaults.colors(
                            focusedTextColor = Color.White,
                            unfocusedTextColor = Color.White,
                            focusedBorderColor = Color.White.copy(alpha = 0.7f),
                            unfocusedBorderColor = Color.White.copy(alpha = 0.5f)
                        ),
                        shape = RoundedCornerShape(12.dp)
                    )
                    
                    ExposedDropdownMenu(
                        expanded = expanded,
                        onDismissRequest = { expanded = false },
                        modifier = Modifier.background(
                            brush = Brush.verticalGradient(
                                colors = listOf(
                                    Color(0xFF2D1B69).copy(alpha = 0.95f), // Rich purple
                                    Color(0xFF11998e).copy(alpha = 0.95f), // Teal
                                    Color(0xFF38ef7d).copy(alpha = 0.95f)  // Light green
                                )
                            )
                        )
                    ) {
                        fareTypes.forEachIndexed { typeIndex, fareType ->
                            DropdownMenuItem(
                                text = { 
                                    Text(
                                        text = fareType, 
                                        fontSize = 12.sp, 
                                        color = Color.White,
                                        textAlign = TextAlign.Center,
                                        lineHeight = 12.sp
                                    ) 
                                },
                                onClick = {
                                    selectedFareType = typeIndex
                                    expanded = false
                                    val aggregateType = Routefolder.Aggregate.entries[typeIndex]
                                    onAggregateTypeChange(aggregateType)
                                },
                                modifier = Modifier.height(36.dp),
                                colors = MenuDefaults.itemColors(
                                    textColor = Color.White
                                )
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
            refreshTrigger = 0,
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
            onCalculateTargetIndex = { 0 },
            isDraggedItem = false,
            dragOffset = 0f,
            onUpdateDragOffset = {},
            lazyListState = rememberLazyListState(),
            scope = rememberCoroutineScope()
        )
    }
}

@Preview(showBackground = true)
@Composable
fun FolderViewScreenPreview() {
    MaterialTheme {
        FolderViewScreen(
            routefolder = Routefolder(),
            route = Route(),
            onItemClick = {},
            onCloseDrawer = {}
        )
    }
}