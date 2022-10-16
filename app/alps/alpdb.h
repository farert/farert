#if !defined _ALPDB_H__

#define _ALPDB_H__

#if !defined _WINDOWS
typedef struct {
    char name[128];             // UTF-8 max 42character
    int32_t tax;
    char createdate[64];    // UTF-8 max 42character '2015/03/14 12:43:43'
    uint32_t options;
} DBsys;
#else
typedef struct {
    TCHAR name[128];             // max 42character
    int32_t tax;
    TCHAR createdate[64];        // max 42character '2015/03/14 12:43:43'
    uint32_t options;
} DBsys;
#endif


#if defined __cplusplus

typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef short int16_t;
#if defined _WINDOWS && _MSC_VER < 1900
typedef char int8_t;
#endif

extern int g_tax;	/* in alps_mfc.cpp(Windows) or main.m(iOS) or main.cpp(unix) */

#include "stdafx.h"
using namespace std;
#include <vector>

/*!	@file alpdb core logic implement.
 *	Copyright(c) sutezo9@me.com 2012.
 */
extern bool isKanaString(LPCTSTR szStr);
extern void conv_to_kana2hira(tstring& kana_str);

extern tstring num_str_km(int32_t num);
extern tstring num_str_yen(int32_t num);


#define iskana(c) (_T('ァ') <= (c)) && ((c) <= _T('ン'))

typedef short IDENT;
typedef uint32_t PAIRIDENT;
typedef uint32_t SPECIFICFLAG;

/* --------------------------------------- */
/* util */
#define NumOf(c)                            (sizeof(c) / sizeof(c[0]))
#define Min(a, b)                           ((a)>(b)?(b):(a))
#define Max(a, b)                           ((a)>(b)?(a):(b))
#define Limit(v, l, h)                      (Min(Max(v, l), h))

#define MASK(bdef)                          (1 << bdef)
#define BIT_CHK(flg, bdef)                  (0 != (flg & MASK(bdef)))
#define BIT_CHK2(flg, bdef1, bdef2)         (0 != (flg & (MASK(bdef1)|MASK(bdef2))))
#define BIT_CHK3(flg, bdef1, bdef2, bdef3)  (0 != (flg & (MASK(bdef1)|MASK(bdef2)|MASK(bdef3))))
#define BIT_ON(flg, bdef)                   (flg |= MASK(bdef))
#define BIT_OFF(flg, bdef)                  (flg &= ~MASK(bdef))
#define BIT_OFFN(flag, pattern)				(flag &= ~(pattern))

#define HWORD_BIT	16		/* Number of bit in half word(unsigned short) */

#if !defined LOBYTE
#define LOBYTE(b) ((b)&0xff)
#endif

/* --------------------------------------- */
#define MAKEPAIR(ident1, ident2)    ((PAIRIDENT)(0xffff & ident1) | ((PAIRIDENT)ident2 << 16))
#define IDENT1(ident)               ((IDENT)(0xffff & ident))
#define IDENT2(ident)               ((IDENT)(0xffff & ((PAIRIDENT)ident >> 16)))

// flag from DB
#define FLG_HIDE_LINE				(1<<19)
#define FLG_HIDE_STATION			(1<<18)
#define IS_FLG_HIDE_LINE(lflg)		(0!=(lflg&FLG_HIDE_LINE))		// 路線非表示
#define IS_FLG_HIDE_STATION(lflg)	(0!=(lflg&FLG_HIDE_STATION))	// 駅非表示

#define BSRJCTSP		31		// [w]分岐特例
#define	BSRJCTHORD		31		// [w]水平型検知フラグ

#define BSRNOTYET_NA	30		// [w]不完全ルート
                                // AttrOfStationOnLineLine()の戻り値(add()時の駅正当チェックに局所的に使用)
                                // 規43−2や、分岐特例途中の中途半端な経路の時ON
                                // 会社線通過連絡運輸で発着駅のみのチェックである場合もON（と同時に、COMPNTERMINALもON）
#define BSRJCTSP_B		29		// [w]分岐特例B
#define BSRSHINZAIREV   28     // [w]新幹線、在在来線折り返し

#define BSR69TERM		24		// [r]  // not used
#define BSR69CONT		23		// [r]
#define BSRSHINKTRSALW  19      // [r] 19:20 並行在来線乗換折返許可
#define BSRJCT			15		// [r]
#define BSRBORDER       16		// [r]

#define JCTSP				 	1
#define JCTSP_2L			 	2
#define JCTSP_B_NISHIKOKURA 	3
#define JCTSP_B_YOSHIZUKA	 	4
#define JCTSP_B_NAGAOKA 		5

#define LID_BRT        0x4000
#define BRTMASK(id)    (~LID_BRT & (id))
#define LID_SHINKANZEN 0x1000
#define LID_COMPANY    0x2000

/* ---------------------------------------!!!!!!!!!!!!!!! */
#define MAX_STATION     4590
//#define MAX_LINE        240
#define IS_SHINKANSEN_LINE(id)  ((LID_SHINKANZEN<(id)) && ((id) < (LID_SHINKANZEN+0x100)))	 /* 新幹線は将来的にも10または15以内 !! */
#define IS_COMPANY_LINE(id)	    ((LID_COMPANY<(id)) && ((id) < (LID_COMPANY+0x100)))	     /* 会社線id */
#define IS_BRT_LINE(id)	        ((LID_BRT<(id)) && ((id) < (LID_BRT+0x100)))            	 /* BRT id */
#define MAX_JCT 350
#define MAX_COMPNPASSSET   25       // 会社線 限定的 通過連絡運輸での 有効路線数の最大 （篠ノ井線、信越線(篠ノ井-長野)）*/
/* ---------------------------------------!!!!!!!!!!!!!!! */

// 駅は分岐駅か
#define STATION_IS_JUNCTION(sid)        (0 != (RouteUtil::AttrOfStationId(sid) & (1<<12)))
//#define STATION_IS_JUNCTION_F(flg)	(0 != (flg & (1<<12)))
// sflg.12は特例乗り換え駅もONなのでlflg.15にした
#define STATION_IS_JUNCTION_F(flg)		(0 != (flg & (1<<15)))

	/* ID for line_id on t_lines */
#define ID_L_RULE70		-10


// length define(UTF-8)
#define MAX_STATION_CHR     64		// 38
#define MAX_LINE_CHR        128
#define MAX_COMPANY_CHR     64		//
#define MAX_PREFECT_CHR     16
#define MAX_COREAREA_CHR    26

#define MAX_NUM_JCT_STATION	103
#define MAX_JCT_NUM			6		// 東京駅乗り入れ路線数が最大で6


const LPCTSTR CLEAR_HISTORY = _T("(clear)");

/* 消費税(四捨五入)加算 */
#define taxadd(fare, tax)    (fare + ((fare * 1000 * tax / 100000) + 5) / 10 * 10)
#define taxadd_ic(fare, tax) (fare + (fare * 1000 * tax / 100000))

/* round up on 5 */
#define round(d) 		(((d) + 5) / 10 * 10)	/* 10円未満四捨五入 */
#define round_up(d)     (((d) + 9) / 10 * 10)	/* 10円未満切り上げ */
#define round_down(d)   ((d) / 10 * 10)			/* 10円未満切り捨て */

#define KM(kmx10)       ((kmx10 + 9) / 10)	/* km単位で端数は切り上げ */

