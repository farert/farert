#include "stdafx.h"

using namespace std;
#include <vector>


#define stationIsJunction(sid)	(0 != (attrOfStationId(sid) & (1<<12)))



//	d•¡ƒ‹[ƒgœ‹
//
//	       Ÿ‰Y
//  ŠO–[ü ‘h‰ä <- remove
//  ŠO–[ü ç—t
//	‘•ü “Œ‹
//
int removeDupRoute(vector<RouteItem> *routeList)
{
	int last = routeList->size() - 2;
	int idx = 0;
	int affects = 0;
	
	while (idx < last) {
		if (routeList->at(idx).lineId == routeList->at(idx + 1).lineId) {
			routeList->erase(idx);
			last--;
			affects++;
		} else {
			idx++;
		}
	}
	return affects;
}

// 	•¶š—ñ‚ÍuŠ¿šv‚©u‚©‚Èv‚©H
//	u‚©‚Èv‚È‚çTrue‚ğ•Ô‚·
// 
static bool isKanaString(LPCTSTR szStr)
{
	LPCTSTR p = szStr;
	while (*p != _T('\0')) {
		if ((*p < _T('‚ ')) || (_T('‚ñ') < *p)) {
			return false;
		}
		p++;
	}
	return p != szStr; // •¶š”0‚àƒ_ƒ
}

//	3Œ…–ˆ‚ÉƒJƒ“ƒ}‚ğ•t‰Á‚µ‚½”’l•¶š—ñ‚ğì¬
//	(1/10‚µ‚½¬”“_•t‚«‰c‹ÆƒLƒ•\¦—p)
//
static CString num_str_km(int num)
{
	CString s;
	TCHAR c;
	int ll;
	
	s.Format(_T("%u"), num / 10);
	for (ll = s.GetLength() - 3; 0 < ll; ll -= 3) {
		s.Insert(ll, _T(","));
	}
	c = _T('0') + (num % 10);
	s += _T('.');
	s += c;
	return s;
}

//	3Œ…–ˆ‚ÉƒJƒ“ƒ}‚ğ•t‰Á‚µ‚½”’l•¶š—ñ‚ğì¬
//	‹àŠz•\¦—p
//
static CString num_str_yen(int num)
{
	CString s;
	int ll;
	
	s.Format(_T("%u"), num);
	for (ll = s.GetLength() - 3; 0 < ll; ll -= 3) {
		s.Insert(ll, _T(","));
	}
	return s;
}

////////////////////////////////////////////////////////////////////////
//
//	RouteƒNƒ‰ƒX ƒCƒ“ƒvƒŠƒƒ“ƒg
//


Route::Route()
{
	memset(jct_mask, 0, sizeof(jct_mask));
	startStationId = endStationId = 0;
}

Route::~Route()
{
}


	
// TimeSel.cpp
// ----------------------------------------------

// static
//	‰ïĞ&“s“¹•{Œ§ˆê——‚ğ—ñ‹“
//
DBO Route::enum_company_prefect()
{
	const static char tsql[] = 
"select name, rowid from t_company where name like 'JR%' union select name, rowid*65536 from t_prefect order by rowid;";
	return DBS::getInstance()->compileSql(tsql);
}

// static
//	“s“¹•{Œ§or‰ïĞ‚É‘®‚·‚é˜Hüˆê——(JR‚Ì‚İ)
//
DBO Route::enum_lines_from_company_prefect(int id)
{
	char sql[512];
	const char tsql[] = 
"select n.name, line_id, lflg from t_line n left join t_lines l on n.rowid=l.line_id left join"
" t_station t on t.rowid=l.station_id where %s=%d and (l.lflg&((1<<23)|(1<<31)|(1<<24)|(1<<22)))=0 and sales_km>=0 group by l.line_id order by n.name";

	int ident;

	if (0x10000 <= id) {
		ident = HIWORD(id);
	} else {
		ident = id;
	}
	sqlite3_snprintf(sizeof(sql), sql, tsql, 
	(0x10000 <= id) ? "prefect_id" : "company_id", ident);

	return DBS::getInstance()->compileSql(sql);
}


// static
//	‰w–¼‚Ìƒpƒ^[ƒ“ƒ}ƒbƒ`‚Ì—ñ‹“
//
DBO Route::enum_station_match(LPCTSTR station)
{
	char sql[256];
	const char tsql[] = "select name, rowid, samename from t_station where (sflg&(1<<23))=0 and %s like '%q%%'";
	const char tsql_s[] = " and samename='%q'";

	CString sameName;
	CString stationName(station);

	int pos = stationName.Find('(');
	if (0 <= pos) {
		sameName = stationName.Mid(pos);
		stationName = stationName.Left(pos);
		CT2A qsName(stationName, CP_UTF8);
		CT2A qsSame(sameName, CP_UTF8);

		sqlite3_snprintf(sizeof(sql), sql, tsql, 
								"name", qsName);
		sqlite3_snprintf(sizeof(sql) - lstrlenA(sql), sql + lstrlenA(sql), tsql_s,
								qsSame);
	} else {
		CT2A qsName(stationName, CP_UTF8);
		sqlite3_snprintf(sizeof(sql), sql, tsql, 
								isKanaString(stationName) ? "kana" : "name", qsName);
	}
	return DBS::getInstance()->compileSql(sql);
}

