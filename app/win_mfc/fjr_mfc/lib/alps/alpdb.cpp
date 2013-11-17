#include "stdafx.h"

/*!	@file alpdb.cpp core logic implement.
 *	Copyright(c) sutezo9@me.com 2012.
 */
#if 0

#endif

using namespace std;
#include <vector>



// Œo˜Hƒ}ƒXƒNƒrƒbƒgƒpƒ^[ƒ“ƒ}ƒXƒN
#define JctMaskOn(jctid)  	jct_mask[(jctid) / 8] |= (1 << ((jctid) % 8))
#define JctMaskOff(jctid) 	jct_mask[(jctid) / 8] &= ~(1 << ((jctid) % 8))
#define JctMaskClear()   	memset(jct_mask, 0, sizeof(jct_mask))
#define IsJctMask(jctid)	((jct_mask[(jctid) / 8] & (1 << ((jctid) % 8))) != 0)


////////////////////////////////////////////
//	static member

/*static */ int DbidOf::StationIdOf_SHINOSAKA = 0;		// V‘åã
/*static */ int DbidOf::StationIdOf_OSAKA = 0;    		// ‘åã
/*static */ int DbidOf::StationIdOf_KOUBE = 0;     		// _ŒË
/*static */ int DbidOf::StationIdOf_HIMEJI = 0;    		// •P˜H
/*static */ int DbidOf::StationIdOf_NISHIAKASHI = 0;    // ¼–¾Î
/*static */ int DbidOf::LineIdOf_TOKAIDO = 0;       	// “ŒŠC“¹ü
/*static */ int DbidOf::LineIdOf_SANYO = 0;        		// R—zü
/*static */ int DbidOf::LineIdOf_SANYOSHINKANSEN = 0; 	// R—zVŠ²ü
/*static */ int DbidOf::LineIdOf_HAKATAMINAMISEN = 0; 	// ”‘½“ìü

/*static */ int DbidOf::StationIdOf_KITASHINCHI = 0;  	// –kV’n
/*static */ int DbidOf::StationIdOf_AMAGASAKI = 0;  	// “òè


////////////////////////////////////////////
//	DbidOf
//
DbidOf::DbidOf()
{
	if (!DbidOf::StationIdOf_SHINOSAKA) {	/* ‘Oˆ— ƒLƒƒƒbƒVƒ…ˆ— */
		DbidOf::StationIdOf_SHINOSAKA 	 = Route::GetStationId(_T("V‘åã"));
		DbidOf::StationIdOf_OSAKA 		 = Route::GetStationId(_T("‘åã"));
		DbidOf::StationIdOf_KOUBE 		 = Route::GetStationId(_T("_ŒË"));
		DbidOf::StationIdOf_HIMEJI 		 = Route::GetStationId(_T("•P˜H"));
		DbidOf::StationIdOf_NISHIAKASHI  = Route::GetStationId(_T("¼–¾Î"));
		DbidOf::LineIdOf_TOKAIDO 		 = Route::GetLineId(_T("“ŒŠC“¹ü"));
		DbidOf::LineIdOf_SANYO 			 = Route::GetLineId(_T("R—zü"));
		DbidOf::LineIdOf_SANYOSHINKANSEN = Route::GetLineId(_T("R—zVŠ²ü"));
		DbidOf::LineIdOf_HAKATAMINAMISEN = Route::GetLineId(_T("”‘½“ìü"));

		DbidOf::StationIdOf_KITASHINCHI = Route::GetStationId(_T("–kV’n"));
		DbidOf::StationIdOf_AMAGASAKI = Route::GetStationId(_T("“òè"));
	}
	ASSERT(0 < DbidOf::StationIdOf_SHINOSAKA);
	ASSERT(0 < DbidOf::StationIdOf_OSAKA);
	ASSERT(0 < DbidOf::StationIdOf_KOUBE);
	ASSERT(0 < DbidOf::StationIdOf_HIMEJI);
	ASSERT(0 < DbidOf::StationIdOf_NISHIAKASHI);
	ASSERT(0 < DbidOf::LineIdOf_TOKAIDO);
	ASSERT(0 < DbidOf::LineIdOf_SANYO);
	ASSERT(0 < DbidOf::LineIdOf_SANYOSHINKANSEN);
	ASSERT(0 < DbidOf::LineIdOf_HAKATAMINAMISEN);

	ASSERT(0 < DbidOf::StationIdOf_KITASHINCHI);
	ASSERT(0 < DbidOf::StationIdOf_AMAGASAKI);
}

////////////////////////////////////////////
//	RouteItem
//


// constructor
//
//	@param [in] lineId_    ˜Hü
//	@param [in] stationId_ ‰w
//
RouteItem::RouteItem(IDENT lineId_, IDENT stationId_)
{
	lineId = lineId_;
	stationId = stationId_;

	if (lineId <= 0) {
		flag = Route::AttrOfStationId((int)stationId_) & MASK_ROUTE_FLAG;
	} else {
		flag = Route::AttrOfStationOnLineLine((int)lineId_, (int)stationId_) & MASK_ROUTE_FLAG;
	}
}

//////////////////////////////////////////////////////

// 	•¶š—ñ‚ÍuŠ¿šv‚©u‚©‚Èv‚©H
//	u‚©‚Èv‚È‚çTrue‚ğ•Ô‚·
// 
// 	@param [in] szStr    •¶š—ñ
// 	@retval true  •¶š—ñ‚Í‚Ğ‚ç‚ª‚È‚Ü‚½‚ÍƒJƒ^ƒJƒi‚Ì‚İ‚Å‚ ‚é
// 	@retval false •¶š—ñ‚Í‚Ğ‚ç‚ª‚ÈAƒJƒ^ƒJƒiˆÈŠO‚Ì•¶š‚ªŠÜ‚Ü‚ê‚Ä‚¢‚é
// 
bool isKanaString(LPCTSTR szStr)
{
	LPCTSTR p = szStr;
	while (*p != _T('\0')) {
		if (((*p < _T('‚Ÿ')) || (_T('‚ñ') < *p)) && ((*p < _T('ƒ@')) || (_T('ƒ“') < *p))) {
			return false;
		}
		p++;
	}
	return p != szStr; // •¶š”0‚àƒ_ƒ
}

//	ƒJƒi‚ğ‚©‚È‚É
//	
//	@param [in][out] kana_str  •ÏŠ·•¶š—ñ
//
void conv_to_kana2hira(tstring& kana_str)
{
	tstring::iterator i = kana_str.begin();
	while (i != kana_str.end()) {
		if (iskana(*i)) {
			*i = *i - _T('ƒ@') + _T('‚Ÿ');
		}
		i++;
	}
}

//	3Œ…–ˆ‚ÉƒJƒ“ƒ}‚ğ•t‰Á‚µ‚½”’l•¶š—ñ‚ğì¬
//	(1/10‚µ‚½¬”“_•t‚«‰c‹ÆƒLƒ•\¦—p)
//
//	@param [in] num  ”’l
//	@return •ÏŠ·‚³‚ê‚½•¶š—ñ(ex. 43000 -> "4,300.0", 25793 -> "2,579.3")
//
static tstring num_str_km(int num)
{
	TCHAR cb[16];
	tstring s;
	TCHAR c;
	int ll;
	
	_sntprintf_s(cb, 16, _T("%u"), num / 10);
	s = cb;
	for (ll = s.size() - 3; 0 < ll; ll -= 3) {
		s.insert(ll, _T(","));
	}
	c = _T('0') + (num % 10);
	s += _T('.');
	s += c;
	return s;
}

//	3Œ…–ˆ‚ÉƒJƒ“ƒ}‚ğ•t‰Á‚µ‚½”’l•¶š—ñ‚ğì¬
//	‹àŠz•\¦—p
//
//	@param [in] num  ”’l
//	@return •ÏŠ·‚³‚ê‚½•¶š—ñ(ex. 43000 -> "\43,000", 3982003 -> "3,982,003")
//
static tstring num_str_yen(int num)
{
	TCHAR cb[16];
	tstring s;
	int ll;
	
	_sntprintf_s(cb, 16, _T("%u"), num);
	s = cb;
	for (ll = s.size() - 3; 0 < ll; ll -= 3) {
		s.insert(ll, _T(","));
	}
	return s;
}

////////////////////////////////////////////////////////////////////////
//
//	RouteƒNƒ‰ƒX ƒCƒ“ƒvƒŠƒƒ“ƒg
//


Route::Route()
{
	JctMaskClear();
	endStationId = 0;
	fare_info.reset();
}

Route::~Route()
{
}


	
// TimeSel.cpp
// ----------------------------------------------

// static
//	‰ïĞ&“s“¹•{Œ§‚Ì—ñ‹“
//
//	@return DBƒNƒGƒŠŒ‹‰ÊƒIƒuƒWƒFƒNƒg(‰ïĞ(JR‚Ì‚İ)A“s“¹•{Œ§ˆê——)
//
DBO Route::Enum_company_prefect()
{
	const static char tsql[] = 
"/**/select name, rowid from t_company where name like 'JR%'"
" union"
" select name, rowid*65536 from t_prefect order by rowid;";
	return DBS::getInstance()->compileSql(tsql, true);
}

// static
//	“s“¹•{Œ§or‰ïĞ‚É‘®‚·‚é˜Hü‚Ì—ñ‹“(JR‚Ì‚İ)
//
//	@return DBƒNƒGƒŠŒ‹‰ÊƒIƒuƒWƒFƒNƒg(˜Hü)
//
DBO Route::Enum_lines_from_company_prefect(int id)
{
	char sql[300];
	const char tsql[] = 
"/**/select n.name, line_id, lflg from t_line n"
" left join t_lines l on n.rowid=l.line_id"
" left join t_station t on t.rowid=l.station_id"
" where %s=%d"
" and (l.lflg&((1<<23)|(1<<31)|(1<<22)))=0"
" group by l.line_id order by n.name";

	int ident;

	if (0x10000 <= id) {
		ident = IDENT2(id);
	} else {
		ident = id;
	}
	sqlite3_snprintf(sizeof(sql), sql, tsql, 
	(0x10000 <= (PAIRIDENT)id) ? "prefect_id" : "company_id", ident);

	return DBS::getInstance()->compileSql(sql, true);
}


// static
//	‰w–¼‚Ìƒpƒ^[ƒ“ƒ}ƒbƒ`‚Ì—ñ‹“(‚Ğ‚ç‚ª‚ÈAƒJƒ^ƒJƒiA‚Ğ‚ç‚ª‚ÈƒJƒ^ƒJƒi¬‚¶‚èAŠ¿šj
//
//	@param [in] station   ‚¦‚«‚ß‚¢
//	@return DBƒNƒGƒŠŒ‹‰ÊƒIƒuƒWƒFƒNƒg(‰w–¼)
//
DBO Route::Enum_station_match(LPCTSTR station)
{
	char sql[256];
	const char tsql[] = "/**/select name, rowid, samename from t_station where (sflg&(1<<23))=0 and %s like '%q%%'";
	const char tsql_s[] = " and samename='%q'";

	tstring sameName;
	tstring stationName(station);		// WIN32 str to C++ string

	int pos = stationName.find('(');
	if (0 <= pos) {
		sameName = stationName.substr(pos);
		stationName = stationName.substr(0, pos);
		CT2A qsName(stationName.c_str(), CP_UTF8);
		CT2A qsSame(sameName.c_str(), CP_UTF8);

		sqlite3_snprintf(sizeof(sql), sql, tsql, 
								"name", qsName);
		sqlite3_snprintf(sizeof(sql) - lstrlenA(sql), sql + lstrlenA(sql), tsql_s,
								qsSame);
	} else {
		bool bKana;
		if (isKanaString(stationName.c_str())) {
			conv_to_kana2hira(stationName);
			bKana = true;
		} else {
			bKana = false;
		}
		CT2A qsName(stationName.c_str(), CP_UTF8);	// C++ string to UTF-8
		sqlite3_snprintf(sizeof(sql), sql, tsql, 
								bKana ? "kana" : "name", qsName);
	}
	return DBS::getInstance()->compileSql(sql, true);
}

// static
//	‰w(Id)‚Ì“s“¹•{Œ§‚ğ“¾‚é 
//
//	@param [in] stationId   ‰wident
//	@return “s“¹•{Œ§–¼
//
tstring Route::GetPrefectByStationId(int stationId)
{
	static const char tsql[] = 
//"select p.name from t_prefect p left join t_station t on t.prefect_id=p.rowid where t.rowid=?";
"select name from t_prefect where rowid=(select prefect_id from t_station where rowid=?)";
//‚±‚±‚Å‚Í‚Ç[‚Å‚à‚¢‚¢‚¯‚ÇAsqlite‚ÍŒ‹‡’x‚­‚ÄƒTƒuƒNƒGƒŠ‚Ì•û‚ª‘‚©‚Á‚½‚Ì‚Å‚±‚¤‚µ‚Ä‚İ‚½
	DBO dbo = DBS::getInstance()->compileSql(tsql);
	dbo.setParam(1, stationId);

	if (dbo.moveNext()) {
		return dbo.getText(0);
	} else {
		return _T("");
	}
}


// static
//	‰ïĞ or “s“¹•{Œ§ + ˜Hü‚Ì‰w‚Ì—ñ‹“
//
//	@param [in] prefectOrCompanyId  “s“¹•{Œ§ or ‰ïĞID~0x10000
//	@param [in] lineId              ˜Hü
//	@return DBƒNƒGƒŠŒ‹‰ÊƒIƒuƒWƒFƒNƒg(“s“¹•{Œ§A‰ïĞ)
//
DBO Route::Enum_station_located_in_prefect_or_company_and_line(int prefectOrCompanyId, int lineId)
{
	static const  char tsql[] =
//"select t.name, station_id from t_lines l left join t_station t on t.rowid=l.station_id "
//" where line_id=? and %s=? order by sales_km";
"/**/select t.name, station_id from t_station t left join t_lines l on t.rowid=l.station_id"
" where line_id=? and %s=? and (l.lflg&((1<<23)|(1<<31)|(1<<22)))=0 order by sales_km";

	char sql[256];
	int ident;

	if (0x10000 <= (PAIRIDENT)prefectOrCompanyId) {
		ident = IDENT2(prefectOrCompanyId);
	} else {
		ident = prefectOrCompanyId;
	}

	sqlite3_snprintf(sizeof(sql), sql, tsql,
	(0x10000 <= prefectOrCompanyId) ? "prefect_id" : "company_id");

	DBO dbo = DBS::getInstance()->compileSql(sql, true);

	dbo.setParam(1, lineId);
	dbo.setParam(2, ident);

	return dbo;
}

// static
//	‰w–¼‚Ì‚æ‚İ‚ğ“¾‚é
//
//	@param [in] stationId   ‰wident
//	@return ‚¦‚«‚ß‚¢
//
tstring Route::GetKanaFromStationId(int stationId)
{
	static const  char tsql[] =
"select kana from t_station where rowid=?";

	DBO dbo = DBS::getInstance()->compileSql(tsql);

	dbo.setParam(1, stationId);

	if (dbo.moveNext()) {
		return tstring(dbo.getText(0));
	} else {
		return tstring();
	}
}


// static
//	‰w‚ÌŠ‘®˜Hü‚ÌƒCƒeƒŒ[ƒ^‚ğ•Ô‚·
//
//	@param [in] stationId   ‰wident
//	@return DBƒNƒGƒŠŒ‹‰ÊƒIƒuƒWƒFƒNƒg(˜Hü)
//	@return field0(text):˜Hü–¼, field1(int):˜Hüid, field2(int):lflg(bit31‚Ì‚İ)
//
DBO Route::Enum_line_of_stationId(int stationId)
{
	static const  char tsql[] =
"select n.name, line_id, lflg from t_line n left join t_lines l on n.rowid=l.line_id"
//" where station_id=? and (lflg&((1<<31)|(1<<22)))=0 and sales_km>=0";
" where station_id=? and (lflg&(1<<22))=0";

	DBO dbo = DBS::getInstance()->compileSql(tsql, true);
	dbo.setParam(1, stationId);

	return dbo;
}



// alps_mfcDlg.cpp
// ----------------------------------------------

//static 
//	˜Hü‚Ì•ªŠò‰wˆê——ƒCƒeƒŒ[ƒ^‚ğ•Ô‚·
//	@param [in] lineId	˜HüId
//	@param [in] stationId ’…‰wId(‚±‚Ì‰w‚àˆê——‚ÉŠÜ‚ß‚é)
//	@return DBƒNƒGƒŠŒ‹‰ÊƒIƒuƒWƒFƒNƒg(•ªŠò‰w)
//
DBO Route::Enum_junction_of_lineId(int lineId, int stationId)
{
	static const  char tsql[] =
#if 1	// •ªŠò“Á—á‚Ì‚İ‚ÌæŠ·‰w(RŒ`‚Æ‚©)‚ğŠÜ‚Ş
"select t.name, station_id, sflg&(1<<12)"
" from t_lines l left join t_station t on t.rowid=l.station_id"
" where line_id=?1 and (lflg&(1<<22))=0 and"
" exists (select * from t_lines where line_id!=?1 and l.station_id=station_id)"
" or (line_id=?1 and (lflg&(1<<22))=0 and station_id=?2) order by l.sales_km";
#else
"select t.name, station_id, sflg&(1<<12)"
" from t_lines l left join t_station t on t.rowid=l.station_id"
" where line_id=?1 and ((lflg & (1<<12))<>0 or station_id=?2)"
//" and (lflg&((1<<31)|(1<<22)))=0 and sales_km>=0"
" and (lflg&(1<<22))=0"
" order by l.sales_km";
#endif


	DBO dbo = DBS::getInstance()->compileSql(tsql, true);
	dbo.setParam(1, lineId);
	dbo.setParam(2, stationId);

	return dbo;
}

//static 
//	˜Hü“à‚Ì‰wˆê——ƒCƒeƒŒ[ƒ^‚ğ•Ô‚·
//
//	@param [in] lineId   ‰wident
//	@return DBƒNƒGƒŠŒ‹‰ÊƒIƒuƒWƒFƒNƒg(‰w)
//
DBO Route::Enum_station_of_lineId(int lineId)
{
	static const  char tsql[] =
"select t.name, station_id, sflg&(1<<12)"
" from t_lines l left join t_station t on t.rowid=l.station_id"
" where line_id=?"
" and (lflg&((1<<31)|(1<<22)))=0"
" order by l.sales_km";

	DBO dbo = DBS::getInstance()->compileSql(tsql, true);
	dbo.setParam(1, lineId);

	return dbo;
}


//static
//	‰w‚Ì—×‚Ì•ªŠò‰w‚ğ•Ô‚·
//	(”ñ•ªŠò‰w‚ğw’è‚·‚é‚Æ³‚µ‚¢Œ‹‰Ê‚É‚È‚ç‚È‚¢)
//
//	@param [in] stationId   ‰wident
//	@return DBƒNƒGƒŠŒ‹‰ÊƒIƒuƒWƒFƒNƒg(—×Ú•ªŠò‰w)
//
DBO Route::Enum_neer_node(int stationId)
{
	static const  char tsql[] =
"select 	station_id , abs(("
"	select case when calc_km>0 then calc_km else sales_km end "
"	from t_lines "
"	where 0=(lflg&((1<<31)|(1<<22))) "
"	and line_id=(select line_id "
"				 from	t_lines "
"				 where	station_id=?1" 
"				 and	0=(lflg&((1<<31)|(1<<22)))) "
"	and station_id=?1)-case when calc_km>0 then calc_km else sales_km end) as cost"
" from 	t_lines "
" where 0=(lflg&((1<<31)|(1<<22)))"
" and	line_id=(select	line_id "
" 				 from	t_lines "
" 				 where	station_id=?1"
" 				 and	0=(lflg&((1<<31)|(1<<22))))"
" and	sales_km in ((select max(y.sales_km)"
"					  from	t_lines x left join t_lines y"
"					  on	x.line_id=y.line_id "
"					  where	0<=x.sales_km and 0=(x.lflg&((1<<31)|(1<<22)))"
"					  and	0<=y.sales_km and (1<<12)=(y.lflg&((1<<31)|(1<<22)|(1<<12)))"
"					  and	x.station_id=?1"
"					  and	x.sales_km>y.sales_km"
"					 ),"
"					 (select min(y.sales_km)"
"					  from	t_lines x left join t_lines y"
"					  on	x.line_id=y.line_id "
"					  where 0<=x.sales_km and 0=(x.lflg&((1<<31)|(1<<22)))"
"					  and	0<=y.sales_km and (1<<12)=(y.lflg&((1<<31)|(1<<22)|(1<<12)))"
"					  and	x.station_id=?1"
"					  and	x.sales_km<y.sales_km))";
	DBO dbo = DBS::getInstance()->compileSql(tsql, true);
	dbo.setParam(1, stationId);

	return dbo;
}

//static 
// ‰w‚Ì—×‚Ì•ªŠò‰w”‚ğ“¾‚é
//	(–Ó’°ü‚Ì‰w‚©”Û‚©‚Ì‚İ‚ğ•Ô‚·)
//
//	@param [in] stationId   ‰wident
//	@return 0 to 2 (—×Ú•ªŠò‰w”)
//
int Route::NumOfNeerNode(int stationId)
{
	if (STATION_IS_JUNCTION(stationId)) {
		return 2;	// 2ˆÈã‚ ‚é‚±‚Æ‚à‚ ‚é‚ª‰R‚Â‚«
	}
	DBO dbo =  Route::Enum_neer_node(stationId);
	int c;
	
	c = 0;
	while (dbo.moveNext()) {
		++c;
	}
	return c;
}

// ‰wID(•ªŠò‰w)‚ÌÅŠñ‚è‚Ì•ªŠò‰w‚ğ“¾‚é(‘S˜Hüj
// [jct_id, calc_km, line_id][N] = f(jct_id)
//
//	@param [in] jctId   •ªŠò‰w
//	@return •ªŠò‰w[0]AŒvZƒLƒ[1]A˜Hü[2]
//
vector<vector<int>> Route::Node_next(int jctId)
{
	const static char tsql[] = 
	"select case jct_id when ?1 then neer_id else jct_id end, cost, line_id"
	" from t_node"
	" where jct_id=?1 or neer_id=?1";
	
	vector<vector<int>> result;

	DBO dbo = DBS::getInstance()->compileSql(tsql, true);
	if (dbo.isvalid()) {
		dbo.setParam(1, jctId);
		
		while (dbo.moveNext()) {
			vector<int> r1;
			r1.push_back(dbo.getInt(0));	// jct_id
			r1.push_back(dbo.getInt(1));	// cost(calc_km)
			r1.push_back(dbo.getInt(2));	// line_id
			result.push_back(r1);
		}
	}
	return result;
}

//static 
//	˜Hü‚Ì‰wŠÔ‚É‰w‚Í‚ ‚é‚©H
//	lineId‚Ìb_stationId to e_stationId in stationId ?
//	’F lflg&(1<<22)‚ğŠÜ‚ß‚Ä‚¢‚È‚¢‚½‚ßAVŠ²ü“à•ªŠò‰wA‚½‚Æ‚¦‚ÎA
//	     “ŒŠC“¹VŠ²ü ‹“s •ÄŒ´ŠÔ‚É‘’Ã‰w‚Í‘¶İ‚·‚é‚Æ‚µ‚Ä•Ô‚µ‚Ü‚·.
//
//	@param [in] stationId   ŒŸõ‰w
//	@param [in] lineId      ˜Hü
//	@param [in] b_stationId ŠJn‰w
//	@param [in] e_stationId I—¹‰w
//	@return 0: not found / not 0: ocunt of found.
//
int Route::InStation(int stationId, int lineId, int b_stationId, int e_stationId)
{
	static const char tsql[] =
"select count(*)"
"	from t_lines"
"	where line_id=?1"
"	and station_id=?4"
"	and (lflg&(1<<31))=0"
"	and sales_km>="
"			(select min(sales_km)"
"			from t_lines"
"			where line_id=?1"
"			and (station_id=?2 or"
"				 station_id=?3))"
"	and sales_km<="
"			(select max(sales_km)"
"			from t_lines"
"			where line_id=?1"
"			and (station_id=?2 or "
"				 station_id=?3))";

	DBO dbo = DBS::getInstance()->compileSql(tsql, true);
	if (dbo.isvalid()) {
		dbo.setParam(1, lineId);
		dbo.setParam(2, b_stationId);
		dbo.setParam(3, e_stationId);
		dbo.setParam(4, stationId);

		if (dbo.moveNext()) {
			return dbo.getInt(0);
		}
	}
	return 0;
}

//static
//	˜Hü‚Ìbegin_station_id‰w‚©‚çto_station_id‰w‚Ü‚Å‚Ì•ªŠò‰wƒŠƒXƒg‚ğ•Ô‚·
//
//	’F lflg&(1<<22)‚ğŠÜ‚ß‚Ä‚¢‚È‚¢‚½‚ßAVŠ²ü“à•ªŠò‰wA‚½‚Æ‚¦‚ÎA
//	     “ŒŠC“¹VŠ²ü ‹“s •ÄŒ´ŠÔ‚É‘’Ã‰w‚Í‘¶İ‚·‚é‚Æ‚µ‚Ä•Ô‚µ‚Ü‚·.
//
//	@param [in] line_id          ˜Hü
//	@param [in] begin_station_id ”­
//	@param [in] to_station_id    Š
//
//	@return DBO& iterator:int •ªŠòID
//
DBO Route::Enum_junctions_of_line(int line_id, int begin_station_id, int to_station_id)
{
	static const char tsql[] = 
"select id from t_lines l join t_jct j on j.station_id=l.station_id where"
"	line_id=?1 and (lflg&((1<<31)|(1<<12)))=(1<<12)"
"	and sales_km>="
"			(select min(sales_km)"
"			from t_lines"
"			where line_id=?1"
"			and (station_id=?2 or"
"				 station_id=?3))"
"	and sales_km<="
"			(select max(sales_km)"
"			from t_lines"
"			where line_id=?1"
"			and (station_id=?2 or"
"				 station_id=?3))"
" order by"
" case when"
"	(select sales_km from t_lines where line_id=?1 and station_id=?3) <"
"	(select sales_km from t_lines where line_id=?1 and station_id=?2)"
" then sales_km"
" end desc,"
" case when"
"	(select sales_km from t_lines where line_id=?1 and station_id=?3) >"
"	(select sales_km from t_lines where line_id=?1 and station_id=?2)"
" then sales_km"
" end asc";

	DBO dbo = DBS::getInstance()->compileSql(tsql, true);
	if (dbo.isvalid()) {
		dbo.setParam(1, line_id);
		dbo.setParam(2, begin_station_id);
		dbo.setParam(3, to_station_id);
	}
	return dbo;
}

//static
//	70ği“ü˜HüA’Eo˜Hü‚©‚çi“üA’Eo‹«ŠE‰w‚Æ‰c‹ÆƒLƒA˜HüID‚ğ•Ô‚·
//
//	@param [in] line_id    ‘åŠÂóüi“ü^’Eo˜Hü
//	@return ˆê”ÔŠO‘¤‚Ì‘åŠÂóü“à(70ğ“K—p)‰w
//
//	@note “Œ‹“s‹æ“à‚È‚Ì‚ÅA‰c‹ÆƒLƒMAXŒÅ’è(‰º‚è‘¦‚¿“Œ‹‚©‚çˆê”Ô‰“‚¢70ğ“K—p‰w)‚Æ‚·‚é
//
int Route::RetrieveOut70Station(int line_id)
{
	static const char tsql[] =
"select station_id from t_lines where line_id=?1 and "
" sales_km=(select max(sales_km) from t_lines where line_id=?1 and (lflg&((1<<6)|(1<<31)))=(1<<6));";
#if 0
"select	t1.line_id,"
"	65535&t1.station_id,"
"	65535&t2.station_id"
//"	65535&t2.station_id,"
//"	(t1.lflg&65535),"
//"	(t2.lflg&65535),"
//"	t2.sales_km + t1.sales_km"
" from	t_lines t1 left join t_lines t2"
" on t1.line_id=t2.line_id and"
"	(t1.lflg&(1<<24))!=0 and"
"	(t2.lflg&(1<<24))!=0 and"
"	(t1.lflg&65535)<>(t2.lflg & 65535)"
" where"
"	(t1.lflg&65535)=?1 and"
"	(t2.lflg&65535)=?2";
#endif
	DBO dbo = DBS::getInstance()->compileSql(tsql, true);
	if (dbo.isvalid()) {
		dbo.setParam(1, line_id);
	}
	if (dbo.moveNext()) {
		return dbo.getInt(0);
	}
	return 0;
}

/*	Œo˜H’Ç‰Á
 *
 *	@param [in] line_id      ˜Hüident
 *	@param [in] stationId1   ‰w1 ident
 *	@param [in] stationId2   ‰w2 ident
 *
 *  @retval 0 = OK(last) +10‚Åƒ‹[ƒg•ÏX‚ ‚è
 *  @retval 1 = OK       +10‚Åƒ‹[ƒg•ÏX‚ ‚è
 *  //@retval 2 = OK(re-route)
 *  @retval -1 = overpass(•œæƒGƒ‰[)
 *  @retval -2 = Œo˜HƒGƒ‰[(stationId1 or stationId2‚Íline_id“à‚É‚È‚¢)
 *  @retval -3 = operation error(ŠJn‰w–¢İ’è)
 *  @retval -100 DB error
 *	@retval last_flag bit4-0 : reserve
 *	@retval last_flag bit5=1 : Ÿ‚ÉremoveTail‚ÅlastItem‚Ì’Ê‰ßƒ}ƒXƒN‚ğOff‚·‚é(typeO‚Å‚àP‚Å‚à‚È‚¢‚Ì‚Å)
 *	@retval last_flag bit5=0 : Ÿ‚ÉremoveTail‚ÅlastItem‚Ì’Ê‰ßƒ}ƒXƒN‚ğOff‚·‚é(typeO‚Å‚àP‚Å‚à‚È‚¢‚Ì‚Å)
 *	@retval last_flag bit6=1 : •ªŠò“Á—á‹æŠÔw’è‚É‚æ‚éŒo˜H•ÏX
 */
int Route::add(int stationId)
{
	removeAll(true, false);
	route_list_raw.push_back(RouteItem(0, stationId));
	TRACE(_T("add-begin %s(%d)\n"), Route::StationName(stationId).c_str(), stationId);
	return 1;
}

