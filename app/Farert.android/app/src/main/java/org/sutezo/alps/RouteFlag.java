package org.sutezo.alps;

import static org.sutezo.alps.farertAssert.*;

public class RouteFlag {

    private byte osakaKanPass = 0;

    boolean no_rule = false;               //8 ON: 特例非適用(User->System)

    // bit 10 JR東海 東海道新幹線区間
    boolean jrtokaistock_enable = false;	//10 提案可能フラグ
    //   東海道新幹線で[区][山][浜][京][阪]が適用する場面でセットされる
    //   JR東海株主優待適用・非適用選択メニューが有効となる
    boolean jrtokaistock_applied= false; 		//11 提案適用フラグ
    //   UI側からユーザが指定する

    // bit7 - 大高-杉本町とかで、[名][阪]をどっちに適用するか
    boolean meihan_city_flag = false;	    //7 ON: APPLIED_START / OFF:APPLIED_TERMINAL(User->System)

    boolean rule88;
    boolean rule69;
    boolean rule70;
    boolean special_fare_enable;
    int    rule115;
    boolean rule70bullet;
    boolean rule16_5;

    boolean bullet_line;    // 新幹線乗車している
    boolean bJrTokaiOnly;

    boolean meihan_city_enable;    //名阪発駅単駅着駅都区市内
    boolean trackmarkctl;	        //5 次にremoveTailでlastItemの通過マスクをOffする(typeOでもPでもないので)
    boolean jctsp_route_change;	//6 分岐特例(add内部使用)

    // bit 11-20 発着 都区市内 適用
    boolean disable_rule86or87 = false;
    boolean ter_begin_city		= false;      //13 [区][浜][名][京][阪][神][広][九][福][仙][札]
    boolean ter_fin_city		= false;      //14
    boolean ter_begin_yamate	= false;      //15/ [山]
    boolean ter_fin_yamate		= false;      //16
    boolean ter_begin_oosaka	= false;      //21 大阪・新大阪
    boolean ter_fin_oosaka		= false;      //22
    void terCityReset() {
        ter_begin_city		= false;      //13 [区][浜][名][京][阪][神][広][九][福][仙][札]
        ter_fin_city		= false;      //14
        ter_begin_yamate	= false;      //15/ [山]
        ter_fin_yamate		= false;      //16
         ter_begin_oosaka	= false;      //21 大阪・新大阪
        ter_fin_oosaka		= false;      //22
    }
    boolean isTerCity() {
        return
                ter_begin_city		||      //13 [区][浜][名][京][阪][神][広][九][福][仙][札]
                ter_fin_city		||      //14
                ter_begin_yamate	||      //15/ [山]
                ter_fin_yamate		||      //16
                ter_begin_oosaka	||      //21 大阪・新大阪
                ter_fin_oosaka		;      //22
    }
    boolean isEnableRule86() {
        return ter_begin_city		||      //13 [区][浜][名][京][阪][神][広][九][福][仙][札]
                ter_fin_city;
    }
    boolean isEnableRule87() {
        return ter_begin_yamate	||      //15/ [山]
                ter_fin_yamate;
    }

    // 会社線
    boolean compncheck	    	= false;      //23 会社線通過チェック有効
    boolean compnpass			= false;      //24 通過連絡運輸
    boolean compnda			    = false;      //25 通過連絡運輸不正フラグ
    boolean compnbegin			= false;      //26	会社線で開始
    boolean compnend			= false;      //27 会社線で終了
    boolean compnterm           = false;      // 会社線通過連絡運輸発着駅チェック
    boolean tokai_shinkansen    = false;

    int urban_neerest;   // 近郊区間内で最安経路算出可能(適用で計算して保持) owner is user/system both.
                            // 0: N/A
                            // 1: Neer , -1: Far
    // Global
    boolean notsamekokurahakatashinzai = false;    //30 Route only : 小倉-博多間 新在別線扱い
    boolean end			= false;               //31 arrive to end.
    // end of last_flag


    RouteFlag() {
        clear();
    }
    RouteFlag(final RouteFlag another) {
        setAnotherRouteFlag(another);
    }