// static
//	‰w(Id)‚Ì“s“¹•{Œ§‚ğ“¾‚é 
//
CString Route::getPrefectByStationId(int stationId)
{
	static const char tsql[] = 
"select p.name from t_prefect p left join t_station t on t.prefect_id=p.rowid where t.rowid=?";

	DBO dbo(DBS::getInstance()->compileSql(tsql));
	dbo.setParam(1, stationId);

	if (dbo.moveNext()) {
		return dbo.getText(0);
	} else {
		return CString();
	}
}


// static
//	‰ïĞor“s“¹•{Œ§+˜Hü‚Ì‰w‚ÌƒCƒeƒŒ[ƒ^‚ğ•Ô‚·
//
DBO Route::enum_station_located_in_prefect_or_company_and_line(int prefectOrCompanyId, int lineId)
{
	static const  char tsql[] =
//"select t.name, station_id from t_lines l left join t_station t on t.rowid=l.station_id "
//" where line_id=? and %s=? order by sales_km";
"select t.name, station_id from t_station t left join t_lines l on t.rowid=l.station_id"
" where line_id=? and %s=? and (l.lflg&((1<<23)|(1<<31)|(1<<22)))=0 and sales_km>=0 order by sales_km";

	char sql[256];
	int ident;

	if (0x10000 <= prefectOrCompanyId) {
		ident = HIWORD(prefectOrCompanyId);
	} else {
		ident = prefectOrCompanyId;
	}

	sqlite3_snprintf(sizeof(sql), sql, tsql,
	(0x10000 <= prefectOrCompanyId) ? "prefect_id" : "company_id");

	DBO dbo(DBS::getInstance()->compileSql(sql));

	dbo.setParam(1, lineId);
	dbo.setParam(2, ident);

	return dbo;
}

// static
//	‰w–¼‚Ì‚æ‚İ‚ğ“¾‚é
//
CString Route::getKanaFromStationId(int stationId)
{
	static const  char tsql[] =
"select kana from t_station where rowid=?";

	DBO dbo = DBS::getInstance()->compileSql(tsql);

	dbo.setParam(1, stationId);

	if (dbo.moveNext()) {
		return dbo.getText(0);
	} else {
		return CString();
	}
}


// static
//	‰w‚ÌŠ‘®˜Hü‚ÌƒCƒeƒŒ[ƒ^‚ğ•Ô‚·
//
DBO* Route::enum_line_of_stationId(int stationId)
{
	static const  char tsql[] =
"select n.name, line_id from t_line n left join t_lines l on n.rowid=l.line_id"
" where station_id=? and (lflg&((1<<31)|(1<<24)|(1<<22)))=0 and sales_km>=0";

	static DBO dbo;
	
	if (!dbo) {
		dbo = DBS::getInstance()->compileSql(tsql);
	} else {
		dbo.reset();
	}
	dbo.setParam(1, stationId);

	return &dbo;
}



// alps_mfcDlg.cpp
// ----------------------------------------------

//static 
//	˜Hü‚Ì•ªŠò‰wˆê——ƒCƒeƒŒ[ƒ^‚ğ•Ô‚·
//	@param [in] lineId	˜HüId
//	@param [in] stationId ’…‰wId(‚±‚Ì‰w‚àˆê——‚ÉŠÜ‚ß‚é)
//
DBO* Route::enum_junction_of_lineId(int lineId, int stationId)
{
	static const  char tsql[] =
"select t.name, station_id, sflg&(1<<12)"
" from t_lines l left join t_station t on t.rowid=l.station_id"
" where line_id=?1 and ((lflg & (1<<12))<>0  or station_id=?2)"
" and (lflg&((1<<24)|(1<<31)|(1<<22)))=0 and sales_km>=0"
" order by l.sales_km";

	static DBO dbo;
	
	if (!dbo) {
		dbo = DBS::getInstance()->compileSql(tsql);
	} else {
		dbo.reset();
	}
	dbo.setParam(1, lineId);
	dbo.setParam(2, stationId);

	return &dbo;
}

//static 
//	˜Hü“à‚Ì‰wˆê——ƒCƒeƒŒ[ƒ^‚ğ•Ô‚·
//
DBO* Route::enum_station_of_lineId(int lineId)
{
	static const  char tsql[] =
"select t.name, station_id, sflg&(1<<12)"
" from t_lines l left join t_station t on t.rowid=l.station_id"
" where line_id=?"
" and (lflg&((1<<24)|(1<<31)|(1<<22)))=0 and sales_km>=0"
" order by l.sales_km";

	static DBO dbo;
	
	if (!dbo) {
		dbo = DBS::getInstance()->compileSql(tsql);
	} else {
		dbo.reset();
	}
	dbo.setParam(1, lineId);

	return &dbo;
}