/* discount */
#define fare_discount(fare, per) ((fare) / 10 * (10 - (per)) / 10 * 10)
/* discount 5円の端数切上 */
#define fare_discount5(fare, per) ((((fare) / 10 * (10 - (per))) + 5) / 10 * 10)


#define CSTART	1
#define CEND	2

// DB 固定定義(DB変更のない前提のDB値定義)
//	速度考慮しSQLで取得しない
//
#define JR_HOKKAIDO	1
#define JR_EAST		2
#define JR_CENTRAL	3
#define JR_WEST		4
#define JR_KYUSYU	5
#define JR_SHIKOKU	6
#define JR_GROUP_MASK   ((1<<5)|(1<<4)|(1<<3)|(1<<2)|(1<<1)|(1<<0))
#define IS_JR_MAJOR_COMPANY(c)	((JR_EAST == c) || (JR_CENTRAL == c) || (JR_WEST == c))

#define CITYNO_TOKYO		1		// 東京都区内[区]
#define CITYNO_YOKOHAMA		2		// 横浜市内[浜]
#define CITYNO_NAGOYA		3		// 名古屋市内[名]
#define CITYNO_KYOUTO		4		// 京都市内[京]
#define CITYNO_OOSAKA		5		// 大阪市内[阪]
#define CITYNO_KOUBE		6		// 神戸市内[神]
#define CITYNO_HIROSIMA		7		// 広島市内[広]
#define CITYNO_KITAKYUSYU	8		// 北九州市内[九]
#define CITYNO_FUKUOKA		9		// 福岡市内[福]
#define CITYNO_SENDAI		10		// 仙台市内[仙]
#define CITYNO_SAPPORO		11		// 札幌市内[札]
#define CITYNO_YAMATE       12      // 山手線内[山]
#define CITYNO_SHINOOSAKA   13      // 大阪・新大阪

#define STATION_ID_AS_CITYNO         10000  // station_id or cityno

#define URB_TOKYO			1
#define URB_NIGATA			2
#define URB_OSAKA			3
#define URB_FUKUOKA			4
#define URB_SENDAI			5

#define FLAG_FARECALC_INITIAL		(1<<15)
#define MASK_CITYNO(flg)			((flg)&0x0f)

/* 近郊区間 */
#define MASK_URBAN					0x380
#define URBAN_ID(flg)				(((int32_t)(flg)>>7)&7)
#define IS_OSMSP(flg)				(((flg)&(1 << 11))!=0)	/* 大阪電車特定区間 ?*/
#define IS_TKMSP(flg)				(((flg)&(1 << 10))!=0)	/* 東京電車特定区間 ?*/
#define IS_YAMATE(flg)				(((flg)&(1 << 5))!=0)	/* 山点線内／大阪環状線内 ?*/

#define MASK_FARECALC_INITIAL       0

#if 0
  vector<RouteItem> 先頭のlineId
  0x01 発駅が都区市内
  0x02 着駅が都区市内


#endif

#define MASK_ROUTE_FLAG_LFLG	0xff000000
#define MASK_ROUTE_FLAG_SFLG	0x0000ffff

/* route_flag */
#define LASTFLG_OFF				MASK(BLF_NOTSAMEKOKURAHAKATASHINZAI)   // all bit clear at removeAll()

// 大阪環状線 通過制御フラグ定義
// ※ ここでいう「内回り」「外回り」は駅1-駅2間の進行方向ではなくDB定義上の
//    順廻り／大回りのことを指す
//                                    from  ex.
// 0) 初期状態
// 1) 近回り1回目 内回り                0 大阪→天王寺
// 2) 近回り1回目 外回り                0 天王寺→西九条
// 3) 近回り2回目 内回り - 内回り       1 今宮→大阪 … 京橋→天王寺
// 4) 近回り2回目 外回り - 内回り       2 西九条→天王寺 … 京橋→大阪
// 5) 近回り2回目 内回り - 外回り       1 大阪→京橋 … 天王寺→西九条
// 6) 近回り2回目 外回り - 外回り       2 N/A(ありえへん)
// 7) 遠回り指定 近回り1回目 内回り     1 大阪→天王寺 (1の時にUI指定した直後の状態)
// 8) 遠回り指定 近回り1回目 外回り     2 天王寺→西九条 (1の時にUI指定した直後の状態)
// 9) 遠回り指定 遠回り1回目 内回り     8 今宮→→京橋 (順廻りが遠回りの例)
// a) 遠回り指定 遠回り1回目 外回り     7 大阪→→天王寺(福島軽油)
// b) 遠回り指定 2回目 内回り - 内回り  9 今宮→→京橋 … 天王寺→京橋
// c) 遠回り指定 2回目 外回り - 内回り  a 西九条→→京橋 … 大阪→西九条
// d) 遠回り指定 2回目 内回り - 外回り  9 今宮→→京橋 … 天王寺→今宮
// e) 遠回り指定 2回目 外回り - 外回り  a ありえへん？
//
// 遠回り指定は、UIから。1,2,9,aのみ許可(7,8はあり得ない)
// 1,2は遠回りへ。 9,aは近回りへ


// bit0-1: 回数 2ビット 0～2
//        00=初期
//        01=1回目
//        10=2回目
//        11=n/a
// bit2: 1回目 内回り/外回り
// bit3: 2回目 内回り/外回り
// bit4: 遠回り指定


//        計算時は、bit 2 BLF_OSAKAKAN_DETOUR のみ使用し、
//		  計算の過程では、ローカル変数フラグ使用

#define JCTMASKSIZE   ((MAX_JCT + 7) / 8)

class RouteFlag {
    unsigned char osakaKanPass = 0;

public:
    bool no_rule;

    bool jrtokaistock_applied;  // owner is user
    bool jrtokaistock_enable;   // owner system b#20090901で未使用
    bool meihan_city_flag;      // True: 発のみ都区市内
    BYTE rule86or87;        // 0: N/A. bit0: term, bit1: end ([区][浜][名][京][阪][神][広][九][福][仙][札])
                            //         bit2: term, bit3: end([山])
                            // bit6:1= disable
    bool rule88;
    bool rule69;
    bool rule70;
    bool special_fare_enable;
    int8_t rule115;
    bool rule70bullet;
    bool rule16_5;

    bool bullet_line;           // 新幹線乗車している
    bool bJrTokaiOnly;

    bool meihan_city_enable;    //名阪発駅単駅着駅都区市内
    bool trackmarkctl;	        //5 次にremoveTailでlastItemの通過マスクをOffする(typeOでもPでもないので)
    bool jctsp_route_change;	//6 分岐特例(add内部使用)

    // bit 11-20 発着 都区市内 適用
    bool ter_begin_oosaka;      //21 大阪・新大阪
    bool ter_fin_oosaka	;      //22

    // 会社線
    bool compncheck	    ;      //23 会社線通過チェック有効
    bool compnpass		;      //24 通過連絡運輸
    bool compnda		;      //25 通過連絡運輸不正フラグ
    bool compnbegin		;      //26	会社線で開始
    bool compnend		;      //27 会社線で終了
    bool compnterm      ;      // 会社線通過連絡運輸発着駅チェック

    bool tokai_shinkansen;

    int8_t urban_neerest;   // 近郊区間内で最安経路算出可能(適用で計算して保持) owner is user/system both.
                            // 0: N/A
                            // 1: Neer , -1: Far

