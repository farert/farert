package org.sutezo.farert

import android.annotation.SuppressLint
import android.app.Activity
import android.app.AlertDialog
import android.content.Intent
import android.os.Build
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.core.content.res.ResourcesCompat
import androidx.activity.compose.setContent
import androidx.activity.OnBackPressedCallback
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.runtime.*
import androidx.lifecycle.viewmodel.compose.viewModel
import org.sutezo.farert.ui.theme.FarertTheme
import org.sutezo.farert.ui.compose.MainScreen
import org.sutezo.farert.ui.state.ResultViewUiState
import org.sutezo.farert.ui.state.MainUiEvent
import org.sutezo.farert.ui.state.MainStateHolderProvider
import org.sutezo.alps.*


class MainActivity : ComponentActivity() {

    private lateinit var mRoute : Route
    private var mRouteScript : String = ""
    private var mDbIndex : Int = DatabaseOpenHelper.dbIndex()
    private var mStateHolder: org.sutezo.farert.ui.state.MainStateHolder? = null

    enum class OSAKA_KAN {
        DISABLE,
        FAR,
        NEAR,
    }
    private var mOsakakan_detour : OSAKA_KAN = OSAKA_KAN.DISABLE

    // Activity Result API launcher
    private val settingsLauncher = registerForActivityResult(
        ActivityResultContracts.StartActivityForResult()
    ) { result ->
        if (result.resultCode == Activity.RESULT_OK) {
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
        }
        mRouteScript = ""
    }



    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        
        setupBackPressedDispatcher()
        
        if (application is FarertApp) {
            android.util.Log.d("MainActivity", "FarertApp instance found - アプリケーション正常初期化")
            (application as? FarertApp)?.apply {
                mRoute = this.ds
                mRoute.isNotSameKokuraHakataShinZai = this.bKokuraHakataShinZaiFlag
                android.util.Log.d("MainActivity", "Route initialized from FarertApp")
            }
        } else {
            android.util.Log.w("MainActivity", "FarertApp instance NOT found - fallback to default Route")
            mRoute = Route()
        }

        setContent {
            FarertTheme {
                val stateHolder = androidx.lifecycle.viewmodel.compose.viewModel<org.sutezo.farert.ui.state.MainStateHolder>()
                mStateHolder = stateHolder
                
                // Set up callback for route changes
                stateHolder.onRouteChanged = { updatedRoute ->
                    // Update MainActivity's mRoute to keep legacy code working
                    mRoute = updatedRoute
                    
                    // Also update FarertApp's ds route for ResultView and other legacy components
                    (application as? FarertApp)?.ds?.assign(updatedRoute, -1)
                }
                
                // Initialize the state holder with the existing route
                LaunchedEffect(Unit) {
                    try {
                        stateHolder.initializeRoute(mRoute)
                    } catch (e: Exception) {
                        // Log error or handle gracefully
                        e.printStackTrace()
                    }
                }
                
                MainScreen(
                    stateHolder = stateHolder,
                    onNavigateToTerminalSelect = {
                        val intent = Intent(this@MainActivity, TerminalSelectActivity::class.java).apply {
                            putExtra("mode", "terminal")
                        }
                        startActivity(intent)
                    },
                    onNavigateToFareDetail = {
                        val intent = Intent(this@MainActivity, ResultViewActivity::class.java).apply {
                            putExtra("arrive", -1)
                            // Pass current route options to ResultView
                            putExtra("stocktokai", ResultViewUiState.Option.N_A.ordinal)
                            putExtra("neerest", ResultViewUiState.Option.N_A.ordinal) 
                            putExtra("sperule", if (mRoute.routeFlag.rule_en()) {
                                if (mRoute.routeFlag.no_rule()) ResultViewUiState.Option.TRUE.ordinal 
                                else ResultViewUiState.Option.FALSE.ordinal
                            } else ResultViewUiState.Option.N_A.ordinal)
                            putExtra("meihancity", if (mRoute.routeFlag.isMeihanCityEnable) {
                                if (mRoute.routeFlag.isStartAsCity) ResultViewUiState.Option.TRUE.ordinal 
                                else ResultViewUiState.Option.FALSE.ordinal
                            } else ResultViewUiState.Option.N_A.ordinal)
                            putExtra("longroute", if (mRoute.routeFlag.isEnableLongRoute) {
                                if (mRoute.routeFlag.isLongRoute) ResultViewUiState.Option.TRUE.ordinal 
                                else ResultViewUiState.Option.FALSE.ordinal
                            } else ResultViewUiState.Option.N_A.ordinal)
                            putExtra("rule115", if (mRoute.routeFlag.isEnableRule115) {
                                if (mRoute.routeFlag.isRule115specificTerm) ResultViewUiState.Option.TRUE.ordinal 
                                else ResultViewUiState.Option.FALSE.ordinal
                            } else ResultViewUiState.Option.N_A.ordinal)
                        }
                        startActivity(intent)
                    },
                    onNavigateToArchive = { routeScript ->
                        val intent = Intent(this@MainActivity, ArchiveRouteActivity::class.java).apply {
                            putExtra("route_script", routeScript)
                        }
                        startActivity(intent)
                    },
                    onNavigateToRouteDetail = { selectItem ->
                        onClickRow(selectItem)
                    },
                    onNavigateToSettings = { openSettings() },
                    onNavigateToVersion = { openVersion() },
                    onToggleOsakakanDetour = { toggleOsakakanDetour() }
                )
            }
        }

