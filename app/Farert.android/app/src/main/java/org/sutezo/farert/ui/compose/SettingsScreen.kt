package org.sutezo.farert.ui.compose

import android.content.ClipboardManager
import android.content.Context
import android.content.Intent
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.ArrowBack
import androidx.compose.material.icons.filled.Share
import androidx.compose.material.icons.filled.Restore
import androidx.compose.material3.*
import androidx.compose.material3.MenuAnchorType
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.lifecycle.viewmodel.compose.viewModel
import org.sutezo.farert.R
import org.sutezo.farert.ui.state.SettingsUiState
import org.sutezo.farert.ui.state.SettingsUiEvent
import org.sutezo.farert.ui.state.SettingsStateHolder
import org.sutezo.farert.ui.components.farertTopAppBarColors

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun SettingsScreen(
    stateHolder: SettingsStateHolder = viewModel(),
    onNavigateUp: (Boolean) -> Unit = {}
) {
    val context = LocalContext.current
    val uiState = stateHolder.uiState
    val snackbarHostState = remember { SnackbarHostState() }
    var showRestoreDialog by remember { mutableStateOf(false) }
    
    // Initialize state
    LaunchedEffect(Unit) {
        stateHolder.initialize(context)
    }
    
    uiState.error?.let { error ->
        LaunchedEffect(error) {
            snackbarHostState.showSnackbar(error)
            stateHolder.handleEvent(SettingsUiEvent.ClearError)
        }
    }

    uiState.message?.let { message ->
        LaunchedEffect(message) {
            snackbarHostState.showSnackbar(message)
            stateHolder.handleEvent(SettingsUiEvent.ClearMessage)
        }
    }

    uiState.shareText?.let { text ->
        LaunchedEffect(text) {
            val sendIntent = Intent(Intent.ACTION_SEND).apply {
                type = "application/json"
                putExtra(Intent.EXTRA_SUBJECT, "Farert backup")
                putExtra(Intent.EXTRA_TEXT, text)
            }
            context.startActivity(Intent.createChooser(sendIntent, "バックアップを共有"))
            stateHolder.handleEvent(SettingsUiEvent.ClearShareText)
        }
    }
    
    Scaffold(
        topBar = {
            TopAppBar(
                title = { Text("設定") },
                navigationIcon = {
                    IconButton(onClick = { 
                        val hasChanges = stateHolder.saveSettings()
                        onNavigateUp(hasChanges)
                    }) {
                        Icon(Icons.AutoMirrored.Filled.ArrowBack, contentDescription = "戻る")
                    }
                },
                colors = farertTopAppBarColors()
            )
        },
        snackbarHost = { SnackbarHost(snackbarHostState) }
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
            SettingsContent(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(paddingValues),
                uiState = uiState,
                stateHolder = stateHolder,
                onShowRestoreDialog = { showRestoreDialog = true }
            )
        }
    }

    if (showRestoreDialog) {
        RestoreConfirmDialog(
            onDismiss = { showRestoreDialog = false },
            onRestore = {
                val clipboard = context.getSystemService(Context.CLIPBOARD_SERVICE) as ClipboardManager
                val text = clipboard.primaryClip?.getItemAt(0)?.coerceToText(context)?.toString().orEmpty()
                stateHolder.handleEvent(SettingsUiEvent.RestoreFromClipboard(text))
                showRestoreDialog = false
            }
        )
    }
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
private fun SettingsContent(
    modifier: Modifier = Modifier,
    uiState: SettingsUiState,
    stateHolder: SettingsStateHolder,
    onShowRestoreDialog: () -> Unit
) {
    Column(
        modifier = modifier
            .verticalScroll(rememberScrollState())
            .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(24.dp)
    ) {
        /*
        // Database Selection
        Card(
            modifier = Modifier.fillMaxWidth()
        ) {
            Column(
                modifier = Modifier.padding(16.dp)
            ) {
                Text(
                    text = "データベース選択",
                    style = MaterialTheme.typography.titleMedium,
                    modifier = Modifier.padding(bottom = 8.dp)
                )
                
                var expanded by remember { mutableStateOf(false) }
                
                ExposedDropdownMenuBox(
                    expanded = expanded,
                    onExpandedChange = { expanded = !expanded }
                ) {
                    TextField(
                        value = if (uiState.selectedDatabaseIndex >= 0 && uiState.selectedDatabaseIndex < uiState.databaseOptions.size) {
                            uiState.databaseOptions[uiState.selectedDatabaseIndex]
                        } else {
                            ""
                        },
                        onValueChange = {},
                        readOnly = true,
                        trailingIcon = { ExposedDropdownMenuDefaults.TrailingIcon(expanded = expanded) },
                        colors = ExposedDropdownMenuDefaults.textFieldColors(),
                        modifier = Modifier
                            .menuAnchor(MenuAnchorType.PrimaryNotEditable, true)
                            .fillMaxWidth()
                            .padding(end = 8.dp),
                        shape = RoundedCornerShape(12.dp)
                    )
                    
                    ExposedDropdownMenu(
                        expanded = expanded,
                        onDismissRequest = { expanded = false },
                        modifier = Modifier.background(
                            brush = Brush.verticalGradient(
                                colors = listOf(
                                    MaterialTheme.colorScheme.surface,
                                    MaterialTheme.colorScheme.surfaceVariant
                                )
                            )
                        )
                    ) {
                        uiState.databaseOptions.forEachIndexed { index, option ->
                            DropdownMenuItem(
                                text = { 
                                    Text(
                                        text = option,
                                        style = MaterialTheme.typography.bodyMedium.copy(
                                            lineHeight = 16.sp
                                        )
                                    ) 
                                },
                                onClick = {
                                    stateHolder.handleEvent(SettingsUiEvent.DatabaseSelectionChanged(index))
                                    expanded = false
                                },
                                modifier = Modifier.height(40.dp),
                                colors = MenuDefaults.itemColors(
                                    textColor = MaterialTheme.colorScheme.onSurface
                                )
                            )
                        }
                    }
                }
            }
        }
        */
        // Kokura-Shinzai Setting
        /*
        Card(
            modifier = Modifier.fillMaxWidth()
        ) {
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(16.dp),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text(
                    text = "小倉-博多間新幹線在来線同一視",
                    style = MaterialTheme.typography.bodyLarge,
                    modifier = Modifier.weight(1f)
                )
                
                Switch(
                    checked = uiState.kokuraShinzaiEnabled,
                    onCheckedChange = { enabled ->
                        stateHolder.handleEvent(SettingsUiEvent.KokuraShinzaiToggled(enabled))
                    }
                )
            }
        }*/
        
        // Info Messages Reset
        Card(
            modifier = Modifier.fillMaxWidth()
        ) {
            Column(
                modifier = Modifier.padding(16.dp)
            ) {
                Text(
                    text = "情報メッセージ抑制リセット",
                    style = MaterialTheme.typography.titleMedium,
                    modifier = Modifier.padding(bottom = 8.dp)
                )
                
                Text(
                    text = "非表示に設定した情報メッセージを再び表示するようにリセットします",
                    style = MaterialTheme.typography.bodyMedium,
                    color = MaterialTheme.colorScheme.onSurfaceVariant,
                    modifier = Modifier.padding(bottom = 12.dp)
                )
                
                Button(
                    onClick = {
                        stateHolder.handleEvent(SettingsUiEvent.ResetInfoMessages)
                    },
                    enabled = uiState.canResetInfoMessages,
                    modifier = Modifier.fillMaxWidth()
                ) {
                    Text("リセット")
                }
            }
        }

        Card(
            modifier = Modifier.fillMaxWidth()
        ) {
            Column(
                modifier = Modifier.padding(16.dp)
            ) {
                Text(
                    text = "バックアップとリストア",
                    style = MaterialTheme.typography.titleMedium,
                    modifier = Modifier.padding(bottom = 8.dp)
                )

                Text(
                    text = "発着駅履歴、保存経路、きっぷホルダを共通 JSON として共有・復元します",
                    style = MaterialTheme.typography.bodyMedium,
                    color = MaterialTheme.colorScheme.onSurfaceVariant,
                    modifier = Modifier.padding(bottom = 12.dp)
                )

                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.spacedBy(12.dp)
                ) {
                    Button(
                        onClick = {
                            stateHolder.handleEvent(SettingsUiEvent.BackupToShare)
                        },
                        modifier = Modifier.weight(1f)
                    ) {
                        Icon(Icons.Default.Share, contentDescription = null)
                        Spacer(modifier = Modifier.width(8.dp))
                        Text("バックアップ")
                    }

                    OutlinedButton(
                        onClick = {
                            onShowRestoreDialog()
                        },
                        modifier = Modifier.weight(1f)
                    ) {
                        Icon(Icons.Default.Restore, contentDescription = null)
                        Spacer(modifier = Modifier.width(8.dp))
                        Text("リストア")
                    }
                }
            }
        }
    }
}

@Composable
private fun RestoreConfirmDialog(
    onDismiss: () -> Unit,
    onRestore: () -> Unit
) {
    AlertDialog(
        onDismissRequest = onDismiss,
        title = { Text("リストア") },
        text = { Text("クリップボードのバックアップ JSON で現在の発着駅履歴、保存経路、きっぷホルダを置き換えます。") },
        confirmButton = {
            TextButton(onClick = onRestore) {
                Text("リストア")
            }
        },
        dismissButton = {
            TextButton(onClick = onDismiss) {
                Text("キャンセル")
            }
        }
    )
}