    // Global
    bool notsamekokurahakatashinzai;    //30 Route only : 小倉-博多間 新在別線扱い
    bool end		;               //31 arrive to end.
    // end of route_flag
public:
    RouteFlag() {
        clear();
    }
    void clear() {
        /* route_flag */
        // boolean LASTFLG_OFF = RouteUtil.MASK(notsamekokurahakatashinzai);   // all bit clear at removeAll()
        osakaKanPass = 0;      //0-1

        // bit 2-3
        osakakan_1dir = false;	//2 大阪環状線 1回目方向
        osakakan_2dir = false;	//3 大阪環状線 2回目方向

        osakakan_detour = false;   //4 大阪環状線 1回目遠回り(UI側から指定, 内部はR/O)

        trackmarkctl = false;	        //5 次にremoveTailでlastItemの通過マスクをOffする(typeOでもPでもないので)
        jctsp_route_change = false;	//6 分岐特例(add内部使用)

        ter_begin_oosaka	= false;      //21 大阪・新大阪
        ter_fin_oosaka		= false;      //22
        compncheck		    = false;      //23 会社線通過チェック有効
        compnpass			= false;      //24 通過連絡運輸
        compnda		    	= false;      //25 通過連絡運輸不正フラグ
        compnbegin			= false;      //26	会社線で開始
        compnend			= false;      //27 会社線で終了
        compnterm           = false;      // 通過連絡運輸有効無効発着駅のみチェック
    

        tokai_shinkansen = false;
        
        urban_neerest = 0;

        notsamekokurahakatashinzai = false;    //30 Route only : 小倉-博多間 新在別線扱い
        end			    = false;      //31 arrive to end.

        rule86or87 = 0;
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

        meihan_city_flag = false;	        //7 ON: APPLIED_START / OFF:APPLIED_TERMINAL(User->System)
        no_rule = false;                //8 ON: 特例非適用(User->System)

        jrtokaistock_enable= false;	    //10 提案可能フラグ b#20090901で未使用
        jrtokaistock_applied= false; 	//11 提案適用フラグ
    }
public:
    void setAnotherRouteFlag(const RouteFlag& other) {
        *this = other;
    }
    bool rule_en() {
        return (0x3f & rule86or87) ||
               rule88 ||
               rule69 ||
               rule70 ||
               special_fare_enable ||
               meihan_city_enable;
    }
    void       setNoRule(bool b_rule) { no_rule = b_rule; }
    
    // UI側からセットする制御フラグ
    bool isEnableLongRoute() const { return !no_rule && 0 != urban_neerest; }
    bool isLongRoute() const { return urban_neerest < 0; }
    void setLongRoute(bool farflag) {
		if (farflag) {
			urban_neerest = -1;
		}
		else {
			urban_neerest = 1;
		}
    }

    bool isEnableRule115() const { return !no_rule && 0 != rule115; }
    bool isRule115specificTerm() const { return rule115 < 0; }
    void setSpecificTermRule115(bool ena) {
		if (ena) {
			rule115 = -1;
		}
		else {
			rule115 = 1;
		}
    }
    void setStartAsCity() { ASSERT(meihan_city_enable); meihan_city_flag = true;    /* 着駅=単駅、発駅市内駅 */ }
    void setArriveAsCity()  { ASSERT(meihan_city_enable); meihan_city_flag = false; /* 発駅=単駅、着駅市内駅 */ }
//    void setJrTokaiStockApply(bool flag) { jrtokaistock_applied = flag; }
                                                                  /* clearRule()潰すと、株主有効が使えないので、こう(上)してみた */
                                                                  /* coreAreaIDByCityId() が影響 */
    void setDisableRule86or87() { rule86or87 |= 0x40; }
    void setEnableRule86or87()  { rule86or87 &= 0x3f; }
    bool isEnableRule86or87() const { return 0 == (rule86or87 & 0x40); }
    bool isAvailableRule86or87() const { return ((rule86or87 & 0x0f) != 0) && ((rule86or87 & 0x40) == 0); }
    bool isAvailableRule86() const { return (rule86or87 & 0x03) != 0; }
    bool isAvailableRule87() const { return (rule86or87 & 0x0c) != 0; }
    bool isAvailableRule88() const { return rule88; }
    bool isAvailableRule70() const { return rule70; }
    bool isAvailableRule69() const { return rule69; }
    bool isAvailableRule115() const { return 0 < rule115; }
    bool isAvailableRule16_5() const { return rule16_5; }

    //
    bool isMeihanCityEnable() const {
        return !no_rule && meihan_city_enable;
    }
    bool isArriveAsCity() const { return (meihan_city_enable == true) && (meihan_city_flag == false); }
    bool isStartAsCity() const { return (meihan_city_enable == true) && (meihan_city_flag == true); }

    // for debug print
    int getOsakaKanPassValue()  const { return osakaKanPass; }

    enum OSAKAKAN_PASS {
        OSAKAKAN_NOPASS,	// 初期状態(大阪環状線未通過)
        OSAKAKAN_1PASS,	    // 大阪環状線 1回通過
        OSAKAKAN_2PASS, 	// 大阪環状線 2回通過
   };

    bool is_osakakan_1pass() const {
        return OSAKAKAN_1PASS == (osakaKanPass & 0x03);
    }

    bool is_osakakan_2pass() const {
        return OSAKAKAN_2PASS == (osakaKanPass & 0x03);
    }

    bool is_osakakan_nopass() const {
        return OSAKAKAN_NOPASS == (osakaKanPass & 0x03);
    }

    void setOsakaKanPass(bool value) {
        if (value) {
            osakaKanPass |= (1 << 0);
        } else {
            osakaKanPass &= ~(1 << 0);
        }
    }
    bool getOsakaKanPass() const { return 0 != (osakaKanPass & (1 << 0)); }

    // bit 2-3
    bool osakakan_1dir = false;	//2 大阪環状線 1回目方向
    bool osakakan_2dir = false;	//3 大阪環状線 2回目方向

    bool osakakan_detour = false;   //4 大阪環状線 1回目遠回り(UI側から指定, 内部はR/O)

    void setOsakaKanFlag(unsigned char pass) {
        this->osakaKanPass = pass;
    }

    void setOsakaKanFlag(const RouteFlag& lf) {
        this->osakaKanPass = lf.osakaKanPass;
        this->osakakan_1dir = lf.osakakan_1dir;
        this->osakakan_2dir = lf.osakakan_2dir;
    }

    bool isRoundTrip() const {
		return !end || compnda;
	}

    void terCityReset() {
        rule86or87 &= 0x40;
        ter_begin_oosaka	= false;      //21 大阪・新大阪
        ter_fin_oosaka		= false;      //22
    }
    void optionFlagReset() {
        special_fare_enable = false;
        meihan_city_enable = false;
        rule88 = false;
        rule69 = false;
        rule70 = false;
        rule70bullet = false;
    }
    bool isTerCity() const {
        return
        (rule86or87 & 0x3f) ||
        ter_begin_oosaka	||      //21 大阪・新大阪
        ter_fin_oosaka		;      //22
    }

    // 特例非適用ならTrueを返す。route_flag.BLF_NO_RULEのコピー
    //
	bool isUseBullet() const { return bullet_line || rule70bullet; }

    // 会社線含んでいる場合Trueを返す
    bool isIncludeCompanyLine() const { return compncheck; }

    tstring showAppliedRule() const;
};