    void clear() {
        /* last_flag */
        // boolean LASTFLG_OFF = RouteUtil.MASK(notsamekokurahakatashinzai);   // all bit clear at removeAll()
        osakaKanPass = 0;      //0-1

        // bit 2-3
        osakakan_1dir = false;	//2 大阪環状線 1回目方向
        osakakan_2dir = false;	//3 大阪環状線 2回目方向

        osakakan_detour = false;   //4 大阪環状線 1回目遠回り(UI側から指定, 内部はR/O)

        trackmarkctl = false;	        //5 次にremoveTailでlastItemの通過マスクをOffする(typeOでもPでもないので)
        jctsp_route_change = false;	//6 分岐特例(add内部使用)

        disable_rule86or87 = false;

        ter_begin_city		= false;      //13 [区][浜][名][京][阪][神][広][九][福][仙][札]
        ter_fin_city		= false;      //14
        ter_begin_yamate	= false;      //15/ [山]
        ter_fin_yamate		= false;      //16
        ter_begin_oosaka	= false;      //21 大阪・新大阪
        ter_fin_oosaka		= false;      //22
        compncheck		    = false;      //23 会社線通過チェック有効
        compnpass			= false;      //24 通過連絡運輸
        compnda			= false;      //25 通過連絡運輸不正フラグ
        compnbegin			= false;      //26	会社線で開始
        compnend			= false;      //27 会社線で終了
        compnterm           = false;      // 会社線通過連絡運輸発着駅チェック

        tokai_shinkansen    = false;

        notsamekokurahakatashinzai = false;    //30 Route only : 小倉-博多間 新在別線扱い
        end			    = false;      //31 arrive to end.

        urban_neerest = 0;
        rule115 = 0;
        rule70bullet = false;
        rule88 = false;
        rule69 = false;
        rule70 = false;
        special_fare_enable = false;
        rule16_5 = false;

        bullet_line = false;
        bJrTokaiOnly = false;

        meihan_city_enable	= false;      //19

        meihan_city_flag = false;	    //7 ON: APPLIED_START / OFF:APPLIED_TERMINAL(User->System)
        no_rule = false;                //8 ON: 特例非適用(User->System)

        jrtokaistock_enable= false;	    //10 提案可能フラグ
        jrtokaistock_applied= false; 	//11 提案適用フラグ
    }


    public void setAnotherRouteFlag(final RouteFlag o) {
        osakaKanPass = o.osakaKanPass;
        osakakan_1dir = o.osakakan_1dir;
        osakakan_2dir = o.osakakan_2dir;
        osakakan_detour = o.osakakan_detour;

        no_rule = o.no_rule;
        jrtokaistock_enable = o.jrtokaistock_enable;
        jrtokaistock_applied = o.jrtokaistock_applied;
        meihan_city_flag = o.meihan_city_flag;
        meihan_city_enable = o.meihan_city_enable;

        rule88 = o.rule88;
        rule69 = o.rule69;
        rule70 = o.rule70;
        special_fare_enable = o.special_fare_enable;
        rule115 = o.rule115;
        rule70bullet = o.rule70bullet;
        bullet_line = o.bullet_line;
        bJrTokaiOnly = o.bJrTokaiOnly;
        rule16_5 = o.rule16_5;

        trackmarkctl = o.trackmarkctl;
        jctsp_route_change = o.jctsp_route_change;
        disable_rule86or87 = o.disable_rule86or87;
        ter_begin_city = o.ter_begin_city;
        ter_fin_city = o.ter_fin_city;
        ter_begin_yamate = o.ter_begin_yamate;
        ter_fin_yamate = o.ter_fin_yamate;
        ter_begin_oosaka = o.ter_begin_oosaka;
        ter_fin_oosaka = o.ter_fin_oosaka;
        compncheck = o.compncheck;
        compnpass = o.compnpass;
        compnda = o.compnda;
        compnbegin = o.compnbegin;
        compnend = o.compnend;
        compnterm = o.compnterm;
        tokai_shinkansen = o.tokai_shinkansen;
        urban_neerest = o.urban_neerest;
        notsamekokurahakatashinzai = o.notsamekokurahakatashinzai;
        end = o.end	;
    }

    public boolean no_rule() { return no_rule; }
    public boolean rule_en() {
        return isEnableRule86() ||
                isEnableRule87() ||
                rule88 ||
                rule69 ||
                rule70 ||
                special_fare_enable ||
                meihan_city_enable;
    }
    public void setNoRule(boolean b_rule) { no_rule = b_rule; }

    // UI側からセットする制御フラグ
    public boolean isEnableLongRoute() { return !no_rule && 0 != urban_neerest; }
    public boolean isLongRoute() { return urban_neerest < 0; }
    public void setLongRoute(boolean far) {
        if (far) {
            urban_neerest = -1;
        } else {
            urban_neerest = 1;
        }
    }