//static
//	‰w‚Ì—×‚Ì•ªŠò‰w‚ğ•Ô‚·
//	(”ñ•ªŠò‰w‚ğw’è‚·‚é‚Æ³‚µ‚¢Œ‹‰Ê‚É‚È‚ç‚È‚¢)
//
DBO* Route::enum_neer_node(int stationId)
{
	static const  char tsql[] =
"select 	station_id , abs(("
"	select case when calc_km>0 then calc_km else sales_km end "
"	from t_lines "
"	where 0<=sales_km and 0=(lflg&((1<<24)|(1<<31))) "
"	and line_id=(select line_id "
"				 from	t_lines "
"				 where	station_id=?1" 
"				 and	0<=sales_km "
"				 and	0=(lflg&((1<<24)|(1<<31)))) "
"	and station_id=?1)-case when calc_km>0 then calc_km else sales_km end) as cost"
" from 	t_lines "
" where 	0<=sales_km and 0=(lflg&((1<<24)|(1<<31)))"
" and	line_id=(select	line_id "
" 				 from	t_lines "
" 				 where	station_id=?1"
" 				 and	0<=sales_km"
" 				 and	0=(lflg&((1<<24)|(1<<31))))"
" and	sales_km in ((select max(y.sales_km)"
"					  from	t_lines x left join t_lines y"
"					  on	x.line_id=y.line_id "
"					  where	0<=x.sales_km and 0=(x.lflg&((1<<24)|(1<<31)))"
"					  and	0<=y.sales_km and (1<<12)=(y.lflg&((1<<24)|(1<<31)|(1<<12)))"
"					  and	x.station_id=?1"
"					  and	x.sales_km>y.sales_km"
"					 ),"
"					 (select min(y.sales_km)"
"					  from	t_lines x left join t_lines y"
"					  on	x.line_id=y.line_id "
"					  where 0<=x.sales_km and 0=(x.lflg&((1<<24)|(1<<31)))"
"					  and	0<=y.sales_km and (1<<12)=(y.lflg&((1<<24)|(1<<31)|(1<<12)))"
"					  and	x.station_id=?1"
"					  and	x.sales_km<y.sales_km))";
	static DBO dbo;
	
	if (!dbo) {
		dbo = DBS::getInstance()->compileSql(tsql);
	} else {
		dbo.reset();
	}
	dbo.setParam(1, stationId);

	return &dbo;
}

//static 
// ‰w‚Ì—×‚Ì•ªŠò‰w”‚ğ“¾‚é
//	(–Ó’°ü‚Ì‰w‚©”Û‚©‚Ì‚İ‚ğ•Ô‚·)
//
int Route::numOfNeerNode(int stationId)
{
	if (stationIsJunction(stationId)) {
		return 2;	// 2ˆÈã‚ ‚é‚±‚Æ‚à‚ ‚é‚ª‰R‚Â‚«
	}
	DBO* dbo =  enum_neer_node(stationId);
	int c;
	
	c = 0;
	while (dbo->moveNext()) {
		++c;
	}
	return c;
}

//
//	˜Hü‚Ì‰wŠÔ‚É‰w‚Í‚ ‚é‚©H
//	lineId‚Ìb_stationId to e_stationId in stationId ?
//
bool Route::inStation(int stationId, int lineId, int b_stationId, int e_stationId)
{
	static const char tsql[] =
"select count(*) from ("
" select station_id"
" from t_lines"
" where line_id=?1"
" and station_id"
" in (select station_id "
"      from t_lines "
"      where line_id=?1 "
"      and ((sales_km>=(select sales_km "
"				from t_lines "
"				where line_id=?1 "
"				and station_id=?2 and (lflg&(1<<31))=0)"
"      and  (sales_km<=(select sales_km "
"				from t_lines "
"				where line_id=?1 "
"				and station_id=?3 and (lflg&(1<<31))=0))) "
"	or  (sales_km<=(select sales_km "
"				from t_lines "
"				where line_id=?1 "
"				and station_id=?2 and (lflg&(1<<31))=0)"
"		and (sales_km>=(select sales_km "
"				from t_lines "
"				where line_id=?1 "
"				and station_id=?3 and (lflg&(1<<31))=0)))))"
") where station_id=?4";

	static DBO dbo;
	if (!dbo) {
		dbo = DBS::getInstance()->compileSql(tsql);
	} else {
		dbo.reset();
	}
	if (dbo.isvalid()) {
		dbo.setParam(1, lineId);
		dbo.setParam(2, b_stationId);
		dbo.setParam(3, e_stationId);
		dbo.setParam(4, stationId);

		if (dbo.moveNext()) {
			return 0 < dbo.getInt(0);
		}
	}
	return false;
}

//	˜Hü‚Ìbegin_station_id‰w‚©‚çto_station_id‰w‚Ü‚Å‚Ì•ªŠò‰wƒŠƒXƒg‚ğ•Ô‚·
//
//
//
DBO& Route::enum_junctions_of_line(int line_id, int begin_station_id, int to_station_id)
{
	static const char tsql[] = 
"select id from t_lines l join t_jct j on j.station_id=l.station_id where"
" line_id=?1 and (lflg&(1<<12))!=0 and (lflg&(1<<31))=0 and"
" case when"
" (select sales_km from t_lines where line_id=?1 and station_id=?3) <"
" (select sales_km from t_lines where line_id=?1 and station_id=?2) then" 
" (((sales_km>=(select sales_km from t_lines where line_id=?1 and station_id=?3)) and"
" (sales_km<=(select sales_km from t_lines where line_id=?1 and station_id=?2))) ) else"
" (((sales_km<=(select sales_km from t_lines where line_id=?1 and station_id=?3)) and"
" (sales_km>=(select sales_km from t_lines where line_id=?1 and station_id=?2))) ) end"
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
	static DBO dbo;
	if (!dbo) {
		dbo = DBS::getInstance()->compileSql(tsql);
	} else {
		dbo.reset();
	}
	if (dbo.isvalid()) {
		dbo.setParam(1, line_id);
		dbo.setParam(2, begin_station_id);
		dbo.setParam(3, to_station_id);
	}
	return dbo;
}