class RouteItem
{
	RouteItem() {}
public:
	IDENT lineId;
	IDENT stationId;
	SPECIFICFLAG flag;
//    unsigned int salesKm;        /* =0 is uninitialized. add by iPhone */
//    unsigned int fare;           /* =0 is uninitialized. add by iPhone */
	RouteItem(IDENT lineId_, IDENT stationId_, SPECIFICFLAG flag_);
	RouteItem(IDENT lineId_, IDENT stationId_);

	RouteItem(const RouteItem& item_) {
		lineId = item_.lineId;
		stationId = item_.stationId;
		flag = item_.flag;
//        salesKm = fare = 0;
	}
	RouteItem& operator=(const RouteItem& item_) {
		lineId = item_.lineId;
		stationId = item_.stationId;
		flag = item_.flag;
//        salesKm = fare = 0;
		return *this;
	}

	void refresh();
	bool operator==(const RouteItem& item_) const {
		return lineId == item_.lineId &&
			   stationId == item_.stationId /* &&
			   flag == item_.flag */;
	}
	bool is_equal(const RouteItem& item_) const {
		return lineId == item_.lineId &&
			   stationId == item_.stationId /* &&
			   flag == item_.flag */;
	}
};


class RouteList
{
protected:
//public:
    vector<RouteItem> route_list_raw;
    RouteFlag route_flag;	// add() - removeTail() work
public:
    RouteList() {}
    RouteList(const RouteList& route_list);
    void    assign(const RouteList& source_route, int32_t count = -1);
    virtual ~RouteList() { route_list_raw.clear(); }

    int32_t departureStationId() const {
        return (route_list_raw.size() <= 0) ? 0 : route_list_raw.front().stationId;
    }
	int32_t arriveStationId() const {
		return (route_list_raw.size() <= 0) ? 0 : route_list_raw.back().stationId;
	}
	const vector<RouteItem>& routeList() const { return route_list_raw; }
    RouteFlag getRouteFlag() const { return route_flag; }
    RouteFlag& refRouteFlag() { return route_flag; }

	RouteFlag sync_flag(const RouteList& source_route) { route_flag = source_route.getRouteFlag(); return route_flag; }

    bool isModified() const {
		return route_flag.jctsp_route_change;
	}
	bool isEnd() const {
		return route_flag.end;
	}
    tstring         route_script();

    bool 			checkPassStation(int32_t stationId);    /* not used current */

//	void            setFareOption(uint16_t cooked, uint16_t availbit);
    bool isAvailableReverse() const {
        int32_t c = (int32_t)route_list_raw.size();

        // Available reverse
        return (1 <= c) && (route_flag.isRoundTrip() ||
                (route_list_raw.front().stationId == route_list_raw.back().stationId));
                // Qの字の大阪環状線ではダメやねん
    }
    bool isRoundTrip() const { return route_flag.isRoundTrip(); }

//TODO	virtual uint32_t   getFareOption();
    virtual int32_t  coreAreaIDByCityId(int32_t startEndFlg) const { return 0;}
};

class IntPair
{
public:
    IntPair() { one = 0; two = 0; }
    IntPair(int one_, int two_) { one = one_; two = two_; }
    int one;
    int two;
};

class Fare {
public:
    int fare;
    int sales_km;
    int calc_km;
    Fare() { fare = sales_km = calc_km = 0; }
    Fare(int f, int sk, int ck) {
        set(f, sk, ck);
    }
    void set(int f, int sk, int ck) {
        fare = f;
        sales_km = sk;
        calc_km = ck;
    }
    void set(const Fare& other) {
        fare = other.fare;
        sales_km = other.sales_km;
        calc_km = other.calc_km;
    }
    void clear() { fare = sales_km = calc_km = 0; }
};

class Rule114Info {
    Fare fare_114;
    int32_t apply_terminal_station;
public:
    Rule114Info() {}
    Rule114Info(const Rule114Info& other) {
        set(other);
    }
    Rule114Info(const Fare& fare_, int32_t station_id_) {
        fare_114.set(fare_);
        apply_terminal_station = station_id_;
    }
    Rule114Info& operator=(const Rule114Info& right) {
        set(right);
        return *this;
    }
    void set(const Rule114Info& other) {
        fare_114.set(other.fare_114);
        apply_terminal_station = other.apply_terminal_station;
    }
    void clear() {
        fare_114.clear();
        apply_terminal_station = 0;
    }
    int32_t stationId() const { return apply_terminal_station; }
    int32_t sales_km() const { return fare_114.sales_km; }
    int32_t calc_km() const { return fare_114.calc_km; }
    int32_t fare() const { return fare_114.fare; }
};

class Route;

class FARE_INFO {
public:
	FARE_INFO() { reset(); FARE_INFO::tax = g_tax; }
private:
	int32_t sales_km;			//*** 有効日数計算用(会社線含む)

	int32_t base_sales_km;		//*** JR本州3社
	int32_t base_calc_km;		//***

	int32_t kyusyu_sales_km;	//***
	int32_t kyusyu_calc_km;		//***

	int32_t hokkaido_sales_km;
	int32_t hokkaido_calc_km;

	int32_t shikoku_sales_km;
	int32_t shikoku_calc_km;

	bool local_only;				/* True: 地方交通線のみ (0 < base_sales_km時のみ有効)*/
	bool local_only_as_hokkaido;	/* True: 北海道路線地方交通線のみ(0 < hokkaidou_sales_km時のみ有効) */
	//幹線のみ
	// (base_sales_km == base_calc_km) && (kyusyu_sales_km == kyusyu_calc_km) &&
	// (hokkaido_sales_km == hokkaido_calc_km) && (shikoku_sales_km == shikoku_calc_km)
	bool major_only;				/* 幹線のみ */
	int32_t total_jr_sales_km;			//***
	int32_t total_jr_calc_km;			//***

	int32_t company_fare;				/* 会社線料金 */
	int32_t company_fare_ac_discount;	/* 学割用会社線割引額 */
	int32_t company_fare_child;			/* 会社線小児運賃 */

    int32_t brt_fare;                   // BRT 運賃
    int32_t brt_sales_km;               // BRT 営業キロ
    int32_t brt_calc_km;                // BRT 計算キロ
#define BRT_DISCOUNT_FARE 100
    int32_t brt_discount_fare;          // BRT 乗り継ぎ割引価格 BRT_DISCOUNT_FARE

private:
	int32_t flag;						//***/* IDENT1: 全t_station.sflgの論理積 IDENT2: bit16-22: shinkansen ride mask  */
	int32_t jr_fare;					//***
	int32_t fare_ic;					//*** 0以外で有効
	int32_t avail_days;					//***
	static int32_t tax;					/* 消費税 */
	int32_t companymask;

    /* 114 */
    Rule114Info rule114Info;

    bool roundTripDiscount;

    tstring route_for_disp;
    tstring calc_route_for_disp;
    int32_t beginTerminalId;
    int32_t endTerminalId;

    int8_t enableTokaiStockSelect;  // 0: NoJR東海 1=JR東海株主可 2=JR東海のみ(Toica)