    public boolean isEnableRule115()  { return !no_rule && 0 != rule115; }
    public boolean isRule115specificTerm()  { return rule115 < 0; }
    public void setSpecificTermRule115(boolean ena) {
        if (ena) {
            rule115 = -1;
        } else {
            rule115 = 1;
        }
    }
    public void setStartAsCity() { /*ASSERT(meihan_city_enable); */ meihan_city_flag = true;   /* 着駅=単駅、発駅市内駅 */ }
    public void setArriveAsCity() { /*ASSERT(meihan_city_enable); */ meihan_city_flag = false; /* 発駅=単駅、着駅市内駅 */ }
//    public void setJrTokaiStockApply(boolean flag) { jrtokaistock_applied = flag; }
    public boolean jrTokaiStockApply() { return jrtokaistock_applied; }
    public boolean jrtokaistock_enable() { return jrtokaistock_enable; }
    /* clearRule()潰すと、株主有効が使えないので、こう(上)してみた */
    /* coreAreaIDByCityId() が影響 */
    void setDisableRule86or87() { disable_rule86or87 = true; }
    void setEnableRule86or87()  { disable_rule86or87 = false; }
    boolean isEnableRule86or87()  { return !disable_rule86or87; }
    boolean isAvailableRule86or87()  { return !disable_rule86or87 && (isEnableRule86() || isEnableRule87()); }
    boolean isAvailableRule86()  { return !disable_rule86or87 && isEnableRule86(); }
    boolean isAvailableRule87()  { return !disable_rule86or87 && isEnableRule87(); }
    boolean isAvailableRule88() { return rule88; }
    boolean isAvailableRule70() { return rule70; }
    boolean isAvailableRule69() { return rule69; }
    boolean isAvailableRule115() { return 0 < rule115; }
    boolean isAvailableRule16_5() { return rule16_5; }
    //
    public boolean isMeihanCityEnable() {
        return !no_rule && meihan_city_enable;
    }
    public boolean isArriveAsCity()  { return (meihan_city_enable == true) &&  (meihan_city_flag == false); }
    public boolean isStartAsCity()  { return (meihan_city_enable == true) &&  (meihan_city_flag == true); }


    // for debug print
    int getOsakaKanPassValue()      { return osakaKanPass; }

    enum OSAKAKAN_PASS {
        OSAKAKAN_NOPASS,	// 初期状態(大阪環状線未通過)
        OSAKAKAN_1PASS,	    // 大阪環状線 1回通過
        OSAKAKAN_2PASS, 	// 大阪環状線 2回通過
    };

    boolean is_osakakan_1pass() {
        return OSAKAKAN_PASS.OSAKAKAN_1PASS.ordinal() == (osakaKanPass & 0x03);
    }

    boolean is_osakakan_2pass() {
        return OSAKAKAN_PASS.OSAKAKAN_2PASS.ordinal() == (osakaKanPass & 0x03);
    }

    boolean is_osakakan_nopass() {
        return OSAKAKAN_PASS.OSAKAKAN_NOPASS.ordinal() == (osakaKanPass & 0x03);
    }

    void setOsakaKanPass(boolean value) {
        if (value) {
            osakaKanPass |= (1 << 0);
        } else {
            osakaKanPass &= ~(1 << 0);
        }
    }
    boolean getOsakaKanPass() { return 0 != (osakaKanPass & (1 << 0)); }

    // bit 2-3
    boolean osakakan_1dir = false;	//2 大阪環状線 1回目方向
    boolean osakakan_2dir = false;	//3 大阪環状線 2回目方向

    boolean osakakan_detour = false;   //4 大阪環状線 1回目遠回り(UI側から指定, 内部はR/O)

    void setOsakaKanFlag(OSAKAKAN_PASS pass) {
        this.osakaKanPass = ((byte) pass.ordinal());
    }

    void setOsakaKanFlag(RouteFlag lf) {
        this.osakaKanPass = lf.osakaKanPass;
        this.osakakan_1dir = lf.osakakan_1dir;
        this.osakakan_2dir = lf.osakakan_2dir;
    }

    boolean isRoundTrip()  {
        return !end || compnda;
    }


    void optionFlagReset() {
        special_fare_enable = false;
        meihan_city_enable = false;
        rule88 = false;
        rule69 = false;
        rule70 = false;
        rule70bullet = false;
    }


    // 特例非適用ならTrueを返す。LAST_FLAG.BLF_NO_RULEのコピー
    //
    boolean isUseBullet()  { return bullet_line || rule70bullet; }

    // 会社線含んでいる場合Trueを返す
    public boolean isIncludeCompanyLine() { return compncheck; }
}
