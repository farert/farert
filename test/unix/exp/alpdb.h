#if !defined _ALPDB_H__

#define _ALPDB_H__

typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef short int16_t;

extern int g_tax;	/* in alps_mfc.cpp */

#include <stdafx.h>
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

#define MAKEPAIR(ident1, ident2) ((PAIRIDENT)(0xffff & ident1) | ((PAIRIDENT)ident2 << 16))
#define IDENT1(ident) ((IDENT)(0xffff & ident))
#define IDENT2(ident) ((IDENT)(0xffff & ((PAIRIDENT)ident >> 16)))

#define FLG_HIDE_LINE					(1<<19)
#define FLG_HIDE_STATION				(1<<18)
#define IS_FLG_HIDE_LINE(lflg)			(0!=(lflg&FLG_HIDE_LINE))		// 路線非表示
#define IS_FLG_HIDE_STATION(lflg)		(0!=(lflg&FLG_HIDE_STATION))		// 駅非表示

#define BSRJCTSP		31		// 分岐特例
#define	BSRJCTHORD		31		// 水平型検知フラグ

#define BSRNOTYET_NA	30		// 不完全ルート
#define BSRJCTSP_B		29		// 分岐特例B

#define BSR69TERM		24
#define BSR69CONT		23
#define BSRJCT			15

#define JCTSP				 	1
#define JCTSP_2L			 	2
#define JCTSP_B_NISHIKOKURA 	3
#define JCTSP_B_YOSHIZUKA	 	4
#define JCTSP_B_NAGAOKA 		5

/* --------------------------------------- */
/* util */
#define NumOf(c)  (sizeof(c) / sizeof(c[0]))
#define Min(a, b) ((a)>(b)?(b):(a))
#define Max(a, b) ((a)>(b)?(a):(b))

#define MASK(bdef)	(1 << bdef)
#define BIT_CHK(flg, bdef) (0 != (flg & MASK(bdef)))
#define BIT_CHK2(flg, bdef1, bdef2) (0 != (flg & (MASK(bdef1)|MASK(bdef2))))
#define BIT_CHK3(flg, bdef1, bdef2, bdef3) (0 != (flg & (MASK(bdef1)|MASK(bdef2)|MASK(bdef3))))
#define BIT_ON(flg, bdef)  (flg |= MASK(bdef))
#define BIT_OFF(flg, bdef) (flg &= ~MASK(bdef))

#define HWORD_BIT	16		/* Number of bit in half word(unsigned short) */

#if !defined LOBYTE
#define LOBYTE(b) ((b)&0xff)
#endif
/* ---------------------------------------!!!!!!!!!!!!!!! */
#define MAX_STATION     4590
#define MAX_LINE        216
#define IS_SHINKANSEN_LINE(id)  ((0<(id))&&((id)<=15))	/* 新幹線は将来的にも10または15以内 !! */
#define IS_COMPANY_LINE_OF(id)     (209<(id))			/* !!!!!!!!!!!!! */
//#define MAX_JCT 311
/* ---------------------------------------!!!!!!!!!!!!!!! */

// 駅は分岐駅か
#define STATION_IS_JUNCTION(sid)	(0 != (Route::AttrOfStationId(sid) & (1<<12)))
//#define STATION_IS_JUNCTION_F(flg)	(0 != (flg & (1<<12)))	
// sflg.12は特例乗り換え駅もONなのでlflg.15にした
#define STATION_IS_JUNCTION_F(flg)		(0 != (flg & (1<<15)))

	/* ID for line_id on t_lines */
#define ID_L_RULE70		-10


// length define(UTF-8)
#define MAX_STATION_CHR	64		// 38
#define MAX_LINE_CHR	128
#define MAX_COMPANY_CHR	64		// 
#define MAX_PREFECT_CHR	16
#define MAX_COREAREA_CHR 26

#define MAX_NUM_JCT_STATION	103
#define MAX_JCT_NUM			6		// 東京駅乗り入れ路線数が最大で6
#define MAX_JCT				320		// 319

const LPCTSTR CLEAR_HISTORY = _T("(clear)");


/* 消費税(四捨五入)加算 */
#define taxadd(fare, tax)    (fare + ((fare * 1000 * tax / 100000) + 5) / 10 * 10)
#define taxadd_ic(fare, tax) (fare + (fare * 1000 * tax / 100000))

/* round up on 5 */
#define round(d) 		(((d) + 5) / 10 * 10)	/* 10円未満四捨五入 */ 
#define round_up(d)     (((d) + 9) / 10 * 10)	/* 10円未満切り上げ */
#define round_down(d)   ((d) / 10 * 10)			/* 10円未満切り捨て */

