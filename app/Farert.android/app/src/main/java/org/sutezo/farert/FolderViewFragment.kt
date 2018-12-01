package org.sutezo.farert

import android.content.Context
import android.os.Bundle
import android.support.v4.app.Fragment
import android.support.v4.widget.DrawerLayout
import android.support.v7.app.ActionBarDrawerToggle
import android.support.v7.widget.LinearLayoutManager
import android.support.v7.widget.RecyclerView
import android.support.v7.widget.Toolbar
import android.view.*
import kotlinx.android.synthetic.main.folder_list.view.*
import kotlinx.android.synthetic.main.fragment_drawer.*
import org.sutezo.alps.Route


class FolderViewFragment : Fragment() {

    private lateinit var adapter: FolderRecyclerAdapter

    private var drawerListener: FragmentDrawerListener? = null
    private var mDrawerLayout: DrawerLayout? = null
    private var containerView: View? = null


    override fun onAttach(context: Context?) {
        super.onAttach(context)
        try {
            drawerListener = context as FragmentDrawerListener
        } catch (e : RuntimeException) {
            e.printStackTrace()
        }
    }

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        return inflater.inflate(R.layout.fragment_drawer, container, false)
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
 //       var titles = activity.resources.getStringArray(R.array.nav_drawer_labels)
 //       val data = ArrayList<String>()
 //       for (i in titles.indices) {
 //           val navItem = DrawerItem(title = titles[i])
 //           data.add(navItem)
 //       }

        adapter = FolderRecyclerAdapter(/*data*/)
        rv_drawer_list.adapter = adapter
        rv_drawer_list.layoutManager = LinearLayoutManager(activity)
        rv_drawer_list.addOnItemTouchListener(RecyclerTouchListener(activity!!, rv_drawer_list, object : ClickListener {
            override fun onClick(view: View, position: Int) {
                drawerListener?.onDrawerItemSelected(view, position)
                mDrawerLayout?.closeDrawer(containerView!!)
            }

            override fun onLongClick(view: View?, position: Int) {

            }
        }))
    }

    fun init(fragmentId: Int, drawerLayout: DrawerLayout, toolbar: Toolbar) {
        containerView = activity!!.findViewById(fragmentId)
        mDrawerLayout = drawerLayout
        val drawerToggle = object : ActionBarDrawerToggle(activity, drawerLayout, toolbar, R.string.navigation_drawer_open, R.string.navigation_drawer_close) {
            override fun onDrawerOpened(drawerView: View) {
                super.onDrawerOpened(drawerView)
                activity!!.invalidateOptionsMenu()
            }

            override fun onDrawerClosed(drawerView: View) {
                super.onDrawerClosed(drawerView)
                activity!!.invalidateOptionsMenu()
            }

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
    }

    internal class RecyclerTouchListener(context: Context, recyclerView: RecyclerView, private val clickListener: ClickListener?) : RecyclerView.OnItemTouchListener {

        private val gestureDetector: GestureDetector

        init {
            gestureDetector = GestureDetector(context, object : GestureDetector.SimpleOnGestureListener() {
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
        }

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
        fun onDrawerItemSelected(view: View, position: Int)
    }


    //class FolderRecyclerAdapter(val context: Context,
//                            val itemClickListener: RecyclerViewHolder.ItemClickListener,
//                            private val itemList:List<String>) : RecyclerView.Adapter<RecyclerViewHolder>() {
    internal class FolderRecyclerAdapter() : RecyclerView.Adapter<FolderRecyclerAdapter.MyViewHolder>() {
        val itemList = arrayListOf<String>("東京-品川", "札幌-沼宮内", "武蔵小杉-博多","渋谷-大久保", "上越国際スキー場前-高崎問屋町")

        override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): MyViewHolder {
            val view = LayoutInflater.from(parent.context).inflate(R.layout.folder_list, parent, false)
            return MyViewHolder(view)
        }

        override fun onBindViewHolder(holder: MyViewHolder, position: Int) {
            val current = itemList[position]
            holder.title.text = current
        }

        override fun getItemCount(): Int {
            return itemList.size
        }

        inner class MyViewHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {
            var title = itemView.itemSection
            init {
            }
        }
    }
}
