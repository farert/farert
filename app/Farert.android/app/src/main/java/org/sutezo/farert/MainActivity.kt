package org.sutezo.farert

import android.app.Activity
import android.app.AlertDialog
import android.content.DialogInterface
import android.content.Intent
import android.content.pm.PackageManager
import android.graphics.Color
import android.os.Bundle
import android.os.PersistableBundle
import android.support.design.internal.BottomNavigationItemView
import android.support.design.internal.BottomNavigationMenuView
import android.support.design.widget.BottomNavigationView
import android.support.v4.view.GravityCompat
import android.support.v7.app.AppCompatActivity
import android.support.v7.widget.RecyclerView
import android.view.*
import android.widget.Toast
import kotlinx.android.synthetic.main.activity_main.*
import kotlinx.android.synthetic.main.app_bar_main.*
import kotlinx.android.synthetic.main.content_main.*
import kotlinx.android.synthetic.main.route_list.view.*
import org.sutezo.alps.*
import java.lang.NumberFormatException


class MainActivity : AppCompatActivity(), FolderViewFragment.FragmentDrawerListener, RouteRecyclerAdapter.RecyclerListener {

    private lateinit var mDrawerFragment: FolderViewFragment
    private lateinit var mRoute : Route
    private var mRouteScript : String = ""

    enum class OSAKA_KAN {
        DISABLE,
        FAR,
        NEAR,
    }
    var mOsakakan_detour : OSAKA_KAN = OSAKA_KAN.DISABLE

    companion object {
        val RESULT_CODE_SETTING = 3948
    }

    private val mOnNavigationItemSelectedListener = BottomNavigationView.OnNavigationItemSelectedListener { item ->
        when (item.itemId) {
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
            R.id.navigation_reverse -> {
                val rc = mRoute.reverse()
                update_fare(rc)
            }
            R.id.navigation_detour -> {
                if (mOsakakan_detour != OSAKA_KAN.DISABLE) {
                    mOsakakan_detour = if (mOsakakan_detour == OSAKA_KAN.FAR) OSAKA_KAN.NEAR else OSAKA_KAN.FAR
                    mRoute.setDetour(mOsakakan_detour == OSAKA_KAN.FAR)
                    update_fare(1)
                }
            }
        }
        false
    }


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

        buttonFareDetail.setOnClickListener {
            if (1 < mRoute.count) {
                val intent = Intent(this, ResultViewActivity::class.java)
                intent.putExtra("arrive", -1)
                startActivity(intent)
            }
        }

        mRoute = (application as FarertApp).ds

        recycler_view_route.adapter = RouteRecyclerAdapter(this, mRoute)

        BottomNavigationViewHelper.disableShiftMode(bottombar)
        bottombar.setOnNavigationItemSelectedListener(mOnNavigationItemSelectedListener)

        val menuView = bottombar.getChildAt(0) as BottomNavigationMenuView
        (menuView.getChildAt(0) as BottomNavigationItemView).apply {
            setEnabled(false)
        }
        (menuView.getChildAt(1) as BottomNavigationItemView).apply {
            isEnabled = false
        }
        (menuView.getChildAt(2) as BottomNavigationItemView).apply {
            isEnabled = false
        }
        update_fare(1)


        // first launch check

        val rd = readParam(this, "hasLaunched")
        var num : Int
        try {
            num = Integer.parseInt(rd)
        } catch (e: NumberFormatException) {
            num = -1
        }
        if (-1 == num) {
            welcome_show()
            val cv = getVersionCode()
            saveParam(this, "hasLaunched", cv.toString())
        }