#define KM(kmx10) ((kmx10 + 9) / 10)	/* km単位で端数は切り上げ */

/* discount */
#define fare_discount(fare, per) ((fare) / 10 * (10 - (per)) / 10 * 10)

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

#define	LINE_TOHOKU_SINKANSEN	1	// 東北新幹線
#define	LINE_JYOETSU_SINKANSEN	2	// 上越新幹線
#define	LINE_HOKURIKU_SINKANSEN	3	// 北陸長野新幹線
#define	LINE_TOKAIDO_SINKANSEN	4	// 東海道新幹線
#define	LINE_SANYO_SINKANSEN	5	// 山陽新幹線
#define	LINE_KYUSYU_SINKANSEN	6	// 九州新幹線
#define	LINE_HOKKAIDO_SINKANSEN	7	// 北海道新幹線
#define	LINE_YAMAGATA_SINKANSEN	8	// 山形新幹線
#define	LINE_NAGASAKI_SINKANSEN	9	// 長崎新幹線


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

#define URB_TOKYO			1
#define URB_NIGATA			2
#define URB_OSAKA			3
#define URB_FUKUOKA			4
#define URB_SENDAI			5

#define FLAG_FARECALC_INITIAL		(1<<15)				
#define MASK_CITYNO(flg)			((flg)&0x0f)

/* 近郊区間 */
#define BCSUBURB					7
#define MASK_URBAN					0x380
#define URBAN_ID(flg)				(((int32_t)(flg)>>7)&7)
#define IS_OSMSP(flg)				(((flg)&(1 << 11))!=0)	/* 大阪電車特定区間 ?*/
#define IS_TKMSP(flg)				(((flg)&(1 << 10))!=0)	/* 東京電車特定区間 ?*/
#define IS_YAMATE(flg)				(((flg)&(1 << 5))!=0)	/* 山点線内／大阪環状線内 ?*/

#define IS_COMPANY_LINE(d)			(((d) & (1<<31)) != 0)

#define BCBULURB                    13	// FARE_INFO.flag: ONの場合大都市近郊区間特例無効(新幹線乗車している)

#if 0
  vector<RouteItem> 先頭のlineId
  0x01 発駅が都区市内
  0x02 着駅が都区市内


#endif

#define MASK_ROUTE_FLAG_LFLG	0xff000000
#define MASK_ROUTE_FLAG_SFLG	0x0000ffff

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

class FARE_INFO {
public:
	FARE_INFO() { reset(); if (FARE_INFO::tax <= 0) { FARE_INFO::tax = g_tax; }}
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
	int32_t flag;						//***/* IDENT1: 全t_station.sflgの論理積 IDENT2: bit16-22: shinkansen ride mask  */
	int32_t fare;						//***
	int32_t fare_ic;					//*** 0以外で有効
	int32_t avail_days;					//***
	static int32_t tax;					/* 消費税 */
	int32_t companymask;

    /* 114 */
    int32_t rule114_fare;
    int32_t rule114_sales_km;
    int32_t rule114_calc_km;
	
    bool roundTripDiscount;
    
    tstring route_for_disp;
    int32_t beginTerminalId;
    int32_t endTerminalId;

	bool retr_fare();
	int32_t aggregate_fare_info(int32_t line_id, int32_t station_id1, int32_t station_id2, int32_t before_staion_id1);
public:
	bool calc_fare(const vector<RouteItem>& routeList_raw, const vector<RouteItem>& routeList_cooked);	//***
    void setRoute(int32_t beginDtationId, int32_t endStationId, const vector<RouteItem>& routeList);
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
		flag = 0;
		fare = 0;
		fare_ic = 0;
		avail_days = 0;
        
        rule114_fare = 0;
        rule114_sales_km = 0;
        rule114_calc_km = 0;

        roundTripDiscount = false;
        