    class ResultFlag {
    public:
        bool comapany_first;		 /* 会社線から開始 */
        bool comapany_end;			 /* 会社線で終了  通常OFF-OFF, ON-ONは会社線のみ */
        bool company_incorrect;		 /* 会社線2社以上通過 */
        bool route_incomplete;	     /* 不完全経路(BSRNOTYET_NA) */
        bool route_empty;            /* empty */
        bool fatal_error;            /* fatal error in agggregate_fare_info() */
        ResultFlag() { clean(); }
        void clean() {
            comapany_first = false;		 /* 会社線から開始 */
            comapany_end = false;			 /* 会社線で終了  通常OFF-OFF, ON-ONは会社線のみ */
            company_incorrect = false;		 /* 会社線2社以上通過 */
            route_incomplete = false;	     /* 不完全経路(BSRNOTYET_NA) */
            route_empty = false;            /* empty */
            fatal_error = false;            /* fatal error in aggregate_fare_info() */
        }
    } result_flag;

	void retr_fare(bool useBullet);
    void calc_brt_fare(const vector<RouteItem>& routeList);
	int32_t aggregate_fare_info(RouteFlag *pRoute_flag, const vector<RouteItem>& routeList);
	int32_t aggregate_fare_jr(bool isbrt, int32_t company_id1, int32_t company_id2, const vector<int32_t>& distance);
	static void CheckIsBulletInUrbanOnSpecificTerm(const vector<RouteItem>& routeList, RouteFlag* pRoute_flag);
    int aggregate_fare_company(bool first_company,
                              const RouteFlag& rRoute_flag,
                              int32_t station_id_0,
                              int32_t station_id,
                              int32_t station_id1);
    bool reCalcFareForOptiomizeRoute(vector<RouteItem>* pShortRouteList,
                                     int32_t start_station_id,
                                     int32_t end_station_id,
                                     RouteFlag* pShort_route_flag,
                                     bool except_local = false);

public:
    void setTerminal(int32_t begin_station_id, int32_t end_station_id) {
        beginTerminalId = begin_station_id;
        endTerminalId = end_station_id;
    }
	bool calc_fare(RouteFlag* p_route_flag, const vector<RouteItem>& routeList);	//***
    bool reCalcFareForOptiomizeRouteForToiCa(const RouteList& route_list);
    bool reCalcFareForOptiomizeRoute(RouteList& route_list);
    RouteList reRouteForToica(const RouteList& route);
    void setRoute(const vector<RouteItem>& routeList, const RouteFlag& rRoute_flag);
    void setTOICACalcRoute(const vector<RouteItem>& routeList, const RouteFlag& rRoute_flag);
    void clrTOICACalcRoute() {calc_route_for_disp.clear();}
	void reset() {				//***
		companymask = 0;
		sales_km = 0;

		base_sales_km = 0;
		base_calc_km = 0;

		kyusyu_sales_km = 0;
		kyusyu_calc_km = 0;

		hokkaido_sales_km = 0;
		hokkaido_calc_km = 0;

		shikoku_sales_km = 0;
		shikoku_calc_km = 0;

		local_only = false;
		local_only_as_hokkaido = false;

		major_only = false;
		total_jr_sales_km = 0;
		total_jr_calc_km = 0;

		company_fare = 0;
		company_fare_ac_discount = 0;
		company_fare_child = 0;

        brt_fare = 0;                   // BRT 運賃
        brt_sales_km = 0;               // BRT 営業キロ
        brt_calc_km = 0;                // BRT 計算キロ
        brt_discount_fare = 0;          // BRT 乗り継ぎ割引価格 BRT_DISCOUNT_FARE

		flag = 0;
		jr_fare = 0;
		fare_ic = 0;
		avail_days = 0;

        rule114Info.clear();

        roundTripDiscount = false;

        //beginTerminalId = 0;          /* for Use isCityterminalWoTokai() / isNotCityterminalWoTokai() */
        //endTerminalId = 0;

		result_flag.clean();

        enableTokaiStockSelect = 0;

        route_for_disp.clear();
        calc_route_for_disp.clear();
	}
    void setIsRule16_5_route(RouteList& route_original);

    class FareResult {
    public:
        int fare;
        bool isDiscount;
        FareResult() { fare = 0; isDiscount = false; }
        FareResult(int fare_, bool isDiscount_) {
            fare = fare_;
            isDiscount = isDiscount_;
        }
    };
    void setEmpty() {
       result_flag.route_empty = true;
    }
    void setInComplete() {
       result_flag.route_incomplete = true;
    }
    bool isMultiCompanyLine() const {
        return result_flag.company_incorrect;
    }
    bool isBeginEndCompanyLine() const {
        return result_flag.comapany_first || result_flag.comapany_end;
    }
    int     resultCode() const {
        if (result_flag.route_incomplete) {
            return -1;
        } else if ((result_flag.route_empty) || (0 == (flag & FLAG_FARECALC_INITIAL))) {
            return -2;
        } else if (result_flag.fatal_error) {
            return -3;
        } else {
            return 0;
        }
    }

	// [名]以外の都区市内・山手線が発着のいずれかにあり
	bool isCityterminalWoTokai() const {
		return ((STATION_ID_AS_CITYNO < beginTerminalId) &&
             (CITYNO_NAGOYA != (beginTerminalId - STATION_ID_AS_CITYNO))) ||
            ((STATION_ID_AS_CITYNO < endTerminalId) &&
             (CITYNO_NAGOYA != (endTerminalId - STATION_ID_AS_CITYNO)));
	}

	// [名]か単駅のみ発着の場合
    bool isNotCityterminalWoTokai() const {
		return ((beginTerminalId < STATION_ID_AS_CITYNO) ||
             (CITYNO_NAGOYA == (beginTerminalId - STATION_ID_AS_CITYNO))) &&
            ((endTerminalId < STATION_ID_AS_CITYNO) ||
             (CITYNO_NAGOYA == (endTerminalId - STATION_ID_AS_CITYNO)));
	}
//    bool isEnableTokaiStockSelect() const {
//        return enableTokaiStockSelect == 1; // JR東海株主有効(品川から新幹線とか)
//    }
    bool isJrTokaiOnly() const {
        return enableTokaiStockSelect == 2; // JR東海TOICA有効
    }
    // 地方交通線を含んでいるか？
    bool didHaveLocalLine() const { return !local_only && total_jr_calc_km != total_jr_sales_km; }
    bool isLocalOnly() const { return local_only; }

	FareResult 	roundTripFareWithCompanyLine() const;
    int32_t 	roundTripFareWithCompanyLinePriorRule114() const;
    int32_t 	roundTripChildFareWithCompanyLine() const;
	int32_t 	getTotalSalesKm() const;
	int32_t		getRule114SalesKm() const { return rule114Info.sales_km(); }
	int32_t		getRule114CalcKm() const  { return rule114Info.calc_km();  }
    tstring     getRule114apply_terminal_station() const;
	int32_t		getJRSalesKm() const;
	int32_t		getJRCalcKm() const;
	int32_t		getCompanySalesKm() const;
	int32_t		getSalesKmForHokkaido() const;
	int32_t		getSalesKmForShikoku() const;
	int32_t		getSalesKmForKyusyu() const;
	int32_t		getCalcKmForHokkaido() const;
	int32_t		getCalcKmForShikoku() const;
	int32_t		getCalcKmForKyusyu() const;
	int32_t		getTicketAvailDays() const;
	int32_t		getFareForCompanyline() const;
	int32_t		getChildFareForDisplay() const;
	int32_t		getFareForJR() const;	/* 114判定用 */
	int32_t 	countOfFareStockDiscount() const;
	int32_t 	getFareStockDiscount(int32_t index, tstring& title, bool applied_r114 = false) const;
	int32_t     getStockDiscountCompany() const;
	int32_t		getAcademicDiscountFare() const;
	int32_t		roundTripAcademicFareWithCompanyLine() const;
	int32_t		getFareForDisplay() const;
    int32_t     getFareForDisplayPriorRule114() const;
	int32_t		getFareForIC() const;
    int32_t     getBRTSalesKm() const;
    int32_t     getFareForBRT() const;
    bool        getIsBRT_discount() const { return brt_discount_fare != 0; }
    bool isUrbanArea() const;

