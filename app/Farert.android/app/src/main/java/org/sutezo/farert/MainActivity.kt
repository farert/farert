package org.sutezo.farert

import android.annotation.SuppressLint
import android.app.Activity
import android.app.AlertDialog
import android.content.Intent
import android.os.Build
import android.os.Bundle
import android.support.annotation.RequiresApi
import android.support.design.internal.BottomNavigationItemView
import android.support.design.internal.BottomNavigationMenuView
import android.support.design.widget.BottomNavigationView
import android.support.v4.view.GravityCompat
import android.support.v7.app.AppCompatActivity
import android.support.v7.widget.RecyclerView
import android.view.*
import android.widget.TextView
import kotlinx.android.synthetic.main.activity_main.*
import kotlinx.android.synthetic.main.app_bar_main.*
import kotlinx.android.synthetic.main.content_main.*
import kotlinx.android.synthetic.main.route_list.view.*
import kotlinx.android.synthetic.main.route_list_last.view.*
import org.sutezo.alps.*


class MainActivity : AppCompatActivity(), FolderViewFragment.FragmentDrawerListener, RouteRecyclerAdapter.RecyclerListener {

    private lateinit var mDrawerFragment: FolderViewFragment
    private lateinit var mRoute : Route
    private var mRouteScript : String = ""
    private var mDbIndex : Int = DatabaseOpenHelper.dbIndex()

    enum class OSAKA_KAN {
        DISABLE,
        FAR,
        NEAR,
    }
    private var mOsakakan_detour : OSAKA_KAN = OSAKA_KAN.DISABLE

    companion object {
        const val RESULT_CODE_SETTING = 3948
    }

    // 再下端のバーボタン

    private val mOnNavigationItemSelectedListener = BottomNavigationView.OnNavigationItemSelectedListener { item ->
        when (item.itemId) {
            // back to erase
            R.id.navigation_back -> {
                if (1 < mRoute.count) {
                    mRoute.removeTail()
                //    val cds = cCalcRoute(mRoute)?: {
                //        fareInfo = cds.calcFare()
                //    } else {
                //        fareInfo = FareInfo()
                //    }
                //    self.fareResultSetting(1)
                } else {
                    mRoute.removeAll()    // removeAll, clear start
                }
                update_fare(1)
            }
            // reverse route
            R.id.navigation_reverse -> {
                val rc = mRoute.reverse()
                update_fare(rc)
                recycler_view_route.smoothScrollToPosition(recycler_view_route.adapter?.itemCount ?: 1 - 1)

            }
            // 経路保存
            R.id.navigation_archive -> {
                val intent = Intent(this, ArchiveRouteActivity::class.java)
                val param = if (mRoute.count <= 1) "" else mRoute.route_script()
                intent.putExtra("route_script", param)
                startActivity(intent)
            }
        }
        false
    }


    @RequiresApi(Build.VERSION_CODES.P)
    @SuppressLint("RestrictedApi")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        toolbar.title = resources.getString(R.string.title_main_view) // Main View Title
        setSupportActionBar(toolbar)

        mDrawerFragment = supportFragmentManager.findFragmentById(R.id.fragment_navigation_drawer) as FolderViewFragment
        mDrawerFragment.init(R.id.fragment_navigation_drawer, main_drawer_layout, toolbar)

        // display
        // changeRoute()

        // 発駅選択
        buttonTerminal.setOnClickListener {
            val intent = Intent(this, TerminalSelectActivity::class.java)
            intent.putExtra("mode", "terminal")
            startActivity(intent)
        }

        // Show Detail Information of Result
        buttonFareDetail.setOnClickListener {
            if (1 < mRoute.count) {
                val intent = Intent(this, ResultViewActivity::class.java)
                intent.putExtra("arrive", -1)
                startActivity(intent)
            }
        }

        if (application is FarertApp) {
            (application as? FarertApp)?.apply {
                mRoute = this.ds
                mRoute.isNotSameKokuraHakataShinZai = this.bKokuraHakataShinZaiFlag
            }
        } else {
            mRoute = Route()
        }

        recycler_view_route.adapter = RouteRecyclerAdapter(this, mRoute)

        BottomNavigationViewHelper.disableShiftMode(bottombar)
        bottombar.setOnNavigationItemSelectedListener(mOnNavigationItemSelectedListener)

        val menuView = bottombar.getChildAt(0) as BottomNavigationMenuView
        // [back]
        (menuView.getChildAt(0) as? BottomNavigationItemView)?.apply {
            isEnabled = false
        }
        // [reverse]
        (menuView.getChildAt(1) as? BottomNavigationItemView)?.apply {
            isEnabled = false
        }
        // [大阪環状線]
        (menuView.getChildAt(2) as? BottomNavigationItemView)?.apply {
            isEnabled = false
        }
        update_fare(1)


