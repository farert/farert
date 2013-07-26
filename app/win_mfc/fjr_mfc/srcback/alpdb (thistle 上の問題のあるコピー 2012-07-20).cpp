#include "stdafx.h"

using namespace std;
#include <vector>


#define stationIsJunction(sid)	(0 != (attrOfStationId(sid) & (1<<12)))



//	�d�����[�g����
//
//	       ���Y
//  �O�[�� �h�� <- remove
//  �O�[�� ��t
//	������ ����
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

// 	������́u�����v���u���ȁv���H
//	�u���ȁv�Ȃ�True��Ԃ�
// 
static bool isKanaString(LPCTSTR szStr)
{
	LPCTSTR p = szStr;
	while (*p != _T('\0')) {
		if ((*p < _T('��')) || (_T('��') < *p)) {
			return false;
		}
		p++;
	}
	return p != szStr; // ������0���_��
}

//	3�����ɃJ���}��t���������l��������쐬
//	(1/10���������_�t���c�ƃL���\���p)
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

//	3�����ɃJ���}��t���������l��������쐬
//	���z�\���p
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
//	Route�N���X �C���v�������g
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
//	���&�s���{���ꗗ���
//
DBO Route::enum_company_prefect()
{
	const static char tsql[] = 
"select name, rowid from t_company where name like 'JR%' union select name, rowid*65536 from t_prefect order by rowid;";
	return DBS::getInstance()->compileSql(tsql);
}

