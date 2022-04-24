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
    private var mbSaved : Boolean = false

    /**
     *
     */
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

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
        val listItems = readParams(this, KEY_ARCHIVE)

        // 上限チェック
        if ((MAX_ARCHIVE_ROUTE - countOfRoute(listItems)) <= 0) {

            val build = AlertDialog.Builder(this).apply {
                setTitle(R.string.title_main_menu_archive)
                setMessage(R.string.arch_title_over)
                setPositiveButton(R.string.agree) { _, _ ->
                    mCurRouteScript = ""    // 追加できない
                }
                setIcon(android.R.drawable.ic_dialog_alert)
            }
            val dlg = build.create()
            dlg.show()
        }
        setContentView(R.layout.activity_archive_route)
        setupRecyclerView(listItems)
    }

    private fun setupRecyclerView(listItems: List<String>) {
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
        val btnIconClear = ResourcesCompat.getDrawable(resources, R.drawable.ic_delete_forever_black_24dp, null)
        val btnIconSave = ResourcesCompat.getDrawable(resources, R.drawable.ic_system_update_alt_black_24dp, null)
        val btnIconExport = ResourcesCompat.getDrawable(resources, R.drawable.ic_menu_share_light, null)

        this.menu = m

        // メニューボタン(保存、クリア、エクスポート)の無効時グレーにする
        // (インポートボタンはいつでも有効なので対象外)

        val mnuClear = m.findItem(R.id.menu_item_all_delete)
        if (!mbAvailClear) {
            btnIconClear?.mutate()?.colorFilter = PorterDuffColorFilter(Color.GRAY, PorterDuff.Mode.SRC_ATOP)
            //btnIconClear?.mutate()?.setColorFilter(BlendModeColorFilter(Color.GRAY, BlendMode.SRC_ATOP))
            mnuClear.icon = btnIconClear
        }
        mnuClear.isEnabled = mbAvailClear

        val mnuSave = m.findItem(R.id.menu_item_save)
        if (!mbAvailSave) {
            btnIconSave?.mutate()?.colorFilter = PorterDuffColorFilter(Color.GRAY, PorterDuff.Mode.SRC_IN)
            //btnIconSave?.mutate()?.setColorFilter(BlendModeColorFilter(Color.GRAY, BlendMode.SRC_IN))
            mnuSave.icon = btnIconSave
        }
        mnuSave.isEnabled = mbAvailSave

        val mnuExport = m.findItem(R.id.menu_item_export)
        if (!mbAvailClear) { // same as clear button and export button
            btnIconExport?.mutate()?.colorFilter = PorterDuffColorFilter(Color.GRAY, PorterDuff.Mode.SRC_IN)
            //btnIconExport?.mutate()?.setColorFilter(BlendModeColorFilter(Color.GRAY, BlendMode.SRC_IN))
            mnuExport.icon = btnIconExport
        }
        mnuExport.isEnabled = mbAvailClear

        return super.onPrepareOptionsMenu(menu)
    }

    /**
     *  Menu選択処理(保存/削除)
     */
    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        when (item.itemId) {
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
                item.isEnabled = false
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
                        item.isEnabled = false
                    }
                    setNegativeButton("No", null)
                    setIcon(android.R.drawable.ic_dialog_alert)
                    create()
                    show()
                }
            }
            R.id.menu_item_import -> {
                // 経路文字列を複数行得る
                // Wait ProgressDialog
                val sw = readParam(this, KEY_INPORT_AVAILABLE)
                if (sw == "true") {
                    this.doImportRoute()
                    // 保存経路リスト
                    val listItems = readParams(this, KEY_ARCHIVE)

                    if (listItems.isEmpty() && mCurRouteScript == "") {
                        mbAvailClear = false
                        mbAvailSave = false
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
                        setIcon(android.R.drawable.ic_dialog_info)
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
     *  @param routeScript 選択した経路
     */
    override fun onClickRow(routeScript: String) {
        if (!mbSaved) {
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
                setIcon(context?.let { ResourcesCompat.getDrawable(it.resources, R.drawable.ic_question_answer, null) })
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
        val listItems = readParams(this, KEY_ARCHIVE)
        if (listItems.contains(routeScript)) {
            val s = listItems[0]
            if (routeScript != s) {
                val wl = listItems.toMutableList()
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
     */
    override fun onChangeItem(enClear: Boolean, enSave: Boolean) {
        mbAvailSave = enSave
        mbAvailClear = enClear
        invalidateOptionsMenu()

        mbSaved = !enSave
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

        val mimeType = "text/plain"
        val subject = "チケットフォルダ"

        val shareIntent = ShareCompat.IntentBuilder.from(this)
                .setChooserTitle(resources.getString(R.string.title_share_text))
                .setType(mimeType)
                .setText(trv.exportRoute()) // 文字列配列にはいった経路を、改行区切りの文字列として返す
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
        const val KEY_ARCHIVE = "archive_route"
        const val KEY_INPORT_AVAILABLE = "import_guide"

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
//    private var existIndex : Int = 0
                    // -3 empty and currentScript nothing
                    // -2 currentScript nothing(保存済み or メイン画面経路なし)
                    // -1 currentScript not in values(未保存)
                    // 0-N currentScript index of values
    enum class CUR_STATE {
        TOP,                // currentScript is exist top
        EXIST,
        EMPTY,              // -3 empty and currentScript nothing
        NOTHING_OR_SAVED,   // -2 currentScript nothing(保存済み or メイン画面経路なし)
        UNSAVE,             // -1 currentScript is exist in values
    }
    private var curState : CUR_STATE = CUR_STATE.EMPTY
    enum class ITEM_TYPE {
        NORMAL,
        UNSAVED,
        SAVED,
        EMPTY,
    }

    /**
     *  初期化では行選択したイベントメソッドを親クラス側に移譲する準備
     */
    init {
        if (curRouteScript == "") {
            curState = if (values.count() <= 0) {
                CUR_STATE.EMPTY     // f
            } else {
                CUR_STATE.NOTHING_OR_SAVED     // d
            }
        } else {
            val exist_idx: Int = values.indexOf(curRouteScript) // 0 or N or -1
            when {
                1 <= exist_idx -> {
                    // b
                    // メイン経路は先頭以外に保存されている
                    curState = CUR_STATE.EXIST
                    val tmp = mutableListOf(curRouteScript)
                    values.filter { it != curRouteScript }.forEach {
                        tmp.add(it)
                    }
                    values = tmp.toList()
                }
                -1 == exist_idx -> {
                    // c    メイン経路は保存されていない
                    curState = CUR_STATE.UNSAVE
                    val tmp = mutableListOf(curRouteScript)
                    tmp.addAll(values)
                    values = tmp.toList()
                }
                0 == exist_idx -> {
                    // a
                    curState = CUR_STATE.TOP
                }
            }
        }

        val enClear = if (curState == CUR_STATE.EMPTY) false else if (curState == CUR_STATE.UNSAVE) 1 < itemCount else 0 < itemCount
        listener.onChangeItem(enClear, curState == CUR_STATE.UNSAVE)

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
        } else if ((viewType == ITEM_TYPE.SAVED.ordinal)
                || (viewType == ITEM_TYPE.EMPTY.ordinal)) {
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
        val item = if (curState == CUR_STATE.EMPTY) {
            holder.itemView.context.resources.getString(R.string.no_archive_route)
        } else {
            values[position]
        }

        with(holder.itemView) {
            holder.idView.text = item
            setOnClickListener(onClickListener)
        }
    }

    /**
     *  リスト数
     */
    override fun getItemCount() = if (curState == CUR_STATE.EMPTY) 1 else values.size

    /**
     *  経路が現在の経路で保存済みなら2、保存されていなければ1、それ以外は0
     */
    override fun getItemViewType(position: Int): Int {
        if (curState == CUR_STATE.EMPTY) return ITEM_TYPE.EMPTY.ordinal

        return if (values[position] == curRouteScript) {
            if (curState == CUR_STATE.UNSAVE) {
                ITEM_TYPE.UNSAVED.ordinal
            } else {
                ITEM_TYPE.SAVED.ordinal
            }
        } else {
            0
        }
    }

    /**
     *  経路を削除
     */
    fun removeAt(position: Int) {
        if (!(values.count() <= 0 || position < 0 || values.count() <= position)) {
            val removedVal = values.filterIndexed { index, _ -> index != position }
            values = removedVal
            if (curState == CUR_STATE.TOP || curState == CUR_STATE.NOTHING_OR_SAVED) {
                // a or d
                if (position == 0) {
                    curState = CUR_STATE.NOTHING_OR_SAVED // same as 'd'
                }
                listener.onSaveRoute(values)
            } else if (curState == CUR_STATE.EXIST) {
                // b
                curState = if (position == 0) {
                    CUR_STATE.NOTHING_OR_SAVED
                } else {
                    CUR_STATE.TOP
                }
                listener.onSaveRoute(values)
            } else if (curState == CUR_STATE.UNSAVE) {
                // c
                if (position == 0) {
                    // c. 未保存で先頭を削除したらメイン経路のみ保存経路のみ破棄なので
                    //    永続データには影響なし
                    curState = CUR_STATE.NOTHING_OR_SAVED
                    // no save
                } else {
                    // c. 未保存の場合は先頭に保存候補の経路が入っているので破棄
                    val saveLists = values.drop(1)
                    listener.onSaveRoute(saveLists)
                }
            }
        }
        notifyItemRemoved(position)
        notifyItemRangeChanged(position, values.size)

        val enClear = if (curState == CUR_STATE.EMPTY) false else if (curState == CUR_STATE.UNSAVE) 1 < itemCount else 0 < itemCount
        listener.onChangeItem(enClear, curState == CUR_STATE.UNSAVE)
    }

    /**
     *  経路を全削除
     */
    fun clearContents() {
        values = listOf()
        listener.onSaveRoute(values)
        curState = CUR_STATE.EMPTY
        listener.onChangeItem(enClear = false, enSave = false)
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

        if (curState == CUR_STATE.UNSAVE) { // 未保存
            saveParam(context, ArchiveRouteActivity.KEY_ARCHIVE, values)
            curState = CUR_STATE.NOTHING_OR_SAVED     // 保存済み
        }
        val enClear = 0 < itemCount
        listener.onChangeItem(enClear, curState == CUR_STATE.UNSAVE)
        this.notifyDataSetChanged()
    }

    /**
     *  エクスポート用テキスト
     */
    fun exportRoute(): String {
        return values.joinToString("\n")
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
            val route_list = mutableListOf(*values.toTypedArray())

            for (scriptRoute: String in route_script_list) {
                if ("" == scriptRoute.trim()) {
                    continue
                }
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

            msg = when {
                new_count <= 0 -> {
                    when {
                        0 < error_count -> {
                            "追加経路はありません(すべて不正書式でした）"
                        }
                        exist_count <= 0 -> {
                            "追加経路はありません."
                        }
                        else -> {
                            "追加経路はありません(既に全経路あります)."
                        }
                    }
                }
                0 < exist_count -> {
                    "%d経路を追加しました(%d経路は既にあります.)".format(new_count, exist_count)
                }
                else -> {
                    "%d経路を追加しました".format(new_count)
                }
            }
            msg += msg_fail ?: ""
            msg += msg_trail ?: ""
        }
        // 了解
        val build = AlertDialog.Builder(context).apply {
            setTitle(R.string.menu_item_import)
            setMessage(msg)
            setIcon(if (0 < new_count) android.R.drawable.ic_dialog_info
                    else android.R.drawable.ic_dialog_alert)
            setPositiveButton(R.string.agree) { _, _ ->

                if (0 < new_count) {
                    if ((curState == CUR_STATE.UNSAVE) && (!is_top_import)) {
                        // c. 未保存の場合は先頭に保存候補の経路が入っているので破棄
                        val route_list = values.drop(1)
                        saveParam(context, ArchiveRouteActivity.KEY_ARCHIVE, route_list)
                    } else {
                        saveParam(context, ArchiveRouteActivity.KEY_ARCHIVE, values)
                    }
                    if ((curState == CUR_STATE.EXIST) || (curState == CUR_STATE.EMPTY)) {
                        // b
                        curState = CUR_STATE.NOTHING_OR_SAVED
                    } else if ((curState == CUR_STATE.UNSAVE) && is_top_import) {
                        // c
                        curState = CUR_STATE.TOP  // same as 'a'
                    }
                    val enClear = if (curState == CUR_STATE.UNSAVE) 1 < itemCount else 0 < itemCount
                    listener.onChangeItem(enClear, curState == CUR_STATE.UNSAVE)
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

