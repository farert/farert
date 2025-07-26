package org.sutezo.farert

import android.content.Intent
import android.os.Bundle
import androidx.activity.compose.setContent
import androidx.appcompat.app.AppCompatActivity
import org.sutezo.farert.ui.theme.FarertTheme
import org.sutezo.farert.ui.compose.StationListScreen

/**
 * An activity representing a single LineListActivity detail screen. This
 * activity is only used on narrow width devices. On tablet-size devices,
 * item details are presented side-by-side with a list of items
 * in a [LineListActivity].
 */
class StationListActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        
        // Extract parameters from intent
        val mode = intent.getStringExtra("mode") ?: ""
        val lineId = intent.getIntExtra("line_id", 0)
        val srcType = intent.getStringExtra("line_to_type") ?: "" // prefect|company
        val srcCompanyOrPrefectId = intent.getIntExtra("line_to_id", 0)
        val stationMode = intent.getStringExtra("station_mode") ?: "" // junction|all
        val srcStationId = intent.getIntExtra("src_station_id", 0)
        val startStationId = intent.getIntExtra("start_station_id", 0)
        
        setContent {
            FarertTheme {
                StationListScreen(
                    mode = mode,
                    lineId = lineId,
                    srcType = srcType,
                    srcCompanyOrPrefectId = srcCompanyOrPrefectId,
                    stationMode = stationMode,
                    srcStationId = srcStationId,
                    startStationId = startStationId,
                    onNavigateToMain = { destStationId ->
                        val intent = Intent(this, MainActivity::class.java).apply {
                            putExtra("dest_station_id", destStationId)
                            putExtra("mode", mode)
                            putExtra("line_id", if (mode == "route") lineId else 0)
                            addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP or Intent.FLAG_ACTIVITY_SINGLE_TOP)
                        }
                        startActivity(intent)
                    },
                    onNavigateUp = { finish() },
                    onSwitchAction = {
                        // Handle switch action by recreating activity with different station_mode
                        val curIntent = this.intent
                        val intent = Intent(this, StationListActivity::class.java).apply {
                            putExtra("mode", curIntent.getStringExtra("mode"))
                            putExtra("line_id", curIntent.getIntExtra("line_id", 0))
                            putExtra("line_to_type", curIntent.getStringExtra("line_to_type"))
                            putExtra("line_to_id", curIntent.getIntExtra("line_to_id", 0))
                            putExtra("src_station_id", curIntent.getIntExtra("src_station_id", 0))
                            putExtra("start_station_id", curIntent.getIntExtra("start_station_id", 0))
                            
                            if (curIntent.getStringExtra("mode") == "route" &&
                                    curIntent.getStringExtra("station_mode") == "junction") {
                                putExtra("station_mode", "all")
                            } else {
                                putExtra("station_mode", "junction")
                            }
                            addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP or Intent.FLAG_ACTIVITY_SINGLE_TOP)
                        }
                        finish()
                        startActivity(intent)
                    }
                )
            }
        }
    }
}
