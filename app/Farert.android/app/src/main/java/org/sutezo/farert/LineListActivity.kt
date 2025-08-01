package org.sutezo.farert

import android.content.Intent
import android.os.Bundle
import androidx.activity.compose.setContent
import androidx.activity.ComponentActivity
import androidx.core.os.bundleOf
import org.sutezo.farert.ui.theme.FarertTheme
import org.sutezo.farert.ui.compose.LineListScreen
import org.sutezo.farert.ui.compose.LineListNavigationData


class LineListActivity : ComponentActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        
        // Extract parameters from intent
        val mode = intent.getStringExtra("mode") ?: ""
        val (srcStationId, startStationId, srcLineId, ident, idType) = if (mode == "route") {
            Triple(
                intent.getIntExtra("src_station_id", 0),
                intent.getIntExtra("start_station_id", 0),
                intent.getIntExtra("src_line_id", 0)
            ).let { (src, start, srcLine) ->
                arrayOf(src, start, srcLine, 0, 0)
            }
        } else {
            // mode = autoroute|terminal
            val identValue = intent.getIntExtra("line_to_id", 0)
            val idTypeValue = if (intent.getStringExtra("line_to_type") == "prefect") 2 else 1
            arrayOf(0, 0, 0, identValue, idTypeValue)
        }
        
        // Check for two-pane mode (simplified - just use false for now)
        val isTwoPane = false // TODO: Check for w900dp layout
        
        setContent {
            FarertTheme {
                LineListScreen(
                    mode = mode,
                    ident = ident,
                    idType = idType,
                    srcStationId = srcStationId,
                    startStationId = startStationId,
                    srcLineId = srcLineId,
                    isTwoPane = isTwoPane,
                    onNavigateToStationList = { navigationData ->
                        val intent = Intent(this@LineListActivity, StationListActivity::class.java).apply {
                            val extras = buildMap {
                                put("mode", navigationData.mode)
                                put("line_id", navigationData.lineId)
                                put("line_to_type", navigationData.lineToType)
                                put("station_mode", navigationData.stationMode)
                                
                                if (navigationData.mode == "route") {
                                    put("src_station_id", navigationData.srcStationId)
                                    put("start_station_id", navigationData.startStationId)
                                } else {
                                    put("line_to_id", navigationData.lineToId)
                                }
                            }
                            
                            extras.forEach { (key, value) ->
                                when (value) {
                                    is String -> putExtra(key, value)
                                    is Int -> putExtra(key, value)
                                }
                            }
                        }
                        startActivity(intent)
                    },
                    onNavigateToTerminalSelect = {
                        val intent = Intent(this@LineListActivity, TerminalSelectActivity::class.java).apply {
                            putExtra("mode", "autoroute")
                        }
                        startActivity(intent)
                    },
                    onNavigateUp = { finish() },
                    onStationFragment = { navigationData ->
                        // Handle fragment replacement for two-pane mode
                        // This would be implemented when two-pane support is added
                    }
                )
            }
        }
    }

}