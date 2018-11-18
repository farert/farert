package org.sutezo.farert

import android.os.Bundle
import android.content.Intent
import android.graphics.Color
import android.graphics.Typeface
import android.support.v4.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.support.v7.widget.RecyclerView
import android.widget.TextView
import kotlinx.android.synthetic.main.row_line_list.view.*
import kotlinx.android.synthetic.main.row_station_list.view.*
import kotlinx.android.synthetic.main.station_list.view.*
import kotlinx.android.synthetic.main.terminal_first_list.view.*
import org.sutezo.alps.*

/**
 * A fragment representing a single LineListActivity detail screen.
 * This fragment is either contained in a [LineListActivity]
 * in two-pane mode (on tablets) or a [StationListActivity]
 * on handsets.
 */
class StationListFragment : Fragment(), StationListRecyclerViewAdapter.RecyclerListener {

    /**
     * The dummy content this fragment is presenting.
     */
    var mode : String = ""          // terminal|autoroute|route
    var lineId : Int = 0
    var srcType:String = ""         // prefect|company
    var srcCompanyOrPrefectId: Int = 0
    var stationMode : String = ""    // junction|
    var srcStationId : Int = 0

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        arguments?.let {
            mode = it.getString("mode", "")
            lineId = it.getInt("line_id", 0)
            srcStationId = it.getInt("src_station_id", 0)
            srcType = it.getString("line_to_type", "")
            srcCompanyOrPrefectId = it.getInt("line_to_id", 0)
            stationMode = it.getString("station_mode", "")
        }
    }

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?,
                              savedInstanceState: Bundle?): View? {
        val rootView = inflater.inflate(R.layout.station_list, container, false)

        var list = {

            if (mode != "route") {
                // terminal or autoroute
                stationsWithInCompanyOrPrefectAnd(srcCompanyOrPrefectId, lineId)
            } else {
                if (stationMode == "junction") {
                    junctionIdsOfLineId(lineId)
                } else {
                    stationsIdsOfLineId(lineId)
                }
            }
        }
        rootView.station_list.adapter = StationListRecyclerViewAdapter(lineId, srcStationId, list(), this)
        return rootView
    }

    override fun onClickRow(stationId: Int) {
        if (stationId == srcStationId) { return }
        val activity = activity
        val intent = Intent(activity, MainActivity::class.java).apply {
            putExtra("dest_station_id", stationId)
            putExtra("mode", mode)
            putExtra("line_id", if (mode == "route") lineId else 0)
            //addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP or Intent.FLAG_ACTIVITY_FORWARD_RESULT)
            addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP or Intent.FLAG_ACTIVITY_SINGLE_TOP)
        }
        //activity!!.setResult(Activity.RESULT_OK, intent)
        //activity!!.startActivity(intent)
        startActivity(intent)
    }
}

class StationListRecyclerViewAdapter(private val lineId: Int,
                                     private val seledStaionId: Int,
                                     private val values: List<Int>,
                                     private val listner: StationListRecyclerViewAdapter.RecyclerListener) :
        RecyclerView.Adapter<StationListRecyclerViewAdapter.ViewHolder>() {

    private val onClickListener: View.OnClickListener

    init {
        onClickListener = View.OnClickListener { v ->
            val item = v.tag as Int
            listner.onClickRow(item)
        }
    }
    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
        val view = LayoutInflater.from(parent.context)
                .inflate(R.layout.row_station_list, parent, false)
        if (viewType == 1) {
            with(view.row_station_item) { //行全体背景をグレーに
                setBackgroundColor(Color.parseColor("gray"))
            }
            view.id_text_station.typeface  = Typeface.DEFAULT_BOLD
            view.id_text_lines_of_station.typeface  = Typeface.DEFAULT_BOLD
        }
        return ViewHolder(view)
    }

    override fun onBindViewHolder(holder: ViewHolder, position: Int) {
        val item = values[position]

        // 駅名
        holder.idView.text = RouteUtil.StationNameEx(item)

        // 駅の所属する路線一覧を表示(item:駅, lineId:除外する路線)
        holder.contentView.text = getDetailStationInfoForSelList(lineId, item)

        if (getItemViewType(position) == 1) {
            return
        }
        with(holder.itemView) {
            tag = item
            setOnClickListener(onClickListener)
        }
    }

    override fun getItemCount() = values.size

    override fun getItemViewType(position: Int): Int {
        return if (values[position] == seledStaionId) 1 else 0
    }

    inner class ViewHolder(view: View) : RecyclerView.ViewHolder(view) {
        val idView: TextView = view.id_text_station
        val contentView: TextView = view.id_text_lines_of_station
    }

    interface RecyclerListener {
        fun onClickRow(stationId: Int)
    }
}

