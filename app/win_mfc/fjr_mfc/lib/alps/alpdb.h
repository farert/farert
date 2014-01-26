#if !defined _ALPDB_H__

#define _ALPDB_H__

using namespace std;
#include <vector>

/*!	@file alpdb core logic implement.
 *	Copyright(c) sutezo9@me.com 2012.
 */
extern bool isKanaString(LPCTSTR szStr);
extern void conv_to_kana2hira(tstring& kana_str);

#define iskana(c) (_T('ƒ@') <= (c)) && ((c) <= _T('ƒ“'))

typedef short IDENT;
typedef unsigned int PAIRIDENT;
typedef unsigned int SPECIFICFLAG;

#define MAKEPAIR(ident1, ident2) ((PAIRIDENT)(0xffff & ident1) | ((PAIRIDENT)ident2 << 16))
#define IDENT1(ident) ((IDENT)(0xffff & ident))
#define IDENT2(ident) ((IDENT)(0xffff & ((PAIRIDENT)ident >> 16)))

#define FLG_HIDE_LINE					(1<<19)
#define FLG_HIDE_STATION				(1<<18)
#define IS_FLG_HIDE_LINE(lflg)			(0!=(lflg&FLG_HIDE_LINE))		// ˜Hü”ñ•\¦
#define IS_FLG_HIDE_STATION(lflg)		(0!=(lflg&FLG_HIDE_STATION))		// ‰w”ñ•\¦

#define BSRJCTSP		31		// •ªŠò“Á—á
#define	BSRJCTHORD		31		// …•½Œ^ŒŸ’mƒtƒ‰ƒO

#define BSRNOTYET_NA	30		// •sŠ®‘Sƒ‹[ƒg
#define BSRJCTSP_B		29		// •ªŠò“Á—áB

#define BSR69TERM		24
#define BSR69CONT		23
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

/* --------------------------------------- */
#define MAX_STATION     4590
#define MAX_LINE        209
#define IS_SHINKANSEN_LINE(id)  ((0<(id))&&((id)<=7))	/* VŠ²ü‚Í«—ˆ“I‚É‚à10‚Ü‚½‚Í15ˆÈ“à */
#define IS_COMPANY_LINE(id)     (202<(id))
//#define MAX_JCT 311

// ‰w‚Í•ªŠò‰w‚©
#define STATION_IS_JUNCTION(sid)	(0 != (Route::AttrOfStationId(sid) & (1<<12)))
#define STATION_IS_JUNCTION_F(flg)	(0 != (flg & (1<<12)))

	/* ID for line_id on t_lines */
#define ID_L_RULE70		-10


// length define(UTF-8)
#define MAX_STATION_CHR	32		// 38
#define MAX_LINE_CHR	32
#define MAX_COMPANY_CHR	32		// 
#define MAX_PREFECT_CHR	16

#define MAX_NUM_JCT_STATION	103
#define MAX_JCT_NUM			6		// “Œ‹‰wæ‚è“ü‚ê˜Hü”‚ªÅ‘å‚Å6
#define MAX_JCT				320		// 311

const LPCTSTR CLEAR_HISTORY = _T("(clear)");


#define _TAX	5	/* Á”ïÅ [%] */

/* Á”ïÅ(lÌŒÜ“ü)‰ÁZ */
#define taxadd(fare) (fare + ((fare * 1000 * _TAX / 100000) + 5) / 10 * 10)

#define KM(kmx10) ((kmx10 + 9) / 10)	/* km’PˆÊ‚Å’[”‚ÍØ‚èã‚° */

#define CSTART	1
#define CEND	2

// DB ŒÅ’è’è‹`(DB•ÏX‚Ì‚È‚¢‘O’ñ‚ÌDB’l’è‹`)
//	‘¬“xl—¶‚µSQL‚Åæ“¾‚µ‚È‚¢
//
#define JR_HOKKAIDO	1	
#define JR_EAST		2	
#define JR_CENTRAL	3	
#define JR_WEST		4	
#define JR_KYUSYU	5	
#define JR_SHIKOKU	6	


