package org.sutezo.farert

import android.content.Intent
import android.os.Bundle
import androidx.activity.compose.setContent
import androidx.appcompat.app.AppCompatActivity
import org.sutezo.farert.ui.theme.FarertTheme
import org.sutezo.farert.ui.compose.LineListScreen
import org.sutezo.farert.ui.compose.LineListNavigationData


class LineListActivity : AppCompatActivity() {

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
                        val intent = Intent(this, StationListActivity::class.java).apply {
                            putExtra("mode", navigationData.mode)
                            putExtra("line_id", navigationData.lineId)
                            putExtra("line_to_type", navigationData.lineToType)
                            if (navigationData.mode == "route") {
                                putExtra("src_station_id", navigationData.srcStationId)
                                putExtra("start_station_id", navigationData.startStationId)
                            } else {
                                putExtra("line_to_id", navigationData.lineToId)
                            }
                            putExtra("station_mode", navigationData.stationMode)
                        }
                        startActivity(intent)
                    },
                    onNavigateToTerminalSelect = {
                        val intent = Intent(this, TerminalSelectActivity::class.java)
                        intent.putExtra("mode", "autoroute")
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