int Route::add(int line_id, int stationId2)
{
	int rc;
	int i;
	int j;
	int num;
	int jnum;
	int jct_on;
	int stationId1;
	SPECIFICFLAG lflg1;
	SPECIFICFLAG lflg2;
	int start_station_id;
	bool replace_flg = false;	// Œo˜H’Ç‰Á‚Å‚Í‚È‚­’uŠ·
	bool jct_flg_on = false;    // …•½Œ^ŒŸ’m(D-2)
	
	last_flag &= ~((1 << 5) | (1 << 6));

	num = route_list_raw.size();
	if (num <= 0) {
		return -3;
	}
	start_station_id = route_list_raw.front().stationId;
	stationId1 = route_list_raw.back().stationId;

	/* ”­‰w */
	lflg1 = Route::AttrOfStationOnLineLine(line_id, stationId1);
	if (!isLflgEnable(lflg1)) {
		return -2;		/* •s³Œo˜H(line_id‚ÉstationId1‚Í‘¶İ‚µ‚È‚¢) */
	}

	/* ’…‰w‚ª”­‰w`Å‰‚Ì•ªŠò‰wŠÔ‚É‚ ‚é‚©? */
	/* (’…‰w–¢w’è, ”­‰w=’…‰w‚Íœ‚­) */
	if ((num == 1) && (0 < endStationId) && (endStationId != start_station_id) && 
		(endStationId != stationId2) &&
			(0 != Route::InStation(endStationId, line_id, stationId1, stationId2))) {
		return -1;	/* <t> already passed error  */
	}

	lflg2 = Route::AttrOfStationOnLineLine(line_id, stationId2);
	if (!isLflgEnable(lflg2)) {
		return -2;		/* •s³Œo˜H(line_id‚ÉstationId2‚Í‘¶İ‚µ‚È‚¢) */
	}

	ASSERT(BIT_CHK(lflg1, BSRJCTSP) || route_list_raw.at(num - 1).stationId == stationId1);

	lflg2 = Route::AttrOfStationOnLineLine(line_id, stationId2);
	if (!isLflgEnable(lflg2)) {
		return -2;		/* •s³Œo˜H(line_id‚ÉstationId2‚Í‘¶İ‚µ‚È‚¢) */
	}
	/* VŠ²üİ—ˆü“¯ˆê‹‹æŠÔ‚Ìd•¡Œo˜Hƒ`ƒFƒbƒN(lastItem‚Ìflag‚ªBSRJCTSP=ON‚ªD-2ƒP[ƒX‚Å‚ ‚é */
	if ((1 < num) && !BIT_CHK(route_list_raw.at(num - 1).flag, BSRJCTSP) && 
		!Route::CheckTransferShinkansen(route_list_raw.at(num - 1).lineId, line_id,
										route_list_raw.at(num - 2).stationId, stationId1, stationId2)) {
		return -1;		// F-3b
	}

	TRACE(_T("add %s(%d)-%s(%d), %s(%d)\n"), Route::LineName(line_id).c_str(), line_id, Route::StationName(stationId1).c_str(), stationId1, Route::StationName(stationId2).c_str(), stationId2);
	
	// ’¼‘O“¯ˆê˜Hüw’è‚Íó‚¯•t‚¯‚È‚¢
	// ’¼‘O“¯ˆê‰w‚Íó‚¯•t‚¯‚È‚¢
	//if (!BIT_CHK(route_list_raw.back().flag, BSRJCTSP) && ((1 < num) && (line_id == route_list_raw.back().lineId))) {
	//	TRACE(_T("iregal parameter by same line_id.\n"));
	//	return -1;		// E-2, G, G-3, f, j1,j2,j3,j4 >>>>>>>>>>>>>>>>>>
	//}
	if (stationId1 == stationId2) {
		TRACE(_T("iregal parameter by same station_id.\n"));
		TRACE(_T("add_abort\n"));
		return -1;		// E-112 >>>>>>>>>>>>>>>>>>
	}
	
	// …•½Œ^ŒŸ’m
	if (BIT_CHK(route_list_raw.at(num - 1).flag, BSRJCTSP)) {
		TRACE("JCT: h_detect 2 (J, B, D)\n");
		if (Route::IsAbreastShinkansen(route_list_raw.at(num - 1).lineId, line_id, stationId1, stationId2)) {
			// 	line_id‚ÍVŠ²ü
			//	route_list_raw.at(num - 1).lineId‚Í•Àsİ—ˆü
			// 
			ASSERT(IS_SHINKANSEN_LINE(line_id));
			if (0 != Route::InStation(route_list_raw.at(num - 2).stationId, line_id, stationId1, stationId2)) {
				TRACE("JCT: D-2\n");
				i = route_list_raw.at(num - 1).lineId;	// •Àsİ—ˆü
				j = Route::NextShinkansenTransferTerm(line_id, stationId1, stationId2);
				if (j <= 0) {	// —×‰w‚ª‚È‚¢ê‡
					// VŠ²ü‚Ì”­‰w‚É‚Í•Àsİ—ˆü(˜Hüb)‚ÉŠ‘®‚µ‚Ä‚¢‚é‚©?
					if (0 == Route::InStationOnLine(jctSpMainLineId, stationId2)) {
						TRACE(_T("prev station is not found in shinkansen.\n"));
						TRACE(_T("add_abort\n"));
						return -1;			// >>>>>>>>>>>>>>>>>>>
					} else {
						removeTail();
						stationId1 = route_list_raw.back().stationId;
						line_id = jctSpMainLineId;
					}
				} else {
					removeTail();
					rc = add(i, j);		//****************
					ASSERT(rc == 1);
					stationId1 = j;
				}
				BIT_ON(last_flag, 6);
			} else {
				TRACE("JCT: B-2\n");
			}
		} else {
			TRACE("JCT: J\n");
		}
	}
	
	// 151 check
	while (BIT_CHK(lflg1, BSRJCTSP)) {
		// ’i·Œ^
		if (BIT_CHK(lflg2, BSRJCTSP)) {	// …•½Œ^‚Å‚à‚ ‚é?
			// retrieve from a, d to b, c 
			retrieveJunctionSpecific(line_id, stationId2); // update jctSpMainLineId(b), jctSpStation(c)
			if (jctSpStationId2 != 0) {
				BIT_OFF(lflg1, BSRJCTSP);				// •Ê‚É—v‚ç‚È‚¢‚¯‚Ç
				break;
			}
		}
		// retrieve from a, d to b, c 
		retrieveJunctionSpecific(line_id, stationId1); // update jctSpMainLineId(b), jctSpStation(c)
		if (stationId2 != jctSpStationId) {
			if (route_list_raw.at(num - 1).lineId == jctSpMainLineId) {
				ASSERT(stationId1 == route_list_raw.at(num - 1).stationId);
				if (0 < Route::InStation(jctSpStationId, route_list_raw.at(num - 1).lineId, route_list_raw.at(num - 2).stationId, stationId1)) {
					TRACE("JCT: C-1\n");
					routePassOff(jctSpMainLineId, jctSpStationId, stationId1);	// C-1
				} else { // A-1
					TRACE("JCT: A-1\n");
				}
				if ((2 <= num) && (jctSpStationId == route_list_raw.at(num - 2).stationId)) {
					removeTail();
					TRACE(_T("JCT: A-C\n"));		// 3, 4, 8, 9, g,h
					--num;
				} else {
					route_list_raw.at(num - 1).stationId = jctSpStationId;
					route_list_raw.at(num - 1).flag = Route::AttrOfStationOnLineLine(route_list_raw.at(num - 1).lineId, jctSpStationId) & MASK_ROUTE_FLAG;
				}
				if (jctSpStationId2 != 0) {		// •ªŠò“Á—á˜Hü2
					rc = add(jctSpMainLineId2, jctSpStationId2);	//**************
					ASSERT(rc == 1);
					num++;
					if (rc != 1) {			// safety
						BIT_ON(last_flag, 6);
						TRACE(_T("A-1/C-1(special junction 2)\n"));
						TRACE(_T("add_abort\n"));
						return -1;
					}
					if (stationId2 == jctSpStationId2) {
						TRACE(_T("KF1,2)\n"));
						line_id = jctSpMainLineId2;
						replace_flg = true;
					}
					stationId1 = jctSpStationId2;
				} else {
					stationId1 = jctSpStationId;
				}
			} else {
				if ((num < 2) || 
				!Route::IsAbreastShinkansen(jctSpMainLineId, route_list_raw.at(num - 1).lineId, stationId1, route_list_raw.at(num - 2).stationId)
				|| (Route::InStation(jctSpStationId, route_list_raw.at(num - 1).lineId, route_list_raw.at(num - 2).stationId, stationId1) <= 0)) {
					// A-0, I, A-2
					TRACE("JCT: A-0, I, A-2\n");	//***************
					rc = add(jctSpMainLineId, /*route_list_raw.at(num - 1).stationId,*/ jctSpStationId);
					ASSERT(rc == 1);
					num++;
					if (rc != 1) {				// safety
						BIT_ON(last_flag, 6);
						TRACE(_T("A-0, I, A-2\n"));
						TRACE(_T("add_abort\n"));
						return -1;					//>>>>>>>>>>>>>>>>>>>>>>>>>>
					}
					if (jctSpStationId2 != 0) {		// •ªŠò“Á—á˜Hü2
						rc = add(jctSpMainLineId2, jctSpStationId2);	//**************
						num++;
						ASSERT(rc == 1);
						if (rc != 1) {			// safety
							BIT_ON(last_flag, 6);
							TRACE(_T("A-0, I, A-2(special junction 2)\n"));
							TRACE(_T("add_abort\n"));
							return -1;				//>>>>>>>>>>>>>>>>>>>>>>>>>>
						}
						if (stationId2 == jctSpStationId2) {
							TRACE(_T("KF0,3,4)\n"));
							line_id = jctSpMainLineId2;
							replace_flg = true;
						}
						stationId1 = jctSpStationId2;
					} else {
						stationId1 = jctSpStationId;
					}
				} else {
					// C-2
					TRACE("JCT: C-2\n");
					ASSERT(IS_SHINKANSEN_LINE(route_list_raw.at(num - 1).lineId));
					routePassOff(jctSpMainLineId, jctSpStationId, stationId1);
					i = Route::NextShinkansenTransferTerm(route_list_raw.at(num - 1).lineId, stationId1, route_list_raw.at(num - 2).stationId);
					if (i <= 0) {	// —×‰w‚ª‚È‚¢ê‡
						TRACE("JCT: C-2(none next station on bullet line)\n");
						// VŠ²ü‚Ì”­‰w‚É‚Í•Àsİ—ˆü(˜Hüb)‚ÉŠ‘®‚µ‚Ä‚¢‚é‚©?
						if (0 == Route::InStationOnLine(jctSpMainLineId, route_list_raw.at(num - 2).stationId)) {
							TRACE(_T("next station is not found in shinkansen.\n"));
							TRACE(_T("add_abort\n"));
							return -1;			// >>>>>>>>>>>>>>>>>>>
						} else {
							removeTail();
							rc = add(jctSpMainLineId, jctSpStationId);	//**************
							ASSERT(rc == 1);
							stationId1 = jctSpStationId;
						}
					} else {
						route_list_raw.at(num - 1).stationId = i;
						route_list_raw.at(num - 1).flag = Route::AttrOfStationOnLineLine(route_list_raw.at(num - 1).lineId, i) & MASK_ROUTE_FLAG;
						route_list_raw.push_back(RouteItem(jctSpMainLineId, jctSpStationId));
						stationId1 = jctSpStationId;
					}
				}
			}
			BIT_ON(last_flag, 6);
		} else {
			// E, G		(stationId2 == jctSpStationId)
			TRACE("JCT: E, G\n");
			if (jctSpStationId2 != 0) {
				TRACE("JCT: KE0-4\n");
				BIT_OFF(lflg2, BSRJCTSP);
			}
			
			line_id = jctSpMainLineId;

			if (route_list_raw.at(num - 1).lineId == jctSpMainLineId) {
				// E-3 , B-0, 5, 6, b, c, d, e
				// E-0, E-1, E-1a, 6, b, c, d, e
				replace_flg = true;
			} else {
				// E-2, G, G-3, G-2, G-4
			}
			BIT_ON(last_flag, 6);
		}
		break;
	}
	if (BIT_CHK(lflg2, BSRJCTSP)) {
		// …•½Œ^
			// a(line_id), d(stationId2) -> b(jctSpMainLineId), c(jctSpStationId)
		retrieveJunctionSpecific(line_id, stationId2);
		if (stationId1 == jctSpStationId) {
			// E10-, F, H
			TRACE("JCT: E10-, F, H/KI0-4\n");
			line_id = jctSpMainLineId;	// a -> b
			if (route_list_raw.at(num - 1).lineId == jctSpMainLineId) {
				replace_flg = true;
				TRACE("JCT: F1, H, E11-14\n");
			} else {
				jct_flg_on = true;	// f3b
			}
		} else {
			// J, B, D
			if ((jctSpStationId2 != 0) && (stationId1 == jctSpStationId2)) {	// •ªŠò“Á—á˜Hü2
				TRACE("JCT: KJ0-4(J, B, D)\n");
				rc = add(jctSpMainLineId2, jctSpStationId);		//**************
				num++;
				if (rc != 1) {
					BIT_ON(last_flag, 6);
					TRACE(_T("junction special (KJ) error.\n"));
					TRACE(_T("add_abort\n"));
					return rc;			// >>>>>>>>>>>>>>>>>>>>>
				}
			} else {
				if (jctSpStationId2 != 0) {	// •ªŠò“Á—á˜Hü2
					TRACE("JCT: KH0-4(J, B, D)\n");
					rc = add(line_id, /*stationId1,*/ jctSpStationId2);	//**************
					num++;
					if (rc == 1) {
						rc = add(jctSpMainLineId2, jctSpStationId);	//**************
						num++;
					}
					if (rc != 1) {
						BIT_ON(last_flag, 6);
						TRACE(_T("junction special horizen type convert error.\n"));
						TRACE(_T("add_abort\n"));
						return rc;			// >>>>>>>>>>>>>>>>>>>>>
					}
				} else {
					TRACE("JCT: J, B, D\n");
					rc = add(line_id, /*stationId1,*/ jctSpStationId);	//**************
					num++;
					if (rc != 1) {
						BIT_ON(last_flag, 6);
						TRACE(_T("junction special horizen type convert error.\n"));
						TRACE(_T("add_abort\n"));
						return rc;			// >>>>>>>>>>>>>>>>>>>>>
					}
					jct_flg_on = true;
				}
			}
			line_id = jctSpMainLineId;
			stationId1 = jctSpStationId;
		}
		BIT_ON(last_flag, 6);
	}

	DBO dbo = Route::Enum_junctions_of_line(line_id, stationId1, stationId2);
	if (!dbo.isvalid()) {
		TRACE(_T("DB error(add-junction-enum)\n"));
		TRACE(_T("add_abort\n"));
		return -100;		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	}

	vector<int> junctions;

	while (dbo.moveNext()) {
		junctions.push_back(dbo.getInt(0));
	}

	jnum = junctions.size();

	for (i = 0; i < jnum; i++) {
		jct_on = junctions[i];
		if (i != 0) {	/* ŠJn‰w(i=0)‚Í‘O˜Hü’…‰w(or ”­‰w)‚È‚Ì‚Åƒ`ƒFƒbƒN‚µ‚È‚¢*/
			if (IsJctMask(jct_on)) {
				// Šù‚É’Ê‰ßÏ‚İ
				break;	/* already passed error */
			}
		}
		JctMaskOn(jct_on);
		TRACE(_T("  add-mask on: %s(%d,%d)\n"), Route::JctName(junctions[i]).c_str(), Route::Jct2id(junctions[i]), junctions[i]);
	}

	if (jnum <= i) {	// •œæ‚È‚µ
		if (((2 <= route_list_raw.size()) && (start_station_id != stationId2) &&
			(0 != Route::InStation(start_station_id, line_id, stationId1, stationId2))) ||
			(((0 < endStationId) && (endStationId != stationId2) && (2 <= route_list_raw.size())) &&
			(0 != Route::InStation(endStationId, line_id, stationId1, stationId2)))) {
			rc = -1;	/* <v> <p> <l> <w> */
		} else if (start_station_id == stationId2) {
			rc = 2;		/* <f> */
		} else {
			rc = 0;		/* <a> <d> <g> */
		}
	} else {	// •œæ
		if ((jnum - 1) == i) { /* last */
			if ((!STATION_IS_JUNCTION(stationId2)) ||
			((2 <= num) && (start_station_id != stationId2) && 
			 (0 != Route::InStation(start_station_id, line_id, stationId1, stationId2))) ||
			(((0 < endStationId) && (endStationId != stationId2) && (2 <= num)) &&
			(0 != Route::InStation(endStationId, line_id, stationId1, stationId2))) ||
			// /*BIT_CHK(lflg1, BSRJCTSP) || */ BIT_CHK(lflg2, BSRJCTSP) ||	// E-12
			// BIT_CHK(route_list_raw.back().flag, BSRJCTSP) ||				// E-14
			// (0 < junctionStationExistsInRoute(stationId2)) ||
			((2 <= num) && (0 < Route::InStation(stationId2, line_id, route_list_raw.at(num - 2).stationId, stationId1)))) {
				rc = -1;	/* <k> <e> <r> <x> <u> <m> */
			} else  {
				rc = 1;		/* <b> <j> <h> */
			}
		} else {
			rc = -1;
		}
	}
	
	if (rc < 0) {
		// •s³ƒ‹[ƒg‚È‚Ì‚Åmask‚ğŒ³‚É–ß‚·
		for (j = 1; j < i; j++) {
			jct_on = junctions[j];
			JctMaskOff(jct_on);	// off
			TRACE(_T("  add_mask off: %s\n"), Route::JctName(jct_on).c_str());
		}
		TRACE(_T("add_abort\n"));
		BIT_OFF(last_flag, 5);
		// E-12, 6, b, c, d, e 
		return -1;	/* already passed error >>>>>>>>>>>>>>>>>>>> */
	}

	/* ’Ç‰Á‚©’uŠ·‚© */
	if (replace_flg) {
		ASSERT((line_id == jctSpMainLineId) || (line_id == jctSpMainLineId2));
		ASSERT((route_list_raw.at(num - 1).lineId == jctSpMainLineId) ||
			   (route_list_raw.at(num - 1).lineId == jctSpMainLineId2));
		route_list_raw.pop_back();
		--num;
	}
	if (jct_flg_on) {
		lflg2 |= MASK(BSRJCTSP);	// …•½Œ^ŒŸ’m(D-2)
	} else {
		lflg2 &= ~MASK(BSRJCTSP);
	}
	route_list_raw.push_back(RouteItem(line_id, stationId2, lflg2));
	++num;

	if (rc == 0) {
		TRACE(_T("added continue.\n"));
		BIT_OFF(last_flag, 5);
	} else if (rc == 1) {
		BIT_ON(last_flag, 5);
	} else if (rc = 2) {
		BIT_OFF(last_flag, 5);	/* Ÿ‚ÉremoveTail‚ÅlastItem‚Ì’Ê‰ßƒ}ƒXƒN‚ğOff‚·‚é(typeO‚Å‚àP‚Å‚à‚È‚¢‚Ì‚Å) */
	} else {
		ASSERT(FALSE);
		BIT_ON(last_flag, 5);
	}
	TRACE(_T("added last.\n"));

	return ((rc == 0) ? 1:0);
}


//	Œo˜H‚Ì––”ö‚ğœ‹
//
//	@param [in] begin_off    ÅIƒm[ƒh‚Ìn“_‚àOff‚·‚é‚©(ƒfƒtƒHƒ‹ƒg:Off‚µ‚È‚¢)
//
void Route::removeTail(bool begin_off/* = false*/)
{
	int line_id;
	int begin_station_id;
	int to_station_id;
	int jct_num;
	int route_num;
	int i;
	vector<int> junctions;	// •ªŠò‰wƒŠƒXƒg

	route_num = route_list_raw.size();
	if (route_num < 2) {
		ASSERT(FALSE);
		BIT_OFF(last_flag, 5);
		return;
	}
	
	/* ”­‰w`Å‰‚ÌæŠ·‰w */
	if (route_num == 2) {
		removeAll(false, false);
		return;
	}
	to_station_id = route_list_raw[route_num - 1].stationId;	// tail
	line_id = route_list_raw[route_num - 1].lineId;
	begin_station_id = route_list_raw[route_num - 2].stationId;	// tail - 1

	DBO dbo = Route::Enum_junctions_of_line(line_id, to_station_id, begin_station_id);
	if (!dbo.isvalid()) {
		return;
	}
	
	while (dbo.moveNext()) {
		junctions.push_back(dbo.getInt(0));
	}
	jct_num = junctions.size();
	if (!begin_off) {
		jct_num -= 1;		/* ŠJn‰w‚ÍOff‚µ‚È‚¢(‘O˜Hü‚Ì’…‰w‚È‚Ì‚Å) */
	}
	for (i = 0; i < jct_num; i++) {
		/* i=0:Å‹ß•ªŠò‰w‚ÅOŒ^Œo˜HAPŒ^Œo˜H‚Ì’…‰w‚Ìê‡‚ÍœŠO */
		if ((i != 0) || !BIT_CHK(last_flag, 5)) {
			JctMaskOff(junctions[i]);
			TRACE(_T("removed   : %s\n"), Route::JctName(junctions[i]).c_str());
		}
	}

	BIT_OFF(last_flag, 5);
	route_list_raw.pop_back();
}


//private:
//	@brief	•ªŠòƒ}[ƒNOff
//
//	@param [in]  line_id          ˜Hü
//	@param [in]  to_station_id    ŠJn‰w(ŠÜ‚Ü‚È‚¢)
//	@param [in]  begin_station_id I—¹‰w(ŠÜ‚Ş)
//
void Route::routePassOff(int line_id, int to_station_id, int begin_station_id)
{
	int jct_num;
	int i;
	vector<int> junctions;	// •ªŠò‰wƒŠƒXƒg

	DBO dbo = Route::Enum_junctions_of_line(line_id, to_station_id, begin_station_id);
	if (!dbo.isvalid()) {
		return;
	}
	
	while (dbo.moveNext()) {
		junctions.push_back(dbo.getInt(0));
	}

	jct_num = junctions.size();

	for (i = 1; i < jct_num; i++) {
		JctMaskOff(junctions[i]);
		TRACE(_T("removed.  : %s\n"), Route::JctName(junctions[i]).c_str());
	}
}

//	Œo˜Hİ’è’† Œo˜Hd•¡”­¶
//	Œo˜Hİ’èƒLƒƒƒ“ƒZƒ‹
//
//	@param [in]  bWithStart  ŠJn‰w‚àƒNƒŠƒA‚·‚é‚©(ƒfƒtƒHƒ‹ƒgƒNƒŠƒA)
//	@param [in]  bWithEnd    I—¹‰w‚àƒNƒŠƒA‚·‚é‚©(ƒfƒtƒHƒ‹ƒgƒNƒŠƒA)
//	@note
//	x stop_jctId > 0 : w’è•ªŠò‰w‘O‚Ü‚ÅƒNƒŠƒA(w’è•ªŠò‰wŠÜ‚Ü‚¸)
//	x stop_jctId = 0 : (default)w’è•ªŠò‰ww’è‚È‚µ
//	x stop_jctId < 0 : ŠJn’n“_‚àíœ
//
void Route::removeAll(bool bWithStart /* =true */, bool bWithEnd /* =true */)
{
	int begin_station_id;
	
	JctMaskClear();
	fare_info.reset();

	if (bWithEnd) {
		endStationId = 0;
	}
	if (!bWithStart) {
		begin_station_id = startStationId();
	}

	route_list_raw.clear();
	route_list_cooked.clear();

	last_flag = 0;

	TRACE(_T("clear-all mask.\n"));

	if (!bWithStart) {
		add(begin_station_id);
	}
}


//	‰w‚ªŒo˜H“à‚ÉŠÜ‚Ü‚ê‚Ä‚¢‚é‚©H
//
//	@param [in] stationId   ‰wident
//	@retval true found
//	@retval false notfound
//
bool Route::checkPassStation(int stationId)
{
	vector<RouteItem>::const_iterator route_item;
	int stationIdFrom = 0;

	for (route_item = route_list_raw.cbegin(); route_item != route_list_raw.cend(); route_item++) {
		if (stationIdFrom != 0) {
			if (0 != Route::InStation(stationId, route_item->lineId, stationIdFrom, route_item->stationId)) {
				return true;
			}
		} else {
			ASSERT(route_item == route_list_raw.cbegin());
			ASSERT(startStationId() == route_item->stationId);
		}
		stationIdFrom = route_item->stationId;
	}
	return false;
}

//	•ªŠò‰w‚ªŒo˜H“à‚ÉŠÜ‚Ü‚ê‚Ä‚¢‚é‚©H
//
//	@param [in] stationId   ‰wident
//	@retval true found
//	@retval false notfound
//
int Route::junctionStationExistsInRoute(int stationId)
{
	int c;
	vector<RouteItem>::const_iterator route_item;

	c = 0;
	for (route_item = route_list_raw.cbegin(); route_item != route_list_raw.cend(); route_item++) {
		if (stationId == route_item->stationId) {
			c++;
		}
	}
	return c;
}

//	Œo˜H“à‚ğ’…‰w‚ÅI—¹‚·‚é(ˆÈ~‚ÌŒo˜H‚ÍƒLƒƒƒ“ƒZƒ‹)
//
//	@param [in] stationId   ‰wident
//
void Route::terminate(int stationId)
{
	int i;
	int stationIdFrom = 0;
	int newLastIndex = 0x7fffffff;
	int line_id;
	int stationIdTo;
	
	for (i = 0; i < (int)route_list_raw.size(); i++) {
		if (stationIdFrom != 0) {
			/* stationId‚Íroute_list_raw[i].lineId“à‚ÌstationIdFrom‚©‚ç
			              route_list_raw[i].statonId‚ÌŠÔ‚É‚ ‚é‚©? */
			if (0 != Route::InStation(stationId, route_list_raw[i].lineId, stationIdFrom, route_list_raw[i].stationId)) {
				newLastIndex = i;
				line_id = route_list_raw[i].lineId;
				stationIdTo = route_list_raw[i].stationId;
				break;
			}
		} else {
			ASSERT(i == 0);
		}
		stationIdFrom = route_list_raw[i].stationId;
	}
	if (newLastIndex < (int)route_list_raw.size()) {
		while (newLastIndex < (int)route_list_raw.size()) {
			removeTail();
		}
		ASSERT((newLastIndex<=1) || route_list_raw[newLastIndex - 1].stationId == stationIdFrom);
		add(line_id, /*stationIdFrom,*/ stationId);
		endStationId = stationId;
	}
}

//static 
//	˜Hü‚ªã‚è^‰º‚è‚©‚ğ“¾‚é
//
//	@param [in] line_id     ˜Hü
//	@param [in] station_id1 ”­
//	@param [in] station_id2 Š
//
//	@retval 0 ‰º‚è(Route::LDIR_ASC)
//	@retval 1 ã‚è(Route::LDIR_DESC)
//
//  @node “¯ˆê‰w‚Ìê‡‰º‚è(0)‚ğ•Ô‚·
//
//
int Route::DirLine(int line_id, int station_id1, int station_id2)
{
	const static char tsql[] = 
"select case when"
"   ((select sales_km from t_lines where line_id=?1 and station_id=?2) - "
"    (select sales_km from t_lines where line_id=?1 and station_id=?3)) <= 0"
" then 0 else 1 end;";
	
	DBO ctx = DBS::getInstance()->compileSql(tsql);
	if (ctx.isvalid()) {
		ctx.setParam(1, line_id);
		ctx.setParam(2, station_id1);
		ctx.setParam(3, station_id2);

		if (ctx.moveNext()) {
			return ctx.getInt(0);
		}
	}
	return 0;	/* ‰º‚è */
}


// static
//	•ªŠòID¨‰wID
//
int Route::Jct2id(int jctId)
{
	DBO ctx = DBS::getInstance()->compileSql(
"select station_id from t_jct where rowid=?");

	if (ctx.isvalid()) {
		ctx.setParam(1, jctId);

		if (ctx.moveNext()) {
			return ctx.getInt(0);
		}
	}
	return 0;	// error
}

// static
//	‰wID¨•ªŠòID
//
int Route::Id2jctId(int stationId)
{
	DBO ctx = DBS::getInstance()->compileSql(
"select id from t_jct where station_id=?");
	if (ctx.isvalid()) {

		ctx.setParam(1, stationId);

		if (ctx.moveNext()) {
			return ctx.getInt(0);
		}
	}
	return 0;	// error
}

//static
//	•ªŠòID¨‰w–¼
//
tstring Route::JctName(int jctId)
{
	TCHAR name[MAX_STATION_CHR];
	
	memset(name, 0, sizeof(name));
	
	DBO ctx = DBS::getInstance()->compileSql(
		"select name from t_jct j left join t_station t on j.station_id=t.rowid where id=?");
	if (ctx.isvalid()) {

		ctx.setParam(1, jctId);

		if (ctx.moveNext()) {
			_tcscpy_s(name, ctx.getText(0).c_str());
		}
	}
	return name;
}

//static 
//	‰wID¨‰w–¼
//
tstring Route::StationName(int id)
{
	TCHAR name[MAX_STATION_CHR];
	
	memset(name, 0, sizeof(name));
	
	DBO ctx = DBS::getInstance()->compileSql(
		"select name from t_station where rowid=?");
	if (ctx.isvalid()) {

		ctx.setParam(1, id);

		if (ctx.moveNext()) {
			_tcscpy_s(name, ctx.getText(0).c_str());
		}
	}
	return name;
}

//static 
//	‰wID¨‰w–¼(“¯–¼‰w‹æ•Ê)
//
tstring Route::StationNameEx(int id)
{
	TCHAR name[MAX_STATION_CHR];
	
	memset(name, 0, sizeof(name));
	
	DBO ctx = DBS::getInstance()->compileSql(
		"select name,samename from t_station where rowid=?");
	if (ctx.isvalid()) {

		ctx.setParam(1, id);

		if (ctx.moveNext()) {
			_tcscpy_s(name, ctx.getText(0).c_str());
			_tcscat_s(name, ctx.getText(1).c_str());
		}
	}
	return name;
}

//static 
//	˜HüID¨˜Hü–¼
//
tstring Route::LineName(int id)
{
	TCHAR name[MAX_STATION_CHR];
	
	memset(name, 0, sizeof(name));
	
	DBO ctx = DBS::getInstance()->compileSql(
		"select name from t_line where rowid=?");
	if (ctx.isvalid()) {

		ctx.setParam(1, id);

		if (ctx.moveNext()) {
			_tcscpy_s(name, ctx.getText(0).c_str());
		}
	}
	return name;
}

//static 
//	‰w‚Ì‘®«‚ğ“¾‚é
//
SPECIFICFLAG Route::AttrOfStationId(int id)
{
	DBO ctx = DBS::getInstance()->compileSql(
		"select sflg from t_station where rowid=?");
	if (ctx.isvalid()) {

		ctx.setParam(1, id);

		if (ctx.moveNext()) {
			return ctx.getInt(0);
		}
	}
	return (1<<30);
}

//static 
//	˜Hü‰w‚Ì‘®«‚ğ“¾‚é
//
SPECIFICFLAG Route::AttrOfStationOnLineLine(int line_id, int station_id)
{
	DBO ctx = DBS::getInstance()->compileSql(
		"select lflg from t_lines where line_id=?1 and station_id=?2", true);
	if (ctx.isvalid()) {

		ctx.setParam(1, line_id);
		ctx.setParam(2, station_id);

		if (ctx.moveNext()) {
			return ctx.getInt(0);
		}
	}
	return (1<<30);
}

