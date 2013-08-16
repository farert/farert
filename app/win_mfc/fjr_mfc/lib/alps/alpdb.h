#if !defined _ALPDB_H__

#define _ALPDB_H__

/*!	@file alpdb core logic implement.
 *	Copyright(c) sutezo9@me.com 2012.
 */
extern bool isKanaString(LPCTSTR szStr);
extern void conv_to_kana2hira(tstring& kana_str);

#define iskana(c) (_T('É@') <= (c)) && ((c) <= _T('Éì'))

typedef short IDENT;
typedef unsigned int PAIRIDENT;
typedef unsigned int SPECIFICFLAG;

#define isLflgEnable(lflg)	(0==((1<<30)&lflg))

#define MAKEPAIR(ident1, ident2) ((PAIRIDENT)(0xffff & ident1) | ((PAIRIDENT)ident2 << 16))
#define IDENT1(ident) ((IDENT)(0xffff & ident))
#define IDENT2(ident) ((IDENT)(0xffff & ((PAIRIDENT)ident >> 16)))

using namespace std;
#include <vector>


/* --------------------------------------- */
/* util */
#define NumOf(c)  (sizeof(c) / sizeof(c[0]))
#define Min(a, b) ((a)>(b)?(b):(a))
#define Max(a, b) ((a)>(b)?(a):(b))

#define MASK(bdef)	(1 << bdef)
#define BIT_CHK(flg, bdef) (0 != (flg & MASK(bdef)))

/* --------------------------------------- */
#define MAX_STATION     4590
#define MAX_LINE        209
#define IS_SHINKANSEN_LINE(id)  ((0<(id))&&((id)<=7))	/* êVä≤ê¸ÇÕè´óàìIÇ…Ç‡10Ç‹ÇΩÇÕ15à»ì‡ */
#define IS_COMPANY_LINE(id)     (202<(id))
//#define MAX_JCT 311


// length define(UTF-8)
#define MAX_STATION_CHR	32		// 38
#define MAX_LINE_CHR	32
#define MAX_COMPANY_CHR	32		// 
#define MAX_PREFECT_CHR	16

#define MAX_NUM_JCT_STATION	103
#define MAX_JCT_NUM			6		// ìåãûâwèÊÇËì¸ÇÍòHê¸êîÇ™ç≈ëÂÇ≈6
#define MAX_JCT				320		// 311

const LPCTSTR CLEAR_HISTORY = _T("(clear)");


#define CSTART	1
#define CEND	2

#define JR_HOKKAIDO	1	
#define JR_EAST		2	
#define JR_CENTRAL	3	
#define JR_WEST		4	
#define JR_KYUSYU	5	
#define JR_SHIKOKU	6	


#define	LINE_TOHOKU_SINKANSEN	1	// ìåñkêVä≤ê¸
#define	LINE_JYOETSU_SINKANSEN	2	// è„âzêVä≤ê¸
#define	LINE_HOKURIKU_SINKANSEN	3	// ñkó§í∑ñÏêVä≤ê¸
#define	LINE_TOKAIDO_SINKANSEN	4	// ìåäCìπêVä≤ê¸
#define	LINE_SANYO_SINKANSEN	5	// éRózêVä≤ê¸
#define	LINE_KYUSYU_SINKANSEN	6	// ã„èBêVä≤ê¸
#define	LINE_HOKKAIDO_SINKANSEN	7	// ñkäCìπêVä≤ê¸

#if 0
  vector<RouteItem> êÊì™ÇÃlineId
  0x01 î≠âwÇ™ìsãÊésì‡
  0x02 íÖâwÇ™ìsãÊésì‡


#endif


class RouteItem
{
	RouteItem() {}
public:
	IDENT lineId;
	IDENT stationId;
	SPECIFICFLAG flag;
	RouteItem(IDENT lineId_, IDENT stationId_, SPECIFICFLAG flag_) {
		lineId = lineId_;
		stationId = stationId_;
		flag = flag_;
	}
	RouteItem(IDENT lineId_, IDENT stationId_);

