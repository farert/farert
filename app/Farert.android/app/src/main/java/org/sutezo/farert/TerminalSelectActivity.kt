package org.sutezo.farert

import android.annotation.SuppressLint
import android.app.AlertDialog
import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.support.design.widget.TabLayout
import android.support.v4.app.Fragment
import android.support.v4.app.FragmentManager
import android.support.v4.app.FragmentPagerAdapter
import android.support.v4.content.ContextCompat
import android.support.v4.view.ViewPager
import android.support.v7.app.AppCompatActivity
import android.support.v7.widget.DividerItemDecoration
import android.support.v7.widget.RecyclerView
import android.support.v7.widget.SearchView
import android.support.v7.widget.helper.ItemTouchHelper
import android.util.Log
import android.view.*
import android.widget.TextView
import kotlinx.android.synthetic.main.activity_terminal_select.*
import kotlinx.android.synthetic.main.content_list_empty.view.*
import kotlinx.android.synthetic.main.fragment_terminal_select.*
import kotlinx.android.synthetic.main.fragment_terminal_select.view.*
import kotlinx.android.synthetic.main.terminal_first_list.view.*
import org.sutezo.alps.*


class TerminalSelectActivity : AppCompatActivity() {

    /**
     * The [android.support.v4.view.PagerAdapter] that will provide
     * fragments for each of the sections. We use a
     * {@link FragmentPagerAdapter} derivative, which will keep every
     * loaded fragment in memory. If this becomes too memory intensive, it
     * may be best to switch to a
     * [android.support.v4.app.FragmentStatePagerAdapter].
     */
    private var mSectionsPagerAdapter: SectionsPagerAdapter? = null
    var mode : String? = null
    enum class TAB_PAGE(val rowValue : Int) {
        COMPANY(0),
        PREFECT(1),
        HISTORY(2),
    }


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_terminal_select)

        mode = intent.getStringExtra("mode")
        if (mode == "autoroute") {
            toolbar.title = resources.getString(R.string.title_terminal_select_arrive)
        } else { // "mode" = "terminal"
            toolbar.title = resources.getString(R.string.title_terminal_select_depart)
        }
        setSupportActionBar(toolbar)
        // back arrow button(戻るボタン有効)
        supportActionBar?.setDisplayHomeAsUpEnabled(true)
        supportActionBar?.setHomeButtonEnabled(true)

        // Create the adapter that will return a fragment for each of the three
        // primary sections of the activity.
        mSectionsPagerAdapter = SectionsPagerAdapter(supportFragmentManager)

        // Set up the ViewPager with the sections adapter.
        tabcontainer.adapter = mSectionsPagerAdapter

        tabcontainer.addOnPageChangeListener(object : ViewPager.OnPageChangeListener {

            override fun onPageScrollStateChanged(state: Int) {

            }

            override fun onPageScrolled(position: Int, positionOffset: Float, positionOffsetPixels: Int) {

            }
            override fun onPageSelected(position: Int) {
                val m = toolbar.menu ?: return
                val mi = m.findItem(R.id.menu_item_all_delete) ?: return

                mi.isVisible = (position == TAB_PAGE.HISTORY.rowValue) // show 'History' tab only
                if (position == TAB_PAGE.HISTORY.rowValue) {
                    //val trv = list_terminal.adapter as TerminalRecyclerViewAdapter
                    //mi.setEnabled(0 < trv.itemCount)
                    val flgm = mSectionsPagerAdapter?.findFragmentByPosition(
                            tabcontainer, TAB_PAGE.HISTORY.rowValue) as? PlaceholderFragment
                    flgm?.let {
                        mi.setEnabled(0 < it.numOfContent())
                    }
                }
            }
        })
        tabcontainer.addOnPageChangeListener(TabLayout.TabLayoutOnPageChangeListener(tabs))
        tabs.addOnTabSelectedListener(TabLayout.ViewPagerOnTabSelectedListener(tabcontainer))
    }

    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        // Inflate the menu; this adds items to the action bar if it is present.
        menuInflater.inflate(R.menu.menu_terminal_select, menu)
        val searchItem = menu.findItem(R.id.menu_search) ?: return false
        val searchView = searchItem.actionView as? SearchView ?: return false
        searchView.queryHint = resources.getString(R.string.label_search_title) // "駅名（よみ）入力"
        searchView.setOnQueryTextListener(object : SearchView.OnQueryTextListener {
            override fun onQueryTextSubmit(text: String?): Boolean {
                // 検索キーが押下された
                //searchview.clearFocus()
                //Log.d(TAG, "submit text: $text")
                return false
            }
            override fun onQueryTextChange(text: String?): Boolean {
                // テキストが変更された
                //Log.d(TAG, "change text: $text")
                if (!text.isNullOrBlank()) {
                    tabs.visibility = View.GONE
                    tabcontainer.visibility = View.GONE
                    container.visibility = View.VISIBLE
                    val fragment = PlaceholderFragment.newInstance(
                            PlaceholderFragment.LIST_TYPE.SEARCH.ordinal, text, mode!!)
                    supportFragmentManager
                            .beginTransaction()
                            .replace(R.id.container, fragment)
                            .commit()
                } else {
                    container.visibility = View.GONE
                    tabs.visibility = View.VISIBLE
                    tabcontainer.visibility = View.VISIBLE
                }
                return false
            }
        })



        searchItem.setOnActionExpandListener(object : MenuItem.OnActionExpandListener {
            override fun onMenuItemActionExpand(item: MenuItem): Boolean {
                Log.d("TAG","onMenuItemActionExpand")
                tabs.visibility = View.VISIBLE

                return true
            }

            override fun onMenuItemActionCollapse(item: MenuItem): Boolean {
                Log.d("TAG","onMenuItemActionCollpa")
                return true
            }
        })

        searchView.setOnSearchClickListener {
            Log.d("TAG","on search click")
            //searchView.onActionViewExpanded()
        }
        searchView.setOnCloseListener {
            Log.d("TAG", "on close")
            searchView.onActionViewCollapsed()
            searchView.clearFocus()

            true
        }

        searchView.setOnClickListener {
            Log.d("TAG", "setOnClickListener")
        }

        return true
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.

        when (item.itemId) {
            R.id.action_settings -> {
                return true
            }
            android.R.id.home -> { // back arrow button(戻る)
                finish()
            }
            R.id.menu_item_all_delete -> {
                // 履歴 全削除ボタン処理
                AlertDialog.Builder(this).apply {
                    setTitle(R.string.main_alert_query_all_clear_title)
                    setMessage(R.string.main_alert_query_all_clear_mesg)
                    setPositiveButton("Yes") { _, _ ->
                        val frgm = mSectionsPagerAdapter?.findFragmentByPosition(
                                tabcontainer, TAB_PAGE.HISTORY.rowValue) as? PlaceholderFragment
                        frgm?.clearContents()
                        saveHistory(context, listOf())
                    }
                    setNegativeButton("No", null)
                    create()
                    show()
                }
            }
            else -> {

            }
        }
        return super.onOptionsItemSelected(item)
    }

    //PlaceholderFragment で履歴削除後に呼ばれるように仕向けた
    //
    fun onChangeList(numItem: Int) {
        val m = toolbar.menu ?: return
        val mi = m.findItem(R.id.menu_item_all_delete) ?: return
        // show 'History' tab only
        if (mi.isVisible) {
              //val trv = list_terminal.adapter as TerminalRecyclerViewAdapter
              //mi.setEnabled(0 < trv.itemCount)
            mi.isEnabled = 0 < numItem
        }
    }

    /**
     * A [FragmentPagerAdapter] that returns a fragment corresponding to
     * one of the sections/tabs/pages.
     */
    inner class SectionsPagerAdapter(fm: FragmentManager) : FragmentPagerAdapter(fm) {

        override fun getItem(position: Int): Fragment {
            // getItem is called to instantiate the fragment for the given page.
            // Return a PlaceholderFragment (defined as a static inner class below).
            // history

            return PlaceholderFragment.newInstance(position, "", mode!!)
        }

        override fun getCount(): Int {
            // Show 3 total pages.
            return 3
        }

        fun findFragmentByPosition(viewPager: ViewPager,
                                   position: Int): Fragment {
            return instantiateItem(viewPager, position) as Fragment
        }
    }

    /**
     * A placeholder fragment containing a simple view.
     */
    class PlaceholderFragment : Fragment(), TerminalRecyclerViewAdapter.RecyclerListener {

        enum class LIST_TYPE {
            COMPANY,
            PREFECT,
            HISTORY,
            SEARCH,
        }
        private var mIdType : LIST_TYPE = LIST_TYPE.COMPANY
        private var mode : String = ""
        private var mNumHistory : Int = 0

        override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?,
                                  savedInstanceState: Bundle?): View? {

            val i = arguments?.get("page_type") as? Int ?: 0
            mIdType = LIST_TYPE.values()[i % LIST_TYPE.values().size]
            mode = arguments?.get("mode") as? String ?: ""

            val listItems : () -> List<Int> =  {
                when (mIdType) {
                    LIST_TYPE.COMPANY -> { // company
                        getCompanys()
                    }
                    LIST_TYPE.PREFECT -> { // prefect
                        getPrefects()
                    }
                    LIST_TYPE.HISTORY -> { // history
                        readParams(activity!!, "history").map{ RouteUtil.GetStationId(it) }.filter { 0 < it }
                    }
                    LIST_TYPE.SEARCH -> { //search text
                        keyMatchStations(arguments?.getString("search_text", "x") ?: "null")
                    }
                }
            }
            val lists = listItems()
            mNumHistory = lists.count()
            if (mIdType == LIST_TYPE.HISTORY && mNumHistory <= 0) {
                val rootView = inflater.inflate(R.layout.content_list_empty, container, false)
                rootView.list_empty.text = resources.getString(R.string.no_history)
                return rootView
            }
            val rootView = inflater.inflate(R.layout.fragment_terminal_select, container, false)

            rootView.list_terminal.addItemDecoration(DividerItemDecoration(activity, DividerItemDecoration.VERTICAL))
            rootView.list_terminal.adapter = TerminalRecyclerViewAdapter(lists, mIdType, this)

            if (mIdType == LIST_TYPE.HISTORY) {
                // 履歴ビューではスワイプによる個別行削除ができるように
                val swipeHandler = object : SwipeToDeleteCallback(activity as Context) {
                    override fun onSwiped(viewHolder: RecyclerView.ViewHolder, direction: Int) {
                        val adapter = rootView.list_terminal.adapter as? TerminalRecyclerViewAdapter
                        adapter?.removeAt(viewHolder.adapterPosition)
                    }
                }
                val itemTouchHelper = ItemTouchHelper(swipeHandler)
                itemTouchHelper.attachToRecyclerView(rootView.list_terminal)
            }
            return rootView
        }

        companion object {
            /**
             * Returns a new instance of this fragment for the given section
             * number.
             */
            fun newInstance(sectionNumber: Int, search_text: String = "", mode: String = ""): PlaceholderFragment {
                val fragment = PlaceholderFragment()
                val args = Bundle().apply {
                    putInt("page_type", sectionNumber)
                    putString("search_text", search_text)
                    putString("mode", mode)
                }
                fragment.arguments = args
                return fragment         // SectionsPagerAdapter.getItem(position)
            }
        }

        override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
            super.onViewCreated(view, savedInstanceState)
            // 検索ビューのとき、リストの上に件数(5件を越えたら)を表示する
            // それ以外ビューでは、非表示にする。
            // 履歴で件数0の場合はレイアウト自体が別物なのでなにもしない
            if (mIdType == LIST_TYPE.SEARCH && 5 < mNumHistory) {
                textViewHeader.visibility =  View.VISIBLE
                textViewHeader.text = resources.getString(R.string.match_disp, mNumHistory)
            } else if (mIdType != LIST_TYPE.HISTORY || 0 < mNumHistory) {
                textViewHeader.visibility =  View.GONE
            }
        }

        override fun onClickRow(selectItem: Int, listType: LIST_TYPE) {
            when (listType) {
                // History page / Search view ?
                LIST_TYPE.HISTORY, LIST_TYPE.SEARCH -> {
                    // 履歴
                    // 検索結果
                    val intent = Intent(activity, MainActivity::class.java).apply {
                        putExtra("dest_station_id", selectItem)
                        putExtra("mode", mode) // 0:Terminal
                        addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP or Intent.FLAG_ACTIVITY_CLEAR_TOP)
                    }
                    startActivity(intent)   // activity!!つけないとどう違う？
                }
                LIST_TYPE.COMPANY, LIST_TYPE.PREFECT -> {
                    val intent = Intent(activity!!, LineListActivity::class.java).apply {
                        putExtra("mode", mode) // 0:Terminal
                        putExtra("line_to_id", selectItem)
                        putExtra("line_to_type", if (listType == LIST_TYPE.COMPANY) "company" else "prefect")
                        // addFlags(Intent.FLAG_ACTIVITY_FORWARD_RESULT)
                    }
                    activity!!.startActivity(intent)
                }
            }
        }
        fun clearContents() {
            if (mIdType != LIST_TYPE.HISTORY) {
                return  // 'History' tab only
            }
            view?.let {
                val trv = list_terminal.adapter as? TerminalRecyclerViewAdapter
                trv?.clearContents()
            }
        }
        fun numOfContent() : Int {
            view?.let {
                return mNumHistory
            }
            return 0
        }

        // Item削除した直後にボタン無効にする
        override fun onChangeItem(numItem: Int) {
            mNumHistory = numItem
            (activity as TerminalSelectActivity).onChangeList(numItem)
        }
    }

    internal class TerminalRecyclerViewAdapter(private var values: List<Int>,
                                               private val listType:PlaceholderFragment.LIST_TYPE,
                                               private val listener: RecyclerListener) :
            RecyclerView.Adapter<TerminalRecyclerViewAdapter.ViewHolder>() {

        // listType: 0 = company
        //           1 = prefect
        //           2 = history
        //           3 = search view

        private val onClickListener: View.OnClickListener = View.OnClickListener { v ->
            val selitem = v.id_text.tag as Int
            listener.onClickRow(selitem, listType)
        }
        private var context : Context? = null

        override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
            val view = LayoutInflater.from(parent.context)
                    .inflate(R.layout.terminal_first_list, parent, false)

            context = parent.context

            if (viewType != 3) {
                view.id_detail_text.visibility = View.GONE
            }
            if (viewType == 2 || viewType == 3) {
                context?.let {
                    val c = ContextCompat.getColor(it, R.color.colorTextLink)
                    if (viewType == 2) {
                        // history
                        view?.id_text?.setTextColor(c)
                    } else {
                        // search view
                        val c1 = ContextCompat.getColor(it, R.color.colorTextSub)
                        view?.id_text?.setTextColor(c1)
                        view?.id_detail_text?.setTextColor(c)
                    }
                }
            }
            return ViewHolder(view)
        }

        override fun onBindViewHolder(holder: ViewHolder, position: Int) {
            val item = values[position]
            holder.idView.tag = item
            when (listType) {
                PlaceholderFragment.LIST_TYPE.COMPANY-> {
                    holder.idView.text = RouteUtil.CompanyName(item)
                }
                PlaceholderFragment.LIST_TYPE.PREFECT-> {
                    holder.idView.text = RouteUtil.PrefectName(item)
                }
                PlaceholderFragment.LIST_TYPE.HISTORY, PlaceholderFragment.LIST_TYPE.SEARCH -> {
                    holder.idView.text = RouteUtil.StationNameEx(item)
                }
            }
            //no need // holder.idView.tag = item
            if (getItemViewType(position) == PlaceholderFragment.LIST_TYPE.SEARCH.ordinal) {
                val s = "${RouteUtil.GetKanaFromStationId(item)}\t(${RouteUtil.GetPrefectByStationId(item)} )"
                holder.idDetailView.text = s
            }
            holder.itemView.setOnClickListener(onClickListener)
        }

        override fun getItemViewType(position: Int) = listType.ordinal

        override fun getItemCount() = values.size

        /*
        fun addItem(name: String) {
            values.add(name)
            notifyItemInserted(values.size)
        }*/

        @SuppressLint("NotifyDataSetChanged")
        fun removeAt(position: Int) {
            val removedVal = values.filterIndexed { index, _ ->  index != position }
            values = removedVal
            context?.let { it ->
                saveHistory(it, values.map { RouteUtil.StationNameEx(it) })
            }
            notifyItemRemoved(position)
            listener.onChangeItem(itemCount)
            notifyDataSetChanged()
        }

        @SuppressLint("NotifyDataSetChanged")
        fun clearContents() {
            values = listOf()
            notifyDataSetChanged()
            listener.onChangeItem(itemCount)
        }

        inner class ViewHolder(view: View) : RecyclerView.ViewHolder(view) {
            val idView: TextView = view.id_text
            val idDetailView : TextView = view.id_detail_text
        }
        interface RecyclerListener {
            fun onClickRow(selectItem: Int, listType: PlaceholderFragment.LIST_TYPE)
            fun onChangeItem(numItem: Int)
        }
    }
}
