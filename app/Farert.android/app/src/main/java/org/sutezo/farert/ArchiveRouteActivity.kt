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

/**
 * 経路保存画面
 */
class ArchiveRouteActivity : AppCompatActivity(),
        ArchiveRouteListRecyclerViewAdapter.ClickListener {

    private var mCurRouteScript : String = ""
    private var menu : Menu? = null

    private var mNumOfArchive : Int = -1
    private var mContainCurRoute : Boolean = false

    /**
     *
     */
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_archive_route)

        setTitle(R.string.title_main_menu_archive)

        // back arrow button(戻るボタン有効)
        supportActionBar?.setDisplayHomeAsUpEnabled(true)

        val ds = Route()
        (application as? FarertApp)?.ds.let {
            ds.assign(it)
        }

        // 保存経路リスト
        var listItems = readParams(this, KEY_ARCHIVE)

        mNumOfArchive = listItems.count()
        mContainCurRoute = false
        if (0 < ds.count) {
            mCurRouteScript = ds.route_script() // 現在表示中の経路
            if (listItems.contains(mCurRouteScript)) {
                mContainCurRoute = true // カレント表示経路はすでに保存されている
            }
        } else {
            mCurRouteScript = ""
        }
        if ((mCurRouteScript != "") && !mContainCurRoute) {
            // 現在経路がすでに保存されていなければ、現在経路を先頭にリストする
            val tmp = mutableListOf(mCurRouteScript)
            tmp.addAll(listItems)
            listItems = tmp.toList()
        }
        if (listItems.isEmpty()) {
            setContentView(R.layout.content_list_empty)
            list_empty.text = resources.getString(R.string.no_archive_route)
        } else {
            archive_route_list.adapter = ArchiveRouteListRecyclerViewAdapter(listItems,
                                                                             mCurRouteScript,
                                                                             mContainCurRoute,
                                                                             this)
            val swipeHandler = object : SwipeToDeleteCallback(this) {
                override fun onSwiped(viewHolder: RecyclerView.ViewHolder, direction: Int) {
                    val adapter = archive_route_list.adapter
                            as ArchiveRouteListRecyclerViewAdapter
                    adapter.removeAt(viewHolder.adapterPosition)
                }
            }
            val itemTouchHelper = ItemTouchHelper(swipeHandler)
            itemTouchHelper.attachToRecyclerView(archive_route_list)
        }
    }

    /**
     *
     */
    override fun onCreateOptionsMenu(menu: Menu?): Boolean {
        menuInflater.inflate(R.menu.menu_archive_route, menu)
        return super.onCreateOptionsMenu(menu)
    }

    /**
     *  menuの有効化/無効化を制御する(保存)
     */
    override fun onPrepareOptionsMenu(menu: Menu?): Boolean {

        val m = menu?: return super.onPrepareOptionsMenu(menu)
        val mnuClear = m.findItem(R.id.menu_item_all_delete)

        this.menu = m

        mnuClear.setEnabled(0 < mNumOfArchive)

        val mnuSave = m.findItem(R.id.menu_item_save)
        mnuSave.setEnabled(!mContainCurRoute && mCurRouteScript != "")

        return super.onPrepareOptionsMenu(menu)
    }

    /**
     *  Menu選択処理(保存/削除)
     */
    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        val id = item.itemId
        when (id) {
            android.R.id.home -> {
                // return to back view
                finish()
                return true
            }
            R.id.menu_item_save -> {
                //保存はRecyclerAdapterクラスに移譲する
                val trv = archive_route_list.adapter
                        as ArchiveRouteListRecyclerViewAdapter
                trv.saveParams(this)
                item.setEnabled(false)
            }
            R.id.menu_item_all_delete -> {
                // 全削除は訊いてからやる
                AlertDialog.Builder(this).apply {
                    setTitle(R.string.main_alert_query_all_clear_title)
                    setMessage(R.string.main_alert_query_all_clear_mesg)
                    setPositiveButton("Yes") { _, _ ->
                        val trv = archive_route_list.adapter
                                as ArchiveRouteListRecyclerViewAdapter
                        trv.clearContents()
                        item.setEnabled(false)
                    }
                    setNegativeButton("No", null)
                    create()
                    show()
                }
            }
        }
        return super.onOptionsItemSelected(item)
    }

    /**
     *  行を選択した場合、選択経路を表示する。既に別の経路が表示されていてその経路が保存されて
     *  いなければ警告し確認させる
     *
     *  @param context コンテキスト
     *  @param routeScript 選択した経路
     */
    override fun onClickRow(context: Context, routeScript: String) {

        if ((mCurRouteScript != "") && (routeScript != mCurRouteScript) && !mContainCurRoute) {
            // 経路が破棄されて更新されるが良いの？と。
            AlertDialog.Builder(context).apply {
                setTitle(R.string.main_alert_query_route_update_title)
                setMessage(R.string.main_alert_query_route_update_mesg)
                setPositiveButton("Yes") { _, _ ->
                    doChangeRoute(routeScript)
                }
                setNegativeButton("No") {_, _ ->

                }
                setCancelable(false)
                create()
                show()
            }
        } else {
            // メイン表示経路はなし or 保存済み
            doChangeRoute(routeScript)
        }
    }

    /**
     *  メイン画面表示経路を選択した経路に置き換え
     *  @param routeScript 経路
     */
    private fun doChangeRoute(routeScript: String) {
        // 選択した経路が保存済みで先頭にない場合は先頭に移動して保存し直す
        var listItems = readParams(this, KEY_ARCHIVE)
        if (listItems.contains(routeScript)) {
            val s = listItems[0]
            if (routeScript != s) {
                var wl = listItems.toMutableList()
                wl.remove(routeScript)
                wl.add(0, routeScript)
                saveParam(this, ArchiveRouteActivity.KEY_ARCHIVE, wl.toList())
            }
        }

        // メイン画面へ遷移する（選択下経路を表示する）
        val intent = Intent(this, MainActivity::class.java).apply {
            putExtra("mode", "archive")
            putExtra("script", routeScript)
            addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP or Intent.FLAG_ACTIVITY_SINGLE_TOP)
        }
        finish()
        startActivity(intent)
    }

    /**
     * アイテム(保存経路)が削除されたり、保存された場合に呼ばれる
     * @param numItem 行Index
     */
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

    /**
     * static object
     * 保存パラメータキー定義
     */
    companion object {
        val KEY_ARCHIVE = "archive_route"
    }
}