//	70ği“ü˜HüA’Eo˜Hü‚©‚çi“üA’Eo‹«ŠE‰w‚Æ‰c‹ÆƒLƒA˜HüID‚ğ•Ô‚·
//
bool Route::retrieve_70inout(int line_id1, int line_id2, int* line_id, int* station_id1, int* station_id2)
{
	static const char tsql[] =
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

	static DBO dbo;
	if (!dbo) {
		dbo = DBS::getInstance()->compileSql(tsql);
	} else {
		dbo.reset();
	}
	if (dbo.isvalid()) {
		dbo.setParam(1, line_id1);
		dbo.setParam(2, line_id2);
	}
	if (dbo.moveNext()) {
		*line_id = dbo.getInt(0);
		*station_id1 = dbo.getInt(1);
		*station_id2 = dbo.getInt(2);
		return true; 
	}
	return false;
}

/*	Œo˜H’Ç‰Á
 *
 *  0 = OK(last)
 *  1 = OK
 *  2 = OK(re-route)
 * -1 = overpass(•œæƒGƒ‰[)
 * -100 DB error
 *
 *	last_flg(rc) :  0 = Ÿ‚ÉremoveTail‚ÅlastItem‚Ì’Ê‰ßƒ}ƒXƒN‚ğOff‚·‚é(typeO‚Å‚àP‚Å‚à‚È‚¢‚Ì‚Å)
 *				not 0 = Ÿ‚ÉremoveTail‚ÅlastItem‚Ì’Ê‰ßƒ}ƒXƒN‚ğOff‚É‚µ‚È‚¢(typeO/P)
 *
 */
int Route::add(int line_id, int stationId1, int stationId2)
{
	int rc;
	unsigned int i;
	unsigned int j;
	unsigned int num;
	int jct_on;

	if (startStationId == stationId1) {	/* ‰‰ñ */
		/* ”­‰w */
		ASSERT(0 == route_list_raw.size());
		route_list_raw.clear();
		memset(jct_mask, 0, sizeof(jct_mask));
		TRACE(_T("clear-all mask.\n"));

		/* ’…‰w‚ª”­‰w`Å‰‚Ì•ªŠò‰wŠÔ‚É‚ ‚é‚©? */
		/* (’…‰w–¢w’è, ”­‰w=’…‰w‚Íœ‚­) */
		if ((0 < endStationId) && (endStationId != startStationId) && 
			(endStationId != stationId2) &&
				(0 != inStation(endStationId, line_id, stationId1, stationId2))) {
			return -1;	/* <t> already passed error  */
		}
		TRACE(_T("add-begin %s(%d)\n"), stationName(stationId1), stationId1);
		route_list_raw.push_back(RouteItem(0, stationId1, attrOfStationOnLineLine(line_id, stationId1)));
	}
	TRACE(_T("add %s(%d)-%s(%d), %s(%d)\n"), lineName(line_id), line_id, stationName(stationId1), stationId1, stationName(stationId2), stationId2);
	
	DBO& dbo = enum_junctions_of_line(line_id, stationId1, stationId2);
	if (!dbo.isvalid()) {
		return -100;
	}

	vector<int> junctions;
	while (dbo.moveNext()) {
		junctions.push_back(dbo.getInt(0));
	}

	num = junctions.size();

	for (i = 0; i < num; i++) {
		jct_on = junctions[i];
		if (i != 0) {	/* ŠJn‰w(i=0)‚Í‘O˜Hü’…‰w(or ”­‰w)‚È‚Ì‚Åƒ`ƒFƒbƒN‚µ‚È‚¢*/
			if ((jct_mask[jct_on / 8] & (1 << (jct_on % 8))) != 0) {
				// Šù‚É’Ê‰ßÏ‚İ
				break;	/* already passed error */
			}
		}
		jct_mask[jct_on / 8] |= (1 << (jct_on % 8));
		TRACE(_T("  add-mask on: %s(%d,%d)\n"), jctName(junctions[i]).GetBuffer(), jct2id(junctions[i]), junctions[i]);
	}
	
	if (num <= i) {	// •œæ‚È‚µ
		if (((2 <= route_list_raw.size()) && (startStationId != stationId2) && (0 != inStation(startStationId, line_id, stationId1, stationId2))) ||
			(((0 < endStationId) && (endStationId != stationId2) && (2 <= route_list_raw.size())) &&
			(0 != inStation(endStationId, line_id, stationId1, stationId2)))) {
			rc = -1;	/* <v> <p> <l> <w> */
		} else if (startStationId == stationId2) {
			rc = 2;		/* <f> */
		} else {
			rc = 0;		/* <a> <d> <g> */
		}
	} else {	// •œæ
		if ((num - 1) == i) { /* last */
			if ((!stationIsJunction(stationId2)) ||
				((2 <= route_list_raw.size()) && (startStationId != stationId2) && (0 != inStation(startStationId, line_id, stationId1, stationId2))) ||
				(((0 < endStationId) && (endStationId != stationId2) && (2 <= route_list_raw.size())) &&
				(0 != inStation(endStationId, line_id, stationId1, stationId2)))) {
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
			jct_mask[jct_on / 8] &= ~(1 << (jct_on % 8));	// off
			TRACE(_T("  add_mask off: %s\n"), jctName(jct_on).GetBuffer());
		}
		TRACE(_T("add_abort\n"));
		last_flag = 0;
		return -1;	/* already passed error */
	}

	route_list_raw.push_back(RouteItem(line_id, stationId2, attrOfStationOnLineLine(line_id, stationId2)));

	if (rc == 0) {
		TRACE(_T("added continue.\n"));
		last_flag = 0;
		return 1;
	} else if (rc == 1) {
		last_flag = 1;
	} else if (rc = 2) {
		last_flag = 0;	/* Ÿ‚ÉremoveTail‚ÅlastItem‚Ì’Ê‰ßƒ}ƒXƒN‚ğOff‚·‚é(typeO‚Å‚àP‚Å‚à‚È‚¢‚Ì‚Å) */
	} else {
		ASSERT(FALSE);
		last_flag = 0;
	}
	TRACE(_T("added last.\n"));
	return 0;
}

//	Œo˜H‚Ì––”ö‚ğœ‹
//
void Route::removeTail(bool begin_off/* = false*/ )
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
		last_flag = 0;
		return;
	}
	
	/* ”­‰w`Å‰‚ÌæŠ·‰w */
	if (route_num == 2) {
		route_list_raw.clear();
		memset(jct_mask, 0, sizeof(jct_mask));
		last_flag = 0;
		TRACE(_T("clear-all mask.\n"));
		return;
	}
	to_station_id = route_list_raw[route_num - 1].stationId;	// tail
	line_id = route_list_raw[route_num - 1].lineId;
	begin_station_id = route_list_raw[route_num - 2].stationId;	// tail - 1

	DBO& dbo = enum_junctions_of_line(line_id, to_station_id, begin_station_id);
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
		if ((i != 0) || (last_flag == 0)) {
			jct_mask[junctions[i] / 8] &= ~(1 << (junctions[i] % 8));
			TRACE(_T("removed   : %s\n"), jctName(junctions[i]).GetBuffer());
		}
	}

	last_flag = 0;

	route_list_raw.pop_back();
}