//static 
//	‰w‚Í˜Hü“à‚É‚ ‚é‚©”Û‚©H
//
int Route::InStationOnLine(int line_id, int station_id)
{
	DBO ctx = DBS::getInstance()->compileSql(
		"select count(*) from t_lines where line_id=?1 and station_id=?2", true);
	if (ctx.isvalid()) {

		ctx.setParam(1, line_id);
		ctx.setParam(2, station_id);

		if (ctx.moveNext()) {
			return ctx.getInt(0);
		}
	}
	return 0;
}


// ‰w(station_id)‚ÌŠ‘®‚·‚é˜HüID‚ğ“¾‚é. 
// ‰w‚Í”ñ•ªŠò‰w‚Å, ˜Hü‚Í1‚Â‚µ‚©‚È‚¢‚Æ‰ğ‚Á‚Ä‚¢‚é‚±‚Æ‚ğ‘O’ñ‚Æ‚·‚é.
//
// line_id = f(station_id)
//
int Route::LineIdFromStationId(int station_id)
{
	DBO ctx = DBS::getInstance()->compileSql(
  "select line_id"
  " from t_lines"
  " where station_id=?"
  " and 0=(lflg&((1<<31)|(1<<22)))");
	if (ctx.isvalid()) {
  		ctx.setParam(1, station_id);
		if (ctx.moveNext()) {
			return ctx.getInt(0);
		}
	}
	return 0;
}


// ‰w–¼‚æ‚è‰wID‚ğ•Ô‚·(„“SŠÜ)
//
// station_id = f("‰w–¼")
//
int Route::GetStationId(LPCTSTR station)
{
	const char tsql[] = "select rowid from t_station where (sflg&(1<<23))=0 and name=?1 and samename=?2";

	tstring sameName;
	tstring stationName(station);

	int pos = stationName.find('(');
	if (0 <= pos) {
		sameName = stationName.substr(pos);
		stationName = stationName.substr(0, pos);
	}

	DBO dbo = DBS::getInstance()->compileSql(tsql);
	if (dbo.isvalid()) {
		dbo.setParam(1, stationName.c_str());
		dbo.setParam(2, sameName.c_str());
		if (dbo.moveNext()) {
			return dbo.getInt(0);
		}
	}
	return 0;
}

// ü–¼‚æ‚èüID‚ğ•Ô‚·
//
// station_id = f("ü–¼")
//
int Route::GetLineId(LPCTSTR lineName)
{
	const char tsql[] = "select rowid from t_line where name=?";

	DBO dbo = DBS::getInstance()->compileSql(tsql);
	if (dbo.isvalid()) {
		dbo.setParam(1, lineName);
		if (dbo.moveNext()) {
			return dbo.getInt(0);
		}
	}
	return 0;
}


//	@brief •ªŠò“Á—á‚Ì•ªŠò˜Hüa+æŠ·‰wd‚©‚ç–{üb‚Æ•ªŠò‰wc‚ğ“¾‚é
//
//	@param [in]  jctLineId         a •ªŠò˜Hü
//	@param [in]  transferStationId d æŠ·‰w
//	@param [out] jctSpMainLineId   b –{ü
//	@param [out] jctSpStationId    c •ªŠò‰w
//
void Route::retrieveJunctionSpecific(int jctLineId, int transferStationId)
{
	const char tsql[] =
	//"select calc_km>>16, calc_km&65535, (lflg>>16)&32767, lflg&32767 from t_lines where (lflg&(1<<31))!=0 and line_id=?1 and station_id=?2";
	"select jctsp_line_id1, jctsp_station_id1, jctsp_line_id2, jctsp_station_id2"
	" from t_jctspcl where id=("
	"	select calc_km from t_lines where (lflg&(1<<31))!=0 and line_id=?1 and station_id=?2)";

	DBO dbo = DBS::getInstance()->compileSql(tsql);
	if (dbo.isvalid()) {
		dbo.setParam(1, jctLineId);
		dbo.setParam(2, transferStationId);
		if (dbo.moveNext()) {
			jctSpMainLineId = dbo.getInt(0);
			jctSpStationId = dbo.getInt(1);
			jctSpMainLineId2 = dbo.getInt(2);
			jctSpStationId2 = dbo.getInt(3);
		}
	}
	ASSERT(((jctSpMainLineId2 == 0) && (jctSpStationId2 == 0)) ||
		   ((jctSpMainLineId2 != 0) && (jctSpStationId2 != 0)));
	if (jctSpStationId2 == 0) {	// safety
		jctSpMainLineId2 = 0;
	}
}

//static
//	‰^’ÀŒvZƒLƒ‚Æ‰c‹ÆƒLƒ‚ğ•Ô‚·
//
//	@param [in] line_id     ˜Hü
//	@param [in] station_id1 ”­
//	@param [in] station_id2 Š
//  
//	@retuen [0]:‰c‹ÆƒLƒ, [1]:ŒvZƒLƒ
//
vector<int> Route::GetDistance(int line_id, int station_id1, int station_id2)
{

	const char tsql[] = 
"select max(sales_km)-min(sales_km), case max(calc_km)-min(calc_km) when 0 then max(sales_km)-min(sales_km) else max(calc_km)-min(calc_km) end" 
" from t_lines"
" where line_id=?1"
" and	(lflg&(1<<31))=0"
" and	sales_km>=(select min(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))"
" and	sales_km<=(select max(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))";

#if 0
"select l1.sales_km - l2.sales_km, l1.calc_km - l2.calc_km"
" from t_lines l1"
" left join t_lines l2"
" where l1.line_id=?1"
" and l2.line_id=?1"
" and l1.sales_km>l2.sales_km"
" and ((l1.station_id=?2 and l2.station_id=?3)" 
" or (l1.station_id=?3 and l2.station_id=?2))";
#endif
	int sales_km;
	int calc_km;
	vector<int> v;

	DBO dbo = DBS::getInstance()->compileSql(tsql, true);
	if (dbo.isvalid()) {
		dbo.setParam(1, line_id);
		dbo.setParam(2, station_id1);
		dbo.setParam(3, station_id2);
	}
	if (dbo.moveNext()) {
		sales_km = dbo.getInt(0);
		calc_km = dbo.getInt(1);
		v.push_back(sales_km);
		v.push_back(calc_km);
	}
	return v;
}

//static
//	‰^’ÀŒvZƒLƒ‚ğ•Ô‚·(Å’Z‹——£Zo—p)
//
//	@param [in] line_id     ˜Hü
//	@param [in] station_id1 ”­
//	@param [in] station_id2 Š
//  
//	@retuen ‰c‹ÆƒLƒ or ŒvZƒLƒ
//
int Route::Get_node_distance(int line_id, int station_id1, int station_id2)
{
	const char tsql[] = 
"select case when sum(calc_km)=0 then max(sales_km)-min(sales_km) else max(calc_km)-min(calc_km) end"
" from	t_lines"
" where line_id=?1"
" and	(lflg&(1<<31))=0"
" and	sales_km>=(select min(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))"
" and	sales_km<=(select max(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))";

#if 0
"select case when (l1.calc_km-l2.calc_km)=0 then l1.sales_km - l2.sales_km else  l1.calc_km - l2.calc_km end"
" from t_lines l1"
" left join t_lines l2"
" where l1.line_id=?1"
" and l2.line_id=?1"
" and l1.sales_km>l2.sales_km"
" and ((l1.station_id=?2 and l2.station_id=?3)" 
" or (l1.station_id=?3 and l2.station_id=?2))";
#endif
	DBO dbo = DBS::getInstance()->compileSql(tsql, true);
	if (dbo.isvalid()) {
		dbo.setParam(1, line_id);
		dbo.setParam(2, station_id1);
		dbo.setParam(3, station_id2);
	}
	if (dbo.moveNext()) {
		return dbo.getInt(0);
	}
	return 0;
}


//static
//	“Á’è“s‹æs“à‰w–¼‚ğ•Ô‚·
//
//	@param [in] startEndFlg     CSTART(”­‰w) / CEND(’…‰w)
//	@param [in] flg             RouteItem”z—ñ‚Ìæ“ª‚ÌlineId‚Éİ’è‚³‚ê‚½ƒtƒ‰ƒO’l(”­‰wA’…‰w‚Ì—L–³ARèü“à‚©)
//	@param [in] flags           RouteItem”z—ñ‚ÌŒ»ˆÊ’u(”­‰w‚Ü‚½‚Í’…‰w)‚Ìflag‚Éİ’è‚³‚ê‚½ƒtƒ‰ƒO’l(“s‹æs“àID)
//
//	@return “Á’è“s‹æs“à
//
tstring Route::CoreAreaNameByCityId(int startEndFlg, int flg, SPECIFICFLAG flags)
{
	const static TCHAR *coreAreaName[] = {  _T("“Œ‹“s‹æ“à[‹æ]"),	// 1
											_T("‰¡•ls“à[•l]"),		// 2
											_T("–¼ŒÃ‰®s“à[–¼]"),	// 3
											_T("‹“ss“à[‹]"),		// 4
											_T("‘åãs“à[ã]"),		// 5
											_T("_ŒËs“à[_]"),		// 6
											_T("L“‡s“à[L]"),		// 7
											_T("–k‹ãBs“à[‹ã]"),	// 8
											_T("•Ÿ‰ªs“à[•Ÿ]"),		// 9
											_T("å‘äs“à[å]"),		// 10
											_T("D–ys“à[D]"),		// 11
											/*****************/
											_T("Rèü“à[R]"), 	// -2
											_T("‘åãEV‘åã"), 	// -1
										};
	int cityno;

	if (((startEndFlg == CSTART) && BIT_CHK(flg, B1LID_BEGIN_YAMATE)) ||
	    ((startEndFlg == CEND)   && BIT_CHK(flg, B1LID_FIN_YAMATE))) {
		return coreAreaName[NumOf(coreAreaName) - 2];	/* Rèü“à*/
	} else if (((startEndFlg == CSTART) && BIT_CHK(flg, B1LID_BEGIN_CITY)) || 
			   ((startEndFlg == CEND)   && BIT_CHK(flg, B1LID_FIN_CITY))) {
		cityno = MASK_CITYNO(flags) - 1;
		if (cityno < (NumOf(coreAreaName) - 2)) {
			return coreAreaName[cityno];
		} else {
			ASSERT(FALSE);
		}
	} else if (((startEndFlg == CSTART) && BIT_CHK(flg, B1LID_BEGIN_OOSAKA)) || 
			   ((startEndFlg == CEND)   && BIT_CHK(flg, B1LID_FIN_OOSAKA))) {
		return coreAreaName[NumOf(coreAreaName) - 1];	/* ‘åãEV‘åã */
	}
	return _T("");
}

//static 
//	70ğ“K—p
//	route_list‚ğ“K—p‚µ‚Äout_route_list‚Éİ’è‚·‚é
//	showFare() => checkOfRuleSpecificCoreLine() => 
//
//	out_route_list‚Í•\¦—p‚Å‚Í‚È‚­ŒvZ—p(˜Hü‚ª70ğê—p‚ª‚ ‚é)
//
//	@param [in]   in_route_list  Œo˜H
//	@param [out]  out_route_list Œo˜H
//	@retval 0: done
//	@retval -1: N/A
//
int Route::ReRouteRule70j(const vector<RouteItem>& in_route_list, vector<RouteItem>* out_route_list)
{
	int stage;
	int stationId_o70;
	int flag;

	vector<RouteItem>::const_iterator route_item;

	stage = 0;

	for (route_item = in_route_list.cbegin(); route_item != in_route_list.cend(); route_item++) {
		bool skip = false;
		RouteItem ri = *route_item;

		if (stage == 0) {
			if ((route_item->flag & (1 << BCRULE70)) == 0) {
				stage = 1;					/* 1: off */
			} else {
				break;		/* 70ğ“K—pƒGƒŠƒA“à”­‚Í”ñ“K—p(86A87ğ) */
			}
		} else if (stage == 1) {
			if ((route_item->flag & (1 << BCRULE70)) != 0) {
				stage = 2;					/* 2: on */ /* ŠO‚©‚çi“ü‚µ‚½ */
								/* ˜Hü‚æ‚èÅŠO‘¤‚Ì‘åŠÂóü“à(70ğ“K—p)‰w‚ğ“¾‚é */
				stationId_o70 = Route::RetrieveOut70Station(route_item->lineId);
				ASSERT(0 < stationId_o70);
				ri.stationId = stationId_o70;
				flag = route_item->flag;
			} else {	// ŠO‚Ì‚Ü‚Ü
				/* do nothing */
			}
		} else if (stage == 2) {
			if ((route_item->flag & (1 << BCRULE70)) == 0) {
				int stationId_tmp;
				stage = 3;					/* 3: off: !70 -> 70 -> !70 (applied) */
								/* i“ü‚µ‚Ä’Eo‚µ‚½ */
								/* ˜Hü‚æ‚èÅŠO‘¤‚Ì‘åŠÂóü“à(70ğ“K—p)‰w‚ğ“¾‚é */
				stationId_tmp = Route::RetrieveOut70Station(route_item->lineId);
				ASSERT(0 < stationId_tmp);
				if (stationId_tmp != stationId_o70) {
					out_route_list->push_back(RouteItem(ID_L_RULE70, stationId_tmp, flag));
				}
			} else {	// ’†‚Ì‚Ü‚Ü
				skip = true;
				flag = route_item->flag;
			}
		} else if (stage == 3) {
			/* 4 */
			if ((route_item->flag & (1 << BCRULE70)) != 0) {
				stage = 4;				// i“ü‚µ‚Ä’Eo‚µ‚Ä(’Ê‰ß‚µ‚Ä)‚Ü‚½i“ü‚µ‚½‚ç70ğ”ñ“K—p
										/* !70 -> 70 -> !70 -> 70 (exclude) */
				break;
			} else {
				/* 70ğ’Ê‰ß(’Ê‰ß‚µ‚ÄŠO‚Éo‚Ä‚¢‚é */
				/* do nothing */
			}
		} else {	/* -1: ”­‰w‚©‚ç70ğ“K—p‰w */
			/* do nothing */
		}
		if (!skip) {
			out_route_list->push_back(ri);
		}
	}
	
	switch (stage) {
	case 0:
		// ”ñ“K‡
		// 0: 70ğ“K—p‰w”­
		out_route_list->assign(in_route_list.cbegin(), in_route_list.cend());
		break;
	case 1:		// 1: 70ğ“K—p‰w‚È‚µ(“s“à‚ğ’Ê‚ç‚È‚¢‘½‚­‚Ìƒ‹[ƒg)
		return -1;
		break;
	case 3:		/* 70ğ“K—p‰w‚ğ’Ê‰ß */
		return 0;	// done
		break;
	case 2:		// 2: 70ğ“K—p‰w’…(”­‰w‚Í”ñ“K—p‰w)
	case 4:		// 4: 70ğ“K—p‰w‹æŠÔ‚ğ’Ê‰ß‚µ‚ÄÄ“xi“ü‚µ‚½ê‡
	default:
		/* cancel */
		out_route_list->clear();
		out_route_list->assign(in_route_list.cbegin(), in_route_list.cend());
		break;
	}
	return -1;
}


//static
//	69ğ“K—p‰w‚ğ•Ô‚·
//	’[‰w1`’[‰w2‚ÌƒyƒA~N‚ğ‚©‚¦‚·.
//	results[]”z—ñ‚Í•K‚¸n*2‚ğ•Ô‚·
//
//	showFare() => checkOfRuleSpecificCoreLine() => ReRouteRule69j()=>
//
//	coninue_flag: false ‘±‚«–³‚µ
//                true  ‘±‚«
//	return: false ‘±‚«–³‚µ
//          true  ‘±‚«
bool Route::Query_a69list(int line_id, int station_id1, int station_id2, vector<PAIRIDENT>* results, bool continue_flag)
{
	const static char tsql[] = 
//" select station_id, (lflg>>17)&15, (lflg>>29)&1,  (lflg>>28)&1"
" select station_id, lflg"
" from t_lines"
" where line_id=?1"
" and (lflg&((1<<31)|(1<<29)))=(1<<29)"
" and station_id"
" in (select station_id"
"      from t_lines"
"      where line_id=?1"
"      and ((sales_km>=(select sales_km"
"                   from t_lines"
"                   where line_id=?1"
"                   and station_id=?2)"
"      and  (sales_km<=(select sales_km"
"                   from t_lines"
"                   where line_id=?1"
"                   and station_id=?3)))"
"      or  (sales_km<=(select sales_km"
"                  from t_lines"
"                  where line_id=?1"
"                  and station_id=?2)"
"      and (sales_km>=(select sales_km"
"                  from t_lines"
"                  where line_id=?1"
"                  and station_id=?3)))))"
" order by"
" case when"
" (select sales_km from t_lines where line_id=?1 and station_id=?3) <"
" (select sales_km from t_lines where line_id=?1 and station_id=?2) then"
" sales_km"
" end desc,"
" case when"
" (select sales_km from t_lines where line_id=?1 and station_id=?3) >"
" (select sales_km from t_lines where line_id=?1 and station_id=?2) then"
" sales_km"
" end asc";

	int prev_flag = 0;
	int prev_stid = 0;
	int cur_flag;
	int cur_stid;
	bool next_continue = false;
	
	vector<PAIRIDENT> pre_list;
	vector<PAIRIDENT>::const_iterator it;
	
	results->clear();

	TRACE(_T("c69 line_id=%d, station_id1=%d, station_id2=%d\n"), line_id, station_id1, station_id2);

	DBO ctx = DBS::getInstance()->compileSql(tsql, true);
	if (ctx.isvalid()) {
		ctx.setParam(1, line_id);
		ctx.setParam(2, station_id1);
		ctx.setParam(3, station_id2);
		while (ctx.moveNext()) {
			cur_stid = ctx.getInt(0);
			cur_flag = ctx.getInt(1);
			pre_list.push_back(MAKEPAIR(IDENT1(cur_stid), IDENT2(cur_flag)));
		}
		cur_stid = 0;
		cur_flag = 0;
		for (it = pre_list.cbegin(); it != pre_list.cend(); it++) {
			cur_stid = IDENT1(*it);
			cur_flag = IDENT2(*it);

			if (((prev_flag >> (17 - HWORD_BIT)) & 0x0f) == ((cur_flag >> (17 - HWORD_BIT)) & 0x0f)) {
				// OK
				if (((cur_flag & (1 << (28 - HWORD_BIT))) != 0) && ((it + 1) == pre_list.cend()) && 
					 (station_id2 == cur_stid)) { /* ÅŒã‚ÅBSR69CONT=1 */
					next_continue = true;
				} else if ((cur_flag & (1 << (28 - HWORD_BIT))) != 0) {
					// NG
					prev_flag = 0;
					continue;
				}
				results->push_back(MAKEPAIR(IDENT1(prev_stid), IDENT1((prev_flag >> (17 - HWORD_BIT)) & 0x0f)));
				results->push_back(MAKEPAIR(IDENT1(cur_stid), IDENT1((cur_flag >> (17 - HWORD_BIT)) & 0x0f)));
				prev_flag = 0;

				TRACE(_T("c69             station_id1=%s(%d), station_id2=%s(%d)\n"), Route::StationName(prev_stid).c_str(), prev_stid, Route::StationName(cur_stid).c_str(), cur_stid);

				if (((cur_flag & (1 << (28 - HWORD_BIT))) != 0) && 
				    ((it + 1) == pre_list.cend()) && 
				    (station_id2 == cur_stid)) { /* ÅŒã‚ÅBSR69CONT=1 */
					next_continue = true;
				} else {

				}
			} else {
				/* Œp‘±ˆÈŠO‚ÅŒp‘±ƒtƒ‰ƒO•tBSR69TERM‚Í–³Œø*/
				if ((continue_flag && ((cur_flag & (1 << (28 - HWORD_BIT))) != 0) && (station_id1 == cur_stid)) ||
				   (!continue_flag && ((cur_flag & (1 << (28 - HWORD_BIT))) == 0))) {
					// OK
					prev_flag = cur_flag;
					prev_stid = cur_stid;
				} else {
					// NG skip
					prev_flag = 0;
					prev_stid = 0;
				}
				continue_flag = false;	/* 1‰ñ–ÚˆÈŠO‚ÍOff */
			}
		}
	}
	return next_continue;
}

//static
//	69ğ’uŠ·˜HüA‰w‚ğ•Ô‚·
//
bool Route::Query_rule69t(const vector<RouteItem>& in_route_list, const RouteItem& cur, int ident, vector<vector<PAIRIDENT>>* results)
{
	int lineId;
	int stationId1;
	int stationId2;
	int in;
	int in1;
	int s1;
	int s2;
	const static char tsql[] = 
	" select line_id, station_id1, station_id2 from t_rule69 where id=? order by ord";

	results->clear();
	
	DBO ctx = DBS::getInstance()->compileSql(tsql, true);
	if (ctx.isvalid()) {
		ctx.setParam(1, ident);
		while (ctx.moveNext()) {
			vector<PAIRIDENT> record;
			
			record.push_back(ctx.getInt(0));
			record.push_back(ctx.getInt(1));
			record.push_back(ctx.getInt(2));
			results->push_back(record);
		}
		ASSERT(0 < results->size());

		vector<RouteItem>::const_iterator route_item;
		route_item = in_route_list.cbegin();
		s1 = 0;
		in1 = 0;
		while (route_item != in_route_list.cend()) {
			vector<vector<PAIRIDENT>>::const_iterator it;
			s2 = route_item->stationId;
			if (s1 != 0) { /* 2‰ñ–ÚˆÈ~ ? */
				if (!cur.is_equal(*route_item)) {
					it = results->cbegin();
					while (it != results->cend()) {
						lineId = it->at(0);
						stationId1 = it->at(1);
						stationId2 = it->at(2);
						in = Route::InStation(s2, lineId, stationId1, stationId2);
						in1 = Route::InStation(s1, lineId, stationId1, stationId2);
						/* 2‰w‚Æ‚à’uŠ·˜Hüã‚É‚ ‚ê‚Î’uŠ·‚µ‚È‚¢
						 * 1‰w‚Ì‚İ’uŠ·˜Hüã‚É‚ ‚éê‡A‚»‚Ì‰w‚ª’[‰w‚Å‚È‚¯‚ê‚Î’uŠ·‚µ‚È‚¢
						 */
						if (results->size() == 1) {
							if (((in != 0) && (in1 != 0)) ||
						        ((in  != 0) && ((s2 != stationId1) && (s2 != stationId2))) || 
						        ((in1 != 0) && ((s1 != stationId1) && (s1 != stationId2)))) {
								results->clear();	/* ’uŠ·‘ÎÛŠO‚Æ‚·‚é */
								return false;
							}
						} else if (it == results->cbegin()) {
							if (((in != 0) && (in1 != 0)) ||
						        ((in  != 0) && (s2 != stationId1)) || 
						        ((in1 != 0) && (s1 != stationId1))) {
								results->clear();	/* ’uŠ·‘ÎÛŠO‚Æ‚·‚é */
								return false;
							}
						} else if ((it + 1) == results->cend()) {
							if (((in != 0) && (in1 != 0)) ||
						        ((in  != 0) && (s2 != stationId2)) || 
						        ((in1 != 0) && (s1 != stationId2))) {
								results->clear();	/* ’uŠ·‘ÎÛŠO‚Æ‚·‚é */
								return false;
							}
						} else {
							if ((in != 0) || (in1 != 0)) {
								results->clear();	/* ’uŠ·‘ÎÛŠO‚Æ‚·‚é */
								return false;
							}
						}
						it++;
					}
				}
			}
			s1 = s2;
			route_item++;
		}
	}
	return 0 < results->size();
}

//static
//	69ğ“K—p
//	in_route_list‚ğ“K—p‚µ‚Äroute_list_cooked‚Éİ’è‚·‚é
//	out_route_list‚Í•\¦—p‚Å‚Í‚È‚­ŒvZ—p
//	showFare() => checkOfRuleSpecificCoreLine() => 
//
//	@param [in] in_route_list  •ÏŠ·‘O‚Ìƒ`ƒFƒbƒNŒo˜H
//	@param [out] out_route_list •ÏŠ·Œã‚Ìƒ`ƒFƒbƒNŒo˜H
//	@retval 0: N/A
//	@retval 0<: replace for number of route
//
int Route::ReRouteRule69j(const vector<RouteItem>& in_route_list, vector<RouteItem>* out_route_list)
{
	int i;
	int change;
	bool continue_flag;
	int station_id1;
	int a69_line_id;
	vector<PAIRIDENT> dbrecord;				// LO:stid1, HI:flg
	vector<PAIRIDENT> a69list;				// LO:stid1, HI:stid2
	vector<vector<PAIRIDENT>> trule69list;	// 

	vector<RouteItem>::iterator route_item;

	change = 0;

	/* dup copy in_route_list -> out_route_list */
	out_route_list->clear();
	out_route_list->assign(in_route_list.cbegin(), in_route_list.cend());

	continue_flag = 0;
	station_id1 = 0;

	for (route_item = out_route_list->begin(); route_item != out_route_list->end(); route_item++) {
		if (station_id1 != 0) {
			a69_line_id = route_item->lineId;
			continue_flag = Route::Query_a69list(a69_line_id, station_id1, route_item->stationId, &dbrecord, continue_flag);
			for (i = 0; i < (int)(dbrecord.size() - (continue_flag ? 2 : 0)); i += 2) {
				if ((dbrecord.size() - i) < 2) {
					ASSERT(FALSE);
					break;
				}
				a69list.push_back(MAKEPAIR(IDENT1(dbrecord[i]), IDENT1(dbrecord[i + 1])));
				// ’uŠ·ˆ—
				Route::Query_rule69t(in_route_list, *route_item, IDENT2(dbrecord[i]), &trule69list);
				if (1 == trule69list.size()) {
					if (0 == a69list.size()) {
						ASSERT(FALSE);
					} else if (1 == a69list.size()) {
						/* ’uŠ·ˆ—1 */
						if (route_item->stationId == IDENT2(a69list.at(0))) { /* a69list.station2 ? */
							route_item->lineId = trule69list.at(0).at(0);
							if (station_id1 != IDENT1(a69list.at(0))) {       /* a69list.station1 ? */
								/* æÔ˜Hü, ‰w2‚ğ‘}“ü */
								route_item = out_route_list->insert(route_item, RouteItem(a69_line_id, IDENT1(a69list.at(0))));
								route_item++;
							}
						} else {
							if (station_id1 != IDENT1(a69list.at(0))) { /* a69list.station1 ? */
								/* æÔ˜Hü, ‰w1‚ğ‘}“ü */
								route_item = out_route_list->insert(route_item, RouteItem(a69_line_id, IDENT1(a69list.at(0))));
								route_item++;
							}
							/* ŒvZ˜HüA‰w2‚ğ‘}“ü */
							route_item = out_route_list->insert(route_item, RouteItem(trule69list.at(0).at(0), IDENT2(a69list.at(0))));
							route_item++;
						}
					} else if (2 == a69list.size()) {
						/* ’uŠ·ˆ—3 */
						if (route_item->stationId == IDENT2(a69list.back())) {
							route_item->lineId = trule69list.at(0).at(0);
							route_item->stationId = IDENT2(a69list.back());
							if ((route_item - a69list.size())->stationId == IDENT1(a69list.front())) {
								// c) in-in
#if 1
								int j = a69list.size() - 1;
								route_item -= j;
								for ( ; 0 < j; j--) {
									route_item = out_route_list->erase(route_item);
								}
#else	/* ‚±‚Á‚¿‚ÍNG(•Ô‚³‚ê‚éiterator‚ª–³Œø‚Å”ò‚Ô) */
								route_item = out_route_list->erase(route_item - (a69list.size() - 1),
								                                   route_item - (a69list.size() - 1) + (a69list.size() - 1));
#endif
							} else { // d) out-in
								if (2 < a69list.size()) {
									int j = a69list.size() - 2;
									route_item -= j;
									for ( ; 0 < j; j--) {
										route_item = out_route_list->erase(route_item);
									}

									route_item = out_route_list->erase(route_item - (a69list.size() - 2),
									                                   route_item - (a69list.size() - 2) + (a69list.size() - 2));
									// 3Œo˜HˆÈã‚ÍŒ»ó‚È‚µ
								}
								(route_item - 1)->stationId = IDENT1(a69list.front());
							}
						} else if ((route_item - a69list.size())->stationId == IDENT1(a69list.front())) {
							/* b) in -> out */
							(route_item - 1)->lineId = trule69list.at(0).at(0);
							(route_item - 1)->stationId = IDENT2(a69list.back());
							if (2 < a69list.size()) {
#if 1
								int j = a69list.size() - 2;
								route_item -= j;
								for ( ; 0 < j; j--) {
									route_item = out_route_list->erase(route_item);
								}
#else	/* ‚±‚Á‚¿‚ÍNG(iterator‚ª–³Œø) */
								route_item = out_route_list->erase(route_item - (a69list.size() - 1),
								                                   route_item - (a69list.size() - 1) + (a69list.size() - 2));
#endif
								// 3Œo˜HˆÈã‚ÍŒ»ó‚È‚µ
							}
						} else {
							/* a) out -> out */
							route_item = out_route_list->insert(route_item, RouteItem(trule69list.at(0).at(0), IDENT2(a69list.back())));
							route_item++;
							(route_item - a69list.size())->stationId = IDENT1(a69list.front());
							if (2 < a69list.size()) {
#if 1
								int j = a69list.size() - 1;
								route_item -= j;
								j--;
								for ( ; 0 < j; j--) {
									route_item = out_route_list->erase(route_item);
								}
#else	/* ‚±‚Á‚¿‚ÍNG(iterator‚ª–³Œø) */
								route_item = out_route_list->erase(route_item - (a69list.size() - 1),
								                                   route_item - (a69list.size() - 1) + (a69list.size() - 2));
#endif
								// 3Œo˜HˆÈã‚ÍŒ»ó‚È‚µ
							}
						}
					} else {
						ASSERT(FALSE);	/* not implement for 3 <= a69list.size() */
					}
				} else if (0 == trule69list.size()) {
					// ASSERT(FALSE);
					// 69ğ uc‚©‚Â‚±“à‚Ì—¼ü˜H‚É‚Ü‚½‚ª‚éê‡‚ğœ‚¢‚Äv‚Ìê‡
				} else if (2 == trule69list.size()) {
					/* ’uŠ·ˆ—2 */
					if (IDENT1(a69list.at(0)) == trule69list.at(0).at(1)) {
						// ‰º‚èü
						route_item->lineId = trule69list.at(1).at(0);
						route_item = out_route_list->insert(route_item, RouteItem(trule69list.at(0).at(0), trule69list.at(0).at(2)));
						route_item++;
					} else {
						// ã‚èü
						route_item->lineId = trule69list.at(0).at(0);
						route_item = out_route_list->insert(route_item, RouteItem(trule69list.at(1).at(0), trule69list.at(1).at(1)));
						route_item++;
					}
				} else {
					ASSERT(FALSE);	/* 0 or <=2 then failured */
				}
				a69list.clear();
				change++;
			}
 			if (continue_flag) {
 				ASSERT(2 <= dbrecord.size() && i == (dbrecord.size() - 2));
				a69list.push_back(MAKEPAIR(dbrecord[i], dbrecord[i + 1]));
 			}
		}
		station_id1 = route_item->stationId;
	}
	Route::RemoveDupRoute(out_route_list);
	TRACE(change == 0 ? "" : "applied rule69 count=%d\n", change);
	return change;
}