        /* database index reset */
        if (num < 5) {
            saveParam(this, "datasource", "0".toString())
        }

    }

    private fun getVersionCode(): Int {
        val pm = this.packageManager
        var versionCode = 0
        try {
            val packageInfo = pm.getPackageInfo(this.packageName, 0)
            versionCode = packageInfo.versionCode
            // versionCode:通算バージョン(数値)
            // versionName: "18.11" とか
        } catch (e: PackageManager.NameNotFoundException) {
            e.printStackTrace()
        }
        return versionCode
    }

    override fun onSaveInstanceState(outState: Bundle?, outPersistentState: PersistableBundle?) {
        super.onSaveInstanceState(outState, outPersistentState)

    }

    override fun onStart() {
        super.onStart()
        /*
        val menuView = menu_main_bottom.menu as BottomNavigationView
        for (i in 0..2) {
            val smallLabel = menuView.getChildAt(i).findViewById(android.support.design.R.id.smallLabel) as View
            val baselineLayout = smallLabel.getParent() as BaselineLayout
            baselineLayout.visibility = View.GONE
        }
        */
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

                val build = AlertDialog.Builder(this).apply {
                    setTitle(R.string.title_line_select_autoroute)
                    setMessage(R.string.main_alert_query_bullet)
                    setPositiveButton("Yes") { _, _ ->
                        // 新幹線
                        val rc = mRoute.changeNeerest(true, stationId)
                        update_fare(rc)
                    }
                    setNegativeButton("No") { _, _ ->
                        // 在来線のみ
                        val rc = mRoute.changeNeerest(false, stationId)
                        update_fare(rc)
                    }
                }
                val dlg = build.create()
                dlg.show()
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
                        show()
                    }
                } else {
                    // はじめて
                    beginRoute(stationId)
                }
            }
            "route" -> {
                val rc = mRoute.add(lineId, stationId)
                update_fare(rc)
            }
            "archive" -> {
                val rc = mRoute.setup_route(newScr)
                update_fare(rc)
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
    private fun update_fare(rc : Int)
    {
        // 下部計算結果
        val msg =
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
                            resources.getString(R.string.main_rc_neerested)
                        }
                        -200 -> {
                            // 不正な駅名が含まれています.
                            resources.getString(R.string.main_rc_iregalstation)
                        }
                        -300 -> {
                            // 不正な路線名が含まれています.
                            resources.getString(R.string.main_rc_iregalline)
                        }
                        -2 -> {
                            // 経路不正
                            resources.getString(R.string.main_rc_iregaroute)
                        }
                        -4 -> {
                            // 許可されていない会社線通過です
                            resources.getString(R.string.main_rc_wrongcompany_pass)
                        }
                        -10, -11 -> {
                            // -10, -11 経路を算出できません
                            resources.getString(R.string.main_rc_cantautostart)
                        }
                        else -> {
                            //経路が重複しているため追加できません
                            resources.getString(R.string.main_rc_duplicate_route)
                        }
                    }
        var revButton = false

        mOsakakan_detour = OSAKA_KAN.DISABLE
        if (0 <= rc && 1 < mRoute.count) {
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
            buttonFareDetail.text = ""
            // 大阪環状線
            if (fi.isOsakakanDetourEnable()) {
                if (fi.isOsakakanDetourShortcut()) {
                    mOsakakan_detour = OSAKA_KAN.NEAR
                } else {
                    mOsakakan_detour = OSAKA_KAN.FAR
                }
            }
            revButton = ((cr.fareOption and 0x400) == 0)
            footer_group.visibility = View.VISIBLE
            mDrawerFragment.route = mRoute
        } else {
            fare_value.visibility = View.INVISIBLE
            saleskm_value.visibility = View.INVISIBLE
            availday_value.visibility = View.INVISIBLE
            buttonFareDetail.text = msg
            footer_group.visibility = View.INVISIBLE
            mDrawerFragment.route = null
        }

        // 先頭部 開始駅
        buttonTerminal.text = if (0 < mRoute.count) terminal_text() else resources.getString(R.string.should_select_terminal)

        // 経路リスト更新
        recycler_view_route.adapter.notifyDataSetChanged()

        // 経路戻り（Last経路削除）の有効化／無効化
        (bottombar.getChildAt(0) as BottomNavigationMenuView).apply {
            (getChildAt(0) as BottomNavigationItemView).apply {
                setEnabled(0 < mRoute.count)
            }
        }
        // リバースボタンの有効化／無効化
        (bottombar.getChildAt(0) as BottomNavigationMenuView).apply {
            (getChildAt(1) as BottomNavigationItemView).apply {
                setEnabled(1 < mRoute.count && revButton)
            }
        }
        //大阪環状線遠回り／近回りボタン
        (bottombar.getChildAt(0) as BottomNavigationMenuView).apply {
            (getChildAt(2) as BottomNavigationItemView).apply {
                setEnabled(1 < mRoute.count && mOsakakan_detour != OSAKA_KAN.DISABLE)
            }
        }

    }

    // 発駅設定
    //
    private fun beginRoute(stationId: Int) {
        mRoute.removeAll()
        mRoute.add(stationId)
        update_fare(1)
    }

    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        // Inflate the menu; this adds items to the action bar if it is present.
        menuInflater.inflate(R.menu.main, menu)
        return true
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        return when (item.itemId) {
            //設定
            R.id.action_settings -> {
                mRouteScript = mRoute.route_script()
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
            // 経路保存
            R.id.archive_route -> {
                val intent = Intent(this, ArchiveRouteActivity::class.java)
                val param = if (mRoute.count <= 1) "" else mRoute.route_script()
                intent.putExtra("route_script", param)
                startActivity(intent)
                true
            }
            else -> super.onOptionsItemSelected(item)
        }
    }

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

                if (!mRouteScript.isEmpty()) {
                    // データベース変更されたら経路を再パースする
                    val rc = mRoute.setup_route(mRouteScript)
                    update_fare(rc)
                }
                // LeftViewも
                mDrawerFragment.reload()
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
            if ((mRoute.getCount() <= 1) || isStrageInRoute(this, curScr)) {
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
                    show()
                }
            }
        }
    }

    override fun onClickRow(selectItem: Int) {
        if (0 < mRoute.count) {
            if ((mRoute.count - 1) <= selectItem) {
                // add route
                val lastIdx = mRoute.count - 1
                val intent = Intent(this, LineListActivity::class.java)
                intent.putExtra("mode", "route")
                intent.putExtra("src_station_id", mRoute.item(lastIdx).stationId())
                intent.putExtra("src_line_id", mRoute.item(lastIdx).lineId())
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

class RouteRecyclerAdapter(private val listener : RouteRecyclerAdapter.RecyclerListener,
                           private val route : RouteList) : RecyclerView.Adapter<RouteRecyclerAdapter.MyViewHolder>() {

    private val onClickListener: View.OnClickListener

    init {
        onClickListener = View.OnClickListener { v ->
            val selitem = v.tag as Int
            listener.onClickRow(selitem)
        }
    }


    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): MyViewHolder {
        val view = LayoutInflater.from(parent.context).inflate(R.layout.route_list, parent, false)
        return MyViewHolder(view)
    }

    override fun onBindViewHolder(holder: MyViewHolder, position: Int) {
        if (position == route.count - 1) {
            // last item
            holder.line_item.text = holder.itemView.resources.getString(R.string.main_last_list_caption)
            holder.line_item.setTextColor(holder.line_item.linkTextColors)
            holder.line_item.gravity = Gravity.RIGHT
        } else {
            val current = route.item(position + 1)
            holder.line_item.text = RouteUtil.LineName(current.lineId())
            holder.station_item.text = RouteUtil.StationName(current.stationId())
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

    inner class MyViewHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {
        var line_item = itemView.junction_line
        var station_item = itemView.junction_station

        init {
        }
    }
    interface RecyclerListener {
        fun onClickRow(selectItem: Int)
    }
}
