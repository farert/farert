package org.sutezo.farert

import android.annotation.SuppressLint
import android.app.AlertDialog
import android.content.Context
import android.os.Bundle
import android.support.v4.app.Fragment
import android.support.v4.app.ShareCompat
import android.support.v4.content.res.ResourcesCompat
import android.support.v4.widget.DrawerLayout
import android.support.v7.app.ActionBarDrawerToggle
import android.support.v7.widget.AppCompatSpinner
import android.support.v7.widget.LinearLayoutManager
import android.support.v7.widget.RecyclerView
import android.support.v7.widget.Toolbar
import android.support.v7.widget.helper.ItemTouchHelper
import android.view.*
import android.widget.AdapterView
import android.widget.ArrayAdapter
import android.widget.CheckBox
import android.widget.TextView
import kotlinx.android.synthetic.main.folder_list.view.*
import kotlinx.android.synthetic.main.fragment_drawer.*
import org.sutezo.alps.RouteList
import org.sutezo.alps.fareNumStr
import org.sutezo.alps.kmNumStr
import org.sutezo.alps.terminalName

interface RecyclerClickListener {
    fun onClickRow(view: View, position: Int)
    fun onChangeItem(numItem: Int)
    fun onSliderChange(view: View?)
}

class FolderViewFragment : Fragment(), RecyclerClickListener {

    private lateinit var adapter: FolderRecyclerAdapter

    private var drawerListener: FragmentDrawerListener? = null
    private var mDrawerLayout: DrawerLayout? = null
    private var mContainerView: View? = null
    private val ticketFolder : Routefolder = Routefolder()
    private var mContext: Context? = null

    var route : RouteList? = null

    fun MutableList<Boolean>.swap(a: Int, b: Int) {
        val t = this[b]
        this[b] = this[a]
        this[a] = t
    }

