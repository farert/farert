package org.sutezo.farert

import android.app.AlertDialog
import android.app.VoiceInteractor
import android.os.Bundle
import android.os.PersistableBundle
import android.support.v7.app.AppCompatActivity
import android.view.Menu
import android.view.MenuItem
import android.view.View
import kotlinx.android.synthetic.main.activity_result_view.*
import org.sutezo.alps.*
import android.support.v4.app.ShareCompat
import android.widget.Toast


class ResultViewActivity : AppCompatActivity() {

    var mCalcRoute : CalcRoute? = null
    var mIsRoundTrip : Boolean = true

    enum class Option {
        N_A,
        FALSE,
        TRUE,
        DO_TRUE,
        DO_FALSE,
    }
    var opt_sperule : Option = Option.N_A       // TRUEは非適用、FALSEは特例適用
    var opt_osakakan : Option = Option.N_A      // TRUEは遠回り、FALSEは近回り
    var opt_meihancity : Option = Option.N_A    // TRUEは着駅を単駅指定、FALSEは発駅を単駅指定
    var opt_stocktokai : Option = Option.N_A    // TRUEはJR東海株主優待券を使用する、FALSEは使用しない
    //var opt_neerest : Option = Option.N_A       // TRUEは適用、FALSEは非適用

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_result_view)

        setTitle(R.string.title_main_menu_result)

        // back arrow button(戻るボタン有効)
        supportActionBar?.setDisplayHomeAsUpEnabled(true)

        val opts = Option.values()

        if (savedInstanceState != null) {
            opt_stocktokai = opts[savedInstanceState.getInt("stocktokai")]
            opt_osakakan = opts[savedInstanceState.getInt("osakakan")]
            //opt_neerest = opts[savedInstanceState.getInt("neerest")]
            opt_sperule = opts[savedInstanceState.getInt("sperule")]
            opt_meihancity = opts[savedInstanceState.getInt("meihancity")]
        } else {
            opt_stocktokai =  opts[intent.getIntExtra("stocktokai", Option.N_A.ordinal)]
            opt_osakakan = opts[intent.getIntExtra("osakakan", Option.N_A.ordinal)]
            //opt_neerest = opts[intent.getIntExtra("neerest", Option.N_A.ordinal)]
            opt_sperule = opts[intent.getIntExtra("sperule", Option.N_A.ordinal)]
            opt_meihancity = opts[intent.getIntExtra("meihancity", Option.N_A.ordinal)]
        }
        val routeEndIndex = intent.getIntExtra("arrive", -1)
        val ds = Route()
        ds.assign((application as FarertApp).ds, routeEndIndex)
        setRouteChange(ds)
        mIsRoundTrip = ds.isRoundTrip
        mCalcRoute = CalcRoute(ds).also {
            it.calcFareInfo()   // apply lastFlag.rule_en
            setRouteOption(it)  // setFareOption()
        }
    }

    override fun onResume() {
        super.onResume()

        mCalcRoute?.let {
            val fi = it.calcFareInfo()
            setContentData(fi)
            setOptionFlag(fi)   // getFareOption() -> this member attributes(opt_xxx)
        }
    }

    override fun onSaveInstanceState(outState: Bundle?, outPersistentState: PersistableBundle?) {
        super.onSaveInstanceState(outState, outPersistentState)

        outState?.putInt("stocktokai", opt_stocktokai.ordinal)
        outState?.putInt("osakakan", opt_osakakan.ordinal)
        //outState?.putInt("neerest", opt_neerest.ordinal)
        outState?.putInt("sperule", opt_sperule.ordinal)
        outState?.putInt("meihancity", opt_meihancity.ordinal)
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        val id = item.itemId
        when (id) {
            android.R.id.home -> {
              finish()
              return true
            }
            R.id.menu_specialrule -> {
                // 特例
                if (opt_sperule != Option.N_A) {
                    opt_sperule = if (item.title.contains(resources.getString(R.string.nothing))) Option.DO_TRUE else Option.DO_FALSE
                    // @"特例を適用しない";
                    // @"特例を適用する"
                    refresh()
                }
            }
            R.id.menu_meihancity -> {
                if (opt_meihancity != Option.N_A) {
                    //                                                                  result_menu_meihancity_arrive: 着駅を単駅指定
                    opt_meihancity = if (item.title.equals(resources.getString(R.string.result_menu_meihancity_arrive))) Option.DO_TRUE else Option.DO_FALSE
                    // "発駅を単駅指定";
                    // "着駅を単駅指定";
                    refresh()
                }
            }
/*
 *          R.id.menu_neerest -> {
 *              if (opt_neerest != Option.N_A) {
 *                  opt_neerest = Option.DO_TRUE
 *                  refresh()
 *              }
 *          }
 */
            R.id.menu_osakakan -> {
                if (Option.N_A != opt_osakakan) {
                    opt_osakakan = if (item.title.equals(resources.getString(R.string.result_menu_osakakan_far))) Option.DO_TRUE else Option.DO_FALSE
                    refresh()
                }
            }
            R.id.menu_stocktokai -> {
                if (Option.N_A != opt_stocktokai) {
                    opt_stocktokai = if (item.title.contains(resources.getString(R.string.action))) Option.DO_TRUE else Option.DO_FALSE
                    // @"JR東海株主優待券を適用しない";
                    // @"JR東海株主優待券を使用する";
                    refresh()
                }
            }
            R.id.menu_share -> {
                mCalcRoute?.let {
                    val fi = it.calcFareInfo()
                    val subject = resultSubject(fi)
                    val text = resultText(subject, it, fi)
                    shareText(subject, text)
                } ?: run {
                    Toast.makeText(this, R.string.error, Toast.LENGTH_LONG).show()
                }
            }
            else -> {

            }
        }
        return super.onOptionsItemSelected(item)
    }

    private fun resultSubject(fi : FareInfo) : String {
       // "運賃詳細(\(cRouteUtil.terminalName(self.fareInfo.beginStationId)!) - \(cRouteUtil.terminalName(self.fareInfo.endStationId)!))"
        return "${resources.getString(R.string.result_mailtitle_faredetail)}(${terminalName(fi.beginStationId)} - ${terminalName(fi.endStationId)})"
    }

    // Result for Export text.
    //
    private fun resultText(subject : String, ds : CalcRoute, fi: FareInfo) : String {

        var body = "$subject\n\n${ds.showFare()}\n"

        body = body.replace('\\','¥')

        // メールビュー生成
        //_mailViewCtl = [[MFMailComposeViewController alloc] init];

        if (fi.isRuleAppliedEnable()) {
            if (fi.isRuleApplied()) {
                body += "(特例適用)\n"
            } else {
                body += "(特例未適用)\n"
            }
        }
        body += "\n[指定経路]\n"
        body += ds.route_script()

        return body
    }

    override fun onCreateOptionsMenu(menu: Menu?): Boolean {
        menuInflater.inflate(R.menu.menu_result_view, menu)
        return super.onCreateOptionsMenu(menu)
    }

    override fun onPrepareOptionsMenu(menu: Menu?): Boolean {

        val m = menu?: return super.onPrepareOptionsMenu(menu)

        mCalcRoute?.let {
            menuControl(m)
        }
        return super.onPrepareOptionsMenu(menu)
    }

    private fun setContentData(fi : FareInfo) {
        if (fi.result != 0) {
            /* wrong result */
            text_title_err.visibility = View.VISIBLE
            result_scroll_view.visibility = View.INVISIBLE
            when (fi.result) {
                1 -> {
                    // incomplete (小倉、西小倉)
                    text_title_err.text = resources.getString(R.string.result_err_1)
                }
//                2-> {
//                    // company line only(会社線のみ)
//                    text_title_err.text = resources.getString(R.string.result_err_2)
//                }
                else -> {
                    // Cant' error (empty or failure)
                    text_title_err.text = resources.getString(R.string.result_err_other)

                }
            }
            return
        }
        // section 区間
        text_section.text = resources.getString(R.string.result_value_section,
                terminalName(fi.beginStationId), terminalName(fi.endStationId))
        text_subtitle.text = resources.getString(R.string.result_value_section,
                terminalName(fi.beginStationId), terminalName(fi.endStationId))

        //------------------------- キロ程 -----------------------------------
        // kiro
        text_saleskm.text = resources.getString(R.string.result_km, kmNumStr(fi.totalSalesKm))
        if (fi.jrCalcKm == fi.jrSalesKm) {
            text_title_calckm.visibility = View.INVISIBLE
        } else {
            text_title_calckm.text = resources.getString(
                    if (fi.companySalesKm != 0) R.string.result_calckm else R.string.result_calckm_wo_jr)
            text_calckm.text = resources.getString(R.string.result_km, kmNumStr(fi.jrCalcKm))
        }

        // JR北海道 km
        if (fi.salesKmForHokkaido != 0) {
            text_jrgroup_km1.text = resources.getString(R.string.result_jrcompany_hokaido)
            text_jrgroup_saleskm1.text = resources.getString(R.string.result_km, kmNumStr(fi.salesKmForHokkaido))
            if (fi.salesKmForHokkaido != fi.calcKmForHokkaido) {
                text_jrgroup_calckm1.text = resources.getString(R.string.result_km, kmNumStr(fi.calcKmForHokkaido))
            }
        } else {
            val row = row_km1
            result_table.removeView(row)
        }

        // JR九州 km
        if (fi.salesKmForKyusyu != 0) {
            text_jrgroup_km2.text = resources.getString(R.string.result_jrcompany_kyusyu)
            text_jrgroup_saleskm2.text = resources.getString(R.string.result_km, kmNumStr(fi.salesKmForKyusyu))
            if (fi.salesKmForKyusyu != fi.calcKmForKyusyu) {
                text_jrgroup_calckm2.text = resources.getString(R.string.result_km, kmNumStr(fi.salesKmForKyusyu))
            }
        } else {
            val row = row_km2
            result_table.removeView(row)
        }

        // JR四国 km
        if (fi.salesKmForShikoku != 0) {
            text_jrgroup_km3.text = resources.getString(R.string.result_jrcompany_shikoku)
            text_jrgroup_saleskm3.text = resources.getString(R.string.result_km, kmNumStr(fi.salesKmForShikoku))
            if (fi.salesKmForShikoku != fi.calcKmForShikoku) {
                text_jrgroup_calckm3.text = resources.getString(R.string.result_km, kmNumStr(fi.calcKmForShikoku))
            }
        } else {
            val row = row_km3
            result_table.removeView(row)
        }

        // 114 km
        if (fi.isRule114Applied) {
            text_rule114_salesKm.text = resources.getString(R.string.result_km, kmNumStr(fi.rule114_salesKm))
            if (fi.jrCalcKm != fi.jrSalesKm) {
                text_rule114_calckm.text = resources.getString(R.string.result_km, kmNumStr(fi.rule114_calcKm))
            }
        } else {
            val row = row_114rule
            result_table.removeView(row)
        }

        // KM 会社線
        if ((fi.brtSalesKm != 0) || (fi.companySalesKm != 0)) {
            if ((fi.brtSalesKm != 0) && (fi.companySalesKm != 0)) {
                // 会社線+BRT
                // Change title JR線->JR線+BRT線
                text_jrline_km.text = resources.getString(R.string.result_km, kmNumStr(fi.jrSalesKm))
                text_company_km.text = resources.getString(R.string.result_km, kmNumStr(fi.companySalesKm))
                text_brtline_km.text = resources.getString(R.string.result_km, kmNumStr(fi.brtSalesKm))
            } else if (fi.companySalesKm != 0) {
                // 会社線のみ
                text_jrline_km.text = resources.getString(R.string.result_km, kmNumStr(fi.jrSalesKm))
                text_company_km.text = resources.getString(R.string.result_km, kmNumStr(fi.companySalesKm))
                val row_brt = row_brtkm
                result_table.removeView(row_brt)   // BRT km
            } else {
                // BRTのみ
                text_jrline_km.text = resources.getString(R.string.result_km, kmNumStr(fi.jrSalesKm))
                text_title_company_km.text = resources.getString(R.string.result_inbrtline)
                val row_brt = row_brtkm
                result_table.removeView(row_brt)   // BRT km
                text_company_km.text = resources.getString(R.string.result_km, kmNumStr(fi.brtSalesKm))
            }
        } else {
            val row = row_jr_or_company_line_km
            result_table.removeView(row)
            val row_brt = row_brtkm
            result_table.removeView(row_brt)   // BRT km
        }

        //------------------------- 運賃 -----------------------------------

        if (fi.isSpecificFare) {
            text_title_fare.text = getText(R.string.result_specific_fare)
        } else {
            text_title_fare.text = getText(R.string.result_fare)
        }
        /* 1行目 普通＋会社 or 普通 + IC */
        /* 2行目 (往復）同上 */
        var strFareOpt = resources.getString(if (fi.isRoundtripDiscount) R.string.result_value_discount else R.string.blank)

        if (fi.isRule114Applied) strFareOpt += "(${fareNumStr(fi.roundTripFareWithCompanyLinePriorRule114)})"

        if (fi.fareForCompanyline != 0) {
            /* 1: 普通運賃 + 会社線 */
            if (fi.fareForBRT == 0) {
                text_normalfare.text = resources.getString(R.string.result_yen, fareNumStr(fi.fare))
                text_title_company_fare.text = resources.getString(R.string.result_companyfare)
                text_company_fare.text = resources.getString(R.string.result_yen, fareNumStr(fi.fareForCompanyline))

                val row_brt = row_brtyen
                result_table.removeView(row_brtyen)   // BRT fare

            } else {
                // 普通運賃,BRT運賃 + 会社線
                text_normalfare.text = resources.getString(R.string.result_yen, fareNumStr(fi.fare))
                text_title_company_fare.text = resources.getString(R.string.result_companyfare)
                text_company_fare.text = resources.getString(R.string.result_yen, fareNumStr(fi.fareForCompanyline))
                text_brtline_fare.text = resources.getString(R.string.result_yen, fareNumStr(fi.fareForBRT))
            }
            /* 2: 往復運賃(割引可否) ＋ 会社線往復 */
            val s = resources.getString(R.string.result_yen, fareNumStr(fi.roundTripFareWithCompanyLine)) + strFareOpt
            text_round.text = s
            text_round2.text = resources.getString(R.string.result_yen, fareNumStr(fi.fareForCompanyline * 2))
            // BRT往復までイラン!
        } else if (fi.fareForBRT != 0) {
            // 1: 普通運賃 + BRT運賃
            text_normalfare.text = resources.getString(R.string.result_yen, fareNumStr(fi.fare))
            text_title_company_fare.text = resources.getString(R.string.result_inbrtline)
            text_company_fare.text = resources.getString(R.string.result_yen, fareNumStr(fi.fareForBRT))

            val row_brt = row_brtyen
            result_table.removeView(row_brtyen)   // BRT fare

            /* 2: 往復運賃(割引可否) ＋ BRT往復 */
            val s = resources.getString(R.string.result_yen, fareNumStr(fi.roundTripFareWithCompanyLine)) + strFareOpt
            text_round.text = s
            text_round2.text = resources.getString(R.string.result_yen, fareNumStr(fi.fareForBRT * 2))

        } else if (fi.fareForIC != 0) {
            /* 1: 普通運賃 ＋ IC運賃 */
            text_normalfare.text = resources.getString(R.string.result_yen, fareNumStr(fi.fare))
            text_title_company_fare.text = resources.getString(R.string.result_icfare)
            text_company_fare.text = resources.getString(R.string.result_yen, fareNumStr(fi.fareForIC))

            val row_brt = row_brtyen
            result_table.removeView(row_brtyen)   // BRT fare

            /* 2: 往復運賃 ＋ IC往復運賃 (割引無し) */
            val s = resources.getString(R.string.result_yen, fareNumStr(fi.roundTripFareWithCompanyLine)) + strFareOpt
            text_round.text = s
            text_round2.text = resources.getString(R.string.result_yen, fareNumStr(fi.fareForIC * 2))

        } else {
            /* 1: 普通運賃 */
            text_normalfare.text = resources.getString(R.string.result_yen, fareNumStr(fi.fare))
            text_title_company_fare.text = resources.getString(R.string.blank)
            text_company_fare.text = resources.getString(R.string.blank)

            val row_brt = row_brtyen
            result_table.removeView(row_brtyen)   // BRT fare

            /* 2: 往復運賃(割引可否) */
            val s = resources.getString(R.string.result_yen, fareNumStr(fi.roundTripFareWithCompanyLine)) + strFareOpt
            text_round.text = s
            text_round2.text = resources.getString(R.string.blank)
        }

        if (!mIsRoundTrip) {
            val row = row_round_trip
            result_table.removeView(row)
        }

        /* 114 exception */
        if (fi.isRule114Applied) {
            text_title_rule114_fare.text = resources.getString(R.string.result_no_rule114_title_fare)
            val s = "(${resources.getString(R.string.result_yen, fareNumStr(fi.farePriorRule114))})"
            text_rule114_fare.text = s
        } else {
            val row = row_114rule_fare
            result_table.removeView(row)
        }

        /* stock discount 株主優待 */
        var sn : Int = 0
        fi.fareForStockDiscounts?.let {
            if (it.isNotEmpty()) {
                sn++
                text_title_stock_discount1.text = it[0].first
                var s : String
                if (fi.isRule114Applied) {
                    s = "${resources.getString(R.string.result_yen,
                            fareNumStr(it[0].third))}(${resources.getString(
                            R.string.result_yen, fareNumStr(it[0].second))})"
                } else {
                    s = resources.getString(R.string.result_yen, fareNumStr(it[0].second))
                }
                text_stock_discount1.text = s

                if (2 <= it.size) {
                    sn++
                    text_title_stock_discount2.text = it[1].first
                    if (fi.isRule114Applied) {
                        s = "${resources.getString(R.string.result_yen, fareNumStr(it[1].third))}" +
                            "(${resources.getString(R.string.result_yen, fareNumStr(it[1].second))})"
                    } else {
                        s = resources.getString(R.string.result_yen, fareNumStr(it[1].second))
                    }
                    text_stock_discount2.text = s
                }
            }
        } ?: run {
            // null
        }
        // remove 株主優待行
        if (sn < 2) {
            val row = row_stock_option2
            result_table.removeView(row)
            if (sn < 1) {
                val rrow = row_stock_option1
                result_table.removeView(rrow)
            }
        }

        // Child fare ガキ
        text_children_fare.text = resources.getString(R.string.result_yen, fareNumStr(fi.childFare))
        if (mIsRoundTrip) {
            text_children_fare_round.text = resources.getString(R.string.result_yen, fareNumStr(fi.childFare * 2))
        } else {
            text_title_children_fare_round.visibility = View.INVISIBLE
            text_children_fare_round.visibility = View.INVISIBLE
        }

        // 学割
        if (fi.isAcademicFare) {
            text_student_fare.text = resources.getString(R.string.result_yen, fareNumStr(fi.academicFare))
            if (mIsRoundTrip) {
                text_student_fare_round.text = resources.getString(R.string.result_yen, fareNumStr(fi.roundtripAcademicFare))
            } else {
                text_title_student_fare_round.visibility = View.INVISIBLE
                text_student_fare_round.visibility = View.INVISIBLE
            }
        } else {
            val row = row_student_discount
            result_table.removeView(row)
        }

        // 備考
        var contentsForMessage : String = ""
        if (fi.isResultCompanyBeginEnd) {
            contentsForMessage += resources.getString(R.string.result_note_company)
        }
        if (fi.isResultCompanyMultipassed) {
            /* 2017.3 以降 ここに来ることはない */
            //contentsForMessage += "複数の会社線を跨っているため乗車券は通し発券できません. 運賃額も異なります.\n"
            contentsForMessage += resources.getString(R.string.result_note_multicompany)

        }
        if (fi.isEnableTokaiStockSelect) {
            //contentsForMessage += "JR東海株主優待券使用オプション選択可\n"
            contentsForMessage += resources.getString(R.string.result_note_jrtokaistock)
        }
        if (fi.isBRT_discount) {
            contentsForMessage += resources.getString(R.string.result_brt_discount)
        }
        text_note.text = contentsForMessage

        // 有効日数
        /* avail days */
        if (!fi.isArbanArea) {
            text_availdays.text = resources.getString(R.string.result_availdays_fmt, fi.ticketAvailDays)
            if (1 < fi.ticketAvailDays) {
                var str : String
                if (fi.isBeginInCity && fi.isEndInCity) {
                    str = resources.getString(R.string.result_availday_except_begin_end)
                } else if (fi.isBeginInCity) {
                    str = resources.getString(R.string.result_availday_except_begin)
                } else if (fi.isEndInCity) {
                    str = resources.getString(R.string.result_availday_except_end)
                } else {
                    str = resources.getString(R.string.result_availday_empty)
                }
                text_availdays2.text = resources.getString(R.string.result_availday_stopover, str)
            } else {
                text_availdays2.text = resources.getString(R.string.result_availday_dontstopover)
            }
        } else {
            if (fi.beginStationId != fi.endStationId) {
                text_availdays.text = resources.getString(
                        if (fi.isRuleApplied()) R.string.result_days_metro1
                        else R.string.result_days_metro2)
            } else {
                text_availdays.text = resources.getString(R.string.result_days_metro_return)
            }
            text_availdays2.visibility = View.INVISIBLE
        }

        /* ROUTE 経由 */
        var s = fi.routeList
        if (!fi.routeListForTOICA.isNullOrEmpty()) {
            s += "\n${resources.getString(R.string.result_toica_calc_route)}\n${fi.routeListForTOICA}"
        }
        s += "\n\n"
        text_via.text = s

    }

    // create first
    private fun setRouteChange(ds: Route) {

        /*
        if (opt_neerest == Option.DO_TRUE) {
            val end_id  = ds.endStationId()
            val begin_id = ds.startStationId()
            if (begin_id == end_id) {
                AlertDialog.Builder(this).apply {
                    setTitle(R.string.query)
                    // 開始駅=終了駅では最短経路は算出しません.
                    setMessage(R.string.result_err_sameautoroute)
                    setPositiveButton("Yes") { _, _ ->
                    }
                    create()
                    show()
                }
            } else {
                // 在来線のみ
                ds.changeNeerest(false, begin_id)
                opt_neerest = Option.FALSE
                opt_sperule = Option.N_A
                opt_osakakan = Option.N_A
                opt_meihancity = Option.N_A
                opt_stocktokai = Option.N_A
            }
            return
        }
        */

        if (opt_osakakan == Option.DO_TRUE || opt_osakakan == Option.DO_FALSE) {
            val rc = ds.setDetour( (opt_osakakan == Option.DO_TRUE)  )  // 遠回り・近回り
            if (0 <= rc) {
                // sucessfully
            } else {
                AlertDialog.Builder(this).apply {
                    setTitle(R.string.error)
                    // 経路が重複するため指定できません
                    setMessage(R.string.result_err_duproute)
                    setPositiveButton("Yes") { _, _ ->
                    }
                    create()
                    show()
                }
            }
        }
    }

    // create last
    private fun setRouteOption(ds : RouteList) {

        // 特例
        if (Option.DO_TRUE == opt_sperule) {
            // @"特例を適用しない";
            ds.setFareOption(RouteUtil.FAREOPT_RULE_NO_APPLIED, RouteUtil.FAREOPT_AVAIL_RULE_APPLIED)
        } else if (Option.DO_FALSE == opt_sperule) {
            // @"特例を適用する";
            ds.setFareOption(RouteUtil.FAREOPT_RULE_APPLIED, RouteUtil.FAREOPT_AVAIL_RULE_APPLIED)
        }

        // 大高-杉本町
        if (Option.DO_TRUE == opt_meihancity) {
            // "着駅を単駅指定";
            ds.setFareOption(RouteUtil.FAREOPT_APPLIED_START, RouteUtil.FAREOPT_AVAIL_APPLIED_START_TERMINAL)
        } else if (Option.DO_FALSE == opt_meihancity) {
            // "発駅を単駅指定";
            ds.setFareOption(RouteUtil.FAREOPT_APPLIED_TERMINAL, RouteUtil.FAREOPT_AVAIL_APPLIED_START_TERMINAL)
        }

        // JR東海株主
        if (Option.DO_FALSE == opt_stocktokai) {
            // @"JR東海株主優待券を適用しない";
            ds.setFareOption(RouteUtil.FAREOPT_JRTOKAI_STOCK_NO_APPLIED, RouteUtil.FAREOPT_JRTOKAI_STOCK_APPLIED)
        } else if (Option.DO_TRUE == opt_stocktokai) {
            // @"JR東海株主優待券を使用する";
            ds.setFareOption(RouteUtil.FAREOPT_JRTOKAI_STOCK_APPLIED, RouteUtil.FAREOPT_JRTOKAI_STOCK_APPLIED)
        }
    }

    // from resume last
    private fun setOptionFlag(fi: FareInfo) {

        if (fi.isRuleAppliedEnable()) {
            if (opt_sperule != Option.DO_TRUE && opt_sperule != Option.DO_FALSE) {
                if (fi.isRuleApplied()) {
                    // 適用中なら"特例を適用しない"メニューが表示
                    opt_sperule = Option.FALSE
                    if (fi.isMeihanCityStartTerminalEnable()) {
                        if (opt_meihancity != Option.DO_TRUE && opt_meihancity != Option.DO_FALSE) {
                            opt_meihancity = if (fi.isMeihanCityStart()) {
                                // 着駅を単駅指定
                                Option.TRUE
                            } else {
                                // 発駅を単駅指定
                                Option.FALSE
                            }
                        }
                    } else {
                        opt_meihancity = Option.N_A
                    }
                } else {
                    // 適用されていなければ、"特例を適用するメニューが表示される
                    opt_sperule = Option.TRUE
                    opt_meihancity = Option.N_A
                }
            }
        } else {
            opt_sperule = Option.N_A
            opt_meihancity = Option.N_A
        }

        // "最短経路算出"
/*        if (opt_neerest != Option.DO_TRUE) {
            opt_neerest = if (fi.isArbanArea) Option.FALSE else Option.N_A    // 最初は無効(メニューのチェック状態を頼る)
        }
 */
        // JR東海株主
        if (opt_stocktokai != Option.DO_TRUE && opt_stocktokai != Option.DO_FALSE) {
            opt_stocktokai =
                    if (fi.isJRCentralStockEnable()) {
                        if (fi.isJRCentralStock()) {
                            // 適用中なら"JR東海株主優待券を適用しない"がメニュー選択できる
                            Option.TRUE
                        } else {
                            // 適用されていなければ、"JR東海株主優待券を適用する"がメニュー選択できる
                            Option.FALSE
                        }
                    } else {
                        Option.N_A
                    }
        }

        //大阪環状線 周り方向
        if (opt_osakakan != Option.DO_TRUE && opt_osakakan != Option.DO_FALSE) {
            opt_osakakan =
                    if (fi.isOsakakanDetourEnable()) {
                        if (fi.isOsakakanDetourShortcut()) {
                            // "大阪環状線 遠回りだと近回り選択メニューが表示される"
                            Option.TRUE
                        } else {
                            // "大阪環状線 近回りだと遠回り選択メニューが表示される
                            Option.FALSE
                        }
                    } else {
                        Option.N_A
                    }
        }
    }


    private fun menuControl(menu: Menu?) {
        // menu　prepare open menu onPrepareOptionsMenu()
        //
        menu?.apply {
            val mi_specialrule = findItem(R.id.menu_specialrule)
            val mi_osakakan = findItem(R.id.menu_osakakan)
            val mi_meihancity = findItem(R.id.menu_meihancity)
            val mi_stocktokai = findItem(R.id.menu_stocktokai)
            //val mi_neerest = findItem(R.id.menu_neerest)

            if ((mi_specialrule == null) || (mi_osakakan == null) || (mi_meihancity == null) || (mi_stocktokai == null)/* || (mi_neerest == null)*/) {
                return
            }

            // (Negative)

            if (opt_sperule != Option.N_A) {
                if (opt_sperule == Option.FALSE || opt_sperule == Option.DO_FALSE) {
                    // 適用中なら"特例を適用しない"メニューが表示
                    mi_specialrule.title = "${resources.getString(R.string.result_menu_sperule)}${resources.getString(R.string.nothing)}"

                    if (opt_meihancity != Option.N_A) {
                        mi_meihancity.setVisible(true)
                        if (opt_meihancity == Option.TRUE || opt_meihancity == Option.DO_TRUE) { // fi.isMeihanCityStart()
                            // 発駅=都区市内
                            // "発駅を単駅指定"
                            mi_meihancity.title = resources.getString(R.string.result_menu_meihancity_start)
                        } else  {
                            // "着駅を単駅指定"
                            mi_meihancity.title = resources.getString(R.string.result_menu_meihancity_arrive)
                        }
                    } else {
                        mi_meihancity.setVisible(false)
                    }
                } else {
                    // 適用されていなければ、"特例を適用するメニューが表示される
                    mi_specialrule.title = "${resources.getString(R.string.result_menu_sperule)}${resources.getString(R.string.action)}"
                    mi_meihancity.setVisible(false)
                }
            } else {
                mi_specialrule.setVisible(false)  //  "特例を適用"
                mi_meihancity.setVisible(false)
            }

            // "最短経路算出"
/*            mi_neerest.setVisible(opt_neerest != Option.N_A)
            mi_neerest.setEnabled(opt_neerest != Option.N_A && !mi_neerest.isChecked)
 */
            // stock
            if (opt_stocktokai != Option.N_A) { // fi.isJRCentralStockEnable()) {
                val tok = resources.getString(R.string.result_menu_stocktokai)
                if (opt_stocktokai == Option.TRUE || opt_stocktokai == Option.DO_TRUE) { // fi.isJRCentralStock()) {
                    // 適用中なら"JR東海株主優待券を適用しない"がメニュー選択できる
                    mi_stocktokai.title = "$tok${resources.getString(R.string.nothing)}"
                } else {
                    // 適用されていなければ、"JR東海株主優待券を適用する"がメニュー選択できる
                    mi_stocktokai.title = "$tok${resources.getString(R.string.action)}"
                }
                mi_stocktokai.setVisible(true)  // 表示
            } else {
                mi_stocktokai.setVisible(false) // 非表示
            }

            if (opt_osakakan != Option.N_A) { // fi.isOsakakanDetourEnable()) {
                if (opt_osakakan == Option.TRUE || opt_osakakan == Option.DO_TRUE) { // fi.isOsakakanDetourShortcut()) {
                    // "大阪環状線 遠回りだと近回り選択メニューが表示される"
                    mi_osakakan.title = resources.getString(R.string.result_menu_osakakan_near)
                } else {
                    // "大阪環状線 近回りだと遠回り選択メニューが表示される
                    mi_osakakan.title = resources.getString(R.string.result_menu_osakakan_far)
                }
                mi_osakakan.setVisible(true)
            } else {
                mi_osakakan.setVisible(false)
            }
        }
    }

    private fun refresh() {
        finish()
        this.intent.putExtra("stocktokai", opt_stocktokai.ordinal)
        this.intent.putExtra("osakakan", opt_osakakan.ordinal)
        //this.intent.putExtra("neerest", opt_neerest.ordinal)
        this.intent.putExtra("sperule", opt_sperule.ordinal)
        this.intent.putExtra("meihancity", opt_meihancity.ordinal)
        startActivity(this.intent)
    }

    fun shareText(subject: String, text: String) {
        val mimeType = "text/plain"

        val shareIntent = ShareCompat.IntentBuilder.from(this)
                .setChooserTitle(resources.getString(R.string.title_share_text))
                .setType(mimeType)
                .setText(text)
                .setSubject(subject)
                .intent
        if (shareIntent.resolveActivity(packageManager) != null) {
            startActivity(shareIntent)
        }
    }
}
