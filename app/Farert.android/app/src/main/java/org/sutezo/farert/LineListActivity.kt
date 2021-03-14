package org.sutezo.farert

import android.content.Intent
import android.graphics.Color
import android.graphics.Typeface
import android.os.Build
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.support.v7.widget.RecyclerView
import android.support.design.widget.Snackbar
import android.view.*
import android.widget.TextView

import kotlinx.android.synthetic.main.activity_line_list.*
import kotlinx.android.synthetic.main.row_line_list.view.*
import kotlinx.android.synthetic.main.line_list.*
import org.sutezo.alps.RouteUtil
import org.sutezo.alps.RouteUtil.*
import org.sutezo.alps.lineIdsFromStation
import org.sutezo.alps.linesFromCompanyOrPrefect


/**
 * An activity representing a list of Pings. This activity
 * has different presentations for handset and tablet-size devices. On
 * handsets, the activity presents a list of items, which when touched,
 * lead to a [StationListActivity] representing
 * item details. On tablets, the activity presents the list of items and
 * item details side-by-side using two vertical panes.
 */
class LineListActivity : AppCompatActivity() , ViewHolder.LineClickListener {

    /**
     * Whether or not the activity is in two-pane mode, i.e. running on a tablet
     * device.
     */
    private var twoPane: Boolean = false
    private var mMode: String = ""
    private var mIdent : Int = 0    // company_id or prefect_id if mode==route notused
    private var mLineId : Int = 0    // select line_id at this view
    private var mIdType : Int = 0    // "prefect=2"|"company=1"
    private var mSrcStationId : Int = 0
    private var mStartStationId : Int = 0
    private var mSrcLineId: Int = 0
    private var mStationSelMode : Int = 0 // twoPane and mode="route"

    private var menu : Menu? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        intent.apply {
            mMode = getStringExtra("mode") ?: ""
            if (mMode == "route") {
                mSrcStationId = getIntExtra("src_station_id", 0)
                mStartStationId = getIntExtra("start_station_id", 0)
                mSrcLineId = getIntExtra("src_line_id", 0)
            } else {
                // mode = autoroute|terminal
                mIdent = getIntExtra("line_to_id", 0)
                mIdType = if (getStringExtra("line_to_type") == "prefect") 2 else 1
            }
        }

        setContentView(R.layout.activity_line_list)

        toolbar.title = title
        setSupportActionBar(toolbar)

        // back arrow button
//?       supportActionBar!!.setDisplayHomeAsUpEnabled(true)
//?        supportActionBar!!.setHomeButtonEnabled(true)
        // Show the Up button in the action bar.
        supportActionBar?.setDisplayHomeAsUpEnabled(true)

        if (line_list_station_list_container != null) {
            // The detail container view will be present only in the
            // large-screen layouts (res/values-w900dp).
            // If this view is present, then the
            // activity should be in two-pane mode.
            twoPane = true
        }

        // View subject

        val listItems =
                when (mIdType) {
                    1-> {
                        // 会社名(JRグループ名)
                        supportActionBar?.subtitle =  RouteUtil.CompanyName(mIdent)
                        linesFromCompanyOrPrefect(mIdent)
                    }
                    2 -> {   //prefect 都道府県
                        supportActionBar?.subtitle =  RouteUtil.PrefectName(mIdent)
                        linesFromCompanyOrPrefect(mIdent)
                    }
                    0 -> {  // 起点駅
                        supportActionBar?.subtitle = RouteUtil.StationName(mSrcStationId)
                        lineIdsFromStation(mSrcStationId)
                    }
                    else -> {
                        arrayListOf<Int>()
                    }
                }

