#include "stdafx.h"

using namespace std;
#include <vector>


#define stationIsJunction(sid)	(0 != (attrOfStationId(sid) & (1<<12)))



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
		ASSERT(0 == route_list.size());
		route_list.clear();
		memset(jct_mask, 0, sizeof(jct_mask));
		TRACE(_T("clear-all mask.\n"));
		/* ���w�����w�`�ŏ��̕���w�Ԃɂ��邩? */
		/* (���w���w��, ���w=���w�͏���) */
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
		if (startStationId == stationId1) {	/* ���� */
			rc = 0;
														TRACE("  ADD-d+\n");
		} else if ((startStationId == stationId2) || (endStationId == stationId2)) {
			// ���Óc-���_�ސ�-���-����-�����q-���Óc
			rc = 2;	/* ���w�ň�M�����I��(2�ɂ���ƃL�����Z�����ŋߕ����Off����) */
					/* 1���ƍŋߕ���͂�typeP/O ���_�Ȃ̂ŃL�����Z����Off���Ă͂Ȃ�Ȃ� */
			//!!!!!!!!!!!!!!!!����ł͕s���S�Ǝv���̂Ō���
														TRACE("  ADD-e/\n");
		} else if ((route_list.size() <= 1) ||
				   (0 == inStation(stationId2, line_id, startStationId, route_list[1].stationId))) {
			// ���w-���w �������
			// �ʏ�̔񕪊�w�`�񕪊�w
			if (0 == inStation(endStationId, line_id, stationId1, stationId2)) {
			// �����q-�b�{-�x�m-���_�ސ�-�����q
				rc = 0;	
														TRACE("  ADD-f+\n");
			} else {
				rc = -1;
														TRACE("  ADD-g-\n");
			}
		} else {
			// ���Óc-���_�ސ�-���-����-�����q-�\���s��(NG)
			rc = -1;
														TRACE("  ADD-h-\n");
		}
	} else {	// ����
		if (i <= (num - 1)) {	// �r������w�ŕ���
			// ���Óc-���{-������-���_�ސ�-�����q �Ȃ�(���{��6�̎�)*/
			rc = -1;	// ����
														TRACE("  ADD-i-\n");
		} else {	// �ŏI����w�ŕ���
			// ���w=�񕪊�w
			if (!stationIsJunction(stationId2)) {
				// --o--o--x
				rc = 1;				// typeO/P
			} else {


			// �J�n�w=���w(�񕪊�w)
			if ((startStationId == stationId2) || (endStationId == stationId2)) {
				if (!stationIsJunction(stationId2)) {
					rc = -1;	// ���X-���q-���c-�䑷�q-��֐�-�V����-���D��-(���qx)-���X
								// ���Óc-���{-������-���_�ސ�-(���{<x>)-(�I)�Бq
														TRACE("  ADD-i-\n");
				} else {
������					rc = 2;		// �C�V��-���{-���_�ސ�-������-(�C�V��x)-���{x
				}
			} else 
		
		
			// !�����q-�b�{-�x�m-���_�ސ�-�����q
			// ���Óc-���{-������-���_�ސ�-�Бq
			// ���Óc-���{-������-���_�ސ�-���{
			// �J�n�w�����w �� ���w�͔񕪊�w�܂��͍ŏI�o�H���ɊJ�n�w������
			if ((startStationId != stationId2) && 
				 (!stationIsJunction(stationId2) ||
				  (0 != inStation(startStationId, line_id, stationId1, stationId2)))) {
				rc = -1;
															TRACE("  ADD-a-\n");
			} else {
				// �����q-���_�ސ�-������-���{
				//	��L�Œ������؂ɂ���ƃ`�F�b�N�R��ƂȂ�B���L��OK(���Óc���ɂ����NG)
				// �����q-���{-������-���_�ސ�-���{
				
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

	route_list.push_back(RouteItem(line_id, stationId2, attrOfStationId(stationId2)));
	if (rc != 0) {
		TRACE(_T("added last.\n"));
		if (rc == 1) {
			last_flag = 1;
		} else { /* rc = 2 */
			last_flag = 0;	/* ����removeTail��lastItem�̒ʉ߃}�X�N��Off����(typeO�ł�P�ł��Ȃ��̂�) */
		}
		return 0;
	} else {	/* rc=0 */
		TRACE(_T("added continue.\n"));
		last_flag = 0;
		return 1;
	}
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


	route_num = route_list.size();
	if (route_num < 2) {
		ASSERT(FALSE);
		last_flag = 0;
		return;
	}
	
	/* ���w�`�ŏ��̏抷�w�� */
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

	route_list.pop_back();
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
	route_list.clear();
}

//	private:
//
//	����w�łȂ��w�̕�����`�F�b�N
//	(stationId1 or stationId2�͉ߋ��ɒʉ߂��Ă��Ȃ�����w�ł��邱�Ɓj
//
//  - route_list�̐擪�w(���w)�̂݃`�F�b�N(���w�ȊO�͕���w�ȊO���蓾�Ȃ��̂�)
//                 ����w�͒ʏ��Addroute�ł̏d���`�F�b�N(jct_mask)��
//                 ����̂ŁB
//  - ���w���H�����`�F�b�N�H��(line_id)�͔�Y��
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
			return 1;	/* ���w�ň�M�����I�� */
		}
		firstStationId = stationId1;	// ���w
		rc = inStation(endStationId, line_id, firstStationId, stationId2);
	} else {
		firstStationId = route_list[0].stationId;	// ���w
		secondStationId = route_list[1].stationId;	// 1st�抷�w
#if 0	/* �{������c(�k���V�����A��z�V����2�d�ː�)�̗Ⴊ���肻���Ȃ̂Ńg�� */
		if (route_list[1].lineId != line_id) { // list[1].line
			return 0;	/* ���w���H�����قȂ�ꍇ�d�Ȃ�悤���Ȃ��̂ŏ��O */
		}
#endif
		if (stationIsJunction(firstStationId)) {
			return 0;	/* ���w=����w�Ȃ�`�F�b�N����Ă���n�Y�Ȃ̂�OK */
		}
		if (stationIsJunction(stationId2)) {
			return 0;	/* �Ō�(Current)�̉w=����w */
		}
		if (firstStationId == stationId2) {
			return 1;	/* ���w�ň�M�����I�� */
		}
		rc = inStation(stationId2, line_id, firstStationId, secondStationId);
	}
	if (0 == rc) {
		return 0;	/* OK continue */
	} else {
		return -1;	/* NG */
	}
}

//	�w���o�H���Ɋ܂܂�Ă��邩�H
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

//	�o�H���𒅉w�ŏI������(�ȍ~�̌o�H�̓L�����Z��)
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
			/* stationId��route_list[i].lineId����stationIdFrom����
			              route_list[i].statonId�̊Ԃɂ��邩? */
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
0		���Óc	
1	���l��	���{
2	���͐�	������
3	���C���� �x�m -> delete ������-�x�m
	�g����	����  -> delete(����-�x�m)


		���Óc	
	���l��	���{
	���͐�	������
	���C���� ���{�� <- 
	��a��� ����
	���C���� �x�m
	�g����	����

	���C���� ������ ��{ ���{��
#endif

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

//	�����v�Z
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