    void     setRule114(const Rule114Info& r114) {
        rule114Info.set(r114);
    }
    void     clrRule114() {
        rule114Info.clear();
    }
    class CompanyFare {
    public:
        int32_t fare;
        int32_t fareChild;
        int32_t fareAcademic;
        int32_t passflg;        // 弊算割引有無, 子供切り捨て
        /* t_clinfar */
        bool is_round_up_children_fare()	const { return (passflg & 0x01) != 0; }
        bool is_connect_non_discount_fare() const { return (passflg & 0x02) != 0; }
        CompanyFare() { fare = 0; fareChild = 0; fareAcademic = 0; passflg = 0; }
    };
    bool	isRule114() const { return 0 != rule114Info.fare(); }
    bool	isRoundTripDiscount() const { /* roundTripFareWithCompanyLine() を前に呼んでいること */ return roundTripDiscount; }
    int32_t getBeginTerminalId() const { return beginTerminalId;}
    int32_t getEndTerminalId() const { return endTerminalId; }
    tstring getRoute_string() const { return route_for_disp; }
    tstring getTOICACalcRoute_string() const { return calc_route_for_disp; }

    tstring 		showFare(const RouteFlag& routeFlag);

    static bool   IsCityId(int32_t id) { return STATION_ID_AS_CITYNO <= id; }
	static int32_t		Retrieve70Distance(int32_t station_id1, int32_t station_id2);
    static int32_t      CenterStationIdFromCityId(int32_t cityId);

private:
           int32_t      jrFare() const;
	static int32_t	 	Fare_basic_f(int32_t km);
	static int32_t	 	Fare_sub_f(int32_t km);
	static int32_t	 	Fare_tokyo_f(int32_t km);
	static int32_t	 	Fare_osaka(int32_t km);
	static int32_t	 	Fare_yamate_f(int32_t km);
	static int32_t	 	Fare_osakakan(int32_t km);
	static int32_t	 	Fare_hokkaido_basic(int32_t km);
	static int32_t	 	Fare_hokkaido_sub(int32_t km);
	static int32_t	 	Fare_shikoku(int32_t skm, int32_t ckm);
	static int32_t	 	Fare_kyusyu(int32_t skm, int32_t ckm);
	static int32_t		days_ticket(int32_t sales_km);
	static bool      	Fare_company(int32_t station_id1, int32_t station_id2, FARE_INFO::CompanyFare* companyFare);
	static int32_t		Fare_table(const char* tbl, const char* field, int32_t km);
	static int32_t		Fare_table(int32_t dkm, int32_t skm, char c);
	static int32_t		Fare_table(const char* tbl, char c, int32_t km);
	static int32_t		CheckSpecificFarePass(int32_t line_id, int32_t station_id1, int32_t station_id2);
	static int32_t		SpecificFareLine(int32_t station_id1, int32_t station_id2, int32_t kind);
	       vector<int32_t> getDistanceEx(int32_t line_id, int32_t station_id1, int32_t station_id2);
	static vector<int32_t> GetDistanceEx(const RouteFlag& osakakan_aggregate, int32_t line_id, int32_t station_id1, int32_t station_id2);
	static bool 		IsBulletInUrban(int32_t line_id, int32_t station_id1, int32_t station_id2, bool isRule88);

	static bool     IsIC_area(int32_t urban_id);

	static int32_t	CheckAndApplyRule43_2j(const vector<RouteItem> &route);
	static int32_t	CheckOfRule89j(const vector<RouteItem> &route);
    static std::vector<RouteItem> IsHachikoLineHaijima(const std::vector<RouteItem>& route_list);
    static std::vector<std::vector<int>> getBRTrecord(int32_t line_id);
}; // FARE_INFO

#define BCRULE70	            6		/* DB:lflag */


/****************/

// station_id & line_id
//
class Station
{
public:
	IDENT lineId;
	IDENT stationId;

	Station(){
		lineId = stationId = 0;
	}

	Station(IDENT lineId_, IDENT stationId_) {
		lineId = lineId_;
		stationId = stationId_;
	}

	Station(const Station& item_) {
		lineId = item_.lineId;
		stationId = item_.stationId;
	}
	Station& operator=(const Station& item_) {
		lineId = item_.lineId;
		stationId = item_.stationId;
		return *this;
	}

	void set(int32_t lineId_, int32_t stationId_) {
		lineId = lineId_;
		stationId = stationId_;
	}

	void set(const RouteItem& routeItem) {
		lineId = routeItem.lineId;
		stationId = routeItem.stationId;
	}

	bool operator==(const Station& item_) const {
		return lineId == item_.lineId &&
			   stationId == item_.stationId;
	}
	bool is_equal(const Station& item_) const {
		return lineId == item_.lineId &&
			   stationId == item_.stationId;
	}
	bool is_equal(const RouteItem& item_) const {
		return lineId == item_.lineId &&
			   stationId == item_.stationId;
	}
};

class Node
{
public:
	IDENT lineId;
	IDENT stationId1;
	IDENT stationId2;
	Node() { lineId = 0; stationId1 = 0; stationId2 = 0; }
};

class DbidOf
{
    DbidOf();
    map<tstring, int> retrieve_id_pool;
public:
    static DbidOf& getInstance() {
        static DbidOf obj;
        return obj;
    }
    int32_t id_of_station(tstring name);
    int32_t id_of_line(tstring name);
};

typedef struct
{
	int32_t		jctSpMainLineId;		// 分岐特例:本線(b)
	int32_t		jctSpStationId;			// 分岐特例:分岐駅(c)
	int32_t		jctSpMainLineId2;		// 分岐特例:本線(b)
	int32_t		jctSpStationId2;		// 分岐特例:分岐駅(c)
} JCTSP_DATA;

// 経路マスクビットパターンマスク
#define JctMaskOn(bit, jctid)  	bit[(jctid) / 8] |= (1 << ((jctid) % 8))
#define JctMaskOff(bit, jctid) 	bit[(jctid) / 8] &= ~(1 << ((jctid) % 8))
#define JctMaskClear(bit)   	memset(bit, 0, JCTMASKSIZE)
#define IsJctMask(bit, jctid)	((bit[(jctid) / 8] & (1 << ((jctid) % 8))) != 0)

#define TITLE_NOTSAMEKOKURAHAKATASHINZAI _T("(小倉博多間新幹線在来線別線)")

/*   route
 *
 */
/* All class method Class */
class RouteUtil
{
public:
    static bool     DbVer(DBsys* dbsys);

    static DBO 		Enum_company_prefect();
	static DBO 		Enum_station_match(LPCTSTR station);
	static DBO 		Enum_lines_from_company_prefect(int32_t id);
	static DBO		Enum_station_located_in_prefect_or_company_and_line(int32_t prefectOrCompanyId, int32_t lineId);
	static tstring 	GetKanaFromStationId(int32_t stationId);
	static DBO	 	Enum_line_of_stationId(int32_t stationId);