#define	LINE_TOHOKU_SINKANSEN	1	// “Œ–kVŠ²ü
#define	LINE_JYOETSU_SINKANSEN	2	// ã‰zVŠ²ü
#define	LINE_HOKURIKU_SINKANSEN	3	// –k—¤’·–ìVŠ²ü
#define	LINE_TOKAIDO_SINKANSEN	4	// “ŒŠC“¹VŠ²ü
#define	LINE_SANYO_SINKANSEN	5	// R—zVŠ²ü
#define	LINE_KYUSYU_SINKANSEN	6	// ‹ãBVŠ²ü
#define	LINE_HOKKAIDO_SINKANSEN	7	// –kŠC“¹VŠ²ü


#define CITYNO_TOKYO		1		// “Œ‹“s‹æ“à[‹æ]
#define CITYNO_YOKOHAMA		2		// ‰¡•ls“à[•l]
#define CITYNO_NAGOYA		3		// –¼ŒÃ‰®s“à[–¼]
#define CITYNO_KYOUTO		4		// ‹“ss“à[‹]
#define CITYNO_OOSAKA		5		// ‘åãs“à[ã]
#define CITYNO_KOUBE		6		// _ŒËs“à[_]
#define CITYNO_HIROSIMA		7		// L“‡s“à[L]
#define CITYNO_KITAKYUSYU	8		// –k‹ãBs“à[‹ã]
#define CITYNO_FUKUOKA		9		// •Ÿ‰ªs“à[•Ÿ]
#define CITYNO_SENDAI		10		// å‘äs“à[å]
#define CITYNO_SAPPORO		11		// D–ys“à[D]


#if 0
  vector<RouteItem> æ“ª‚ÌlineId
  0x01 ”­‰w‚ª“s‹æs“à
  0x02 ’…‰w‚ª“s‹æs“à


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
	RouteItem(IDENT lineId_, IDENT stationId_, SPECIFICFLAG flag_);
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

// ’n•ûŒğ’Êü‚Ì‚İ‚©A’n•ûŒğ’Êü¬‚¶‚è‚©‚ğ”»’f‚·‚é•K—v‚ª‚ ‚éB
// —¼Ò‚Ìˆá‚¢‚ÍH
class FARE_INFO {
public:
	int sales_km;			// —LŒø“ú”ŒvZ—p(‰ïĞüŠÜ‚Ş)

	int base_sales_km;		// JR–{B3Ğ
	int base_calc_km;

	int kyusyu_sales_km;
	int kyusyu_calc_km;

	int hokkaido_sales_km;
	int hokkaido_calc_km;

	int shikoku_sales_km;
	int shikoku_calc_km;

	bool local_only;				/* True: ’n•ûŒğ’Êü‚Ì‚İ (0 < base_sales_km‚Ì‚İ—LŒø)*/
	bool local_only_as_hokkaido;	/* True: –kŠC“¹˜Hü’n•ûŒğ’Êü‚Ì‚İ(0 < hokkaidou_sales_km‚Ì‚İ—LŒø) */
	//Š²ü‚Ì‚İ
	// (base_sales_km == base_calc_km) && (kyusyu_sales_km == kyusyu_calc_km) && 
	// (hokkaido_sales_km == hokkaido_calc_km) && (shikoku_sales_km == shikoku_calc_km)
	bool major_only;				/* Š²ü‚Ì‚İ */
	int total_jr_sales_km;
	int total_jr_calc_km;