//	Œo˜Hİ’è’† Œo˜Hd•¡”­¶
//	Œo˜Hİ’èƒLƒƒƒ“ƒZƒ‹
//	stop_jctId > 0 : w’è•ªŠò‰w‘O‚Ü‚ÅƒNƒŠƒA(w’è•ªŠò‰wŠÜ‚Ü‚¸)
//	stop_jctId = 0 : (default)w’è•ªŠò‰ww’è‚È‚µ
//	stop_jctId < 0 : ŠJn’n“_‚àíœ
//
void Route::removeAll()
{
	memset(jct_mask, 0, sizeof(jct_mask));
	startStationId = endStationId = 0;
	route_list_raw.clear();
}


//	‰w‚ªŒo˜H“à‚ÉŠÜ‚Ü‚ê‚Ä‚¢‚é‚©H
//
bool Route::checkPassStation(int stationId)
{
	vector<RouteItem>::const_iterator route_item;
	int stationIdFrom = 0;

	for (route_item = route_list_raw.cbegin(); route_item != route_list_raw.cend(); route_item++) {
		if (stationIdFrom != 0) {
			if (inStation(stationId, route_item->lineId, stationIdFrom, route_item->stationId)) {
				return true;
			}
		} else {
			ASSERT(route_item == route_list_raw.cbegin());
			ASSERT(startStationId == route_item->stationId);
		}
		stationIdFrom = route_item->stationId;
	}
	return false;
}