    static DBO	 	Enum_junction_of_lineId(int32_t lineId, int32_t stationId);
	static DBO	 	Enum_station_of_lineId(int32_t lineId);

    static int32_t 	GetStationId(LPCTSTR stationName);
	static int32_t 	GetLineId(LPCTSTR lineName);

    static tstring 	StationName(int32_t id);
	static tstring 	StationNameEx(int32_t id);

    static tstring 	LineName(int32_t id);
    static tstring  PrefectName(int32_t id);
    static tstring  CompanyName(int32_t id);

    static tstring  CoreAreaCenterName(int32_t id);

    static SPECIFICFLAG AttrOfStationId(int32_t id);
	static SPECIFICFLAG AttrOfStationOnLineLine(int32_t line_id, int32_t station_id);

    static tstring 	GetPrefectByStationId(int32_t stationId);

    static tstring  Show_route(const vector<RouteItem>& routeList, const RouteFlag& rRoute_flag);
    static tstring  Show_route_full(const vector<RouteItem>& routeList, const RouteFlag& rRoute_flag);
private:
    static tstring  RouteOsakaKanDir(int32_t station_id1, int32_t station_id2, const RouteFlag& rRoute_flag);
protected:
public:
    static int32_t  DirOsakaKanLine(int32_t station_id_a, int32_t station_id_b);
    static int32_t  CompanyIdFromStation(int32_t station_id);

	static vector<int32_t>	GetDistance(int32_t line_id, int32_t station_id1, int32_t station_id2);
	static vector<int32_t>	GetDistance(const RouteFlag&  oskkflg, int32_t line_id, int32_t station_id1, int32_t station_id2);
	static int32_t			GetDistanceOfOsakaKanjyouRvrs(int32_t line_id, int32_t station_id1, int32_t station_id2);

    enum LINE_DIR {
		LDIR_ASC  = 1,		// 下り
		LDIR_DESC = 2		// 上り
	};
	static int32_t  DirLine(int32_t line_id, int32_t station_id1, int32_t station_id2);

    static int32_t  GetHZLine(int32_t line_id, int32_t station_id, int32_t station_id2 = -1);
	static vector<uint32_t>  EnumHZLine(int32_t line_id, int32_t station_id, int32_t station_id2);

    static int32_t	NextShinkansenTransferTermInRange(int32_t line_id, int32_t station_id1, int32_t station_id2);
    static int32_t	NextShinkansenTransferTerm(int32_t line_id, int32_t station_id1, int32_t station_id2);

public: /* only user route */
    static vector<PAIRIDENT> GetNeerNode(int32_t station_id);
    static DBO	 	Enum_neer_node(int32_t stationId);
	static int32_t 	NumOfNeerNode(int32_t stationId);

    static int32_t	 	InStation(int32_t stationId, int32_t lineId, int32_t b_stationId, int32_t e_stationId);
    static vector<int32_t>  getIntersectOnLine(int32_t line_id, int32_t station_id1, int32_t station_id2, int32_t station_id3, int32_t station_id4);
    static bool  inlineOnline(int32_t line_id, int32_t station_id1, int32_t station_id2, int32_t station_id3, int32_t station_id4);
};


class Route : public RouteList
{
	BYTE jct_mask[JCTMASKSIZE];	// about 40byte
public:

public:
	Route();
    Route(const RouteList& route_list);
	virtual ~Route();

    void    assign(const RouteList& source_route, int32_t count = -1);
    int32_t	setup_route(LPCTSTR route_str);

protected:
    bool	chk_jctsb_b(int32_t kind, int32_t num);

private:
	class RoutePass
	{
	//friend class Route;
        BYTE   _jct_mask[JCTMASKSIZE];	// [JCTMASKSIZE] about 40byte
        const BYTE*   _source_jct_mask;
        int32_t _line_id;
        int32_t _station_id1;
        int32_t _station_id2;
        int32_t _start_station_id;
        int32_t _num;		        // number of junction
        bool    _err;
		RouteFlag _route_flag;	// add() - removeTail() work
		RoutePass(const RoutePass& rp) // copy constructor
		             { memcpy(this, &rp, sizeof(*this)); }
		void clear() { memset(_jct_mask, 0, JCTMASKSIZE); _err = 0; }
		void update(const RoutePass& rp)
		             { memcpy(this, &rp, sizeof(*this)); }
		RoutePass() { memset(this, 0, sizeof(*this)); } // default constructor
    public:
        RoutePass(const BYTE* jct_mask, const RouteFlag& rRoute_flag, int32_t line_id, int32_t station_id1, int32_t station_id2, int32_t start_station_id = 0);
        ~RoutePass() {  }
        int32_t check(bool is_no_station_id1_first_jct = false) const;
        void off(int32_t jid);
        void off(BYTE* jct_mask);
        void on(BYTE* jct_mask);
        void update_flag(RouteFlag* p_source_flg) {
            p_source_flg->setOsakaKanFlag(_route_flag);
        }
		int32_t num_of_junction() const { return _num; }

    private:
	    int32_t		enum_junctions_of_line();
	    int32_t		enum_junctions_of_line_for_oskk_rev();
	    int32_t     enum_junctions_of_line_for_osakakan();
		static int32_t 	InStationOnOsakaKanjyou(int32_t dir, int32_t start_station_id, int32_t station_id_a, int32_t station_id_b);
	};

private:
	void		    routePassOff(int32_t line_id, int32_t to_station_id, int32_t begin_station_id);
	static int32_t 	RetrieveJunctionSpecific(int32_t jctLineId, int32_t transferStationId, JCTSP_DATA* jctspdt);
	int32_t			getBsrjctSpType(int32_t line_id, int32_t station_id);
	int32_t			junctionStationExistsInRoute(int32_t stationId);
public:
    int32_t         typeOfPassedLine(int offset);
private:
    int32_t         reBuild();
public:
	int32_t         setDetour(bool enabled = true);
	void            setNoRule(bool no_rule);
    void            setNotSameKokuraHakataShinZai(bool enabled = true);
    bool            isNotSameKokuraHakataShinZai();

    //static tstring  Route_script(const vector<RouteItem>& routeList);

public:
	int32_t 		add(int32_t line_id, int32_t stationId2, int32_t ctlflg = 0);
	int32_t 		add(int32_t stationId);
	void 			removeTail(bool begin_off = false);
	void 			removeAll(bool bWithStart =true/* Dec.2017:Remove terminal, bool bWithEnd =true*/);
	int32_t			reverse();

	int32_t			changeNeerest(uint8_t useBulletTrain, int end_station_id);

protected:
	int32_t			companyPassCheck(int32_t line_id, int32_t stationId1, int32_t stationId2, int32_t num);
	static int32_t 	CompanyConnectCheck(int32_t station_id);
	int32_t			preCompanyPassCheck(int32_t line_id, int32_t stationId1, int32_t stationId2, int32_t num);
	int32_t			postCompanyPassCheck(int32_t line_id, int32_t stationId1, int32_t stationId2, int32_t num);
    int32_t         brtPassCheck(int32_t stationId2);

	class CompnpassSet
	{
		class recordset
		{
		public:
			int32_t line_id;
			int32_t stationId1;
			int32_t stationId2;
		} *results;
		const int max_record;
		int num_of_record;
        bool terminal;
	public:
		CompnpassSet() : max_record(MAX_COMPNPASSSET) {
			results = new recordset[max_record];
			num_of_record = 0;
            terminal = false;
		}
		~CompnpassSet() {
			delete[] results;
		}