	RouteItem(const RouteItem& item_) {
		lineId = item_.lineId; 
		stationId = item_.stationId; 
		flag = item_.flag; 
	}
	RouteItem& operator=(const RouteItem& item_) { 
		lineId = item_.lineId; 
		stationId = item_.stationId; 
		flag = item_.flag; 
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

// ínï˚åí ê¸ÇÃÇ›Ç©ÅAínï˚åí ê¸ç¨Ç∂ÇËÇ©ÇîªífÇ∑ÇÈïKóvÇ™Ç†ÇÈÅB
// óºé“ÇÃà·Ç¢ÇÕÅH
class FARE_INFO {
public:
	int sales_km;			// óLå¯ì˙êîåvéZóp
	int calc_km;

	int base_sales_km;		// ñ{èB3é–
	int base_calc_km;

	int kyusyu_sales_km;
	int kyusyu_calc_km;

	int hokkaido_sales_km;
	int hokkaido_calc_km;

	int shikoku_sales_km;
	int shikoku_calc_km;

	bool local_only;				/* True: ínï˚åí ê¸ÇÃÇ› (0 < base_sales_kméûÇÃÇ›óLå¯)*/
	bool local_only_as_hokkaido;	/* True: ñkäCìπòHê¸ínï˚åí ê¸ÇÃÇ›(0 < hokkaidou_sales_kméûÇÃÇ›óLå¯) */
	//ä≤ê¸ÇÃÇ›
	// (base_sales_km == base_calc_km) && (kyusyu_sales_km == kyusyu_calc_km) && 
	// (hokkaido_sales_km == hokkaido_calc_km) && (shikoku_sales_km == shikoku_calc_km)
	bool major_only;				/* ä≤ê¸ÇÃÇ› */
	int total_jr_sales_km;
	int total_jr_calc_km;

	int company_fare;				/* âÔé–ê¸óøã‡ */
	int flag;						/* IDENT1: ëSt_station.sflgÇÃò_óùêœ IDENT2: bit16-22: shinkansen ride mask  */

#define MASK_TOHOKU_SHINKANSEN		(0x10000 << 0)	// 16:ìåñkêVä≤ê¸
#define MASK_JYOETSU_SHINKANSEN		(0x10000 << 1)	// 17:è„âzêVä≤ê¸
#define MASK_HOKURIKU_SHINKANSEN	(0x10000 << 2)	// 18:ñkó§í∑ñÏêVä≤ê¸
#define MASK_TOKAIDO_SHINKANSEN		(0x10000 << 3)	// 19:ìåäCìπêVä≤ê¸
#define MASK_SANYO_SHINKANSEN		(0x10000 << 4)	// 20:éRózêVä≤ê¸
#define MASK_KYUSYU_SHINKANSEN		(0x10000 << 5)	// 21:ã„èBêVä≤ê¸
#define MASK_HOKKAIDO_SHINKANSEN	(0x10000 << 6)	// 22:ñkäCìπêVä≤ê¸
#define MASK_FLAG_SHINKANSEN(flg)	((flg)&0x007f0000)	// êVä≤ê¸	aggregate_fare_info()ÇÃç≈å„Ç≈ê›íË
#define MASK_CITYNO(flg)			((flg)&0x0f)
#define BCSUBURB					7
#define MASK_URBNNO(flg)			(((flg)>>7)&0x07)
#define IS_OSMSP(flg)				(((flg)&(1 << 11))!=0)	/* ëÂç„ìdé‘ì¡íËãÊä‘ ?*/
#define IS_TKMSP(flg)				(((flg)&(1 << 10))!=0)	/* ìåãûìdé‘ì¡íËãÊä‘ ?*/
#define IS_YAMATE(flg)				(((flg)&(1 << 5))!=0)	/* éRì_ê¸ì‡Å^ëÂç„ä¬èÛê¸ì‡ ?*/
	int fare;
	int avail_days;

	FARE_INFO() { reset(); }
	bool retr_fare();
	bool aggregate_fare_info(int line_id, int station_id1, int station_id2);
	bool calc_fare(const vector<RouteItem>& routeList);
	void reset() {
		sales_km = 0;
		calc_km = 0;

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
		avail_days = 0;
	}
	static int	 	Fare_a(int km);
	static int	 	Fare_b(int km);
	static int	 	Fare_c(int km);
	static int	 	Fare_d(int km);
	static int	 	Fare_e(int km);
	static int	 	Fare_f(int km);
	static int	 	Fare_g(int km);
	static int	 	Fare_h(int km);
	static int		days_ticket(int sales_km);
	static int		Fare_company(int station_id1, int station_id2);
	static int		Fare_table(const char* tbl, const char* field, int km);
	static PAIRIDENT Fare_table(int dkm, int skm);
	static int		CheckSpecficFarePass(int line_id, int station_id1, int station_id2);
	static int		SpecficFareLine(int station_id1, int station_id2);
	static vector<int> GetDistanceEx(int line_id, int station_id1, int station_id2);
};

// 1ÉåÉRÅ[ÉhÇﬂÇÃlineIdÇÃíËã`
#define B1LID_BEGIN_CITY		0
#define B1LID_FIN_CITY			1
#define B1LID_BEGIN_YAMATE		2
#define B1LID_FIN_YAMATE		3
#define B1LID_BEGIN_2CITY		4	// not used
#define B1LID_FIN_2CITY			5	// not used
#define B1LID_BEGIN_CITY_OFF	6
#define B1LID_FIN_CITY_OFF		7

#define BSR70		24
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
	
	void set(int lineId_, int stationId_) {
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

class Route
{
	BYTE jct_mask[(MAX_JCT + 7)/ 8];	// about 40byte
	vector<RouteItem> route_list_raw;
	vector<RouteItem> route_list_cooked;
	FARE_INFO fare_info;
private:
	SPECIFICFLAG last_flag;	// add() - removeTail() work
public:
	int startStationId;
	int endStationId;

	static int StationIdOf_SHINOSAKA;	// êVëÂç„
	static int StationIdOf_OSAKA;    	// ëÂç„
	static int StationIdOf_KOUBE;     	// ê_åÀ
	static int StationIdOf_HIMEJI;    	// ïPòH
	static int LineIdOf_TOKAIDO;       	// ìåäCìπê¸
	static int LineIdOf_SANYO;        	// éRózê¸
	static int LineIdOf_SANYOSHINKANSEN; // éRózêVä≤ê¸

	static int StationIdOf_KITASHINCHI; // ñkêVín
	static int StationIdOf_AMAGASAKI;	// ìÚçË

	enum LINE_DIR {
		LDIR_ASC  = 0,
		LDIR_DESC = 1
	};

public:
	Route();
	~Route();

	const FARE_INFO& getCurFare() {
		return fare_info;
	}
	vector<RouteItem>& routeList() { return route_list_raw; }
	vector<RouteItem>& cookedRouteList() { return route_list_cooked; }

private:
	bool			checkOfRuleSpecificCoreLine(int dis_cityflag, int* rule114);
	static DBO	 	Enum_junctions_of_line(int line_id, int begin_station_id, int to_station_id);

private:
	bool			aggregate_fare_info(int line_id, int station_id1, int station_id2);

	static bool 	InStation(int stationId, int lineId, int b_stationId, int e_stationId);
	static bool 	Retrieve_70inout(int line_id1, int line_id2, Node* node);
	
	vector<vector<int>> Node_next(int jctId);

	static int	 	ReRouteRule69j(const vector<RouteItem>& in_route_list, vector<RouteItem>* out_route_list);
	static int  	ReRouteRule70j(const vector<RouteItem>& in_route_list, vector<RouteItem>* out_route_list);
	static bool 	Query_a69list(int line_id, int station_id1, int station_id2, vector<PAIRIDENT>* results, bool continue_flag);
	static bool 	Query_rule69t(const vector<RouteItem>& in_route_list, const RouteItem& cur, int ident, vector<vector<PAIRIDENT>>* results);
	static int  	CheckOfRule86(const vector<RouteItem>& in_route_list, Station* exit, Station* entr, PAIRIDENT* cityId_pair);
	static int  	CheckOfRule87(const vector<RouteItem>& in_route_list);

	static void  	ReRouteRule86j87j(PAIRIDENT cityId, int mode, const Station& exit, const Station& enter, vector<RouteItem>* out_route_list);

public:
	static int 		Id2jctId(int stationId);
	static int 		Jct2id(int jctId);
	static tstring 	JctName(int jctId);
	static tstring 	StationName(int id);
	static tstring 	LineName(int id);
	static SPECIFICFLAG AttrOfStationId(int id);
	static SPECIFICFLAG AttrOfStationOnLineLine(int line_id, int station_id);
	static int		InStationOnLine(int line_id, int station_id);

	static int		LineIdFromStationId(int station_id);

	static int 		GetStationId(LPCTSTR stationName);
	static int 		GetLineId(LPCTSTR lineName);


public:	// termsel
	static DBO 		Enum_company_prefect();
	static DBO 		Enum_station_match(LPCTSTR station);
	static tstring 	GetPrefectByStationId(int stationId);
	static DBO 		Enum_lines_from_company_prefect(int id);
	static DBO		Enum_station_located_in_prefect_or_company_and_line(int prefectOrCompanyId, int lineId);
	static tstring 	GetKanaFromStationId(int stationId);
	static DBO	 	Enum_line_of_stationId(int stationId);

	// alps_mfcDlg
	tstring 		showFare(int cooked);
	int				fareCalcOption();
	
	tstring 		show_route(bool cooked);

	int				setup_route(LPCTSTR route_str);

	int 			add(int line_id, int stationId1, int stationId2);
	void 			removeTail(bool begin_off = false);
	void 			removeAll();
	bool 			checkPassStation(int stationId);
	void 			terminate(int stationId);
	bool			changeNeerest(bool useBulletTrain);

	static DBO	 	Enum_junction_of_lineId(int lineId, int stationId);
	static DBO	 	Enum_station_of_lineId(int lineId);

	static DBO	 	Enum_neer_node(int stationId);
	static int 		NumOfNeerNode(int stationId);
	static int 		RemoveDupRoute(vector<RouteItem> *routeList);

	static int  	DirLine(int line_id, int station_id1, int station_id2);
	static bool 	IsSpecificCoreDistance(const vector<RouteItem>& route);
	static vector<int>		GetDistance(int line_id, int station_id1, int station_id2);
	static int				Get_node_distance(int line_id, int station_id1, int station_id2);
	static vector<int>		Get_route_distance(const vector<RouteItem>& route);
	static vector<Station>	SpecificCoreAreaFirstTransferStationBy(int lineId, int cityId);
	static int 		Retrieve_SpecificCoreStation(int cityId);
	static int		Retreive_SpecificCoreAvailablePoint(int km, int km_offset, int line_id, int station_id);
	static bool		CheckOfRule114j(const vector<RouteItem>& route, const vector<RouteItem>& routeSpecial, int kind, int* result);
	static int		CheckOfRule88j(vector<RouteItem> *route);
	static int		CheckOfRule89j(const vector<RouteItem> &route);

	static tstring  CoreAreaNameByCityId(int startEndFlg, int flg, SPECIFICFLAG flags);
	static int      GetHZLine(int line_id, int station_id);
	static bool		CheckTransferShinkansen(int line_id1, int line_id2, int station_id1, int station_id2, int station_id3);

	static vector<PAIRIDENT> GetNeerNode(int station_id);
};



#endif	/* _ALPDB_H__ */


