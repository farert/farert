package org.sutezo.farert

import android.app.AlertDialog
import android.content.Context
import android.content.Intent
import android.graphics.Color
import android.graphics.Typeface
import android.os.Build
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.support.v7.widget.RecyclerView
import android.support.v7.widget.helper.ItemTouchHelper
import android.view.*
import android.widget.TextView
import kotlinx.android.synthetic.main.activity_archive_route.*
import kotlinx.android.synthetic.main.activity_terminal_select.*
import kotlinx.android.synthetic.main.content_archive_route_list.view.*
import kotlinx.android.synthetic.main.content_list_empty.*
import org.sutezo.alps.*

class ArchiveRouteActivity : AppCompatActivity(),
        ArchiveRouteListRecyclerViewAdapter.ClickListener {

    private var mCurRouteScript : String = ""
    private var menu : Menu? = null

    private var mNumOfArchive : Int = -1
    private var mContainCurRoute : Boolean = false

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_archive_route)

        setTitle(R.string.title_main_menu_archive)

        // back arrow button(戻るボタン有効)
        supportActionBar?.setDisplayHomeAsUpEnabled(true)

        val ds = Route()
        ds.assign((application as FarertApp).ds)

        var listItems = readParams(this, KEY_ARCHIVE)

        if (RouteDB.getInstance().isLatest()) {
            listItems = listItems.filter { item -> !item.contains("{") }
        } else {
            val dbknd = "{${RouteDB.getInstance().name()}}"
            listItems.filter { item-> item.contains(dbknd) }
            listItems = listItems.map { item-> item.replaceFirst(dbknd, "") }
        }

        mNumOfArchive = listItems.count()
        mContainCurRoute = false
        if (0 < ds.count) {
            mCurRouteScript = ds.route_script()
            if (listItems.contains(mCurRouteScript)) {
                mContainCurRoute = true
            }
        } else {
            mCurRouteScript = ""
        }
        if (mCurRouteScript != "" && !mContainCurRoute) {
            // 現在経路がすでに保存されていなければ、現在経路を先頭にリストする
            val tmp = mutableListOf(mCurRouteScript)
            tmp.addAll(listItems)
            listItems = tmp.toList()
        }
        if (listItems.isEmpty()) {
            setContentView(R.layout.content_list_empty)
            list_empty.text = resources.getString(R.string.no_archive_route)
        } else {
            archive_route_list.adapter = ArchiveRouteListRecyclerViewAdapter(listItems, mCurRouteScript, this)
            val swipeHandler = object : SwipeToDeleteCallback(this) {
                override fun onSwiped(viewHolder: RecyclerView.ViewHolder, direction: Int) {
                    val adapter = archive_route_list.adapter as ArchiveRouteListRecyclerViewAdapter
                    adapter.removeAt(viewHolder.adapterPosition)
                }
            }
            val itemTouchHelper = ItemTouchHelper(swipeHandler)
            itemTouchHelper.attachToRecyclerView(archive_route_list)
        }
    }


    override fun onCreateOptionsMenu(menu: Menu?): Boolean {
        menuInflater.inflate(R.menu.menu_archive_route, menu)
        return super.onCreateOptionsMenu(menu)
    }

    override fun onPrepareOptionsMenu(menu: Menu?): Boolean {

        val m = menu?: return super.onPrepareOptionsMenu(menu)
        val mnuClear = m.findItem(R.id.menu_item_all_delete)

        this.menu = m

        mnuClear.setEnabled(0 < mNumOfArchive)

        val mnuSave = m.findItem(R.id.menu_item_save)
        mnuSave.setEnabled(!mContainCurRoute && mCurRouteScript != "")

        return super.onPrepareOptionsMenu(menu)
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        val id = item.itemId
        when (id) {
            android.R.id.home -> {
                finish()
                return true
            }
            R.id.menu_item_save -> {
                val trv = archive_route_list.adapter as ArchiveRouteListRecyclerViewAdapter
                trv.saveParams(this)
                item.setEnabled(false)
            }
            R.id.menu_item_all_delete -> {
                AlertDialog.Builder(this).apply {
                    setTitle(R.string.main_alert_query_all_clear_title)
                    setMessage(R.string.main_alert_query_all_clear_mesg)
                    setPositiveButton("Yes") { _, _ ->
                        val trv = archive_route_list.adapter as ArchiveRouteListRecyclerViewAdapter
                        trv.clearContents()
                        item.setEnabled(false)
                    }
                    setNegativeButton("No", null)
                    show()
                }
            }
        }
        return super.onOptionsItemSelected(item)
    }

    override fun onClickRow(context: Context, routeScript: String) {

        if (mCurRouteScript != "" && routeScript != mCurRouteScript && !mContainCurRoute) {

            AlertDialog.Builder(context).apply {
                setTitle(R.string.main_alert_query_route_update_title)
                setMessage(R.string.main_alert_query_route_update_mesg)
                setPositiveButton("Yes") { _, _ ->
                    doChangeRoute(routeScript)
                }
                setNegativeButton("No") {_, _ ->

                }
                setCancelable(false)
                show()
            }
        } else {
            doChangeRoute(routeScript)
        }
    }

    private fun doChangeRoute(routeScript: String) {
        val intent = Intent(this, MainActivity::class.java).apply {
            putExtra("mode", "archive")
            putExtra("script", routeScript)
            addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP or Intent.FLAG_ACTIVITY_SINGLE_TOP)
        }
        finish()
        startActivity(intent)
    }


    override fun onChangeItem(numItem: Int) {

        this.menu?.apply {
            val mi_clear = findItem(R.id.menu_item_all_delete)
            val mi_save = findItem(R.id.menu_item_save)

            //val trv = list_terminal.adapter as TerminalRecyclerViewAdapter
            //mi.setEnabled(0 < trv.itemCount)
            mi_clear.setEnabled(0 < numItem)
            mi_save.setEnabled(false)
        }
    }


    companion object {
        val KEY_ARCHIVE = "archive_route"
    }
}