		// 結果数を返す（0~N, -1 Error：レコード数がオーバー(あり得ないバグ)）
		int open(int32_t key1, int32_t key2);
		int check(int32_t postcheck_flag, int32_t line_id, int32_t station_id1, int32_t station_id2);
        bool is_terminal() const { return terminal; }
	protected:
		int en_line_id(int index) {
			return results[Limit(index, 0, MAX_COMPNPASSSET - 1)].line_id;
		}
		int en_station_id1(int index) {
			return results[Limit(index, 0, MAX_COMPNPASSSET - 1)].stationId1;
		}
		int en_station_id2(int index) {
			return results[Limit(index, 0, MAX_COMPNPASSSET - 1)].stationId2;
		}

	};

private:
	static vector<vector<int32_t>> Node_next(int32_t jctId, bool except_local);

private:
    static int32_t 	Id2jctId(int32_t stationId);
	static int32_t 	Jct2id(int32_t jctId);
	static tstring 	JctName(int32_t jctId);

	static int32_t	InStationOnLine(int32_t line_id, int32_t station_id, bool flag = false);

	static int32_t	LineIdFromStationId(int32_t station_id);
	static int32_t	LineIdFromStationId2(int32_t station_id1, int32_t station_id2);


    static bool		IsSameNode(int32_t line_id, int32_t station_id1, int32_t station_id2);
	static int32_t  NeerJunction(int32_t line_id, int32_t station_id1, int32_t station_id2);


private:
	static int32_t	Get_node_distance(int32_t line_id, int32_t station_id1, int32_t station_id2);
#if defined TEST || defined _DEBUG
public:
#endif
	static bool		IsAbreastShinkansen(int32_t line_id1, int32_t line_id2, int32_t station_id1, int32_t station_id2);
	static int		CheckTransferShinkansen(int32_t line_id1, int32_t line_id2, int32_t station_id1, int32_t station_id2, int32_t station_id3);

public:
};


class CalcRoute : public RouteList
{
    vector<RouteItem> route_list_cooked;
    Rule114Info rule114Info;
    CalcRoute() {
    }

public:
    CalcRoute(const RouteList& route);
    CalcRoute(const RouteList& route, int count);
	virtual ~CalcRoute() { route_list_cooked.clear(); }
    void sync(const RouteList& route);
    void sync(const RouteList& route, int count);

    const vector<RouteItem>& rawRouteList() { return route_list_raw; }
    const vector<RouteItem>& routeList() const { return route_list_cooked; }
    int32_t         beginStationId();
    int32_t         endStationId();
    void            checkOfRuleSpecificCoreLine(bool isCheckRule114 = false);
    int32_t            calcFare(FARE_INFO* pFi);
    int32_t            calcFare(FARE_INFO* pFi, int32_t count);
public:
	// alps_mfcDlg
    static tstring  BeginOrEndStationName(int32_t ident);
    int32_t  coreAreaIDByCityId(int32_t startEndFlg) const;
private:
    void   checkIsJRTokaiOnly(void);
	static int32_t	CheckOfRule88j(vector<RouteItem> *route);
public:
	static vector<int32_t>	Get_route_distance(const RouteFlag& rRoute_flag, const vector<RouteItem>& route);
private:
	static int32_t	ReRouteRule69j(const vector<RouteItem>& in_route_list, vector<RouteItem>* out_route_list);
	       int32_t	reRouteRule70j(const vector<RouteItem>& in_route_list, vector<RouteItem>* out_route_list);
           bool     isBulletInRouteOfRule70(int32_t station_id1, int32_t station_id2, int32_t stationId_o70, int32_t stationId_e70);
	static bool 	Query_a69list(int32_t line_id, int32_t station_id1, int32_t station_id2, vector<PAIRIDENT>* results, bool continue_flag);
	static bool 	Query_rule69t(const vector<RouteItem>& in_route_list, const RouteItem& cur, int32_t ident, vector<vector<PAIRIDENT>>* results);
	static uint32_t CheckOfRule86(const vector<RouteItem>& in_route_list, const RouteFlag& rRoute_flag, Station* exit, Station* entr, PAIRIDENT* cityId_pair);
	static uint32_t CheckOfRule87(const vector<RouteItem>& in_route_list);
	static void  	ReRouteRule86j87j(PAIRIDENT cityId, int32_t mode, const Station& exit, const Station& enter, vector<RouteItem>* out_route_list);
	static uint8_t  InRouteUrban(const vector<RouteItem>& route_list);
	static int32_t	RetrieveOut70Station(int32_t line_id);
	static int32_t	InCityStation(int32_t cityno, int32_t lineId, int32_t stationId1, int32_t stationId2);
	static vector<Station>	SpecificCoreAreaFirstTransferStationBy(int32_t lineId, int32_t cityId);
	static int32_t 	Retrieve_SpecificCoreStation(int32_t cityId);
    static int32_t  RemoveDupRoute(vector<RouteItem> *routeList);

#if defined TEST || defined _DEBUG
    public:
        void convertShinkansen2ZairaiFor114Judge() {
            CRule114::ConvertShinkansen2ZairaiFor114Judge(&route_list_raw);
        }
#endif
    class CRule114 {
        vector<RouteItem> route_list;
        vector<RouteItem> route_list_special;
        vector<RouteItem> route_list_replace;
        map<uint32_t, uint32_t> collectCheckedJunction;
        RouteFlag route_flag;
        bool is_start_city;  /* true : start is city otherwise arrive is city */
        bool is100km;        /* true : rule87 otherwise rule86 */
        int32_t deep_count;
        int32_t locost_fare;
        static vector<int32_t> ArrayOfLinesOfStationId(int32_t station_id);
        int32_t sales_km_special;
    public:
        Fare fare;
        int32_t  apply_terminal_station;
        int32_t  normal_fare;
    public:
        CRule114();
        bool check(const RouteFlag& rRouteFlag, uint32_t chk, uint32_t sk, 
                            const vector<RouteItem>& rRoute_list_no_applied_86or87, 
                            const vector<RouteItem>& rRoute_list_applied_86or87, 
                            const PAIRIDENT cityId, const Station& enter, const Station& exit);
            int32_t	retreive_SpecificCoreAvailablePoint(int32_t km, int32_t km_offset, int32_t line_id, int32_t station_id);
            vector<IntPair>  enumJunctionRange(int32_t cond_km, int32_t base_sales_km, int32_t base_line_id, int32_t base_station_id);
            void judgementOfFare(int32_t station_id, int32_t base_line_id, int32_t base_station_id);
            void get86or87firstPoint(int32_t km, uint32_t aSales_km, uint32_t line_id, uint32_t station_id1);
            bool checkOfRule114j(int32_t kind);
            static bool ConvertShinkansen2ZairaiFor114Judge(vector<RouteItem>* route);
        bool isEnable() { return fare.fare != 0; }
    };
};

#define STATION_ID(station_name) DbidOf::getInstance().id_of_station(station_name)
#define LINE_ID(line_name) DbidOf::getInstance().id_of_line(line_name)

#endif /* _cplusplus */

/**** public ****/

#define ADDRC_LAST  0   // add() return code
#define ADDRC_OK    1
#define ADDRC_END	5
#define ADDRC_CEND	4
// ADDRC_NG -1 to -N


#endif	/* _ALPDB_H__ */