	int company_fare;				/* ‰ïĞü—¿‹à */
	int flag;						/* IDENT1: ‘St_station.sflg‚Ì˜_—Ï IDENT2: bit16-22: shinkansen ride mask  */

#define FLAG_TOHOKU_SHINKANSEN		(1<<16)				// 1:“Œ–kVŠ²ü
#define FLAG_JYOETSU_SHINKANSEN		(2<<16)				// 2:ã‰zVŠ²ü
#define FLAG_HOKURIKU_SHINKANSEN	(3<<16)				// 3:–k—¤’·–ìVŠ²ü
#define FLAG_TOKAIDO_SHINKANSEN		(4<<16)				// 4:“ŒŠC“¹VŠ²ü
#define FLAG_SANYO_SHINKANSEN		(5<<16)				// 5:R—zVŠ²ü
#define FLAG_KYUSYU_SHINKANSEN		(6<<16)				// 6:‹ãBVŠ²ü
#define FLAG_HOKKAIDO_SHINKANSEN	(7<<16)				// 7:–kŠC“¹VŠ²ü
#define MASK_FLAG_SHINKANSEN(flg)	((flg)&0x000f0000)	// VŠ²ü	aggregate_fare_info()‚ÌÅŒã‚Åİ’è
#define MASK_CITYNO(flg)			((flg)&0x0f)
#define BCSUBURB					7
#define MASK_URBNNO(flg)			(((flg)>>7)&0x07)
#define IS_OSMSP(flg)				(((flg)&(1 << 11))!=0)	/* ‘åã“dÔ“Á’è‹æŠÔ ?*/
#define IS_TKMSP(flg)				(((flg)&(1 << 10))!=0)	/* “Œ‹“dÔ“Á’è‹æŠÔ ?*/
#define IS_YAMATE(flg)				(((flg)&(1 << 5))!=0)	/* R“_ü“à^‘åãŠÂóü“à ?*/
	int fare;
	int avail_days;

	FARE_INFO() { reset(); }
	bool retr_fare();
	bool aggregate_fare_info(int line_id, int station_id1, int station_id2);
	bool calc_fare(const vector<RouteItem>& routeList, bool applied_rule = true);
	void reset() {
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
	static int		Retrieve70Distance(int station_id1, int station_id2);
};

// 1ƒŒƒR[ƒh‚ß‚ÌlineId‚Ì’è‹`
#define B1LID_MARK				0x8000		// line_id‚Æ‚Ì‹æ•Ê‚Å•‰”‚É‚È‚é‚æ‚¤‚É
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

class DbidOf
{
public:
	DbidOf();
	static int StationIdOf_SHINOSAKA;	// V‘åã
	static int StationIdOf_OSAKA;    	// ‘åã
	static int StationIdOf_KOUBE;     	// _ŒË
	static int StationIdOf_HIMEJI;    	// •P˜H
	static int StationIdOf_NISHIAKASHI; // ¼–¾Î
	static int LineIdOf_TOKAIDO;       	// “ŒŠC“¹ü
	static int LineIdOf_SANYO;        	// R—zü
	static int LineIdOf_SANYOSHINKANSEN; // R—zVŠ²ü
	static int LineIdOf_HAKATAMINAMISEN; // ”‘½“ìü

	static int StationIdOf_KITASHINCHI; 	// –kV’n
	static int StationIdOf_AMAGASAKI;		// “òè

	static int StationIdOf_KOKURA;  		// ¬‘q
	static int StationIdOf_NISHIKOKURA;  	// ¼¬‘q
	static int StationIdOf_HAKATA;  		// ”‘½
	static int StationIdOf_YOSHIZUKA;	  	// ‹g’Ë
};

typedef struct 
{
	int		jctSpMainLineId;		// •ªŠò“Á—á:–{ü(b)
	int		jctSpStationId;			// •ªŠò“Á—á:•ªŠò‰w(c)
	int		jctSpMainLineId2;		// •ªŠò“Á—á:–{ü(b)
	int		jctSpStationId2;		// •ªŠò“Á—á:•ªŠò‰w(c)
} JCTSP_DATA;

class Route
{
	BYTE jct_mask[(MAX_JCT + 7)/ 8];	// about 40byte
	vector<RouteItem> route_list_raw;
	vector<RouteItem> route_list_cooked;
	FARE_INFO fare_info;
private:
	SPECIFICFLAG last_flag;	// add() - removeTail() work
public:
	int startStationId() 
	{ return (route_list_raw.size() <= 0) ? 0 : route_list_raw.front().stationId; }

	int endStationId;

