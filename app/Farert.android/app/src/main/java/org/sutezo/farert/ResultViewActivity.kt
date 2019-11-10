package org.sutezo.farert

import android.app.AlertDialog
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

/**
 * 運賃詳細結果ビューActivivty
 */
class ResultViewActivity : AppCompatActivity() {

    var mCalcRoute : CalcRoute? = null
    var mIsRoundTrip : Boolean = true

    enum class Option {
        N_A,
        FALSE,
        TRUE,
//        DO_TRUE,
//        DO_FALSE,
    }

    enum class OptionItem {
        stocktokai,     // TRUEはJR東海株主優待券を使用する、FALSEは使用しない
        osakakan,       // TRUEは遠回り、FALSEは近回り
        neerest,        // TRUEは適用、FALSEは非適用
        sperule,        // TRUEは非適用、FALSEは特例適用
        meihancity,     // TRUEは着駅を単駅指定、FALSEは発駅を単駅指定
        longroute,      // TRUEは近郊区間遠回り
        rule115,        // TRUEは115条 都区市内発着指定
    }

    var mOpt_items = arrayOf(
        Option.N_A,
        Option.N_A,
        Option.N_A,
        Option.N_A,
        Option.N_A,
        Option.N_A,
        Option.N_A)

    val mOptMap = mutableMapOf<OptionItem, String>(
            OptionItem.stocktokai to "stocktokai",
            OptionItem.osakakan to "osakakan",
            OptionItem.neerest to "neerest",
            OptionItem.sperule to "sperule",
            OptionItem.meihancity to "meihancity",
            OptionItem.longroute to "longroute",
            OptionItem.rule115 to "rule115")

    /**
     * 起動直後のオプションパラメータを取得してメンバ変数をセットアップ
     */
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_result_view)

        setTitle(R.string.title_main_menu_result)

        // back arrow button(戻るボタン有効)
        supportActionBar?.setDisplayHomeAsUpEnabled(true)

        val opts = Option.values()

        if (savedInstanceState != null) {
            mOptMap.forEach {
                mOpt_items[it.key.ordinal] = opts[savedInstanceState.getInt(it.value)]
            }
        } else {
            mOptMap.forEach {
                System.out.println("@@@ %d:%s, %d @@@".format(it.key.ordinal, it.value, intent.getIntExtra(it.value, Option.N_A.ordinal)))
                mOpt_items[it.key.ordinal] = opts[intent.getIntExtra(it.value, Option.N_A.ordinal)]
            }
        }