        // first launch check

        val rd = readParam(this, "hasLaunched")
        val num : Int = try {
            Integer.parseInt(rd)
        } catch (e: NumberFormatException) {
            -1
        }
        if ((-1 == num) || (num < 16)) {
            welcome_show()
            val cv = (application as? FarertApp)?.getVersionCode() ?: "0"
            saveParam(this, "hasLaunched", cv.toString())
        }
    }

    override fun onResume() {
        super.onResume()
        main_constraint_layout.invalidate()

        setTitlebar()
    }

    override fun onNewIntent(intent: Intent?) {
        super.onNewIntent(intent)
        val func = intent?.getStringExtra("mode") ?: ""
        val stationId = intent?.getIntExtra("dest_station_id", 0)?:0
        val lineId = intent?.getIntExtra("line_id", 0) ?:0
        val newScr = intent?.getStringExtra("script") ?: ""

        when (func) {
            "autoroute" -> {
                // 履歴へ追加
                appendHistory(this, RouteUtil.StationNameEx(stationId))

                val subtitle = RouteUtil.StationNameEx(stationId)
                val dlg_title = resources.getString(R.string.title_autoroute_selection, subtitle)
                val choiceTitles = resources.getStringArray(R.array.select_autoroute_option)
                val choiceTitle = mutableListOf<String>()
                val rtTest = Route()
                rtTest.assign(mRoute)
                rtTest.changeNeerest(3, stationId)
                var args = arrayOf<Int>()
                when (rtTest.typeOfPassedLine(mRoute.count)) {
                    0 -> {
                        // 在来線のみなので、選択肢なし（新幹線も在来線も使う=3で検索）
                        // do-nothing
                    }
                    1 -> {
                        // "在来線のみ, 新幹線を使う
                        args = arrayOf(0, 1)
                        choiceTitle.addAll(listOf(choiceTitles[args[0]], choiceTitles[args[1]]))
                    }
                    2 -> {
                        // 在来線のみ、会社線も使う
                        args = arrayOf(0, 2)
                        choiceTitle.addAll(listOf(choiceTitles[args[0]], choiceTitles[args[1]]))
                    }
                    3 -> {
                        choiceTitle.addAll(choiceTitles)
                        args = arrayOf(0, 1, 2, 3)
                    }
                }
                fun neerest(mode: Int) {
                    val rc = mRoute.changeNeerest(mode, stationId)
                    update_fare(if (rc == 4) 40 else rc)
                    recycler_view_route.smoothScrollToPosition(
                       recycler_view_route.adapter?.itemCount ?: 1 - 1
                    )
                }
                if (args.isNotEmpty()) {
                    AlertDialog.Builder(this).apply {
                        setTitle(dlg_title)
                        setItems(choiceTitle.toTypedArray()) { _, which ->
                            neerest(args[which % args.size])
                        }
                        create()
                        show()
                    }
                } else {
                    neerest(3)
                }
            }
            "terminal" -> {
                // 履歴へ追加
                appendHistory(this, RouteUtil.StationNameEx(stationId))

                // Routeがあれば、消してよいか？
                if (1 < mRoute.count) {
                    AlertDialog.Builder(this).apply {
                        setTitle(R.string.main_alert_query_terminal_title)
                        setMessage(R.string.main_alert_query_terminal_mesg)
                        setPositiveButton("Yes") { _, _ ->
                            beginRoute(stationId)
                        }
                        setNegativeButton("No", null)
                        create()
                        show()
                    }
                } else {
                    // はじめて
                    beginRoute(stationId)
                }
            }
            "route" -> {
                if (lineId <= 0 || stationId <= 0) {
                    return  // safety code
                }
                val rc = mRoute.add(lineId, stationId)
                update_fare(rc)
                // scroll to last
                recycler_view_route.smoothScrollToPosition(recycler_view_route.adapter?.itemCount ?: 1 - 1)
            }
            "archive" -> {
                val rc = mRoute.setup_route(newScr)
                update_fare(rc)
                // scroll to last
                recycler_view_route.smoothScrollToPosition(recycler_view_route.adapter?.itemCount ?: 1 - 1)
            }
            else -> {
                // illegal(nothing)
            }
        }
        //Toast.makeText(this, "${func.toString()}Select menu_station_select:${RouteUtil.StationNameEx(stationId)}", Toast.LENGTH_LONG).show()
    }



    // welcome

    private fun welcome_show() {
        val build = AlertDialog.Builder(this).apply {
            setTitle(R.string.welcome)
            setMessage(R.string.eula)
            setPositiveButton(R.string.agree, null)
            setNegativeButton(R.string.disagree) { _, _ ->
                finish()
            }
        }
        val dlg = build.create()
        dlg.show()
    }

    // 計算結果表示
    @SuppressLint("RestrictedApi")
    private fun update_fare(rc : Int)
    {
        // 下部計算結果
        var msg =
                    when (rc) {
                        0 -> {
                            //"経路は終端に達しています"
                            resources.getString(R.string.main_rc_finished)
                        }
                        1 -> {
                            // 正常
                            ""
                        }
                        4 -> {
                            // 会社線利用路線はこれ以上追加できません
                            resources.getString(R.string.main_rc_company_end)
                        }
                        5 -> {
                            resources.getString(R.string.main_rc_finished)
                        }
                        40 -> {
                            resources.getString(R.string.main_rc_auto_not_enough)
                        }
                        -200 -> {
                            // 不正な駅名が含まれています.
                            resources.getString(R.string.main_rc_iregal_station)
                        }
                        -300 -> {
                            // 不正な路線名が含まれています.
                            resources.getString(R.string.main_rc_iregal_line)
                        }
                        -2 -> {
                            // 経路不正
                            resources.getString(R.string.main_rc_iregal_route)
                        }
                        -4 -> {
                            // 許可されていない会社線通過です
                            resources.getString(R.string.main_rc_wrongcompany_pass)
                        }
                        -10, -11, -1002 -> {
                            // -10, -11 経路を算出できません
                            resources.getString(R.string.main_rc_cantautostart)
                        }
                        else -> {
                            //経路が重複しているため追加できません
                            resources.getString(R.string.main_rc_duplicate_route)
                        }
                    }
        if (rc == -2 || rc == -200 || rc == -300) {
            val message = msg
            msg = ""
            // Script parse 経路エラー
            val build = AlertDialog.Builder(this).apply {
                setTitle(R.string.main_rc_alert_title)
                setMessage(resources.getString(R.string.main_rc_script_parse_error, message))
                setPositiveButton(R.string.title_version_close) { _, _ -> }
                setIcon(android.R.drawable.ic_dialog_info)
            }
            val dlg = build.create()
            dlg.show()
        }

        var revButton = false

        mOsakakan_detour = OSAKA_KAN.DISABLE
        if (1 < mRoute.count) {
            fare_value.visibility = View.VISIBLE
            saleskm_value.visibility = View.VISIBLE
            availday_value.visibility = View.VISIBLE
            val cr = CalcRoute(mRoute)
            val fi = cr.calcFareInfo()
            // 運賃(大都市近郊区間では指定した経路の計算値)
            fare_value.text = resources.getString(R.string.result_yen, fareNumStr(fi.fare))
            // 営業km
            saleskm_value.text = resources.getString(R.string.result_km, kmNumStr(fi.totalSalesKm))
            // 有効日数
            availday_value.text = resources.getString(R.string.result_availdays_fmt, fi.ticketAvailDays)
            (recycler_view_route.adapter as? RouteRecyclerAdapter)?.status_message(msg)
            // 大阪環状線
            if (mRoute.isOsakakanDetourEnable) {
                mOsakakan_detour = if (mRoute.isOsakakanDetour) {
                    OSAKA_KAN.FAR
                } else {
                    OSAKA_KAN.NEAR
                }
            }
            revButton = cr.isAvailableReverse

            footer_group.visibility = View.VISIBLE
            buttonFareDetail.isEnabled = true

            mDrawerFragment.route = mRoute
        } else {
            fare_value.visibility = View.INVISIBLE
            saleskm_value.visibility = View.INVISIBLE
            availday_value.visibility = View.INVISIBLE
            (recycler_view_route.adapter as? RouteRecyclerAdapter)?.status_message(msg)
            footer_group.visibility = View.INVISIBLE

            buttonFareDetail.isEnabled = false
            mDrawerFragment.route = null
        }

        // 先頭部 開始駅
        buttonTerminal.text = if (0 < mRoute.count) terminal_text() else resources.getString(R.string.should_select_terminal)

        // 経路リスト更新
        recycler_view_route.adapter?.notifyDataSetChanged()

        // 経路戻り（Last経路削除）の有効化／無効化
        (bottombar.getChildAt(0) as? BottomNavigationMenuView)?.apply {
            (getChildAt(0) as? BottomNavigationItemView)?.apply {
                isEnabled = 0 < mRoute.count
            }
        }
        // リバースボタンの有効化／無効化
        (bottombar.getChildAt(0) as? BottomNavigationMenuView)?.apply {
            (getChildAt(1) as? BottomNavigationItemView)?.apply {
                this.isEnabled = (1 < mRoute.count && revButton)
            }
        }

        // 経路保存
        (bottombar.getChildAt(0) as? BottomNavigationMenuView)?.apply {
            (getChildAt(2) as? BottomNavigationItemView)?.apply {
                this.isEnabled = true   // always enable
            }
        }
        val m = toolbar.menu ?: return
        val mi = m.findItem(R.id.action_osakakanrev) ?: return
        mi.isEnabled = (1 < mRoute.count && mOsakakan_detour != OSAKA_KAN.DISABLE)
    }

    // 発駅設定
    //
    private fun beginRoute(stationId: Int) {
        mRoute.removeAll()
        mRoute.add(stationId)
        update_fare(1)
    }

    // menu
    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        // Inflate the menu; this adds items to the action bar if it is present.
        menuInflater.inflate(R.menu.main, menu)
        return true
    }

    // menu didApear
    override fun onPrepareOptionsMenu(menu: Menu?): Boolean {

        // 大阪環状線
        val menu_osakakan = menu?.findItem(R.id.action_osakakanrev) ?: return true

        if (1 < mRoute.count && mRoute.isOsakakanDetourEnable) {
            if (mRoute.isOsakakanDetour) {
                // 「遠回り」になっているので「近回り」と表示する
                menu_osakakan.title = resources.getString(R.string.result_menu_osakakan_near)
            } else {
                // 「近回り」になっているので「遠回り」と表示する
                menu_osakakan.title = resources.getString(R.string.result_menu_osakakan_far)
            }
            menu_osakakan.isEnabled = true
        } else {
            menu_osakakan.isEnabled = false
        }
        return super.onPrepareOptionsMenu(menu)
    }


    // menu selected
    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        return when (item.itemId) {
            //設定
            R.id.action_settings -> {
                mRouteScript = mRoute.route_script()
                mDbIndex = DatabaseOpenHelper.dbIndex()
                val intent = Intent(this, SettingsActivity::class.java)
                startActivityForResult(intent, RESULT_CODE_SETTING)
                true
            }
            // バージョン情報
            R.id.version_view -> {
                val intent = Intent(this, VersionActivity::class.java)
                startActivity(intent)
                true
            }
            // 大阪環状線 大回り/近回り
            R.id.action_osakakanrev -> {
                if (1 < mRoute.count && mOsakakan_detour != OSAKA_KAN.DISABLE) {
                    mOsakakan_detour = if (mOsakakan_detour == OSAKA_KAN.FAR) {
                        OSAKA_KAN.NEAR
                    } else {
                        OSAKA_KAN.FAR
                    }
                    val far = (mOsakakan_detour == OSAKA_KAN.FAR)
                    //if (far) {
                    //    showInfoAsOsakaKanjyouDetour()
                    //}
                    val rc = mRoute.setDetour(far)   // True=Far, False=Neerest

                    update_fare(rc)

                    item.isEnabled = (1 < mRoute.count && mOsakakan_detour != OSAKA_KAN.DISABLE)
                }
                true
            }
            else -> super.onOptionsItemSelected(item)
        }
    }

    /*
    fun showInfoAsOsakaKanjyouDetour() {
        val key = "setting_key_hide_osakakan_detour_info"
        val r = readParam(this, key)
        if (r != "true") {
            val title = resources.getString(R.string.title_specific_calc_option_osakakan)
            val msg = resources.getString(R.string.desc_specific_calc_option, title)
            val build = AlertDialog.Builder(this).apply {
                setTitle(title)
                setMessage(msg)
                setNegativeButton(R.string.hide_specific_calc_option_info) { _, _ ->
                    saveParam(context, key, "true")
                }
                setPositiveButton(R.string.agree, null)
            }
            val dlg = build.create()
            dlg.show()
        }
    }
    */

    // <- bottun
    override fun onBackPressed() {
        if (main_drawer_layout.isDrawerOpen(GravityCompat.START)) {
            main_drawer_layout.closeDrawer(GravityCompat.START)
        } else {
            super.onBackPressed()
        }
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        // from SettingsActivity
        if (requestCode == RESULT_CODE_SETTING) {
            if (resultCode == Activity.RESULT_OK) {
                // val r = data!!.getIntExtra("test", 0)
                setTitlebar()

                var doFolderReCalculate = false
                if (mRouteScript.isNotEmpty()) {
                    // データベース変更されたら経路を再パースする
                    val rc = mRoute.setup_route(mRouteScript)
                    update_fare(rc)
                }
                if (mDbIndex != DatabaseOpenHelper.dbIndex()) {
                    doFolderReCalculate = true
                }
                mDrawerFragment.reload(doFolderReCalculate)
            }
        }
        mRouteScript = ""
    }


    // Main titlebar
    //  最新以外のDBを選択しているときにタイトルバーにDB名を表示する
    //
    private fun setTitlebar() {
        // Show database name on titlebar if isn't default DB.
        if (DatabaseOpenHelper.dbIndex() != DatabaseOpenHelper.DEFAULT_DB_IDX) {
            toolbar.title = "${resources.getString(R.string.title_main_view)}(${RouteDB.getInstance().name()}tax${RouteDB.getInstance().tax()})"
        } else {
            toolbar.title = resources.getString(R.string.title_main_view)
        }
    }

    // FolderViewFragment.FragmentDrawerListener
    override fun onDrawerItemSelected(view: View, leftRouteList: RouteList) {
        val curScr = mRoute.route_script()
        val newScr = leftRouteList.route_script()
        if (curScr != newScr) {
            if ((mRoute.count <= 1) || isStrageInRoute(this, curScr)) {
                // すぐやる
                val rc = mRoute.setup_route(newScr)
                update_fare(rc)
            } else {
                // 聞いてからやる
                AlertDialog.Builder(this).apply {
                    setTitle(R.string.main_alert_query_left_route_overwrite_title)
                    setMessage(R.string.main_alert_query_left_route_overwrite_mesg)
                    setPositiveButton("Yes") { _, _ ->
                        val rc = mRoute.setup_route(newScr)
                        update_fare(rc)
                    }
                    setNegativeButton("No", null)
                    create()
                    show()
                }
            }
        }
    }

    //  select row item
    //
    override fun onClickRow(selectItem: Int) {
        if (0 < mRoute.count) {
            if ((mRoute.count - 1) <= selectItem) {
                // add route
                val lastIdx = mRoute.count - 1
                val intent = Intent(this, LineListActivity::class.java)
                intent.putExtra("mode", "route")
                intent.putExtra("src_station_id", mRoute.item(lastIdx).stationId())
                intent.putExtra("src_line_id", mRoute.item(lastIdx).lineId())
                intent.putExtra("start_station_id", mRoute.item(0).stationId())
                startActivity(intent)
            } else {
                // result display
                val intent = Intent(this, ResultViewActivity::class.java)
                intent.putExtra("arrive", selectItem + 2)
                startActivity(intent)
            }
        }
    }
    private fun terminal_text() : String {
        return resources.getString(R.string.main_head_caption,
                RouteUtil.StationNameEx(mRoute.item(0).stationId()),
                RouteUtil.GetKanaFromStationId(mRoute.item(0).stationId()))
    }
 }