//static
//	d•¡ƒ‹[ƒgœ‹
//
//	       Ÿ‰Y
//  ŠO–[ü ‘h‰ä <- remove
//  ŠO–[ü ç—t
//	‘•ü “Œ‹
//
int Route::RemoveDupRoute(vector<RouteItem> *routeList)
{
	int affects = 0;
#if 1	/* erase(iterator)‚µ‚©‚È‚¢‚Ì‚Å */
	vector<RouteItem>::iterator pos;

	for (pos = routeList->begin(); pos != routeList->end() && (pos + 1) != routeList->end(); ) {
		if (pos->lineId == (pos + 1)->lineId) {
			pos = routeList->erase(pos);
			affects++;
		} else {
			pos++;
		}
	}

#else	/* erase(index)‚ª‚ ‚ê‚Î‚±‚Á‚¿‚Å‚à‰Â */
	int last = routeList->size() - 2;
	int idx = 0;
	
	while (idx < last) {
		if (routeList->at(idx).lineId == routeList->at(idx + 1).lineId) {
			routeList->erase(idx);
			last--;
			affects++;
		} else {
			idx++;
		}
	}
#endif
	return affects;
}

//static 
//	˜Hü‚Ì‰wŠÔ‚É“s‹æs“à‰w‚Í‚ ‚é‚©H
//	     “ŒŠC“¹VŠ²ü ‹“s •ÄŒ´ŠÔ‚É‘’Ã‰w‚Í‘¶İ‚·‚é‚Æ‚µ‚Ä•Ô‚µ‚Ü‚·.
//
//	@param [in] cityno     ŒŸõ‚·‚é“s‹æs“àNo
//	@param [in] lineId     ˜Hü
//	@param [in] stationId1 ŠJn‰w
//	@param [in] stationId2 I—¹‰w
//	@return 0: not found / not 0: ocunt of found.
//
int Route::InCityStation(int cityno, int lineId, int stationId1, int stationId2)
{
	static const char tsql[] =
"select count(*)"
"	from t_lines"
"	where line_id=?1"
"	and (lflg&2147483663)=?4"
"	and sales_km>="
"			(select min(sales_km)"
"			from t_lines"
"			where line_id=?1"
"			and (station_id=?2 or"
"				 station_id=?3))"
"	and sales_km<="
"			(select max(sales_km)"
"			from t_lines"
"			where line_id=?1"
"			and (station_id=?2 or "
"				 station_id=?3))";
//2147483663 = 0x8000000f = (lflg&(1<<31) = 0 and (lflg&15)=?4) 
	DBO dbo = DBS::getInstance()->compileSql(tsql, true);
	if (dbo.isvalid()) {
		dbo.setParam(1, lineId);
		dbo.setParam(2, stationId1);
		dbo.setParam(3, stationId2);
		dbo.setParam(4, cityno & 0x0f);

		if (dbo.moveNext()) {
			return dbo.getInt(0);
		}
	}
	return 0;
}