        linelistactivity_list.adapter = LineListRecyclerViewAdapter(listItems, mSrcLineId, this)
    }

    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        return when (mMode) {
            "route", "terminal" -> {
                // Inflate the menu; this adds items to the action bar if it is present.
                menuInflater.inflate(R.menu.menu_station_select, menu)
                true
            }
            else -> {
                super.onCreateOptionsMenu(menu)
            }
        }
    }

    override fun onPrepareOptionsMenu(menu: Menu?): Boolean {
        this.menu = menu
        menuTitleChangeAtTwoPain(menu)
        return super.onPrepareOptionsMenu(menu)
    }

    private fun menuTitleChangeAtTwoPain(menu: Menu?) {
        when (mStationSelMode) {
            1 -> {
                // junction
                menu?.findItem(R.id.action_switch)?.title = resources.getString(R.string.title_station_select_arrive)
            }
            2 -> {
                // terminal
                menu?.findItem(R.id.action_switch)?.title = resources.getString(R.string.title_station_select_junction)
            }
            else -> {
                if ((mMode == "autoroute") || (mMode == "terminal")) {
                    invalidateOptionsMenu()
                    menu?.findItem(R.id.action_switch)?.setVisible(false)
                }
            }
        }
    }
    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        val id = item.itemId

        when (id) {
            R.id.action_settings -> {
                return true
            }
            android.R.id.home -> { // back arrow button
                finish()
            }
            R.id.action_switch -> {
                if (0 == mStationSelMode) {
                    val intent = Intent(this, TerminalSelectActivity::class.java)
                    intent.putExtra("mode", "autoroute")
                    startActivity(intent)

                } else {
                    val fragment = StationListFragment().apply {
                        arguments = Bundle().apply {
                            putString("mode", mMode)
                            putInt("line_id", mLineId)
                            putInt("src_station_id", mSrcStationId)
                            putInt("start_station_id", mStartStationId)
                            if (mStationSelMode == 1) {
                                putString("station_mode", "all")
                                mStationSelMode = 2
                            } else {
                                putString("station_mode", "junction")
                                mStationSelMode = 1
                            }
                        }
                    }
                    menuTitleChangeAtTwoPain(this.menu)
                    toolbar.title = "${title} - ${resources.getString(
                            if (mStationSelMode == 2) R.string.title_station_select_arrive
                                                 else R.string.title_station_select_junction)}"
                    supportFragmentManager
                            .beginTransaction()
                            .replace(R.id.line_list_station_list_container, fragment)
                            .commit()
                }
            }
        }
        return super.onOptionsItemSelected(item)
    }



    override fun onLineClick(view: View, lineId: Int) {
        mLineId = lineId
        val type = when(mIdType) {
            1 -> { "company" }
            2 -> { "prefect" }
            else -> { "" }
        }
        val stationList = when (mMode) {
            "route" -> "junction"
            else -> ""
        }
        mStationSelMode = 0
        if (twoPane) {
            val fragment = StationListFragment().apply {
                arguments = Bundle().apply {
                    putString("mode", mMode)
                    putInt("line_id", lineId)
                    putString("line_to_type", type)
                    if (mMode == "route") {
                        putInt("src_station_id", mSrcStationId)
                        putInt("start_station_id", mStartStationId)
                    } else {
                        //
                        putInt("line_to_id",  mIdent)
                    }
                    putString("station_mode", stationList)
                }
            }
            if (mMode == "route") {
                mStationSelMode = 1 // junction
                menuTitleChangeAtTwoPain(this.menu)
            }
            supportFragmentManager
                    .beginTransaction()
                    .replace(R.id.line_list_station_list_container, fragment)
                    .commit()
        } else {
            val intent = Intent(this, StationListActivity::class.java).apply {
                putExtra("mode", mMode)
                putExtra("line_id", lineId)
                putExtra("line_to_type", type)
                if (mMode == "route") {
                    putExtra("src_station_id", mSrcStationId)
                    putExtra("start_station_id", mStartStationId)
                } else {
                    putExtra("line_to_id", mIdent)
                }
                putExtra("station_mode", stationList)
                //addFlags(Intent.FLAG_ACTIVITY_FORWARD_RESULT)
            }
            startActivity(intent)
        }
    }
}

private class LineListRecyclerViewAdapter(private val values: List<Int>,
                                          private val srcLineId : Int,
                                          private val listener : ViewHolder.LineClickListener) :
        RecyclerView.Adapter<ViewHolder>() {

    private val onClickListener: View.OnClickListener

    init {
        onClickListener = View.OnClickListener { v ->
            val item = v.tag as Int
            listener.onLineClick(v, item)
        }
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
        val view = LayoutInflater.from(parent.context)
                .inflate(R.layout.row_line_list, parent, false)
        if (viewType == 1) {
            with(view.id_line) {
                //setBackgroundColor(Color.parseColor("gray"))
                setBackgroundResource(R.color.colorBackOriginTerm)
                typeface = Typeface.DEFAULT_BOLD
            }
        }
        return ViewHolder(view)
    }

    override fun onBindViewHolder(holder: ViewHolder, position: Int) {
        val item = values[position]

        holder.idView.text = LineName(item)

        if (getItemViewType(position) == 1) {
            return  // 選択中の路線は選択不可
        }
        with(holder.itemView) {
            tag = item
            setOnClickListener(onClickListener)
        }
    }

    override fun getItemCount() = values.size

    override fun getItemViewType(position: Int): Int {
        return if (values[position] == srcLineId ) {
            1
        } else {
            0
        }
    }
}

private class ViewHolder(view: View) : RecyclerView.ViewHolder(view) {
    interface LineClickListener {
        fun onLineClick(view: View, lineId: Int)
    }

    val idView: TextView = view.id_line
}