////////////////////////////////////////////////////////////////

/**
 *  経路保存リストビュー管理クラス
 *  @param values  保存経路リスト
 *  @param curRouteScript 現在表示している経路
 *  @paramm isSaved curRouteScriptは保存済みか
 *  @param listener 親クラス(経路保存画面Activity)
 */
private class ArchiveRouteListRecyclerViewAdapter(private var values: List<String>,
                                                  private val curRouteScript : String,
                                                  private var saveFlag : Boolean,
                                                  private val listener : ArchiveRouteListRecyclerViewAdapter.ClickListener) :
        RecyclerView.Adapter<ArchiveRouteListRecyclerViewAdapter.ViewHolder>() {

    private val onClickListener: View.OnClickListener
    private var context : Context? = null

    /**
     *  初期化では行選択したイベントメソッドを親クラス側に移譲する準備
     */
    init {
        onClickListener = View.OnClickListener { v ->
            listener.onClickRow(context!!, v.id_route.text.toString())
        }
    }

    /**
     *  ビューをセットアップ
     */
    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
        val view = LayoutInflater.from(parent.context)
                .inflate(R.layout.content_archive_route_list, parent, false)
        if (viewType == 1) {
            with(view.id_route) {
                setTextColor(Color.parseColor("red"))
                typeface = Typeface.DEFAULT_BOLD
            }
        } else if (viewType == 2) {
            with(view.id_route) {
                setTextColor(Color.parseColor("gray"))
                typeface = Typeface.DEFAULT_BOLD
            }
        }
        this.context = parent.context
        return ViewHolder(view)
    }

    /**
     *  ビューに値（経路）を設定
     */
    override fun onBindViewHolder(holder: ViewHolder, position: Int) {
        val item = values[position]

        with(holder.itemView) {
            holder.idView.text = item
            setOnClickListener(onClickListener)
        }
    }

    /**
     *  リスト数
     */
    override fun getItemCount() = values.size

    /**
     *  経路が現在の経路で保存済みなら2、保存されていなければ1、それ以外は0
     */
    override fun getItemViewType(position: Int): Int {
        if (values[position] == curRouteScript) {
            if (!saveFlag) {
                return 1
            } else {
                return 2
            }
        } else {
            return 0
        }
    }

    /**
     *  経路を削除
     */
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

    /**
     *  経路を全削除
     */
    fun clearContents() {
        values = listOf()
        context?.let {
            saveParam(it, ArchiveRouteActivity.KEY_ARCHIVE, values)
            saveFlag = true
        }
        listener.onChangeItem(if (curRouteScript != "") itemCount - 1 else itemCount)
        notifyDataSetChanged()
    }

    /**
     *  経路を保存
     */
    fun saveParams(context : Context) {
        saveParam(context, ArchiveRouteActivity.KEY_ARCHIVE, values)
        saveFlag = true
        notifyDataSetChanged()
    }

    /**
     *  View
     */
    inner class ViewHolder(view: View) : RecyclerView.ViewHolder(view) {
        val idView: TextView = view.id_route
    }

    /**
     *  I/F定義
     */
    interface ClickListener {
        fun onClickRow(context: Context, routeScript: String)
        fun onChangeItem(numItem: Int)
    }
}
