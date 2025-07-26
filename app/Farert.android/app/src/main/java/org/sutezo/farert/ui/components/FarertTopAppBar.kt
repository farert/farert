package org.sutezo.farert.ui.components

import androidx.compose.foundation.layout.Box
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.ArrowBack
import androidx.compose.material.icons.filled.Menu
import androidx.compose.material.icons.filled.MoreVert
import androidx.compose.material3.*
import androidx.compose.runtime.Composable

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
    showMenuIcon: Boolean = true
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
        },
        colors = farertTopAppBarColors()
    )
}