////////////////////////////////////////////////////////////////

private class ArchiveRouteListRecyclerViewAdapter(private var values: List<String>,
                                                  private val curRouteScript : String,
                                                  private val listener : ArchiveRouteListRecyclerViewAdapter.ClickListener) :
        RecyclerView.Adapter<ArchiveRouteListRecyclerViewAdapter.ViewHolder>() {

    private val onClickListener: View.OnClickListener
    private var context : Context? = null
    private var saveFlag : Boolean = false

    init {
        onClickListener = View.OnClickListener { v ->
            listener.onClickRow(context!!, v.id_route.text.toString())
        }
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
        val view = LayoutInflater.from(parent.context)
                .inflate(R.layout.content_archive_route_list, parent, false)
        if (viewType == 1) {
            with(view.id_route) {
                setTextColor(Color.parseColor("red"))
                typeface = Typeface.DEFAULT_BOLD
            }
        }
        this.context = parent.context
        return ViewHolder(view)
    }

    override fun onBindViewHolder(holder: ViewHolder, position: Int) {
        val item = values[position]

        with(holder.itemView) {
            holder.idView.text = item
            setOnClickListener(onClickListener)
        }
    }

    override fun getItemCount() = values.size

    override fun getItemViewType(position: Int): Int {
        return if (values[position] == curRouteScript && !saveFlag) {
            1
        } else {
            0
        }
    }

    fun removeAt(position: Int) {
        val removedVal = values.filterIndexed { index, _ ->  index != position }
        values = removedVal
        context?.let {
            saveParam(it, ArchiveRouteActivity.KEY_ARCHIVE, values)
            saveFlag = true
        }
        notifyItemRemoved(position)
        notifyItemRangeChanged(position, values.size)

        listener.onChangeItem(if (curRouteScript != "") itemCount - 1 else itemCount)
    }

    fun clearContents() {
        values = listOf()
        context?.let {
            saveParam(it, ArchiveRouteActivity.KEY_ARCHIVE, values)
            saveFlag = true
        }
        listener.onChangeItem(if (curRouteScript != "") itemCount - 1 else itemCount)
        notifyDataSetChanged()
    }

    fun saveParams(context : Context) {
        if (!RouteDB.getInstance().isLatest) {
            val dbkind = RouteDB.getInstance().name()
            val v = values.map { item -> "{${dbkind}}${item}" }
            values = v
        }
        saveParam(context, ArchiveRouteActivity.KEY_ARCHIVE, values)
        saveFlag = true
        notifyDataSetChanged()
    }

    inner class ViewHolder(view: View) : RecyclerView.ViewHolder(view) {
        val idView: TextView = view.id_route
    }

    interface ClickListener {
        fun onClickRow(context: Context, routeScript: String)
        fun onChangeItem(numItem: Int)
    }
}