        beginTerminalId = 0;
        endTerminalId = 0;
        route_for_disp.clear();
	}
	int32_t 	roundTripFareWithComapnyLine(bool& return_discount) const;
	bool 	isUrbanArea() const;
	int32_t 	getTotalSalesKm() const;
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
	int32_t		getFareForJR() const;
	int32_t 	countOfFareStockDistount() const;
	int32_t 	getFareStockDistount(int32_t index, tstring& title) const;
	int32_t		getAcademicDiscount() const;
	int32_t		getFareForIC() const;
    bool     getRule114(int32_t* fare, int32_t* sales_km, int32_t* calc_km) const {
        *fare = rule114_fare;
        *sales_km = rule114_sales_km;
        *calc_km = rule114_calc_km;
        return rule114_fare != 0;
    }
    void     setRule114(int32_t fare, int32_t sales_km, int32_t calc_km) {
        rule114_fare = fare;
        rule114_sales_km = sales_km;
        rule114_calc_km = calc_km;
    }
    void     clrRule114() {
        rule114_fare = 0;
        rule114_sales_km = 0;
        rule114_calc_km = 0;
    }
    bool	isRoundTripDiscount() const { /* roundTripFareWithComapnyLine() を前に呼んでいること */ return roundTripDiscount; }
    int32_t getBeginTerminalId() const { return beginTerminalId;}
    int32_t getEndTerminalId() const { return endTerminalId; }
    tstring getRoute_string() const { return route_for_disp; }
private:
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
	static int32_t		Fare_company(int32_t station_id1, int32_t station_id2);
	static int32_t		Fare_table(const char* tbl, const char* field, int32_t km);
	static int32_t		Fare_table(int32_t dkm, int32_t skm, char c);
	static int32_t		Fare_table(const char* tbl, char c, int32_t km);
	static int32_t		CheckSpecficFarePass(int32_t line_id, int32_t station_id1, int32_t station_id2);
	static int32_t		SpecficFareLine(int32_t station_id1, int32_t station_id2);
	static vector<int32_t> GetDistanceEx(int32_t line_id, int32_t station_id1, int32_t station_id2);
	static int32_t		Retrieve70Distance(int32_t station_id1, int32_t station_id2);
	static bool 		IsBulletInUrban(int32_t line_id, int32_t station_id1, int32_t station_id2);

	static bool     IsIC_area(int32_t urban_id);
};

// 1レコードめのlineIdの定義
#define B1LID_MARK				0x8000		// line_idとの区別で負数になるように
#define B1LID_BEGIN_CITY		0
#define B1LID_FIN_CITY			1
#define B1LID_BEGIN_YAMATE		2
#define B1LID_FIN_YAMATE		3
#define B1LID_BEGIN_2CITY		4	// not used
#define B1LID_FIN_2CITY			5	// not used
#define B1LID_BEGIN_CITY_OFF	6
#define B1LID_FIN_CITY_OFF		7
#define B1LID_BEGIN_OOSAKA		8
#define B1LID_FIN_OOSAKA		9

#define BCRULE70	6

/* cooked flag for shoFare(), show_route() */
#define	RULE_NO_APPLIED			0x8000
#define	RULE_APPLIED			0
#define APPLIED_START			0x0010
#define APPLIED_TERMINAL		0x0000


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
public:
	DbidOf();
	static int32_t StationIdOf_SHINOSAKA;	// 新大阪
	static int32_t StationIdOf_OSAKA;    	// 大阪
	static int32_t StationIdOf_KOUBE;     	// 神戸
	static int32_t StationIdOf_HIMEJI;    	// 姫路
	static int32_t StationIdOf_NISHIAKASHI; // 西明石
	static int32_t LineIdOf_TOKAIDO;       	// 東海道線
	static int32_t LineIdOf_SANYO;        	// 山陽線
	static int32_t LineIdOf_SANYOSHINKANSEN; // 山陽新幹線
	static int32_t LineIdOf_HAKATAMINAMISEN; // 博多南線
	static int32_t LineIdOf_OOSAKAKANJYOUSEN; // 大阪環状線

	static int32_t StationIdOf_KITASHINCHI; 	// 北新地
	static int32_t StationIdOf_AMAGASAKI;		// 尼崎

	static int32_t StationIdOf_KOKURA;  		// 小倉
	static int32_t StationIdOf_NISHIKOKURA;  	// 西小倉
	static int32_t StationIdOf_HAKATA;  		// 博多
	static int32_t StationIdOf_YOSHIZUKA;	  	// 吉塚
};

typedef struct 
{
	int32_t		jctSpMainLineId;		// 分岐特例:本線(b)
	int32_t		jctSpStationId;			// 分岐特例:分岐駅(c)
	int32_t		jctSpMainLineId2;		// 分岐特例:本線(b)
	int32_t		jctSpStationId2;		// 分岐特例:分岐駅(c)
} JCTSP_DATA;

/* last_flag */
#define BLF_TRACKMARKCTL		5	// 次にremoveTailでlastItemの通過マスクをOffする(typeOでもPでもないので)
#define BLF_JCTSP_ROUTE_CHANGE	6
#define BLF_END					16
#define LASTFLG_OFF				0   // all bit clear at removeAll()

