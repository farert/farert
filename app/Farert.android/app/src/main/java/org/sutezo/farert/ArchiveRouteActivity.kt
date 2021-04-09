package org.sutezo.farert

import android.R.color
import android.app.AlertDialog
import android.content.ClipboardManager
import android.content.Context
import android.content.Intent
import android.graphics.*
import android.os.Bundle
import android.support.v4.app.ShareCompat
import android.support.v4.content.res.ResourcesCompat
import android.support.v7.app.AppCompatActivity
import android.support.v7.widget.RecyclerView
import android.support.v7.widget.helper.ItemTouchHelper
import android.view.*
import android.widget.TextView
import kotlinx.android.synthetic.main.activity_archive_route.*
import kotlinx.android.synthetic.main.content_archive_route_list.view.*
import kotlinx.android.synthetic.main.content_line_list.*
import kotlinx.android.synthetic.main.content_list_empty.*
import kotlinx.android.synthetic.main.route_list.*
import org.sutezo.alps.*


/**
 * 経路保存画面
 */
class ArchiveRouteActivity : AppCompatActivity(),
        ArchiveRouteListRecyclerViewAdapter.ClickListener {

    private var menu : Menu? = null
    private var mbAvailClear : Boolean = false
    private var mbAvailSave : Boolean = false
    private var mCurRouteScript : String = ""
    private var mbAvailRecyclerView : Boolean = false

    /**
     *
     */
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_archive_route)

        setTitle(R.string.title_main_menu_archive)

        // back arrow button(戻るボタン有効)
        supportActionBar?.setDisplayHomeAsUpEnabled(true)

        mCurRouteScript = ""
        (application as? FarertApp)?.ds?.let {
            if (1 < it.count) {
                mCurRouteScript = it.route_script() ?: "" // 現在表示中の経路
            }
        }

        // 保存経路リスト
        var listItems = readParams(this, KEY_ARCHIVE)

        if (listItems.isEmpty() && mCurRouteScript == "") {
            setContentView(R.layout.content_list_empty)
            list_empty.text = resources.getString(R.string.no_archive_route)
            mbAvailClear = false
            mbAvailSave = false
            mbAvailRecyclerView = false
        } else {
            // 上限チェック
            if ((MAX_ARCHIVE_ROUTE - countOfRoute(listItems)) <= 0) {

                val build = AlertDialog.Builder(this).apply {
                    setTitle(R.string.title_main_menu_archive)
                    setMessage(R.string.arch_title_over)
                    setPositiveButton(R.string.agree) { _, _ ->
                        mCurRouteScript = ""    // 追加できない
                    }
                }
                val dlg = build.create()
                dlg.show()
            } else {
                setupRecyclerView(listItems)
                mbAvailRecyclerView = true
            }
        }
    }

    fun setupRecyclerView(listItems: List<String>) {
        archive_route_list.adapter =
                ArchiveRouteListRecyclerViewAdapter(listItems,
                        mCurRouteScript,
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
        val btnIconClear = ResourcesCompat.getDrawable(getResources(), R.drawable.ic_delete_forever_black_24dp, null)
        val btnIconSave = ResourcesCompat.getDrawable(getResources(), R.drawable.ic_system_update_alt_black_24dp, null)
        val btnIconExport = ResourcesCompat.getDrawable(getResources(), R.drawable.ic_menu_share_light, null)

        this.menu = m

        // メニューボタン(保存、クリア、エクスポート)の無効時グレーにする
        // (インポートボタンはいつでも有効なので対象外)

        val mnuClear = m.findItem(R.id.menu_item_all_delete)
        if (!mbAvailClear) {
            btnIconClear?.mutate()?.colorFilter = PorterDuffColorFilter(Color.GRAY, PorterDuff.Mode.SRC_ATOP)
            //btnIconClear?.mutate()?.setColorFilter(BlendModeColorFilter(Color.GRAY, BlendMode.SRC_ATOP))
            mnuClear.setIcon(btnIconClear)
        }
        mnuClear.setEnabled(mbAvailClear)

        val mnuSave = m.findItem(R.id.menu_item_save)
        if (!mbAvailSave) {
            btnIconSave?.mutate()?.colorFilter = PorterDuffColorFilter(Color.GRAY, PorterDuff.Mode.SRC_IN)
            //btnIconSave?.mutate()?.setColorFilter(BlendModeColorFilter(Color.GRAY, BlendMode.SRC_IN))
            mnuSave.setIcon(btnIconSave)
        }
        mnuSave.setEnabled(mbAvailSave)

        val mnuExport = m.findItem(R.id.menu_item_export)
        if (!mbAvailClear) { // same as clear button and export button
            btnIconExport?.mutate()?.colorFilter = PorterDuffColorFilter(Color.GRAY, PorterDuff.Mode.SRC_IN)
            //btnIconExport?.mutate()?.setColorFilter(BlendModeColorFilter(Color.GRAY, BlendMode.SRC_IN))
            mnuExport.setIcon(btnIconExport)
        }
        mnuExport.setEnabled(mbAvailClear)

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
            R.id.menu_item_import -> {
                // 経路文字列を複数行得る
                // Wait ProgressDialog
                var sw = readParam(this, KEY_INPORT_AVAILABLE)
                if (sw == "true") {
                    if (!mbAvailRecyclerView) {
                        setContentView(R.layout.activity_archive_route)
                        setupRecyclerView(listOf<String>())
                        mbAvailRecyclerView = true
                    }
                    this.doImportRoute()
                    // 保存経路リスト
                    var listItems = readParams(this, KEY_ARCHIVE)

                    if (listItems.isEmpty() && mCurRouteScript == "") {
                        setContentView(R.layout.content_list_empty)
                        list_empty.text = resources.getString(R.string.no_archive_route)
                        mbAvailClear = false
                        mbAvailSave = false
                        mbAvailRecyclerView = false
                    }

                } else {
                    val build = AlertDialog.Builder(this).apply {
                        setTitle(R.string.menu_item_import) // "インポート"
                        // インポートする経路をテキストで「メモ」等の外部エディタで作成してください
                        // (スペースまたはカンマ','区切りで1行1経路)で指定します）
                        // その後、クリップボードにコピーしてで本機能を実行します.
                        // わからないという人は使用しないでください
                        setMessage(R.string.arch_title_import)
                        setNegativeButton(R.string.arch_hide_specific_import) { _, _ ->
                            saveParam(context, KEY_INPORT_AVAILABLE, "true")
                        }
                        setPositiveButton(R.string.agree, null) // "了解" -> なにもしない
                        // "今後、この画面は表示せず、インポート機能を実行する" -> 以降実行可能
                    }
                    val dlg = build.create()
                    dlg.show()
                }
            }
            R.id.menu_item_export -> {
                doExportRoute()
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
    override fun onClickRow(routeScript: String) {
        if ((mCurRouteScript != "") && (routeScript != mCurRouteScript)) {
            // 経路が破棄されて更新されるが良いの？と。
            AlertDialog.Builder(this).apply {
                setTitle(R.string.main_alert_query_route_update_title)
                setMessage(R.string.main_alert_query_route_update_mesg)
                setPositiveButton("Yes") { _, _ ->
                    doChangeRoute(routeScript)
                }
                setNegativeButton("No") { _, _ ->

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
                saveParam(this, KEY_ARCHIVE, wl.toList())
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
     *
     * @param numItem 行Index
     */
    override fun onChangeItem(enClear: Boolean, enSave: Boolean) {
        mbAvailSave = enSave
        mbAvailClear = enClear
        invalidateOptionsMenu()
    }

    /**
     * 経路を保存
     * @param routes 保存経路
     */
    override fun onSaveRoute(routes: List<String>) {
        saveParam(this, KEY_ARCHIVE, routes)
    }


    /**
     * インポート
     */
    private fun doImportRoute() {
        val clm = getSystemService(Context.CLIPBOARD_SERVICE) as ClipboardManager
        val cliptext = (clm.primaryClip?.getItemAt(0)?.text ?: "" ).toString()

        val trv = archive_route_list.adapter as? ArchiveRouteListRecyclerViewAdapter
        trv?.importRoute(this, cliptext)
    }

    /**
     * エクスポート
     */
    private fun doExportRoute() {
        val trv = archive_route_list.adapter as ArchiveRouteListRecyclerViewAdapter

        // 文字列配列にはいった経路を、改行区切りの文字列として返す
        val route_script_all = trv.exportRoute()

        val mimeType = "text/plain"
        val subject = "チケットフォルダ"

        val shareIntent = ShareCompat.IntentBuilder.from(this)
                .setChooserTitle(resources.getString(R.string.title_share_text))
                .setType(mimeType)
                .setText(route_script_all)
                .setSubject(subject)
                .intent
        if (shareIntent.resolveActivity(this.packageManager) != null) {
            startActivity(shareIntent)
        }
    }

    /**
     * static object
     * 保存パラメータキー定義
     */
    companion object {
        val KEY_ARCHIVE = "archive_route"
        val KEY_INPORT_AVAILABLE = "import_guide"

        val delim = arrayOf(" ", ",", ".", "[", "]", "<", ">", "/", "{", "}")

        // 経路の経路数をカウントする。
        // 東京 東海道線 大阪 なら、3を返す
        //
        fun countOfRoute(rt: List<String>) : Int {
            var c = 0
            for (r in rt) {
                val a = r.split(*delim).filter { it != "" }
                c += a.size
            }
            return c
        }

        fun countOfRoute(rt: String) : Int {
            return countOfRoute(rt.split("\n"))
        }

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
                                                  private val curRouteScript: String,
                                                  private val listener: ClickListener) :
        RecyclerView.Adapter<ArchiveRouteListRecyclerViewAdapter.ViewHolder>() {

    private val onClickListener: View.OnClickListener
    private var existIndex : Int = 0

    enum class ITEM_TYPE {
        NORMAL,
        UNSAVED,
        SAVED,
    }

    /**
     *  初期化では行選択したイベントメソッドを親クラス側に移譲する準備
     */
    init {
        if (curRouteScript == "") {
            existIndex = -2     // d
        } else {
            existIndex = values.indexOf(curRouteScript) // 0 or N or -1
            if (1 <= existIndex) {
                // b
                // メイン経路は先頭以外に保存されている
                val tmp = mutableListOf(curRouteScript)
                values.filter { it != curRouteScript }.forEach {
                    tmp.add(it)
                }
                values = tmp.toList()
            } else if (-1 == existIndex) {
                // c    メイン経路は保存されていない
                val tmp = mutableListOf(curRouteScript)
                tmp.addAll(values)
                values = tmp.toList()
            } // else if existIndex == 0 // a
        }

        val enClear = if (existIndex == -1) 1 < itemCount else 0 < itemCount
        listener.onChangeItem(enClear, existIndex == -1)

        onClickListener = View.OnClickListener { v ->
            listener.onClickRow(v.id_route.text.toString())
        }
    }

    /**
     *  ビューをセットアップ
     */
    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
        val view = LayoutInflater.from(parent.context)
                .inflate(R.layout.content_archive_route_list, parent, false)
        if (viewType == ITEM_TYPE.UNSAVED.ordinal) {
            with(view.id_route) {
                setTextColor(Color.parseColor("red"))
                typeface = Typeface.DEFAULT_BOLD
            }
        } else if (viewType == ITEM_TYPE.SAVED.ordinal) {
            with(view.id_route) {
                setTextColor(Color.parseColor("gray"))
                typeface = Typeface.DEFAULT_BOLD
            }
        }
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
            if (existIndex == -1) {
                return ITEM_TYPE.UNSAVED.ordinal
            } else {
                return ITEM_TYPE.SAVED.ordinal
            }
        } else {
            return 0
        }
    }

    /**
     *  経路を削除
     */
    fun removeAt(position: Int) {
        if (values.count() <= 0 || position < 0 || values.count() <= position) {
            return
        }
        val removedVal = values.filterIndexed { index, _ ->  index != position }
        values = removedVal
        if (existIndex == 0 || existIndex == -2) {
            // a or d
            if (position == 0) {
                existIndex = -2 // same as 'd'
            }
            listener.onSaveRoute(values)
        } else if (0 < existIndex) {
            // b
            if (position == 0) {
                existIndex = -2
            } else {
                existIndex = 0
            }
            listener.onSaveRoute(values)
        } else if (existIndex == -1) {
            // c
            if (position == 0) {
                // c. 未保存で先頭を削除したらメイン経路のみ保存経路のみ破棄なので
                //    永続データには影響なし
                existIndex = -2
                // no save
            } else {
                // c. 未保存の場合は先頭に保存候補の経路が入っているので破棄
                val saveLists = values.drop(1)
                listener.onSaveRoute(saveLists)
            }
        }

        notifyItemRemoved(position)
        notifyItemRangeChanged(position, values.size)

        val enClear = if (existIndex == -1) 1 < itemCount else 0 < itemCount
        listener.onChangeItem(enClear, existIndex == -1)
    }

    /**
     *  経路を全削除
     */
    fun clearContents() {
        values = listOf()
        listener.onSaveRoute(values)
        existIndex = -2
        listener.onChangeItem(false, false)
        notifyDataSetChanged()
    }

    /**
     *  経路を保存
     */
    fun saveParams(context: Context) {
        // 上限チェック
        if ((MAX_ARCHIVE_ROUTE - ArchiveRouteActivity.countOfRoute(values)) <= 0) {
            return
        }

        if (existIndex == -1) {
            saveParam(context, ArchiveRouteActivity.KEY_ARCHIVE, values)
            existIndex = -2
        }
        val enClear = if (existIndex == -1) 1 < itemCount else 0 < itemCount
        listener.onChangeItem(enClear, existIndex == -1)
        notifyDataSetChanged()
    }

    /**
     *  エクスポート用テキスト
     */
    fun exportRoute() : String {
        val s = values.joinToString("\n")
        return s
    }

    /**
     * インポート
     */
    fun importRoute(context: Context, text: String) {
        // 文字列をパースして配列を作成する(エラーがあったらエラーカウントする)(経路にXX箇所不正な路線・駅を検出しました）
        // 配列へマージする(既に入っているものは削除して後ろへ追加する）
        // 配列数が上限に達したら切り捨てられる(後半XX経路が切り捨てられました）
        // 文字数単位で、60✖️100=6000までで、Overしたら経路単位で切り捨てる

        var exist_count = 0
        var new_count = 0           // new add
        var error_count = 0
        var noadd_count = 0
        var is_top_import = false
        var msg : String

        // カレント経路数
        var free_archive_route = MAX_ARCHIVE_ROUTE - ArchiveRouteActivity.countOfRoute(values)

        if (free_archive_route <= 0) {
            msg = "これ以上経路は追加できません. 既存の経路をいくつか削除してください"
        } else {
            // クリップボードから経由を得る
            val route_script_list = text.split("\n")
            var route_list = mutableListOf<String>(*values.toTypedArray())

            for (scriptRoute: String in route_script_list) {
                val scr_route = scriptRoute.split(*ArchiveRouteActivity.delim).filter { it != "" }.joinToString(",")
                if (free_archive_route <= 0) {
                    noadd_count += 1
                } else {
                    val rt = Route()
                    val rc = rt.setup_route(scr_route)
                    if (rc < 0) {
                        // error
                        error_count += 1
                    } else {
                        // OK
                        val inIndex = route_list.indexOf(scr_route)
                        if (0 <= inIndex) {
                            // already exist
                            if (inIndex == 0) {
                                is_top_import = true
                            }
                            exist_count += 1
                        } else {
                            new_count += 1 // 追加数
                            // 経路数
                            free_archive_route -= ArchiveRouteActivity.countOfRoute(scr_route)
                            route_list.add(scr_route)
                        }
                    }
                }
            }
            if (0 < new_count) {
                values = route_list.toList()
            }


            var msg_trail: String? = null
            if (0 < noadd_count) {
                msg_trail = "\n上限を越えたため追加できなかった経路が%d経路あります".format(noadd_count)
            }
            var msg_fail: String? = null
            if ((0 < new_count) && (0 < error_count)) {
                msg_fail = "\n%d経路が不正書式でした.".format(error_count)
            }

            if (new_count <= 0) {
                if (0 < error_count) {
                    msg = "追加経路はありません(すべて不正書式でした）"
                } else if (exist_count <= 0) {
                    msg = "追加経路はありません."
                } else {
                    msg = "追加経路はありません(既に全経路あります)."
                }
            } else if (0 < exist_count) {
                msg = "%d経路を追加しました(%d経路は既にあります.)".format(new_count, exist_count)
            } else {
                msg = "%d経路を追加しました".format(new_count)
            }
            msg += msg_fail ?: ""
            msg += msg_trail ?: ""
        }
        // 了解
        val build = AlertDialog.Builder(context).apply {
            setTitle(R.string.menu_item_import)
            setMessage(msg)
            setPositiveButton(R.string.agree) { _, _ ->

                if (0 < new_count) {
                    if ((existIndex == -1) && (!is_top_import)) {
                        // c. 未保存の場合は先頭に保存候補の経路が入っているので破棄
                        val route_list = values.drop(1)
                        saveParam(context, ArchiveRouteActivity.KEY_ARCHIVE, route_list)
                    } else {
                        saveParam(context, ArchiveRouteActivity.KEY_ARCHIVE, values)
                    }

                    if (0 < existIndex) {
                        // b
                        existIndex = -2
                    } else if ((existIndex == -1) && is_top_import) {
                        // c
                        existIndex = 0  // same as 'a'
                    }
                    val enClear = if (existIndex == -1) 1 < itemCount else 0 < itemCount
                    listener.onChangeItem(enClear, existIndex == -1)
                    notifyDataSetChanged()
                }
            }
        }
        val dlg = build.create()
        dlg.show()
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
        fun onSaveRoute(routes: List<String>)
        fun onClickRow(routeScript: String)
        fun onChangeItem(enClear: Boolean, enSave: Boolean)
    }
}