// static
//	�s���{��or��Ђɑ�����H���ꗗ(JR�̂�)
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
//	�w���̃p�^�[���}�b�`�̗�
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
//	�w(Id)�̓s���{���𓾂� 
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
//	���or�s���{��+�H���̉w�̃C�e���[�^��Ԃ�
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
//	�w���̂�݂𓾂�
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
//	�w�̏����H���̃C�e���[�^��Ԃ�
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
//	�H���̕���w�ꗗ�C�e���[�^��Ԃ�
//	@param [in] lineId	�H��Id
//	@param [in] stationId ���wId(���̉w���ꗗ�Ɋ܂߂�)
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
//	�H�����̉w�ꗗ�C�e���[�^��Ԃ�
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
//	�w�ׂ̗̕���w��Ԃ�
//	(�񕪊�w���w�肷��Ɛ��������ʂɂȂ�Ȃ�)
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
// �w�ׂ̗̕���w���𓾂�
//	(�Ӓ����̉w���ۂ��݂̂�Ԃ�)
//
int Route::numOfNeerNode(int stationId)
{
	if (stationIsJunction(stationId)) {
		return 2;	// 2�ȏ゠�邱�Ƃ����邪�R��
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
//	�H���̉w�Ԃɉw�͂��邩�H
//	lineId��b_stationId to e_stationId in stationId ?
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

//	�H����begin_station_id�w����to_station_id�w�܂ł̕���w���X�g��Ԃ�
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

//	70��i���H���A�E�o�H������i���A�E�o���E�w�Ɖc�ƃL���A�H��ID��Ԃ�
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

/*	�o�H�ǉ�
 *
 *  0 = OK(last)
 *  1 = OK
 *  2 = OK(re-route)
 * -1 = overpass(����G���[)
 * -100 DB error
 *
 *	last_flg(rc) :  0 = ����removeTail��lastItem�̒ʉ߃}�X�N��Off����(typeO�ł�P�ł��Ȃ��̂�)
 *				not 0 = ����removeTail��lastItem�̒ʉ߃}�X�N��Off�ɂ��Ȃ�(typeO/P)
 *
 */
int Route::add(int line_id, int stationId1, int stationId2)
{
	int rc;
	unsigned int i;
	unsigned int j;
	unsigned int num;
	int jct_on;

	if (startStationId == stationId1) {	/* ���� */
		/* ���w */
		ASSERT(0 == route_list_raw.size());
		route_list_raw.clear();
		memset(jct_mask, 0, sizeof(jct_mask));
		TRACE(_T("clear-all mask.\n"));

		/* ���w�����w�`�ŏ��̕���w�Ԃɂ��邩? */
		/* (���w���w��, ���w=���w�͏���) */
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
		if (i != 0) {	/* �J�n�w(i=0)�͑O�H�����w(or ���w)�Ȃ̂Ń`�F�b�N���Ȃ�*/
			if ((jct_mask[jct_on / 8] & (1 << (jct_on % 8))) != 0) {
				// ���ɒʉߍς�
				break;	/* already passed error */
			}
		}
		jct_mask[jct_on / 8] |= (1 << (jct_on % 8));
		TRACE(_T("  add-mask on: %s(%d,%d)\n"), jctName(junctions[i]).GetBuffer(), jct2id(junctions[i]), junctions[i]);
	}
	
	if (num <= i) {	// ����Ȃ�
		if (((2 <= route_list_raw.size()) && (startStationId != stationId2) && (0 != inStation(startStationId, line_id, stationId1, stationId2))) ||
			(((0 < endStationId) && (endStationId != stationId2) && (2 <= route_list_raw.size())) &&
			(0 != inStation(endStationId, line_id, stationId1, stationId2)))) {
			rc = -1;	/* <v> <p> <l> <w> */
		} else if (startStationId == stationId2) {
			rc = 2;		/* <f> */
		} else {
			rc = 0;		/* <a> <d> <g> */
		}
	} else {	// ����
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
		// �s�����[�g�Ȃ̂�mask�����ɖ߂�
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
		last_flag = 0;	/* ����removeTail��lastItem�̒ʉ߃}�X�N��Off����(typeO�ł�P�ł��Ȃ��̂�) */
	} else {
		ASSERT(FALSE);
		last_flag = 0;
	}
	TRACE(_T("added last.\n"));
	return 0;
}

//	�o�H�̖���������
//
void Route::removeTail(bool begin_off/* = false*/ )
{
	int line_id;
	int begin_station_id;
	int to_station_id;
	int jct_num;
	int route_num;
	int i;
	vector<int> junctions;	// ����w���X�g


	route_num = route_list_raw.size();
	if (route_num < 2) {
		ASSERT(FALSE);
		last_flag = 0;
		return;
	}
	
	/* ���w�`�ŏ��̏抷�w�� */
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
		jct_num -= 1;		/* �J�n�w��Off���Ȃ�(�O�H���̒��w�Ȃ̂�) */
	}
	for (i = 0; i < jct_num; i++) {
		/* i=0:�ŋߕ���w��O�^�o�H�AP�^�o�H�̒��w�̏ꍇ�͏��O */
		if ((i != 0) || (last_flag == 0)) {
			jct_mask[junctions[i] / 8] &= ~(1 << (junctions[i] % 8));
			TRACE(_T("removed   : %s\n"), jctName(junctions[i]).GetBuffer());
		}
	}

	last_flag = 0;

	route_list_raw.pop_back();
}


//	�o�H�ݒ蒆 �o�H�d��������
//	�o�H�ݒ�L�����Z��
//	stop_jctId > 0 : �w�蕪��w�O�܂ŃN���A(�w�蕪��w�܂܂�)
//	stop_jctId = 0 : (default)�w�蕪��w�w��Ȃ�
//	stop_jctId < 0 : �J�n�n�_���폜
//
void Route::removeAll()
{
	memset(jct_mask, 0, sizeof(jct_mask));
	startStationId = endStationId = 0;
	route_list_raw.clear();
}


//	�w���o�H���Ɋ܂܂�Ă��邩�H
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

//	�o�H���𒅉w�ŏI������(�ȍ~�̌o�H�̓L�����Z��)
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
			/* stationId��route_list_raw[i].lineId����stationIdFrom����
			              route_list_raw[i].statonId�̊Ԃɂ��邩? */
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
//	����ID���wID
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
//	�wID������ID
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
//	����ID���w��
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
//	�wID���w��
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
//	�H��ID���H����
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
//	�w�̑����𓾂�
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
//	�H���w�̑����𓾂�
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

//	�����v�Z
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
//	�v�Z�L���v�Z
//
int Route::calc_km(int line_id, int stationId1, int stationId2)
{

	return 0;
}

//	�����v�Z
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
		// 1�̈ʂ�؂�グ
		fare = (fare + 9999) / 10000 * 10;
		if (ava_days) {
			*ava_days = 1;									// �L������
		}
	} else {
		fare = (fare + 50000) / 100000 * 100;
		
		// �L������
		if (ava_days) {
			*ava_days = (e_km + 1990) / 2000 + 1;
		}
	}
	if (b_km < 100) {
		// ����
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
//		�^���\��
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
_T("�c�ƃL���F%6s km\r\n")
_T("�v�Z�L���F%6s km\r\n")
_T("�^���F\\%5s\r\n")
_T("      \\%5s[2����]  \\%5s[0.5����]\r\n")
_T("      \\%5s[����]   \\%5s[4����]\r\n")
_T("�L�������F%4u\r\n"),
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


//	70��K�p
//	route_list��K�p����out_route_list�ɐݒ肷��
//	out_route_list�͕\���p�ł͂Ȃ��v�Z�p(�H����70���p������)
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
				break;	/* 70��K�p�G���A�����͔�K�p(86�A87��) stage==0 */
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
		// �K��
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
		// ��K��
		return -1;
	}
}

#if 0
		�@�b�{
���������@���� route_item_1
���C�����@�É� route_item_2

		�@�b�{
���������@��X�� station_id1
(line_id) �i�� station_id2
���C�����@�É�

	�o�t
��֐��@���闢
���k���@���R

	�o�t
��֐��@���闢
(line_id) ���闢
���k���@���R

#endif


//	69��K�p�w��Ԃ�
//	�[�w1�`�[�w2�̃y�A�~N��������.
//	results[]�z��͕K��n*2��Ԃ�
//
//	flg: 0 ��������
//       1 ����
//	return: 0 ��������
//          1 ����
//         -1 a69�Ȃ�
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

//	69��u���H���A�w��Ԃ�
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


//	69��K�p
//	route_list��K�p����out_route_list�ɐݒ肷��
//	out_route_list�͕\���p�ł͂Ȃ��v�Z�p
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
						// �u������
						
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