/*   route
 *
 */
class Route
{
	BYTE jct_mask[(MAX_JCT + 7)/ 8];	// about 40byte
	vector<RouteItem> route_list_raw;
	vector<RouteItem> route_list_cooked;
private:
    int32_t end_station_id;
	SPECIFICFLAG last_flag;	// add() - removeTail() work
public:
	int32_t startStationId() 
	{ return (route_list_raw.size() <= 0) ? 0 : route_list_raw.front().stationId; }

	int32_t endStationId() { return end_station_id; }
	void setEndStationId(int32_t stataion_id) { end_station_id = stataion_id; }

	enum LINE_DIR {
		LDIR_ASC  = 1,		// 下り
		LDIR_DESC = 2		// 上り
	};

public:
	Route();
	~Route();
    void assign(const Route& source_route, int32_t count);

	vector<RouteItem>& routeList() { return route_list_raw; }
	vector<RouteItem>& cookedRouteList() { return route_list_cooked; }
	bool isModified() {
		return (last_flag & (1 << BLF_JCTSP_ROUTE_CHANGE)) != 0;
	}
	void end()			{ BIT_ON(last_flag, BLF_END); }

private:
	bool				checkOfRuleSpecificCoreLine(int32_t dis_cityflag, int32_t* rule114);
	static DBO	 		Enum_junctions_of_line(int32_t line_id, int32_t begin_station_id, int32_t to_station_id);

private:
	static int32_t	 	InStation(int32_t stationId, int32_t lineId, int32_t b_stationId, int32_t e_stationId);
	static int32_t		RetrieveOut70Station(int32_t line_id);
	
	static vector<vector<int32_t>> Node_next(int32_t jctId);

	static int32_t	 	ReRouteRule69j(const vector<RouteItem>& in_route_list, vector<RouteItem>* out_route_list);
	static int32_t  	ReRouteRule70j(const vector<RouteItem>& in_route_list, vector<RouteItem>* out_route_list);
	static bool 	    Query_a69list(int32_t line_id, int32_t station_id1, int32_t station_id2, vector<PAIRIDENT>* results, bool continue_flag);
	static bool 	    Query_rule69t(const vector<RouteItem>& in_route_list, const RouteItem& cur, int32_t ident, vector<vector<PAIRIDENT>>* results);
	static uint32_t  	CheckOfRule86(const vector<RouteItem>& in_route_list, Station* exit, Station* entr, PAIRIDENT* cityId_pair);
	static uint32_t  	CheckOfRule87(const vector<RouteItem>& in_route_list);
	static uint8_t      InRouteUrban(const vector<RouteItem>& route_list);

	static void  		ReRouteRule86j87j(PAIRIDENT cityId, int32_t mode, const Station& exit, const Station& enter, vector<RouteItem>* out_route_list);

	static int32_t		InCityStation(int32_t cityno, int32_t lineId, int32_t stationId1, int32_t stationId2);

public:
	static int32_t 	Id2jctId(int32_t stationId);
	static int32_t 	Jct2id(int32_t jctId);
	static tstring 	JctName(int32_t jctId);
	static tstring 	StationName(int32_t id);
	static tstring 	StationNameEx(int32_t id);
	static tstring 	LineName(int32_t id);
    static tstring  CoreAreaCenterName(int32_t id);
    static int32_t  CoreAreaIDByCityId(int32_t startEndFlg, int32_t flg, SPECIFICFLAG flags);

	static SPECIFICFLAG AttrOfStationId(int32_t id);
	static SPECIFICFLAG AttrOfStationOnLineLine(int32_t line_id, int32_t station_id);
	static int32_t	InStationOnLine(int32_t line_id, int32_t station_id);

	static int32_t	LineIdFromStationId(int32_t station_id);
	static int32_t	LineIdFromStationId2(int32_t station_id1, int32_t station_id2);

	static int32_t 	GetStationId(LPCTSTR stationName);
	static int32_t 	GetLineId(LPCTSTR lineName);