//	Œo˜H“à‚ğ’…‰w‚ÅI—¹‚·‚é(ˆÈ~‚ÌŒo˜H‚ÍƒLƒƒƒ“ƒZƒ‹)
//
void Route::terminate(int stationId)
{
	unsigned i;
	int stationIdFrom = 0;
	unsigned newLastIndex = 0xffffffff;
	int line_id;
	int stationIdTo;
	
	for (i = 0; i < route_list_raw.size(); i++) {
		if (stationIdFrom != 0) {
			/* stationId‚Íroute_list_raw[i].lineId“à‚ÌstationIdFrom‚©‚ç
			              route_list_raw[i].statonId‚ÌŠÔ‚É‚ ‚é‚©? */
			if (inStation(stationId, route_list_raw[i].lineId, stationIdFrom, route_list_raw[i].stationId)) {
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
	if (newLastIndex < route_list_raw.size()) {
		while (newLastIndex < route_list_raw.size()) {
			removeTail();
		}
		ASSERT((newLastIndex<=1) || route_list_raw[newLastIndex - 1].stationId == stationIdFrom);
		add(line_id, stationIdFrom, stationId);
		endStationId = stationId;
	}
}


// static
//	•ªŠòID¨‰wID
//
int Route::jct2id(int jctId)
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
int Route::id2jctId(int stationId)
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
CString Route::jctName(int jctId)
{
	TCHAR name[MAX_STATION_CHR];
	
	memset(name, 0, sizeof(name));
	
	DBO ctx = DBS::getInstance()->compileSql(
		"select name from t_jct j left join t_station t on j.station_id=t.rowid where id=?");
	if (ctx.isvalid()) {

		ctx.setParam(1, jctId);

		if (ctx.moveNext()) {
			_tcscpy_s(name, ctx.getText(0));
		}
	}
	return name;
}

//static 
//	‰wID¨‰w–¼
//
CString Route::stationName(int id)
{
	TCHAR name[MAX_STATION_CHR];
	
	memset(name, 0, sizeof(name));
	
	DBO ctx = DBS::getInstance()->compileSql(
		"select name from t_station where rowid=?");
	if (ctx.isvalid()) {

		ctx.setParam(1, id);

		if (ctx.moveNext()) {
			_tcscpy_s(name, ctx.getText(0));
		}
	}
	return name;
}

//static 
//	˜HüID¨˜Hü–¼
//
CString Route::lineName(int id)
{
	TCHAR name[MAX_STATION_CHR];
	
	memset(name, 0, sizeof(name));
	
	DBO ctx = DBS::getInstance()->compileSql(
		"select name from t_line where rowid=?");
	if (ctx.isvalid()) {

		ctx.setParam(1, id);

		if (ctx.moveNext()) {
			_tcscpy_s(name, ctx.getText(0));
		}
	}
	return name;
}

//static 
//	‰w‚Ì‘®«‚ğ“¾‚é
//
unsigned int Route::attrOfStationId(int id)
{
	TCHAR name[MAX_STATION_CHR];
	
	memset(name, 0, sizeof(name));
	
	DBO ctx = DBS::getInstance()->compileSql(
		"select sflg from t_station where rowid=?");
	if (ctx.isvalid()) {

		ctx.setParam(1, id);

		if (ctx.moveNext()) {
			return ctx.getInt(0);
		}
	}
	return 0;
}

//static 
//	˜Hü‰w‚Ì‘®«‚ğ“¾‚é
//
unsigned int Route::attrOfStationOnLineLine(int line_id, int station_id)
{
	TCHAR name[MAX_STATION_CHR];
	
	memset(name, 0, sizeof(name));
	
	DBO ctx = DBS::getInstance()->compileSql(
		"select lflg from t_lines where line_id=?1 and station_id=?2");
	if (ctx.isvalid()) {

		ctx.setParam(1, line_id);
		ctx.setParam(2, station_id);

		if (ctx.moveNext()) {
			return ctx.getInt(0);
		}
	}
	return 0;
}


// Private:

//	—¿‹àŒvZ
//
void Route::calc_fare()
{
	vector<RouteItem>::const_iterator ite;
	int lineId;
	int t1;
	int t2;
	
	memset(&fare_info, 0, sizeof(fare_info));

	for (ite = route_list_raw.cbegin(); ite != route_list_raw.cend(); ite++) {
		lineId = ite->lineId;		// HIWORD(*ite);
		if (lineId == 0) {
			t1 = ite->stationId;	// LOWORD(*ite);
		} else {
			t2 = ite->stationId;	// LOWORD(*ite);
			calc_km(lineId, t1, t2);
			t1 = t2;
		}
	}
}

// Private:
//	ŒvZƒLƒŒvZ
//
int Route::calc_km(int line_id, int stationId1, int stationId2)
{

	return 0;
}

//	—¿‹àŒvZ
//
//	@param [in] km			km[x10]
//	@param [out] ava_days	day of the available.
//							(null is not receive)
//	@retval fare [yen]
//
int Route::fare(int km, int* ava_days)
{
	int b_km, e_km;
	int fare;
	int c_km;
	
	if (6000 < km) {
		b_km = (km - 1) / 400 * 400 + 10;
		c_km = b_km + 190;
		e_km = b_km + 390;
	} else if (1000 < km) {
		b_km = (km - 1) / 200 * 200 + 10;
		c_km = b_km + 90;
		e_km = b_km + 190;
	} else if (500 < km) {
		b_km = (km - 1) / 100 * 100 + 10;
		c_km = b_km + 40;
		e_km = b_km + 90;
	} else if (100 < km) {
		b_km = (km - 1) / 50 * 50 + 10;
		c_km = b_km + 20;
		e_km = b_km + 40;
	} else if (60 < km) {
		b_km = (61 - 1) / 30 * 30 + 10;
		c_km = b_km + 10;
		e_km = b_km + 30;
	} else if (30 < km) {
		b_km = (31 - 1) / 10 * 10 + 10;
		c_km = b_km + 10;
		e_km = b_km + 20;
	} else {
		b_km = (11 - 1) / 20 * 20 + 10;
		c_km = b_km + 10;
		e_km = b_km + 20;
	}


	if (6000 <= c_km) {
		fare = 1620 * 3000 + 1285 * 3000 + 705 * (c_km - 6000);
	} else if (3000 < c_km) {
		fare = 1620 * 3000 + 1285 * (c_km - 3000);
	} else {
		fare = 1620 * c_km;
	}
	
	if (b_km < 1000) {
		// 1‚ÌˆÊ‚ğØ‚èã‚°
		fare = (fare + 9999) / 10000 * 10;
		if (ava_days) {
			*ava_days = 1;									// —LŒøŠúŠÔ
		}
	} else {
		fare = (fare + 50000) / 100000 * 100;
		
		// —LŒøŠúŠÔ
		if (ava_days) {
			*ava_days = (e_km + 1990) / 2000 + 1;
		}
	}
	if (b_km < 100) {
		// “Á—á
		if (b_km < 30) {
			fare = 140;	// fare
		} else if (b_km < 60) {
			fare = 180;	// fare
		} else {
			fare = 190;	// fare
		}
	} else {
		fare = fare + ((fare / 10 / 2) + 5) / 10 * 10;		// tax = 5%
	}
	return fare;
}

//	public:
//		‰^’À•\¦
//
CString Route::showFare()
{
	int skm,  ckm,  fare,  avaDays,  speflg;
	int fare2;
	int fare5;
	int fareW;
	int fare4;

	CString sResult;
	CString sRoute;
	
	vector<RouteItem>::iterator pos;
	for (pos = routeList().begin(); pos != routeList().end(); ++pos) {
		if (pos == routeList().begin()) {
			sRoute.Format(_T("%s - "), Route::stationName(pos->stationId));
		} else {
			sRoute += _T('<');
			sRoute += Route::lineName(pos->lineId);
			sRoute += _T('>');
			if (*pos == routeList().back()) {
				sRoute += _T(" - ");
			}
			sRoute += Route::stationName(pos->stationId);
		}
	}
	sRoute += _T("\r\n----------\r\n");
	
	getCurFare(&skm, &ckm, &fare, &avaDays, &speflg);

	fare2 = fare - fare / 20;
	fare5 = fare - fare / 5;
	if (6000 < skm) {
		fareW = (fare - (fare / 10)) * 2;
	} else {
		fareW = fare * 2;
	}
	fare4 = fare - fare / 40;

	sResult.Format(
_T("‰c‹ÆƒLƒF%6s km\r\n")
_T("ŒvZƒLƒF%6s km\r\n")
_T("‰^’ÀF\\%5s\r\n")
_T("      \\%5s[2Š„ˆø]  \\%5s[0.5Š„ˆø]\r\n")
_T("      \\%5s[‰•œ]   \\%5s[4Š„ˆø]\r\n")
_T("—LŒø“ú”F%4u\r\n"),
						num_str_km(skm),
						num_str_km(ckm),
						num_str_yen(fare),
						num_str_yen(fare2),
						num_str_yen(fare5),
						num_str_yen(fareW),
						num_str_yen(fare4),
						avaDays);
	return sRoute + sResult;
}


//	70ğ“K—p
//	route_list‚ğ“K—p‚µ‚Äout_route_list‚Éİ’è‚·‚é
//	out_route_list‚Í•\¦—p‚Å‚Í‚È‚­ŒvZ—p(˜Hü‚ª70ğê—p‚ª‚ ‚é)
//
int Route::checkOfRule70(vector<RouteItem>* out_route_list)
{
	int stage;

	vector<RouteItem>::const_iterator route_item;
	vector<RouteItem>::const_iterator route_item_1;
	vector<RouteItem>::const_iterator route_item_2;

	stage = 0;

	for (route_item = route_list_raw.cbegin(); route_item != route_list_raw.cend(); route_item++) {
		if (stage == 0) {
			if ((route_item->flag & 24) == 0) {
				stage = 1;					/* 1: off */
			} else {
				break;	/* 70ğ“K—pƒGƒŠƒA“à”­‚Í”ñ“K—p(86A87ğ) stage==0 */
			}
		} else if (stage == 1) {
			if ((route_item->flag & 24) != 0) {
				stage = 2;					/* 2: on */
				route_item_1 = route_item;
			}
		} else if (stage == 2) {
			if ((route_item->flag & 24) == 0) {
				stage = 3;					/* 3: on: !70 -> 70 -> !70 (applied) */
				route_item_2 = route_item;
			}
		} else if (stage == 3) {
			/* 4 */
			if ((route_item->flag & 24) != 0) {
				stage = 4;
				break;					/* !70 -> 70 -> !70 -> 70 (exclude) */
			}
		}
	}
	if (stage == 3) {
		// “K‡
		vector<RouteItem> route_70list;
		int line_id;
		int station_id1;
		int station_id2;
		if (!retrieve_70inout(route_item_1->lineId, route_item_2->lineId,
							  &line_id, &station_id1, &station_id2)) {
			ASSERT(FALSE);
			return -1;
		}
		if (station_id1 == station_id2) {
			for (route_item = route_list_raw.cbegin(); route_item != route_list_raw.cend(); route_item++) {
				out_route_list->push_back(*route_item);
			}
		} else {
			for (route_item = route_list_raw.cbegin(); route_item != route_list_raw.cend(); route_item++) {
				if ((stage == 3) && (route_item == route_item_1)) {
					stage = 4;
					out_route_list->push_back(RouteItem(route_item->lineId, station_id1, 0));
				} else if ((stage == 3) || (stage == 5)) {
					out_route_list->push_back(*route_item);
				} else if ((stage == 4) && (route_item == route_item_2)) {
					out_route_list->push_back(RouteItem(line_id, station_id2, 0));
					out_route_list->push_back(*route_item);
					stage = 5;
				}
			}
		}
		return 0;	// done
	} else {
		// ”ñ“K‡
		return -1;
	}
}

#if 0
		@b•{
’†‰›“Œü@“Œ‹ route_item_1
“ŒŠC“¹ü@Ã‰ª route_item_2

		@b•{
’†‰›“Œü@‘ãX–Ø station_id1
(line_id) •iì station_id2
“ŒŠC“¹ü@Ã‰ª

	‘o—t
í”Öü@“ú•é—¢
“Œ–kü@¬R

	‘o—t
í”Öü@“ú•é—¢
(line_id) “ú•é—¢
“Œ–kü@¬R

#endif


//	69ğ“K—p‰w‚ğ•Ô‚·
//	’[‰w1`’[‰w2‚ÌƒyƒA~N‚ğ‚©‚¦‚·.
//	results[]”z—ñ‚Í•K‚¸n*2‚ğ•Ô‚·
//
//	flg: 0 ‘±‚«–³‚µ
//       1 ‘±‚«
//	return: 0 ‘±‚«–³‚µ
//          1 ‘±‚«
//         -1 a69‚È‚µ
int Route::query_a69list(int line_id, int station_id1, int station_id2, vector<unsigned int>* results, int coninue_flg)
{
	const static char tsql[] = 
//" select station_id, (lflg>>17)&15, (lflg>>29)&1,  (lflg>>28)&1"
" select station_id, lflg"
" from t_lines"
" where line_id=?1"
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
"                  and station_id=?3))))) and (lflg&(1<<29))!=0";
	
	int prev_flag = 0;
	int prev_stid = 0;
	int cur_flag;
	int cur_stid;
	
	results->clear();
	
	vector<int> pre_list;
	
	DBO ctx = DBS::getInstance()->compileSql(tsql);
	if (ctx.isvalid()) {
		ctx.setParam(1, line_id);
		ctx.setParam(2, station_id1);
		ctx.setParam(3, station_id2);
		if (ctx.moveNext()) {
			pre_list.push_back(ctx.getInt(0));
			pre_list.push_back(ctx.getInt(1));
		}
		vector<int>::const_iterator it;
		for (it = pre_list.cbegin(); it != pre_list.cend(); it++) {
			cur_stid = *it;
			++it;
			ASSERT(it != pre_list.cend());
			cur_flag = *it;

			if ((0 < prev_stid) && (((prev_flg >> 17) & 0x0f) == ((cur_flg >> 17) & 0x0f))) {
				if (continue_flag != 0) {
					if (((prev_flg & (1 << 28)) != 0) && 
					    (((continue_flag >> 17) & 0x0f) == ((prev_flg >> 17) & 0x0f))) {
						// OK
						results->push_back(MAKELONG(prev_stid, prev_flag));
						results->push_back(MAKELONG(cur_stid, cur_flag));
						prev_stid = 0;
					} else {
						// NG
						prev_flag = cur_flag;
						prev_stid = cur_stid;
					}
				} else { /* continue_flag == 0) */
					if (((it - 1) == pre_list.cbegin()) && (station_id1 == prev_stid)) {
						// OK
					} else {

					}
				} else if ((coninue_flg == 0) && ((prev_flg & (1 << 28)) != 0)) && 
				  ((
				(((cur_flg & (1 << 28)) == 0) || (((it + 1) == pre_list.cend()) && (station_id2 == cur_stid)))) {
				results->push_back(MAKELONG(prev_stid, prev_flag));
				results->push_back(MAKELONG(cur_stid, cur_flag));
				prev_stid = 0;
			} else {
				prev_flag = cur_flag;
				prev_stid = cur_stid;
			}
		}
		
		return 2 <= results->size();
	}
	return -1;
}

//	69ğ’uŠ·˜HüA‰w‚ğ•Ô‚·
//
bool Route::query_rule69t(int ident, vector<vector<unsigned int>>* results)
{
	const static char tsql[] = 
	" select station_id1, station_id2, line_id from t_rule69 where id=? order by ord";

	vector<unsigned int> record;

	results->clear();
	
	DBO ctx = DBS::getInstance()->compileSql(tsql);
	if (ctx.isvalid()) {
		ctx.setParam(1, ident);
		if (ctx.moveNext()) {
			record.push_back(ctx.getInt(0), ctx.getInt(1), ctx.getInt(2));
			results->push_back(record);
		}
	}
	return 0 < results->size();
}


//	69ğ“K—p
//	route_list‚ğ“K—p‚µ‚Äout_route_list‚Éİ’è‚·‚é
//	out_route_list‚Í•\¦—p‚Å‚Í‚È‚­ŒvZ—p
//
int Route::checkOfRule69(vector<RouteItem>* out_route_list)
{
	int i;
	int continue_flag;
	int station_id1;
	vector<unsigned int> dbrecord;
	vector<unsigned int> a69list;

	vector<RouteItem>::const_iterator route_item;

	continue_flag = 0;
	station_id1 = 0;

	for (route_item = route_list_raw.cbegin(); route_item != route_list_raw.cend(); route_item++) {
		if (station_id1 != 0) {
			continue_flag = query_a69list(route_item->lineId, station_id1, route_item->stationId2, &dbrecord, continue_flag);
			if (0 <= continue_flag) {
				for (i = 0; i < dbrecord.size(); i += 2) {
					if (dbrecord.size() < 2) {
						ASSERT(FALSE);
						break;
					}
					if (continue_flag) {
						if (station_id1 == LOWORD(dbrecord[i])) {
							// OK
						} else {
							continue_flg = 0;
						}
					}
					/* continue ? */
					if ((dbrecord[i + 1] & (1 << 28)) != 0) {
						continue_flag = HIWORD(dbrecord[i + 1]);
						ASSERT((i + 1) == dbrecord.size());	// last
						a69list.push_back(MAKE
					} else {
						// ’uŠ·ˆ—
						
					}
					station_id1 = 0;
				}
			} else {
				continue_flag = 0;
				station_id1 = 0;
			}
		} else {
			station_id1 = route_item->stationId;
		}
	}

}





