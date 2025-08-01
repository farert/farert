package org.sutezo.farert.ui.components

import androidx.compose.animation.core.animateFloatAsState
import androidx.compose.animation.core.tween
import androidx.compose.foundation.layout.Box
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.ArrowBack
import androidx.compose.material.icons.filled.Menu
import androidx.compose.material.icons.filled.MoreVert
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.draw.rotate
import androidx.compose.ui.graphics.graphicsLayer

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun farertTopAppBarColors() = TopAppBarDefaults.topAppBarColors(
    containerColor = MaterialTheme.colorScheme.primary,
    titleContentColor = MaterialTheme.colorScheme.onPrimary,
    navigationIconContentColor = MaterialTheme.colorScheme.onPrimary,
    actionIconContentColor = MaterialTheme.colorScheme.onPrimary
)

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun FarertTopAppBar(
    title: String,
    onBackClick: () -> Unit = {},
    showMenu: Boolean = false,
    onMenuClick: () -> Unit = {},
    onDismissMenu: () -> Unit = {},
    dropdownContent: @Composable () -> Unit = {},
    useBackArrow: Boolean = false,
    showMenuIcon: Boolean = true,
    customActionText: String? = null,
    onCustomActionClick: () -> Unit = {},
    enableVerticalRotation: Boolean = false
) {
    TopAppBar(
        title = { Text(title) },
        navigationIcon = {
            IconButton(onClick = onBackClick) {
                if (useBackArrow) {
                    Icon(Icons.AutoMirrored.Filled.ArrowBack, contentDescription = "戻る")
                } else {
                    Icon(Icons.Filled.Menu, contentDescription = "ハンバーガーメニュー")
                }
            }
        },
        actions = {
            customActionText?.let { text ->
                var isClicked by remember { mutableStateOf(false) }
                val rotationValue by animateFloatAsState(
                    targetValue = if (isClicked) 180f else 0f,
                    animationSpec = tween(durationMillis = 600),
                    finishedListener = { 
                        if (isClicked) {
                            onCustomActionClick()
                        }
                        isClicked = false 
                    }
                )
                
                TextButton(
                    onClick = {
                        isClicked = true
                    }
                ) {
                    Text(
                        text = text,
                        color = MaterialTheme.colorScheme.onPrimary
                    )
                }
            } ?: run {
                if (showMenuIcon) {
                    Box {
                        IconButton(onClick = onMenuClick) {
                            Icon(Icons.Filled.MoreVert, contentDescription = "メニュー")
                        }
                        DropdownMenu(
                            expanded = showMenu,
                            onDismissRequest = onDismissMenu
                        ) {
                            dropdownContent()
                        }
                    }
                }
            }
        },
        colors = farertTopAppBarColors()
    )
}