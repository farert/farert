package org.sutezo.farert

import android.content.Intent
import android.os.Build
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.view.Menu
import android.view.MenuItem
import kotlinx.android.synthetic.main.activity_station_list.*
import org.sutezo.alps.RouteUtil

/**
 * An activity representing a single LineListActivity detail screen. This
 * activity is only used on narrow width devices. On tablet-size devices,
 * item details are presented side-by-side with a list of items
 * in a [LineListActivity].
 */
class StationListActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_station_list)
        setSupportActionBar(station_toolbar)

        // Show the Up button in the action bar.
        supportActionBar?.setDisplayHomeAsUpEnabled(true)

        // savedInstanceState is non-null when there is fragment state
        // saved from previous configurations of this activity
        // (e.g. when rotating the screen from portrait to landscape).
        // In this case, the fragment will automatically be re-added
        // to its container so we don't need to manually add it.
        // For more information, see the Fragments API guide at:
        //
        // http://developer.android.com/guide/components/fragments.html
        //
        //if (savedInstanceState == null) {
            // 分岐駅、着駅切り替えで自身をstartActivity()したときに表示がされないため
            // savedInstanceStateはみないようにした
            // Create the detail fragment and add it to the activity
            // using a fragment transaction.
            val mode = intent.getStringExtra("mode")
            val lineId = intent.getIntExtra("line_id", 0)
            val srcType = intent.getStringExtra("line_to_type") ?: "" // prefect|company
            val srcCompanyOrPrefectId = intent.getIntExtra("line_to_id", 0)
            val stationMode = intent.getStringExtra("station_mode") ?:""// junction | all
            val srcStationId = intent.getIntExtra("src_station_id", 0)
            val startStationId = intent.getIntExtra("start_station_id", 0)

            val title = {
                if (mode == "route") {
                    if (stationMode == "junction") {
                        R.string.title_station_select_junction // "分岐駅指定"
                    } else {
                        R.string.title_station_select_arrive // "着駅指定"
                    }
                } else if (mode == "autoroute") {
                    R.string.title_terminal_select_arrive // "着駅指定（最短経路)"
                } else {
                    R.string.title_terminal_select_depart //"発駅指定"
                }
            }
            setTitle(title())

            val fragment = StationListFragment().apply {
                arguments = Bundle().apply {
                    putString("mode", mode)
                    putInt("line_id", lineId)
                    putString("line_to_type", srcType)
                    putInt("line_to_id", srcCompanyOrPrefectId)
                    putString("station_mode", stationMode)
                    putInt("src_station_id", srcStationId)
                    putInt("start_station_id", startStationId)
                }
            }
            supportFragmentManager.beginTransaction()
                    .add(R.id.frameLayout, fragment)
                    .commit()

            supportActionBar?.subtitle = when (srcType) {
                "prefect" -> {
                    "${RouteUtil.PrefectName(srcCompanyOrPrefectId)} - ${RouteUtil.LineName(lineId)}"
                }
                "company" -> {
                    "${RouteUtil.CompanyName(srcCompanyOrPrefectId)} - ${RouteUtil.LineName(lineId)}"
                }
                else -> {
                    RouteUtil.LineName(lineId)
                }
            }
        //}
    }

    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        // Inflate the menu; this adds items to the action bar if it is present.
        menuInflater.inflate(R.menu.menu_station_select, menu)

        return true
    }

    override fun onPrepareOptionsMenu(menu: Menu?): Boolean {

        if (title == resources.getString(R.string.title_station_select_junction)) {
            menu?.findItem(R.id.action_switch)?.title = resources.getString(R.string.title_station_select_arrive)
        } else if (title == resources.getString(R.string.title_station_select_arrive)) {
            menu?.findItem(R.id.action_switch)?.title = resources.getString(R.string.title_station_select_junction)
        } else {
            invalidateOptionsMenu();
            menu?.findItem(R.id.action_switch)?.setVisible(false)
        }
        return super.onPrepareOptionsMenu(menu)
    }


    override fun onOptionsItemSelected(item: MenuItem) =
            when (item.itemId) {
                android.R.id.home -> {
                    // This ID represents the Home or Up button. In the case of this
                    // activity, the Up button is shown. For
                    // more details, see the Navigation pattern on Android Design:
                    //
                    // http://developer.android.com/design/patterns/navigation.html#up-vs-back
                    navigateUpTo(Intent(this, LineListActivity::class.java))
                    //finish()//!!!!!
                    super.onOptionsItemSelected(item)
                }
                R.id.action_switch -> {
                    val curIntent = this.intent
                    val intent = Intent(this, StationListActivity::class.java).apply {
                        putExtra("mode", curIntent.getStringExtra("mode"))
                        putExtra("line_id", curIntent.getIntExtra("line_id", 0))
                        putExtra("src_station_id", curIntent.getIntExtra("src_station_id", 0))
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
                    true   //super.onOptionsItemSelected(item)
                }
                else -> {super.onOptionsItemSelected(item)}
            }
}