class RouteRecyclerAdapter(private val listener : RecyclerListener,
                           private val route : RouteList) : RecyclerView.Adapter<RouteRecyclerAdapter.RouteListViewHolder>() {

    private val onClickListener: View.OnClickListener = View.OnClickListener { v ->
        val selitem = v.tag as Int
        listener.onClickRow(selitem)
    }
    private var status : String = ""


    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): RouteListViewHolder {
        val view = LayoutInflater.from(parent.context).inflate(
                if (viewType == 1) R.layout.route_list_last
                else R.layout.route_list,
                parent, false)
        return RouteListViewHolder(view)
    }

    override fun onBindViewHolder(holder: RouteListViewHolder, position: Int) {
        if (position == route.count - 1) {
            // last item
            holder.status_message?.text = status
        } else {
            val current = route.item(position + 1)
            holder.line_item?.text = RouteUtil.LineName(current.lineId())
            holder.station_item?.text = RouteUtil.StationName(current.stationId())
        }
        with(holder.itemView) {
            tag = position
            setOnClickListener(onClickListener)
        }
    }

    override fun getItemCount(): Int {
        //return route.count
        return route.count - 1 + 1
    }

    override fun getItemViewType(position: Int): Int {
        // return 1 if last index
        return if (position == route.count - 1) 1 else 0
    }

    fun status_message(msg : String) {
        status = msg
        if (1 < route.count) {
            notifyItemChanged(route.count - 1)
        }
    }

    inner class RouteListViewHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {
        var line_item: TextView? = itemView.junction_line
        var station_item: TextView? = itemView.junction_station
        var status_message: TextView? = itemView.status_message
    }
    interface RecyclerListener {
        fun onClickRow(selectItem: Int)
    }
}
