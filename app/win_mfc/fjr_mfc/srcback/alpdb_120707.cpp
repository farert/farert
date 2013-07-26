#include "stdafx.h"

using namespace std;
#include <vector>


#define stationIsJunction(sid)	(0 != (attrOfStationId(sid) & (1<<12)))



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
		ASSERT(0 == route_list.size());
		route_list.clear();
		memset(jct_mask, 0, sizeof(jct_mask));
		TRACE(_T("clear-all mask.\n"));
		/* ’…‰w‚ª”­‰w`Å‰‚Ì•ªŠò‰wŠÔ‚É‚ ‚é‚©? */
		/* (’…‰w–¢w’è, ”­‰w=’…‰w‚Íœ‚­) */
		if ((0 < endStationId) && (endStationId != startStationId)) {
			rc = inStation(endStationId, line_id, stationId1, stationId2);
			if (rc != 0) {
				return -1;	/* <t> already passed error  */
			}
		}
		TRACE(_T("add-begin %s(%d)\n"), stationName(stationId1), stationId1);
		route_list.push_back(RouteItem(0, stationId1, attrOfStationId(stationId1)));
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
		if (startStationId == stationId1) {	/* ‰‰ñ */
			rc = 0;
														TRACE("  ADD-d+\n");
		} else if ((startStationId == stationId2) || (endStationId == stationId2)) {
			// ’·’Ã“c-“Œ_“Şì-ìè-—§ì-”ª‰¤q-’·’Ã“c
			rc = 2;	/* ’…‰w‚Åˆê•M‘‚«I—¹(2‚É‚·‚é‚ÆƒLƒƒƒ“ƒZƒ‹Å‹ß•ªŠò‚àOff‚·‚é) */
					/* 1‚¾‚ÆÅ‹ß•ªŠò‚Í‚ÍtypeP/O •Â‚¶“_‚È‚Ì‚ÅƒLƒƒƒ“ƒZƒ‹Off‚µ‚Ä‚Í‚È‚ç‚È‚¢ */
			//!!!!!!!!!!!!!!!!‚±‚ê‚Å‚Í•sŠ®‘S‚Æv‚¤‚Ì‚ÅŒŸ“¢
														TRACE("  ADD-e/\n");
		} else if ((route_list.size() <= 1) ||
				   (0 == inStation(stationId2, line_id, startStationId, route_list[1].stationId))) {
			// ”­‰w-’…‰w “¯ˆêü“à
			// ’Êí‚Ì”ñ•ªŠò‰w`”ñ•ªŠò‰w
			if (0 == inStation(endStationId, line_id, stationId1, stationId2)) {
			// ”ª‰¤q-b•{-•xm-“Œ_“Şì-”ª‰¤q
				rc = 0;	
														TRACE("  ADD-f+\n");
			} else {
				rc = -1;
														TRACE("  ADD-g-\n");
			}
		} else {
			// ’·’Ã“c-“Œ_“Şì-ìè-—§ì-”ª‰¤q-\“úsê(NG)
			rc = -1;
														TRACE("  ADD-h-\n");
		}
	} else {	// •œæ
		if (i <= (num - 1)) {	// “r’†•ªŠò‰w‚Å•œæ
			// ’·’Ã“c-‹´–{-Šƒ–è-“Œ_“Şì-”ª‰¤q ‚È‚Ç(‹´–{‚Å6‚Ìš)*/
			rc = -1;	// •œæ
														TRACE("  ADD-i-\n");
		} else {	// ÅI•ªŠò‰w‚Å•œæ
			// ’…‰w=”ñ•ªŠò‰w
			if (!stationIsJunction(stationId2)) {
				// --o--o--x
				rc = 1;				// typeO/P
			} else {


			// ŠJn‰w=’…‰w(”ñ•ªŠò‰w)
			if ((startStationId == stationId2) || (endStationId == stationId2)) {
				if (!stationIsJunction(stationId2)) {
					rc = -1;	// ”ªŠX-²‘q-¬“c-‰ä‘·q-í”Öü-V¼ŒË-¼‘D‹´-(²‘qx)-”ªŠX
								// ’·’Ã“c-‹´–{-Šƒ–è-“Œ_“Şì-(‹´–{<x>)-(I)•Ğ‘q
														TRACE("  ADD-i-\n");
				} else {
‚˜‚˜‚˜					rc = 2;		// ŠC˜V–¼-‹´–{-“Œ_“Şì-Šƒ–è-(ŠC˜V–¼x)-‹´–{x
				}
			} else 
		
		
			// !”ª‰¤q-b•{-•xm-“Œ_“Şì-”ª‰¤q
			// ’·’Ã“c-‹´–{-Šƒ–è-“Œ_“Şì-•Ğ‘q
			// ’·’Ã“c-‹´–{-Šƒ–è-“Œ_“Şì-‹´–{
			// ŠJn‰w‚’…‰w ‚Å ’…‰w‚Í”ñ•ªŠò‰w‚Ü‚½‚ÍÅIŒo˜H“à‚ÉŠJn‰w‚ª‚ ‚é
			if ((startStationId != stationId2) && 
				 (!stationIsJunction(stationId2) ||
				  (0 != inStation(startStationId, line_id, stationId1, stationId2)))) {
				rc = -1;
															TRACE("  ADD-a-\n");
			} else {
				// ”ª‰¤q-“Œ_“Şì-Šƒ–è-‹´–{
				//	ã‹L‚Å’…‚ğŒú–Ø‚É‚·‚é‚Æƒ`ƒFƒbƒN˜R‚ê‚Æ‚È‚éB‰º‹L‚ÍOK(’·’Ã“c’…‚É‚·‚é‚ÆNG)
				// ”ª‰¤q-‹´–{-Šƒ–è-“Œ_“Şì-‹´–{
				
				if ((0 == endStationId) || 
					(0 == inStation(endStationId, line_id, stationId1, stationId2))) {
					rc = 1;			// type O/P
															TRACE("  ADD-b/\n");
				} else {
					rc = -1;
															TRACE("  ADD-c-\n");
				}
			}
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

	route_list.push_back(RouteItem(line_id, stationId2, attrOfStationId(stationId2)));
	if (rc != 0) {
		TRACE(_T("added last.\n"));
		if (rc == 1) {
			last_flag = 1;
		} else { /* rc = 2 */
			last_flag = 0;	/* Ÿ‚ÉremoveTail‚ÅlastItem‚Ì’Ê‰ßƒ}ƒXƒN‚ğOff‚·‚é(typeO‚Å‚àP‚Å‚à‚È‚¢‚Ì‚Å) */
		}
		return 0;
	} else {	/* rc=0 */
		TRACE(_T("added continue.\n"));
		last_flag = 0;
		return 1;
	}
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


	route_num = route_list.size();
	if (route_num < 2) {
		ASSERT(FALSE);
		last_flag = 0;
		return;
	}
	
	/* ”­‰w`Å‰‚ÌæŠ·‰w */
	if (route_num == 2) {
		route_list.clear();
		memset(jct_mask, 0, sizeof(jct_mask));
		last_flag = 0;
		TRACE(_T("clear-all mask.\n"));
		return;
	}
	to_station_id = route_list[route_num - 1].stationId;	// tail
	line_id = route_list[route_num - 1].lineId;
	begin_station_id = route_list[route_num - 2].stationId;	// tail - 1

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

	route_list.pop_back();
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
	route_list.clear();
}

//	private:
//
//	•ªŠò‰w‚Å‚È‚¢‰w‚Ì•¡æ‚ğƒ`ƒFƒbƒN
//	(stationId1 or stationId2‚Í‰ß‹‚É’Ê‰ß‚µ‚Ä‚¢‚È‚¢•ªŠò‰w‚Å‚ ‚é‚±‚Æj
//
//  - route_list‚Ìæ“ª‰w(”­‰w)‚Ì‚İƒ`ƒFƒbƒN(”­‰wˆÈŠO‚Í•ªŠò‰wˆÈŠO‚ ‚è“¾‚È‚¢‚Ì‚Å)
//                 •ªŠò‰w‚Í’Êí‚ÌAddroute‚Å‚Ìd•¡ƒ`ƒFƒbƒN(jct_mask)‚Å
//                 ”»‚é‚Ì‚ÅB
//  - ”­‰w”­˜Hü‚ƒ`ƒFƒbƒN˜Hü(line_id)‚Í”ñŠY“–
//  - 
//  
//  
//  @retval 1   dup(terminate)
//	@retval	0	no dup(OK)
//	@retval -1  dup(NG)
//
int Route::lineDupCheck(int line_id, int stationId1, int stationId2)
{
	int rc;
	int firstStationId, secondStationId;
	
	if (route_list.size() <= 1) {
		if (endStationId <= 0) {
			return 0;
		}
		if (endStationId == stationId2) {
			return 1;	/* ’…‰w‚Åˆê•M‘‚«I—¹ */
		}
		firstStationId = stationId1;	// ”­‰w
		rc = inStation(endStationId, line_id, firstStationId, stationId2);
	} else {
		firstStationId = route_list[0].stationId;	// ”­‰w
		secondStationId = route_list[1].stationId;	// 1stæŠ·‰w
#if 0	/* –{¯‘ˆî“c(–k—¤VŠ²üAã‰zVŠ²ü2dŒËĞ)‚Ì—á‚ª‚ ‚è‚»‚¤‚È‚Ì‚Åƒgƒ‹ */
		if (route_list[1].lineId != line_id) { // list[1].line
			return 0;	/* ”­‰w”­˜Hü‚ªˆÙ‚È‚éê‡d‚È‚è‚æ‚¤‚ª‚È‚¢‚Ì‚ÅœŠO */
		}
#endif
		if (stationIsJunction(firstStationId)) {
			return 0;	/* ”­‰w=•ªŠò‰w‚È‚çƒ`ƒFƒbƒN‚³‚ê‚Ä‚¢‚éƒnƒY‚È‚Ì‚ÅOK */
		}
		if (stationIsJunction(stationId2)) {
			return 0;	/* ÅŒã(Current)‚Ì‰w=•ªŠò‰w */
		}
		if (firstStationId == stationId2) {
			return 1;	/* ’…‰w‚Åˆê•M‘‚«I—¹ */
		}
		rc = inStation(stationId2, line_id, firstStationId, secondStationId);
	}
	if (0 == rc) {
		return 0;	/* OK continue */
	} else {
		return -1;	/* NG */
	}
}

//	‰w‚ªŒo˜H“à‚ÉŠÜ‚Ü‚ê‚Ä‚¢‚é‚©H
//
bool Route::checkPassStation(int stationId)
{
	vector<RouteItem>::const_iterator route_item;
	int stationIdFrom = 0;

	for (route_item = route_list.cbegin(); route_item != route_list.cend(); route_item++) {
		if (stationIdFrom != 0) {
			if (inStation(stationId, route_item->lineId, stationIdFrom, route_item->stationId)) {
				return true;
			}
		} else {
			ASSERT(route_item == route_list.cbegin());
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
	
	for (i = 0; i < route_list.size(); i++) {
		if (stationIdFrom != 0) {
			/* stationId‚Íroute_list[i].lineId“à‚ÌstationIdFrom‚©‚ç
			              route_list[i].statonId‚ÌŠÔ‚É‚ ‚é‚©? */
			if (inStation(stationId, route_list[i].lineId, stationIdFrom, route_list[i].stationId)) {
				newLastIndex = i;
				line_id = route_list[i].lineId;
				stationIdTo = route_list[i].stationId;
				break;
			}
		} else {
			ASSERT(i == 0);
		}
		stationIdFrom = route_list[i].stationId;
	}
	if (newLastIndex < route_list.size()) {
		while (newLastIndex < route_list.size()) {
			removeTail();
		}
		ASSERT((newLastIndex<=1) || route_list[newLastIndex - 1].stationId == stationIdFrom);
		add(line_id, stationIdFrom, stationId);
		endStationId = stationId;
	}
}

#if 0
0		’·’Ã“c	
1	‰¡•lü	‹´–{
2	‘Š–Íü	Šƒ–è
3	“ŒŠC“¹ü •xm -> delete Šƒ–è-•xm
	g‰„ü	‘•ê  -> delete(‘•ê-•xm)


		’·’Ã“c	
	‰¡•lü	‹´–{
	‘Š–Íü	Šƒ–è
	“ŒŠC“¹ü ‘•{’Ã <- 
	Œä“aêü À’Ã
	“ŒŠC“¹ü •xm
	g‰„ü	‘•ê

	“ŒŠC“¹ü Šƒ–è “ñ‹{ ‘•{’Ã
#endif

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
int Route::attrOfStationId(int id)
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

	for (ite = route_list.cbegin(); ite != route_list.cend(); ite++) {
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