	enum LINE_DIR {
		LDIR_ASC  = 1,		// ‰º‚è
		LDIR_DESC = 2		// ã‚è
	};

public:
	Route();
	~Route();

	const FARE_INFO& getCurFare() {
		return fare_info;
	}
	vector<RouteItem>& routeList() { return route_list_raw; }
	vector<RouteItem>& cookedRouteList() { return route_list_cooked; }
	bool isModified() {
		return (last_flag & (1 << 6)) != 0;
	}

private:
	bool			checkOfRuleSpecificCoreLine(int dis_cityflag, int* rule114);
	static DBO	 	Enum_junctions_of_line(int line_id, int begin_station_id, int to_station_id);

private:
	bool			aggregate_fare_info(int line_id, int station_id1, int station_id2);

	static int	 	InStation(int stationId, int lineId, int b_stationId, int e_stationId);
	static int		RetrieveOut70Station(int line_id);
	
	vector<vector<int>> Node_next(int jctId);

	static int	 	ReRouteRule69j(const vector<RouteItem>& in_route_list, vector<RouteItem>* out_route_list);
	static int  	ReRouteRule70j(const vector<RouteItem>& in_route_list, vector<RouteItem>* out_route_list);
	static bool 	Query_a69list(int line_id, int station_id1, int station_id2, vector<PAIRIDENT>* results, bool continue_flag);
	static bool 	Query_rule69t(const vector<RouteItem>& in_route_list, const RouteItem& cur, int ident, vector<vector<PAIRIDENT>>* results);
	static int  	CheckOfRule86(const vector<RouteItem>& in_route_list, Station* exit, Station* entr, PAIRIDENT* cityId_pair);
	static int  	CheckOfRule87(const vector<RouteItem>& in_route_list);

	static void  	ReRouteRule86j87j(PAIRIDENT cityId, int mode, const Station& exit, const Station& enter, vector<RouteItem>* out_route_list);

	static int		InCityStation(int cityno, int lineId, int stationId1, int stationId2);

public:
	static int 		Id2jctId(int stationId);
	static int 		Jct2id(int jctId);
	static tstring 	JctName(int jctId);
	static tstring 	StationName(int id);
	static tstring 	StationNameEx(int id);
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

private:
	void		routePassOff(int line_id, int to_station_id, int begin_station_id);
	static int 	retrieveJunctionSpecific(int jctLineId, int transferStationId, JCTSP_DATA* jctspdt);
	int			getBsrjctSpType(int line_id, int station_id);
	int			junctionStationExistsInRoute(int stationId);

public:
	// alps_mfcDlg
	tstring 		showFare(int cooked);
	int				fareCalcOption();
	
	tstring 		show_route(bool cooked);

	int				setup_route(LPCTSTR route_str);

	bool			chk_jctsb_b(int kind, int num);

	int 			add(int line_id, int stationId2, int ctlflg = 0);
	int 			add(int stationId);
	void 			removeTail(bool begin_off = false);
	void 			removeAll(bool bWithStart =true, bool bWithEnd =true);
	int				reverse();
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
	static int		CheckAndApplyRule43_2j(const vector<RouteItem> &route, FARE_INFO* fare_inf);
	static bool		CheckOfRule114j(const vector<RouteItem>& route, const vector<RouteItem>& routeSpecial, int kind, int* result);
	static int		CheckOfRule88j(vector<RouteItem> *route);
	static int		CheckOfRule89j(const vector<RouteItem> &route);

	static tstring  CoreAreaNameByCityId(int startEndFlg, int flg, SPECIFICFLAG flags);
	static bool		IsAbreastShinkansen(int line_id1, int line_id2, int station_id1, int station_id2);
	static int      GetHZLine(int line_id, int station_id, int station_id2 = -1);
	static bool		CheckTransferShinkansen(int line_id1, int line_id2, int station_id1, int station_id2, int station_id3);
	static int		NextShinkansenTransferTerm(int line_id, int station_id1, int station_id2);

	static vector<PAIRIDENT> GetNeerNode(int station_id);
};



#endif	/* _ALPDB_H__ */