    static tstring  PrefectName(int32_t id);
    static tstring  CompanyName(int32_t id);

public:	// termsel
	static DBO 		Enum_company_prefect();
	static DBO 		Enum_station_match(LPCTSTR station);
	static tstring 	GetPrefectByStationId(int32_t stationId);
	static DBO 		Enum_lines_from_company_prefect(int32_t id);
	static DBO		Enum_station_located_in_prefect_or_company_and_line(int32_t prefectOrCompanyId, int32_t lineId);
	static tstring 	GetKanaFromStationId(int32_t stationId);
	static DBO	 	Enum_line_of_stationId(int32_t stationId);

private:
	void		    routePassOff(int32_t line_id, int32_t to_station_id, int32_t begin_station_id);
	static int32_t 	RetrieveJunctionSpecific(int32_t jctLineId, int32_t transferStationId, JCTSP_DATA* jctspdt);
	int32_t			getBsrjctSpType(int32_t line_id, int32_t station_id);
	int32_t			junctionStationExistsInRoute(int32_t stationId);

public:
	// alps_mfcDlg
	tstring 		showFare(int32_t cooked);
    int32_t         calcFare(int32_t cooked, FARE_INFO* fare_info);
    int32_t         calcFare(int32_t cooked, int32_t count, FARE_INFO* fare_info);
	int32_t			fareCalcOption();
	tstring 		beginStationName(bool applied_agree);
	tstring 		endStationName(bool applied_agree);
	int32_t 		beginStationId(bool applied_agree);
	int32_t 		endStationId(bool applied_agree);
	
    static tstring  Show_route(const vector<RouteItem>& routeList);
	tstring         route_script();
    static tstring  Route_script(const vector<RouteItem>& routeList);

	int32_t			setup_route(LPCTSTR route_str);

	bool			chk_jctsb_b(int32_t kind, int32_t num);

	int32_t 		add(int32_t line_id, int32_t stationId2, int32_t ctlflg = 0);
	int32_t 		add(int32_t stationId);
	void 			removeTail(bool begin_off = false);
	void 			removeAll(bool bWithStart =true, bool bWithEnd =true);
	int32_t			reverse();
	bool 			checkPassStation(int32_t stationId);
	void 			terminate(int32_t stationId);
	static bool		IsSameNode(int32_t line_id, int32_t station_id1, int32_t station_id2);
	static int32_t  NeerJunction(int32_t line_id, int32_t station_id1, int32_t station_id2);
	int32_t			changeNeerest(bool useBulletTrain);

	static DBO	 	Enum_junction_of_lineId(int32_t lineId, int32_t stationId);
	static DBO	 	Enum_station_of_lineId(int32_t lineId);

	static DBO	 	Enum_neer_node(int32_t stationId);
	static int32_t 	NumOfNeerNode(int32_t stationId);
	static int32_t 	RemoveDupRoute(vector<RouteItem> *routeList);

	static int32_t  DirLine(int32_t line_id, int32_t station_id1, int32_t station_id2);
	static bool 	IsSpecificCoreDistance(const vector<RouteItem>& route);
	static vector<int32_t>		GetDistance(int32_t line_id, int32_t station_id1, int32_t station_id2);
	static int32_t				GetDistanceOfOsakaKanjyou(int32_t line_id, int32_t station_id1, int32_t station_id2, int32_t sales_km);
	static int32_t				Get_node_distance(int32_t line_id, int32_t station_id1, int32_t station_id2);
	static vector<int32_t>		Get_route_distance(const vector<RouteItem>& route);
	static vector<Station>	SpecificCoreAreaFirstTransferStationBy(int32_t lineId, int32_t cityId);
	static int32_t 	Retrieve_SpecificCoreStation(int32_t cityId);
	static int32_t	Retreive_SpecificCoreAvailablePoint(int32_t km, int32_t km_offset, int32_t line_id, int32_t station_id);
	static int32_t	CheckAndApplyRule43_2j(const vector<RouteItem> &route);
	static bool		CheckOfRule114j(const vector<RouteItem>& route, const vector<RouteItem>& routeSpecial, int32_t kind, int32_t* result);
	static int32_t	CheckOfRule88j(vector<RouteItem> *route);
	static int32_t	CheckOfRule89j(const vector<RouteItem> &route);

	static tstring  CoreAreaNameByCityId(int32_t startEndFlg, int32_t flg, SPECIFICFLAG flags);
	static bool		IsAbreastShinkansen(int32_t line_id1, int32_t line_id2, int32_t station_id1, int32_t station_id2);
	static int32_t  GetHZLine(int32_t line_id, int32_t station_id, int32_t station_id2 = -1);
	static bool		CheckTransferShinkansen(int32_t line_id1, int32_t line_id2, int32_t station_id1, int32_t station_id2, int32_t station_id3);
	static int32_t	NextShinkansenTransferTerm(int32_t line_id, int32_t station_id1, int32_t station_id2);

	static vector<PAIRIDENT> GetNeerNode(int32_t station_id);
};



#endif	/* _ALPDB_H__ */


