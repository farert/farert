package org.sutezo.farert

import android.annotation.SuppressLint
import android.app.AlertDialog
import android.content.Context
import android.content.Intent
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import androidx.activity.compose.setContent
import androidx.compose.runtime.LaunchedEffect
import android.util.Log
import android.view.*
import android.widget.TextView
import org.sutezo.farert.ui.theme.FarertTheme
import org.sutezo.farert.ui.compose.TerminalSelectScreen
import org.sutezo.alps.*


class TerminalSelectActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        
        val mode = intent.getStringExtra("mode") ?: "terminal"
        
        setContent {
            FarertTheme {
                TerminalSelectScreen(
                    mode = mode,
                    onNavigateToLineList = { itemId, type, currentMode ->
                        val intent = Intent(this, LineListActivity::class.java).apply {
                            putExtra("mode", currentMode)
                            putExtra("line_to_id", itemId)
                            putExtra("line_to_type", type)
                        }
                        startActivity(intent)
                    },
                    onNavigateToMain = { stationId, currentMode ->
                        val intent = Intent(this, MainActivity::class.java).apply {
                            putExtra("dest_station_id", stationId)
                            putExtra("mode", currentMode)
                            addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP or Intent.FLAG_ACTIVITY_CLEAR_TOP)
                        }
                        startActivity(intent)
                    },
                    onNavigateUp = { finish() }
                )
            }
        }
    }

}