    override fun onAttach(context: Context?) {
        super.onAttach(context)
        try {
            drawerListener = context as FragmentDrawerListener
            mContext = context
        } catch (e : RuntimeException) {
            e.printStackTrace()
        }
    }

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        return inflater.inflate(R.layout.fragment_drawer, container, false)
    }

    @SuppressLint("QueryPermissionsNeeded")
    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        ticketFolder.load(view.context)

        adapter = FolderRecyclerAdapter(ticketFolder, this)
        rv_drawer_list.adapter = adapter
        rv_drawer_list.layoutManager = LinearLayoutManager(activity)

        val touchHelper = ItemTouchHelper(object : ItemTouchHelper.SimpleCallback(
                ItemTouchHelper.UP or ItemTouchHelper.DOWN,
                /* ItemTouchHelper.RIGHT or ItemTouchHelper.LEFT*/0) {

            override fun onMove(recyclerView: RecyclerView, viewHolder: RecyclerView.ViewHolder, target: RecyclerView.ViewHolder): Boolean {

                val from = viewHolder.adapterPosition
                val to = target.adapterPosition

                adapter.routefolder.swap(adapter.mContext!!, from, to)
                adapter.mCheck?.swap(from, to)
                adapter.notifyItemMoved(from, to)

                return true
            }

            override fun onSwiped(viewHolder: RecyclerView.ViewHolder, direction: Int) {

                val idx = viewHolder.adapterPosition

                adapter.mContext?: return
                adapter.routefolder.remove(adapter.mContext!!, idx)
                adapter.mCheck?.removeAt(idx)
            }
        })

        touchHelper.attachToRecyclerView(rv_drawer_list)

        rv_drawer_list.addItemDecoration(touchHelper)
        rv_drawer_list.addOnItemTouchListener(RecyclerTouchListener(activity!!, rv_drawer_list, object : ClickListener {
            override fun onClick(view: View, position: Int) {
//                val route = (rv_drawer_list.adapter as FolderRecyclerAdapter).routefolder.routeItem(position)
//                drawerListener?.onDrawerItemSelected(view, route)
//                mDrawerLayout?.closeDrawer(mContainerView!!)
            }

            //close navigation view
            override fun onLongClick(view: View?, position: Int) {

            }

            override fun onSliderChange(view: View?) {

            }
        }))

        // [+Add] button
        btnAppend.setOnClickListener {
            if (route != null) {
                (rv_drawer_list.adapter as FolderRecyclerAdapter).add(view.context, route!!)
                updateFareInfo()
            }
        }
        // Delete button
        btnDelete.setOnClickListener {
            (rv_drawer_list.adapter as FolderRecyclerAdapter).deleteChecked()
            checkBox.isChecked = false
            updateFareInfo()
        }
        // Checkbox
        checkBox.setOnClickListener {
            (rv_drawer_list.adapter as FolderRecyclerAdapter).checkAll(checkBox.isChecked)
        }

        // export
        btnExport.setOnClickListener {
            activity.let {
                val text = ticketFolder.makeExportText()

                val mimeType = "text/plain"
                val subject = "チケットフォルダ"

                val shareIntent = ShareCompat.IntentBuilder.from(it)
                        .setChooserTitle(resources.getString(R.string.title_share_text))
                        .setType(mimeType)
                        .setText(text)
                        .setSubject(subject)
                        .intent
                if (shareIntent.resolveActivity(it!!.packageManager) != null) {
                    startActivity(shareIntent)
                }
            }
        }

        // Fare Information(集計値初期化)
        updateFareInfo()
    }

    @SuppressLint("NotifyDataSetChanged")
    fun reload(doCalc: Boolean?) {
        mContext?:return
        if (doCalc == true) {
            ticketFolder.load(mContext!!, doCalc)
        }
        rv_drawer_list.adapter?.notifyDataSetChanged()
        updateFareInfo()
    }

    fun init(fragmentId: Int, drawerLayout: DrawerLayout, toolbar: Toolbar) {
        mContainerView = activity!!.findViewById(fragmentId)
        mDrawerLayout = drawerLayout
        val drawerToggle = object : ActionBarDrawerToggle(activity, drawerLayout, toolbar, R.string.navigation_drawer_open, R.string.navigation_drawer_close) {
            // Leftview open
            override fun onDrawerOpened(drawerView: View) {
                super.onDrawerOpened(drawerView)
                activity!!.invalidateOptionsMenu()
                if (ticketFolder.count() <= 0) {
                    btnDelete.visibility = View.INVISIBLE
                    btnExport.visibility = View.INVISIBLE
                } else {
                    btnDelete.visibility = View.VISIBLE
                    btnExport.visibility = View.VISIBLE
                }
                if (route != null) {
                    btnAppend.visibility = View.VISIBLE
                } else {
                    btnAppend.visibility = View.INVISIBLE
                }
            }
            // Leftview close
            override fun onDrawerClosed(drawerView: View) {
                super.onDrawerClosed(drawerView)
                activity!!.invalidateOptionsMenu()
            }

            // Leftview open/close行数分呼ばれる
            override fun onDrawerSlide(drawerView: View, slideOffset: Float) {
                super.onDrawerSlide(drawerView, slideOffset)
                toolbar.alpha = 1 - slideOffset / 2
            }
        }

        mDrawerLayout?.addDrawerListener(drawerToggle)
        mDrawerLayout?.post { drawerToggle.syncState() }
    }

    interface ClickListener {
        fun onClick(view: View, position: Int)
        fun onLongClick(view: View?, position: Int)
        fun onSliderChange(view: View?)
    }

    private fun updateFareInfo() {
        txtTotalSalesKm.text = ticketFolder.totalSalesKm()
        txtTotalFare.text = ticketFolder.totalFare()
    }


    internal class RecyclerTouchListener(context: Context, recyclerView: RecyclerView, private val clickListener: ClickListener?) : RecyclerView.OnItemTouchListener {

        private val gestureDetector: GestureDetector = GestureDetector(context, object : GestureDetector.SimpleOnGestureListener() {
            override fun onSingleTapUp(e: MotionEvent): Boolean {
                return true
            }
            override fun onLongPress(e: MotionEvent) {
                val child = recyclerView.findChildViewUnder(e.x, e.y)
                if (child != null && clickListener != null) {
                    clickListener.onLongClick(child, recyclerView.getChildAdapterPosition(child))
                }
            }
        })

        override fun onInterceptTouchEvent(rv: RecyclerView, e: MotionEvent): Boolean {

            val child = rv.findChildViewUnder(e.x, e.y)
            if (child != null && clickListener != null && gestureDetector.onTouchEvent(e)) {
                clickListener.onClick(child, rv.getChildAdapterPosition(child))
            }
            return false
        }

        override fun onTouchEvent(rv: RecyclerView, e: MotionEvent) {}

        override fun onRequestDisallowInterceptTouchEvent(disallowIntercept: Boolean) {

        }
    }

    interface FragmentDrawerListener {
        fun onDrawerItemSelected(view: View, leftRouteList : RouteList)
    }

    internal class FolderRecyclerAdapter(val routefolder: Routefolder, val listener : RecyclerClickListener)
        : RecyclerView.Adapter<FolderRecyclerAdapter.MyViewHolder>() {

        var mContext : Context? = null
        var mCheck : MutableList<Boolean>? = null

        override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): MyViewHolder {
            val view = LayoutInflater.from(parent.context).inflate(R.layout.folder_list, parent, false)
            mContext = parent.context
            return MyViewHolder(view)
        }

        @SuppressLint("SetTextI18n")
        override fun onBindViewHolder(holder: MyViewHolder, position: Int) {
            val current = routefolder.routeItem(position)
            val begin = current.departureStationId()
            val term = current.arriveStationId()
            val label  =  "${terminalName(begin)} - ${terminalName(term)}"
            holder.title?.text  = label
            holder.fareType?.tag = position
            holder.fareType?.setSelection(routefolder.aggregateType(position).ordinal)
            holder.fareType?.onItemSelectedListener = object : AdapterView.OnItemSelectedListener{
                override fun onNothingSelected(parent: AdapterView<*>?) {

                }

                // 運賃種別選択
                override fun onItemSelected(parent: AdapterView<*>?, view: View?, position: Int, id: Long) {
                    parent?.let {
                        try {
                            val row_index: Int = it.tag as Int
                            val agt = Routefolder.Aggregate.values()[position]
                            routefolder.setAggregateType(it.context, row_index, agt)
                            notifyItemChanged(row_index) // 運賃額の変更
                            listener.onChangeItem(row_index)
                        } catch (e: Exception) {

                        }
                    }
                }
            }
            val fareValue  = routefolder.routeItemFareKm(position)
            holder.fareValue?.text  = "¥${fareNumStr(fareValue.first)}"    // 運賃額
            holder.salesKm?.text  = "${kmNumStr(fareValue.second)}km"
            if (mCheck == null || mCheck!!.count() != routefolder.count()) {
                mCheck = MutableList(routefolder.count()) {false}
            }
            holder.deleteCheck?.isChecked  = mCheck?.get(position) ?: false
            holder.deleteCheck?.tag = position
            holder.deleteCheck?.setOnClickListener { v: View? ->
                v?.let {
                    val idx = v.tag as Int
                    val chk = v as CheckBox
                    mCheck?.set(idx, chk.isChecked)
                }
            }
            holder.itemView.tag = position
            holder.itemView.setOnClickListener {
                listener.onClickRow(it, holder.itemView.tag as Int)
            }
        }

        override fun getItemCount(): Int {
            return routefolder.count()
        }

        // 経路追加ボタン
        @SuppressLint("NotifyDataSetChanged")
        fun add(context: Context, route : RouteList) {
            val rl = RouteList(route)    // ここでコピーをつくっておかないと追加経路がみな後に追加したものと同じになってしまう
            routefolder.add(context, rl)
            mCheck?.add(false)
            // notifyItemInserted(routefolder.count() - 1)
            mCheck = MutableList(routefolder.count()) {false}
            notifyDataSetChanged()
        }

        // 削除ボタン
        @SuppressLint("NotifyDataSetChanged")
        fun deleteChecked() {

            AlertDialog.Builder(mContext).apply {
                setTitle(R.string.query)
                setMessage(R.string.folder_content_query_clear_mesg)
                setPositiveButton("Yes") { _, _ ->
                    for (i in (routefolder.count() - 1) downTo  0) {
                        if (mCheck?.get(i) == true) {
                            mContext?.let {
                                routefolder.remove(it, i)
                                notifyItemRemoved(i)
                                //notifyItemRangeChanged(i, routefolder.count())
                            }
                        }
                    }
                    mCheck = MutableList(routefolder.count()) {false}

                    notifyDataSetChanged()
                }
                setNegativeButton("No", null)
                setIcon(mContext?.let { ResourcesCompat.getDrawable(it.resources, R.drawable.ic_question_answer, null) })
                create()
                show()
            }
        }

        // 削除用チェックボタンを全切り替え
        @SuppressLint("NotifyDataSetChanged")
        fun checkAll(value : Boolean) {
            mCheck = MutableList(routefolder.count()) {value}
            notifyDataSetChanged()
        }

        inner class MyViewHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {
            var title: TextView? = itemView.itemSection
            var fareType: AppCompatSpinner? = itemView.ticket_type
            var fareValue: TextView? = itemView.itemFare
            var deleteCheck: CheckBox? = itemView.chk_delete
            var salesKm: TextView? = itemView.itemKm
            init {
                if (mContext != null) {
                    val adapter = ArrayAdapter.createFromResource(mContext!!, R.array.list_ticket_type, R.layout.tv_folder_ticket_type)
                    adapter.setDropDownViewResource(R.layout.tv_folder_ticket_type_drop_down)
                    fareType?.adapter = adapter
                }
            }
        }
    }

    // folder ticket row clicked
    override fun onClickRow(view: View, position: Int) {
        val route = ticketFolder.routeItem(position)
        drawerListener?.onDrawerItemSelected(view, route)
        mDrawerLayout?.closeDrawer(mContainerView!!)
    }

    // 行追加, 運賃種別変更
    override fun onChangeItem(numItem: Int) {
        updateFareInfo()
    }
    override fun onSliderChange(view: View?) {

    }
}