//        mCalcRoute = CalcRoute(ds).apply {
//            this.calcFareInfo()   // apply lastFlag.rule_en
//            // 経路設定フラグを設定)opt_xxxによって計算オプションを設定
//            setRouteOption(this)  // setFareOption()
//        }
    }

    /**
     * 経路を計算する
     */
    override fun onResume() {
        super.onResume()

        val routeEndIndex = intent.getIntExtra("arrive", -1)
        val ds = Route()

        (application as? FarertApp)?.ds ?: return

        ds.assign((application as FarertApp).ds, routeEndIndex)

        mIsRoundTrip = ds.isAvailableReverse

        setRouteOption(ds)

        mCalcRoute = CalcRoute(ds)

        mCalcRoute?.let {
            val fi = it.calcFareInfo()
            setContentData(fi)  // 表示計算結果
            // 設定可能フラグを設定(make opt_xxx)
            setOptionFlag(fi)   // getFareOption() -> this member attributes(opt_xxx)
        }
    }

    /**
     * 計算に必要なオプションパラメータを破棄される前に保存
     */
    override fun onSaveInstanceState(outState: Bundle?, outPersistentState: PersistableBundle?) {
        super.onSaveInstanceState(outState, outPersistentState)

        mOptMap.forEach { outState?.putInt(it.value, it.key.ordinal) }
    }

    //  Action menu
    //
    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        val id = item.itemId
        when (id) {
            android.R.id.home -> {
              finish()
              return true
            }
            R.id.menu_specialrule -> {
                // 特例
                if (mOpt_items[OptionItem.sperule.ordinal] != Option.N_A) {
                    mOpt_items[OptionItem.sperule.ordinal] =
                            if (item.title.contains(resources.getString(R.string.nothing))) Option.TRUE else Option.FALSE
                    // @"特例を適用しない";
                    // @"特例を適用する"

                    changeOption(OptionItem.sperule)

                    if (mOpt_items[OptionItem.sperule.ordinal] == Option.TRUE) {
                        showInfoAlert(R.string.title_specific_calc_option_norule)
                        refresh()
                    } else {
                        refresh()
                    }
                }
            }
            R.id.menu_meihancity -> {
                if (mOpt_items[OptionItem.meihancity.ordinal] != Option.N_A) {
                    //        result_menu_meihancity_arrive: 着駅を単駅指定
                    mOpt_items[OptionItem.meihancity.ordinal] =
                            if (item.title.equals(resources.getString(R.string.result_menu_meihancity_arrive))) {
                                Option.TRUE  // to Optional
                            } else {
                                Option.FALSE // to Default
                            }
                    // "発駅を単駅指定";
                    // "着駅を単駅指定";
                    changeOption(OptionItem.meihancity)
                    refresh()
                }
            }
/*
            R.id.menu_neerest -> {
                if (opt_neerest != Option.N_A) {
                    opt_neerest = Option.DO_TRUE
                    refresh()
                }
            }
*/
            R.id.menu_osakakan -> {
                if (Option.N_A != mOpt_items[OptionItem.osakakan.ordinal]) {
                    mOpt_items[OptionItem.osakakan.ordinal] =
                            if (item.title.equals(resources.getString(R.string.result_menu_osakakan_far))) {
                                Option.TRUE // to optional
                            } else {
                                Option.FALSE // to default
                            }
                    changeOption(OptionItem.osakakan)

                    if (mOpt_items[OptionItem.osakakan.ordinal] == Option.TRUE) {
                        showInfoAlert(R.string.title_specific_calc_option_osakakan)
                        refresh()
                    } else {
                        refresh()
                    }
                }
            }
            R.id.menu_stocktokai -> {
                if (Option.N_A != mOpt_items[OptionItem.stocktokai.ordinal]) {
                    mOpt_items[OptionItem.stocktokai.ordinal] =
                            if (item.title.contains(resources.getString(R.string.action))) Option.TRUE else Option.FALSE
                    // @"JR東海株主優待券を適用しない";
                    // @"JR東海株主優待券を使用する";
                    changeOption(OptionItem.stocktokai)
                    refresh()
                }
            }
            R.id.menu_longroute -> {
                if (Option.N_A != mOpt_items[OptionItem.longroute.ordinal]) {
                    mOpt_items[OptionItem.longroute.ordinal] =
                        if (item.title.equals(resources.getString(R.string.result_menu_longroute_long))) {
                            //指定した経路で運賃計算
                            Option.TRUE
                        } else {
                            //最安経路で運賃計算(Default)
                            Option.FALSE
                        }
                    changeOption(OptionItem.longroute)
                    refresh()
                }
            }
            R.id.menu_rule115 -> {
                if (Option.N_A != mOpt_items[OptionItem.rule115.ordinal]) {
                    mOpt_items[OptionItem.rule115.ordinal] =
                        if (item.title.equals(resources.getString(R.string.result_menu_rule115_city))) {
                            //旅客営業取扱基準規程115条(特定都区市内発着)
                            Option.TRUE // to optional
                        } else {
                            //旅客営業取扱基準規程115条(単駅最安)(default)
                            Option.FALSE    // to default
                        }
                    changeOption(OptionItem.rule115)
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

    /**
     * 外部エクスポート用Subjectを生成
     */
    private fun resultSubject(fi : FareInfo) : String {
       // "運賃詳細(\(cRouteUtil.terminalName(self.fareInfo.beginStationId)!) - \(cRouteUtil.terminalName(self.fareInfo.endStationId)!))"
        return "${resources.getString(R.string.result_mailtitle_faredetail)}(${terminalName(fi.beginStationId)} - ${terminalName(fi.endStationId)})"
    }

    /**
     * 外部エクスポート用結果テキストを生成
     * @param subject 題名
     * @param ds 経路オブジェクト
     * @param fi 計算結果オブジェクト
     * @return 結果表示テキスト
     */
    private fun resultText(subject : String, ds : CalcRoute, fi: FareInfo) : String {

        var body = "$subject\n\n${ds.showFare()}\n"

        body = body.replace('\\','¥')

        // メールビュー生成
        //_mailViewCtl = [[MFMailComposeViewController alloc] init];

        if (fi.isRuleAppliedEnable) {
            if (fi.isRuleApplied) {
                body += "(特例適用)\n"
            } else {
                body += "(特例未適用)\n"
            }
        }
        body += "\n[指定経路]\n"
        body += ds.route_script()

        return body
    }

    /**
     * menuをリソースマネージャから生成
     */
    override fun onCreateOptionsMenu(menu: Menu?): Boolean {
        menuInflater.inflate(R.menu.menu_result_view, menu)
        return super.onCreateOptionsMenu(menu)
    }

    /**
     * menuを表示する直前に毎回呼ばれる
     * 各メニューアイテムの有効／無効、表示文字列を変える
     */
    override fun onPrepareOptionsMenu(menu: Menu?): Boolean {

        val m = menu?: return super.onPrepareOptionsMenu(menu)

        mCalcRoute?.let {
            menuControl(m)
        }
        return super.onPrepareOptionsMenu(menu)
    }

    /**
     * 画面へ運賃詳細結果テキストを設定
     * @param fi 運賃結果オブジェクト
     */
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
                text_jrgroup_calckm2.text = resources.getString(R.string.result_km, kmNumStr(fi.calcKmForKyusyu))
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
                val rowtbl = row_brtkm
                result_table.removeView(rowtbl)   // BRT km
            } else {
                // BRTのみ
                text_jrline_km.text = resources.getString(R.string.result_km, kmNumStr(fi.jrSalesKm))
                text_title_company_km.text = resources.getString(R.string.result_inbrtline)
                val rowtbl = row_brtkm
                result_table.removeView(rowtbl)   // BRT km
                text_company_km.text = resources.getString(R.string.result_km, kmNumStr(fi.brtSalesKm))
            }
        } else {
            val row = row_jr_or_company_line_km
            result_table.removeView(row)
            val rowtbl = row_brtkm
            result_table.removeView(rowtbl)   // BRT km
        }

        //------------------------- 運賃 -----------------------------------

        text_title_fare.text = getText(R.string.result_fare)

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
                result_table.removeView(row_brt)   // BRT fare

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

            val rowtbl = row_brtyen
            result_table.removeView(rowtbl)   // BRT fare

            /* 2: 往復運賃(割引可否) ＋ BRT往復 */
            val s = resources.getString(R.string.result_yen, fareNumStr(fi.roundTripFareWithCompanyLine)) + strFareOpt
            text_round.text = s
            text_round2.text = resources.getString(R.string.result_yen, fareNumStr(fi.fareForBRT * 2))

        } else if (fi.fareForIC != 0) {
            /* 1: 普通運賃 ＋ IC運賃 */
            text_normalfare.text = resources.getString(R.string.result_yen, fareNumStr(fi.fare))
            text_title_company_fare.text = resources.getString(R.string.result_icfare)
            text_company_fare.text = resources.getString(R.string.result_yen, fareNumStr(fi.fareForIC))

            val rowtbl = row_brtyen
            result_table.removeView(rowtbl)   // BRT fare

            /* 2: 往復運賃 ＋ IC往復運賃 (割引無し) */
            val s = resources.getString(R.string.result_yen, fareNumStr(fi.roundTripFareWithCompanyLine)) + strFareOpt
            text_round.text = s
            text_round2.text = resources.getString(R.string.result_yen, fareNumStr(fi.fareForIC * 2))

        } else {
            /* 1: 普通運賃 */
            text_normalfare.text = resources.getString(R.string.result_yen, fareNumStr(fi.fare))
            text_title_company_fare.text = resources.getString(R.string.blank)
            text_company_fare.text = resources.getString(R.string.blank)

            val rowtbl = row_brtyen
            result_table.removeView(rowtbl)   // BRT fare

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
        var sn = 0
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

        // 有効日数
        /* avail days */
        text_availdays.text = resources.getString(R.string.result_availdays_fmt, fi.ticketAvailDays)

        if (!fi.isArbanArea) {
            if (1 < fi.ticketAvailDays) {
                var str : String
                if (fi.isBeginInCity && fi.isEndInCity) {
                    //発着駅都区市内の駅を除き
                    str = resources.getString(R.string.result_availday_except_begin_end)
                } else if (fi.isBeginInCity) {
                    //発駅都区市内の駅を除き
                    str = resources.getString(R.string.result_availday_except_begin)
                } else if (fi.isEndInCity) {
                    //着駅都区市内の駅を除き
                    str = resources.getString(R.string.result_availday_except_end)
                } else {
                    //''
                    str = resources.getString(R.string.result_availday_empty)
                }
                // 途中下車可能
                text_availdays2.text = resources.getString(R.string.result_availday_stopover, str)
            } else {
                // 途中下車不可
                text_availdays2.text = resources.getString(R.string.result_availday_dontstopover)
            }
        } else {
            text_availdays2.text = resources.getString(R.string.result_availday_dontstopover)
        }

        // 備考
        text_note.text = fi.resultMessage

        /* ROUTE 経由 */
        var s = fi.routeList
        if (!fi.routeListForTOICA.isNullOrEmpty()) {
            s += "\n${resources.getString(R.string.result_toica_calc_route)}\n${fi.routeListForTOICA}"
        }
        s += "\n\n"
        text_via.text = s

    }

    /**
     * 結果オプション を設定する
     * onCreateの終わり、運賃計算前に処理する
     * @param ds 経路オブジェクト
     */
    private fun setRouteOption(ds : Route) {

        // 最短経路
        /*
        if (opt_neerest == Option.DO_TRUE) {
            val end_id = ds.arriveStationId()
            val begin_id = ds.departureStationId()
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
                ds.changeNeerest(0, begin_id)
                //opt_neerest = Option.FALSE
                opt_sperule = Option.N_A
                opt_osakakan = Option.N_A
                opt_meihancity = Option.N_A
                opt_stocktokai = Option.N_A
            }
            return
        }
        */

        if (mOpt_items[OptionItem.osakakan.ordinal] == Option.TRUE
                || mOpt_items[OptionItem.osakakan.ordinal] == Option.FALSE) {
            // 遠回り・近回り
            val rc = ds.setDetour((mOpt_items[OptionItem.osakakan.ordinal] == Option.TRUE))
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

        // 特例
        if (Option.TRUE == mOpt_items[OptionItem.sperule.ordinal]) {
            // @"特例を適用しない";
            ds.setNoRule(true)
        } else if (Option.FALSE == mOpt_items[OptionItem.sperule.ordinal]) {
            // @"特例を適用する";
            ds.setNoRule(false)
        }

        // 大高-杉本町
        if (Option.TRUE == mOpt_items[OptionItem.meihancity.ordinal]) {
            // "着駅を単駅指定";
            ds.routeFlag.setStartAsCity()
        } else if (Option.FALSE == mOpt_items[OptionItem.meihancity.ordinal]) {
            // "発駅を単駅指定";
            ds.routeFlag.setArriveAsCity()
        }

        // JR東海株主
        if (Option.FALSE == mOpt_items[OptionItem.stocktokai.ordinal]) {
            // @"JR東海株主優待券を適用しない"(default);
            ds.routeFlag.setJrTokaiStockApply(false)
        } else if (Option.TRUE == mOpt_items[OptionItem.stocktokai.ordinal]) {
            // @"JR東海株主優待券を使用する";
            ds.routeFlag.setJrTokaiStockApply(true)
        }

        // 指定した経路で運賃計算 / 最安経路で運賃計算(Default)
        if (Option.FALSE == mOpt_items[OptionItem.longroute.ordinal]) {
            // 最安経路で運賃計算(Default)
            ds.routeFlag.setLongRoute(false)
        } else if (Option.TRUE == mOpt_items[OptionItem.longroute.ordinal]) {
            // 指定した経路で運賃計算
            ds.routeFlag.setLongRoute(true)
        }

        // 旅客営業取扱基準規程115条(特定都区市内発着) / (単駅最安)(default)
        if (Option.FALSE == mOpt_items[OptionItem.rule115.ordinal]) {
            // 単駅最安(default)
            ds.routeFlag.setSpecificTermRule115(false)
        } else if (Option.TRUE == mOpt_items[OptionItem.rule115.ordinal]) {
            // 旅客営業取扱基準規程115条(特定都区市内発着)
            ds.routeFlag.setSpecificTermRule115(true)
        }
    }

    // from resume last
    /**
     * 運賃計算オプションをメンバ変数へ設定
     * onCreateの最終工程、運賃計算後に呼ばれる
     * @param fi 結果オブジェクト
     */
    private fun setOptionFlag(fi: FareInfo) {

        mOpt_items[OptionItem.sperule.ordinal] =
            if (fi.isRuleAppliedEnable) {
                if (fi.isRuleApplied) {
                    // 適用中なら"特例を適用しない"メニューが表示
                    // default is applied(not no_rule)
                     Option.FALSE
                } else {
                    // 適用されていなければ、"特例を適用するメニューが表示される
                    Option.TRUE
                }
            } else {
                Option.N_A
            }

        // 名阪 着駅を都区市内、発駅を都区市内
        mOpt_items[OptionItem.meihancity.ordinal] =
            if (fi.isMeihanCityStartTerminalEnable) {
                    if (fi.isMeihanCityStart) {
                        // 発駅を単駅指定(Default)
                        Option.FALSE
                    } else {
                        // 着駅を単駅指定
                        Option.TRUE
                    }
            } else {
                Option.N_A
            }

        // 指定した経路で運賃計算 / 最安経路で運賃計算(Default)
        mOpt_items[OptionItem.longroute.ordinal] =
            if (fi.isEnableLongRoute) {
                if (fi.isLongRoute) {
                    // 指定した経路で運賃計算
                    Option.TRUE
                } else {
                    // 最安経路で運賃計算(Default)
                    Option.FALSE
                }
            } else {
                Option.N_A
            }

        // 旅客営業取扱基準規程115条(特定都区市内発着) / (単駅最安)(default)
        mOpt_items[OptionItem.rule115.ordinal] =
            if (fi.isEnableRule115) {
                if (fi.isRule115specificTerm) {
                    // 旅客営業取扱基準規程115条(特定都区市内発着)
                    Option.TRUE
                } else {
                    // (単駅最安)(default)
                    Option.FALSE
                }
            } else {
                Option.N_A
            }

        //大阪環状線 周り方向
        mOpt_items[OptionItem.osakakan.ordinal] =
            if (fi.isOsakakanDetourEnable) {
                if (fi.isOsakakanDetour) {
                    // "大阪環状線 遠回りだと近回り選択メニューが表示される"
                    Option.TRUE
                } else {
                    // "大阪環状線 近回りだと遠回り選択メニューが表示される
                    // Default is nearest.
                    Option.FALSE
                }
            } else {
                Option.N_A
            }

        // "最短経路算出"
        /*
        if (opt_neerest != Option.DO_TRUE) {
            opt_neerest = if (fi.isArbanArea) Option.FALSE else Option.N_A    // 最初は無効(メニューのチェック状態を頼る)
        }
*/

        // JR東海株主
        mOpt_items[OptionItem.stocktokai.ordinal] =
            if (fi.isJRCentralStockEnable) {
                if (fi.isJRCentralStock) {
                    // 適用中なら"JR東海株主優待券を適用しない"がメニュー選択できる
                    Option.TRUE
                } else {
                    // 適用されていなければ、"JR東海株主優待券を適用する"がメニュー選択できる
                    // default
                    Option.FALSE
                }
            } else {
                Option.N_A
            }
    }

    /**
     * メニュー制御で、メニュー表示直前によばれる
     */
    private fun menuControl(menu: Menu?) {
        // menu　prepare open menu onPrepareOptionsMenu()
        //
        menu?.apply {
            val mi_specialrule = findItem(R.id.menu_specialrule) ?: return
            val mi_osakakan = findItem(R.id.menu_osakakan) ?: return
            val mi_meihancity = findItem(R.id.menu_meihancity) ?: return
            val mi_stocktokai = findItem(R.id.menu_stocktokai) ?: return
            val mi_neerest = findItem(R.id.menu_neerest) ?: return
            val mi_longroute = findItem(R.id.menu_longroute) ?: return
            val mi_rule115 = findItem(R.id.menu_rule115) ?: return

            when (mOpt_items[OptionItem.sperule.ordinal]) {
                Option.TRUE -> {
                    mi_specialrule.setVisible(true)
                    // 適用されていなければ、"特例を適用するメニューが表示される(Optional -> default)
                    mi_specialrule.title = "${resources.getString(R.string.result_menu_sperule)}${resources.getString(R.string.action)}"
                }
                Option.FALSE -> {
                    mi_specialrule.setVisible(true)
                    // 適用中なら"特例を適用しない"メニューが表示
                    mi_specialrule.title = "${resources.getString(R.string.result_menu_sperule)}${resources.getString(R.string.nothing)}"
                }
                else -> {
                    // Option.N_A
                    mi_specialrule.setVisible(false)  //  hide
                }
            }

            if (mOpt_items[OptionItem.meihancity.ordinal] != Option.N_A) {
                mi_meihancity.setVisible(true)
                if (mOpt_items[OptionItem.meihancity.ordinal] == Option.TRUE) {
                    // fi.isMeihanCityStart()
                    // 発駅=都区市内
                    // "発駅を単駅指定"(Optional -> default)
                    mi_meihancity.title = resources.getString(R.string.result_menu_meihancity_start)
                } else  if (mOpt_items[OptionItem.meihancity.ordinal] == Option.FALSE) {
                    // "着駅を単駅指定"
                    mi_meihancity.title = resources.getString(R.string.result_menu_meihancity_arrive)
                } else {
                    assert(false)
                }
            } else {
                mi_meihancity.setVisible(false)
            }

            // 指定した経路で運賃計算 / 最安経路で運賃計算(Default)
            if (mOpt_items[OptionItem.longroute.ordinal] != Option.N_A) {
                mi_longroute.setVisible(true)
                if (mOpt_items[OptionItem.longroute.ordinal] == Option.TRUE) {
                    // 最安経路で運賃計算(Optional -> default)
                    mi_longroute.title = resources.getString(R.string.result_menu_longroute_lowcost)
                } else  {
                    // 指定した経路で運賃計算
                    mi_longroute.title = resources.getString(R.string.result_menu_longroute_long)
                }
            } else {
                mi_longroute.setVisible(false)
            }

            // 旅客営業取扱基準規程115条(特定都区市内発着) / (単駅最安)(default)
            if (mOpt_items[OptionItem.rule115.ordinal] != Option.N_A) {
                mi_rule115.setVisible(true)
                if (mOpt_items[OptionItem.rule115.ordinal] == Option.TRUE) {
                    // 単駅最安(Optional -> default)
                    mi_rule115.title = resources.getString(R.string.result_menu_rule115_station)
                } else  {
                    // 旅客営業取扱基準規程115条(特定都区市内発着)
                    mi_rule115.title = resources.getString(R.string.result_menu_rule115_city)
                }
            } else {
                mi_rule115.setVisible(false)
            }


            // "最短経路算出"
            //mi_neerest.setVisible(mOpt_items[OptionItem.neerest.ordinal] != Option.N_A)
            mi_neerest.setVisible(false)
            //mi_neerest.setEnabled(mOpt_items[OptionItem.neerest.ordinal] != Option.N_A
            // && !mi_neerest.isChecked)

            // stock
            if (mOpt_items[OptionItem.stocktokai.ordinal] != Option.N_A) {
                // fi.isJRCentralStockEnable()) {
                val tok = resources.getString(R.string.result_menu_stocktokai)
                if (mOpt_items[OptionItem.stocktokai.ordinal] == Option.TRUE) {
                    // fi.isJRCentralStock()) {
                    // 適用中なら"JR東海株主優待券を適用しない"がメニュー選択できる
                    // (Optional -> default)
                    mi_stocktokai.title = "$tok${resources.getString(R.string.nothing)}"
                } else {
                    // 適用されていなければ、"JR東海株主優待券を適用する"がメニュー選択できる
                    mi_stocktokai.title = "$tok${resources.getString(R.string.action)}"
                }
                mi_stocktokai.setVisible(true)  // 表示
            } else {
                mi_stocktokai.setVisible(false) // 非表示
            }

            if (mOpt_items[OptionItem.osakakan.ordinal] != Option.N_A) { // fi.isOsakakanDetourEnable) {
                if (mOpt_items[OptionItem.osakakan.ordinal] == Option.TRUE) { // fi.isOsakakanDetour) {
                    // "大阪環状線 遠回りだと近回り選択メニューが表示される"(Optional -> default)
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

    /**
     * 運賃計算結果画面を最表示する
     * 運賃計算オプションを保持しActivityを再起動する
     */
    private fun refresh() {
        finish()
        mOptMap.forEach {
            this.intent.putExtra(it.value, mOpt_items[it.key.ordinal].ordinal)
        }
        startActivity(this.intent)
    }

    /**
     *  外部エクスポート用結果テキストをエクスポートする
     *  @param subject タイトル
     *  @param text 運賃計算結果テキスト
     */
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

    /**
     *  情報アラートを抑制するか(最初の一回)
     *  @param key 大阪環状線か特例非適用か
     */
    fun showInfoAlert(key : Int) {

        val info = mapOf(
                R.string.title_specific_calc_option_norule to arrayOf("setting_key_hide_no_rule_info"),
                R.string.title_specific_calc_option_osakakan to arrayOf("setting_key_hide_osakakan_detour_info"))

        val keyInfo = info.get(key) ?: return

        val valKey = keyInfo.get(0)

        if (valKey.isNullOrEmpty()) {
            return
        }

        val r = readParam(this, valKey)
        if (r != "true") {
            val title = resources.getString(key)
            val msg = resources.getString(R.string.desc_specific_calc_option, title)
            val build = AlertDialog.Builder(this).apply {
                setTitle(title)
                setMessage(msg)
                setNegativeButton(R.string.hide_specific_calc_option_info) { _, _ ->
                    saveParam(context, valKey, "true")
                }
                setPositiveButton(R.string.agree) { _, _ ->
                }
            }
            val dlg = build.create()
            dlg.show()
        }
    }

    /**
     *  Menu actionの最後に呼ばれる
     *  運賃計算オプションは依存する部分があり、設定してみてさらに状態を読み込んでメンバ変数へ保持
     *  @param opt オプションの種類
     */
    private fun changeOption(opt : OptionItem) {
        val route = Route(mCalcRoute)

        when (opt) {
            OptionItem.stocktokai -> {     // TRUEはJR東海株主優待券を使用する、FALSEは使用しない
                route.routeFlag.setJrTokaiStockApply(mOpt_items[opt.ordinal] == Option.TRUE)
            }
            OptionItem.osakakan -> {      // TRUEは遠回り、FALSEは近回り
                route.setDetour(mOpt_items[opt.ordinal] == Option.TRUE)
            }
            OptionItem.sperule -> {  // TRUEは非適用、FALSEは特例適用
                route.setNoRule(mOpt_items[opt.ordinal] == Option.TRUE)
            }
            OptionItem.meihancity -> {  // TRUEは着駅を単駅指定、FALSEは発駅を単駅指定
                if (Option.TRUE == mOpt_items[opt.ordinal]) {
                    // "着駅を単駅指定";
                    route.routeFlag.setStartAsCity()
                } else if (Option.FALSE == mOpt_items[opt.ordinal]) {
                    // "発駅を単駅指定";
                    route.routeFlag.setArriveAsCity()
                }
            }
            OptionItem.longroute -> { // TRUEは近郊区間遠回り
                route.routeFlag.setLongRoute(mOpt_items[opt.ordinal] == Option.TRUE)
            }
            OptionItem.rule115 -> {  // TRUEは115条 都区市内発着指定
                route.routeFlag.setSpecificTermRule115(mOpt_items[opt.ordinal] == Option.TRUE)
            }
        }

        // 大阪環状線 遠回り TRUE=遠回り
        if (route.isOsakakanDetourEnable) {
            mOpt_items[OptionItem.osakakan.ordinal] = if (route.isOsakakanDetour)
                Option.TRUE else Option.FALSE
        } else {
            mOpt_items[OptionItem.osakakan.ordinal] = Option.N_A
        }

        // 特例非適用(TRUE) / 適用
        if (route.routeFlag.rule_en()) {
            mOpt_items[OptionItem.sperule.ordinal] = if (route.routeFlag.no_rule())
                Option.TRUE else Option.FALSE
        } else {
            mOpt_items[OptionItem.sperule.ordinal] = Option.N_A
        }

        // 旅客営業取扱基準規定 第115条(TRUE) ／ 都区市内発着指定(FALSE)
        if (route.routeFlag.isEnableRule115) {
            mOpt_items[OptionItem.rule115.ordinal] =
                    if (route.routeFlag.isRule115specificTerm())
                Option.TRUE else Option.FALSE
        } else {
            mOpt_items[OptionItem.rule115.ordinal] = Option.N_A
        }

        // 指定経路(TRUE)、最安経路(FALSE)
        if (route.routeFlag.isEnableLongRoute) {
            mOpt_items[OptionItem.longroute.ordinal] = if (route.routeFlag.isLongRoute)
                Option.TRUE else Option.FALSE
        } else {
            mOpt_items[OptionItem.longroute.ordinal] = Option.N_A
        }

        // JR東海株主 使用 有(TRUE)無(FALSE)
        if (route.routeFlag.jrtokaistock_enable()) {
            mOpt_items[OptionItem.stocktokai.ordinal] = if (route.routeFlag.jrTokaiStockApply())
                Option.TRUE else Option.FALSE
        } else {
            mOpt_items[OptionItem.stocktokai.ordinal] = Option.N_A
        }

        // 名阪 着駅を単駅に
        // ／発駅を単駅に TRUEは着駅を単駅指定、FALSEは発駅を単駅指定
        if (route.routeFlag.isMeihanCityEnable) {
            mOpt_items[OptionItem.meihancity.ordinal] = if (route.routeFlag.isArriveAsCity)
                Option.TRUE else Option.FALSE
        } else {
            mOpt_items[OptionItem.meihancity.ordinal] = Option.N_A
        }
    }
}