        // first launch check
        val rd = readParam(this, "hasLaunched")
        val num : Int = try {
            Integer.parseInt(rd)
        } catch (e: NumberFormatException) {
            -1
        }
        if ((-1 == num) || (num < 16)) {
            welcome_show()
            val cv = (application as? FarertApp)?.getVersionCode()?.toString() ?: "0"
            saveParam(this, "hasLaunched", cv)
        }
    }

    override fun onResume() {
        super.onResume()
        setTitlebar()
    }

    override fun onNewIntent(intent: Intent) {
        super.onNewIntent(intent)
        val func = intent.getStringExtra("mode") ?: ""
        val stationId = intent.getIntExtra("dest_station_id", 0)
        val lineId = intent.getIntExtra("line_id", 0)
        val newScr = intent.getStringExtra("script") ?: ""

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
                }
                if (args.isNotEmpty()) {
                    AlertDialog.Builder(this).apply {
                        setTitle(dlg_title)
                        setItems(choiceTitle.toTypedArray()) { _, which ->
                            neerest(args[which % args.size])
                        }
                        setIcon(ResourcesCompat.getDrawable(resources, R.drawable.ic_auto_route, null))
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
                        setIcon(ResourcesCompat.getDrawable(resources, R.drawable.ic_question_answer, null))
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
            }
            "archive" -> {
                // Use StateHolder to handle confirmation dialog
                mStateHolder?.handleEvent(MainUiEvent.RequestRouteChange(newScr))
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
            setIcon(android.R.drawable.ic_dialog_info)
        }
        val dlg = build.create()
        dlg.show()
    }

    // 計算結果表示
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
                setIcon(android.R.drawable.ic_dialog_alert)
            }
            val dlg = build.create()
            dlg.show()
        }

        // 大阪環状線の状態管理はStateHolderで行うため、ここでの処理は不要
        
        // StateHolderに変更を通知
        println("DEBUG: MainActivity.update_fare - mRoute.count=${mRoute.count}")
        println("DEBUG: MainActivity.update_fare - mRoute.isOsakakanDetourEnable=${mRoute.isOsakakanDetourEnable}")
        println("DEBUG: MainActivity.update_fare - mRoute.isOsakakanDetour=${mRoute.isOsakakanDetour}")
        mStateHolder?.updateRoute(mRoute)
        
        // MainActivity's mRoute should be kept in sync for legacy code compatibility
        // But we need to avoid circular dependencies by only updating when necessary
    }

    // 発駅設定
    //
    private fun beginRoute(stationId: Int) {
        mRoute.removeAll()
        mRoute.add(stationId)
        update_fare(1)
    }

    // Settings and version navigation handlers
    private fun openSettings() {
        mRouteScript = mRoute.route_script()
        mDbIndex = DatabaseOpenHelper.dbIndex()
        val intent = Intent(this@MainActivity, SettingsActivity::class.java)
        settingsLauncher.launch(intent)
    }
    
    private fun openVersion() {
        val intent = Intent(this@MainActivity, VersionActivity::class.java)
        startActivity(intent)
    }
    
    private fun toggleOsakakanDetour() {
        // Use StateHolder to handle Osaka Kanjou Line toggle
        mStateHolder?.handleEvent(MainUiEvent.ToggleOsakaKanDetour(true))
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

    private fun setupBackPressedDispatcher() {
        onBackPressedDispatcher.addCallback(this, object : OnBackPressedCallback(true) {
            override fun handleOnBackPressed() {
                // Handle back press - previously handled in onBackPressed
                finish()
            }
        })
    }



    // Main titlebar
    //  最新以外のDBを選択しているときにタイトルバーにDB名を表示する
    //
    private fun setTitlebar() {
        // Show database name on titlebar if isn't default DB.
        // This will be handled in Compose UI now
    }


    //  select row item
    //
    fun onClickRow(selectItem: Int) {
        if (0 < mRoute.count) {
            if ((mRoute.count - 1) <= selectItem) {
                // add route
                val lastIdx = mRoute.count - 1
                val intent = Intent(this@MainActivity, LineListActivity::class.java).apply {
                    putExtra("mode", "route")
                    putExtra("src_station_id", mRoute.item(lastIdx).stationId())
                    putExtra("src_line_id", mRoute.item(lastIdx).lineId())
                    putExtra("start_station_id", mRoute.item(0).stationId())
                }
                startActivity(intent)
            } else {
                // result display
                val intent = Intent(this@MainActivity, ResultViewActivity::class.java).apply {
                    putExtra("arrive", selectItem + 2)
                    // Pass current route options to ResultView
                    putExtra("stocktokai", ResultViewUiState.Option.N_A.ordinal)
                    putExtra("neerest", ResultViewUiState.Option.N_A.ordinal) 
                    putExtra("sperule", if (mRoute.routeFlag.rule_en()) {
                        if (mRoute.routeFlag.no_rule()) ResultViewUiState.Option.TRUE.ordinal 
                        else ResultViewUiState.Option.FALSE.ordinal
                    } else ResultViewUiState.Option.N_A.ordinal)
                    putExtra("meihancity", if (mRoute.routeFlag.isMeihanCityEnable) {
                        if (mRoute.routeFlag.isStartAsCity) ResultViewUiState.Option.TRUE.ordinal 
                        else ResultViewUiState.Option.FALSE.ordinal
                    } else ResultViewUiState.Option.N_A.ordinal)
                    putExtra("longroute", if (mRoute.routeFlag.isEnableLongRoute) {
                        if (mRoute.routeFlag.isLongRoute) ResultViewUiState.Option.TRUE.ordinal 
                        else ResultViewUiState.Option.FALSE.ordinal
                    } else ResultViewUiState.Option.N_A.ordinal)
                    putExtra("rule115", if (mRoute.routeFlag.isEnableRule115) {
                        if (mRoute.routeFlag.isRule115specificTerm) ResultViewUiState.Option.TRUE.ordinal 
                        else ResultViewUiState.Option.FALSE.ordinal
                    } else ResultViewUiState.Option.N_A.ordinal)
                }
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