//static
//	86ğ“K—p‚³‚ê‚é‚©‚Ìƒ`ƒFƒbƒN
//	showFare() => checkOfRuleSpecificCoreLine() =>
//
//	@param [in]  in_route_list Œo˜H
//	@param [out] exit          ’Eo˜HüE‰w
//	@param [out] entr          i“ü˜HüE‰w
//	@param [out] cityId_pair   IDENT1(”­‰w“Á’è“s‹æs“àId), IDENT2(’…‰w“Á’è“s‹æs“àId)
//
//	@return	0: ”ñ“K‰(—¼’[’P‰w)              -
//			1: ”­‰w‚ª“Á’è“s‹æs“à            exit—LŒø
//			2: ’…‰w‚ª“Á’è“s‹æs“à            entr—LŒø
//			3: ”­‰w‚’…‰w‚Æ‚à“Á’è“s‹æs“à    entr, exit —LŒø
//		 0x83: ”­‰w=’…‰w=“Á’è“s‹æs“à        entr, exit —LŒø
//			4: ‘S‰w“¯ˆê“Á’è“s‹æs“à          -
//
//	 [‹ã] R—zVŠ²ü
//	 [•Ÿ] ­™“‡ü
//	 [_] R—zVŠ²ü
//	 [D] ”ŸŠÙü
//	 [å] “Œ–kVŠ²üA“Œ–kü
//	 [L] R—zVŠ²üAR—zü
//	 [ã] “ŒŠC“¹ü
//	 [‹] “ŒŠC“¹ü
//	 [–¼] “ŒŠC“¹ü
//	 [•l] “ŒŠC“¹ü
//	 ‚ÆA“s‹æ“àˆÈŠO‚Í‚İ‚ñ‚È‚ ‚è“¾‚é
// “¯ˆêü‚Å“s‹æs“à‰w(o)‚Ìƒpƒ^[ƒ“
//	--o-- ã‹Lˆê——
//	ooooo ‚ ‚è(–â‘è‚È‚µ)
//	o---- ’Êí
//	----o ’Êí
//	o---o ‚È‚µ(æÔ‰w‚Ü‚½‚Í•ªŠò‰w`•ªŠò‰w‚Ü‚½‚Í~Ô‰w‚ª“s‹æs“à‚¾‚ªŠÔ‚É”ñ“s‹æs“à‚ªŠÜ‚Ü‚ê‚é—á‚Í‚È‚µB
//
int Route::CheckOfRule86(const vector<RouteItem>& in_route_list, Station* exit, Station* entr, PAIRIDENT* cityId_pair)
{
	DbidOf dbid;
	vector<RouteItem>::const_iterator fite;
	vector<RouteItem>::const_reverse_iterator rite;
	int city_no_s;
	int city_no_e;
	int c;
	int r;
	Station in_line;
	Station out_line;
	int stationId;
	int lineId;
	
	r = 0;
	fite = in_route_list.cbegin();
	if (fite == in_route_list.cend()) {
		return 0;	/* empty */
	}
	city_no_s = MASK_CITYNO(fite->flag);
	// ”­‰w‚ª“òè‚Ìê‡‘åãs“à”­‚Å‚Í‚È‚¢@Šî153-2
	if ((city_no_s == CITYNO_OOSAKA) && (dbid.StationIdOf_AMAGASAKI == fite->stationId)) {
		city_no_s = 0;
	}
	
	rite = in_route_list.crbegin();
	if (rite == in_route_list.crend()) {
		ASSERT(FALSE);
		return 0;	/* fatal error */
	}
	city_no_e = MASK_CITYNO(rite->flag);
	// ’…‰w‚ª“òè‚Ìê‡‘åãs“à’…‚Å‚Í‚È‚¢@Šî153-2
	if ((city_no_e == CITYNO_OOSAKA) && (dbid.StationIdOf_AMAGASAKI == rite->stationId)) {
		city_no_e = 0;
	}

	*cityId_pair = MAKEPAIR(city_no_s, city_no_e);

	if (city_no_s != 0) {
		// ”­‰w=“Á’è“s‹æs“à
		c = 0;
		stationId = fite->stationId;	// ”­ 
		for (fite++; fite != in_route_list.cend(); fite++) {
			int cno = MASK_CITYNO(fite->flag);
			if (c == 0) {
				if (cno != city_no_s) {
					c = 1;			// ”²‚¯‚½
					out_line.set(*fite);
				}
			} else {
				if (cno == city_no_s) {
					if (c == 1) {
						c = 2;		// –ß‚Á‚Ä‚«‚½
						in_line.set(*fite);
					} else {
						ASSERT(c == 2);
					}
				} else {
					if ((c == 2) || (0 < InCityStation(city_no_s, fite->lineId, stationId, fite->stationId))) {
						c = 3;		// –ß‚Á‚Ä‚Ü‚½”²‚¯‚½
						break;
					}
					ASSERT(c == 1);
				}
			}
			stationId = fite->stationId;	// ”­ 
		}
		switch (c) {
		case 0:
			return 4;	/* ‘S‰w“Á’è“s‹æs“à */
			break;
		case 1:
			r = 1;		/* ”­‰w“Á’è“s‹æs“à */
			*exit = out_line;
			break;
		case 2:
			*exit = out_line;
			*entr = in_line;
			return 0x83;	/* ”­‰w=’…‰w=“Á’è“s‹æs“à */
			break;
		case 3:
			// 8‚Ìš(‚¢‚Á‚½‚ñ—£‚ê‚Ä–ß‚Á‚Ä‚«‚Ä‚Ü‚½—£‚ê‚½) (’P‰w(“Á’è“s‹æs“à–³Œø))
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}

	if (city_no_e != 0) {
		// ’…‰w=“Á’è“s‹æs“à
		c = 0;
		in_line.set(*rite);
		for (rite++; rite != in_route_list.crend(); rite++) {
			int cno = MASK_CITYNO(rite->flag);
			if (c == 0) {
				if (cno != city_no_e) {
					c = 1;			// 
				} else {
					in_line.set(*rite);
				}
			} else {
				if (cno == city_no_e) {
					if (c == 1) {
						c = 2;
					} else {
						ASSERT(c == 2);
					}
				} else {
					if ((c == 2) || (0 < InCityStation(city_no_e, lineId, stationId, rite->stationId))) {
						c = 3;
						break;
					}
					ASSERT(c == 1);
				}
			}
			stationId = rite->stationId;	// ”­ 
			lineId = rite->lineId;
		}
		switch (c) {
		case 1:
			*entr = in_line;
			r |= 0x02;	/* ’…‰w“Á’è“s‹æs“à(”­‰w‚’…‰w) */
			break;
		case 3:
			// 8‚Ìš(‚¢‚Á‚½‚ñ—£‚ê‚Ä–ß‚Á‚Ä‚«‚Ä‚Ü‚½—£‚ê‚½) (’P‰w(“Á’è“s‹æs“à–³Œø))
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}
	return r;
}

//static
// Rèü“à”­’…‚©‚ğƒ`ƒFƒbƒN
//	showFare() => checkOfRuleSpecificCoreLine() => 
//
//	@param [in] in_route_list  route
//	@return 0  : ”­’…‚Å‚Í‚È‚¢
//			1  : ”­‰w‚ÍRèü“à
//			2  : ’…‰w‚ÍRèü“à
//			3  : (•Ô‚·‚±‚Æ‚Í‚È‚¢)
//		 0x83  : ”­’…‰wRèü“à
//		    4  : ‘S‰w‰wRèü“à
//
int Route::CheckOfRule87(const vector<RouteItem>& in_route_list)
{
	vector<RouteItem>::const_iterator fite;
	vector<RouteItem>::const_reverse_iterator rite;
	int c;
	int r;
	
	r = 0;

	fite = in_route_list.cbegin();
	if ((fite != in_route_list.cend()) && (((fite->flag & ((1 << 10) | (1 << 5))) == ((1 << 10)|(1 << 5))))) {
		// start=Rèü
		c = 0;
		for (fite++; fite != in_route_list.cend(); fite++) {
			if (c == 0) {
				if (((fite->flag & ((1 << 10) | (1 << 5))) != ((1 << 10)|(1 << 5)))) {
					c = 1;	// ”²‚¯‚½
				}
			} else {
				if (((fite->flag & ((1 << 10) | (1 << 5))) == ((1 << 10)|(1 << 5)))) {
							// Rèü“à
					if (c == 1) {
						c = 2;
					} else {
						ASSERT(c == 2);
					}
				} else {
					if (c == 2) {
						c = 3;
						break;
					}
				}
			}
		}
		switch (c) {
		case 0:
			return 4;		/* ‘S‰wRèü“à */
			break;
		case 1:
			r = 1;			/* ”­‰wRèü“à */
			break;
		case 2:
			return 0x83;	/* ”­‰w=’…‰w=Rèü“à */
			break;
		case 3:
			// 8‚Ìš(‚¢‚Á‚½‚ñ—£‚ê‚Ä–ß‚Á‚Ä‚«‚Ä‚Ü‚½—£‚ê‚½) (’P‰w(“Á’è“s‹æs“à–³Œø))
			return 0;		/* ’P‰w‚Æ‚·‚é */
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}
	rite = in_route_list.crbegin();
	if ((rite != in_route_list.crend()) && (((rite->flag & ((1 << 10) | (1 << 5))) == ((1 << 10)|(1 << 5))))) {
		// ’…‰w=Rèü“à
		c = 0;
		for (rite++; rite != in_route_list.crend(); rite++) {
			if (c == 0) {
				if (((rite->flag & ((1 << 10) | (1 << 5))) != ((1 << 10)|(1 << 5)))) {
					c = 1;			// —£‚ê‚½
				} else {
					//
				}
			} else {
				if (((fite->flag & ((1 << 10) | (1 << 5))) == ((1 << 10)|(1 << 5)))) {
									// Rèü“à
					if (c == 1) {
						c = 2;
					} else {
						ASSERT(c == 2);
					}
				} else {
					if (c == 2) {
						c = 3;
						break;
					}
				}
			}
		}
		switch (c) {
		case 1:
			ASSERT(r == 0);		/* ã‚Å0x83‚Å•Ô‚µ‚Ä‚¢‚é‚Í‚¸‚È‚Ì‚Å */
			r |= 0x02;			/* ’…‰wRèü“à */
			break;
		case 3:
			// 8‚Ìš(‚¢‚Á‚½‚ñ—£‚ê‚Ä–ß‚Á‚Ä‚«‚Ä‚Ü‚½—£‚ê‚½) (’P‰w(“Á’è“s‹æs“à–³Œø))
			break;				/* ’P‰w */
		default:
			ASSERT(FALSE);
			break;
		}
	}
	return r;
}


//static
//	i“ü/’Eo˜Hü‚©‚ç86ğ’†S‰w‚©‚çÅ‰‚ÌæŠ·‰w‚Ü‚Å‚Ì˜Hü‚ÆæŠ·‰w‚ğ•Ô‚·
//	showFare() => checkOfRuleSpecificCoreLine() => ReRouteRule86j87j() =>
//
//  @param [in] lineId : i“ü^’Eo˜Hü
//	@param [in] cityId : “s‹æs“àId
//
//  @retval vector<Station> : •Ô‹p”z—ñ(lineId, stationId)
//
//	—áFlineId:í”Öü, cityId:[‹æ] -> “Œ–küA“ú•é—¢
//
vector<Station> Route::SpecificCoreAreaFirstTransferStationBy(int lineId, int cityId)
{
	const static char tsql[] = 
"select station_id, line_id2 from t_rule86 where"
" line_id1=?1 and (city_id & 255)=?2 order by city_id";

	int station_id;
	int line_id;
	vector<Station> firstTransferStation;

	DBO dbo = DBS::getInstance()->compileSql(tsql, true);
	if (dbo.isvalid()) {
		dbo.setParam(1, lineId);
		dbo.setParam(2, cityId);
		while (dbo.moveNext()) {
			station_id = dbo.getInt(0);
			line_id = dbo.getInt(1);
			firstTransferStation.push_back(Station(line_id, station_id));
		}
	}
	return firstTransferStation;
}

//static
//	86ğ’†S‰w‚ğ•Ô‚·
//	showFare() => checkOfRuleSpecificCoreLine() => ReRouteRule86j87j() =>
//	
//  @param [in]  cityId : “s‹æs“àId
//
//  @return ’†S‰w
//
int Route::Retrieve_SpecificCoreStation(int cityId)
{
	int stationId;

	const static char tsql[] = 
"select rowid from t_station where"
" (sflg & (1 << 4))!=0 and (sflg & 15)=?1";

	DBO dbo = DBS::getInstance()->compileSql(tsql, true);
	if (dbo.isvalid()) {
		dbo.setParam(1, cityId);
		if (dbo.moveNext()) {
			stationId = dbo.getInt(0);
			return stationId;
		}
	}
	return 0;
}

//static
//	Route ‰c‹ÆƒLƒ‚ğŒvZ
//	showFare() => checkOfRuleSpecificCoreLine() =>
//	CheckOfRule114j() =>
//
//	@param [in]  route     ŒvZƒ‹[ƒg
//	@retuen ‰c‹ÆƒLƒ[0] ^ ŒvZƒLƒ[1]
//
vector<int> Route::Get_route_distance(const vector<RouteItem>& route)
{
	vector<RouteItem>::const_iterator it;
	int total_sales_km;
	int total_calc_km;
	int stationId;

	it = route.cbegin();
	total_sales_km = 0;
	total_calc_km = 0;
	stationId = 0;
	while (it != route.cend()) {
		if (stationId != 0) {
			vector<int> km = Route::GetDistance(it->lineId, stationId, it->stationId);
			ASSERT(km.size() == 2);
			total_sales_km += km.at(0);
			total_calc_km  += km.at(1);
		}
		stationId = it->stationId;
		it++;
	}
	vector<int> v;
	v.push_back(total_sales_km);
	v.push_back(total_calc_km);
	return v;
}

//static
//	86, 87ğ“K—p•ÏŠ·
//	showFare() => checkOfRuleSpecificCoreLine() =>
//	route_list_raw => route_list_cooked
//
//	@param [in]     cityId      core id
//
//	@param [in]     mode
//						0: ”ñ“K‰(—¼’[’P‰w)              steady
//						1: ”­‰w‚ª“Á’è“s‹æs“à            modified
//						2: ’…‰w‚ª“Á’è“s‹æs“à            modified
//						3: ”­‰w‚’…‰w‚Æ‚à“Á’è“s‹æs“à    modified
//					 0x83: ”­‰w=’…‰w=“Á’è“s‹æs“à        modified
//						4: ‘S‰w“¯ˆê“Á’è“s‹æs“à          steady
//
//	@param [in]     exit            ’Eo˜HüE‰w
//	@param [in]     entr            i“ü˜HüE‰w
//	@param [in/out] out_route_list  •ÏŠ·Œ³E•ÏŠ·æŒo˜H
//
//	@remark ‚±‚Ì‚ ‚Æ69ğ“K—p‚ª•K—v(69ğ“K—pŒã‚Å‚à‰Â)
//	@remark ƒ‹[ƒ‹–¢“K—p‚Íroute_list_cooked = route_list_raw‚Å‚ ‚é
//
void  Route::ReRouteRule86j87j(PAIRIDENT cityId, int mode, const Station& exit, const Station& enter, vector<RouteItem>* out_route_list)
{
	int coreStationId;
	bool skip;
	int lineId;
	vector<RouteItem>::const_iterator itr;
	vector<RouteItem> work_route_list;
	vector<Station> firstTransferStation;
	
	if ((mode & 1) != 0) {
		/* ”­‰w-’Eo: exit —LŒø*/		/* ex) [[“Œ–küA“ú•é—¢]] = (í”Öü, [‹æ]) */
		firstTransferStation = Route::SpecificCoreAreaFirstTransferStationBy(exit.lineId, IDENT1(cityId));
		if (firstTransferStation.size() <= 0) {
			ASSERT(FALSE);				/* ƒe[ƒuƒ‹–¢’è‹` */
			work_route_list.assign(out_route_list->cbegin(), out_route_list->cend());
		} else {
			vector<Station>::const_reverse_iterator sta_ite;
			sta_ite = firstTransferStation.crbegin();
			if (exit.lineId == sta_ite->lineId) {
				work_route_list.push_back(RouteItem(0, sta_ite->stationId));	/* ”­‰w:“s‹æs“à‘ã•\‰w */
				// ASSERT(sta_ite->stationId == Route::Retrieve_SpecificCoreStation(IDENT(cityId))); (V‰¡•l‚Æ‚©V_ŒË‚ª‚ ‚é‚Ì‚Å)
			} else {
				coreStationId = Route::Retrieve_SpecificCoreStation(IDENT1(cityId));
				ASSERT(0 < coreStationId);
				work_route_list.push_back(RouteItem(0, coreStationId));			/* ”­‰w:“s‹æs“à‘ã•\‰w */
				while (sta_ite != firstTransferStation.crend()) {
					work_route_list.push_back(RouteItem(sta_ite->lineId, sta_ite->stationId));
					++sta_ite;
				}
			}
		}
		for (skip = true, itr = out_route_list->cbegin(); itr != out_route_list->cend(); ++itr) {
			if (skip) {
				if (exit.is_equal(*itr)) {
					skip = false;
					work_route_list.push_back(*itr);
				}/* else  ’Eo ˜Hü:‰w‚Ì‘O‚ÌŒo˜H‚ğƒXƒLƒbƒv*/
			} else {
				work_route_list.push_back(*itr);
			}
		}
		TRACE("start station is re-route rule86/87\n");
	} else {
		work_route_list.assign(out_route_list->cbegin(), out_route_list->cend());
	}

	firstTransferStation.clear();
	out_route_list->clear();

	if ((mode & 2) != 0) {
		/* ’…‰w-i“ü: entr —LŒø */
		itr = work_route_list.cbegin();
		while (itr != work_route_list.cend()) {
			if (enter.is_equal(*itr)) {
				break;
			} else {
				out_route_list->push_back(*itr);
			}
			++itr;
		}

		firstTransferStation = Route::SpecificCoreAreaFirstTransferStationBy(enter.lineId, IDENT2(cityId));
		if (firstTransferStation.size() <= 0) {
			ASSERT(FALSE);
			out_route_list->assign(work_route_list.cbegin(), work_route_list.cend());
		} else {
			vector<Station>::const_iterator sta_ite;
			sta_ite = firstTransferStation.cbegin();
			ASSERT(sta_ite != firstTransferStation.cend());
			if (enter.lineId == sta_ite->lineId) {
				out_route_list->push_back(RouteItem(sta_ite->lineId, sta_ite->stationId));
				// ASSERT(sta_ite->stationId == Route::Retrieve_SpecificCoreStation(IDENT2(cityId))); V‰¡•l‚Æ‚©‚ ‚é‚Ì‚Å
			} else {
				out_route_list->push_back(RouteItem(enter.lineId, sta_ite->stationId));
				lineId = sta_ite->lineId;
				while (++sta_ite != firstTransferStation.cend()) {
					out_route_list->push_back(RouteItem(lineId, sta_ite->stationId));
					lineId = sta_ite->lineId;
				}
				coreStationId = Route::Retrieve_SpecificCoreStation(IDENT2(cityId));
				ASSERT(0 < coreStationId);
				out_route_list->push_back(RouteItem(lineId, coreStationId));
			}
			TRACE("end station is re-route rule86/87\n");
		}
	} else {
		out_route_list->assign(work_route_list.cbegin(), work_route_list.cend());
	}
}

//public
//	86A87ğA69ğ”»’è•Œo˜HZo
//	showFare() =>
//	route_list_raw => route_list_cooked
//
//	Œã”¼‚ÅB1LID_xxx(route[0].lineId)‚ğİ’è‚µ‚Ü‚·
//
//	@param [in]  dis_cityflag     bit0:1 = ”­‰w:’…‰w –³Œø(0)/—LŒø(1)
//	@param [out] rule114	 [0] = ‰^’À, [1] = ‰c‹ÆƒLƒ, [2] = ŒvZƒLƒ
//	@return false : rule 114 no applied. true: rule 114 applied(available for rule114[] )
//	@remark ƒ‹[ƒ‹–¢“K—p‚Íroute_list_cooked = route_list_raw‚Å‚ ‚é
//
bool Route::checkOfRuleSpecificCoreLine(int dis_cityflag, int* rule114)
{
	PAIRIDENT cityId;
	int sales_km;
	int skm;
	vector<RouteItem> route_list_tmp;
	vector<RouteItem> route_list_tmp2;
	vector<RouteItem> route_list_tmp3;
	vector<RouteItem> route_list_tmp4;
	Station enter;
	Station exit;
	int n;
	int sk;		/* 114 check 90km or 190km */
	int sk2;	/* begin and arrive point as city, 101km or 201km */
	int chk;	/* 86 applied flag */
	int rtky;	/* 87 applied flag */
	bool is114;
	int flg;
	int aply88;

	// 69‚ğ“K—p‚µ‚½‚à‚Ì‚ğroute_list_tmp2‚Ö
	n = Route::ReRouteRule69j(route_list_raw, &route_list_tmp);	/* 69ğ“K—p(route_list_raw->route_list_tmp) */
	TRACE("Rule 69 applied %dtimes.\n", n);

	// route_list_tmp2 = route_list_tmp
	// 70‚ğ“K—p‚µ‚½‚à‚Ì‚ğroute_list_tmp2‚Ö
	n = Route::ReRouteRule70j(route_list_tmp, &route_list_tmp2);
	TRACE(0 == n ? "Rule70 applied.\n" : "Rule70 not applied.\n");

	// 88‚ğ“K—p‚µ‚½‚à‚Ì‚ğroute_list_tmp‚Ö
	aply88 = Route::CheckOfRule88j(&route_list_tmp2);
	if (0 != aply88) {
		if ((aply88 & 1) != 0) {
			TRACE("Apply to rule88 for start.\n");
			route_list_tmp2.at(0).lineId = (1 << B1LID_BEGIN_OOSAKA);
		} else if ((aply88 & 2) != 0) {
			TRACE("Apply to rule88 for arrive.\n");
			route_list_tmp2.at(0).lineId = (1 << B1LID_FIN_OOSAKA);
		}
	}

	/* “Á’è“s‹æs“à”­’…‰Â”Û”»’è */
	chk = Route::CheckOfRule86(route_list_tmp2, &exit, &enter, &cityId);
	TRACE("RuleSpecific:chk 0x%0x, %d -> %d\n", chk, IDENT1(cityId), IDENT2(cityId));
	
	/* 86, 87“K—p‰Â”\«‚ ‚èH */
	if ((chk == 4) || (chk == 0)) {	/* ‘S‰w“Á’è“s‹æs“à‰w or ”­’…‚Æ‚à“Á’è“s‹æs“à‰w‚Å‚È‚¢ê‡ */
		/* –¢•ÏŠ· */
		TRACE("no applied for rule86/87\n");
		route_list_cooked.assign(route_list_tmp2.cbegin(), route_list_tmp2.cend());
		return false;			// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	}

	/* (”­‰w=“s‹æs“à or ’…‰w=“s‹æs“à) 
	 */
	/* route_list_tmp = route_list_tmp2 */
	route_list_tmp.assign(route_list_tmp2.cbegin(), route_list_tmp2.cend());

	/* •ÏŠ· -> route_list_tmp:86“K—p(‰¼) 
	   88•ÏŠ·‚µ‚½‚à‚Ì‚Í‘ÎÛŠO(=R—zVŠ²ü V‘åã’…A”ñ•\¦ƒtƒ‰ƒO‚ªÁ‚¦‚Ä‚µ‚Ü‚¤‚Ì‚ğ”ğ‚¯‚éŒø‰Ê‚ ‚è) */
	Route::ReRouteRule86j87j(cityId, chk & ~aply88, exit, enter, &route_list_tmp);

	// 88‚ğ“K—p
	aply88 = Route::CheckOfRule88j(&route_list_tmp);
	if (0 != aply88) {
		if ((aply88 & 1) != 0) {
			TRACE("Apply to rule88(2) for start.\n");
			route_list_tmp.at(0).lineId = (1 << B1LID_BEGIN_OOSAKA);
		} else if ((aply88 & 2) != 0) {
			TRACE("Apply to rule88(2) for arrive.\n");
			route_list_tmp.at(0).lineId = (1 << B1LID_FIN_OOSAKA);
		}
	}

	// 69‚ğ“K—p‚µ‚½‚à‚Ì‚ğroute_list_tmp3‚Ö
	n = Route::ReRouteRule69j(route_list_tmp, &route_list_tmp3);	/* 69ğ“K—p(route_list_tmp->route_list_tmp3) */
	TRACE("Rule 69(2) applied %dtimes.\n", n);

	/* route_list_tmp	70-88-69-86“K—p
	 * route_list_tmp2	70-88-69“K—p
	 * route_list_tmp3	70-88-69-86-69“K—p
	 */
	/* compute of sales_km by route_list_cooked */
	sales_km = Route::Get_route_distance(route_list_tmp3).at(0);
	
	if (2000 < sales_km) {
		/* <<<“s‹æs“à“K—p>>> */
		/* 201km <= sales_km */
		/* enable */
		route_list_tmp3.at(0).lineId = (chk & 0x03);	// B1LID_BEGIN_CITY, B1LID_FIN_CITY
		TRACE("applied for rule86(%d)\n", chk & 0x03);

		// route_list_cooked = route_list_tmp3
		route_list_cooked.assign(route_list_tmp3.cbegin(), route_list_tmp3.cend());

		return false;			// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	}
	
	/* 101km - 200km : Rèü”­’… or 200kmˆÈ‰º‚Ì“s‹æs“àŠÔ(–¼ŒÃ‰®-‘åã‚È‚Ç)ƒ`ƒFƒbƒN */
	rtky = Route::CheckOfRule87(route_list_tmp2);
	if ((3 & rtky) != 0) {
		/* apply to 87 */
		if (1000 < sales_km) {
			/* Rèü“à”­’… enable */
			route_list_tmp3.at(0).lineId = ((rtky & 0x03) << 2);	// B1LID_BEGIN_YAMATE, B1LID_FIN_YAMATE
			TRACE("applied for rule87\n");

			// route_list_cooked = route_list_tmp3
			route_list_cooked.assign(route_list_tmp3.cbegin(), route_list_tmp3.cend());
			return false;			// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		}
		sk = 900;	/* 90km */
	} else {
		/* can't apply to 87 */
		sk = 1900;	/* 190km */
	}

	// route_list_tmp4 = route_list_tmp3
	route_list_tmp4.assign(route_list_tmp3.cbegin(), route_list_tmp3.cend());

	/* ”­’…‚Æ‚à“s‹æs“à? */
	if ((0x03 & (rtky | chk)) == 3) { /* –¼ŒÃ‰®s“à-‘åãs“à‚È‚Ç([–¼]-[ã]A[‹ã]-[•Ÿ]A[‹æ]-[•l]) */
							/*  [‹æ]-[‹æ], [–¼]-[–¼], [R]-[‹æ], ... */
		for (sk2 = 2000; true; sk2 = 1000) {
			flg = 0;

			/* route_list_tmp = route_list_tmp2 */
			route_list_tmp.assign(route_list_tmp2.cbegin(), route_list_tmp2.cend());
			/* ”­‰w‚Ì‚İ“Á’è“s‹æs“à’…Œo˜H‚É•ÏŠ· */
			Route::ReRouteRule86j87j(cityId, 1, exit, enter, &route_list_tmp);

			// 69‚ğ“K—p‚µ‚½‚à‚Ì‚ğroute_list_tmp3‚Ö
			n = Route::ReRouteRule69j(route_list_tmp, &route_list_tmp3);	/* 69ğ“K—p(route_list_tmp->route_list_tmp3) */
			TRACE("Rule 69(3) applied %dtimes.\n", n);

			/* ”­‰w‚Ì‚İ“s‹æs“à‚É‚µ‚Ä‚à201/101kmˆÈã‚©H */
			skm = Route::Get_route_distance(route_list_tmp3).at(0);
			if (sk2 < skm) {
				// ”­ “s‹æs“à—LŒø
				flg = 0x01;
			}

			/* route_list_tmp = route_list_tmp2 */
			route_list_tmp.assign(route_list_tmp2.cbegin(), route_list_tmp2.cend());
			/* ’…‰w‚Ì‚İ“Á’è“s‹æs“à’…Œo˜H‚É•ÏŠ·‰¼“K—p */
			Route::ReRouteRule86j87j(cityId, 2, exit, enter, &route_list_tmp);

			// 69‚ğ“K—p‚µ‚½‚à‚Ì‚ğroute_list_tmp3‚Ö
			n = Route::ReRouteRule69j(route_list_tmp, &route_list_tmp3);	/* 69ğ“K—p(route_list_tmp->route_list_tmp3) */
			TRACE("Rule 69(4) applied %dtimes.\n", n);

			/* ’…‰w‚Ì‚İ“s‹æs“à‚É‚µ‚Ä‚à201/101kmˆÈã‚©H */
			skm = Route::Get_route_distance(route_list_tmp3).at(0);
			if (sk2 < skm) {
				// ’… “s‹æs“à—LŒø
				flg |= 0x02;
			}
			if (flg == 0x03) {	/* ”­E’…‚Æ‚à200km‰z‚¦‚¾‚ªA“s‹æs“àŠÔ‚Í200kmˆÈ‰º */
				if (0 != (dis_cityflag & APPLIED_START)) {
					/* ”­‚Ì‚İ“s‹æs“à“K—p */
					/* route_list_tmp = route_list_tmp2 */
					route_list_tmp.assign(route_list_tmp2.cbegin(), route_list_tmp2.cend());
					/* ”­‰w‚Ì‚İ“Á’è“s‹æs“à’…Œo˜H‚É•ÏŠ· */
					Route::ReRouteRule86j87j(cityId, 1, exit, enter, &route_list_tmp);

					// 69‚ğ“K—p‚µ‚½‚à‚Ì‚ğroute_list_tmp3‚Ö
					n = Route::ReRouteRule69j(route_list_tmp, &route_list_tmp3);	/* 69ğ“K—p(route_list_tmp->route_list_tmp3) */
					TRACE("Rule 69(5) applied %dtimes.\n", n);

					/* ”­‰wE’…‰w“Á’è“s‹æs“à‚¾‚ª”­‰w‚Ì‚İ“s‹æs“à“K—p */
					if (sk == 900) {
						TRACE("applied for rule87(start)\n");
						route_list_tmp3.at(0).lineId = (1 << B1LID_BEGIN_YAMATE) | (1 << B1LID_BEGIN_CITY_OFF);
					} else {
						TRACE("applied for rule86(start)\n");
						route_list_tmp3.at(0).lineId = (1 << B1LID_BEGIN_CITY) | (1 << B1LID_BEGIN_CITY_OFF);
					}
					// route_list_cooked = route_list_tmp3
					route_list_cooked.assign(route_list_tmp3.cbegin(), route_list_tmp3.cend());
					return false;			// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
				} else {
					/* ’…‚Ì‚İ“s‹æs“à“K—p */
					/* ”­‰wE’…‰w“Á’è“s‹æs“à‚¾‚ª’…‰w‚Ì‚İ“s‹æs“à“K—p */
					if (sk == 900) {
						TRACE("applied for rule87(end)\n");
						route_list_tmp3.at(0).lineId = (1 << B1LID_FIN_YAMATE) | (1 << B1LID_FIN_CITY_OFF);
					} else {
						TRACE("applied for rule86(end)\n");
						route_list_tmp3.at(0).lineId = (1 << B1LID_FIN_CITY) | (1 << B1LID_FIN_CITY_OFF);
					}
					// route_list_cooked = route_list_tmp3
					route_list_cooked.assign(route_list_tmp3.cbegin(), route_list_tmp3.cend());
					return false;			// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
				}
			} else if (flg == 0x01) {
				/* route_list_tmp = route_list_tmp2 */
				route_list_tmp.assign(route_list_tmp2.cbegin(), route_list_tmp2.cend());
				/* ”­‰w‚Ì‚İ“Á’è“s‹æs“à’…Œo˜H‚É•ÏŠ· */
				Route::ReRouteRule86j87j(cityId, 1, exit, enter, &route_list_tmp);

				// 69‚ğ“K—p‚µ‚½‚à‚Ì‚ğroute_list_tmp3‚Ö
				n = Route::ReRouteRule69j(route_list_tmp, &route_list_tmp3);	/* 69ğ“K—p(route_list_tmp->route_list_tmp3) */
				TRACE("Rule 69(6) applied %dtimes.\n", n);

				/* ”­‰wE’…‰w“Á’è“s‹æs“à‚¾‚ª”­‰w‚Ì‚İ“s‹æs“à“K—p */
				if (sk == 900) {
					TRACE("applied for rule87(start)\n");
					route_list_tmp3.at(0).lineId = (1 << B1LID_BEGIN_YAMATE);
				} else {
					TRACE("applied for rule86(start)\n");
					route_list_tmp3.at(0).lineId = (1 << B1LID_BEGIN_CITY);
				}
				// route_list_cooked = route_list_tmp3
				route_list_cooked.assign(route_list_tmp3.cbegin(), route_list_tmp3.cend());
				return false;			// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
			} else if (flg == 0x02) {
				/* ”­‰wE’…‰w“Á’è“s‹æs“à‚¾‚ª’…‰w‚Ì‚İ“s‹æs“à“K—p */
				if (sk == 900) {
					TRACE("applied for rule87(end)\n");
					route_list_tmp3.at(0).lineId = (1 << B1LID_FIN_YAMATE);
				} else {
					TRACE("applied for rule86(end)\n");
					route_list_tmp3.at(0).lineId = (1 << B1LID_FIN_CITY);
				}
				// route_list_cooked = route_list_tmp3
				route_list_cooked.assign(route_list_tmp3.cbegin(), route_list_tmp3.cend());
				return false;			// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
			}
			/* flg == 0 */
			if ((sk != 900) || (sk2 == 1000)) {
				/* 87–³Œø or ‰c‹ÆƒLƒ200km‰z‚¦”»’è‚ÍÏ */
				break;
			}
		}
		/* passthru */
	}

	/* route_list_tmp	x
	 * route_list_tmp2	70-88-69“K—p
	 * route_list_tmp3	x
	 * route_list_tmp4	70-88-69-86-69“K—p
	 * route_list_cooked ‹ó
	 */

	/* –¢•ÏŠ· */
	TRACE("no applied for rule86/87(sales_km=%d)\n", sales_km);

	if (sk <= sales_km) {
			/* 114ğ“K—p‚©ƒ`ƒFƒbƒN */
		if ((0x03 & chk) == 3) {
			is114 =					/* 86,87“K—p‘O,   86,87“K—pŒã */
			(Route::CheckOfRule114j(route_list_tmp2, route_list_tmp4,
									0x01 | ((sk2 == 2000) ? 0 : 0x8000), 
									rule114)) ||
			(Route::CheckOfRule114j(route_list_tmp2, route_list_tmp4,
									0x02 | ((sk2 == 2000) ? 0 : 0x8000),
									rule114));
		} else {
			ASSERT(((0x03 & chk) == 1) || ((0x03 & chk) == 2));
			is114 =
			Route::CheckOfRule114j(route_list_tmp2, route_list_tmp4,
								   (chk & 0x03) | ((sk == 1900) ? 0 : 0x8000),
								   rule114);
		}
	} else {
		is114 = false;
	}
	/* 86-87”ñ“K—p */
	// route_list_cooked = route_list_tmp2
	route_list_cooked.assign(route_list_tmp2.cbegin(), route_list_tmp2.cend());
	return is114;
}


//static:
//	88ğ‚Ìƒ`ƒFƒbƒN‚Æ•ÏŠ·
//	V‘åã”­i’…j-[“ŒŠC“¹ü]-_ŒË-[R—zü]-•P˜HˆÈ‰“‚È‚çAV‘åã¨‘åã’uŠ·
//	‘åã-[“ŒŠC“¹ü]-V‘åã-[R—zVŠ²ü]-
//	‚Ìê‡A‘åã-[“ŒŠC“¹ü]-V‘åã‚Ì‹æŠÔ‚ğŠO‚·i201kmˆÈã‚È‚çŠO‚³‚È‚¢)
//	(‚±‚ÌŠÖ”‚Ö‚Í201kmˆÈã‚Ì86ğ“K—p‚É‚ÍŒÄ‚Î‚ê‚é‚±‚Æ‚Í‚È‚¢)
//	B1LID_xxx(route[0].lineId)‚Ìİ’è‘O‚Å‚ ‚é‚±‚Æ‚ğ‘O’ñ(0‚ğİ’è)‚Æ‚µ‚Ü‚·
//
//	@param [in/out] route    route
//	
//	@retval 0: no-convert
//	@retval 1: change start
//	@retval 2: change arrived
//
int Route::CheckOfRule88j(vector<RouteItem> *route)
{
	DbidOf	dbid;
	int lastIndex;
	static int chk_distance1 = 0;
	static int chk_distance2 = 0;
	
	lastIndex = route->size() - 1;

	if (!chk_distance1) {	/* chk_distance: R—zü _ŒË-•P˜HŠÔ‰c‹ÆƒLƒ, VŠ²ü V‘åã-•P˜H */
		chk_distance1 = Route::GetDistance(dbid.LineIdOf_SANYO, dbid.StationIdOf_KOUBE, dbid.StationIdOf_HIMEJI)[0];
		chk_distance2 = Route::GetDistance(dbid.LineIdOf_SANYOSHINKANSEN, dbid.StationIdOf_SHINOSAKA, dbid.StationIdOf_HIMEJI)[0];
	}

	if (2 <= lastIndex) {
		    // V‘åã ”­ “ŒŠC“¹ü - R—zü
		if ((route->front().stationId == dbid.StationIdOf_SHINOSAKA) && 
			(route->at(1).lineId == dbid.LineIdOf_TOKAIDO) &&
		    (route->at(2).lineId == dbid.LineIdOf_SANYO) &&
		    (chk_distance1 <= Route::GetDistance(dbid.LineIdOf_SANYO, dbid.StationIdOf_KOUBE, route->at(2).stationId)[0])) {

			ASSERT(route->at(1).stationId == dbid.StationIdOf_KOUBE);
			/*	V‘åã”­“ŒŠC“¹ü-R—zü-•P˜HˆÈ‰“‚È‚ç”­‰w‚ğV‘åã->‘åã‚Ö */
			route->front() = RouteItem(0, dbid.StationIdOf_OSAKA);	// V‘åã->‘åã

			return 1;
		}	// V‘åã ’… R—zü - “ŒŠC“¹ü
		else if ((route->back().stationId == dbid.StationIdOf_SHINOSAKA) && 
				 (route->back().lineId == dbid.LineIdOf_TOKAIDO) &&
				 (route->at(lastIndex - 1).lineId == dbid.LineIdOf_SANYO) &&
		    	 (chk_distance1 <= Route::GetDistance(dbid.LineIdOf_SANYO, dbid.StationIdOf_KOUBE, route->at(lastIndex - 2).stationId)[0])) {

			ASSERT(route->at(lastIndex - 1).stationId == dbid.StationIdOf_KOUBE);
			/*	V‘åã’…“ŒŠC“¹ü-R—zü-•P˜HˆÈ‰“‚È‚ç’…‰w‚ğV‘åã->‘åã‚Ö */
			route->back() = RouteItem(dbid.LineIdOf_TOKAIDO, dbid.StationIdOf_OSAKA);	// V‘åã->‘åã

			return 2;
		}
		    // ‘åã ”­ V‘åã Œo—R R—zVŠ²ü
		if ((route->front().stationId == dbid.StationIdOf_OSAKA) && 
			(route->at(2).lineId == dbid.LineIdOf_SANYOSHINKANSEN) &&
			(route->at(1).stationId == dbid.StationIdOf_SHINOSAKA) &&
			(chk_distance2 <= Route::GetDistance(dbid.LineIdOf_SANYOSHINKANSEN, dbid.StationIdOf_SHINOSAKA, route->at(2).stationId)[0])) {

			ASSERT(route->at(1).lineId == dbid.LineIdOf_TOKAIDO);

			/* ‘åã”­-“ŒŠC“¹üã‚è-V‘åã-R—zVŠ²ü •P˜HˆÈ‰“‚Ìê‡A‘åã”­-“ŒŠC“¹ü-R—zü ¼–¾ÎŒo—R‚É•t‚¯‘Ö‚¦‚é */
			
			route->at(1) = RouteItem(dbid.LineIdOf_TOKAIDO, dbid.StationIdOf_KOUBE);
			route->at(1).flag |= (FLG_HIDE_LINE | FLG_HIDE_STATION);

			vector<RouteItem>::iterator ite = route->begin();
			ite += 2;	// at(2)						// R—zü-¼–¾Î
			ite = route->insert(ite, RouteItem(dbid.LineIdOf_SANYO, dbid.StationIdOf_NISHIAKASHI));
			ite->flag |= (FLG_HIDE_LINE | FLG_HIDE_STATION);

			return 1;
		}	// R—zVŠ²ü V‘åã Œo—R ‘åã ’…
		else if ((route->back().stationId == dbid.StationIdOf_OSAKA) && 
				 (route->at(lastIndex - 1).stationId == dbid.StationIdOf_SHINOSAKA) &&
				 (route->at(lastIndex - 1).lineId == dbid.LineIdOf_SANYOSHINKANSEN) &&
				 (chk_distance2 <= Route::GetDistance(dbid.LineIdOf_SANYOSHINKANSEN, dbid.StationIdOf_SHINOSAKA, route->at(lastIndex - 2).stationId)[0])) {

			ASSERT((route->back().lineId == dbid.LineIdOf_TOKAIDO));

			/* R—zVŠ²ü •P˜HˆÈ‰“`V‘åãæŠ·“ŒŠC“¹ü-‘åã’…‚Ìê‡AÅŒã‚Ì“ŒŠC“¹ü-‘åã ‚ğ¼–¾Î R—züA“ŒŠC“¹ü‚É•t‚¯‘Ö‚¦‚é */

			route->at(lastIndex - 1) = RouteItem(dbid.LineIdOf_SANYOSHINKANSEN, dbid.StationIdOf_NISHIAKASHI);	// V‘åã->¼–¾Î
			route->at(lastIndex - 1).flag |= FLG_HIDE_STATION;
			route->at(lastIndex).flag |= FLG_HIDE_LINE;	// “ŒŠC“¹ü ”ñ•\¦
			vector<RouteItem>::iterator ite = route->end();
			ite--;
			ite = route->insert(ite, RouteItem(dbid.LineIdOf_SANYO, dbid.StationIdOf_KOUBE));
			ite->flag |= (FLG_HIDE_LINE | FLG_HIDE_STATION);

			return 2;
		}
	}
	if (1 <= lastIndex) {
		    // V‘åã ”­ R—zVŠ²ü
		if ((route->front().stationId == dbid.StationIdOf_SHINOSAKA) && 
			(route->at(1).lineId == dbid.LineIdOf_SANYOSHINKANSEN) &&
			(chk_distance2 <= Route::GetDistance(dbid.LineIdOf_SANYOSHINKANSEN, dbid.StationIdOf_SHINOSAKA, route->at(1).stationId)[0])) {

			/* ‘åã”­-“ŒŠC“¹üã‚è-V‘åã-R—zVŠ²ü •P˜HˆÈ‰“‚Ìê‡A‘åã”­-“ŒŠC“¹ü-R—zü ¼–¾ÎŒo—R‚É•t‚¯‘Ö‚¦‚é */
			
			vector<RouteItem>::iterator ite = route->begin();
			*ite = RouteItem(0, dbid.StationIdOf_OSAKA);
			ite++;
			ite = route->insert(ite, RouteItem(dbid.LineIdOf_SANYO, dbid.StationIdOf_NISHIAKASHI));
			ite->flag |= (FLG_HIDE_LINE | FLG_HIDE_STATION);

			ite = route->insert(ite, RouteItem(dbid.LineIdOf_TOKAIDO, dbid.StationIdOf_KOUBE));
			ite->flag |= (FLG_HIDE_LINE | FLG_HIDE_STATION);

			return 1;

		}	// R—zVŠ²ü ‘åã ’…
		else if ((route->back().stationId == dbid.StationIdOf_SHINOSAKA) && 
				 (route->back().lineId == dbid.LineIdOf_SANYOSHINKANSEN) &&
				 (chk_distance2 <= Route::GetDistance(dbid.LineIdOf_SANYOSHINKANSEN, dbid.StationIdOf_SHINOSAKA, route->at(lastIndex - 1).stationId)[0])) {

			/* R—zVŠ²ü •P˜HˆÈ‰“`V‘åãæŠ·“ŒŠC“¹ü-‘åã’…‚Ìê‡AÅŒã‚Ì“ŒŠC“¹ü-‘åã ‚ğ¼–¾Î R—züA“ŒŠC“¹ü‚É•t‚¯‘Ö‚¦‚é */

			route->back() = RouteItem(dbid.LineIdOf_SANYOSHINKANSEN, dbid.StationIdOf_NISHIAKASHI);	// V‘åã->¼–¾Î
			route->back().flag |= FLG_HIDE_STATION;

			route->push_back(RouteItem(dbid.LineIdOf_SANYO, dbid.StationIdOf_KOUBE));	// add R—zü-_ŒË
			route->back().flag |= (FLG_HIDE_LINE | FLG_HIDE_STATION);

			route->push_back(RouteItem(dbid.LineIdOf_TOKAIDO, dbid.StationIdOf_OSAKA));	// add “ŒŠC“¹ü-‘åã
			route->back().flag |= FLG_HIDE_LINE;

			return 2;
		}
	}
	return 0;
}


//static:
//	showFare() => calc_fare() =>
//	CheckOfRule114j() => calc_fare() =>
//
//	89ğ‚Ìƒ`ƒFƒbƒN‚Æ•ÏŠ·
//	–kV’n”­i’…j-[JR“Œ¼ü]-“òè‚È‚çA–kV’n¨‘åã’uŠ·
//
//	@param [in] route    route
//	
//	@retval 0: no-convert
//	@retval <0: ‘åã-“òè‚Ì‰c‹ÆƒLƒ - –kV’n-“òè‚Ì‰c‹ÆƒLƒ(•‰”)
//
int Route::CheckOfRule89j(const vector<RouteItem>& route)
{
	DbidOf dbid;
	int lastIndex;
	static int distance = 0;
	
	lastIndex = route.size() - 1;
	if (lastIndex < 2) {
		return 0;
	}

				// –kV’n”­
	if (((route.front().stationId == dbid.StationIdOf_KITASHINCHI) && 
		(route.at(1).stationId == dbid.StationIdOf_AMAGASAKI)) && 
		((lastIndex <= 1) || (route.at(2).lineId != dbid.LineIdOf_TOKAIDO) || 
			(LDIR_ASC == Route::DirLine(dbid.LineIdOf_TOKAIDO, dbid.StationIdOf_AMAGASAKI, route.at(2).stationId)))) {
		/* –kV’n-(JR“Œ¼ü)-“òè ‚Ìê‡A”­‰wi–kV’nj‚ğ‘åã‚É */
		// route.front().stationId = dbid.StationIdOf_OSAKA;
		if (distance == 0) {
			distance = Route::GetDistance(dbid.LineIdOf_TOKAIDO,   dbid.StationIdOf_OSAKA, dbid.StationIdOf_AMAGASAKI)[0] -
					   Route::GetDistance(route.at(1).lineId, dbid.StationIdOf_AMAGASAKI, dbid.StationIdOf_KITASHINCHI)[0];
		}
		ASSERT(distance < 0);
		return distance;
	}			// –kV’nI’…
	else if (((route.back().stationId == dbid.StationIdOf_KITASHINCHI) && 
			 (route.at(lastIndex - 1).stationId == dbid.StationIdOf_AMAGASAKI)) && 
			 ((lastIndex <= 1) || (route.at(lastIndex - 1).lineId != dbid.LineIdOf_TOKAIDO) || 
		(LDIR_DESC == Route::DirLine(dbid.LineIdOf_TOKAIDO, route.at(lastIndex - 2).stationId, dbid.StationIdOf_AMAGASAKI)))) {
		//route.back().stationId = dbid.StationIdOf_OSAKA;
		if (distance == 0) {
			distance = Route::GetDistance(dbid.LineIdOf_TOKAIDO,   dbid.StationIdOf_OSAKA, dbid.StationIdOf_AMAGASAKI)[0] -
					   Route::GetDistance(route.back().lineId, dbid.StationIdOf_AMAGASAKI, dbid.StationIdOf_KITASHINCHI)[0];
		}
		ASSERT(distance < 0);
		return distance;
	} else {
		return 0;
	}
}

//static
//public
//	114ğ‚Ìƒ`ƒFƒbƒN
//
//	@param [in] route         Œo˜H(86, 87•ÏŠ·‘O)
//	@param [in] routeSpecial  Œo˜H(86, 87•ÏŠ·Œã)
//	@param [in] kind       í•Ê
//							bit0:”­‰w‚ª“Á’è“s‹æs“à‚Ü‚½‚ÍRèü“à
//							bit1:’…‰w‚ª“Á’è“s‹æs“à‚Ü‚½‚ÍRèü“à
//							bit15:OFF=“Á’è“s‹æs“à
//							      ON =Rèü“à
//	@param [out] rule114   (–ß‚è’lTrue‚Ì‚İ—LŒø) [0]“K—p‰^’À(–¢“K—pA0), [1]‰c‹ÆƒLƒ, [2]ŒvZƒLƒ)
//	@retval true  114“K—p
//	@retval false 114”ñ“K—p
//
//	@note 86/87“K—pŒã‚Ì‰c‹ÆƒLƒ‚ª200km/100kmˆÈ‰º‚Å‚ ‚é‚±‚Æ.
//
bool Route::CheckOfRule114j(const vector<RouteItem>& route, const vector<RouteItem>& routeSpecial, int kind, int* result)
{
	int dkm;
	int km;				// 100km or 200km
	int aSales_km;		// 86/87 applied
	int line_id;
	int station_id1;
	int station_id2;
	int station_id3;
#ifdef _DEBUG
	vector<int> km_raw;		// 86 or 87 “K—p‘O [0]:‰c‹ÆƒLƒA[1]ŒvZƒLƒ
#endif
	vector<int> km_spe;		// 86 or 87 “K—pŒã [0]:‰c‹ÆƒLƒA[1]ŒvZƒLƒ
	FARE_INFO fi;
	int fare_normal;			/* 200(100)kmˆÈ‰º‚É‚æ‚è, 86 or 87 ”ñ“K—p‚Ì’ÊíŒvZ‰^’À */
	int fare_applied;			/* ‚æ‚è‰“‚¢‰w‚Ü‚Å‚Ì“s‹æs“à”­’…‚Ì‰¼“K—p‰^’À */

	/* Œo˜H‚ÍæŠ·‚È‚µ‚Ì’Pˆê˜Hü */
	if (route.size() <= 2) {
		return 0;
	}

#ifdef _DEBUG
	km_raw = Route::Get_route_distance(route); 			/* Œo˜H‹——£ */
	ASSERT(km_raw.size() == 2);			// [0]:‰c‹ÆƒLƒA[1]ŒvZƒLƒ
#endif
	km_spe = Route::Get_route_distance(routeSpecial); 	/* Œo˜H‹——£(86,87“K—pŒã) */
	ASSERT(km_spe.size() == 2);
	
	aSales_km = km_spe.at(0);			// ‰c‹ÆƒLƒ

	/* ’†S‰w`–Ú“I’n‚ÍA180(90) - 200(100)km–¢–‚Å‚ ‚é‚Ì‚ª‘O’ñ */
	if ((0x8000 & kind) == 0) {
		// 200km
		km = 2000;
	} else {
		// 100km
		km = 1000;
	}
	/* ‹——£‚ª‚ ‚Æ86A87ğ“K—p‹——£-10km‚Ì”ÍˆÍ“à‚Å‚Í‚È‚¢ */
	if ((aSales_km < (km * 9 / 10)) || (km < aSales_km)) {
		return 0;
	}

	if ((kind & 1) != 0) {		/* ”­‰w‚ª“Á’è“s‹æs“à */
		line_id = route.at(route.size() - 1).lineId;				// ’… ˜Hü ”­-’…
		station_id1 = route.at(route.size() - 2).stationId;
		station_id2 = route.at(route.size() - 1).stationId;
	} else if ((kind & 2) != 0) { /* ’…‰w‚ª“Á’è“s‹æs“à */
		line_id = route.at(1).lineId;								// ”­ ˜Hü.”­-’…
		station_id1 = route.at(1).stationId;
		station_id2 = route.at(0).stationId;
	} else {
		ASSERT(FALSE);
		return false;					// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	}
	// ex. ‘•ê-‰¡•l-’·’Ã“c‚Ìê‡Ag‰„ü.•xm-‘•ê ŠÔ‚Ì‹——£‚ğˆø‚­
	dkm = Route::GetDistance(line_id, station_id1, station_id2).at(0);
	aSales_km -= dkm;	/* ”­‰w‚©‚ç‰‰ñæŠ·‰w‚Ü‚Å‚Ì‰c‹ÆƒLƒ‚ğœ‚¢‚½‰c‹ÆƒLƒ */

	ASSERT(0 <= aSales_km);
	ASSERT(0 < dkm);

	if (LDIR_ASC != Route::DirLine(line_id, station_id1, station_id2)) {
		/* ã‚è */
		km = -km;
	}
	/* ’†S‰w‚©‚ç–Ú“I’n•ûŒü‚ÉÅ‰‚É200(100)km‚É“’B‚·‚é‰w‚ğ“¾‚é */
	/* •xm‚©‚çg‰„ü‚Åb•{•ûŒü‚É */
	station_id3 = Route::Retreive_SpecificCoreAvailablePoint(km, aSales_km, line_id, station_id1);
	
	//#2013.6.28:modified>> ASSERT(0 < station_id3);
	if (station_id3 <= 0) {
		return false;
	}
	//<<

	/* (86, 87ğ“K—p‘O‚³‚ê‚È‚©‚Á‚½)‰^’À‚»‚Ì‰w‚Ü‚Å‚Ì‰^’À‚æ‚è‚‚¯‚ê‚ÎA‚»‚Ì‰w‚Ü‚Å‚Ì‰^’À‚Æ‚·‚é */

	vector<RouteItem> route_work;	// <- routeSpecial
	route_work.assign(routeSpecial.cbegin(), routeSpecial.cend());

	if ((kind & 1) != 0) {			/* ”­‰w‚ª“Á’è“s‹æs“à */
		route_work.back().stationId = station_id3;	/* ÅI’…‰w‚ğ’u‚«Š·‚¦‚é */
	} else if ((kind & 2) != 0) {	/* ’…‰w‚ª“Á’è“s‹æs“à */
		route_work.front().stationId = station_id3;	/* n”­‰w‚ğ’u‚«Š·‚¦‚é */
	} else {
		ASSERT(FALSE);
		return false;					// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	}
	
	/* ’Êí‰^’À‚ğ“¾‚é */
	if (!fi.calc_fare(route)) {
		ASSERT(FALSE);
		return false;					// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	}
	fare_normal = fi.fare;			/* 200(100)kmˆÈ‰º‚É‚æ‚è, 86 or 87 ”ñ“K—p‚Ì’ÊíŒvZ‰^’À(’·’Ã“c-‰¡•l-‘•ê) */

#ifdef _DEBUG
	ASSERT(km_raw[0] == fi.total_jr_sales_km);
	ASSERT(km_raw[1] == fi.total_jr_calc_km);
#endif
	/* 86,87“K—p‚µ‚½Å’Z‰w‚Ì‰^’À‚ğ“¾‚é(ã—á‚Å‚Íb”ãZ‹g-‰¡•lŠÔ) */
	if (!fi.calc_fare(route_work)) {
		ASSERT(FALSE);
		return false;					// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	} 
	fare_applied = fi.fare;			/* ‚æ‚è‰“‚¢‰w‚Ü‚Å‚Ì“s‹æs“à”­’…‚Ì‰¼“K—p‰^’À(‰¡•l-b”ãZ‹g) */

	if (fare_applied < fare_normal) {	
		/* 114ğ“K—p */
		TRACE("Rule 114 Applied(%d->%d)\n", fare_normal, fare_applied);
		result[0] = fare_applied;		/* æ‚Ì‰w‚Ì86,87“K—p‰^’À */
		result[1] = fi.total_jr_sales_km;
		result[2] = fi.total_jr_calc_km;
		return true;
	}
	TRACE("Rule 114 no applied\n");
	return false;
}


//static
//	w’è˜HüE‰w‚©‚ç‚Ì100/200km“’B’n“_‚Ì‰w‚ğ“¾‚é
//	CheckOfRule114j() =>
//
//	@param [in] km  ‰º‚è: 100=1000, 200=2000(ã‚è: -1000 / -2000)
//	@param [in] line_id   ˜Hü
//	@param [in] station_id   ‹N“_‰w
//	@retval “’B‰wid. 0‚Í‚È‚µ
//
int Route::Retreive_SpecificCoreAvailablePoint(int km, int km_offset, int line_id, int station_id)
{
	// ã‚è•ûŒü‚Å200km“’B’n“_
	static const char tsql_desc[] = 
	"select l2.sales_km - l1.sales_km+%u as sales_km, l1.station_id"
	" from  t_lines l1 left join t_lines l2"
	" where l1.line_id=?1 and l2.line_id=?1"
	" and   l2.sales_km>l1.sales_km"
	" and   l2.station_id=?2"
	" and	(l1.lflg&(1<<31))=0"
	" and   (l2.sales_km-l1.sales_km)>%u order by l1.sales_km desc limit(1)";

	// ‰º‚è•ûŒü‚Å200km“’B’n“_
	static const char tsql_asc[] =
	"select l1.sales_km-l2.sales_km+%u as sales_km, l1.station_id"
	" from  t_lines l1 left join t_lines l2"
	" where l1.line_id=?1 and l2.line_id=?1"
	" and   l1.sales_km>l2.sales_km"
	" and   l2.station_id=?2"
	" and	(l1.lflg&(1<<31))=0"
	" and   (l1.sales_km-l2.sales_km)>%u order by l1.sales_km limit(1)";

	char sql_buf[512];

	if (km < 0) {
		km = -km;	/* ã‚è */
		sqlite3_snprintf(sizeof(sql_buf), sql_buf, tsql_desc, km_offset, km - km_offset);
	} else {		/* ‰º‚è */
		sqlite3_snprintf(sizeof(sql_buf), sql_buf, tsql_asc, km_offset, km - km_offset);
	}
	int akm;
	int aStationId;
	DBO dbo(DBS::getInstance()->compileSql(sql_buf));

	dbo.setParam(1, line_id);
	dbo.setParam(2, station_id);

	if (dbo.moveNext()) {
		akm = dbo.getInt(0);		// –¢g—p(‰c‹ÆƒLƒ‚¶‚á–á‚Á‚Ä‚à—v‚ç‚ñ)
		aStationId = dbo.getInt(1);
		return aStationId;
	}
	return 0;
}


//static
//	˜Hü‚ÍVŠ²ü‚Ìİ—ˆü‚©?
//
//	@param [in] line_id1		”äŠrŒ³˜Hü(İ—ˆü)
//	@param [in] line_id2		˜Hü(VŠ²ü)
//	@param [in] station_id1		‰w1 (”­) İ—ˆüÚ‘±‰w‚Å‚ ‚é‚±‚Æ
//	@param [in] station_id2		‰w2 (’…) İ—ˆüÚ‘±‰w‚Å‚È‚­‚Ä‚à‰Â
//
//	@return	true •Àsİ—ˆü
//
bool Route::IsAbreastShinkansen(int line_id1, int line_id2, int station_id1, int station_id2)
{
const char tsql[] = "select line_id from t_hzline where rowid=("
	"	select ((lflg>>13)&15) from t_lines where line_id=?1 and station_id=?2"
	"	)";
const char tsql2[] = "select line_id from t_hzline where line_id>0 and rowid in ("
	"	select ((lflg>>13)&15) from t_lines where ((lflg>>13)&15)!=0 and line_id=?1 and "
	"	case when (select sales_km from t_lines where line_id=?1 and station_id=?2)<"
	"	          (select sales_km from t_lines where line_id=?1 and station_id=?3)"
	"	then"
	"	sales_km>(select sales_km from t_lines where line_id=?1 and station_id=?2)"
	"	else"
	"	sales_km<(select sales_km from t_lines where line_id=?1 and station_id=?2)"
	"	end"
	") limit(1);";

	int line_id = -1;

	if (!IS_SHINKANSEN_LINE(line_id2)) {
		return false;
	}
	DBO dbo = DBS::getInstance()->compileSql(tsql);
	if (dbo.isvalid()) {
		dbo.setParam(1, line_id2);
		dbo.setParam(2, station_id1);
		if (dbo.moveNext()) {
			line_id = dbo.getInt(0);
			if (line_id <= 0) {
				DBO dbo2 = DBS::getInstance()->compileSql(tsql2); // ‚è(ã‰zVŠ²ü/–k—¤VŠ²ü)‚Æ‚©
				if (dbo2.isvalid()) {
					dbo2.setParam(1, line_id2);
					dbo2.setParam(2, station_id1);
					dbo2.setParam(3, station_id2);
					if (dbo2.moveNext()) {
						line_id = dbo2.getInt(0);
					}
				}
			}
		}
	}
	return line_id == line_id1;
}


//static
//	•Àsİ—ˆü‚ğ“¾‚é
//
//	@param [in] line_id     ˜Hü(VŠ²ü)
//	@param [in] station_id	‰w(•Àsİ—ˆü‰w(VŠ²üÚ‘±‰w)
//
//	@retval not 0 •Àsİ—ˆü
//	@retval 0xffff •Àsİ—ˆü‚Í2‚Â‚ ‚èA‚»‚Ì‹«ŠE‰w‚Å‚ ‚é(ã‰zVŠ²ü ‚è)
//
int Route::GetHZLine(int line_id, int station_id)
{
	const static char tsql[] =
"select line_id from t_hzline where rowid="
"(select (lflg>>13)&15 from t_lines where line_id=?1 and station_id=?2)";

	DBO dbo(DBS::getInstance()->compileSql(tsql));

	ASSERT(IS_SHINKANSEN_LINE(line_id));

	dbo.setParam(1, line_id);
	dbo.setParam(2, station_id);

	if (dbo.moveNext()) {
		return dbo.getInt(0);
	}
	return 0;
}



//static
//	VŠ²üæŠ·‰Â”Û‚Ìƒ`ƒFƒbƒN
//	add() =>
//
//	@param [in] line_id1  ‘O˜Hü
//	@param [in] line_id2  ¡˜Hü
//	@param [in] station_id1  ‘O‰ñÚ‘±(”­)‰w
//	@param [in] station_id2  Ú‘±‰w
//	@param [in] station_id3  ’…‰w
//
//	@return true: OK / false: NG
//
//                 ‘•{’Ã s1                “Œ‹
// l1 “ŒŠC“¹ü     ¬“cŒ´ s2 “ŒŠC“¹VŠ²ü   Ã‰ª
// l2 “ŒŠC“¹VŠ²ü –¼ŒÃ‰® s3 “ŒŠC“¹ü       ‘“ã
//
bool Route::CheckTransferShinkansen(int line_id1, int line_id2, int station_id1, int station_id2, int station_id3)
{
	int bullet_line;
	int local_line;
	int dir;
	int hzl;
	
	if (IS_SHINKANSEN_LINE(line_id2)) {
		bullet_line = line_id2;		// İ—ˆü->VŠ²üæŠ·
		local_line = line_id1;
	} else if (IS_SHINKANSEN_LINE(line_id1)) {
		bullet_line = line_id1;		// VŠ²ü->İ—ˆüæŠ·
		local_line = line_id2;
	} else {
		return true;				// ‚»‚êˆÈŠO‚Í‘ÎÛŠO
	}
	hzl = Route::GetHZLine(bullet_line, station_id2);
	if (hzl == 0x0fff) {
		// •Àsİ—ˆüŒËĞ‚ªØ‚è‘Ö‚í‚é(ã‰zü ‚è)
		hzl = Route::NextShinkansenTransferTerm(bullet_line, station_id2, 
								(bullet_line == line_id2) ? station_id2 : station_id1);
		if (local_line != Route::GetHZLine(bullet_line, hzl)) {
			return true;
		}
	} else if (hzl != local_line) {
		return true;				// ‚»‚êˆÈŠO‚Í‘ÎÛŠO
	}

	// table.A
	dir = Route::DirLine(line_id1, station_id1, station_id2);
	if (dir == Route::DirLine(line_id2, station_id2, station_id3)) {
		return true;		// ã‚è¨ã‚è or ‰º‚è¨‰º‚è
	}
	if (dir == LDIR_ASC) {	// ‰º‚è¨ã‚è
		return (((Route::AttrOfStationOnLineLine(local_line, station_id2) >> 25) & 0x01) != 0);
	} else {				// ã‚è¨‰º‚è
		return (((Route::AttrOfStationOnLineLine(local_line, station_id2) >> 25) & 0x02) != 0);
	}
	return true;
}


//static
//	VŠ²ü‚Ì‰w1‚©‚ç‰w2•û–Ê‚Ì—×‚Ìİ—ˆüÚ‘±‰w‚ğ•Ô‚·
//
//	@param [in] line_id1  VŠ²ü
//	@param [in] station_id1  ‰w1
//	@param [in] station_id2  ‰w2(•ûŒü)
//
//	@return ‰wid 0‚ğ•Ô‚µ‚½ê‡A—×‰w‚Í‰w2‚Ü‚½‚Í‚»‚ê‚æ‚èæ‚Ì‰w
//
int Route::NextShinkansenTransferTerm(int line_id, int station_id1, int station_id2)
{
	const static char tsql[] =
	"select station_id from t_lines where line_id=?1 and"
	" case when"
	"(select sales_km from t_lines where line_id=?1 and station_id=?3)<"
	"(select sales_km from t_lines where line_id=?1 and station_id=?2) then"
	" sales_km=(select max(sales_km) from t_lines where line_id=?1 and"
	"	((lflg>>13)&15)!=0 and (lflg&((1<<22)|(1<<31)))=0 and"
	"	sales_km<(select sales_km from t_lines where line_id=?1 and station_id=?2) and"
	"	sales_km>(select sales_km from t_lines where line_id=?1 and station_id=?3))"
	" else"
	" sales_km=(select min(sales_km) from t_lines where line_id=?1 and"
	"	((lflg>>13)&15)!=0 and (lflg&((1<<22)|(1<<31)))=0 and"
	"	sales_km>(select sales_km from t_lines where line_id=?1 and station_id=?2) and"
	"	sales_km<(select sales_km from t_lines where line_id=?1 and station_id=?3))"
	" end";

	DBO dbo(DBS::getInstance()->compileSql(tsql));

	ASSERT(IS_SHINKANSEN_LINE(line_id));

	dbo.setParam(1, line_id);
	dbo.setParam(2, station_id1);
	dbo.setParam(3, station_id2);

	if (dbo.moveNext()) {
		return dbo.getInt(0);
	}
	return 0;
}

//static
//	—¼—×‚Ì•ªŠò‰w‚ğ“¾‚é(”ñ•ªŠò‰ww’èA1‚Â‚©2‚Â)
//	changeNeerest() =>
//
//	@param [in] station_id    ‰w(”ñ•ªŠò‰w)
//	@return PAIRIDENT[2] or PAIRIDENT[1] ‹ß—×‚Ì•ªŠò‰w(–Ó’°ü‚Ìê‡1AˆÈŠO‚Í2)
//			IDENT1: station_id, IDENT2: calc_km
//
vector<PAIRIDENT> Route::GetNeerNode(int station_id)
{
	vector<PAIRIDENT> result;
	DBO dbo = Route::Enum_neer_node(station_id);
	while (dbo.moveNext()) {
		int stationId = dbo.getInt(0);
		int cost = dbo.getInt(1);
		result.push_back(MAKEPAIR(stationId, cost));
	}
	return result;
}

//public:
//
//	Å’ZŒo˜H‚É•ÏX(raw immidiate)
//
//	@param [in] useBulletTrain (bool)VŠ²üg—p—L–³
//	@retval true success
//	@retval false fail
//
bool Route::changeNeerest(bool useBulletTrain)
{
	ASSERT(0 < startStationId());
	ASSERT(0 < endStationId);
	ASSERT(startStationId() != endStationId);

	IDENT startNode;
	IDENT lastNode;
	IDENT lastNode1;
	IDENT lastNode1_distance;
	IDENT lastNode2;
	IDENT lastNode2_distance;
	int minCost[MAX_JCT];
	IDENT fromNode[MAX_JCT];
	bool done_flg[MAX_JCT];
	IDENT line_id[MAX_JCT];
	int i;
	//int km;
	int a;
	int b;
	int doneNode;
	int cost;
	int id;
	int lid;
	int stationId;
	int nLastNode;
	
	/* “r’†’Ç‰Á‚©AÅ‰‚©‚ç‚© */
	if (1 < route_list_raw.size()) {
		stationId = route_list_raw.back().stationId;
	} else {
		stationId = startStationId();
	}

	if (stationId == endStationId) {
		return false;			/* already routed */
	}

	/* ƒ_ƒCƒNƒXƒgƒ‰•Ï”‰Šú‰» */
	for (i = 0; i < MAX_JCT; i++) {
		minCost[i] = -1;
		fromNode[i] = 0;
		done_flg[i] = false;
		line_id[i] = 0;
	}

	startNode = Route::Id2jctId(stationId);
	lastNode = Route::Id2jctId(endStationId);
	if (startNode == 0) { /* ŠJn‰w‚Í”ñ•ªŠò‰w */
		lid = Route::LineIdFromStationId(stationId);
		if (lid == Route::LineIdFromStationId(endStationId)) { /* ”­‰w‚Æ’…‰w‚Í“¯ˆê˜Hü */
			///km = Route::Get_node_distance(lid, stationId, endStationId);
			// a’J-Œ´h‚È‚Ç >>>>>>>>>>>>>>>>>>>>>
			if (1 < route_list_raw.size() && route_list_raw.back().lineId == lid) {
				removeTail();
			}
			return 0 <= add(lid, /*stationId,*/ endStationId);
		}
		
		// ‰w‚Ì—¼—×‚ÌÅŠñ•ªŠò‰wID‚Æ‚»‚Ì‰w‚Ü‚Å‚ÌŒvZƒLƒ‚ğ“¾‚é
		vector<PAIRIDENT> neer_node = Route::GetNeerNode(stationId);

		// ”­‰w`Å‰‚Ì•ªŠò‰w‚Ü‚Å‚ÌŒvZƒLƒ‚ğÅ‰‚Ì•ªŠò‰w‚Ü‚Å‚Ì‰ŠúƒRƒXƒg‚Æ‚µ‚Ä‰Šú‰»
		a = Route::Id2jctId(IDENT1(neer_node.at(0)));
		if (!IsJctMask(a)) {
			minCost[a - 1] = IDENT2(neer_node.at(0));
			fromNode[a - 1] = -1;	// from‰w‚ğ-1(•ªŠò‰w‚Å‚È‚¢‚Ì‚Å‘¶İ‚µ‚È‚¢•ªŠò‰w)‚Æ‚µ‚Ä‰Šú‰»
			line_id[a - 1] = lid;
		}
		if (2 <= neer_node.size()) {
			b = Route::Id2jctId(IDENT1(neer_node.at(1)));
			if (!IsJctMask(b)) {
				minCost[b - 1] = IDENT2(neer_node.at(1));
				fromNode[b - 1] = -1;
				line_id[b - 1] = lid;
			} else if (IsJctMask(a)) {
				return false;
			}
		} else {
			if (IsJctMask(a)) {
				return false;
			}
		}
	} else {
		minCost[startNode - 1] = 0;
	}
	if (lastNode == 0) { /* I—¹‰w‚Í”ñ•ªŠò‰w ? */
		// ‰w‚Ì—¼—×‚ÌÅŠñ•ªŠò‰wID‚Æ‚»‚Ì‰w‚Ü‚Å‚ÌŒvZƒLƒ‚ğ“¾‚é
		vector<PAIRIDENT> neer_node = Route::GetNeerNode(endStationId);

		// dijkstra‚Ì‚ ‚Æ‚Åg—p‚Ì‚½‚ß‚É•Ï”‚ÉŠi”[
	
		lastNode1 = Route::Id2jctId(IDENT1(neer_node.at(0)));
		lastNode1_distance = IDENT2(neer_node.at(0));
		nLastNode = neer_node.size();
		ASSERT((nLastNode == 1) || (nLastNode == 2));
		if (2 <= nLastNode) {
			lastNode2 = Route::Id2jctId(IDENT1(neer_node.at(1)));
			lastNode2_distance = IDENT2(neer_node.at(1));
			nLastNode = 2;
		} else {
			nLastNode = 1;
		}
	} else {
		nLastNode = 0;
	}

	/* dijkstra */
	for (;;) {
		doneNode = -1;
		for (i = 0; i < MAX_JCT; i++) {
			// ƒm[ƒhi‚ªŠm’è‚µ‚Ä‚¢‚é‚Æ‚«
			// ƒm[ƒhi‚Ü‚Å‚ÌŒ»“_‚Å‚ÌÅ¬ƒRƒXƒg‚ª•s–¾‚Ì
			if (done_flg[i] || (minCost[i] < 0)) {
				continue;
			}
			/*  Šm’è‚µ‚½ƒm[ƒh”Ô†‚ª-1‚©ƒm[ƒhi‚ÌŒ»“_‚ÌÅ¬ƒRƒXƒg‚ª¬‚³‚¢‚Æ‚«
			 *  Šm’èƒm[ƒh”Ô†XV‚·‚é
			 */
			if ((doneNode < 0) || (minCost[i] < minCost[doneNode])) {
				doneNode = i;
			}
		}
		if (doneNode == -1) {
			break;	/* ‚·‚×‚Ä‚Ìƒm[ƒh‚ªŠm’è‚µ‚½‚çI—¹ */
		}
		done_flg[doneNode] = true;	// Enter start node

		TRACE(_T("[%s]"), Route::StationName(Jct2id(doneNode + 1)).c_str());
		if (nLastNode == 0) {
			if ((doneNode + 1) == lastNode) {
				break;	/* ’…ƒm[ƒh‚ªŠ®—¹‚µ‚Ä‚àI—¹‰Â */
			}
		} else if (nLastNode == 1) {
			if ((doneNode + 1) == lastNode1) {
				break;	/* ’…ƒm[ƒh‚ªŠ®—¹‚µ‚Ä‚àI—¹‰Â */
			}
		} else if (nLastNode == 2) {
			if (done_flg[lastNode1 - 1] && 
				done_flg[lastNode2 - 1]) {
				break;	/* ’…ƒm[ƒh‚ªŠ®—¹‚µ‚Ä‚àI—¹‰Â */
			}
		}

		vector<vector<int>> nodes = Route::Node_next(doneNode + 1);
		vector<vector<int>>::const_iterator ite;
		for (ite = nodes.cbegin(); ite != nodes.cend(); ite++) {
			a = ite->at(0) - 1;	// jctId
			if ((!IsJctMask(a + 1) || (lastNode == (a + 1))) && (useBulletTrain || !IS_SHINKANSEN_LINE(ite->at(2)))) {
				/* VŠ²ü‚Å‚È‚¢ */
				cost = minCost[doneNode] + ite->at(1); // cost
				// ƒm[ƒhto‚Í‚Ü‚¾–K‚ê‚Ä‚¢‚È‚¢ƒm[ƒh
				// ‚Ü‚½‚Íƒm[ƒhto‚Ö‚æ‚è¬‚³‚¢ƒRƒXƒg‚ÌŒo˜H‚¾‚Á‚½‚ç
				// ƒm[ƒhto‚ÌÅ¬ƒRƒXƒg‚ğXV
				if (((minCost[a] < 0) || (cost <= minCost[a])) &&
					((cost != minCost[a]) || IS_SHINKANSEN_LINE(ite->at(2)))) {
					/* ª “¯ˆê‹——£‚É2ü‚ ‚éê‡VŠ²ü‚ğÌ—p */
					minCost[a] = cost;
					fromNode[a] = doneNode;
					line_id[a] = ite->at(2);
					TRACE( _T("+<%s>"), Route::StationName(Jct2id(a + 1)).c_str());
				} else {
					TRACE(_T("-<%s>"), Route::StationName(Jct2id(a + 1)).c_str());
				}
			}
		}
	}

	TRACE(_T("\n"));

	vector<IDENT> route;
	int lineid = 0;
	int idb;
	
	if (lastNode == 0) { /* I—¹‰w‚Í”ñ•ªŠò‰w ? */
		// ÅŒã‚Ì•ªŠò‰w‚ÌŒˆ’èF
		// 2‚Â‚ÌÅŒã‚Ì•ªŠò‰wŒó•â(I—¹‰w(”ñ•ªŠò‰wj‚Ì—¼—×‚Ì•ªŠò‰w)`I—¹‰w(”ñ•ªŠò‰w)‚Ü‚Å‚Ì
		// ŒvZƒLƒ{2‚Â‚ÌÅŒã‚Ì•ªŠò‰wŒó•â‚Ü‚Å‚ÌŒvZƒLƒ‚ÍA
		// ‚Ç‚¿‚ç‚ª’Z‚¢‚©H
	
		if ((2 == nLastNode) &&
			((minCost[lastNode2 - 1] + lastNode2_distance) < (minCost[lastNode1 - 1] + lastNode1_distance))) {
			id = lastNode2;		// ’Z‚¢•û‚ğÅŒã‚Ì•ªŠò‰w‚Æ‚·‚é
		} else {
			id = lastNode1;
		}
	} else {
		id = lastNode;
	}

	if (fromNode[id - 1] <= 0) {
		TRACE( _T("can't lowcost route.\n"));
		return false;
	}

	// ”­‰w(=•ªŠò‰w)‚Å‚È‚­Å‰‚Ì•ªŠò‰w(-1+1=0)‚Å‚È‚¢ŠÔ
	// ÅŒã‚Ì•ªŠò‰w‚©‚çfrom‚ğƒgƒŒ[ƒX >> route[]
	while ((id != startNode) && (0 < id)) {
		TRACE( _T("  %s, %s, %s."), Route::LineName(lineid).c_str(), Route::LineName(line_id[id - 1]).c_str(), Route::StationName(Jct2id(id)).c_str());
		if (lineid != line_id[id - 1]) {
			if (IS_SHINKANSEN_LINE(lineid)) {
				//printf("@@@@->%d\n", lineid);
				 	/* VŠ²ü¨•Àsİ—ˆü */
				int zline = Route::GetHZLine(lineid, Route::Jct2id(id));
				for (idb = id; (idb != startNode) && (line_id[idb - 1] == zline); idb = fromNode[idb - 1] + 1) {
					TRACE( _T("    ? %s %s/"),  Route::LineName(line_id[idb - 1]).c_str(), Route::StationName(Jct2id(idb)).c_str());
					;
				}
				if (line_id[idb - 1] == lineid) { /* ‚à‚Æ‚ÌVŠ²ü‚É–ß‚Á‚½ ? */
					//printf(".-.-.-");
					id = idb;
					continue;
				} else if (idb == startNode) { /* ”­‰wH */
					if (zline == Route::GetHZLine(lineid, Route::Jct2id(idb))) {
						id = idb;
						continue;
					}
					/* thru */
					//printf("*-*-*-");
				} else if (idb != id) { /* ‘¼˜Hü‚Ìæ‚èŠ·‚¦‚½ ? */
					TRACE( _T("%s‚Í‚»‚¤‚¾‚ªA%s‚É‚àVŠ²ü’âÔ‚·‚é‚©?"), Route::StationName(Jct2id(id)).c_str(), Route::StationName(Jct2id(idb)).c_str());
					if (zline == Route::GetHZLine(lineid, Route::Jct2id(idb))) {
						id = idb;
						continue;
					}
					/* thru */
					TRACE( _T("+-+-+-: %s(%s) : "), Route::LineName(line_id[idb - 1]).c_str(), Route::LineName(lineid).c_str());
				} else {
					//printf("&");
				}
			} else { /* ‘O‰ñVŠ²ü‚Å‚È‚¢‚È‚ç */
				/* thru */
				//printf("-=-=-=");
			}
// Ÿ‚ÌVŠ²ü‚Å‚à•Àsİ—ˆü‚Å‚à‚È‚¢˜Hü‚Ö‚Ì•ªŠò‰w‚ÉVŠ²ü•ªŠò‰w‚Å‚È‚¢ê‡(‹àR)Å‰‚Ìİ—ˆüØ‚è‘Ö‚¦‚ğ—LŒø‚É‚·‚éiO‰ÍˆÀéj
// VŠ²ü‚É–ß‚Á‚Ä‚«‚Ä‚¢‚éê‡(‰ÔŠª¨·‰ªjA‰ÔŠª‚Ü‚Åi–kã‚©‚çj–³Œø‰»‚É‚·‚é
// 
			route.push_back(id - 1);
			lineid = line_id[id - 1];
			TRACE( _T("  o\n"));
		} else {
			TRACE( _T("  x\n"));
		}
		id = fromNode[id - 1] + 1;
	}
	
	//// ”­‰w=•ªŠò‰w


	TRACE( _T("----------%d------\n"), id);

	vector<IDENT> route_rev;
	vector<IDENT>::const_reverse_iterator ritr = route.crbegin();
	int bid = -1;
	while (ritr != route.crend()) {
		TRACE(_T("> %s %s\n"), Route::LineName(line_id[*ritr]).c_str(), Route::StationName(Jct2id(*ritr + 1)).c_str());
		if (0 < bid && IS_SHINKANSEN_LINE(line_id[bid])) {
			if (Route::GetHZLine(line_id[bid], Route::Jct2id(*ritr + 1)) == line_id[*ritr]) {
				line_id[*ritr] = line_id[bid];	/* local line -> bullet train */
				route_rev.pop_back();
			}
		}
		route_rev.push_back(*ritr);
		bid = *ritr;
		++ritr;
	}
	route.assign(route_rev.cbegin(), route_rev.cend());
	route_rev.clear();

	if (lastNode == 0) {	// ’…‰w‚Í”ñ•ªŠò‰w?
		lid = Route::LineIdFromStationId(endStationId); // ’…‰wŠ‘®˜HüID
		// ÅI•ªŠò‰w`’…‰w‚Ü‚Å‚Ì‰c‹ÆƒLƒA‰^’ÀŒvZƒLƒ‚ğæ“¾
		//km = Route::Get_node_distance(lid, endStationId, Route::Jct2id(a));
		//km += minCost[route.back()];	// ÅŒã‚Ì•ªŠò‰w‚Ü‚Å‚Ì—İÏŒvZƒLƒ‚ğXV
		if (lid == line_id[route.back()]) { // ’…‰w‚ÌÅŠñ•ªŠò‰w‚Ì˜Hü=ÅŒã‚Ì•ªŠò‰w?
			route.pop_back();	// if   a’J-Vh-¼‘•ª›-‘—§
								// else a’J-Vh-O‘é
		}
	} else {
		lid = 0;
	}
	
	if ((1 < route_list_raw.size()) && (1 < route.size()) && (route_list_raw.back().lineId == line_id[route[0]])) {
		removeTail();
		ASSERT(0 < route_list_raw.size()); /* route_list_raw.size() ‚Í0‚©2ˆÈã */
		stationId = route_list_raw.back().stationId;
	}
	for (i = 0; i < (int)route.size(); i++) {
		if (add(line_id[route[i]], /*stationId,*/ Route::Jct2id(route[i] + 1)) < 0) {
			ASSERT(FALSE);
			return false;
		}
		stationId = Route::Jct2id(route[i] + 1);
	}
	if (lastNode == 0) {
		ASSERT(0 < lid);
		if (add(lid, /*stationId,*/ endStationId) < 0) {
			ASSERT(FALSE);
			return false;
		}
	}
	route_list_cooked.clear();
	return true;
}

/*	˜Hü‚Ì2“_ŠÔ‰c‹ÆƒLƒAŒvZƒLƒA‰ïĞ(JR-Gr.)‹«ŠE‚ğ“¾‚é
 *	calc_fare() => aggregate_fare_info() =>
 *
 *	@param [in] line_id   	 ˜HüID
 *	@param [in] station_id1  ‰w1
 *	@param [in] station_id2  ‰w2
 *
 *	@return vector<int> [0] ‰c‹ÆƒLƒ
 *	@return vector<int> [1] ŒvZƒLƒ
 *	@return vector<int> [2] ‰w1‚Ì‰ïĞ‹æŠÔ•”‚Ì‰c‹ÆƒLƒ(‰w1‚Ì‰ïĞID‚‰w2‚Ì‰ïĞID‚Ì‚İ—LŒø)
 *                          ‰w1‚ª‹«ŠE‰w‚È‚ç-1‚ğ•Ô‚·, ‹«ŠE‰w‚ª‰w1`‰w2ŠÔ‚É‚È‚¯‚ê‚ÎANone‚ğ•Ô‚·
 *	@return vector<int> [3] ‰w1‚Ì‰ïĞ‹æŠÔ•”‚ÌŒvZƒLƒ(‰w1‚Ì‰ïĞID‚‰w2‚Ì‰ïĞID‚Ì‚İ—LŒø)
 *                          ‰w2‚ª‹«ŠE‰w‚È‚ç-1‚ğ•Ô‚·, ‹«ŠE‰w‚ª‰w1`‰w2ŠÔ‚É‚È‚¯‚ê‚ÎANone‚ğ•Ô‚·
 *	@return vector<int> [4] IDENT1(‰w1‚Ì‰ïĞID) + IDENT2(‰w2‚Ì‰ïĞID)
 *	@return vector<int> [5] bit31:1=JRˆÈŠO‚Ì‰ïĞü^0=JRƒOƒ‹[ƒvĞü / IDENT1(‰w1‚Ìsflg) / IDENT2(‰w2‚Ìsflg(MSB=bit15œ‚­))
*/
vector<int> FARE_INFO::GetDistanceEx(int line_id, int station_id1, int station_id2)
{
	vector<int> result;

	DBO ctx = DBS::getInstance()->compileSql(
"select"
"	(select max(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))-"
"	(select min(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3)),"		// 0
"	(select max(calc_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))-"
"	(select min(calc_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3)),"		// 1
"	case when exists (select * from t_lines	where line_id=?1 and (lflg&((1<<21)|(1<<31)))=(1<<21) and station_id=?2)"
"	then -1 else"
"	abs((select sales_km from t_lines"
"	where line_id=?1 and (lflg&((1<<21)|(1<<31)))=(1<<21)"
"	and	sales_km>(select min(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))"
"	and	sales_km<(select max(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3)))-"
"	(select sales_km from t_lines where line_id=?1 and station_id=?2)) end,"						// 2
"	case when exists (select * from t_lines"
"	where line_id=?1 and (lflg&((1<<21)|(1<<31)))=(1<<21) and station_id=?3)"
"	then -1 else"
"	abs((select calc_km from t_lines"
"	where line_id=?1 and (lflg&((1<<21)|(1<<31)))=(1<<21)"
"	and	sales_km>(select min(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))"
"	and	sales_km<(select max(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3)))-"
"	(select calc_km from t_lines where line_id=?1 and station_id=?2)) end,"							// 3
"	((select company_id from t_station where rowid=?2) + (65536 * (select company_id from t_station where rowid=?3))),"	// 4
"	((select 2147483648*(1&(lflg>>23)) from t_lines where line_id=?1 and station_id=?3) + "
"	(select sflg&8191 from t_station where rowid=?2) + (select sflg&8191 from t_station where rowid=?3) * 65536)"		// 5
, true);
//	2147483648 = 0x80000000
	DbidOf dbid;

	if (ctx.isvalid()) {
		ctx.setParam(1, line_id);
		ctx.setParam(2, station_id1);
		ctx.setParam(3, station_id2);

		if (ctx.moveNext()) {
			result.push_back(ctx.getInt(0));	// sales_km
			result.push_back(ctx.getInt(1));	// calc_km
			result.push_back(ctx.getInt(2));	// sales_km for in company as station_id1
			result.push_back(ctx.getInt(3));	// calc_km  for in company as station_id1
			result.push_back(ctx.getInt(4));	// IDENT1(‰wID1‚Ì‰ïĞID) + IDENT2(‰wID2‚Ì‰ïĞID)
			result.push_back(ctx.getInt(5));	// bit31:1=JRˆÈŠO‚Ì‰ïĞü^0=JRƒOƒ‹[ƒvĞü / IDENT1(‰w1‚Ìsflg) / IDENT2(‰w2‚Ìsflg(MSB=bit15œ‚­))

			if ((line_id == dbid.LineIdOf_HAKATAMINAMISEN) || (dbid.LineIdOf_SANYOSHINKANSEN == line_id)) {	// R—zVŠ²ü
				result[2] = 0;
				result[3] = 0;
				result[4] = MAKEPAIR(JR_WEST, JR_WEST);
			}
		}
	}
	return result;
}

// static
//		@brief 70ğ’Ê‰ß‚Ì‰c‹ÆƒLƒ‚ğ“¾‚é
//
//	@param [in] station_id1		‰w1
//	@param [in] station_id2		‰w2
//
//	@return station_id1, station_id2ŠÔ‚Ì‰c‹ÆƒLƒ
//
int FARE_INFO::Retrieve70Distance(int station_id1, int station_id2)
{
	static const char tsql[] = 
"select sales_km from t_rule70"
" where "
" ((station_id1=?1 and station_id2=?2) or" 
"  (station_id1=?2 and station_id2=?1))";

	DBO dbo(DBS::getInstance()->compileSql(tsql, true));
	dbo.setParam(1, station_id1);
	dbo.setParam(2, station_id2);

	if (dbo.moveNext()) {
		return dbo.getInt(0);
	}
	return 0;
}


//	1Œo˜H‚Ì‰c‹ÆƒLƒAŒvZƒLƒ‚ğWŒv
//	calc_fare() =>
//
//	@param [in] line_id      ˜Hü
//	@param [in] station_id1  ‰w1
//	@param [in] station_id2  ‰w2
//	@retval true Success
//	@retval false Fatal error
//
bool FARE_INFO::aggregate_fare_info(int line_id, int station_id1, int station_id2)
{
	int company_id1;
	int company_id2;
	int flag;

	if (line_id == ID_L_RULE70) {
		int sales_km;
		sales_km = FARE_INFO::Retrieve70Distance(station_id1, station_id2);
		ASSERT(0 < sales_km);
		this->sales_km += sales_km;			// total ‰c‹ÆƒLƒ(‰ïĞüŠÜ‚ŞA—LŒø“ú”ŒvZ—p)
		this->base_sales_km	+= sales_km;
		this->base_calc_km += sales_km;
		this->local_only = false;		// Š²ü

		return true;
	}

	vector<int> d = FARE_INFO::GetDistanceEx(line_id, station_id1, station_id2);
	
							// “Á•Ê‰ÁZ‹æŠÔ
	this->fare += FARE_INFO::CheckSpecficFarePass(line_id, station_id1, station_id2);

	if (6 != d.size()) {
		ASSERT(FALSE);
		return false;	/* failure abort end. >>>>>>>>> */
	}
	company_id1 = IDENT1(d.at(4));
	company_id2 = IDENT2(d.at(4));
	
	if ((company_id1 == JR_CENTRAL) || (company_id1 == JR_WEST)) {
		company_id1 = JR_EAST;	/* –{BOĞ‚Æ‚µ‚ÄJR“ŒŠCAJR¼‚ÍAJR“Œ‚Æ‚·‚é */
	}
	if ((company_id2 == JR_CENTRAL) || (company_id2 == JR_WEST)) {
		company_id2 = JR_EAST;	/* –{BOĞ‚Æ‚µ‚ÄJR“ŒŠCAJR¼‚ÍAJR“Œ‚Æ‚·‚é */
	}
	if (d.at(2) == -1) {		/* station_id1‚ª‹«ŠE‰w‚Ìê‡ */
		company_id1 = company_id2;
	}
	if (d.at(3) == -1) {		/* station_id2‚ª‹«ŠE‰w‚Ìê‡ */
		company_id2 = company_id1;
	}
	this->sales_km += d.at(0);			// total ‰c‹ÆƒLƒ(‰ïĞüŠÜ‚ŞA—LŒø“ú”ŒvZ—p)
	if (IS_SHINKANSEN_LINE(line_id)) {
		/* R—zVŠ²ü L“‡-“¿RŠÔ‚ğ’Ê‚éê‡‚Ì1Œo˜H‚Å‚ÍAfare(l, i) ‚Æ‚³‚ê‚é‚Ä‚µ‚Ü‚¤‚©‚ç.
		 */
		this->local_only = false;		// Š²ü
	}
	if (company_id1 == company_id2) {		// “¯ˆê 1Ğ
		if ((d.at(5) & 0x80000000) != 0) {	/* ‰ïĞü */
			this->company_fare += FARE_INFO::Fare_company(station_id1, station_id2);
		} else {
				/* ’Pˆê JR Group */
			switch (company_id1) {
			case JR_HOKKAIDO:
				this->hokkaido_sales_km 	+= d.at(0);
				if (0 == d.at(1)) { 		/* Š²ü? */
					this->hokkaido_calc_km 	+= d.at(0);
					this->local_only_as_hokkaido = false;
					this->local_only = false;	// Š²ü
				} else {
					this->hokkaido_calc_km 	+= d.at(1);
				}
				break;
			case JR_SHIKOKU:
				this->shikoku_sales_km 		+= d.at(0);
				if (0 == d.at(1)) { 		/* Š²ü? */
					this->shikoku_calc_km 	+= d.at(0);
					this->local_only = false;	// Š²ü
				} else {
					this->shikoku_calc_km 	+= d.at(1);
				}
				break;
			case JR_KYUSYU:
				this->kyusyu_sales_km 		+= d.at(0);
				if (0 == d.at(1)) { 		/* Š²ü? */
					this->kyusyu_calc_km 	+= d.at(0);
					this->local_only = false;	// Š²ü
				} else {
					this->kyusyu_calc_km 	+= d.at(1);
				}
				break;
			case JR_EAST:
			case JR_CENTRAL:
			case JR_WEST:
				this->base_sales_km 		+= d.at(0);
				if (0 == d.at(1)) { 		/* Š²ü? */
					this->base_calc_km 		+= d.at(0);
					this->local_only = false;	// Š²ü
				} else {
					this->base_calc_km 		+= d.at(1);
				}
				break;
			default:
				ASSERT(FALSE);	/* ‚ ‚è“¾‚È‚¢ */
				break;
			}
		}
	} else {								// ‰ïĞŒ×‚è?
		switch (company_id1) {
		case JR_HOKKAIDO:					// JR–kŠC“¹->–{B3Ğ(JR“Œ)
			ASSERT(FALSE);		// –kŠC“¹VŠ²ü‚ªŠJ’Ê‚·‚é‚Ü‚Å‚¨—a‚¯
			this->hokkaido_sales_km 	+= d.at(2);
			this->base_sales_km 		+= (d.at(0) - d.at(2));
			if (0 == d.at(1)) { /* Š²ü? */
				ASSERT(FALSE);	/* ‚ ‚è‚¦‚È‚¢ */
				this->hokkaido_calc_km  += d.at(2);
				this->base_calc_km  	+= (d.at(0) - d.at(2));
				this->local_only_as_hokkaido = false;
			} else {
				this->hokkaido_calc_km  += d.at(3);
				this->base_calc_km  	+= (d.at(1) - d.at(3));
			}
			break;
		case JR_SHIKOKU:					// JRl‘->–{B3Ğ(JR¼)
			this->shikoku_sales_km 		+= d.at(2);
			this->base_sales_km 		+= (d.at(0) - d.at(2));
			if (0 == d.at(1)) { 			/* Š²ü? */
				this->shikoku_calc_km 	+= d.at(2);
				this->base_calc_km  	+= (d.at(0) - d.at(2));
				this->local_only = false;	// Š²ü
			} else {
				this->shikoku_calc_km 	+= d.at(3);
				this->base_calc_km  	+= (d.at(1) - d.at(3));
			}
			break;
		case JR_KYUSYU:					// JR‹ãB->–{B3Ğ(JR¼)
			this->kyusyu_sales_km 		+= d.at(2);
			this->base_sales_km 		+= (d.at(0) - d.at(2));
			if (0 == d.at(1)) { /* Š²ü? */
				this->kyusyu_calc_km 	+= d.at(2);
				this->base_calc_km 		+= (d.at(0) - d.at(2));
				this->local_only = false;	// Š²ü
			} else {
				this->kyusyu_calc_km 	+= d.at(3);
				this->base_calc_km 		+= (d.at(1) - d.at(3));
			}
			break;
		default:
			this->base_sales_km 		+= d.at(2);
			if (0 == d.at(1)) { /* Š²ü? */
				this->base_calc_km 		+= d.at(2);
				this->local_only = false;	// Š²ü
			} else {
				this->base_calc_km 		+= d.at(3);
			}
			switch (company_id2) {
			case JR_HOKKAIDO:			// –{B3Ğ(JR“Œ)->JR–kŠC“¹
				ASSERT(FALSE);	// –kŠC“¹VŠ²ü‚ª—ˆ‚é‚Ü‚Å‚¨—a‚¯
				this->hokkaido_sales_km		+= (d.at(0) - d.at(2));
				if (0 == d.at(1)) { /* Š²ü? */
					this->hokkaido_calc_km	+= (d.at(0) - d.at(2));
					this->local_only_as_hokkaido = false;
				} else {
					this->hokkaido_calc_km	+= (d.at(1) - d.at(3));
				}
				break;
			case JR_SHIKOKU:			// –{B3Ğ(JR¼)->JRl‘
				this->shikoku_sales_km		+= (d.at(0) - d.at(2));
				if (0 == d.at(1)) { /* Š²ü? */
					this->shikoku_calc_km	+= (d.at(0) - d.at(2));
				} else {
					this->shikoku_calc_km	+= (d.at(1) - d.at(3));
				}
				break;
			case JR_KYUSYU:				// –{B3Ğ(JR¼)->JR‹ãB
				this->kyusyu_sales_km		+= (d.at(0) - d.at(2));
				if (0 == d.at(1)) { /* Š²ü? */
					this->kyusyu_calc_km	+= (d.at(0) - d.at(2));
				} else {
					this->kyusyu_calc_km	+= (d.at(1) - d.at(3));
				}
				break;
			default:
				ASSERT(FALSE);
				return false;	/* failure abort end. >>>>>>>>> */
				break;	/* ‚ ‚è“¾‚È‚¢ */
			}
			break;
		}
	}
	if ((this->flag & 0x8000) == 0) {		// b15‚ª0‚Ìê‡Å‰‚È‚Ì‚Å‰w1‚Ìƒtƒ‰ƒO‚à”½‰f
		this->flag = IDENT1(d.at(5));		// ‰w1 sflg‚Ì‰º13ƒrƒbƒg
	}
	flag = IDENT2(d.at(5));					// ‰w2 sflg‚Ì‰º13ƒrƒbƒg
	if ((flag & 0x0380) != (this->flag & 0x0380)) {/* ‹ßx‹æŠÔ(b7-9) ‚Ì”äŠr */
		flag &= ~0x380;						/* ‹ßx‹æŠÔ OFF */
	}
	this->flag &= flag;						/* b11,10,5(‘åã/“Œ‹“dÔ“Á’è‹æŠÔ, Rèü^‘åãŠÂóü“à) */
	this->flag |= 0x8000;					/* Ÿ‰ñ‚©‚ç‰w1‚Í•s—v */
	if (IS_SHINKANSEN_LINE(line_id)) {
		this->flag |= ((line_id & 0x0f) << 16);	/* make flag for MASK_FLAG_SHINKANSEN() */
	}
	/* flag(sflg)‚ÍAb11,10,5, 7-9 ‚Ì‚İg—p‚Å‘¼‚ÍFARE_INFO‚Å‚Íg—p‚µ‚È‚¢ */
	return true;
}

// Private:
//	showFare() =>
//	CheckOfRule114j() =>
//
//	‰^’ÀŒvZ
//
//	@return ˆÙí‚Ì‚Ífalse
//
bool FARE_INFO::calc_fare(const vector<RouteItem>& routeList)
{
	vector<RouteItem>::const_iterator ite;
	int station_id1;
	
	reset();

	station_id1 = 0;
	this->local_only = true;
	this->local_only_as_hokkaido = true;
	for (ite = routeList.cbegin(); ite != routeList.cend(); ite++) {

//ASSERT((ite->flag  & ~MASK_ROUTE_FLAG) == 0);
		if (station_id1 != 0) {
							/* ‰ïĞ•Ê‰c‹ÆƒLƒAŒvZƒLƒ‚Ìæ“¾ */
			if (!FARE_INFO::aggregate_fare_info(ite->lineId, station_id1, ite->stationId)) {
				ASSERT(FALSE);
				goto err;		/* >>>>>>>>>>>>>>>>>>> */
			}
		}
		station_id1 = ite->stationId;
	}
	/* æÔŒ”‚Ì—LŒø“ú” */
	this->avail_days = FARE_INFO::days_ticket(this->sales_km);

	/* 89ğ“K—p */
	this->base_calc_km += Route::CheckOfRule89j(routeList);

	/* Š²ü‚Ì‚İ ? */
	this->major_only = ((this->base_sales_km == this->base_calc_km) && 
						(this->kyusyu_sales_km == this->kyusyu_calc_km) && 
						(this->hokkaido_sales_km == this->hokkaido_calc_km) && 
						(this->shikoku_sales_km == this->shikoku_calc_km));

	if (MASK_URBNNO(this->flag) != 0) {  /* ‹ßx‹æŠÔ(Å’Z‹——£‚ÅZo‰Â”\) */
						/* “Á’è‹æŠÔ‚Ì”»’è */
		int fare = FARE_INFO::SpecficFareLine(routeList.front().stationId, routeList.back().stationId);
		if (0 < fare) {
			TRACE("specific fare section replace for Metro.\n");
			this->fare = fare;	/* ‘å“ss“Á’è‹æŠÔ‰^’À */

			this->total_jr_sales_km = this->base_sales_km;
			ASSERT(this->kyusyu_sales_km + this->hokkaido_sales_km + this->shikoku_sales_km == 0);

			this->total_jr_calc_km =this->base_calc_km;
			ASSERT(this->kyusyu_calc_km + this->hokkaido_calc_km + this->shikoku_calc_km == 0);

			ASSERT(this->company_fare == 0);	// ‰ïĞü‚Í’Ê‚Á‚Ä‚¢‚È‚¢
			
			return true;	// >>>>>>>>>>>>>>>>>>>>
		} else {
			// “ss‹ßx‹æŠÔ“à‚Ì”­’…‚Å‚·‚Ì‚ÅÅ’ZŒo˜H‚Ì‰^’À‚Å‚²—˜—p‰Â”\‚Å‚·(“r’†‰ºÔ•s‰ÂA—LŒø“ú”“–“úŒÀ‚è)
		}
	}
	/* ‰^’ÀŒvZ */
	if (retr_fare()) {
		return true;
	}
err:
	ASSERT(FALSE);
	this->reset();
	return false;
}

//static
//	WŒv‚³‚ê‚½‰c‹ÆƒLƒAŒvZƒLƒ‚æ‚è‰^’ÀŠz‚ğZo(‰^’ÀŒvZÅIH’ö)
//	(JR‚Ì‚İ)
//	calc_fare() =>
//
//	@retval true Success
//	@retval false Fatal error
//
bool FARE_INFO::retr_fare()
{
	int fare;
	this->total_jr_sales_km = this->base_sales_km +
								this->kyusyu_sales_km +
								this->hokkaido_sales_km +
								this->shikoku_sales_km;

	this->total_jr_calc_km =this->base_calc_km +
								this->kyusyu_calc_km +
								this->hokkaido_calc_km +
								this->shikoku_calc_km;

				// –{B3Ğ‚ ‚èH
	if (0 < (this->base_sales_km + this->base_calc_km)) {
		if (IS_OSMSP(this->flag)) {
			/* ‘åã“dÔ“Á’è‹æŠÔ‚Ì‚İ */
			ASSERT(this->fare == 0); /* “Á•Ê‰ÁZ‹æŠÔ‚ğ’Ê‚Á‚Ä‚¢‚È‚¢‚Í‚¸‚È‚Ì‚Å */
			ASSERT(this->company_fare == 0);	// ‰ïĞü‚Í’Ê‚Á‚Ä‚¢‚È‚¢
			ASSERT(this->base_sales_km == this->total_jr_sales_km);
			ASSERT(this->base_sales_km == this->sales_km);
			if (IS_YAMATE(this->flag)) {
				TRACE("fare(e)\n");
				this->fare = FARE_INFO::Fare_e(this->total_jr_sales_km);
			} else {
				TRACE("fare(c)\n");
				this->fare = FARE_INFO::Fare_c(this->total_jr_sales_km);
			}
		} else if (IS_TKMSP(this->flag)) { 
			/* “Œ‹“dÔ“Á’è‹æŠÔ‚Ì‚İ */
			ASSERT(this->fare == 0); /* “Á•Ê‰ÁZ‹æŠÔ‚ğ’Ê‚Á‚Ä‚¢‚È‚¢‚Í‚¸‚È‚Ì‚Å */
			ASSERT(this->company_fare == 0);	// ‰ïĞü‚Í’Ê‚Á‚Ä‚¢‚È‚¢
			ASSERT(this->base_sales_km == this->total_jr_sales_km);
			ASSERT(this->base_sales_km == this->sales_km);
			if (IS_YAMATE(this->flag)) {
				TRACE("fare(d)\n");
				this->fare = FARE_INFO::Fare_d(this->total_jr_sales_km);
			} else {
				TRACE("fare(b)\n");
				this->fare = FARE_INFO::Fare_b(this->total_jr_sales_km);
			}
		} else if (this->local_only || (!this->major_only && (this->total_jr_sales_km <= 100))) {
			/* –{B3Ğ’n•ûŒğ’Êü‚Ì‚İ or JR“Œ+JR–k */
			/* Š²ü+’n•ûŒğ’Êü‚Åƒg[ƒ^ƒ‹‰c‹ÆƒLƒ‚ª10kmˆÈ‰º */
			// (i)<s>
			TRACE("fare(l,i)\n");
			this->fare += FARE_INFO::Fare_table("l", "i", this->total_jr_sales_km);
			
		} else { /* Š²ü‚Ì‚İ^Š²ü+’n•ûŒğ’Êü */
			// (a) + this->calc_km‚ÅZo
			TRACE("fare(a)\n");
			this->fare += FARE_INFO::Fare_a(this->total_jr_calc_km);
		}

		// JR–k‚ ‚è?
		if (0 < (this->hokkaido_sales_km + this->hokkaido_calc_km)) {
			/* JR“Œ + JR–k */
			if (this->local_only_as_hokkaido || 
			  (!this->major_only && (this->total_jr_sales_km <= 100))) {
				/* JR–k‚Í’n•ûŒğ’Êü‚Ì‚İ */
				/* or Š²ü+’n•ûŒğ’Êü‚Åƒg[ƒ^ƒ‹‰c‹ÆƒLƒ‚ª10kmˆÈ‰º */
				// (r) sales_km add
				TRACE("fare(r,r)\n");
				this->fare += FARE_INFO::Fare_table("r", "r", this->hokkaido_sales_km);

			} else { /* Š²ü‚Ì‚İ^Š²ü+’n•ûŒğ’Êü‚Å10km‰z‚¦ */
				// (o) calc_km add
				TRACE("fare(opq, o)\n");
				this->fare += FARE_INFO::Fare_table("opq", "o", this->hokkaido_calc_km);
			}
		}				// JR‹ã‚ ‚èH
		if (0 < (this->kyusyu_sales_km + this->kyusyu_calc_km)) {
			/* JR¼ + JR‹ã */
			/* Š²ü‚Ì‚İAŠ²ü+’n•ûŒğ’Êü */

			// JR‹ãB‘¤(q)<s><c> ‰ÁZ 
			TRACE("fare(opq,q)\n");
			this->fare += FARE_INFO::Fare_table("opq", "q", this->kyusyu_calc_km);
		}				// JRl‚ ‚è?
		if (0 < (this->shikoku_sales_km + this->shikoku_calc_km)) {
			/* JR¼ + JRl */
			/* Š²ü‚Ì‚İAŠ²ü+’n•ûŒğ’Êü */

			// JRl‘‘¤(p)<s><c> ‰ÁZ 
			TRACE("fare(opq,p)\n");
			this->fare += FARE_INFO::Fare_table("opq", "p", this->shikoku_calc_km);
		}				// JR–k
	} else if (0 < (this->hokkaido_sales_km + this->hokkaido_calc_km)) {
		/* JR–kŠC“¹‚Ì‚İ */
		ASSERT(this->total_jr_sales_km == this->hokkaido_sales_km);
		ASSERT(this->total_jr_calc_km == this->hokkaido_calc_km);
		ASSERT(this->fare == 0);
		if (this->local_only_as_hokkaido) {
			/* JR–kŠC“¹ ’n•ûŒğ’Êü‚Ì‚İ */
			
			// (j)<s>
			TRACE("fare(l,j)\n");
			this->fare += FARE_INFO::Fare_table("l", "j", this->total_jr_sales_km);
		} else {
			/* JR–kŠC“¹ Š²ü‚Ì‚İAŠ²ü+’n•ûŒğ’Êü */
			// (f)<c>	
			TRACE("fare(m,f)\n");
			fare = FARE_INFO::Fare_table("m", "f", this->total_jr_calc_km);
			if (fare <= 0) {
				TRACE("Table farem.f overflow!");
				this->fare += FARE_INFO::Fare_f(this->total_jr_calc_km);
			} else {
				this->fare += fare;
			}
		}				// JR‹ã
	} else if (0 < (this->kyusyu_sales_km + this->kyusyu_calc_km)) {
		/* JR‹ãB‚Ì‚İ */
		ASSERT(this->total_jr_sales_km == this->kyusyu_sales_km);
		ASSERT(this->total_jr_calc_km == this->kyusyu_calc_km);
		ASSERT(this->fare == 0);

		if (this->local_only) {
			/* JR‹ãB ’n•ûŒğ’Êü */
			TRACE("fare(ls)[9]?\n");
			/* (l) */
			this->fare = IDENT2(FARE_INFO::Fare_table(this->total_jr_calc_km, this->total_jr_sales_km));
		} else if (this->total_jr_calc_km != this->total_jr_sales_km) {
				/* JR‹ãB Š²ü+’n•ûŒğ’Êü */
				/* (n) */
			if ((KM(this->total_jr_calc_km) == 4) && (KM(this->total_jr_sales_km) == 3)) {
				this->fare = 170;	/* \ */
			} else if ((KM(this->total_jr_calc_km) == 11) && (KM(this->total_jr_sales_km) == 10)) {
				this->fare = 240;	/* \ */
			}
		}
		/* JR‹ãB Š²ü‚Ì‚İAŠ²ü{’n•ûŒğ’ÊüA’n•ûŒğ’Êü‚Ì‚İ ‚Ì(l), (n)”ñ“K—p */
		// (h)<s><c>
		if (this->fare == 0) {
			TRACE("fare(m, h)[9]\n");
			this->fare = FARE_INFO::Fare_table("m", "h", this->total_jr_calc_km);
			if (this->fare <= 0) {
				TRACE("Table farem.h overflow!");
				this->fare = FARE_INFO::Fare_h(this->total_jr_calc_km);
			}
		}
	} else if (0 < (this->shikoku_sales_km + this->shikoku_calc_km)) {
		/* JRl‘‚Ì‚İ */
		ASSERT(this->total_jr_sales_km == this->shikoku_sales_km);
		ASSERT(this->total_jr_calc_km == this->shikoku_calc_km);
		ASSERT(this->fare == 0);

		if (this->local_only) {
			/* JRl‘ ’n•ûŒğ’Êü */
			TRACE("fare(ls)[4]?\n");
			/* (k) */
			this->fare = IDENT1(FARE_INFO::Fare_table(this->total_jr_calc_km, this->total_jr_sales_km));
		} else if (this->total_jr_calc_km != this->total_jr_sales_km) {
				/* JRl‘ Š²ü+’n•ûŒğ’Êü */
				/* (m) */
			if ((KM(this->total_jr_calc_km) == 4) && (KM(this->total_jr_sales_km) == 3)) {
				this->fare = 160;	/* \ */
			} else if ((KM(this->total_jr_calc_km) == 11) && (KM(this->total_jr_sales_km) == 10)) {
				this->fare = 230;	/* \ */
			}
		}
		/* JRl‘ Š²ü‚Ì‚İAŠ²ü{’n•ûŒğ’ÊüA’n•ûŒğ’Êü‚Ì‚İ ‚Ì(l), (n)”ñ“K—p */
		// (g)<s><c>
		if (this->fare == 0) {
			TRACE("fare(m, g)[4]\n");
			this->fare = FARE_INFO::Fare_table("m", "g", this->total_jr_calc_km);
			if (this->fare <= 0) {
				TRACE("Table farem.g overflow!");
				this->fare = FARE_INFO::Fare_g(this->total_jr_calc_km);
			}
		}
	} else {
		ASSERT(FALSE);
		return false;
	}
	return true;
}

// ‰c‹ÆƒLƒ‚©‚ç—LŒø“ú”‚ğ“¾‚é
//	calc_fare() => 
//
//	@param [in] km    ‰c‹ÆƒLƒ
//	@return —LŒø“ú”
//
int FARE_INFO::days_ticket(int sales_km)
{
	if (sales_km < 1001) {
		return 1;
	}
	return (sales_km + 1999) / 2000 + 1;
}


//static
//	‰ïĞü‚Ì‰^’À‚ğ“¾‚é
//	calc_fare() => aggregate_fare_info() =>
//
//	@param [in] station_id1   ‰w1
//	@param [in] station_id2   ‰w2
//	@return ‰^’ÀŠz
//
int	FARE_INFO::Fare_company(int station_id1, int station_id2)
{
	static const char tsql[] = 
"select fare from t_clinfar"
" where "
" ((station_id1=?1 and station_id2=?2) or" 
"  (station_id1=?2 and station_id2=?1))";

	DBO dbo(DBS::getInstance()->compileSql(tsql));
	dbo.setParam(1, station_id1);
	dbo.setParam(2, station_id2);

	if (dbo.moveNext()) {
		return dbo.getInt(0);
	}
	return 0;
}

//static
//	‰^’Àƒe[ƒuƒ‹QÆ
//	calc_fare() => retr_fare() =>
//
//	@param [in] tbl   "m", "l",  "opq", "r"
//	@param [in] field name of column for retrieve fare in table.
//	@code
//			(tbl="m")	"f", "g", "h" JRO“‡
//	        (tbl="l")   "i", "j"      JR–{B^JR–k ’n•ûŒğ’Êü‚Ì‚İ
//	        (tbl="opq") "o", "p", "q" JR“Œ¼+JRO“‡:‰ÁZŠz
//	        (tbl="r")   "r"           JR“Œ+JR–k JR–k•”•ª‚ª’n•ûŒğ’Êü‚Ì‚İ:‰ÁZŠz
//	@endcode
//	@param [in] km   
//	@return fare [yen]
//
int	FARE_INFO::Fare_table(const char* tbl, const char* field, int km)
{
	static const char tsql[] = 
"select %s from t_fare%s where km<=? order by km desc limit(1)";
	char sql[128];
	sqlite3_snprintf(sizeof(sql), sql, tsql, field, tbl); 
	DBO dbo(DBS::getInstance()->compileSql(sql));
	dbo.setParam(1, KM(km));
	if (dbo.moveNext()) {
		return dbo.getInt(0);
	}
	return 0;
}

//static
//	‰^’Àƒe[ƒuƒ‹QÆ(ls)
//	calc_fare() => retr_fare() =>
//
//	@param [in] dkm   ‹[§ƒLƒ
//	@param [in] skm	  ‰c‹ÆƒLƒ
//	@return IDENT1(fare.k) + IDENT2(fare.l)
//
PAIRIDENT FARE_INFO::Fare_table(int dkm, int skm)
{
	static const char tsql[] = 
#if 1
	"select k,l from t_farels where dkm=?2 and (skm=-1 or skm=?1)";
#else
"select k,l from t_farels where "
"(-1=(select skm from t_farels where skm=(select max(skm) from t_farels where skm<=?1))"
"	or"
" skm=(select skm from t_farels where skm=(select max(skm) from t_farels where skm<=?1))"
") and dkm="
"(select dkm from t_farels where dkm=(select max(dkm) from t_farels where dkm<=?2))";
#endif
	DBO dbo(DBS::getInstance()->compileSql(tsql));
	dbo.setParam(1, KM(skm));
	dbo.setParam(2, KM(dkm));
	if (dbo.moveNext()) {
		return MAKEPAIR(dbo.getInt(0), dbo.getInt(1));
	} else {
		return MAKEPAIR(0, 0);
	}
}


//static 
//	“Á•Ê’Ê‰ß‰^’À‹æŠÔ‚©”»’è‚µ’Ê‰ß‚µ‚Ä‚¢‚½‚ç‰ÁZ‰^’ÀŠz‚ğ•Ô‚·
//	calc_fare() => aggregate_fare_info() =>
//
//	@param [in] line_id     ˜Hü
//	@param [in] station_id1 ‰w1
//	@param [in] station_id2 ‰w2
//
//	@return ‰w1`‰w2‚É‰^’À‚Æ‹æ•Ê‹æŠÔ‚ªŠÜ‚Ü‚ê‚Ä‚¢‚éê‡‚»‚Ì‰ÁZŠz‚ğ•Ô‚·
//
int FARE_INFO::CheckSpecficFarePass(int line_id, int station_id1, int station_id2)
{
	static const char tsql[] = 
"select station_id1, station_id2, fare from t_farest f where kind=0 and exists ("
"select *"
"	from t_lines"
"	where line_id=?1"
"	and station_id=f.station_id1"
"	and sales_km>="
"			(select min(sales_km)"
"			from t_lines"
"			where line_id=?1"
"			and (station_id=?2 or"
"				 station_id=?3))"
"	and sales_km<="
"			(select max(sales_km)"
"			from t_lines"
"			where line_id=?1"
"			and (station_id=?2 or"
"				 station_id=?3)))"
" and exists ("
" select *"
"	from t_lines"
"	where line_id=?1"
"	and station_id=f.station_id2"
"	and sales_km>="
"			(select min(sales_km)"
"			from t_lines"
"			where line_id=?1"
"			and (station_id=?2 or"
"				 station_id=?3))"
"	and sales_km<="
"			(select max(sales_km)"
"			from t_lines"
"			where line_id=?1"
"			and (station_id=?2 or"
"				 station_id=?3)))"
" order by fare desc"
" limit(1)";

	DBO dbo(DBS::getInstance()->compileSql(tsql));
	dbo.setParam(1, line_id);
	dbo.setParam(2, station_id1);
	dbo.setParam(3, station_id2);
	if (dbo.moveNext()) {
		int fare = dbo.getInt(2);
		TRACE(_T("CheckSpecificFarePass found: %s, %s, +%d\n"), Route::StationName(dbo.getInt(0)).c_str(), Route::StationName(dbo.getInt(1)).c_str(), fare);
		/* found, return values is add fare */
		return fare;
	}
	return 0;	/* not found */
}

//static 
//	“Á•Ê‰^’À‹æŠÔ‚©”»’è‚µŠY“–‚µ‚Ä‚¢‚½‚ç‰^’ÀŠz‚ğ•Ô‚·
//	calc_fare() =>
//
//	@param [in] station_id1 ‰w1(startStationId)
//	@param [in] station_id2 ‰w2(endStationId)
//
//	@return “Á•Ê‹æŠÔ‰^’À
//
int FARE_INFO::SpecficFareLine(int station_id1, int station_id2)
{
	static const char tsql[] = 
	"select fare from t_farest where kind=1 and"
	" ((station_id1=?1 and station_id2=?2) or" 
	"  (station_id1=?2 and station_id2=?1))";

	DBO dbo(DBS::getInstance()->compileSql(tsql));
	dbo.setParam(1, station_id1);
	dbo.setParam(2, station_id2);
	if (dbo.moveNext()) {
		int fare = dbo.getInt(0);
		TRACE(_T("SpecficFareLine found: %s - %s, +%d\n"), Route::StationName(station_id1).c_str(), Route::StationName(station_id2).c_str(), fare);
		return fare;	/* fare */
	}
	return 0;	/* not found */
}


//	‰^’ÀŒvZ(Šî–{)
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    ŒvZƒLƒ
//	@return ‰^’ÀŠz
//
int FARE_INFO::Fare_a(int km)
{
	int fare;
	int c_km;

	if (km < 31) {							// 1 to 3km
		return 140;
	}
	if (km < 61) {							// 4 to 6km
		return 180;
	}
	if (km < 101) {							// 7 to 10km
		return 190;
	}
	if (6000 < km) {						// 600km‰z‚¦‚Í40ƒLƒ‚İ
		c_km = (km - 1) / 400 * 400 + 200;
	} else if (1000 < km) {					// 100.1-600km‚Í20ƒLƒ‚İ
		c_km = (km - 1) / 200 * 200 + 100;
	} else if (500 < km) {					// 50.1-100km‚Í10ƒLƒ‚İ
		c_km = (km - 1) / 100 * 100 + 50;
	} else if (100 < km) {					// 10.1-50km‚Í5ƒLƒ‚İ
		c_km = (km - 1) / 50 * 50 + 30;
	} else {
		ASSERT(FALSE);
	}
	if (6000 <= c_km) {
		fare = 1620 * 3000 + 1285 * 3000 + 705 * (c_km - 6000);
	} else if (3000 < c_km) {
		fare = 1620 * 3000 + 1285 * (c_km - 3000);
	} else {
		fare = 1620 * c_km;
	}
	if (c_km <= 1000) {						// 100kmˆÈ‰º‚ÍØ‚èã‚°
		// 1‚ÌˆÊ‚ğØ‚èã‚°
		fare = (fare + 9999) / 10000 * 10;
	} else {								// 100‡q‰z‚¦‚ÍlÌŒÜ“ü
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd(fare);	// tax = +5%, lÌŒÜ“ü
}

//	b: “dÔ“Á’è‹æŠÔ(“Œ‹)
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    ‰c‹ÆƒLƒ
//	@return ‰^’ÀŠz
//
int FARE_INFO::Fare_b(int km)
{
	int fare;
	int c_km;

	if (km < 31) {							// 1 to 3km
		return 130;
	}
	if (km < 61) {							// 4 to 6km
		return 150;
	}
	if (km < 101) {							// 7 to 10km
		return 160;
	}
	
	if (6000 < km) {						// 600km‰z‚¦‚Í40ƒLƒ‚İ
		ASSERT(FALSE);
		return -1;
	} else if (1000 < km) {					// 100.1-600km‚Í20ƒLƒ‚İ
		c_km = (km - 1) / 200 * 200 + 100;
	} else if (500 < km) {					// 50.1-100km‚Í10ƒLƒ‚İ
		c_km = (km - 1) / 100 * 100 + 50;
	} else if (100 < km) {					// 10.1-50km‚Í5ƒLƒ‚İ
		c_km = (km - 1) / 50 * 50 + 30;
	} else {
		ASSERT(FALSE);
	}
	if (3000 < c_km) {
		fare = 1530 * 3000 + 1215 * (c_km - 3000);
	} else {
		fare = 1530 * c_km;
	}
	if (c_km <= 1000) {						// 100kmˆÈ‰º‚ÍØ‚èã‚°
		// 1‚ÌˆÊ‚ğØ‚èã‚°
		fare = (fare + 9999) / 10000 * 10;
	} else {				// 100‡q‰z‚¦‚ÍlÌŒÜ“ü
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd(fare);	// tax = +5%, lÌŒÜ“ü
}

//	C: “dÔ“Á’è‹æŠÔ(‘åã)
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    ‰c‹ÆƒLƒ
//	@return ‰^’ÀŠz
//
int FARE_INFO::Fare_c(int km)
{
	int fare;
	int c_km;

	if (km < 31) {							// 1 to 3km
		return 120;
	}
	if (km < 61) {							// 4 to 6km
		return 160;
	}
	if (km < 101) {							// 7 to 10km
		return 170;
	}
	if (6000 < km) {						// 600km‰z‚¦‚Í40ƒLƒ‚İ
		ASSERT(FALSE);
		return -1;
	} else if (1000 < km) {					// 100.1-600km‚Í20ƒLƒ‚İ
		c_km = (km - 1) / 200 * 200 + 100;
	} else if (500 < km) {					// 50.1-100km‚Í10ƒLƒ‚İ
		c_km = (km - 1) / 100 * 100 + 50;
	} else if (100 < km) {					// 10.1-50km‚Í5ƒLƒ‚İ
		c_km = (km - 1) / 50 * 50 + 30;
	} else {
		ASSERT(FALSE);
	}
	
	if (3000 < c_km) {
		fare = 1530 * 3000 + 1215 * (c_km - 3000);
	} else {
		fare = 1530 * c_km;
	}
	if (c_km <= 1000) {						// 100kmˆÈ‰º‚ÍØ‚èã‚°
		// 1‚ÌˆÊ‚ğØ‚èã‚°
		fare = (fare + 9999) / 10000 * 10;
	} else {								// 100‡q‰z‚¦‚ÍlÌŒÜ“ü
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd(fare);	// tax = +5%, lÌŒÜ“ü
}

//	d: “dÔ“Á’è‹æŠÔ(Rèü)
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    ‰c‹ÆƒLƒ
//	@return ‰^’ÀŠz
//
int FARE_INFO::Fare_d(int km)
{
	int fare;
	int c_km;

	if (km < 31) {							// 1 to 3km
		return 130;
	}
	if (km < 61) {							// 4 to 6km
		return 150;
	}
	if (km < 101) {							// 7 to 10km
		return 160;
	}
	if (3000 < km) {						// 300km‰z‚¦‚Í–¢’è‹`
		ASSERT(FALSE);
		return -1;
	} else if (1000 < km) {					// 100.1-300km‚Í20ƒLƒ‚İ
		c_km = (km - 1) / 200 * 200 + 100;
	} else if (500 < km) {					// 50.1-100km‚Í10ƒLƒ‚İ
		c_km = (km - 1) / 100 * 100 + 50;
	} else if (100 < km) {					// 10.1-50km‚Í5ƒLƒ‚İ
		c_km = (km - 1) / 50 * 50 + 30;
	} else {
		ASSERT(FALSE);
	}
	fare = 1325 * c_km;
	
	if (c_km <= 1000) {	// 100kmˆÈ‰º‚ÍØ‚èã‚°
		// 1‚ÌˆÊ‚ğØ‚èã‚°
		fare = (fare + 9999) / 10000 * 10;
	} else {								// 100‡q‰z‚¦‚ÍlÌŒÜ“ü
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd(fare);	// tax = +5%, lÌŒÜ“ü
}

//	e: “dÔ“Á’è‹æŠÔ(‘åãŠÂóü)
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    ‰c‹ÆƒLƒ
//	@return ‰^’ÀŠz
//
int FARE_INFO::Fare_e(int km)
{
	int fare;
	int c_km;

	if (km < 31) {							// 1 to 3km
		return 120;
	}
	if (km < 61) {							// 4 to 6km
		return 160;
	}
	if (km < 101) {							// 7 to 10km
		return 170;
	}
	if (3000 < km) {						// 300km‰z‚¦‚Í–¢’è‹`
		ASSERT(FALSE);
		return -1;
	} else if (1000 < km) {					// 100.1-300km‚Í20ƒLƒ‚İ
		c_km = (km - 1) / 200 * 200 + 100;
	} else if (500 < km) {					// 50.1-100km‚Í10ƒLƒ‚İ
		c_km = (km - 1) / 100 * 100 + 50;
	} else if (100 < km) {					// 10.1-50km‚Í5ƒLƒ‚İ
		c_km = (km - 1) / 50 * 50 + 30;
	} else {
		ASSERT(FALSE);
	}
	fare = 1325 * c_km;

	if (c_km <= 1000) {						// 100kmˆÈ‰º‚ÍØ‚èã‚°
		// 1‚ÌˆÊ‚ğØ‚èã‚°
		fare = (fare + 9999) / 10000 * 10;
	} else {								// 100‡q‰z‚¦‚ÍlÌŒÜ“ü
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd(fare);	// tax = +5%, lÌŒÜ“ü
}

//	f: JR–kŠC“¹Š²ü
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    ŒvZƒLƒ
//	@return ‰^’ÀŠz
//
int FARE_INFO::Fare_f(int km)
{
	int fare;
	int c_km;

	if (km < 31) {							// 1 to 3km
		return 160;
	}
	if (km < 61) {							// 4 to 6km
		return 200;
	}
	if (km < 101) {							// 7 to 10km
		return 210;
	}

	if (6000 < km) {						// 600km‰z‚¦‚Í40ƒLƒ‚İ
		c_km = (km - 1) / 400 * 400 + 200;
	} else if (1000 < km) {					// 100.1-600km‚Í20ƒLƒ‚İ
		c_km = (km - 1) / 200 * 200 + 100;
	} else if (500 < km) {					// 50.1-100km‚Í10ƒLƒ‚İ
		c_km = (km - 1) / 100 * 100 + 50;
	} else if (100 < km) {					// 10.1-50km‚Í5ƒLƒ‚İ
		c_km = (km - 1) / 50 * 50 + 30;
	} else {
		ASSERT(FALSE);
	}
	
	if (6000 < c_km) {
		fare = 1785 * 2000 + 1620 * 1000 + 1285 * 3000 + 705 * (c_km - 6000);
	} else if (3000 < c_km) {
		fare = 1785 * 2000 + 1620 * 1000 + 1285 * (c_km - 3000);
	} else if (2000 < c_km) {
		fare = 1785 * 2000 + 1620 * (c_km - 2000);
	} else {
		fare = 1785 * c_km;
	}
	if (c_km <= 1000) {						// 100kmˆÈ‰º‚ÍØ‚èã‚°
		// 1‚ÌˆÊ‚ğØ‚èã‚°
		fare = (fare + 9999) / 10000 * 10;
	} else {								// 100‡q‰z‚¦‚ÍlÌŒÜ“ü
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd(fare);	// tax = +5%, lÌŒÜ“ü
}

//	g: JRl‘
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    ŒvZƒLƒ
//	@return ‰^’ÀŠz
//
int FARE_INFO::Fare_g(int km)
{
	int fare;
	int c_km;

	if (km < 31) {							// 1 to 3km
		return 160;
	}
	if (km < 61) {							// 4 to 6km
		return 200;
	}
	if (km < 101) {							// 7 to 10km
		return 210;
	}

	if (6000 < km) {						// 600km‰z‚¦‚Í40ƒLƒ‚İ
		c_km = (km - 1) / 400 * 400 + 200;
	} else if (1000 < km) {					// 100.1-600km‚Í20ƒLƒ‚İ
		c_km = (km - 1) / 200 * 200 + 100;
	} else if (500 < km) {					// 50.1-100km‚Í10ƒLƒ‚İ
		c_km = (km - 1) / 100 * 100 + 50;
	} else if (100 < km) {					// 10.1-50km‚Í5ƒLƒ‚İ
		c_km = (km - 1) / 50 * 50 + 30;
	} else {
		ASSERT(FALSE);
	}
	
	if (6000 <= c_km) {
		fare = 1821 * 1000 + 1620 * 2000 + 1285 * 3000 + 705 * (c_km - 6000);
	} else if (3000 < c_km) {
		fare = 1821 * 1000 + 1620 * 2000 + 1285 * (c_km - 3000);
	} else if (1000 < c_km) {
		fare = 1821 * 1000 + 1620 * (c_km - 1000);
	} else {
		fare = 1821 * c_km;
	}
	
	if (c_km <= 1000) {						// 100kmˆÈ‰º‚ÍØ‚èã‚°
		// 1‚ÌˆÊ‚ğØ‚èã‚°
		fare = (fare + 9999) / 10000 * 10;
	} else {								// 100‡q‰z‚¦‚ÍlÌŒÜ“ü
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd(fare);	// tax = +5%, lÌŒÜ“ü
}

//	h: JR‹ãB
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    ŒvZƒLƒ
//	@return ‰^’ÀŠz
//
int FARE_INFO::Fare_h(int km)
{
	int fare;
	int c_km;

	// 1961kmˆÈ‰º‚Íƒe[ƒuƒ‹QÆ‚È‚Ì‚Å•s³Šm‚Å‚æ‚¢
	if (km <= 100) {						// ‚Æ‚Í‚¢‚Á‚Ä‚à100ˆÈ‰º‚¾‚¯‚ğÈ—ª
		return 0;
	}
	
	if (6000 < km) {						// 600km‰z‚¦‚Í40ƒLƒ‚İ
		c_km = (km - 1) / 400 * 400 + 200;
	} else if (1000 < km) {					// 100.1-600km‚Í20ƒLƒ‚İ
		c_km = (km - 1) / 200 * 200 + 100;
	} else if (500 < km) {					// 50.1-100km‚Í10ƒLƒ‚İ
		c_km = (km - 1) / 100 * 100 + 50;
	} else if (100 < km) {					// 10.1-50km‚Í5ƒLƒ‚İ
		c_km = (km - 1) / 50 * 50 + 30;
	} else {
		ASSERT(FALSE);
	}
	if (6000 <= c_km) {
		fare = 1775 * 3000 + 1285 * 3000 + 705 * (c_km - 6000);
	} else if (3000 < c_km) {
		fare = 1775 * 3000 + 1285 * (c_km - 3000);
	} else if (1000 < c_km) {
		fare = 1775 * c_km;
	} else {
		fare = 0;
	}

	if (c_km <= 1000) {							// 100kmˆÈ‰º‚ÍØ‚èã‚°
		// 1‚ÌˆÊ‚ğØ‚èã‚°
		fare = (fare + 9999) / 10000 * 10;
	} else {									// 100‡q‰z‚¦‚ÍlÌŒÜ“ü
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd(fare);	// tax = +5%, lÌŒÜ“ü
}

//public:
//	‰^’À•\¦
//	@param [in]  cooked : bit15=0/1  = ‹K‘¥“K—p/”ñ“K—p
//						: bit0=0/1 = ”­E’…‚ª“Á’è“s‹æs“à‚Å”­-’…‚ª100/200kmˆÈ‰º‚Ì‚Æ‚«A’…‚Ì‚İ“s‹æs“à—LŒø(APPLIED_TERMINAL)
//						: bit1=0/1 = ”­E’…‚ª“Á’è“s‹æs“à‚Å”­-’…‚ª100/200kmˆÈ‰º‚Ì‚Æ‚«A”­‚Ì‚İ“s‹æs“à—LŒø(APPLIED_START)
//							(–{ŠÖ”‚ª”²‚¯‚½ŒãAŠJn‰w‚ÌlineId‚Ìƒrƒbƒg7, 6‚ğQÆ‚µ‚ÄUI‚É‚æ‚èŒˆ‚ß‚é)
//	@return ‰^’ÀA‰c‹ÆƒLƒî•ñ •\¦•¶š—ñ
//
tstring Route::showFare(int cooked)
{
//	int fare2;			// Š”å2^ŠwŠ„
//	int fareC;			// üŠ„2 or üŠ„.5
	int fareW;			// ‰•œ
//	int fare4;			// Š”å4
#define MAX_BUF	1024
	TCHAR cb[MAX_BUF];
	tstring sResult;
	tstring sWork;
	tstring sExt;
	int rule114[3];

	if (route_list_raw.size() <= 1) {
		return tstring(_T(""));
	}

	if (RULE_APPLIED == (RULE_NO_APPLIED & cooked)) {
		/* ‹K‘¥“K—p */
		/* 86, 87, 69, 70ğ 114ğ“K—p‚©ƒ`ƒFƒbƒN */
		if (!checkOfRuleSpecificCoreLine(cooked, rule114)) {	// route_list_raw -> route_list_cooked
			rule114[0] = 0;
			sExt = _T("");
		} else {
			// rule 114 applied
			_sntprintf_s(cb, MAX_BUF, _T("‹K’ö114ğ“K—p‰^’ÀF\\%-5s ‰c‹ÆƒLƒF%6s km / ŒvZƒLƒF%6s km\r\n"), 
						 num_str_yen(rule114[0]).c_str(),
						 num_str_km(rule114[1]).c_str(),
						 num_str_km(rule114[2]).c_str());
			sExt = cb;
		}
		// ‰¼ª

		if (route_list_cooked.size() <= 1) {
ASSERT(FALSE);
			if (!fare_info.calc_fare(route_list_raw)) {
				return tstring(_T(""));
			}
		} else {
			/* Œo˜H•ÏX */
			sResult = _T("");

			/* ”­‰w */
			sWork = Route::CoreAreaNameByCityId(CSTART, 
						 						  route_list_cooked.front().lineId, 
						 						  route_list_cooked.front().flag);
			if (sWork == _T("")) {
				/* ’P‰w */
				sWork = Route::StationNameEx(route_list_cooked.front().stationId);
			}
			sResult += sWork;
			sResult += _T(" -> ");

			/* ’…‰w */
			sWork = Route::CoreAreaNameByCityId(CEND, 
						 						route_list_cooked.front().lineId, 
						 						route_list_cooked.back().flag);
			if (sWork == _T("")) {
				/* ’P‰w */
				sWork = Route::StationNameEx(route_list_cooked.back().stationId);
			}
			sResult += sWork;
			sResult += _T("\r\nŒo—RF");
			sResult += Route::show_route(true);
			
			if (!fare_info.calc_fare(route_list_cooked)) {
				return tstring(_T(""));
			}
		}
	} else {
		/* ‹K‘¥”ñ“K—p */
		sResult = _T("");

		/* ’P‰w */
		sWork = Route::StationNameEx(route_list_raw.front().stationId);
		sResult += sWork;
		sResult += _T(" -> ");
		sWork = Route::StationNameEx(route_list_raw.back().stationId);
		sResult += sWork;
		sResult += _T("\r\nŒo—RF");
		sResult += Route::show_route(false);
		
		if (!fare_info.calc_fare(route_list_raw)) {
			return tstring(_T(""));
		}
	}

	ASSERT(100<=fare_info.fare);
//	fare2 = fare_info.fare / 10 * 8 / 10 * 10;
//	fare4 = fare_info.fare / 10 * 6 / 10 * 10;
	if (6000 < fare_info.total_jr_calc_km) {
		fareW = fare_info.fare / 10 * 9 / 10 * 10 * 2;
//		if (MASK_FLAG_SHINKANSEN(fare_info.flag) == FLAG_TOKAIDO_SHINKANSEN) {
//			/* 600kmˆÈ‰º‚Å“ŒŠC“¹VŠ²üŠÜ‚Ş */
//			fareC = fare_info.fare / 100 * 95 / 10 * 10;	/* 0.5Š„ */
//		} else {
//			fareC = fare2;									/* 2Š„ */
//		}
	} else {
		fareW = fare_info.fare * 2;
//		if (2000 < fare_info.total_jr_calc_km) {
//			fareC = fare2;									/* 2Š„ */
//		} else {
//			fareC = fare_info.fare;							/* –³Š„ */
//		}
	}

	if ((MASK_URBNNO(fare_info.flag) != 0) && (MASK_FLAG_SHINKANSEN(fare_info.flag) == 0)) { 
		 /* ‘SŒo˜H‚ª‹ßx‹æŠÔ“à‚ÅVŠ²ü–¢—˜—p‚Ì‚Æ‚«Å’Z‹——£‚ÅZo‰Â”\ */
		_sntprintf_s(cb, MAX_BUF,
			_T("‹ßx‹æŠÔ“à‚Å‚·‚Ì‚ÅÅ’ZŒo˜H‚Ì‰^’À‚Å—˜—p‰Â”\‚Å‚·(“r’†‰ºÔ•s‰ÂA—LŒø“ú”“–“úŒÀ‚è)\r\n"));
		sResult += cb;
		//ASSERT(fare_info.avail_days <= 2);
		fare_info.avail_days = 1;	/* “–“úŒÀ‚è */
	}
	if (fare_info.total_jr_sales_km != fare_info.sales_km) {
		_sntprintf_s(cb, MAX_BUF,
							_T("‰c‹ÆƒLƒF%6s km(JRü‰c‹ÆƒLƒ / ŒvZƒLƒF%6s km / %6s km ‰ïĞü‰c‹ÆƒLƒF%6s km)\r\n"), 
							num_str_km(fare_info.sales_km).c_str(),
							num_str_km(fare_info.total_jr_sales_km).c_str(),
							num_str_km(fare_info.total_jr_calc_km).c_str(),
							num_str_km(fare_info.sales_km - fare_info.total_jr_sales_km).c_str());
		sResult += cb;
	} else {
		_sntprintf_s(cb, MAX_BUF,
							_T("‰c‹ÆƒLƒF%6s km ŒvZƒLƒF%6s km\r\n"), 
							num_str_km(fare_info.total_jr_sales_km).c_str(),
							num_str_km(fare_info.total_jr_calc_km).c_str());
		sResult += cb;
	}
	if (0 < fare_info.hokkaido_sales_km) {
		_sntprintf_s(cb, MAX_BUF, _T("JR–kŠC“¹‰c‹ÆƒLƒ / ŒvZƒLƒF%6s km / %6s km\r\n"),
						num_str_km(fare_info.hokkaido_sales_km).c_str(),
						num_str_km(fare_info.hokkaido_calc_km).c_str());
		sResult += cb;
	}
	if (0 < fare_info.shikoku_sales_km) {
		if (0 < fare_info.shikoku_calc_km) {
			_sntprintf_s(cb, MAX_BUF, _T("JRl‘‰c‹ÆƒLƒ / ŒvZƒLƒF%6s km / %6s km\r\n"),
							num_str_km(fare_info.shikoku_sales_km).c_str(),
							num_str_km(fare_info.shikoku_calc_km).c_str());
		} else {
			_sntprintf_s(cb, MAX_BUF, _T("JRl‘‰c‹ÆƒLƒF%6s km \r\n"),
							num_str_km(fare_info.shikoku_sales_km).c_str());
		}
		sResult += cb;
	}
	if (0 < fare_info.kyusyu_sales_km) {
		if (0 < fare_info.kyusyu_calc_km) {
			_sntprintf_s(cb, MAX_BUF, _T("JR‹ãB‰c‹ÆƒLƒ / ŒvZƒLƒF%6s km / %6s km\r\n"),
							num_str_km(fare_info.kyusyu_sales_km).c_str(),
							num_str_km(fare_info.kyusyu_calc_km).c_str());
		} else {
			_sntprintf_s(cb, MAX_BUF, _T("JR‹ãB‰c‹ÆƒLƒF%6s km \r\n"),
							num_str_km(fare_info.kyusyu_sales_km).c_str());
		}
		sResult += cb;
	}
	_sntprintf_s(cb, MAX_BUF,
				//_T("‰^’ÀF\\%5s     \\%5s[2Š„ˆø] \\%5s[üŠ„] \\%5s[‰•œ] \\%5s[4Š„ˆø]\r\n")
				_T("‰^’ÀF\\%-5s     \\%-5s[‰•œ]"),
						num_str_yen(fare_info.fare + fare_info.company_fare).c_str(),
						//num_str_yen(fare2).c_str(),
						//num_str_yen(fareC).c_str(),
						num_str_yen(fareW + fare_info.company_fare * 2).c_str());
						//num_str_yen(fare4).c_str(),
	sWork = cb;
	if (0 < fare_info.company_fare) {
		_sntprintf_s(cb, MAX_BUF,
					_T(" (‰ïĞüF\\%-5s)"), num_str_yen(fare_info.company_fare).c_str());
		sWork += cb;
	}
	_sntprintf_s(cb, MAX_BUF,
				_T("\r\n—LŒø“ú”F%4u“ú\r\n"),
						fare_info.avail_days);
	return sExt + sResult + sWork + cb;
}

//public:
//	‰^’ÀŒvZƒIƒvƒVƒ‡ƒ“‚ğ“¾‚é
//	@return 0 : –³‚µ(’Êí)(”­E’…‚ª“Á’è“s‹æs“à‰w‚Å“Á’è“s‹æs“àŠÔ‚ª100/200kmˆÈ‰º‚Å‚Í‚È‚¢)
//			 (ˆÈ‰ºA”­E’…‚ª“Á’è“s‹æs“à‰w‚Å“Á’è“s‹æs“àŠÔ‚ª100/200km‚ğ‰z‚¦‚é)
//			1 : Œ‹‰Ê•\¦ó‘Ô‚Í{“Á’è“s‹æs“à -> ’P‰w} (u”­‰w‚ğ’P‰w‚Éw’èv‚Æ•\¦)
//			2 : Œ‹‰Ê•\¦ó‘Ô‚Í{’P‰w -> “Á’è“s‹æs“à} (u’…‰w‚ğ’P‰w‚Éw’èv‚Æ•\¦)
//
int Route::fareCalcOption()
{
	if ((route_list_raw.size() <= 1) || (route_list_cooked.size() <= 1)) {
		return 0;
	}
	if (0 != (route_list_cooked.at(0).lineId & (1 << B1LID_BEGIN_CITY_OFF))) {
		return 1;
	}
	if (0 != (route_list_cooked.at(0).lineId & (1 << B1LID_FIN_CITY_OFF))) {
		return 2;
	}
	return 0;
}

//public:
//	Œo˜H‚ğ‹t“]
//
//	@retval 1   sucess
//	@retval 0   sucess(empty)
//	@retval -1	failure(6‚Ìš‚ğ‹t“]‚·‚é‚Æ9‚É‚È‚èŒo˜Hd•¡‚Æ‚È‚é‚½‚ß)
//
int Route::reverse()
{
	int station_id;
	int line_id;
	vector<RouteItem> route_list_rev;
	vector<RouteItem>::const_reverse_iterator rev_pos;
	vector<RouteItem>::const_iterator pos;

	if (route_list_raw.size() <= 1) {
		return 0;
	}

	for (rev_pos = route_list_raw.crbegin(); rev_pos != route_list_raw.crend(); rev_pos++) {
		route_list_rev.push_back(*rev_pos);
	}

	removeAll();	/* clear route_list_raw */

	pos = route_list_rev.cbegin();
	station_id = pos->stationId;
	add(station_id);
	line_id = pos->lineId;
	for (pos++; pos != route_list_rev.cend(); pos++) {
		int rc = add(line_id, /*station_id,*/ pos->stationId);
		if (rc < 0) {
			/* error */
			/* restore */
	
			removeAll();	/* clear route_list_raw */

			rev_pos = route_list_rev.crbegin();
			station_id = rev_pos->stationId;
			add(station_id);
			for (rev_pos++; rev_pos != route_list_rev.crend(); rev_pos++) {
				rc = add(rev_pos->lineId, /*station_id,*/ rev_pos->stationId);
				ASSERT(0 <= rc);
				station_id = rev_pos->stationId;
			}
			TRACE(_T("cancel reverse route\n"));
			return -1;
		}
		station_id = pos->stationId;
		line_id = pos->lineId;
	}
	TRACE(_T("reverse route\n"));
	return 1;
}


/*!	@brief ƒ‹[ƒgì¬(•¶š—ñ‚©‚çRouteƒIƒuƒWƒFƒNƒg‚Ìì¬)
 *
 *	@param [in] route_str	ƒJƒ“ƒ}‚È‚Ç‚ÌƒfƒŠƒ~ƒ^‚Å‹æØ‚ç‚ê‚½•¶š—ñ("‰wA˜HüA•ªŠò‰wA˜HüA..."j
 *	@retval -200 failure(‰w–¼•s³)
 *	@retval -300 failure(ü–¼•s³)
 *	@retval -1   failure(Œo˜Hd•¡•s³)
 *	@retval -2   failure(Œo˜H•s³)
 *	@retval 1 success
 *	@retval 0 success
 */
int Route::setup_route(LPCTSTR route_str)
{
	const static TCHAR* token = _T(", |/\t");
	TCHAR* p;
	int lineId = 0;
	int stationId1 = 0;
	int stationId2 = 0;
	int rc = 1;
	int len;
	TCHAR* ctx = NULL;
	
	removeAll();

	len = _tcslen(route_str) + 1;
	TCHAR *rstr = new TCHAR [len];
	if (rstr == NULL) {
		return -1;
	}
	_tcscpy_s(rstr, len, route_str);

	for (p = _tcstok_s(rstr, token, &ctx); p; p = _tcstok_s(NULL, token, &ctx)) {
		if (stationId1 == 0) {
			stationId1 = Route::GetStationId(p);
			if (stationId1 <= 0) {
				rc = -200;		/* illegal station name */
				break;
			}
			add(stationId1);
		} else if (lineId == 0) {
			lineId = Route::GetLineId(p);
			if (lineId <= 0) {
				rc = -300;		/* illegal line name */
				break;
			}
		} else {
			stationId2 = Route::GetStationId(p);
			if (stationId2 <= 0) {
				rc = -200;		/* illegal station name */
				break;
			}
			rc = add(lineId, /*stationId1,*/ stationId2);
			/* -1: route pass error
			 * -2: route error
			 * -100: db error
			 * 0: success(last)
			 * 1: success
			 */
ASSERT((rc == 0) || (rc == 1) || (rc == 10) || (rc == 11));
			if (rc < 0) {
				break;
			}
			lineId = 0;
			stationId1 = stationId2;
		}
	}
	delete [] rstr;

	return rc;
}

/*	ƒ‹[ƒg•\¦
 *	@param [in]  cooked : true/false  = ‹K‘¥“K—p/”ñ“K—p
 *	@return ƒ‹[ƒg•¶š—ñ
 *
 *	@remark showFare()‚ÌŒÄ‚Ño‚µŒã‚É‚Ì‚İ—LŒø
 */
tstring Route::show_route(bool cooked)
{
	tstring lineName;
	tstring stationName;
	const vector<RouteItem>* routeList;
#define MAX_BUF 1024
	//TCHAR buf[MAX_BUF];
	tstring result_str;
	
	if (cooked && 1 < route_list_cooked.size()) {
		routeList = &route_list_cooked;	/* ‹K‘¥“K—p */
	} else {
		routeList = &route_list_raw;	/* ‹K‘¥”ñ“K—p */
	}
	
	if (routeList->size() == 0) {	/* Œo˜H‚È‚µ(AutoRoute) */
		return _T("");
	}
	
	vector<RouteItem>::const_iterator pos = routeList->cbegin();

	result_str = _T("");

	for (pos++; pos != routeList->cend() ; pos++) {

		lineName = LineName(pos->lineId);

		if ((pos + 1) != routeList->cend()) {
			/* ’†ŠÔ‰w */
			if (!IS_FLG_HIDE_LINE(pos->flag)) {
				if (ID_L_RULE70 != pos->lineId) {
					result_str += _T("<");
					result_str += lineName;
					result_str += _T(">");
				} else {
					result_str += _T(",");
				}
			}
			if (!IS_FLG_HIDE_STATION(pos->flag)) {
				stationName = Route::StationName(pos->stationId);
				result_str += stationName;
			}
		} else {
			/* ’…‰w */
			if (!IS_FLG_HIDE_LINE(pos->flag)) {
				result_str += _T("<");
				result_str += lineName;
				result_str += _T(">");
			}
			//result_str += stationName;	// ’…‰w
			result_str += _T("\r\n");
		}
		//result_str += buf;
	}
	return result_str;
}

