#include "stdafx.h"
#include "alpdb.h"

/*!	@file alpdb.cpp core logic implement.
 *	Copyright(c) sutezo9@me.com 2012.
 *  Visual C++ Version2010 and Objective-C(X-code5.0)
 */
#if 0

'Farert'
Copyright (C) 2014 Sutezo (sutezo666@gmail.com)

   'Farert' is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    'Farert' is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with 'Farert'.  If not, see <http://www.gnu.org/licenses/>.

このプログラムはフリーソフトウェアです。あなたはこれを、フリーソフトウェ
ア財団によって発行された GNU 一般公衆利用許諾契約書(バージョン3か、希
望によってはそれ以降のバージョンのうちどれか)の定める条件の下で再頒布
または改変することができます。

このプログラムは有用であることを願って頒布されますが、*全くの無保証*
です。商業可能性の保証や特定の目的への適合性は、言外に示されたものも含
め全く存在しません。詳しくはGNU 一般公衆利用許諾契約書をご覧ください。

あなたはこのプログラムと共に、GNU 一般公衆利用許諾契約書の複製物を一部
受け取ったはずです。もし受け取っていなければ、フリーソフトウェア財団ま
で請求してください

#endif

using namespace std;
#include <vector>



/////////////
// DEBUG
#define SNAME(c) RouteUtil::StationName(c).c_str()
#define LNAME(l) RouteUtil::LineName(l).c_str()

////////////////////////////////////////////
//	static member

///*static*/ BYTE Route::jct_mask[JCTMASKSIZE];	// about 40byte


////////////////////////////////////////////
//	DbidOf
//
DbidOf::DbidOf()
{
    /**** global variables *****/
}

int32_t DbidOf::id_of_station(tstring name) {
    if (retrieve_id_pool.find(name) != retrieve_id_pool.end()) {
        return retrieve_id_pool[name];
    }
    int id = RouteUtil::GetStationId(name.c_str());
    retrieve_id_pool[name] = id;

    return id;
}

int32_t DbidOf::id_of_line(tstring name) {
    if (retrieve_id_pool.find(name) != retrieve_id_pool.end()) {
        return retrieve_id_pool[name];
    }
    int id = RouteUtil::GetLineId(name.c_str());
    retrieve_id_pool[name] = id;

    return id;
}

////////////////////////////////////////////
//	RouteFlag
//
tstring RouteFlag::showAppliedRule() const
{
	TCHAR cb[128];

	_sntprintf_s(cb, NumOf(cb), _T("rule8687=0x%02x, rule88=%d, rule69=%d, rule70=%d, specific_fare=%d, meihan=%d\n"),
					(0x3f & rule86or87),
							rule88,
							rule69,
							rule70,
							special_fare_enable,
							meihan_city_enable);
	return cb;
}

////////////////////////////////////////////
//	RouteItem
//


// constructor
//
//	@param [in] lineId_    路線
//	@param [in] stationId_ 駅
//
RouteItem::RouteItem(IDENT lineId_, IDENT stationId_)
{
	lineId = lineId_;
	stationId = stationId_;
//    salesKm = fare = 0;

//	if (lineId <= 0) {
		flag = RouteUtil::AttrOfStationId((int32_t)stationId_) & MASK_ROUTE_FLAG_SFLG;
//	} else {
//		flag = RouteUtil::AttrOfStationOnLineLine((int32_t)lineId_, (int32_t)stationId_);
//	}
}

//	constructor
//	@param [in] lineId_    路線
//	@param [in] stationId_ 駅
//	@param [in] flag_      mask(bit31-16のみ有効)
//
RouteItem::RouteItem(IDENT lineId_, IDENT stationId_, SPECIFICFLAG flag_)
{
//    salesKm = fare = 0;

	lineId = lineId_;
	stationId = stationId_;
	flag = RouteUtil::AttrOfStationId((int32_t)stationId_) & MASK_ROUTE_FLAG_SFLG;
	flag |= (flag_ & MASK_ROUTE_FLAG_LFLG);
}

void RouteItem::refresh()
{
	flag = RouteUtil::AttrOfStationId((int32_t)stationId) & MASK_ROUTE_FLAG_SFLG;
}

//////////////////////////////////////////////////////
// 	文字列は「漢字」か「かな」か？
//	「かな」ならTrueを返す
//
// 	@param [in] szStr    文字列
// 	@retval true  文字列はひらがなまたはカタカナのみである
// 	@retval false 文字列はひらがな、カタカナ以外の文字が含まれている
//
#ifndef _WINDOWS   /* UTF-8 */
/* UTF-8が文字列で使用できるのはC11(Visual C++ 2013)から
 */
bool isKanaString(LPCTSTR szStr)
{
    unsigned char lead;
    int32_t cs;
    int32_t pt;
    string str(szStr);

    for (pt = 0; pt < (int32_t)str.size(); pt += cs) {
        lead = str[pt];
        if (lead < 0x80) {
            cs = 1;
        } else if (lead < 0xe0) {
            cs = 2;
        } else if (lead < 0xf0) {
            cs = 3;
        } else {
            cs = 4;
        }
        if ((1 < cs) &&
            ((str.substr(pt, cs).compare(u8"ぁ") < 0) || (0 < str.substr(pt, cs).compare(u8"ん"))) &&
            ((str.substr(pt, cs).compare(u8"ァ") < 0) || (0 < str.substr(pt, cs).compare(u8"ン")))) {
            return false;
        }
    }
    return ! str.empty();
}

#else  /* unicode */
bool isKanaString(LPCTSTR szStr)
{
	LPCTSTR p = szStr;
	while (*p != _T('\0')) {
		if (((*p < _T('ぁ')) || (_T('ん') < *p)) && ((*p < _T('ァ')) || (_T('ン') < *p))) {
			return false;
		}
		p++;
	}
	return p != szStr; // 文字数0もダメ
}
#endif

//	カナをかなに
//
//	@param [in][out] kana_str  変換文字列
//
void conv_to_kana2hira(tstring& kana_str)
{
#ifndef _WINDOWS   /* UTF-8 */
    uint32_t lead;
    uint32_t cs;
    uint32_t pt;
    uint32_t knum;
    char c[4];

    for (pt = 0; pt < kana_str.size(); pt += cs) {
        lead = (unsigned char)kana_str[pt];
        if (lead < 0x80) {
            cs = 1;
        } else if (lead < 0xe0) {
            cs = 2;
        } else if (lead < 0xf0) {
            cs = 3;
        } else {
            cs = 4;
        }
        if ((1 < cs) &&
            ((0 <= kana_str.substr(pt, cs).compare(u8"ァ")) && (kana_str.substr(pt, cs).compare(u8"ン") <= 0))) {
            ASSERT(3 == cs);
            knum = (0xff0000 & ((unsigned)kana_str[pt] << 16)) | (0x00ff00 & ((unsigned)kana_str[pt + 1] << 8)) | (0x0000ff & ((unsigned)kana_str[pt + 2]));
            if ((0 <= kana_str.substr(pt, cs).compare(u8"ダ")) && (kana_str.substr(pt, cs).compare(u8"ミ") <= 0)) {
                knum -= ((0xff0000 & ((unsigned)u8"ダ"[0] << 16)) |
                         (0x00ff00 & ((unsigned)u8"ダ"[1] << 8)) |
                         (0xff & ((unsigned)u8"ダ"[2])));
                knum += ((0xff0000 & ((unsigned)u8"だ"[0] << 16)) |
                         (0x00ff00 & ((unsigned)u8"だ"[1] << 8)) |
                         (0xff & ((unsigned)u8"だ"[2])));
            } else {
                knum -= ((0xff0000 & ((unsigned)u8"ァ"[0] << 16)) |
                         (0x00ff00 & ((unsigned)u8"ァ"[1] << 8)) |
                         (0xff & ((unsigned)u8"ァ"[2])));
                knum += ((0xff0000 & ((unsigned)u8"ぁ"[0] << 16)) |
                         (0x00ff00 & ((unsigned)u8"ぁ"[1] << 8)) |
                         (0xff & ((unsigned)u8"ぁ"[2])));
            }
            c[0] = (knum >> 16) & 0xff;
            c[1] = (knum >> 8) & 0xff;
            c[2] = knum & 0xff;
            c[3] = (char)0;
            kana_str.replace(pt, cs, c);
        }
    }

#else   /* UNICODE */
	tstring::iterator i = kana_str.begin();
	while (i != kana_str.end()) {
		if (iskana(*i)) {
			*i = *i - _T('ァ') + _T('ぁ');
		}
		i++;
	}
#endif
}

//	3桁毎にカンマを付加した数値文字列を作成
//	(1/10した小数点付き営業キロ表示用)
//
//	@param [in] num  数値
//	@return 変換された文字列(ex. 61000 -> "6,100.0", 25793 -> "2,579.3")
//
tstring num_str_km(int32_t num)
{
	TCHAR cb[16];
	tstring s;
	TCHAR c;
	int32_t ll;

	_sntprintf_s(cb, 16, _T("%u"), num / 10);
	s = cb;
	for (ll = (int32_t)s.size() - 3; 0 < ll; ll -= 3) {
		s.insert(ll, _T(","));
	}
	c = _T('0') + (num % 10);
	s += _T('.');
	s += c;
	return s;
}

//	3桁毎にカンマを付加した数値文字列を作成
//	金額表示用
//
//	@param [in] num  数値
//	@return 変換された文字列(ex. 61000 -> "\61,000", 3982003 -> "3,982,003")
//
tstring num_str_yen(int32_t num)
{
	TCHAR cb[16];
	tstring s;
	int32_t ll;

	_sntprintf_s(cb, 16, _T("%u"), num);
	s = cb;
	for (ll = (int32_t)s.size() - 3; 0 < ll; ll -= 3) {
		s.insert(ll, _T(","));
	}
	return s;
}

//	strcat_s
//
void alp_strcat(size_t maxlen, char* dest, const char* src)
{
	size_t len;
	size_t cplen;
	char* cp;

	len = strlen(dest);
	cp = dest + len;
	cplen = strlen(src);

	if ((maxlen - len) < cplen) {
		cplen = maxlen - len - 1;
	}
	memcpy(cp, src, cplen);
	*(cp + cplen) = '\0';
}


////////////////////////////////////////////////////////////////////////
//
//	Routeクラス インプリメント
//


Route::Route()
{
	JctMaskClear(jct_mask);
}


Route::Route(const RouteList& route_list)
{
	JctMaskClear(jct_mask);
	assign(route_list);
}


Route::~Route()
{
}


// operator=(const Route& source_route)
void Route::assign(const RouteList& source_route, int32_t count /* = -1 */)
{
    RouteList::assign(source_route, count);
}


RouteList::RouteList(const RouteList& route_list)
{
    assign(route_list);
}

// operator=(const Route& source_route)
void RouteList::assign(const RouteList& source_route, int32_t count /* = -1 */)
{
    if (count < 0) {
        route_list_raw.assign(source_route.routeList().cbegin(), source_route.routeList().cend());
        route_flag = source_route.getRouteFlag();
    } else {
        vector<RouteItem>::const_iterator pos = source_route.routeList().cbegin();
        int row = 1;
        // build
        Route build_route;
        if (0 < count) {
            build_route.add(pos->stationId);
            for (pos++; pos != source_route.routeList().cend() && row < count ; pos++, row++) {
                build_route.add(pos->lineId, pos->stationId);
            }
        }
        // copy of route
        route_list_raw.assign(build_route.routeList().cbegin(),
                              build_route.routeList().cend());
        // copy of flag
        if (source_route.getRouteFlag().osakakan_detour) {
            build_route.setDetour(true);
        }
        route_flag = build_route.getRouteFlag();
    }
    /* It's necessary to rebuild() if Route object. */
}

CalcRoute::CalcRoute(const RouteList& route)
{
	sync(route, -1);
}

CalcRoute::CalcRoute(const RouteList& route, int count)
{
    sync(route, count);
}


void CalcRoute::sync(const RouteList& route)
{
	sync(route, -1);
}

void CalcRoute::sync(const RouteList& route, int count)
{
    route_list_cooked.clear();
    RouteList::assign(route, count);
    TRACE("CalcRoute::sync() %d\n", route_flag.is_osakakan_1pass());
}


// TimeSel.cpp
// ----------------------------------------------

// static
//	会社&都道府県の列挙
//
//	@return DBクエリ結果オブジェクト(会社(JRのみ)、都道府県一覧)
//
DBO RouteUtil::Enum_company_prefect()
{
	const static char tsql[] =
"select name, rowid from t_company where name like 'JR%'"
" union"
" select name, rowid*65536 from t_prefect order by rowid;";
	return DBS::getInstance()->compileSql(tsql, false);
}

// static
//	都道府県or会社に属する路線の列挙(JRのみ)
//
//	@return DBクエリ結果オブジェクト(路線)
//
DBO RouteUtil::Enum_lines_from_company_prefect(int32_t id)
{
	char sql[300];
	const char tsql[] =
"select n.name, line_id, lflg from t_line n"
" left join t_lines l on n.rowid=l.line_id"
" left join t_station t on t.rowid=l.station_id"
" where %s=%d"
" and (l.lflg&((1<<18)|(1<<31)|(1<<17)))=0"
" group by l.line_id order by n.kana";

	int32_t ident;

	if (0x10000 <= id) {
		ident = IDENT2(id);
	} else {
		ident = id;
	}
	sqlite3_snprintf(sizeof(sql), sql, tsql,
	(0x10000 <= (PAIRIDENT)id) ? "prefect_id" : "company_id", ident);

	return DBS::getInstance()->compileSql(sql, false);
}


// static
//	駅名のパターンマッチの列挙(ひらがな、カタカナ、ひらがなカタカナ混じり、漢字）
//  ひらがな昇順ソートで返す / 会社線駅も含む
//
//	@param [in] station   えきめい
//	@return DBクエリ結果オブジェクト(駅名)
//
DBO RouteUtil::Enum_station_match(LPCTSTR station)
{
#ifdef _WINDOWS
	USES_CONVERSION;
#endif
	char sql[256];
//	const char tsql[] = "/**/select name, rowid, samename from t_station where (sflg&(1<<18))=0 and %s like '%q%%'";
	const char tsql[] = "/**/select name, rowid, samename from t_station where %s like '%q%%'";
	const char tsql_s[] = " and samename='%q'";
	const char tsql_e[] = " order by kana";

	tstring sameName;
	tstring stationName(station);		// WIN32 str to C++ string

	if (MAX_STATION_CHR < stationName.length()) {
		stationName = stationName.erase(MAX_STATION_CHR);
	}

	int32_t pos = (int32_t)stationName.find('(');
	if (0 <= pos) {
		sameName = stationName.substr(pos);
		stationName = stationName.substr(0, pos);
#ifdef _WINDOWS
		CT2A qsName(stationName.c_str(), CP_UTF8);
		CT2A qsSame(sameName.c_str(), CP_UTF8);

		sqlite3_snprintf(sizeof(sql), sql, tsql,
								"name", LPSTR(qsName));
		sqlite3_snprintf(sizeof(sql) - lstrlenA(sql), sql + lstrlenA(sql), tsql_s,
								LPSTR(qsSame));
#else
		sqlite3_snprintf(sizeof(sql), sql, tsql,
								"name", stationName.c_str());
		sqlite3_snprintf((int32_t)sizeof(sql) - (int32_t)strlen(sql), sql + (int32_t)strlen(sql), tsql_s,
								sameName.c_str());
#endif
	} else {
		bool bKana;
		if (isKanaString(stationName.c_str())) {
			conv_to_kana2hira(stationName);
			bKana = true;
		} else {
			bKana = false;
		}
#ifdef _WINDOWS
		CT2A qsName(stationName.c_str(), CP_UTF8);	// C++ string to UTF-8
		sqlite3_snprintf(sizeof(sql), sql, tsql,
								bKana ? "kana" : "name", LPSTR(qsName));
#else
		sqlite3_snprintf(sizeof(sql), sql, tsql,
								bKana ? "kana" : "name", stationName.c_str());
#endif
	}
	alp_strcat(sizeof(sql), sql, tsql_e);
	return DBS::getInstance()->compileSql(sql, false);
}

// static
//	駅(Id)の都道府県を得る
//
//	@param [in] stationId   駅ident
//	@return 都道府県名
//
tstring RouteUtil::GetPrefectByStationId(int32_t stationId)
{
	static const char tsql[] =
//"select p.name from t_prefect p left join t_station t on t.prefect_id=p.rowid where t.rowid=?";
"select name from t_prefect where rowid=(select prefect_id from t_station where rowid=?)";
//ここではどーでもいいけど、sqliteは結合遅くてサブクエリの方が早かったのでこうしてみた
	DBO dbo = DBS::getInstance()->compileSql(tsql);
	dbo.setParam(1, stationId);

	if (dbo.moveNext()) {
		return dbo.getText(0);
	} else {
		return _T("");
	}
}


// static
//	会社 or 都道府県 + 路線の駅の列挙
//
//	@param [in] prefectOrCompanyId  都道府県 or 会社ID×0x10000
//	@param [in] lineId              路線
//	@return DBクエリ結果オブジェクト(都道府県、会社)
//
DBO RouteUtil::Enum_station_located_in_prefect_or_company_and_line(int32_t prefectOrCompanyId, int32_t lineId)
{
	static const  char tsql[] =
//"select t.name, station_id from t_lines l left join t_station t on t.rowid=l.station_id "
//" where line_id=? and %s=? order by sales_km";
"/**/select t.name, station_id from t_station t left join t_lines l on t.rowid=l.station_id"
" where line_id=? and %s=? and (l.lflg&((1<<18)|(1<<31)|(1<<17)))=0 order by sales_km";
// 18:Company, 17:virtual junction for Shinkansen
	char sql[256];
	int32_t ident;

	if (0x10000 <= (PAIRIDENT)prefectOrCompanyId) {
		ident = IDENT2(prefectOrCompanyId);
	} else {
		ident = prefectOrCompanyId;
	}

	sqlite3_snprintf(sizeof(sql), sql, tsql,
	(0x10000 <= prefectOrCompanyId) ? "prefect_id" : "company_id");

	DBO dbo = DBS::getInstance()->compileSql(sql);

	dbo.setParam(1, lineId);
	dbo.setParam(2, ident);

	return dbo;
}

// static
//	駅名のよみを得る
//
//	@param [in] stationId   駅ident
//	@return えきめい
//
tstring RouteUtil::GetKanaFromStationId(int32_t stationId)
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
//	駅の所属路線のイテレータを返す
//
//	@param [in] stationId   駅ident
//	@return DBクエリ結果オブジェクト(路線)
//	@return field0(text):路線名, field1(int32_t):路線id, field2(int32_t):lflg(bit31のみ)
//
DBO RouteUtil::Enum_line_of_stationId(int32_t stationId)
{
	static const  char tsql[] =
"select n.name, line_id, lflg from t_line n left join t_lines l on n.rowid=l.line_id"
//" where station_id=? and (lflg&((1<<31)|(1<<17)))=0 and sales_km>=0";
" where station_id=? and (lflg&(1<<17))=0 order by n.kana";

	DBO dbo = DBS::getInstance()->compileSql(tsql);
	dbo.setParam(1, stationId);

	return dbo;
}



// alps_mfcDlg.cpp
// ----------------------------------------------

//static
//	路線の分岐駅一覧イテレータを返す
//	@param [in] lineId	路線Id
//	@param [in] stationId 着駅Id(この駅も一覧に含める)
//	@return DBクエリ結果オブジェクト(分岐駅)
//
DBO RouteUtil::Enum_junction_of_lineId(int32_t lineId, int32_t stationId)
{
	static const  char tsql[] =
#if 1	// 実レコードではなくフラグで分岐駅判定にする（新今宮を載せたくない）
//#ifdef __OBJC__
//#else
"select t.name, station_id, lflg&(1<<15)"
" from t_lines l left join t_station t on t.rowid=l.station_id"
" where line_id=?1 and (lflg&(1<<17))=0 and (sflg&(1<<12))!=0 or (station_id=?2 and line_id=?1)"
" order by l.sales_km";
//#endif
#else
#if 1	// 分岐特例のみの乗換駅(弘前とか)を含む
"select t.name, station_id, sflg&(1<<12)"
" from t_lines l left join t_station t on t.rowid=l.station_id"
" where line_id=?1 and (lflg&(1<<17))=0 and"
" station_id in (select station_id from t_lines where line_id!=?1 or station_id=?2)"
" order by l.sales_km";

//"select t.name, station_id, sflg&(1<<12)"
//" from t_lines l left join t_station t on t.rowid=l.station_id"
//" where line_id=?1 and (lflg&(1<<17))=0 and"
//" exists (select * from t_lines where line_id!=?1 and l.station_id=station_id)"
//" or (line_id=?1 and (lflg&(1<<17))=0 and station_id=?2) order by l.sales_km";
// -- exists を使うとやたら遅い
#else
"select t.name, station_id, sflg&(1<<12)"
" from t_lines l left join t_station t on t.rowid=l.station_id"
" where line_id=?1 and ((sflg & (1<<12))<>0 or (station_id=?2 and line_id=?1))"
//" and (lflg&((1<<31)|(1<<17)))=0 and sales_km>=0"
" and (lflg&(1<<17))=0"
" order by l.sales_km";
#endif
#endif

	DBO dbo = DBS::getInstance()->compileSql(tsql);
	dbo.setParam(1, lineId);
	dbo.setParam(2, stationId);

	return dbo;
}

//static
//	路線内の駅一覧イテレータを返す
//
//	@param [in] lineId   駅ident
//	@return DBクエリ結果オブジェクト(駅)
//
DBO RouteUtil::Enum_station_of_lineId(int32_t lineId)
{
	static const  char tsql[] =
"select t.name, station_id, lflg&(1<<15)"
" from t_lines l left join t_station t on t.rowid=l.station_id"
" where line_id=?"
" and (lflg&((1<<31)|(1<<17)))=0"
" order by l.sales_km";

	DBO dbo = DBS::getInstance()->compileSql(tsql);
	dbo.setParam(1, lineId);

	return dbo;
}


//static
//	駅の隣の分岐駅を返す
//	(非分岐駅を指定すると正しい結果にならない)
//
//	@param [in] stationId   駅ident
//	@return DBクエリ結果オブジェクト(隣接分岐駅)
//
DBO RouteUtil::Enum_neer_node(int32_t stationId)
{
	static const  char tsql[] =
"select 	station_id , abs(("
"	select case when calc_km>0 then calc_km else sales_km end "
"	from t_lines "
"	where 0=(lflg&((1<<31)|(1<<17))) "
"	and line_id=(select line_id "
"				 from	t_lines "
"				 where	station_id=?1"
"				 and	0=(lflg&((1<<31)|(1<<17)))) "
"	and station_id=?1)-case when calc_km>0 then calc_km else sales_km end) as cost"
" from 	t_lines "
" where 0=(lflg&((1<<31)|(1<<17)))"
" and	line_id=(select	line_id "
" 				 from	t_lines "
" 				 where	station_id=?1"
" 				 and	0=(lflg&((1<<31)|(1<<17))))"
" and	sales_km in ((select max(y.sales_km)"
"					  from	t_lines x left join t_lines y"
"					  on	x.line_id=y.line_id "
"					  where	0<=x.sales_km and 0=(x.lflg&((1<<31)|(1<<17)))"
"					  and   0<=y.sales_km and (1<<15)=(y.lflg&((1<<31)|(1<<17)|(1<<15)))"
"					  and	x.station_id=?1"
"					  and	x.sales_km>y.sales_km"
"					 ),"
"					 (select min(y.sales_km)"
"					  from	t_lines x left join t_lines y"
"					  on	x.line_id=y.line_id "
"					  where 0<=x.sales_km and 0=(x.lflg&((1<<31)|(1<<17)))"
"					  and	0<=y.sales_km and (1<<15)=(y.lflg&((1<<31)|(1<<17)|(1<<15)))"
"					  and	x.station_id=?1"
"					  and	x.sales_km<y.sales_km))";
	DBO dbo = DBS::getInstance()->compileSql(tsql);
	dbo.setParam(1, stationId);

	return dbo;
}

//static
// 駅の隣の分岐駅数を得る
//	(盲腸線の駅か否かのみを返す)
//
//	@param [in] stationId   駅ident
//	@return 0 to 2 (隣接分岐駅数)
//
int32_t RouteUtil::NumOfNeerNode(int32_t stationId)
{
	if (STATION_IS_JUNCTION(stationId)) {
		return 2;	// 2以上あることもあるが嘘つき
	}
	DBO dbo =  RouteUtil::Enum_neer_node(stationId);
	int32_t c;

	c = 0;
	while (dbo.moveNext()) {
		++c;
	}
	return c;
}

//static
// 駅ID(分岐駅)の最寄りの分岐駅を得る(全路線）
// [jct_id, calc_km, line_id][N] = f(jct_id)
//
//	@param [in] jctId   分岐駅
//	@return 分岐駅[0]、計算キロ[1]、路線[2]
//
vector<vector<int32_t>> Route::Node_next(int32_t jctId, bool except_local)
{
	const static char tsql[] =
	"select case jct_id when ?1 then neer_id else jct_id end as node, cost, line_id, attr"
	" from t_node"
	" where jct_id=?1 or neer_id=?1 order by node";

	vector<vector<int32_t>> result;

	DBO dbo = DBS::getInstance()->compileSql(tsql);
	if (dbo.isvalid()) {
		dbo.setParam(1, jctId);

		while (dbo.moveNext()) {
			vector<int32_t> r1;

            if (!except_local || (dbo.getInt(3) != 2)) {
                r1.push_back(dbo.getInt(0));	// jct_id
    			r1.push_back(dbo.getInt(1));	// cost(calc_km)
    			r1.push_back(dbo.getInt(2));	// line_id
    			result.push_back(r1);
            }
		}
	}
	return result;
}

//static
//	路線の駅間に駅はあるか？
//	lineIdのb_stationId to e_stationId in stationId ?
//	注： lflg&(1<<17)を含めていないため、新幹線内分岐駅、たとえば、
//	     東海道新幹線 京都 米原間に草津駅は存在するとして返します.
//
//	@param [in] stationId   検索駅
//	@param [in] lineId      路線
//	@param [in] b_stationId 開始駅
//	@param [in] e_stationId 終了駅
//	@return 0: not found / not 0: ocunt of found.
//
int32_t RouteUtil::InStation(int32_t stationId, int32_t lineId, int32_t b_stationId, int32_t e_stationId)
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

	DBO dbo = DBS::getInstance()->compileSql(tsql);
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
//
// 路線内の駅1〜駅2内に、駅3〜駅4が含まれるか？重なる部分の営業キロ、計算キロを返す
// ---:駅1〜駅2, ===:駅3-駅4
// -----
//   == 　　==を返す
// --
//    == 　0を返す
// ----
//   ====  ==の距離を返す
// ----
// ====    ====の距離を返す
// @param [in] line_id     路線
// @param [in] station_id1 検査する駅1
// @param [in] station_id2 検査する駅2 (sales_kmは駅１<駅2であることつまり下り)
// @param [in] station_id3 検査する駅3
// @param [in] station_id4 検査する駅4 (上り下り関係なし)
//
//	@retuen [0]:営業キロ, [1]:計算キロ
//
vector<int32_t>  RouteUtil::getIntersectOnLine(int32_t line_id, int32_t station_id1, int32_t station_id2, int32_t station_id3, int32_t station_id4)
{
    const char tsql[] =
    "select ifnull(max(sales_km) - min(sales_km), 0), ifnull(max(calc_km) - min(calc_km), 0)"
    " from t_lines"
    " where line_id=?1 and sales_km>=(select sales_km from t_lines where line_id=?1 and station_id=?2)"
    "                  and sales_km<=(select sales_km from t_lines where line_id=?1 and station_id=?3)"
    "                  and sales_km>=(select min(sales_km)"
    "                                  from t_lines"
    "                                  where line_id=?1 and (station_id=?4 or station_id=?5))"
    "                  and sales_km<=(select max(sales_km)"
    "                                  from t_lines"
    "                                  where line_id=?1 and (station_id=?4 or station_id=?5));";
    vector<int32_t> sales_calc_km;

    int sales_km = 0;
    int calc_km = 0;

    DBO dbo = DBS::getInstance()->compileSql(tsql);
    if (dbo.isvalid()) {
        dbo.setParam(1, line_id);
        dbo.setParam(2, station_id1);
        dbo.setParam(3, station_id2);
        dbo.setParam(4, station_id3);
        dbo.setParam(5, station_id4);
        if (dbo.moveNext()) {
            sales_km = dbo.getInt(0);
            calc_km = dbo.getInt(1);
        }
    }
    sales_calc_km.push_back(sales_km);
    sales_calc_km.push_back(calc_km);

    return sales_calc_km;
}

//static
//
// 路線内の駅1〜駅2内に、駅3〜駅4が完全に含まれるか？
// @param [in] line_id     路線
// @param [in] station_id1 検査する駅1
// @param [in] station_id2 検査する駅2 (sales_kmは駅１<駅2であることつまり下り)
// @param [in] station_id3 検査する駅3
// @param [in] station_id4 検査する駅4 (上り下り関係なし)
//
//	@retuen [0]:営業キロ, [1]:計算キロ
//
bool  RouteUtil::inlineOnline(int32_t line_id, int32_t station_id1, int32_t station_id2, int32_t station_id3, int32_t station_id4)
{
    const char tsql[] =
    "select case when 2=count(*) then 1 else 0 end"
    " from t_lines"
    " where line_id=?1 and sales_km>=(select sales_km from t_lines where line_id=?1 and station_id=?2)"
    "                and sales_km<=(select sales_km from t_lines where line_id=?1 and station_id=?3)"
    "                and (station_id=?4 or station_id=?5)";
    bool rc = false;

    DBO dbo = DBS::getInstance()->compileSql(tsql);
	if (dbo.isvalid()) {
        dbo.setParam(1, line_id);
        dbo.setParam(2, station_id1);
        dbo.setParam(3, station_id2);
        dbo.setParam(4, station_id3);
        dbo.setParam(5, station_id4);
        if (dbo.moveNext()) {
            int res = dbo.getInt(0);
            if (res == 1) {
                rc = true;
            }
        }
    }
    return rc;
}

//public
// 経路マークリストコンストラクタ
//
//	@param [in]  jct_mask        分岐マーク(used check()) NULLはremoveTail()用
//	@param [in]  route_flag       制御フラグ
//	@param [in]  line_id         路線
//	@param [in]  station_id1	 発 or 至
//	@param [in]  station_id2     至 or 発
//
Route::RoutePass::RoutePass(const BYTE* jct_mask, const RouteFlag& rRoute_flag, int32_t line_id, int32_t station_id1, int32_t station_id2, int32_t start_station_id /* =0 */)
{
	JctMaskClear(_jct_mask);

	_source_jct_mask = jct_mask;
	_line_id = line_id;
	_station_id1 = station_id1;
	_station_id2 = station_id2;
	_start_station_id = start_station_id;

	_route_flag = rRoute_flag;

	_err = false;

	if (station_id1 == station_id2) {
		_num = 0;
	} else {
        if (line_id == LINE_ID(_T("大阪環状線"))) {
			_num = enum_junctions_of_line_for_osakakan();
        } else if ((line_id == LINE_ID(_T("山陽新幹線"))) &&
					rRoute_flag.notsamekokurahakatashinzai) {
			if (station_id1 == STATION_ID(_T("博多"))) {
				if (station_id2 == STATION_ID(_T("小倉"))) {
					/* 山陽新幹線 博多 -> 小倉 */
					/* 博多の西はもうなかと、小倉の東は本州で閉塞区間ばい */
					JctMaskOn(_jct_mask, Route::Id2jctId(station_id1));
					JctMaskOn(_jct_mask, Route::Id2jctId(station_id2));
					_num = 2;
				}
				else {
					/* 博多 -> 小倉、広島方面の場合 */
					JctMaskOn(_jct_mask, Route::Id2jctId(station_id1)); // 博多
					_station_id1 = STATION_ID(_T("小倉"));
					_num = 1 + enum_junctions_of_line(); // 小倉->小倉～新大阪間のどれか
					_station_id1 = station_id1; // restore(博多)
				}
			}
			else if (station_id2 == STATION_ID(_T("博多"))) {
				/* 山陽新幹線 -> 博多 */
				if (station_id1 == STATION_ID(_T("小倉"))) {
					JctMaskOn(_jct_mask, Route::Id2jctId(station_id1));
					JctMaskOn(_jct_mask, Route::Id2jctId(station_id2));
					_num = 2;
				}
				else {
					_station_id2 = STATION_ID(_T("小倉"));
					_num = enum_junctions_of_line(); //新大阪-小倉間のどれかの駅～小倉
					_station_id2 = station_id2;	// restore(博多)
					JctMaskOn(_jct_mask, Route::Id2jctId(station_id2));
					_num += 1;
				}
			}
			else {
				_num = enum_junctions_of_line();
			}
        } else if (IS_COMPANY_LINE(line_id)) {
            JctMaskOn(_jct_mask, Route::Id2jctId(station_id1));
            JctMaskOn(_jct_mask, Route::Id2jctId(station_id2));
            _num = 2;
        } else {
			_num = enum_junctions_of_line();
		}
	}
}

//	路線のbegin_station_id駅からto_station_id駅までの分岐駅リストを返す
//
//	注： lflg&(1<<17)を含めていないため、新幹線内分岐駅、たとえば、
//	     東海道新幹線 京都 米原間に草津駅は存在するとして返します.
//
//	@return 分岐点数
//
int32_t Route::RoutePass::enum_junctions_of_line()
{
	int32_t c;
	static const char tsql[] =
"select id from t_lines l join t_jct j on j.station_id=l.station_id where"
"	line_id=?1 and (lflg&((1<<31)|(1<<15)))=(1<<15)"
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

	DBO dbo = DBS::getInstance()->compileSql(tsql);
	c = 0;

	if (dbo.isvalid()) {
		dbo.setParam(1, _line_id);
		dbo.setParam(2, _station_id1);
		dbo.setParam(3, _station_id2);
		++c;
		while (dbo.moveNext()) {
			JctMaskOn(_jct_mask, dbo.getInt(0));
			c++;
		}
	} else {
		c = -1;	/* error */
	}
	return c;
}

//	路線のbegin_station_id駅からto_station_id駅までの分岐駅リストを返す(大阪環状線DB上の逆回り(今宮経由))
//
//	@return 分岐点数
//
int32_t Route::RoutePass::enum_junctions_of_line_for_oskk_rev()
{
	int32_t c;
	static const char tsql[] =
"select id from t_jct where station_id in ( "
"select station_id from t_lines where line_id=?1 and (lflg&((1<<31)|(1<<15)))=(1<<15) and sales_km<=(select min(sales_km) from t_lines where "
"line_id=?1 and (station_id=?2 or station_id=?3)) union all "
"select station_id from t_lines where line_id=?1 and (lflg&((1<<31)|(1<<15)))=(1<<15) and sales_km>=(select max(sales_km) from t_lines where "
"line_id=?1 and (station_id=?2 or station_id=?3)))";

	ASSERT(_line_id == LINE_ID(_T("大阪環状線")));

	DBO dbo = DBS::getInstance()->compileSql(tsql);
	c = 0;

	if (dbo.isvalid()) {
		dbo.setParam(1, _line_id);
		dbo.setParam(2, _station_id1);
		dbo.setParam(3, _station_id2);

		while (dbo.moveNext()) {
			JctMaskOn(_jct_mask, dbo.getInt(0));
			c++;
		}
	} else {
		c = -1;	/* error */
	}
	return c;
}


//	RoutePass Constructor: 大阪環状線の乗車経路の分岐駅リストを返す
//
//	@return 分岐点数(plus and 0 or minus)
//
int32_t Route::RoutePass::enum_junctions_of_line_for_osakakan()
{
	int32_t i;
	int32_t dir;
	int32_t jnum = -1;
	uint8_t  check_result = 0;
	RoutePass farRoutePass(*this);

	ASSERT(_line_id == LINE_ID(_T("大阪環状線")));

	if (_source_jct_mask == NULL) {
		/* removeTail() */
		if (_route_flag.is_osakakan_2pass()) {
			if (_route_flag.osakakan_2dir) {
			    jnum = enum_junctions_of_line_for_oskk_rev();
			} else {
				jnum = enum_junctions_of_line();
			}
            _route_flag.osakakan_2dir = false;
            _route_flag.setOsakaKanFlag(RouteFlag::OSAKAKAN_1PASS);

        } else if (_route_flag.is_osakakan_1pass()) {
            if (_route_flag.osakakan_1dir) {
			    jnum = enum_junctions_of_line_for_oskk_rev();
			} else {
				jnum = enum_junctions_of_line();
			}
            _route_flag.setOsakaKanFlag(RouteFlag::OSAKAKAN_NOPASS);
            _route_flag.osakakan_1dir = false;
            _route_flag.osakakan_2dir = false;
            _route_flag.osakakan_detour = false;

		} else {
			/* 一回も通っていないはあり得ない */
			TRACE("osaka-kan pass flag is not 1 or 2 (%d)\n", _route_flag.getOsakaKanPassValue());
			ASSERT(FALSE);
		}
		return jnum;
	}

	dir = RouteUtil::DirOsakaKanLine(_station_id1, _station_id2);

	if (_route_flag.is_osakakan_nopass()) {
		// 始めての大阪環状線
		//    detour near far
		// a:   1       o    o  far,1p,
		// b:   1       -    x  far,  n/a あり得へんけどそのままret
		// c:   1       x    o  far,2p
		// d:   1       -    x  far,  n/a あり得へんけどそのままret
		// e:   0       o    o  near,1p
		// f:   0       o    x  near,2p
		// g:   0       x    o  far, 2p
		// h:   0       x    x  x
		if (_route_flag.osakakan_detour) {
			// 大回り指定
			dir ^= 1;
        	TRACE("Osaka-kan: 1far\n");
		} else {
			// 通常(大回り指定なし)
        	TRACE("Osaka-kan: 1near\n");
		}

		/* DB定義上の順廻り（内回り) : 外回り */
		if ((0x01 & dir) == 0) {
			jnum = enum_junctions_of_line();
        	TRACE("Osaka-kan: 2fwd\n");
		} else {
		    jnum = enum_junctions_of_line_for_oskk_rev();
        	TRACE("Osaka-kan: 2rev\n");
		}

		// 大阪環状線内駅が始発ポイントか見るため.
		// (始発ポイントなら1回通過でなく2回通過とするため)

		check_result = (uint8_t)(check() & 0x01);
		if (((0x01 & check_result) == 0) || !_route_flag.osakakan_detour) {
			// 近回りがOK または
			// 近回りNGだが遠回り指定でない場合(大阪環状線内駅が始発ポイントである)
			// a, c, e, f, g, h
			if ((0x01 & dir) == 0) {
				// 順廻り(DB上の. 実際には逆回り)が近回り.
				// ...のときは遠回りを見てみる
			    i = farRoutePass.enum_junctions_of_line_for_oskk_rev();
        		TRACE("Osaka-kan: 3rev\n");
			} else {
				// 逆回りが近回り
				// ...のときは遠回りを見てみる
				i = farRoutePass.enum_junctions_of_line();
        		TRACE("Osaka-kan: 3fwd\n");
			}
		} else {
			// b, d
			// 遠回り指定がNG
			//ASSERT(FALSE);
			TRACE(_T("Osaka-kan Illegal detour flag.\n"));
			return jnum;
		}
		check_result |= (uint8_t)((farRoutePass.check() & 0x01) << 4);
		if ((0x11 & check_result) == 0) {
			// a, e
			/* 両方向OK */
            _route_flag.setOsakaKanFlag(RouteFlag::OSAKAKAN_1PASS);
			if ((0x01 & dir) == 0) {
				_route_flag.osakakan_1dir = false;
        		TRACE("Osaka-kan: 5fwd\n");
			} else {
				_route_flag.osakakan_1dir = true;
	        	TRACE("Osaka-kan: 5rev\n");
			}
		} else if ((0x11 & check_result) != 0x11) {
			// c, f, g
			// 近回りか遠回りのどっちかダメ
			if ((0x11 & check_result) == 0x01) {
				// 近回りがNG
				// f
				dir ^= 1;                        /* 戻す */
				jnum = i;
				update(farRoutePass);	// 遠回りを採用
										// updateは_route_flagを壊す
	        	TRACE("Osaka-kan: 6far\n");
			} else {
				// c, g
	        	TRACE("Osaka-kan: 6near\n");
			}
            _route_flag.setOsakaKanFlag(RouteFlag::OSAKAKAN_2PASS); /* 大阪環状線駅始発 */
			if ((0x01 & dir) == 0) {
	        	TRACE("Osaka-kan:7fwd\n");
				_route_flag.osakakan_1dir = false;
			} else {                                      // 計算時の経路上では1回目なので、
				_route_flag.osakakan_1dir = true;   // BLF_OSAKAKAN_2DIRでなく、BLF_OSAKAKAN_1DIR
	        	TRACE("Osaka-kan:7rev\n");
			}
		} else {
			// どっち廻りもダメ(1回目でそれはない)
			// h
			// thru
        	TRACE("Osaka-kan:8\n");
        	//ASSERT(FALSE); 大阪 東海道線 草津 草津線 柘植 関西線 今宮 大阪環状線 x鶴橋
		}
	} else if (_route_flag.is_osakakan_1pass()) {

		// 2回目の大阪環状線
		// 最初近回りで試行しダメなら遠回りで。
		for (i = 0; i < 2; i++) {

		    clear();

			if ((0x01 & dir) == 0) {
				jnum = enum_junctions_of_line();
	        	TRACE("Osaka-kan: 9fwd\n");
			} else {
			    jnum = enum_junctions_of_line_for_oskk_rev();
	        	TRACE("Osaka-kan: 9rev\n");
			}
			if (((0x01 & check()) == 0) &&
				 (STATION_IS_JUNCTION(_start_station_id) || (_start_station_id == _station_id2) ||
					(Route::InStationOnLine(LINE_ID(_T("大阪環状線")), _start_station_id) <= 0) ||
					(Route::RoutePass::InStationOnOsakaKanjyou(dir, _start_station_id, _station_id1, _station_id2) <= 0))) {
			// 開始駅が大阪環状線なら、開始駅が通過範囲内にあるか ?
			// 野田 大阪環状線 大阪 東海道線 尼崎 JR東西線 京橋 大阪環状線 福島(環)
			// の2回目の大阪環状線は近回りの内回りは大阪でひっかかるが、外回りは分岐駅のみで判断
			// すると通ってしまうので */
	        	TRACE(_T("Osaka-kan: 9(%d, %d, %d)\n"), STATION_IS_JUNCTION(_start_station_id), Route::InStationOnLine(LINE_ID(_T("大阪環状線")), _start_station_id), Route::RoutePass::InStationOnOsakaKanjyou(dir, _start_station_id, _station_id1, _station_id2));
	        	TRACE("Osaka-kan: 9ok\n");
				break; /* OK */
			} else {
				_err = true;	/* for jct_mask all zero and 0 < InStationOnOsakaKanjyou() */
			}
//TRACE(_T("%d %d"), STATION_IS_JUNCTION(_start_station_id), _start_station_id);
//TRACE(_T("Osaka-kan: @(%d, %d, %d)\n"), check(), Route::InStationOnLine(LINE_ID(_T("大阪環状線")), _start_station_id), Route::RoutePass::InStationOnOsakaKanjyou(dir, _start_station_id, _station_id1, _station_id2));
        	TRACE("Osaka-kan: 9x\n");
		    dir ^= 0x1;	/* 方向を逆にしてみる */
		} /* NG */

		if (i < 2) {
			/* OK */
            _route_flag.setOsakaKanFlag(RouteFlag::OSAKAKAN_2PASS); /* 大阪環状線駅始発 */

			if ((dir & 0x01) == 0) {
				_route_flag.osakakan_2dir = false;
	        	TRACE("Osaka-kan: 10ok_fwd\n");
			} else {
				_route_flag.osakakan_2dir = true;
	        	TRACE("Osaka-kan: 10ok_rev\n");
			}
		} else {
			/* NG */
	       	TRACE("Osaka-kan: 11x\n");
		}

	} else {
		// 既に2回通っているので無条件で通過済みエラー
        TRACE("Osaka-kan: 3time\n");
        jnum = enum_junctions_of_line();
		ASSERT((check() & 0x01) != 0);
		enum_junctions_of_line_for_oskk_rev(); // safety
		ASSERT((check() & 0x01) != 0);
	}
	TRACE("\ncheck:%d, far=%d, err=%d\n", check(), farRoutePass.check(), _err);
	return jnum;	/* Failure */
}

//public
//	分岐駅リストの指定分岐idの乗車マスクをOff
//
//	@param [in]  jid   分岐id
//
void Route::RoutePass::off(int jid)
{
	TRACE(_T("off %s\n"), Route::JctName(jid).c_str());
	JctMaskOff(_jct_mask, jid);
}

//public
//	分岐駅リストの乗車マスクをOff
//
//	@param [in]  jct_mask   分岐mask
//
void Route::RoutePass::off(BYTE* jct_mask)
{
	int32_t i;

	for (i = 0; i < JCTMASKSIZE; i++) {
		jct_mask[i] &= ~_jct_mask[i];
#if defined _DEBUG
		for (int j = 0; j < 8; j++) {
			if ((_jct_mask[i] & (1 << j)) != 0) {
				TRACE(_T("removed.  : %s\n"), Route::JctName(i * 8 + j).c_str());
			}
		}
#endif
	}
}

//public
//	分岐駅リストの乗車マスクをOn
//
//	@param [in]  jct_mask   分岐mask
//
void Route::RoutePass::on(BYTE* jct_mask)
{
	int32_t i;

	for (i = 0; i < JCTMASKSIZE; i++) {
		jct_mask[i] |= _jct_mask[i];
#if defined _DEBUG
		for (int j = 0; j <= 8; j++) {
			if (((1 << j) & _jct_mask[i]) != 0) {
				TRACE(_T("  add-mask on: %s(%d,%d)\n"), Route::JctName((i * 8) + j).c_str(), Route::Jct2id((i * 8) + j), (i * 8) + j);
			}
		}
#endif
	}
}

//public
//	分岐駅リストの乗車マスクをCheck
//
//	@param [in]  jct_mask   分岐mask
//	@retval 0 = success
//	@retval 1 = already passed.
//	@retval 3 = already passed and last arrive point passed.
//	@retval 2 = last arrive point.
//
int32_t Route::RoutePass::check(bool is_no_station_id1_first_jct /* = false */) const
{
	int32_t i;
	int32_t j;
	int32_t k;
	int32_t jctid;
	int32_t rc;

	if (_source_jct_mask == NULL) {
		ASSERT(FALSE);
		return -1;	// for removeTail()
	}
	rc = 0;
	for (i = 0; i < JCTMASKSIZE; i++) {
		k = (_source_jct_mask[i] & _jct_mask[i]);
		if (0 != k) {
		    /* 通過済みポイントあり */
			for (j = 0; j < 8; j++) {
				if ((k & (1 << j)) != 0) {
					jctid = (i * 8) + j; /* 通過済みポイント */
					if (Route::Jct2id(jctid) == _station_id2) {
						rc |= 2;   /* 終了駅 */
					} else if (is_no_station_id1_first_jct || (Route::Jct2id(jctid) != _station_id1)) {
						// 前回着駅=今回着駅は、通過済みフラグON
						rc |= 1;	/* 既に通過済み */
						TRACE(_T("  already passed error: %s(%d,%d)\n"), Route::JctName(jctid).c_str(), Route::Jct2id(jctid), jctid);
						break;
					}
				}
			}
			if ((rc & 1) != 0) {
				break;   /* 既に通過済み */
			}
		}
	}
	if (((rc & 0x01) == 0) && _err) {
		TRACE(_T("Osaka-kan Pass check error.\n"));
		rc |= 1;
	}
	return rc;
}



//static
//	大阪環状線最短廻り方向を返す
//
//	@param [in]  station_id_a   発or至
//	@param [in]  station_id_b   発or至
// 	@retval 0 = 内回り(DB定義上の順廻り)が最短
// 	@retval 1 = 外回りが最短
//
//	@note station_id_a, station_id_bは区別はなし
//
int32_t RouteUtil::DirOsakaKanLine(int32_t station_id_a, int32_t station_id_b)
{
	if (RouteUtil::GetDistance(LINE_ID(_T("大阪環状線")), station_id_a, station_id_b)[0] <=
	    RouteUtil::GetDistanceOfOsakaKanjyouRvrs(LINE_ID(_T("大阪環状線")), station_id_a, station_id_b)) {
	    return 0;
	} else {
		return 1;
	}
}

//static
//	大阪環状線最短廻り方向を返す
//
//	@param [in]  station_id_a   発or至
//	@param [in]  station_id_b   発or至
// 	@retval 0 = 内回り(DB定義上の順廻り)が最短
// 	@retval 1 = 外回りが最短
//
//	@note station_id_a, station_id_bは区別はなし
//
int32_t Route::RoutePass::InStationOnOsakaKanjyou(int32_t dir, int32_t start_station_id, int32_t station_id_a, int32_t station_id_b)
{
	int32_t n = 0;

	if ((dir & 0x01) == 0) {
		n = RouteUtil::InStation(start_station_id, LINE_ID(_T("大阪環状線")), station_id_a, station_id_b);
	} else {
		static const char tsql[] =
"select count(*) from ("
"select station_id from t_lines where line_id=?1 and (lflg&(1<<31))=0 and sales_km<=(select min(sales_km) from t_lines where "
"line_id=?1 and (station_id=?2 or station_id=?3)) union all "
"select station_id from t_lines where line_id=?1 and (lflg&(1<<31))=0 and sales_km>=(select max(sales_km) from t_lines where "
"line_id=?1 and (station_id=?2 or station_id=?3))"
") where station_id=?4";

		DBO dbo = DBS::getInstance()->compileSql(tsql);
		if (dbo.isvalid()) {
			dbo.setParam(1, LINE_ID(_T("大阪環状線")));
			dbo.setParam(2, station_id_a);
			dbo.setParam(3, station_id_b);
			dbo.setParam(4, start_station_id);

			if (dbo.moveNext()) {
				n = dbo.getInt(0);
			}
		}
	}
	return n;
}

//static
//	70条進入路線、脱出路線から進入、脱出境界駅と営業キロ、路線IDを返す
//
//	@param [in] line_id    大環状線進入／脱出路線
//	@return 一番外側の大環状線内(70条適用)駅
//
//	@note 東京都区内なので、営業キロMAX固定(下り即ち東京から一番遠い70条適用駅)とする
//
int32_t CalcRoute::RetrieveOut70Station(int32_t line_id)
{
	static const char tsql[] =
"select station_id from t_lines where line_id=?1 and "
" sales_km=(select max(sales_km) from t_lines where line_id=?1 and (lflg&(1<<31))=0 and"
" exists (select * from t_station where rowid=station_id and (sflg&(1<<6))!=0));";
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
	DBO dbo = DBS::getInstance()->compileSql(tsql);
	if (dbo.isvalid()) {
		dbo.setParam(1, line_id);
	}
	if (dbo.moveNext()) {
		return dbo.getInt(0);
	}
	return 0;
}

// 分岐特例のひとつ

bool Route::chk_jctsb_b(int32_t kind, int32_t num)
{
	switch (kind) {
	case JCTSP_B_NISHIKOKURA:
		/* 博多-新幹線-小倉*/
		return (2 <= num) &&
			(LINE_ID(_T("山陽新幹線")) == route_list_raw.at(num - 1).lineId) &&
			(STATION_ID(_T("小倉")) == route_list_raw.at(num - 1).stationId) &&
			(STATION_ID(_T("博多")) == route_list_raw.at(num - 2).stationId);
		break;
	case JCTSP_B_YOSHIZUKA:
		/* 小倉-新幹線-博多 */
        return (2 <= num) &&
             (LINE_ID(_T("山陽新幹線")) == route_list_raw.at(num - 1).lineId) &&
             (STATION_ID(_T("博多")) == route_list_raw.at(num - 1).stationId); //&&
             //(0 < RouteList::InStation(STATION_ID(_T("小倉"),
             //                      LINE_ID(_T("山陽新幹線")),
             //                      route_list_raw.at(num - 1).stationId,
             //                      route_list_raw.at(num - 2).stationId));
            //return (2 <= num) &&
            //	(LINE_ID(_T("山陽新幹線")) == route_list_raw.at(num - 1).lineId) &&
            //	(STATION_ID(_T("博多") == route_list_raw.at(num - 1).stationId) &&
            //	(STATION_ID(_T("小倉") == route_list_raw.at(num - 2).stationId);
		break;
	default:
		break;
	}
	return false;
}

/*  通過連絡運輸チェック
 *  param [in] line_id  路線id
 *
 *  @param [in] line_id  路線
 *  @param [in] 駅1
 *  @param [in] 駅2
 *  @param [in] num route_list.size()=経路数
 *  @retval 0 = continue
 *	@retval -4 = 会社線 通過連絡運輸なし
 */
int32_t Route::companyPassCheck(int32_t line_id, int32_t stationId1, int32_t stationId2, int32_t num)
{
	int rc;

    if (route_flag.compnda ||
    (IS_COMPANY_LINE(line_id) && route_flag.compnpass)) {
        return -4;      /* error x a c */
    }
    if (IS_COMPANY_LINE(line_id) &&
    (!route_flag.compncheck && !route_flag.compnpass)) {

		route_flag.compnend = true;	// if company_line

        /* pre block check d */
		route_flag.compncheck = true;
		return preCompanyPassCheck(line_id, stationId1, stationId2, num);

    } else if (!IS_COMPANY_LINE(line_id) && (route_flag.compncheck)) {

        /* after block check e f */
		route_flag.compnterm = false;	// initialize
		rc = postCompanyPassCheck(line_id, stationId1, stationId2, num);
		if (0 <= rc) {
			route_flag.compnpass = true;
			route_flag.compnend = false;	// if company_line
			if (rc == 1) {
				route_flag.tokai_shinkansen = true;
			} else if (rc == 3) {
				if (STATION_IS_JUNCTION(stationId2)) {
                    route_flag.compnterm = true;    // 
				} else {
					rc = -4;	/* 分岐駅ではない場合、”~続けて経路を指定してください"ができない*/
				}
			} else {
				rc = 0;
			}
		}
		return rc;

    } else if (IS_COMPANY_LINE(line_id)) {
		/* b */
		ASSERT(!route_flag.compnda);
		ASSERT(route_flag.compncheck);
		ASSERT(!route_flag.compnpass);
		ASSERT(route_list_raw.back().lineId != line_id);
		//通常はこれでよしだが、終端エラーで再トライ時にNGとなる：ASSERT(IS_COMPANY_LINE(route_list_raw.back().lineId));

		route_flag.compnend = true;	// if company_line

		if (route_flag.compnbegin) {
			return 0;	/* 会社線始発はとりあえず許す！ */
		}

		/* 会社線乗継可否チェック(市振、目時、妙高高原、倶利伽羅) */
		rc = CompanyConnectCheck(stationId1);
        if (rc == 0) {
            // route_list_raw.at(num - 2).stationId is 会社線
            // term1 jr_line1 term2 company_line2 term3 company_line3 term4 だから num - 2.stationIdはterm2
            return preCompanyPassCheck(line_id, route_list_raw.at(num - 2).stationId, stationId2, num);
        }
		return rc;
	} else {
        /* g h */
		route_flag.compnend = false;	// if company_line
        return 0;
    }
}

/*	会社線通過連連絡運輸テーブル取得
 *	@param [in] key1   駅１
 *	@param [in] key2   駅2
 *	@return  結果数を返す（0~N, -1 Error：レコード数がオーバー(あり得ないバグ)）
 */
int Route::CompnpassSet::open(int key1, int key2)
{
	static const char tsql[] =
"select en_line_id, en_station_id1, en_station_id2, option"
" from t_compnpass"
" where station_id1=? and station_id2=?";
	int i;
	int station_id1;
	int station_id2;

	terminal = false;
	DBO dbo = DBS::getInstance()->compileSql(tsql);
	if (dbo.isvalid()) {
		dbo.setParam(1, key1);
		dbo.setParam(2, key2);

		for (i = 0; dbo.moveNext(); i++) {
			if (max_record <= i) {
				ASSERT(FALSE);
				return -1;		/* too many record */
			}
			results[i].line_id = dbo.getInt(0);
			station_id1 = dbo.getInt(1);
			station_id2 = dbo.getInt(2);
			if (!terminal) {
				terminal = 0 < dbo.getInt(3);
			}
			results[i].stationId1 = station_id1;
			results[i].stationId2 = station_id2;
		}
		num_of_record = i;
		return i;	/* num of receord */
	} else {
		return -1;	/* db error */
	}
}

/*!
 *	@brief 通過連絡運輸チェック
 *
 *  @param [in] postcheck_flag plus is postCompnayPasscheck, minus is preCompanyPasscheck and leave, otherwise zero
 *	@param [in] line_id      add(),追加予定路線
 *	@param [in] station_id1  add(),最後に追加した駅
 *	@param [in] station_id2  add(),追加予定駅
 *	@retval 0 : エラーなし(継続)
 *	@retval 2 : 許可駅発着駅OK
 *	@retval 3 : 着駅有効(pending)
 *	@retval -4 : 通過連絡運輸禁止
 */
int Route::CompnpassSet::check(int32_t postcheck_flag, int32_t line_id, int32_t station_id1, int32_t station_id2)
{
	int i;
	int rc = -4;
	int terminal_id;

	if (num_of_record <= 0) {
		return 0;
	}
	if (0 < postcheck_flag) {
		// post
		terminal_id = station_id2;
	} else if (postcheck_flag < 0) {
		// pre
		terminal_id = station_id1;
	} else {
		// pre and throgh
		terminal_id = 0;
	}
	for (i = 0; i < num_of_record; i++) {
		if ((terminal_id != 0) && terminal
					&& (results[i].stationId1 != 0)) {
			ASSERT(terminal_id != 0 && postcheck_flag != 0);
		    if (0 < RouteUtil::InStation(terminal_id, results[i].line_id, results[i].stationId1, results[i].stationId2)) {
				TRACE("Company check OK(terminal) %s %s\n", 0 < postcheck_flag ? "arrive":"leave", SNAME(terminal_id));
				TRACE(_T("    (%d/%d %s,%s-%s def= %s:%s-%s)\n"), i, num_of_record,
														LNAME(line_id), SNAME(station_id1), SNAME(station_id2),
														LNAME(results[i].line_id), SNAME(results[i].stationId1), SNAME(results[i].stationId2));
				return 2; 	// OK possible to pass
			} else {
				TRACE(_T("Check company terminal notfound(%s)(%s:%s-%s) in %s:%s-%s)\n"), SNAME(terminal_id), LNAME(line_id), SNAME(station_id1), SNAME(station_id2), LNAME(results[i].line_id), SNAME(results[i].stationId1), SNAME(results[i].stationId2));
			}
		}
		if ((results[i].line_id & 0x80000000) != 0) {
			/* company */
			int32_t company_mask = results[i].line_id;
			int32_t cid1 = RouteUtil::CompanyIdFromStation(station_id1);
			int32_t cid2 = RouteUtil::CompanyIdFromStation(station_id2);
            int32_t cid11 = IDENT1(cid1) & 0x7f;
            int32_t cid12 = IDENT2(cid1) & 0x7f;
            int32_t cid21 = IDENT1(cid2) & 0x7f;
            int32_t cid22 = IDENT2(cid2) & 0x7f;

			if ((0 != (company_mask & (1 << cid11 | 1 << cid12)))
             && (0 != (company_mask & (1 << cid21 | 1 << cid22)))) {
				return 0;	/* OK possible pass */
			}
			/* JR東海なら新幹線を許す */
			if (((company_mask & ((1 << JR_CENTRAL) | (1 << JR_EAST))) == (1 << JR_CENTRAL))
			&& (line_id == LINE_ID(_T("東海道新幹線")))) {
				// 都区市内適用しないように (CheckOfRule86())
				// b#20090901 tokai_shinkansen = true;
				TRACE("JR-tolai Company line\n");
				return 1;	/* OK possible pass */
			}
		} else if (results[i].line_id == line_id) {
			TRACE(_T("Company check begin(%d/%d %s,%s-%s def= %s:%s-%s)\n"), i, num_of_record,
						LNAME(line_id), SNAME(station_id1), SNAME(station_id2),
						LNAME(results[i].line_id), SNAME(results[i].stationId1), SNAME(results[i].stationId2));
			if ((results[i].stationId1 == 0) || (
				(0 < RouteUtil::InStation(station_id1, line_id, results[i].stationId1, results[i].stationId2)) &&
			    (0 < RouteUtil::InStation(station_id2, line_id, results[i].stationId1, results[i].stationId2)))) {
				TRACE(_T("Company check OK(%s,%s in %s:%s-%s)\n"), SNAME(station_id1), SNAME(station_id2), LNAME(line_id), SNAME(results[i].stationId1), SNAME(results[i].stationId2));
				return 0;	/* OK possible to pass */
			}
		} else if (results[i].line_id == 0) {
			TRACE(_T("Company check NG(%s,%s-%s = %s:%s-%s)\n"), LNAME(line_id), SNAME(station_id1), SNAME(station_id2), LNAME(results[i].line_id), SNAME(results[i].stationId1), SNAME(results[i].stationId2));
			break;	/* can't possoble */
		}
	}
	if (terminal) {
		TRACE(_T("Company check pending to terminal\n"));
		return 3;	/* 会社線契約着駅まで保留 */
	}
	return rc;
}


/*!
 * static
 *	前段チェック 通過連絡運輸
 *	@param [in] station_id2  add(),追加予定駅
 *	@retval 0 : エラーなし(継続)
 *	@retval -4 : 通過連絡運輸禁止
 */
int32_t Route::CompanyConnectCheck(int32_t station_id)
{
	static const char tsql[] =
	"select pass from t_compnconc where station_id=?";
	DBO dbo = DBS::getInstance()->compileSql(tsql);
	int r = 0;	/* if disallow */

	if (dbo.isvalid()) {
		dbo.setParam(1, station_id);
		if (dbo.moveNext()) {
			r = dbo.getInt(0);
		}
	}
    TRACE(_T("CompanyConnectCheck: %s(%d)\n"), SNAME(station_id), r);
	return r == 0 ? -4 : 0;
}


/*!
 *	前段チェック 通過連絡運輸
 *	@param [in] line_id      add(),追加予定路線
 *	@param [in] station_id1  add(),最後に追加した駅
 *	@param [in] station_id2  add(),追加予定駅
 *  @param [in] num          route_list.size()=経路数
 *	@retval 0 : エラーなし(継続)
 *	@retval -4 : 通過連絡運輸禁止
 */
int32_t Route::preCompanyPassCheck(int32_t line_id, int32_t station_id1, int32_t station_id2, int32_t num)
{
	CompnpassSet cs;
	int i;
	int rc;

	ASSERT(IS_COMPANY_LINE(line_id));
	TRACE(_T("Enter preCompanyPassCheck(%s, %s %s %d)\n"), LNAME(line_id), SNAME(station_id1), SNAME(station_id2), num);
    TRACE(_T("  key1=%s, key2=%s\n"), SNAME(station_id1), SNAME(station_id2));

	if (num <= 1) {
		/* 会社線で始まる */
		route_flag.compnbegin = true;
        TRACE("preCompanyPassCheck begin company line\n");
		return 0;
	}
	rc = cs.open(station_id1, station_id2);
	if (rc <= 0) {
        TRACE("preCompanyPassCheck not found db record(pass)\n");
		return 0;		/* Error or Non-record(always pass) as continue */
	}
    rc = 0;
	for (i = 1; i < num; i++) {
        if (IS_COMPANY_LINE(route_list_raw.at(i).lineId)) {
            continue;
        }
		rc = cs.check((i == 1) ? -1 : 0, route_list_raw.at(i).lineId,
					  route_list_raw.at(i - 1).stationId,
					  route_list_raw.at(i).stationId);
        TRACE(_T("preCompanyPassCheck %d/%d->%d(%s:%s-%s)\n"), i, num, rc, 
		        LNAME(route_list_raw.at(i).lineId), SNAME(route_list_raw.at(i - 1).stationId), SNAME(route_list_raw.at(i).stationId));
		if (rc < 0) {
			break;	/* disallow */
		}
		if (rc == 1) {
			route_flag.tokai_shinkansen = true;
			rc = 0;
		} else if (rc == 3) {
			rc = -4;	// disallow pending through pre.
		} else if (rc == 2) {
			break;	// OK
		} else {
			ASSERT(rc == 0);
		}
	}
	if (0 <= rc) {
        TRACE("preCompanyPassCheck always pass(%d)\n", rc);
		return 0;		/* Error or Non-record(always pass) as continue */
	} else {
        TRACE("preCompanyPassCheck will fail\n");
		route_flag.compnda = true; /* 通過連絡運輸不正 */
		return 0;	/* -4 受け入れて（追加して）から弾く */
	}
}

/*	@brief 後段 通過連絡運輸チェック
 *	@param [in] line_id      add(),追加予定路線
 *	@param [in] station_id1  add(),最後に追加した駅
 *	@param [in] station_id2  add(),追加予定駅
 *	@retval 0 : エラーなし(継続)
 *	@retval 1 : エラーなし(継続)(JR東海新幹線)
 *	@retval 3 : エラー保留（発着駅指定）
 *	@retval -4 : 通過連絡運輸禁止
 */
int32_t Route::postCompanyPassCheck(int32_t line_id, int32_t station_id1, int32_t station_id2, int32_t num)
{
	CompnpassSet cs;
	int rc;
	int i;
	int32_t key1 = 0;
	int32_t key2 = 0;

	TRACE(_T("Enter postCompanyPassCheck(%s, %s %s %d)\n"), LNAME(line_id), SNAME(station_id1), SNAME(station_id2), num);

	do {
// 1st後段チェック	ASSERT(IS_COMPANY_LINE(route_list_raw.back().lineId));
		ASSERT(!IS_COMPANY_LINE(line_id));

		for (i = num - 1; 0 < i; i--) {
			if ((key1 == 0) && IS_COMPANY_LINE(route_list_raw.at(i).lineId)) {
				key1 = route_list_raw.at(i).stationId;
			} else if ((key1 != 0) && !IS_COMPANY_LINE(route_list_raw.at(i).lineId)) {
				key2 = route_list_raw.at(i).stationId;
				break;
			}
		}
		TRACE(_T("  key1=%s, key2=%s\n"), SNAME(key1), SNAME(key2));
		if (i <= 0) {
			route_flag.compnda = true; /* 通過連絡運輸不正 */
			if (route_flag.compnbegin) {
				TRACE("postCompanyPassCheck begin company line\n");
				rc = 0;	/* 会社線始発なら終了 */
				break; // return 0
			}
			else {
				ASSERT(FALSE);
				rc = -4;
				break;	// return -4
			}
		}
		rc = cs.open(key1, key2);
		if (rc <= 0) {
			TRACE("postCompanyPassCheck not found in db(pass)\n");
			rc = 0;		/* Error or Non-record(always pass) as continue */
			break;		// return 0
		}
		rc = cs.check(1, line_id, station_id1, station_id2);
	} while (false);
    TRACE("Leave postCompanyPassCheck(%d)\n", rc);
	return rc;	/* 0 / -4 */
}

int32_t Route::brtPassCheck(int32_t stationId2)
{
//    if (stationId2 == STATION_ID(_T("柳津"))) {
//        return -1;
//    }
    return 0;
}


/*	経路追加
 *
 *	@param [in] line_id      路線ident
 *	@param [in] stationId1   駅1 ident
 *	@param [in] stationId2   駅2 ident
 *	@param [in] ctlflg       デフォルト0(All Off)
 *							 bit8: 新幹線乗換チェックしない
 *
 *  @retval 0 = OK(last)         ADDRC_LAST
 *  @retval 1 = OK               ADDRC_OK
 *  @retval 4 = OK(last-company) ADDRC_CEND
 *  //@retval 2 = OK(re-route)
 *  @retval 5 = already finished ADDRC_END)
 *  @retval -1 = overpass(復乗エラー)
 *  @retval -2 = 経路エラー(stationId1 or stationId2はline_id内にない)
 *  @retval -3 = operation error(開始駅未設定)
 *	@retval -4 = 会社線 通過連絡運輸なし
 *  @retval -100 DB error
 *	@retval route_flag bit4-0 : reserve
 *	@retval route_flag bit5=1 : 次にremoveTailでlastItemの通過マスクをOffする(typeOでもPでもないので)
 *	@retval route_flag bit5=0 : 次にremoveTailでlastItemの通過マスクをOffする(typeOでもPでもないので)
 *	@retval route_flag bit6=1 : 分岐特例区間指定による経路変更あり
 */
#define ADD_BULLET_NC	(1<<8)
int32_t Route::add(int32_t stationId)
{
	removeAll(true);
	route_list_raw.push_back(RouteItem(0, stationId));
	TRACE(_T("add-begin %s(%d)\n"), SNAME(stationId), stationId);
	return 1;
}

int32_t Route::add(int32_t line_id, int32_t stationId2, int32_t ctlflg)
{
	int32_t rc;
	int32_t i;
	int32_t j;
	int32_t num;
	int32_t stationId1;
	SPECIFICFLAG lflg1;
	SPECIFICFLAG lflg2;
	int32_t start_station_id;
	bool replace_flg = false;	// 経路追加ではなく置換
	SPECIFICFLAG jct_flg_on = 0;   // 水平型検知(D-2) / BSRNOTYET_NA
	int32_t type = 0;
	int32_t is_no_station_id1_first_jct = 0;
	JCTSP_DATA jctspdt;
#ifdef _DEBUG
	int32_t original_line_id = line_id;
#endif
	int32_t first_station_id1;

	if (route_flag.end) {
        if (route_flag.compnda) {
            TRACE(_T("route.add(): Can't add for disallow company line.\n"));
            return -4;          // not allow company connect.
        } else {
		    TRACE(_T("route.add(): already terminated.\n"));
		    return	ADDRC_END;		/* already terminated. */
        }
	}

    route_flag.trackmarkctl = false;
    route_flag.jctsp_route_change = false;

	num = (int32_t)route_list_raw.size();
	if (num <= 0) {
		ASSERT(FALSE);	// bug. must be call to add(station);
		return -3;
	}
	start_station_id = route_list_raw.front().stationId;
	stationId1 = route_list_raw.back().stationId;
	
	first_station_id1 = stationId1;

	/* 発駅 */
	lflg1 = RouteUtil::AttrOfStationOnLineLine(line_id, stationId1);
	if (BIT_CHK(lflg1, BSRNOTYET_NA)) {
		ASSERT(FALSE);	// normally don't come here.
		return -2;		/* 不正経路(line_idにstationId1は存在しない) */
	}

	/* 着駅が発駅～最初の分岐駅間にあるか? */
	/* (着駅未指定, 発駅=着駅は除く) */
#if 0 /* Dec.2016: Remove teminal */
	if ((num == 1) && (0 < end_station_id) && (end_station_id != start_station_id) &&
	(end_station_id != stationId2) &&
	(0 != RouteList::InStation(end_station_id, line_id, stationId1, stationId2))) {
		return -1;	/* <t> already passed error  */
	}
#endif

	ASSERT(BIT_CHK(lflg1, BSRJCTHORD) || route_list_raw.at(num - 1).stationId == stationId1);

	lflg2 = RouteUtil::AttrOfStationOnLineLine(line_id, stationId2);
	if (BIT_CHK(lflg2, BSRNOTYET_NA)) {
		return -2;		/* 不正経路(line_idにstationId2は存在しない) */
	}

	// 直前同一路線指定は受け付けない
	// 直前同一駅は受け付けない
	//if (!BIT_CHK(route_list_raw.back().flag, BSRJCTHORD) && ((1 < num) && (line_id == route_list_raw.back().lineId))) {
	//	TRACE(_T("iregal parameter by same line_id.\n"));
	//	return -1;		// E-2, G, G-3, f, j1,j2,j3,j4 >>>>>>>>>>>>>>>>>>
	//}
	if (stationId1 == stationId2) {
		TRACE(_T("iregal parameter by same station_id.\n"));
		TRACE(_T("add_abort\n"));
		return -1;		// E-112 >>>>>>>>>>>>>>>>>>
	}

	// 同一路線で折り返した場合
	if ((BRTMASK(route_list_raw.at(num - 1).lineId) == BRTMASK(line_id)) && (2 <= num) &&
	(RouteUtil::DirLine(route_list_raw.at(num - 1).lineId, route_list_raw.at(num - 2).stationId, stationId1) !=
	 RouteUtil::DirLine(line_id, stationId1, stationId2))) {
		TRACE(_T("re-route error.\n"));
		TRACE(_T("add_abort\n"));
		return -1;		//  >>>>>>>>>>>>>>>>>>
	}

	rc = companyPassCheck(line_id, stationId1, stationId2, num);
	if (rc < 0) {
		return rc;	/* 通過連絡運輸なし >>>>>>>>>>>> */
	}

	// 分岐特例B(BSRJCTSP_B)水平型検知
	if (BIT_CHK(lflg2, BSRJCTSP_B) && chk_jctsb_b((type = getBsrjctSpType(line_id, stationId2)), num)) {
		TRACE("JCT: h_(B)detect\n");
		BIT_ON(jct_flg_on, BSRNOTYET_NA);	/* 不完全経路フラグ */
	} else {
		/* 新幹線在来線同一視区間の重複経路チェック(lastItemのflagがBSRJCTHORD=ONがD-2ケースである */
        int32_t shinzairev = 0; // 新幹線在来線別線区間でも折返は重複駅までではなくその直前まで(OやPは認めれない。Uまで)
        BIT_OFF(jct_flg_on, BSRSHINZAIREV);
        if ((0 == (ctlflg & ADD_BULLET_NC)) 
		   && (1 < num)
           && (0 != (shinzairev = Route::CheckTransferShinkansen(route_list_raw.at(num - 1).lineId, line_id,
                                    route_list_raw.at(num - 2).stationId, stationId1, stationId2)))) {
            if ((0 < shinzairev) && checkPassStation(shinzairev)) {
				// 在来線戻り
	            TRACE(_T("assume detect shinkansen-zairaisen too return.%s,%s %s %s\n"), LNAME(route_list_raw.at(num - 1).lineId), LNAME(line_id), SNAME(route_list_raw.at(num - 1).stationId), SNAME(stationId2));
				int32_t local_line;
				int32_t bullet_line;
				if (IS_SHINKANSEN_LINE(route_list_raw.at(num - 1).lineId)) {
					local_line = line_id;
					bullet_line = route_list_raw.at(num - 1).lineId;
				} else {
					local_line = route_list_raw.at(num - 1).lineId;
					bullet_line = line_id;
				}
				if ((((RouteUtil::AttrOfStationOnLineLine(local_line, stationId2) >> BSRSHINKTRSALW) & 0x03) != 0) &&
					(0 < Route::InStationOnLine(bullet_line, stationId2, true))) {
		            TRACE(_T("      disable\n"));
	                BIT_ON(jct_flg_on, BSRSHINZAIREV); // この後着駅が終端だったらエラー
				}
				// 第16条の2-2 "〜の各駅を除く。）"
				// return -1;	// 高崎~長岡 上越新幹線 新潟 "信越線(直江津-新潟)" 長岡 (北長岡までなら良い)
				// 大宮 上越新幹線 長岡 信越線(直江津-新潟) 直江津
				// 岡山 山陽新幹線 新大阪 東海道線 大阪 福知山線 谷川
				// では、許されるので、保留にする
            } 
			if ((shinzairev < 0) && !BIT_CHK2(route_list_raw.at(num - 1).flag, BSRJCTHORD, BSRJCTSP_B)) {
				/* 上の2条件で、直江津 信越線(直江津-新潟) 長岡 上越新幹線 大宮 は除外する */
				TRACE("JCT: F-3b\n");
				return -1;		// F-3b
			}
        }
	}
	TRACE(_T("add %s(%d)-%s(%d), %s(%d)\n"), LNAME(line_id), line_id, SNAME(stationId1), stationId1, SNAME(stationId2), stationId2);

	if (BIT_CHK(route_list_raw.at(num - 1).flag, BSRJCTSP_B)) {
		 /* 信越線上り(宮内・直江津方面) ? (フラグけちってるので
		  * t_jctspcl.type Route::RetrieveJunctionSpecific()で吉塚、小倉廻りと区別しなければならない) */
		if ((RouteUtil::LDIR_DESC == RouteUtil::DirLine(line_id, route_list_raw.at(num - 1).stationId, stationId2)) &&
		    ((num < 2) || ((2 <= num) &&
		    (RouteUtil::LDIR_ASC  == RouteUtil::DirLine(route_list_raw.at(num - 1).lineId,
		                                 route_list_raw.at(num - 2).stationId,
		                                 route_list_raw.at(num - 1).stationId)))) &&
			(JCTSP_B_NAGAOKA == Route::RetrieveJunctionSpecific(route_list_raw.at(num - 1).lineId,
			                                             route_list_raw.at(num - 1).stationId, &jctspdt))) {
			if (stationId2 == jctspdt.jctSpStationId2) { /* 宮内止まり？ */
				TRACE("JSBH004\n");
				TRACE("add_abort\n");
				return -1;
			} else {
					// 長岡 → 浦佐
				// 新幹線 長岡-浦佐をOff
				routePassOff(route_list_raw.at(num - 1).lineId,
				             route_list_raw.at(num - 1).stationId,
				             jctspdt.jctSpStationId);
				route_list_raw.at(num - 1) = RouteItem(route_list_raw.at(num - 1).lineId,
				                                       jctspdt.jctSpStationId);
					// 上越線-宮内追加
				rc = add(jctspdt.jctSpMainLineId, jctspdt.jctSpStationId2, ADD_BULLET_NC);		//****************
				route_flag.jctsp_route_change = true;	/* route modified */
				if (rc != ADDRC_OK) {
					TRACE(_T("junction special (JSBS001) error.\n"));
					TRACE(_T("add_abort\n"));
					return rc;			// >>>>>>>>>>>>>>>>>>>>>
				}
				num++;
				stationId1 = jctspdt.jctSpStationId2; // 宮内
				// line_id : 信越線
				// stationId2 : 宮内～長岡
			}
		} else if ((2 <= num) && (LINE_ID(_T("山陽新幹線")) == line_id)) { /* b#14021205 add */
			JctMaskOff(jct_mask, Route::Id2jctId(route_list_raw.at(num - 2).stationId));
			TRACE("b#14021205-1\n");
		}
	} else if ((2 <= num) && BIT_CHK(lflg2, BSRJCTSP_B) &&	/* b#14021205 add */
               (LINE_ID(_T("山陽新幹線")) == route_list_raw.at(num - 1).lineId) &&
               (RouteUtil::DirLine(route_list_raw.at(num - 1).lineId,
                               route_list_raw.at(num - 2).stationId,
                               route_list_raw.at(num - 1).stationId)
                != RouteUtil::DirLine(line_id, route_list_raw.at(num - 1).stationId, stationId2))) {
		JctMaskOff(jct_mask, Route::Id2jctId(route_list_raw.at(num - 1).stationId));
		TRACE("b#14021205-2\n");
		BIT_ON(jct_flg_on, BSRNOTYET_NA);	/* 不完全経路フラグ */
	}

	// 水平型検知
	if (BIT_CHK(route_list_raw.at(num - 1).flag, BSRJCTHORD)) {
		TRACE("JCT: h_detect 2 (J, B, D)\n");
		if ((STATION_ID(_T("博多")) != stationId1) &&
		Route::IsAbreastShinkansen(route_list_raw.at(num - 1).lineId, line_id, stationId1, stationId2)) {
			// 	line_idは新幹線
			//	route_list_raw.at(num - 1).lineIdは並行在来線
			//
			ASSERT(IS_SHINKANSEN_LINE(line_id));
			if (0 != RouteUtil::InStation(route_list_raw.at(num - 2).stationId, line_id, stationId1, stationId2)) {
				TRACE("JCT: D-2\n");
				j = RouteUtil::NextShinkansenTransferTermInRange(line_id, stationId1, stationId2);
				if (j <= 0) {	// 隣駅がない場合
#ifdef _DEBUG
                    ASSERT(original_line_id == line_id);
#endif
					i = route_list_raw.at(num - 1).lineId;	// 並行在来線
					// 新幹線の発駅には並行在来線(路線b)に所属しているか?
					if (0 == Route::InStationOnLine(i, stationId2)) {
						TRACE(_T("prev station is not found in shinkansen.\n"));
						TRACE(_T("add_abort\n"));	// 恵那-名古屋-東京方面で、
													// 名古屋-三河安城間に在来線にない駅が存在し、
													// その駅が着駅(stationId2)の場合
													// ありえない
						return -1;			// >>>>>>>>>>>>>>>>>>>
					} else {
						TRACE("JCT: hor.(D-2x)\n");
						removeTail();
						num--;
						stationId1 = route_list_raw.back().stationId;
						line_id = i;
					}
				} else {
					TRACE("JCT: hor.1(D-2)\n");
					i = route_list_raw.at(num - 1).lineId;	// 並行在来線
					if (Route::InStationOnLine(i, j) <= 0) {
						TRACE(_T("junction special (JSBX001) error.\n"));
						TRACE(_T("add_abort\n"));
						return -1;
					}
					removeTail();
					rc = add(i, j, ADD_BULLET_NC);		//****************
					ASSERT(rc == ADDRC_OK);
					stationId1 = j;
				}
				route_flag.jctsp_route_change = true;	/* route modified */
			} else {
				TRACE("JCT: B-2\n");
			}
		} else {
			TRACE("JCT: J\n");
		}
		TRACE(">\n");
	}

	// 151 check
	while (BIT_CHK(lflg1, BSRJCTSP)) {
		TRACE(">\n");
		// 段差型
		if (BIT_CHK(lflg2, BSRJCTSP)) {	// 水平型でもある?
			// retrieve from a, d to b, c
#ifdef _DEBUG
ASSERT(original_line_id == line_id);
#endif
			type = Route::RetrieveJunctionSpecific(line_id, stationId2, &jctspdt); // update jctSpMainLineId(b), jctSpStation(c)
			ASSERT(0 < type);
			TRACE("JCT: detect step-horiz:%u\n", type);
			if (jctspdt.jctSpStationId2 != 0) {
				BIT_OFF(lflg1, BSRJCTSP);				// 別に要らないけど
				break;
			}
		}
#ifdef _DEBUG
ASSERT(original_line_id == line_id);
#endif
ASSERT(first_station_id1 == stationId1);
		// retrieve from a, d to b, c
		type = Route::RetrieveJunctionSpecific(line_id, stationId1, &jctspdt); // update jctSpMainLineId(b), jctSpStation(c)
		ASSERT(0 < type);
		TRACE("JCT: detect step:%u\n", type);
		if (stationId2 != jctspdt.jctSpStationId) {
			if (route_list_raw.at(num - 1).lineId == jctspdt.jctSpMainLineId) {
				ASSERT(stationId1 == route_list_raw.at(num - 1).stationId);
				if (0 < RouteUtil::InStation(jctspdt.jctSpStationId,
										 route_list_raw.at(num - 1).lineId,
										 route_list_raw.at(num - 2).stationId,
										 stationId1)) {
					TRACE("JCT: C-1\n");
					routePassOff(jctspdt.jctSpMainLineId, jctspdt.jctSpStationId, stationId1);	// C-1
				} else { // A-1
					TRACE("JCT: A-1\n");
					is_no_station_id1_first_jct = 1;
				}
				if ((2 <= num) && (jctspdt.jctSpStationId == route_list_raw.at(num - 2).stationId)) {
					removeTail();
					TRACE(_T("JCT: A-C\n"));		// 3, 4, 8, 9, g,h
					--num;
				} else {
					TRACE("JCT: b#21072801D\n");
					route_list_raw.at(num - 1) = RouteItem(route_list_raw.at(num - 1).lineId,
														   jctspdt.jctSpStationId);
					is_no_station_id1_first_jct++;
				}
				if (jctspdt.jctSpStationId2 != 0) {		// 分岐特例路線2
					TRACE("JCT: step_(2)detect\n");
					rc = add(jctspdt.jctSpMainLineId2, jctspdt.jctSpStationId2, ADD_BULLET_NC);	//**************
					ASSERT(rc == ADDRC_OK);
					num++;
					if (rc != ADDRC_OK) {			// safety
						route_flag.jctsp_route_change = true;	/* route modified */
						TRACE(_T("A-1/C-1(special junction 2)\n"));
						TRACE(_T("add_abort\n"));
						return -1;
					}
					if (stationId2 == jctspdt.jctSpStationId2) {
						TRACE(_T("KF1,2)\n"));
						line_id = jctspdt.jctSpMainLineId2;
						replace_flg = true;
					}
					stationId1 = jctspdt.jctSpStationId2;
				} else {
					if ((is_no_station_id1_first_jct == 2) && !STATION_IS_JUNCTION_F(lflg1)) {
                        TRACE(_T("is_no_station_id1_first_jct is on: is_no_station_id1_first_jct=%d, is_junction %d, %s <- %s\n"), is_no_station_id1_first_jct, STATION_IS_JUNCTION_F(lflg1), SNAME(stationId1), SNAME(jctspdt.jctSpStationId));
						is_no_station_id1_first_jct = 555;
					}
					stationId1 = jctspdt.jctSpStationId;
				}
			} else {
                ASSERT(first_station_id1 == stationId1);

                if ((num < 2) ||
				!Route::IsAbreastShinkansen(jctspdt.jctSpMainLineId,
											route_list_raw.at(num - 1).lineId,
											stationId1,
											route_list_raw.at(num - 2).stationId)
				|| (jctspdt.jctSpStationId == STATION_ID(_T("西小倉"))) // KC-2
				|| (jctspdt.jctSpStationId == STATION_ID(_T("吉塚"))) // KC-2
				|| (RouteUtil::InStation(jctspdt.jctSpStationId,
									 route_list_raw.at(num - 1).lineId,
									 route_list_raw.at(num - 2).stationId,
									 stationId1) <= 0)) {
					// A-0, I, A-2
					TRACE("JCT: A-0, I, A-2\n");	//***************

					if ((jctspdt.jctSpStationId == STATION_ID(_T("西小倉"))) // KC-2
					   || (jctspdt.jctSpStationId == STATION_ID(_T("吉塚")))) { // KC-2
						routePassOff(jctspdt.jctSpMainLineId,
									 stationId1, jctspdt.jctSpStationId);
						TRACE("JCT: KC-2\n");
					}
					rc = add(jctspdt.jctSpMainLineId,
							 /*route_list_raw.at(num - 1).stationId,*/ jctspdt.jctSpStationId,
							 ADD_BULLET_NC);
					ASSERT(rc == ADDRC_OK);
					num++;
					if (rc != ADDRC_OK) {				// safety
						route_flag.jctsp_route_change = true;	/* route modified */
						TRACE(_T("A-0, I, A-2\n"));
						TRACE(_T("add_abort\n"));
						return -1;					//>>>>>>>>>>>>>>>>>>>>>>>>>>
					}
					if (jctspdt.jctSpStationId2 != 0) {		// 分岐特例路線2
						rc = add(jctspdt.jctSpMainLineId2, jctspdt.jctSpStationId2, ADD_BULLET_NC);	//**************
						num++;
						ASSERT(rc == ADDRC_OK);
						if (rc != ADDRC_OK) {			// safety
							route_flag.jctsp_route_change = true;	/* route modified */
							TRACE(_T("A-0, I, A-2(special junction 2)\n"));
							TRACE(_T("add_abort\n"));
							return -1;				//>>>>>>>>>>>>>>>>>>>>>>>>>>
						}
						if (stationId2 == jctspdt.jctSpStationId2) {
							TRACE(_T("KF0,3,4)\n"));
							line_id = jctspdt.jctSpMainLineId2;
							replace_flg = true;
						}
						stationId1 = jctspdt.jctSpStationId2;
					} else {
						stationId1 = jctspdt.jctSpStationId;
					}
				} else {
					ASSERT(first_station_id1 == stationId1);
					// C-2
					TRACE("JCT: C-2\n");
					ASSERT(IS_SHINKANSEN_LINE(route_list_raw.at(num - 1).lineId));
					routePassOff(jctspdt.jctSpMainLineId,
								 jctspdt.jctSpStationId, stationId1);
					i = RouteUtil::NextShinkansenTransferTermInRange(route_list_raw.at(num - 1).lineId, stationId1, route_list_raw.at(num - 2).stationId);
					if (i <= 0) {	// 隣駅がない場合
						TRACE("JCT: C-2(none next station on bullet line)\n");
						// 新幹線の発駅には並行在来線(路線b)に所属しているか?
						if (0 == Route::InStationOnLine(jctspdt.jctSpMainLineId,
														route_list_raw.at(num - 2).stationId)) {
							route_flag.jctsp_route_change = true;	/* route modified */
							TRACE(_T("next station is not found in shinkansen.\n"));
							TRACE(_T("add_abort\n"));
							return -1;			// >>>>>>>>>>>>>>>>>>>
						} else {
							removeTail();
							rc = add(jctspdt.jctSpMainLineId,
									 jctspdt.jctSpStationId, ADD_BULLET_NC);	//**************
							ASSERT(rc == ADDRC_OK);
							stationId1 = jctspdt.jctSpStationId;
						}
					} else {
						route_list_raw.at(num - 1) = RouteItem(route_list_raw.at(num - 1).lineId, i);
						route_list_raw.push_back(RouteItem(jctspdt.jctSpMainLineId,
														   jctspdt.jctSpStationId));
						stationId1 = jctspdt.jctSpStationId;
					}
				}
			}
			route_flag.jctsp_route_change = true;	/* route modified */
		} else {
			// E, G		(stationId2 == jctspdt.jctSpStationId)
			TRACE("JCT: E, G\n");
			if (jctspdt.jctSpStationId2 != 0) {
				TRACE("JCT: KE0-4\n");
				BIT_OFF(lflg2, BSRJCTSP);
			}
			line_id = jctspdt.jctSpMainLineId;
			ASSERT(first_station_id1 == stationId1);

			if ((2 <= num) &&
//			!BIT_CHK(RouteUtil::AttrOfStationOnLineLine(line_id, stationId2), BSRJCTSP_B) &&
			(0 < RouteUtil::InStation(stationId2, jctspdt.jctSpMainLineId,
			route_list_raw.at(num - 2).stationId, stationId1))) {
				TRACE(_T("E-3:duplicate route error.\n"));
				TRACE(_T("add_abort\n"));
				return -1;	// Duplicate route error >>>>>>>>>>>>>>>>>
			}
			if (BIT_CHK(RouteUtil::AttrOfStationOnLineLine(line_id, stationId2), BSRJCTSP_B)) {
				// 博多-小倉-西小倉
				//
				TRACE("jct-b nisi-kokura-stop/yoshizuka-stop\n");
			}
			if (route_list_raw.at(num - 1).lineId == jctspdt.jctSpMainLineId) {
				// E-3 , B-0, 5, 6, b, c, d, e
				// E-0, E-1, E-1a, 6, b, c, d, e
				TRACE("JCT: E-3, B0,5,6,b,c,d,e, E-0,E-1,E-1a,6,b,c,d,e\n");
				replace_flg = true;
			} else {
				// E-2, G, G-3, G-2, G-4
				TRACE("JCT: E-2, G, G-3, G-2, G-4\n");
			}
			route_flag.jctsp_route_change = true;	/* route modified */
		}
		break;
	}
	if (BIT_CHK(lflg2, BSRJCTSP)) {
		// 水平型
			// a(line_id), d(stationId2) -> b(jctSpMainLineId), c(jctSpStationId)
#ifdef _DEBUG
ASSERT(original_line_id == line_id);
//ASSERT(first_station_id1 == stationId2);
#endif
		type = Route::RetrieveJunctionSpecific(line_id, stationId2, &jctspdt);
		ASSERT(0 < type);
		TRACE("JCT:%u\n", type);
		if (stationId1 == jctspdt.jctSpStationId) {
			// E10-, F, H
			TRACE("JCT: E10-, F, H/KI0-4\n");
			line_id = jctspdt.jctSpMainLineId;	// a -> b
			if (route_list_raw.at(num - 1).lineId == jctspdt.jctSpMainLineId) {
				if ((2 <= num) &&
				(0 < RouteUtil::InStation(stationId2, jctspdt.jctSpMainLineId,
				route_list_raw.at(num - 2).stationId, stationId1))) {
					TRACE(_T("E11:duplicate route error.\n"));
					TRACE(_T("add_abort\n"));
					return -1;	// Duplicate route error >>>>>>>>>>>>>>>>>
				}
				replace_flg = true;
				TRACE("JCT: F1, H, E11-14\n");
			} else {
				// F-3bはエラーとするため. BIT_ON(jct_flg_on, BSRJCTHORD);	// F-3b
			}
		} else {
			// J, B, D
			if ((jctspdt.jctSpStationId2 != 0) && (stationId1 == jctspdt.jctSpStationId2)) {	// 分岐特例路線2
				TRACE("JCT: KJ0-4(J, B, D)\n");
				rc = add(jctspdt.jctSpMainLineId2, jctspdt.jctSpStationId, ADD_BULLET_NC);		//**************
				num++;
				if (rc != ADDRC_OK) {
					route_flag.jctsp_route_change = true;	/* route modified */
					TRACE(_T("junction special (KJ) error.\n"));
					TRACE(_T("add_abort\n"));
					return rc;			// >>>>>>>>>>>>>>>>>>>>>
				}
			} else {
				if (jctspdt.jctSpStationId2 != 0) {	// 分岐特例路線2
					TRACE(_T("JCT: KH0-4(J, B, D) add(日田彦山線, 城野c')\n"));
					rc = add(line_id, /*stationId1,*/ jctspdt.jctSpStationId2, ADD_BULLET_NC);	//**************
					num++;
					if (rc == ADDRC_OK) {
						TRACE(_T("JCT: add(日豊線b', 西小倉c)\n"));
						rc = add(jctspdt.jctSpMainLineId2, jctspdt.jctSpStationId, ADD_BULLET_NC);	//**************
						num++;
					}
					if (rc != ADDRC_OK) {
						route_flag.jctsp_route_change = true;	/* route modified */
						TRACE(_T("junction special horizen type convert error.\n"));
						TRACE(_T("add_abort\n"));
						return rc;			// >>>>>>>>>>>>>>>>>>>>>
					}
				} else {
					TRACE("JCT: J, B, D\n");
					rc = add(line_id, /*stationId1,*/ jctspdt.jctSpStationId, ADD_BULLET_NC);	//**************
					num++;
					if (rc != ADDRC_OK) {
						route_flag.jctsp_route_change = true;	/* route modified */
						TRACE(_T("junction special horizen type convert error.\n"));
						TRACE(_T("add_abort\n"));
						return rc;			// >>>>>>>>>>>>>>>>>>>>>
					}
					if (stationId2 != STATION_ID(_T("小倉"))) {
						// b#15032701
						BIT_ON(jct_flg_on, BSRJCTHORD);	//b#14021202
					}
				}
			}
			// b#14021202 BIT_ON(jct_flg_on, BSRJCTHORD);
			line_id = jctspdt.jctSpMainLineId;
			stationId1 = jctspdt.jctSpStationId;
		}
		route_flag.jctsp_route_change = true;	/* route modified */
		is_no_station_id1_first_jct = 0;
	}

	// 長岡周りの段差型
	if ((2 <= num) && BIT_CHK(lflg1, BSRJCTSP_B)) {
		is_no_station_id1_first_jct = 0;
		if (JCTSP_B_NAGAOKA == Route::RetrieveJunctionSpecific(line_id,
		                                                route_list_raw.at(num - 1).stationId, &jctspdt)) {
		 	/* 信越線下り(直江津→長岡方面) && 新幹線|上越線上り(長岡-大宮方面)? */
			if ((RouteUtil::LDIR_ASC == RouteUtil::DirLine(route_list_raw.at(num - 1).lineId,
		                                    route_list_raw.at(num - 2).stationId,
		                                    route_list_raw.at(num - 1).stationId)) &&
			    (RouteUtil::LDIR_DESC == RouteUtil::DirLine(line_id,
		                                    route_list_raw.at(num - 1).stationId,
		                                    stationId2))) {
				/* 宮内発 */
				if (route_list_raw.at(num - 2).stationId == jctspdt.jctSpStationId2) {
					TRACE(_T("junction special 2(JSBS004) error.\n"));
					TRACE(_T("add_abort\n"));
					return -1;			// >>>>>>>>>>>>>>>>>>>>>
				}
				// 長岡Off
				JctMaskOff(jct_mask, Route::Id2jctId(route_list_raw.at(num - 1).stationId));

				// 長岡->宮内へ置換
				route_list_raw.at(num - 1) = RouteItem(route_list_raw.at(num - 1).lineId,
				                                       jctspdt.jctSpStationId2);

                // 上越線 宮内→浦佐
				rc = add(jctspdt.jctSpMainLineId, jctspdt.jctSpStationId, ADD_BULLET_NC);		//****************
				route_flag.jctsp_route_change = true;	/* route modified */
				if (ADDRC_OK != rc) {
					TRACE(_T("junction special 2(JSBH001) error.\n"));
					TRACE(_T("add_abort\n"));
					return rc;			// >>>>>>>>>>>>>>>>>>>>>
				}
				stationId1 = jctspdt.jctSpStationId;
				num += 1;
        	}
	    }
	}

	RoutePass route_pass(jct_mask, route_flag, line_id, stationId1, stationId2, start_station_id);
	if (route_pass.num_of_junction() < 0) {
		TRACE(_T("DB error(add-junction-enum)\n"));
		TRACE(_T("add_abort\n"));
		ASSERT(FALSE);
		return -100;		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	}

	// Route passed check
	rc = route_pass.check(is_no_station_id1_first_jct == 555);

	if (line_id == LINE_ID(_T("大阪環状線"))) {
		if ((rc & 0x01) != 0) {
			rc = -1;
		} else if ((rc & 0x02) != 0) {
			/* 着駅終了*/
			if (STATION_IS_JUNCTION_F(lflg2)) {
				rc = 1;		/* BLF_TRACKMARKCTL をONにして、次のremoveTail()で削除しない */
TRACE(_T("osaka-kan last point junction\n"));
			} else {
TRACE(_T("osaka-kan last point not junction\n"));
				rc = 2;
			}
		} else if ((rc & 0x03) == 0) {
			if ((2 <= route_list_raw.size()) && (route_list_raw.at(1).lineId == line_id) &&
			   !STATION_IS_JUNCTION(start_station_id) && (start_station_id != stationId2) &&
				(0 != RouteUtil::InStation(stationId2, line_id, start_station_id, route_list_raw.at(1).stationId))) {
TRACE(_T("osaka-kan passed error\n"));	// 要るか？2015-2-15
				rc = -1;	/* error */
				rc = 0;
			} else if (start_station_id == stationId2) {
				rc = 1;
			} else {
				rc = 0;	/* OK */
			}
		} else {
			ASSERT(FALSE);	// rc & 0x03 = 0x03
			rc = -1;		// safety
		}
	} else if (rc == 0) {	// 復乗なし
		if (((2 <= route_list_raw.size()) && (start_station_id != stationId2) &&
			(0 != RouteUtil::InStation(start_station_id, line_id, stationId1, stationId2)))
			/* =Dec.2016:Remove terminal= ||
			(((0 < end_station_id) && (end_station_id != stationId2) && (2 <= route_list_raw.size())) &&
			(0 != RouteList::InStation(end_station_id, line_id, stationId1, stationId2)))*/) {
			rc = -1;	/* <v> <p> <l> <w> */
			TRACE("y p l w\n");
		} else if (start_station_id == stationId2) {
			rc = 2;		/* <f> */
			TRACE("<f>\n");
		} else {
			rc = 0;		/* <a> <d> <g> */
			TRACE("<a> <d> <g>\n");
		}
	} else {	// 復乗
		if ((rc & 1) == 0) { /* last */
#if 0
//よくわかんないが、着駅指定UIだった頃の残骸？不要と思われるので消してみる。test_exec は、OK
			if (
		//	(!STATION_IS_JUNCTION(stationId2)) ||
		// sflg.12は特例乗り換え駅もONなのでlflg.15にした
		(!STATION_IS_JUNCTION_F(lflg2)) ||
		((2 <= num) && (start_station_id != stationId2) &&
		 (0 != RouteUtil::InStation(start_station_id, line_id, stationId1, stationId2)))
		 /* =Dec.2016:Reomve terminal=  ||
		(((0 < end_station_id) && (end_station_id != stationId2) && (2 <= num)) &&
		(0 != RouteList::InStation(end_station_id, line_id, stationId1, stationId2)))
		*/
			// ||
			// /*BIT_CHK(lflg1, BSRJCTSP) || */ BIT_CHK(lflg2, BSRJCTSP) ||	// E-12
			// BIT_CHK(route_list_raw.back().flag, BSRJCTHORD) ||				// E-14
			// (0 < junctionStationExistsInRoute(stationId2)) ||
		//	((2 <= num) && (0 < RouteList::InStation(stationId2, line_id, route_list_raw.at(num - 2).stationId, stationId1)))
			) {
				rc = -1;	/* <k> <e> <r> <x> <u> <m> */
				TRACE(_T("%x %d %d %d<k> <e> <r> <x> <u> <m> %s in %s: between %s and %s\n"), 
				lflg2, STATION_IS_JUNCTION_F(lflg2), start_station_id,stationId2,
				SNAME(start_station_id), LNAME(line_id), SNAME(stationId1), SNAME(stationId2));
			} else  {
#endif
				rc = 1;		/* <b> <j> <h> */
				TRACE("<b> <j> <h>\n");
#if 0
			}
#endif
		} else {
			rc = -1;	/* 既に通過済み */
		}
	}

	if (rc < 0) {
		// 不正ルートなのでmaskを反映しないで破棄する

		if (is_no_station_id1_first_jct == 555) {
			// b#21072801D
			// don't necessary   stationId1 = first_station_id1;
			route_list_raw.at(num - 1).stationId = first_station_id1;
			TRACE(_T("Detect finish. %d\n"), first_station_id1);
		}
		TRACE(_T("add_abort(%d)\n"), rc);
		route_flag.trackmarkctl = false;
		// E-12, 6, b, c, d, e
		return -1;	/* already passed error >>>>>>>>>>>>>>>>>>>> */

	} else {
		// normality
		// 通過bit ON(maskをマージ)
		route_pass.on(jct_mask);

		// 大阪環状線通過フラグを設定
		route_pass.update_flag(&route_flag); /* update route_flag LF_OSAKAKAN_MASK */
	}

	if ((stationId1 == STATION_ID(_T("吉塚"))) && (stationId2 == STATION_ID(_T("博多"))) && (rc == 1)) {
		// 上りで、博多南 博多南線 博多 鹿児島線 原田 筑豊線 桂川\(九\) 篠栗線 吉塚 鹿児島線 博多 山陽新幹線 広島
		TRACE(_T("43-2 博多pre-Off\n"));
		JctMaskOff(jct_mask, Route::Id2jctId(stationId2));
		rc = 0;
	}

	if (type == JCTSP_B_YOSHIZUKA) {
		// 広島 山陽新幹線 博多 鹿児島線 吉塚 篠栗線 桂川\(九\) 筑豊線  原田 鹿児島線 博多 博多南線 博多南
		TRACE(_T("43-2 博多Off\n"));
		ASSERT(route_list_raw.at(num - 1).stationId == stationId1);
		JctMaskOff(jct_mask, Route::Id2jctId(route_list_raw.at(num - 1).stationId));
	}

	/* 追加か置換か */
	if (replace_flg) {
		ASSERT(0 < type);	// enable jctspdt
		ASSERT((line_id == jctspdt.jctSpMainLineId) || (line_id == jctspdt.jctSpMainLineId2));
		ASSERT((route_list_raw.at(num - 1).lineId == jctspdt.jctSpMainLineId) ||
			   (route_list_raw.at(num - 1).lineId == jctspdt.jctSpMainLineId2));
		route_list_raw.pop_back();
		--num;
	}
	lflg1 = RouteUtil::AttrOfStationOnLineLine(line_id, stationId1);
	lflg2 = RouteUtil::AttrOfStationOnLineLine(line_id, stationId2);

	lflg2 |= (lflg1 & 0xff000000);
	lflg2 &= (0xff00ffff & ~(1<<BSRJCTHORD));	// 水平型検知(D-2);

	lflg2 |= jct_flg_on;	// BSRNOTYET_NA:BSRJCTHORD
	route_list_raw.push_back(RouteItem(line_id, stationId2, lflg2));
	++num;

	if (rc == 0) {
		TRACE(_T("added continue.\n"));
		route_flag.trackmarkctl = false;
	} else if (rc == 1) {
		route_flag.trackmarkctl = true;
	} else if (rc == 2) {
		route_flag.trackmarkctl = false;	/* 次にremoveTailでlastItemの通過マスクをOffする(typeOでもPでもないので) */
	} else {
		ASSERT(FALSE);
		route_flag.trackmarkctl = true;
	}

	TRACE(_T("added fin.(%d).\n"), rc);

	if (rc != 0) {
		if (BIT_CHK(lflg2, BSRNOTYET_NA)) {
			TRACE(_T("？？？西小倉・吉塚.rc=%d\n"), rc);
			return ADDRC_OK;	/* 西小倉、吉塚 */
        } else if (BIT_CHK(lflg2, BSRSHINZAIREV)) {
            TRACE(_T("detect shinkansen-zairaisen too return.arrive %s replace to %s\n"), SNAME(route_list_raw.at(num - 1).stationId), SNAME(route_list_raw.at(num - 2).stationId));
            removeTail();
            return -1;  /* route is duplicate */
        } else {
			route_flag.end = true;
			TRACE(_T("detect finish.\n"));
			return ADDRC_LAST;
		}
	} else {
		if (route_flag.compnda) {
			route_flag.end = true;
			return ADDRC_CEND;
		} else {
            rc = brtPassCheck(stationId2);
            if (rc < 0) {
                route_flag.end = true;
    			TRACE(_T("detect BRT finish.\n"));
                return ADDRC_LAST;
            }
			return ADDRC_OK;	/* OK - Can you continue */
		}
	}
} // end of add()


//	経路の末尾を除去
//
//	@param [in] begin_off    最終ノードの始点もOffするか(デフォルト:Offしない)
//
void Route::removeTail(bool begin_off/* = false*/)
{
	int32_t line_id;
	int32_t begin_station_id;
	int32_t to_station_id;
	int32_t i;
	int32_t list_num;

	TRACE(_T("Enter removeTail\n"));

	list_num = (int32_t)route_list_raw.size();
	if (list_num < 2) {
		route_flag.trackmarkctl = false;
		return;
	}

	/* 発駅～最初の乗換駅時 */
	if (list_num <= 2) {
		removeAll(false);
		return;
	}

	line_id = route_list_raw[list_num - 1].lineId;

	to_station_id = route_list_raw[list_num - 1].stationId;	// tail
	begin_station_id = route_list_raw[list_num - 2].stationId;	// tail - 1

	RoutePass route_pass(NULL, route_flag, line_id, to_station_id, begin_station_id);

	if (begin_off) {
		/* 開始駅はOffしない(前路線の着駅なので) */
		i = Route::Id2jctId(begin_station_id);
		if (0 < i) {
			route_pass.off(i);
			TRACE(_T("removeTail-begin %s\n"), SNAME(begin_station_id));
		}
	}

	i = Route::Id2jctId(to_station_id);
	if ((0 < i) && route_flag.trackmarkctl) {
		/* 最近分岐駅でO型経路、P型経路の着駅の場合は除外 */
		route_pass.off(i);
		TRACE(_T("removeTail %s\n"), SNAME(to_station_id));
	}

#if defined _DEBUG
	BYTE tmp[JCTMASKSIZE];
	memcpy(tmp, jct_mask, JCTMASKSIZE);
#endif
	route_pass.off(jct_mask);
#if defined _DEBUG
    for (i = 0; i < JCTMASKSIZE; i++) {
        if (tmp[i] != jct_mask[i]) {
            for (int j = 0; j < 8; j++) {
                if (((1 << j) & tmp[i]) != ((1 << j) & jct_mask[i])) {
                    if (((1 << j) & jct_mask[i]) != 0) {
						ASSERT(FALSE);
                    } else {
						TRACE(_T("   off: %s\n"), JctName(i * 8 + j).c_str());
                    }
                }
            }
        }
    }
#endif

	route_pass.update_flag(&route_flag); /* update route_flag LF_OSAKAKAN_MASK */
    route_flag.end = false;
    route_flag.trackmarkctl = false;
	if (IS_COMPANY_LINE(route_list_raw.back().lineId)) {
		if (!IS_COMPANY_LINE(route_list_raw.at(list_num - 2).lineId)) {
            route_flag.compncheck = false;
		}
	}

	route_list_raw.pop_back();

	/* 後半リストチェック */
	if (IS_COMPANY_LINE(route_list_raw.back().lineId)) {
		route_flag.compnend = true;
		route_flag.compnpass = false;
	} else {
		route_flag.compnend = false;
	}
	route_flag.compnda = false;
}

//private:
//	大阪環状線経路変更による経路再構成
//
//	@return 0 success(is last)
//	@retval 1 success
//	@retval otherwise failued
//
int32_t Route::reBuild()
{
	Route routeWork;
	int32_t rc = 0;
	vector<RouteItem>::const_iterator pos;

	if (route_list_raw.size() <= 1) {
		return 0;
	}

	pos = route_list_raw.cbegin();
	routeWork.add(pos->stationId);

	// add() の開始駅追加時removeAll()が呼ばれroute_flagがリセットされるため)
    routeWork.route_flag.osakakan_detour = route_flag.osakakan_detour;
    routeWork.route_flag.notsamekokurahakatashinzai = route_flag.notsamekokurahakatashinzai;

	for (++pos; pos != route_list_raw.cend(); pos++) {
		rc = routeWork.add(pos->lineId, pos->stationId);
		if (rc != ADDRC_OK) {
			/* error */
			break;
		}
	}
    if (rc == ADDRC_LAST) {
        ++pos;
    }
	if ((rc < 0) || ((rc != ADDRC_OK) && ((rc == ADDRC_LAST) && (pos != route_list_raw.cend())))) {
        route_flag.osakakan_detour = false;
        TRACE(_T("Can't reBuild() rc=%d¥n"), rc);
		return -1;	/* error */
	}

    // 経路自体は変わらない。フラグのみ
	// 特例適用、発着駅を単駅指定フラグは保持(大阪環状線廻りは既に持っている)
    route_flag = routeWork.route_flag;
    memcpy(jct_mask, routeWork.jct_mask, sizeof(jct_mask));

    return rc;
}


//public:
//	運賃表示
//	@return 運賃、営業キロ情報 表示文字列
//
tstring FARE_INFO::showFare(const RouteFlag& refRouteFlag)
{
#define MAX_BUF	1024
	TCHAR cb[MAX_BUF];
	tstring sResult;
	tstring sWork;

    switch (this->resultCode()) {
    case 0:     // success, company begin/first or too many company
        break;  // OK
    case -1:    /* In completed */
        return tstring(_T("この経路の片道乗車券は購入できません.続けて経路を指定してください."));	//"));
        break;
    case -2:     // empty. BRF_ROUTE_EMPTY Dont yet calcRoute()
        return tstring(_T(""));
        break;
    default:
        return tstring(_T("予期せぬエラーです"));
        break;
    }

    /* 発駅→着駅 */
    sResult = CalcRoute::BeginOrEndStationName(this->getBeginTerminalId());
	sResult += _T(" -> ");
    sResult += CalcRoute::BeginOrEndStationName(this->getEndTerminalId());

	sResult += _T("\r\n経由：");
	sResult += this->getRoute_string();
	sResult += _T("\r\n");

    ASSERT(100<=this->getFareForDisplay());

    const static TCHAR msgPossibleLowcost[] = _T("近郊区間内ですので最短経路の運賃で利用可能です(途中下車不可、有効日数当日限り)\r\n");
    const static TCHAR msgAppliedLowcost[] = _T("近郊区間内ですので最安運賃の経路にしました(途中下車不可、有効日数当日限り)\r\n");

    if (!refRouteFlag.no_rule && !refRouteFlag.osakakan_detour &&
        this->isUrbanArea() && !refRouteFlag.isUseBullet() 
		&& !refRouteFlag.isIncludeCompanyLine()) {
        if (this->getBeginTerminalId() == this->getEndTerminalId()) {
            //_sntprintf_s(cb, MAX_BUF,
            //    _T("近郊区間内ですので同一駅発着のきっぷは購入できません.\r\n"));
			memset(cb, 0, sizeof(cb));
        } else if (refRouteFlag.isEnableRule115() && refRouteFlag.isRule115specificTerm()) {
            // 115の都区市内発着指定Optionは最安最短じゃあないので.
			memset(cb, 0, sizeof(cb));
        } else {
            _sntprintf_s(cb, MAX_BUF,
                refRouteFlag.isLongRoute() ?
                msgPossibleLowcost : msgAppliedLowcost);
        }
        sResult += cb;

        // 大回り指定では115適用はみない
        if (refRouteFlag.isEnableRule115() && !refRouteFlag.isEnableLongRoute()) {
            _sntprintf_s(cb, MAX_BUF,
                refRouteFlag.isRule115specificTerm() ?
                _T("「単駅最短適用」で単駅発着が選択可能です\r\n") :
                _T("「都区内発着適用」で特定都区市内発着が選択可能です\r\n"));
            sResult += cb;
        }
    }

	if ((0 < this->getJRCalcKm()) &&
		(this->getJRSalesKm() != this->getJRCalcKm())) {
		_sntprintf_s(cb, MAX_BUF,
				(this->getCompanySalesKm() <= 0) ?
				_T("営業キロ： %s km     計算キロ： %s km\r\n") :
				_T("営業キロ： %s km JR線計算キロ： %s km\r\n"),
				num_str_km(this->getTotalSalesKm()).c_str(),
				num_str_km(this->getJRCalcKm()).c_str());
	} else {
		_sntprintf_s(cb, MAX_BUF,
				_T("営業キロ： %s km\r\n"),
				num_str_km(this->getTotalSalesKm()).c_str());
	}
	sResult += cb;

	/* 会社線(通過連絡運輸)あり */
	if (this->getJRSalesKm() != this->getTotalSalesKm()) {
		ASSERT(0 != this->getCompanySalesKm());
        if (this->getBRTSalesKm() != 0) {
            _sntprintf_s(cb, MAX_BUF,
    				_T("(JR線営業キロ： %-6s km うちBRT： %-6s km   会社線営業キロ： %s km)\r\n"),
    				num_str_km(this->getJRSalesKm()).c_str(),
                    num_str_km(this->getBRTSalesKm()).c_str(),
                    num_str_km(this->getCompanySalesKm()).c_str());
        } else {
            _sntprintf_s(cb, MAX_BUF,
    				_T("(JR線営業キロ： %-6s km   会社線営業キロ： %s km)\r\n"),
    				num_str_km(this->getJRSalesKm()).c_str(),
    				num_str_km(this->getCompanySalesKm()).c_str());
        }
		sResult += cb;
	} else if (this->getBRTSalesKm() != 0) {
            /* 会社線なし BRTあり */
            _sntprintf_s(cb, MAX_BUF,
    				_T("(うちBRT営業キロ： %s km)\r\n"),
                    num_str_km(this->getBRTSalesKm()).c_str());
    		sResult += cb;
            ASSERT(0 == this->getCompanySalesKm());
    } else {
		ASSERT(0 == this->getCompanySalesKm());
	}

	if (0 < this->getSalesKmForHokkaido()) {
		if ((0 < this->getCalcKmForHokkaido()) &&
		    (this->getCalcKmForHokkaido() != this->getSalesKmForHokkaido())) {
			_sntprintf_s(cb, MAX_BUF, _T("JR北海道営業キロ： %-6s km 計算キロ： %s km\r\n"),
						num_str_km(this->getSalesKmForHokkaido()).c_str(),
						num_str_km(this->getCalcKmForHokkaido()).c_str());
		} else {
			_sntprintf_s(cb, MAX_BUF, _T("JR北海道営業キロ： %-6s km\r\n"),
						num_str_km(this->getSalesKmForHokkaido()).c_str());
		}
		sResult += cb;
	}
	if (0 < this->getSalesKmForShikoku()) {
		if ((0 < this->getCalcKmForShikoku()) &&
		    (this->getSalesKmForShikoku() != this->getCalcKmForShikoku())) {
			_sntprintf_s(cb, MAX_BUF, _T("JR四国営業キロ： %-6s km 計算キロ： %s km\r\n"),
							num_str_km(this->getSalesKmForShikoku()).c_str(),
							num_str_km(this->getCalcKmForShikoku()).c_str());
		} else {
			_sntprintf_s(cb, MAX_BUF, _T("JR四国営業キロ： %-6s km \r\n"),
							num_str_km(this->getSalesKmForShikoku()).c_str());
		}
		sResult += cb;
	}
	if (0 < this->getSalesKmForKyusyu()) {
		if ((0 < this->getCalcKmForKyusyu()) &&
		    (this->getSalesKmForKyusyu() != this->getCalcKmForKyusyu())) {
			_sntprintf_s(cb, MAX_BUF, _T("JR九州営業キロ： %-6s km  計算キロ： %-6s km\r\n"),
							num_str_km(this->getSalesKmForKyusyu()).c_str(),
							num_str_km(this->getCalcKmForKyusyu()).c_str());
		} else {
			_sntprintf_s(cb, MAX_BUF, _T("JR九州営業キロ： %-6s km \r\n"),
							num_str_km(this->getSalesKmForKyusyu()).c_str());
		}
		sResult += cb;
	}

#if 0
運賃(IC)          ： \123,456(\123,456)  小児： \123,456(\123,456) 学割： \123,456
往復(IC)|(割)     ： \123,456(\123,456)         \123,456(\123,456)        \123,456
JR東日本 株主優待2： \123,456
JR東日本 株主優待4： \123,456
#endif

#if 0
運賃(IC): \123,456(\123,45)   往復: \123,45(\123,45)
運賃: \123,456   往復: \123,45 (割)
運賃: \123,456   往復: \123,45
#endif

	int32_t company_fare = this->getFareForCompanyline();
	int32_t normal_fare = this->getFareForDisplay();
	FARE_INFO::FareResult fareW = this->roundTripFareWithCompanyLine();
	int32_t fare_ic = this->getFareForIC();

	if ((0 < company_fare) && (0 == this->getFareForBRT())) {
		_sntprintf_s(cb, MAX_BUF,
					_T("  (うち会社線： ¥%s)"), num_str_yen(company_fare).c_str());
		sWork = cb;
	} else if ((0 < company_fare) && (0 != this->getFareForBRT())) {
        _sntprintf_s(cb, MAX_BUF,
					_T("  (うち会社線： ¥%s, BRT線： ¥%s)"), num_str_yen(company_fare).c_str(),
                                                            num_str_yen(this->getFareForBRT()).c_str());
		sWork = cb;
	} else if ((0 == company_fare) && (0 != this->getFareForBRT())) {
        _sntprintf_s(cb, MAX_BUF,
					_T("  (うちBRT線： ¥%s)"), num_str_yen(this->getFareForBRT()).c_str());
		sWork = cb;
	} else {
        sWork = _T("");
	}
	if (this->isRule114()) {
		_sntprintf_s(cb, MAX_BUF, _T("規程114条適用 営業キロ： %s km 計算キロ： %s km\r\n"),
					 num_str_km(this->getRule114SalesKm()).c_str(),
					 num_str_km(this->getRule114CalcKm()).c_str());
		sResult += cb;
	}
	if (fare_ic == 0) {
		if (!refRouteFlag.isRoundTrip()) {
			_sntprintf_s(cb, MAX_BUF, _T("運賃： ¥%-7s%s\r\n"),
		                              num_str_yen(normal_fare).c_str(),
		                              sWork.c_str());
		} else {
			_sntprintf_s(cb, MAX_BUF, _T("運賃： ¥%-7s%s     往復： ¥%-5s"),
		                              num_str_yen(normal_fare).c_str(),
		                              sWork.c_str(),
		                              num_str_yen(fareW.fare).c_str());
			if (fareW.isDiscount) {
				_tcscat_s(cb, NumOf(cb), _T("(割)\r\n"));
			} else {
				_tcscat_s(cb, NumOf(cb), _T("\r\n"));
			}
		}
		if (this->isRule114()) {
			sResult += cb;
			if (!refRouteFlag.isRoundTrip()) {
				_sntprintf_s(cb, MAX_BUF, _T("規程114条 適用前運賃： {¥%-5s}\r\n"),
			                              num_str_yen(this->getFareForDisplayPriorRule114()).c_str());
			} else {
				_sntprintf_s(cb, MAX_BUF, _T("規程114条 適用前運賃： {¥%-5s} 往復： {¥%-5s}\r\n"),
			                              num_str_yen(this->getFareForDisplayPriorRule114()).c_str(),
			                              num_str_yen(this->roundTripFareWithCompanyLinePriorRule114()).c_str());
			}
		}
	} else {
        //ASSERT(!fareW.isDiscount);
		ASSERT(company_fare == 0);
		//ASSERT(normal_fare  *  2 == this->roundTripFareWithCompanyLine().fare);
		if (!refRouteFlag.isRoundTrip()) {
			_sntprintf_s(cb, MAX_BUF, _T("運賃(IC)： ¥%s(¥%s)%s\r\n"),
			             num_str_yen(normal_fare).c_str(), num_str_yen(fare_ic).c_str(),
		                 sWork.c_str());
		} else {
			_sntprintf_s(cb, MAX_BUF, _T("運賃(IC)： ¥%s(¥%s)%s    往復： ¥%s(¥%s)"),
			             num_str_yen(normal_fare).c_str(), num_str_yen(fare_ic).c_str(),
		                 sWork.c_str(),
			             num_str_yen(normal_fare * 2).c_str(), num_str_yen(fare_ic * 2).c_str());
            if (fareW.isDiscount) {
 				_tcscat_s(cb, NumOf(cb), _T("(割)\r\n"));
 			} else {
 				_tcscat_s(cb, NumOf(cb), _T("\r\n"));
 			}
		}
	}

	sWork = _T("");

	for (int32_t i = 0; true; i++) {
		tstring s;
		tstring dummy;
		TCHAR sb[64];
		int32_t fareStock = this->getFareStockDiscount(i, s);

		if (fareStock <= 0) {
			break;
		}
		if (0 < i) {
			sWork += _T("\r\n");
		}
		if (this->isRule114()) {
			_sntprintf_s(sb, NumOf(sb), _T("%s： ¥%s{¥%s}"),
					     s.c_str(),
						 num_str_yen(this->getFareStockDiscount(i, dummy, true) +
					                 company_fare).c_str(),
						 num_str_yen(fareStock + company_fare).c_str());
		} else {
			_sntprintf_s(sb, NumOf(sb), _T("%s： ¥%s"),
						 s.c_str(),
						 num_str_yen(fareStock + company_fare).c_str());
		}
		sWork += sb;
	}

	sWork = cb + sWork;

	/* child fare */
	if (!refRouteFlag.isRoundTrip()) {
		_sntprintf_s(cb, MAX_BUF, _T("\r\n小児運賃： ¥%-7s\r\n"),
		                              num_str_yen(this->getChildFareForDisplay()).c_str());
	} else {
		_sntprintf_s(cb, MAX_BUF, _T("\r\n小児運賃： ¥%-7s 往復： ¥%-5s\r\n"),
		                              num_str_yen(this->getChildFareForDisplay()).c_str(),
		                              num_str_yen(this->roundTripChildFareWithCompanyLine()).c_str());
	}
	sWork += cb;

	normal_fare = this->getAcademicDiscountFare();
	if (0 < normal_fare) {
		if (!refRouteFlag.isRoundTrip()) {
			_sntprintf_s(cb, MAX_BUF, _T("学割運賃： ¥%-7s\r\n"),
		    	                          num_str_yen(normal_fare).c_str());
		} else {
			_sntprintf_s(cb, MAX_BUF, _T("学割運賃： ¥%-7s 往復： ¥%-5s\r\n"),
		    	                          num_str_yen(normal_fare).c_str(),
		        	                      num_str_yen(this->roundTripAcademicFareWithCompanyLine()).c_str());
		}
		sWork += cb;
	}
	_sntprintf_s(cb, MAX_BUF,
				_T("\r\n有効日数：%4u日\r\n%s\r\n"),
				this->getTicketAvailDays(),
                1 < this->getTicketAvailDays() ?
                _T("途中下車できます") : _T("途中下車前途無効"));
	sResult += sWork + cb;

    if (this->isMultiCompanyLine()) {
		sResult += _T("\r\n複数の会社線を跨っているため、乗車券は通し発券できません. 運賃額も異なります.");
	} else if (this->isBeginEndCompanyLine()) {
		sResult += _T("\r\n会社線通過連絡運輸ではないためJR窓口で乗車券は発券されません.");
	}
//	if (this->isEnableTokaiStockSelect()) {
//		sResult += _T("\r\nJR東海株主優待券使用オプション選択可");
//	}
    if (this->getIsBRT_discount()) {
        sResult += _T("\r\nBRT乗継ぎ割引適用");
    }

    if (!refRouteFlag.no_rule && !refRouteFlag.osakakan_detour &&
        refRouteFlag.special_fare_enable) {
        sResult += _T("\r\n特定区間割引運賃適用");
    }
	if (refRouteFlag.no_rule && refRouteFlag.special_fare_enable) {
        sResult += _T("\r\n特定区間割引運賃を適用していません");
	}
	if (refRouteFlag.no_rule && refRouteFlag.isAvailableRule86()) {
        sResult += _T("\r\n旅客営業規則第86条を適用していません");
	}
	if (refRouteFlag.no_rule && refRouteFlag.isAvailableRule87()) {
        sResult += _T("\r\n旅客営業規則第87条を適用していません");
	}
	if (refRouteFlag.no_rule && refRouteFlag.isAvailableRule88()) {
        sResult += _T("\r\n旅客営業規則第88条を適用していません");
	}
	if (refRouteFlag.no_rule && refRouteFlag.isAvailableRule69()) {
        sResult += _T("\r\n旅客営業規則第69条を適用していません");
	}
	if (refRouteFlag.no_rule && refRouteFlag.isAvailableRule70()) {
        sResult += _T("\r\n旅客営業規則第70条を適用していません");
	}
	if (refRouteFlag.no_rule && refRouteFlag.isAvailableRule115()) {
        sResult += _T("\r\n旅客営業取扱基準規程第115条を適用していません");
	}
	if (refRouteFlag.isAvailableRule16_5()) {
        sResult += _T("\r\nこの乗車券はJRで発券されません. 東京メトロでのみ発券されます");
	}
	if (this->isRule114()) {
        sResult += _T("\r\n旅客営業取扱基準規程第114条適用営業キロ計算駅:");
		sResult += this->getRule114apply_terminal_station();
	}
	if (refRouteFlag.compnterm) {
        sResult += tstring(_T("\r\nこの経路の会社線通過連絡は許可されていません."));
	}
    sWork = this->getTOICACalcRoute_string();
    if (sWork != _T("")) {
        sResult += _T("\r\nIC運賃計算経路: ");
        sResult += sWork;
    }
    sResult += _T("\r\n");
	return sResult;
}

// Rule114 適用となった計算駅
//
tstring     FARE_INFO::getRule114apply_terminal_station() const
{
	return RouteUtil::StationName(rule114Info.stationId()).c_str(); 
}

/*  public
 *  運賃計算
 *  @retval 0 : empty root
 *  @retval -2 : 吉塚、西小倉における不完全ルート：この経路の片道乗車券は購入できません.
 *  @retval 1 : normal(Success)
 */
int32_t CalcRoute::calcFare(FARE_INFO* pFi)
{
	if (route_list_raw.size() <= 1) {
		pFi->setEmpty();

#if 0 /* fare_info.result_flag は privateなので でもASSERTは正当 */
ASSERT((BIT_CHK(fare_info.result_flag, BRF_COMAPANY_FIRST) && route_flag.compnbegin) ||
	(!BIT_CHK(fare_info.result_flag, BRF_COMAPANY_FIRST) && !route_flag.compnbegin));
ASSERT((BIT_CHK(fare_info.result_flag, BRF_COMAPANY_END) && route_flag.compnend) ||
	(!BIT_CHK(fare_info.result_flag, BRF_COMAPANY_END) && !route_flag.compnend));
#endif
		return 0;
	}

	if (BIT_CHK(route_list_raw.back().flag, BSRNOTYET_NA)) {
					// BIT_ON(result_flag, BRF_ROUTE_INCOMPLETE)
		pFi->setInComplete();	// この経路の片道乗車券は購入できません."));
#if 0 /* fare_info.result_flag は privateなので でもASSERTは正当 */
		ASSERT((BIT_CHK(fare_info.result_flag, BRF_COMAPANY_FIRST) && route_flag.compnbegin) ||
			(!BIT_CHK(fare_info.result_flag, BRF_COMAPANY_FIRST) && !route_flag.compnbegin));
		ASSERT((BIT_CHK(fare_info.result_flag, BRF_COMAPANY_END) && route_flag.compnend) ||
			(!BIT_CHK(fare_info.result_flag, BRF_COMAPANY_END) && !route_flag.compnend));
#endif
		return -2;
	}


    /* 86, 87, 69, 70条 114条適用かチェック */
    if (!route_flag.no_rule && !route_flag.osakakan_detour) {
        // これをここに置かないと86.87＋近郊でNG
        checkOfRuleSpecificCoreLine(true);	// route_list_raw -> route_list_cooked
    		/* 規則適用 */
        pFi->setTerminal(this->beginStationId(),
                              this->endStationId());    // set is begin/end terminal Id.

    	if (pFi->calc_fare(&route_flag, route_list_cooked)) {
            bool b_more_low_cost;
            pFi->setRoute(this->route_list_cooked, route_flag);
            if (pFi->isJrTokaiOnly()) {
                b_more_low_cost = pFi->reCalcFareForOptiomizeRouteForToiCa(*this);
            } else {
                b_more_low_cost = pFi->reCalcFareForOptiomizeRoute(*this);
            }
            if (b_more_low_cost) {
                TRACE("changed fare for lowcost\n");
                ; // DO NOTHING
            } else {
                // rule 114 applied
                pFi->setRule114(rule114Info);
            }
			pFi->setIsRule16_5_route(*this);
        } else {
    		pFi->reset();
    	}
    } else {
        /* 規則非適用 */ /* 単駅 */
        (void)checkOfRuleSpecificCoreLine();	// route_list_raw -> route_list_cooked
            /* 規則非適用 */ /* 単駅 */
		pFi->setTerminal(this->beginStationId(),
		 				 this->endStationId());
		if (pFi->calc_fare(&route_flag, route_list_raw)) {
            pFi->setRoute(this->route_list_raw, route_flag);
			   // routeFlag.rule115 のflag set する為だけに以下を実行
			(void)pFi->reCalcFareForOptiomizeRoute(*this);
			ASSERT(pFi->getBeginTerminalId() == this->beginStationId());
			ASSERT(pFi->getEndTerminalId() == this->endStationId());
        }
		pFi->setIsRule16_5_route(*this);
    }
	return 1;
}


int32_t CalcRoute::calcFare(FARE_INFO* pFi, int32_t count)
{
    if (route_list_raw.size() < (unsigned)count) {
        return 0;
    }

    CalcRoute aRoute;
    aRoute.route_list_raw.assign(route_list_raw.cbegin(), route_list_raw.cbegin() + count);
    return aRoute.calcFare(pFi);
}


// static
// public
//      86条、87条、88条 中心駅を返す
//  @param [in] cityId  都区市内ID
//  @retval 中心駅ID
//
int32_t FARE_INFO::CenterStationIdFromCityId(int32_t cityId)
{
    const uint32_t cityIds[] = {
            CITYNO_TOKYO,
            CITYNO_YOKOHAMA,
            CITYNO_NAGOYA,
            CITYNO_KYOUTO,
            CITYNO_OOSAKA,
            CITYNO_KOUBE,
            CITYNO_HIROSIMA,
            CITYNO_KITAKYUSYU,
            CITYNO_FUKUOKA,
            CITYNO_SENDAI,
            CITYNO_SAPPORO,
            CITYNO_YAMATE,
            CITYNO_SHINOOSAKA,
        };
    const LPCTSTR centerName[] = {
        _T("東京"),
        _T("横浜"),
        _T("名古屋"),
        _T("京都"),
        _T("大阪"),
        _T("神戸"),
        _T("広島"),
        _T("小倉"),
        _T("博多"),
        _T("仙台"),
        _T("札幌"),
        _T("東京"),
        _T("新大阪"),
    };
    uint32_t n;

    for (n = 0; n < NumOf(cityIds); n++) {
        if (cityIds[n] == cityId) {
            return STATION_ID(centerName[n]);
        }
    }
    return 0;   /* notfound */
}


//	遠回り/近回り設定
//
//	@return 0 success(is last)
//	@retval 1 success
//	@retval otherwise failued
//
int32_t Route::setDetour(bool enabled)
{
	int32_t rc;
    route_flag.osakakan_detour = enabled;
    rc = reBuild();
	return rc;
}

void Route::setNoRule(bool no_rule)
{
	if (!no_rule && route_flag.osakakan_detour) {
		route_flag.osakakan_detour = false;
		reBuild();
	}
	route_flag.no_rule = no_rule;
}


//	小倉ー博多 新幹線・在来線別線扱い
//	@param [in] enabled  : true = 有効／ false = 無効
//
void Route::setNotSameKokuraHakataShinZai(bool enabled)
{
	if (enabled) {
        route_flag.notsamekokurahakatashinzai = true;
	} else {
        route_flag.notsamekokurahakatashinzai = false;
	}
}

bool Route::isNotSameKokuraHakataShinZai()
{
    return route_flag.notsamekokurahakatashinzai;
}

//public
//  開始駅IDを得る
//  @param [in] applied_agree : True=特例適用 / False=特例非適用
//  @return  開始駅ID(1000を越えると都区市内ID)
//
int32_t CalcRoute::beginStationId()
{
	int32_t stid;

	if (route_flag.no_rule || route_flag.osakakan_detour) {
		return route_list_raw.front().stationId;
	}
	else {
		stid = coreAreaIDByCityId(CSTART);
		if (stid == 0) {
			ASSERT(route_list_cooked.front().stationId == route_list_raw.front().stationId);
			return route_list_raw.front().stationId;
		}
		else {
			return stid + STATION_ID_AS_CITYNO;
		}
	}
}

//  public
//  着（終了）駅IDを得る
//  @param [in] applied_agree : True=特例適用 / False=特例非適用
//  @return 開始駅ID(1000を越えると都区市内ID)
//
int32_t CalcRoute::endStationId()
{
    int32_t stid;

    if (route_flag.no_rule || route_flag.osakakan_detour) {
        return route_list_raw.back().stationId;

    } else {
        stid = coreAreaIDByCityId(CEND);
        if (stid == 0) {
			ASSERT(route_list_cooked.back().stationId == route_list_raw.back().stationId);
			return route_list_raw.back().stationId;
        } else {
			return stid + STATION_ID_AS_CITYNO;
		}
	}
}

//  public
//  着（終了）駅を得る
//  @param [in] ident 都区市内 or 駅ID
//  @return 終了駅名文字列を返す(tstring)
//
tstring CalcRoute::BeginOrEndStationName(int32_t ident)
{
    if (ident < STATION_ID_AS_CITYNO) {
        return RouteUtil::StationNameEx(ident);
    } else {
        return RouteUtil::CoreAreaCenterName(ident);
    }
}

//static
//	駅ID→駅名
//
tstring RouteUtil::CoreAreaCenterName(int32_t id)
{
	TCHAR name[MAX_COREAREA_CHR];

	memset(name, 0, sizeof(name));

	DBO ctx = DBS::getInstance()->compileSql(
        "select name from t_coreareac where rowid=?");
	if (ctx.isvalid()) {
        if (STATION_ID_AS_CITYNO <= id) {
            id -= STATION_ID_AS_CITYNO;
        }
		ctx.setParam(1, id);

		if (ctx.moveNext()) {
			_tcscpy_s(name, NumOf(name), ctx.getText(0).c_str());
		}
	}
	return name;
}


//private
//	経路フラグから発着都区市内IDを得る
//
//	@param [in] startEndFlg     CSTART(発駅) / CEND(着駅)
//
//	@return 特定都区市内ID(0だと都区市内ではない単駅)
//
int32_t CalcRoute::coreAreaIDByCityId(int32_t startEndFlg) const
{
	int32_t cityno;

	if (route_flag.isAvailableRule86or87()) {
        ASSERT(
            (route_flag.rule86or87 == 0x01) ||
            (route_flag.rule86or87 == 0x02) ||
            (route_flag.rule86or87 == 0x04) ||
            (route_flag.rule86or87 == 0x08) ||
            (route_flag.rule86or87 == 0x03) ||
            (route_flag.rule86or87 == 0x0c));

		if (((startEndFlg == CSTART) && (0 != (route_flag.rule86or87 & 0x04))) ||
		    ((startEndFlg == CEND)   && (0 != (route_flag.rule86or87 & 0x08)))) {
			return CITYNO_YAMATE;	/* 山手線内*/
		} else if (((startEndFlg == CSTART) && (0 != (route_flag.rule86or87 & 0x01))) ||
				   ((startEndFlg == CEND)   && (0 != (route_flag.rule86or87 & 0x02)))) {
			int flags;
			if (startEndFlg == CSTART) {
				flags = route_list_cooked.front().flag;
			} else {
				flags = route_list_cooked.back().flag;
			}
			cityno = MASK_CITYNO(flags);
			if ((0 < cityno) && (cityno < CITYNO_YAMATE)) {
				return cityno;
			} else {
				ASSERT(FALSE);
			}
		}
    }
    if (route_flag.rule88) {
        if (((startEndFlg == CSTART) && route_flag.ter_begin_oosaka) ||
				   ((startEndFlg == CEND)   && route_flag.ter_fin_oosaka)) {
			return CITYNO_SHINOOSAKA;	/* 大阪・新大阪 */
		}
		// else
		// thru
	}
	return 0;
}

//public:
//	経路を逆転
//
//	@retval 1   sucess
//	@retval 0   sucess(end)
//	@retval -1	failure(6の字を逆転すると9になり経路重複となるため)
//
int32_t Route::reverse()
{
	int32_t station_id;
	int32_t line_id;
    int32_t rc = -1;
	vector<RouteItem> route_list_rev;
	vector<RouteItem>::const_reverse_iterator rev_pos;
	vector<RouteItem>::const_iterator pos;

	if (route_list_raw.size() <= 1) {
		return rc;  // -1
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
		rc = add(line_id, /*station_id,*/ pos->stationId);
		if (rc < 0) {
			/* error */
			/* restore */

			removeAll();	/* clear route_list_raw */

			rev_pos = route_list_rev.crbegin();
			station_id = rev_pos->stationId;
			add(station_id);
			for (rev_pos++; rev_pos != route_list_rev.crend(); rev_pos++) {
				int r = add(rev_pos->lineId, /*station_id,*/ rev_pos->stationId, 1<<8);
				ASSERT(0 <= r);
				station_id = rev_pos->stationId;
			}
			TRACE(_T("cancel reverse route\n"));
			return rc;
		}
		station_id = pos->stationId;
		line_id = pos->lineId;
	}
	TRACE(_T("reverse route\n"));
	return rc;
}


/*!	@brief ルート作成(文字列からRouteオブジェクトの作成)
 *
 *	@param [in] route_str	カンマなどのデリミタで区切られた文字列("駅、路線、分岐駅、路線、..."）
 *	@retval -200 failure(駅名不正)
 *	@retval -300 failure(線名不正)
 *	@retval -1   failure(経路重複不正)
 *	@retval -2   failure(経路不正)
 *	@retval 1 success
 *	@retval 0 success
 */
int32_t Route::setup_route(LPCTSTR route_str)
{
	const static TCHAR* token = _T(", |/\t");
	TCHAR* p;
	int32_t lineId = 0;
	int32_t stationId1 = 0;
	int32_t stationId2 = 0;
	int32_t rc = 1;
	int32_t len;
	TCHAR* ctx = NULL;
	bool backup_notsamekokurahakatashinzai = route_flag.notsamekokurahakatashinzai;

	removeAll();

	len = (int32_t)_tcslen(route_str) + 1;
	TCHAR *rstr = new TCHAR [len];
	if (rstr == NULL) {
		return -1;
	}
	_tcscpy_s(rstr, len, route_str);
	len = _tcslen(TITLE_NOTSAMEKOKURAHAKATASHINZAI);
	if (0 == _tcsncmp(rstr, TITLE_NOTSAMEKOKURAHAKATASHINZAI, len)) {
		route_flag.notsamekokurahakatashinzai = true;
		p = rstr + len;
	} else {
		p = rstr;
	}

	for (p = _tcstok_s(p, token, &ctx); p; p = _tcstok_s(NULL, token, &ctx)) {
		if (stationId1 == 0) {
			stationId1 = RouteUtil::GetStationId(p);
			if (stationId1 <= 0) {
				rc = -200;		/* illegal station name */
				break;
			}
			add(stationId1);
		} else if (lineId == 0) {
			if (*p == _T('r')) {	/* 大阪環状線 遠回り */
				++p;
				route_flag.osakakan_detour = true;
			}
			lineId = RouteUtil::GetLineId(p);
			if (lineId <= 0) {
				rc = -300;		/* illegal line name */
				break;
			}
		} else {
			stationId2 = RouteUtil::GetStationId(p);
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
			 * 4: company pass finish
			 * -4: wrong company pass
			 */
ASSERT((rc == 0) || (rc == 1) || (rc == 10) || (rc == 11) || (rc == 4));
			if (rc <= 0) {
				break;
			}
			lineId = 0;
			stationId1 = stationId2;
		}
	}
	delete [] rstr;

	route_flag.notsamekokurahakatashinzai = backup_notsamekokurahakatashinzai;
	return rc;
}


// static version
//	@brief 経由文字列を返す
//
//	@param [in] routeList    route
//	@param [in] route_flag    route flag(LF_OSAKAKAN_MASK:大阪環状線関連フラグのみ).
//	@retval 文字列
//
tstring RouteUtil::Show_route(const vector<RouteItem>& routeList, const RouteFlag& rRoute_flag)
{
	tstring lineName;
	tstring stationName;
#define MAX_BUF 1024
	//TCHAR buf[MAX_BUF];
	tstring result_str;
	int32_t station_id1;
    RouteFlag routeFlag_ = rRoute_flag;

	if (routeList.size() == 0) {	/* 経路なし(AutoRoute) */
		return _T("");
	}

	vector<RouteItem>::const_iterator pos = routeList.cbegin();

	result_str = _T("");
	station_id1 = pos->stationId;
    routeFlag_.setOsakaKanFlag(RouteFlag::OSAKAKAN_NOPASS);

	for (pos++; pos != routeList.cend() ; pos++) {

		lineName = RouteUtil::LineName(pos->lineId);

		if ((pos + 1) != routeList.cend()) {
			/* 中間駅 */
			if (!IS_FLG_HIDE_LINE(pos->flag)) {
				if (ID_L_RULE70 != pos->lineId) {
					result_str += _T("[");
					result_str += lineName;
					if (LINE_ID(_T("大阪環状線")) == pos->lineId) {
						result_str += RouteOsakaKanDir(station_id1, pos->stationId, routeFlag_);
                        routeFlag_.setOsakaKanPass(true);
					}
					result_str += _T("]");
				} else {
					result_str += _T(",");
				}
			}
			station_id1 = pos->stationId;
			if (!IS_FLG_HIDE_STATION(pos->flag)) {
				stationName = RouteUtil::StationName(station_id1);
				result_str += stationName;
			}
		} else {
			/* 着駅 */
			if (!IS_FLG_HIDE_LINE(pos->flag)) {
				result_str += _T("[");
				result_str += lineName;
				if (LINE_ID(_T("大阪環状線")) == pos->lineId) {
					result_str += RouteOsakaKanDir(station_id1, pos->stationId, routeFlag_);
					routeFlag_.setOsakaKanPass(true);
				}
				result_str += _T("]");
			}
			//result_str += stationName;	// 着駅
		}
		//result_str += buf;
	}
	return result_str;
}

// static version
//	@brief 完全な経由文字列を返す
//	(for Debug only use)
//	@param [in] routeList    route
//	@param [in] route_flag    route flag(LF_OSAKAKAN_MASK:大阪環状線関連フラグのみ).
//	@retval 文字列
//
tstring RouteUtil::Show_route_full(const vector<RouteItem>& routeList, const RouteFlag& rRoute_flag)
{
	if (routeList.size() == 0) {	/* 経路なし(AutoRoute) */
		return _T("");
	}
	tstring startStationName = RouteUtil::StationName(routeList.front().stationId);
	tstring route_str = RouteUtil::Show_route(routeList, rRoute_flag);
	tstring arriveStationName = RouteUtil::StationName(routeList.back().stationId);
	return startStationName + route_str + arriveStationName;
}


//static private
//	@brief 大阪環状線 方向文字列を返すで
//
//	@param [in] station_id1  発駅
//	@param [in] station_id2  着駅
//	@param [in] route_flag    route flag.
//	@retval 文字列
//
tstring  RouteUtil::RouteOsakaKanDir(int32_t station_id1, int32_t station_id2, const RouteFlag& rRoute_flag)
{
	tstring result_str;
	const TCHAR * const result[] = {
		_T(""),
		_T("(内回り)"),
		_T("(外回り)"),
	};
	uint8_t inner_outer[] = {
		0, 2, 2, 2, 1, 1, 0, 1, 1, 1, 1, 1, 2, 2, 0, 2, 0,
 //     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
	};
/*
         f e d c b a 9 8 7 6 5 4 3 2 1 0
neerdir  1 0 1 0 1 0 1 0 1 0 1 0 1 0 1 0  順廻りが近道なら1
detour   1 1 0 0 1 1 0 0 1 1 0 0 1 1 0 0  遠回り指定で1
forward  1 1 1 1 0 0 0 0 1 1 1 1 0 0 0 0  逆回りで1
dir      1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0  下り(反時計)廻りで1
F/R      O   o O I I I i I   I I O O O   0/1 neer/far
         | | | | | | | | | | | | | | | +- 0
         | | | | | | | | | | | | | | +--- 0 1西九条 > 大阪    1天王寺 >> 大阪
         | | | | | | | | | | | | | +----- 1 1大正 >> 寺田町   1芦原橋 >> 天王寺
         | | | | | | | | | | | | +------- 1                   2-大阪 >> 天王寺,,,京橋 > 鶴橋
         | | | | | | | | | | | +--------- 0 1大正 > 寺田町
         | | | | | | | | | | +----------- 0                   2桃谷 大阪環状線 天王寺 関西線 ... 大阪 大阪環状線 天王寺
         | | | | | | | | | +------------- 1
         | | | | | | | | +--------------- 1 1西九条 >> 大阪
         | | | | | | | +----------------- 0                   2芦原橋・天王寺 … 京橋 > 大正
         | | | | | | +------------------- 0 1大阪 > 西九条    2天王寺 > 京橋 1/2天王寺 > 大阪
         | | | | | +--------------------- 1 1寺田町 >> 大正
         | | | | +----------------------- 1                    2-大阪 >> 天王寺,,,京橋 > 天満
         | | | +------------------------- 0 1寺田町 > 大正     2天王寺 > 今宮 2天王寺 > 西九条
         | | +--------------------------- 0                    2天王寺 |> 大阪
         | +----------------------------- 1
         +------------------------------- 1 1大阪 >> 西九条    1天王寺 >> 大阪

         > 近回り
         >>一回目遠回り
         |> 2回目 やむを得ず遠回り
         1 1回目
         2 2回目

遠回り指定、今宮経由が近道、  今宮経由、  上り 1010 x
遠回り指定、非今宮経由が近道、非今宮経由、下り 1101 I                   2-大阪 >> 天王寺,,,京橋 > 天満
遠回り指定、非今宮経由が近道、非今宮経由、上り 1100 O                   2-大阪 >> 天王寺,,,京橋 > 鶴橋
近回り、    非今宮経由が近道、今宮経由、  上り 0110 x
近回り、　　非今宮経由が近道、今宮経由、  下り 0111 x
近回り、　　今宮経由が近道、  非今宮経由、上り 0000 x
遠回り指定、今宮経由が近道、  非今宮経由、上り 1000 O 1西九条 > 大阪    1天王寺 >> 大阪
遠回り指定、今宮経由が近道、  今宮経由、  下り 1011 o                   2天王寺 |> 大阪
遠回り指定、非今宮経由が近道、今宮経由、  下り 1111 O 1大阪 >> 西九条    1天王寺 >> 大阪
近回り、　　今宮経由が近道、  今宮経由、  下り 0011 O 1寺田町 > 大正     2天王寺 > 今宮 2天王寺 > 西九条
近回り、　　非今宮経由が近道、非今宮経由、上り 0100 O 1大正 >> 寺田町   1芦原橋 >> 天王寺
近回り、    今宮経由が近道、  非今宮経由、下り 0001 i 2-芦原橋 大阪環状線 天王寺 関西線 久宝寺 おおさか東線 放出 片町線 京橋 大阪環状線 大正
遠回り指定、今宮経由が近道、  非今宮経由、下り 1001 I 1大阪 > 西九条      2天王寺 > 京橋 1/2天王寺 > 大阪
遠回り指定、非今宮経由が近道、今宮経由、  上り 1110 I 1西九条 >> 大阪
近回り、　　非今宮経由が近道、非今宮経由、下り 0101 I 1寺田町 >> 大正
近回り、　　今宮経由が近道、  今宮経由、  上り 0010 I 1大正 > 寺田町
*/
	int  pass;
	uint8_t  c;

	c = DirOsakaKanLine(station_id1, station_id2) == 0 ? 1 : 0;
    c |= rRoute_flag.osakakan_detour ? 0x02 : 0;
    if (rRoute_flag.getOsakaKanPass()) {
        c |= rRoute_flag.osakakan_2dir ? 0x04 : 0;
        pass = 2;
    } else {
        c |= rRoute_flag.osakakan_1dir ? 0x04 : 0;
        pass = 1;
    }

	c |= LDIR_ASC == RouteUtil::DirLine(LINE_ID(_T("大阪環状線")), station_id1, station_id2) ? 0x08 : 0;
TRACE(_T("RouteOsakaKanDir:[%d] %s %s %s: %d %d %d %d\n"),
      pass,
      SNAME(station_id1),
      (0 != (c & 0x02) && (pass != 2)) ? _T(">>") : _T(">"),
      SNAME(station_id2), 0x1 & c, 0x1 & (c >> 1), 0x01 & (c >> 2), 0x01 & (c >> 3));

	if (NumOf(inner_outer) <= c) {
		c = NumOf(inner_outer) - 1;
	}
	return result[inner_outer[c]];
}

/*	ルート表示
 *	@return ルート文字列
 *
 *	@remark showFare()の呼び出し後にのみ有効
 */
tstring RouteList::route_script()
{
	const vector<RouteItem>* routeList;
	tstring result_str;
	bool oskk_flag;

	routeList = &route_list_raw;	/* 規則非適用 */

	if (routeList->size() == 0) {	/* 経路なし(AutoRoute) */
		return _T("");
	}

	vector<RouteItem>::const_iterator pos = routeList->cbegin();

	if (route_flag.notsamekokurahakatashinzai) {
		result_str = TITLE_NOTSAMEKOKURAHAKATASHINZAI;
	} else {
		result_str = _T("");
	}
	result_str += RouteUtil::StationNameEx(pos->stationId);

	oskk_flag = false;
	for (pos++; pos != routeList->cend() ; pos++) {
		result_str += _T(",");
		if (!oskk_flag && (pos->lineId == LINE_ID(_T("大阪環状線")))) {
			if (route_flag.osakakan_detour) {
				result_str += _T("r");
			}
			oskk_flag = true;
		}
		result_str += RouteUtil::LineName(pos->lineId);
		result_str += _T(",");
		result_str += RouteUtil::StationNameEx(pos->stationId);
	}
//	result_str += _T("\n\n");
	return result_str;
}

#if 0	// 大阪環状線方向実装したしroute_flagどうするか困るし未使用だし
//
// static
tstring RouteUtil::Route_script(const vector<RouteItem>& routeList)
{
	tstring result_str;
	bool oskk_flag;

	if (routeList.size() == 0) {	/* 経路なし(AutoRoute) */
		return _T("");
	}

	vector<RouteItem>::const_iterator pos = routeList.cbegin();

	result_str = RouteUtil::StationNameEx(pos->stationId);

	oskk_flag = false;
	for (pos++; pos != routeList.cend() ; pos++) {
		result_str += _T(",");
		result_str += RouteUtil::LineName(pos->lineId);
		result_str += _T(",");
		result_str += RouteUtil::StationNameEx(pos->stationId);
	}
	result_str += _T("\n\n");
	return result_str;
}
#endif

//private:
//	@brief	分岐マークOff
//
//	@param [in]  line_id          路線
//	@param [in]  to_station_id    開始駅(含まない)
//	@param [in]  begin_station_id 終了駅(含む)
//
//	@note add()で分岐特例経路自動変換時に使用大阪環状線はないものとする
//
void Route::routePassOff(int32_t line_id, int32_t to_station_id, int32_t begin_station_id)
{
	RoutePass route_pass(jct_mask, route_flag, line_id, to_station_id, begin_station_id);
	route_pass.off(jct_mask);
	route_pass.update_flag(&route_flag); /* update route_flag LF_OSAKAKAN_MASK */
}


//	経路設定中 経路重複発生時
//	経路設定キャンセル
//
//	@param [in]  bWithStart  開始駅もクリアするか(デフォルトクリア)
//	@note
//	x stop_jctId > 0 : 指定分岐駅前までクリア(指定分岐駅含まず)
//	x stop_jctId = 0 : (default)指定分岐駅指定なし
//	x stop_jctId < 0 : 開始地点も削除
//
void Route::removeAll(bool bWithStart /* =true */)
{
	int32_t begin_station_id = 0;

	JctMaskClear(jct_mask);

	if (!bWithStart) {
		begin_station_id = departureStationId();
	}

	route_list_raw.clear();

    bool shinzaikyusyu = route_flag.notsamekokurahakatashinzai; // backup
    route_flag.clear();
    route_flag.notsamekokurahakatashinzai = shinzaikyusyu; // restore

	TRACE(_T("clear-all mask.\n"));

	if (!bWithStart) {
		add(begin_station_id);
	}
}


//	駅が経路内に含まれているか？
//
//	@param [in] stationId   駅ident
//	@retval true found
//	@retval false notfound
//
bool RouteList::checkPassStation(int32_t stationId)
{
	vector<RouteItem>::const_iterator route_item;
	int32_t stationIdFrom = 0;

	for (route_item = route_list_raw.cbegin(); route_item != route_list_raw.cend(); route_item++) {
		if (stationIdFrom != 0) {
			if (0 != RouteUtil::InStation(stationId, route_item->lineId, stationIdFrom, route_item->stationId)) {
				return true;
			}
		} else {
			ASSERT(route_item == route_list_raw.cbegin());
			ASSERT(departureStationId() == route_item->stationId);
		}
		stationIdFrom = route_item->stationId;
	}
	return false;
}

//	分岐駅が経路内に含まれているか？
//
//	@param [in] stationId   駅ident
//	@retval count of found(0 or 1)
//
int32_t Route::junctionStationExistsInRoute(int32_t stationId)
{
	int32_t c;
	vector<RouteItem>::const_iterator route_item;

	c = 0;
	for (route_item = route_list_raw.cbegin(); route_item != route_list_raw.cend(); route_item++) {
		if (stationId == route_item->stationId) {
			c++;
		}
	}
	return c;
}

//	経路の種類を得る
//
//	@retval bit0 : there is shinkansen.
//	@retval bit1 : there is company line.
//
int32_t Route::typeOfPassedLine(int offset)
{
	int32_t c;
	int32_t count;
	vector<RouteItem>::const_iterator route_item;

	c = 0;
	count = 0;
	route_item = route_list_raw.cbegin();
	if (route_item != route_list_raw.cend()) {
		count++;
		for (route_item++; route_item != route_list_raw.cend(); route_item++) {
			count++;
			if (offset < count) {
				if (IS_SHINKANSEN_LINE(route_item->lineId)) {
					c |= 0x01;
				}
				if (IS_COMPANY_LINE(route_item->lineId)) {
					c |= 0x02;
				}
				if (c == 0x03) {
					break;
				}
			}
		}
	}
	return c;
}

#if 0 /* Dec.2916 */
//	経路内を着駅で終了する(以降の経路はキャンセル)
//
//	@param [in] stationId   駅ident
//
void Route::terminate(int32_t stationId)
{
	int32_t i;
	int32_t stationIdFrom = 0;
	int32_t newLastIndex = 0x7fffffff;
	int32_t line_id = -1;
	//int32_t stationIdTo;

	for (i = 0; i < (int32_t)route_list_raw.size(); i++) {
		if (stationIdFrom != 0) {
			/* stationIdはroute_list_raw[i].lineId内のstationIdFromから
			              route_list_raw[i].stationIdの間にあるか? */
			if (0 != RouteUtil::InStation(stationId, route_list_raw[i].lineId, stationIdFrom, route_list_raw[i].stationId)) {
				newLastIndex = i;
				line_id = route_list_raw[i].lineId;
				//stationIdTo = route_list_raw[i].stationId;
				break;
			}
		} else {
			ASSERT(i == 0);
		}
		stationIdFrom = route_list_raw[i].stationId;
	}
	if (newLastIndex < (int32_t)route_list_raw.size()) {
		while (newLastIndex < (int32_t)route_list_raw.size()) {
			removeTail();
		}
		ASSERT((newLastIndex<=1) || route_list_raw[newLastIndex - 1].stationId == stationIdFrom);
		add(line_id, /*stationIdFrom,*/ stationId);
		end_station_id = stationId;
	}
}
#endif /* Dec.2016 */

//static
//	路線が上り／下りかを得る
//
//	@param [in] line_id     路線
//	@param [in] station_id1 発
//	@param [in] station_id2 至
//
//	@retval 1 下り(LDIR_ASC)
//	@retval 2 上り(LDIR_DESC)
//
//  @node 同一駅の場合下り(0)を返す
//
//
int32_t RouteUtil::DirLine(int32_t line_id, int32_t station_id1, int32_t station_id2)
{
	const static char tsql[] =
"select case when"
"   ((select sales_km from t_lines where line_id=?1 and station_id=?2) - "
"    (select sales_km from t_lines where line_id=?1 and station_id=?3)) <= 0"
" then 1 else 2 end;";

	DBO ctx = DBS::getInstance()->compileSql(tsql);
	if (ctx.isvalid()) {
		ctx.setParam(1, line_id);
		ctx.setParam(2, station_id1);
		ctx.setParam(3, station_id2);

		if (ctx.moveNext()) {
			return ctx.getInt(0);
		}
	}
	return 0;	/* 下り */
}


// static
//	分岐ID→駅ID
//
int32_t Route::Jct2id(int32_t jctId)
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
//	駅ID→分岐ID
//
int32_t Route::Id2jctId(int32_t stationId)
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
//	分岐ID→駅名
//
tstring Route::JctName(int32_t jctId)
{
	TCHAR name[MAX_STATION_CHR];

	memset(name, 0, sizeof(name));

	DBO ctx = DBS::getInstance()->compileSql(
		"select name from t_jct j left join t_station t on j.station_id=t.rowid where id=?");
	if (ctx.isvalid()) {

		ctx.setParam(1, jctId);

		if (ctx.moveNext()) {
			_tcscpy_s(name, MAX_STATION_CHR, ctx.getText(0).c_str());
		}
	}
	return name;
}

//static
//	駅ID→駅名
//
tstring RouteUtil::StationName(int32_t id)
{
	TCHAR name[MAX_STATION_CHR];

	memset(name, 0, sizeof(name));

	DBO ctx = DBS::getInstance()->compileSql(
		"select name from t_station where rowid=?");
	if (ctx.isvalid()) {

		ctx.setParam(1, id);

		if (ctx.moveNext()) {
			_tcscpy_s(name, NumOf(name), ctx.getText(0).c_str());
		}
	}
	return name;
}

//static
//	駅ID→駅名(同名駅区別)
//
tstring RouteUtil::StationNameEx(int32_t id)
{
	TCHAR name[MAX_STATION_CHR];

	memset(name, 0, sizeof(name));

	DBO ctx = DBS::getInstance()->compileSql(
		"select name,samename from t_station where rowid=?");
	if (ctx.isvalid()) {

		ctx.setParam(1, id);

		if (ctx.moveNext()) {
			_tcscpy_s(name, NumOf(name), ctx.getText(0).c_str());
			_tcscat_s(name, NumOf(name), ctx.getText(1).c_str());
		}
	}
	return name;
}

//static
//	路線ID→路線名
//
tstring RouteUtil::LineName(int32_t id)
{
	TCHAR name[MAX_STATION_CHR];

	memset(name, 0, sizeof(name));

	DBO ctx = DBS::getInstance()->compileSql(
		"select name from t_line where rowid=?");
	if (ctx.isvalid()) {

		ctx.setParam(1, id);

		if (ctx.moveNext()) {
			_tcscpy_s(name, NumOf(name), ctx.getText(0).c_str());
		}
	}
	return name;
}


//static
//	駅の属性を得る
//
SPECIFICFLAG RouteUtil::AttrOfStationId(int32_t id)
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
//	路線駅の属性を得る
//	BIT_CHECK(BSRNOTYET_NA) is ON = 路線内にその駅はない
//
SPECIFICFLAG RouteUtil::AttrOfStationOnLineLine(int32_t line_id, int32_t station_id)
{
	DBO ctx = DBS::getInstance()->compileSql(
	//  "select lflg, sflg from t_lines where line_id=?1 and station_id=?2", true);
		"select sflg, lflg from t_station t left join t_lines on t.rowid=station_id where line_id=?1 and station_id=?2");
	if (ctx.isvalid()) {
		int32_t s;
		int32_t l;
		ctx.setParam(1, line_id);
		ctx.setParam(2, station_id);

		if (ctx.moveNext()) {
			s = 0x00007fff & ctx.getInt(0);
			l = 0xffff8000 & ctx.getInt(1);	// b15はSTATION_IS_JUNCTION_F(lflg)で必要なので注意
			return s | l;
		}
	}
	return (1<<BSRNOTYET_NA);	// 路線内にその駅はない
}

//static
//	駅は路線内にあるか否か？
//	@param [in] line_id    路線ID
//	@param [in] station_id 駅ID
//	@param [in] flag　     true: set follow flag / false: no-flag(default)
//	注： lflg&(1<<17)を含めていないため、新幹線内分岐駅、たとえば、
//	     東海道新幹線 京都 米原間に草津駅は存在するとして返します.
//
int32_t Route::InStationOnLine(int32_t line_id, int32_t station_id, bool flag /* = false */ )
{
	const char tsql_normal[] =
	//		"select count(*) from t_lines where line_id=?1 and station_id=?2");
		"select count(*) from t_lines where (lflg&((1<<31)|(1<<17)))=0 and line_id=?1 and station_id=?2";
	const char tsql_flag_off[] =
		"select count(*) from t_lines where (lflg&(1<<31))=0 and line_id=?1 and station_id=?2";

	DBO ctx = DBS::getInstance()->compileSql(flag ? tsql_flag_off : tsql_normal);
	if (ctx.isvalid()) {

		ctx.setParam(1, line_id);
		ctx.setParam(2, station_id);

		if (ctx.moveNext()) {
			return ctx.getInt(0);
		}
	}
	return 0;
}

// 駅(station_id)の所属する路線IDを得る.
// 駅は非分岐駅で, 路線は1つしかないと解っていることを前提とする.
//
// line_id = f(station_id)
//
int32_t Route::LineIdFromStationId(int32_t station_id)
{
	DBO ctx = DBS::getInstance()->compileSql(
  "select line_id"
  " from t_lines"
  " where station_id=?"
  " and 0=(lflg&((1<<31)|(1<<17)))");
	if (ctx.isvalid()) {
  		ctx.setParam(1, station_id);
		if (ctx.moveNext()) {
			return ctx.getInt(0);
		}
	}
	return 0;
}


// 駅(station_id)の所属する路線IDを得る.
// 駅は非分岐駅で, 路線は1つしかないと解っていることを前提とする.
//
// line_id = f(station_id)
//
int32_t Route::LineIdFromStationId2(int32_t station_id1, int32_t station_id2)
{
	DBO ctx = DBS::getInstance()->compileSql(
   "select line_id from t_lines where station_id=?1 and 0=(lflg&((1<<31)|(1<<17))) and line_id in "
  "(select line_id from t_lines where station_id=?2 and 0=(lflg&((1<<31)|(1<<17))))");

	if (ctx.isvalid()) {
  		ctx.setParam(1, station_id1);
  		ctx.setParam(2, station_id2);
		if (ctx.moveNext()) {
			return ctx.getInt(0);
		}
	}
	return 0;
}


// 駅名より駅IDを返す(私鉄含)
//
// station_id = f("駅名")
//
int32_t RouteUtil::GetStationId(LPCTSTR station)
{
	const char tsql[] = "select rowid from t_station where (sflg&(1<<18))=0 and name=?1 and samename=?2";

	tstring sameName;
	tstring stationName(station);

	int32_t pos = (int32_t)stationName.find(_T('('));
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

// 線名より線IDを返す
//
// station_id = f("線名")
//
int32_t RouteUtil::GetLineId(LPCTSTR lineName)
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

//static
//	都道府県名
//
tstring RouteUtil::PrefectName(int32_t id)
{
	TCHAR name[MAX_PREFECT_CHR];

	memset(name, 0, sizeof(name));

	DBO ctx = DBS::getInstance()->compileSql(
                                             "select name from t_prefect where rowid=?");
	if (ctx.isvalid()) {

        if (0x10000 <= id) {
            id = (unsigned)id >> 16;
        }
		ctx.setParam(1, id);

		if (ctx.moveNext()) {
			_tcscpy_s(name, NumOf(name), ctx.getText(0).c_str());
		}
	}
	return name;
}

//static
//	会社線名
//
tstring RouteUtil::CompanyName(int32_t id)
{
	TCHAR name[MAX_PREFECT_CHR];

	memset(name, 0, sizeof(name));

	DBO ctx = DBS::getInstance()->compileSql(
                                             "select name from t_company where rowid=?");
	if (ctx.isvalid()) {
        ctx.setParam(1, id);

		if (ctx.moveNext()) {
			_tcscpy_s(name, NumOf(name), ctx.getText(0).c_str());
		}
	}
	return name;
}

//static
//	駅の所属会社IDを得る
//	(境界駅はあいまい)
//
//	@param [in] station_id   駅id
//
int32_t  RouteUtil::CompanyIdFromStation(int32_t station_id)
{
	int32_t id1;
	int32_t id2;
    DBO ctx = DBS::getInstance()->compileSql(
				"select company_id, sub_company_id from t_station where rowid=?");
	if (ctx.isvalid()) {
		ctx.setParam(1, station_id);	// station_id
		if (ctx.moveNext()) {
			id1 = ctx.getInt(0);
			id2 = ctx.getInt(1);
			return MAKEPAIR(id1, id2);
		}
	}
	ASSERT(FALSE);
	return 0;
}

//static
//	DB ver
//
bool RouteUtil::DbVer(DBsys* db_sys)
{
    memset(db_sys, 0, sizeof(DBsys));

    DBO ctx = DBS::getInstance()->compileSql(
    "select name, db_createdate from t_dbsystem limit(1)");
    if (ctx.isvalid()) {
        if (ctx.moveNext()) {
            _tcscpy_s(db_sys->name, NumOf(db_sys->name), ctx.getText(0).c_str());
            _tcscpy_s(db_sys->createdate, NumOf(db_sys->createdate), ctx.getText(1).c_str());
            return true;
        }
    }
    return false;
}



//Static
//	@brief 分岐特例の分岐路線a+乗換駅dから本線bと分岐駅cを得る
//
//	@param [in]  jctLineId         a 分岐路線
//	@param [in]  transferStationId d 乗換駅
//	@param [out] jctspdt   b 本線, c 分岐駅
//
//	@return type 0: usual, 1-3:type B
//
int32_t Route::RetrieveJunctionSpecific(int32_t jctLineId, int32_t transferStationId, JCTSP_DATA* jctspdt)
{
	const char tsql[] =
	//"select calc_km>>16, calc_km&65535, (lflg>>16)&32767, lflg&32767 from t_lines where (lflg&(1<<31))!=0 and line_id=?1 and station_id=?2";
//	"select type,jctsp_line_id1, jctsp_station_id1, jctsp_line_id2, jctsp_station_id2"
//	" from t_jctspcl where id=("
//	"	select calc_km from t_lines where (lflg&(1<<31))!=0 and line_id=?1 and station_id=?2)";
	"select type,jctsp_line_id1, jctsp_station_id1, jctsp_line_id2, jctsp_station_id2" \
	" from t_jctspcl where id=(" \
	"	select lflg&255 from t_lines where (lflg&((1<<31)|(1<<29)))!=0 and line_id=?1 and station_id=?2)";
	int32_t type = 0;

	memset(jctspdt, 0, sizeof(JCTSP_DATA));

	DBO dbo = DBS::getInstance()->compileSql(tsql);
	if (dbo.isvalid()) {
		dbo.setParam(1, jctLineId);
		dbo.setParam(2, transferStationId);
		if (dbo.moveNext()) {
			type = dbo.getInt(0);
			jctspdt->jctSpMainLineId = dbo.getInt(1);
			jctspdt->jctSpStationId = dbo.getInt(2);
			jctspdt->jctSpMainLineId2 = dbo.getInt(3);
			jctspdt->jctSpStationId2 = dbo.getInt(4);
		}
	}
	ASSERT(((jctspdt->jctSpMainLineId2 == 0) && (jctspdt->jctSpStationId2 == 0)) ||
		   ((jctspdt->jctSpMainLineId2 != 0) && (jctspdt->jctSpStationId2 != 0)));
	if (jctspdt->jctSpStationId2 == 0) {	// safety
		jctspdt->jctSpMainLineId2 = 0;
	}
	return type;
}

//	@brief 分岐特例の分岐路線a+乗換駅dから本線bと分岐駅cを得る
//
//	@param [in]  jctLineId         a 分岐路線
//	@param [in]  transferStationId d 乗換駅
//	@param [out] jctSpMainLineId   b 本線
//	@param [out] jctSpStationId    c 分岐駅
//
//	@return type 0: usual, 1-3:type B
//
int32_t Route::getBsrjctSpType(int32_t line_id, int32_t station_id)
{
	const char tsql[] =
	"select type from t_jctspcl where id=(select lflg&255 from t_lines where line_id=?1 and station_id=?2)";
	int32_t type = -1;
	DBO dbo = DBS::getInstance()->compileSql(tsql);
	if (dbo.isvalid()) {
		dbo.setParam(1, line_id);
		dbo.setParam(2, station_id);
		if (dbo.moveNext()) {
			type = dbo.getInt(0);
		}
	}
	return type;
}

//static
//	運賃計算キロと営業キロを返す
//	(大阪環状線は新今宮を通らない経路)
//	@param [in] line_id     路線
//	@param [in] station_id1 発
//	@param [in] station_id2 至
//
//	@retuen [0]:営業キロ, [1]:計算キロ
//
vector<int32_t> RouteUtil::GetDistance(int32_t line_id, int32_t station_id1, int32_t station_id2)
{
	const char tsql[] =
"select max(sales_km)-min(sales_km), case max(calc_km)-min(calc_km) when 0 then max(sales_km)-min(sales_km) else max(calc_km)-min(calc_km) end"
" from t_lines"
" where line_id=?1 and (lflg&(1<<31))=0 and (station_id=?2 or station_id=?3)";

#if 0	/* 140416 */
"select max(sales_km)-min(sales_km), case max(calc_km)-min(calc_km) when 0 then max(sales_km)-min(sales_km) else max(calc_km)-min(calc_km) end"
" from t_lines"
" where line_id=?1"
" and	(lflg&(1<<31))=0"
" and	sales_km>=(select min(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))"
" and	sales_km<=(select max(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))";

/* old #if 0 */
"select l1.sales_km - l2.sales_km, l1.calc_km - l2.calc_km"
" from t_lines l1"
" left join t_lines l2"
" where l1.line_id=?1"
" and l2.line_id=?1"
" and l1.sales_km>l2.sales_km"
" and ((l1.station_id=?2 and l2.station_id=?3)"
" or (l1.station_id=?3 and l2.station_id=?2))";
#endif
	vector<int32_t> v;

	DBO dbo = DBS::getInstance()->compileSql(tsql);
	if (dbo.isvalid()) {
		dbo.setParam(1, line_id);
		dbo.setParam(2, station_id1);
		dbo.setParam(3, station_id2);
		if (dbo.moveNext()) {
			v.push_back(dbo.getInt(0));
			v.push_back(dbo.getInt(1));
		}
	}
	return v;
}


//static
//	運賃計算キロと営業キロを返す(大阪環状線)
//
//	@param [in] line_id     路線
//	@param [in] station_id1 発
//	@param [in] station_id2 至
//	@param [in] flag        b1lid flag
//
//	@retuen [0]:営業キロ, [1]:計算キロ
//	@note used aggregate_fare_info()* -> GetDistanceEx(), Get_route_distance()
//
vector<int32_t> RouteUtil::GetDistance(const RouteFlag& oskkflg, int32_t line_id, int32_t station_id1, int32_t station_id2)
{
	vector<int32_t> d;
	int32_t sales_km;

	if (line_id != LINE_ID(_T("大阪環状線")))  {
		ASSERT(FALSE);
		return RouteUtil::GetDistance(line_id, station_id1, station_id2);
	}

	// pass 0 1 0 1 0 1 0 1
	// 1dir 0 0 1 1 0 0 1 1
	// 2dir 0 0 0 0 1 1 1 1
	//      0 0 1 0 0 1 1 1
	// ~pass & 1dir | pass & 2dir

    if ((!oskkflg.getOsakaKanPass() && oskkflg.osakakan_1dir) ||
        (oskkflg.getOsakaKanPass() && oskkflg.osakakan_2dir)) {
		sales_km = RouteUtil::GetDistanceOfOsakaKanjyouRvrs(line_id, station_id1, station_id2);
		TRACE(_T("Osaka-kan reverse\n"));
	} else {
   		sales_km = RouteUtil::GetDistance(line_id, station_id1, station_id2)[0];
		TRACE(_T("Osaka-kan forward\n"));
	}

	d.push_back(sales_km);
	d.push_back(sales_km);

	return d;
}


//static
//	営業キロを算出（大阪環状線 新今宮を通る経路)
//
//	@param [in] line_id     大阪環状線line id
//	@param [in] station_id1 発
//	@param [in] station_id2 至
//
//	@retuen 営業キロ
//
int32_t RouteUtil::GetDistanceOfOsakaKanjyouRvrs(int32_t line_id, int32_t station_id1, int32_t station_id2)
{
#if defined _WINDOWS
	const TCHAR tsql[] =
_T("select (select max(sales_km)-min(sales_km) from t_lines where line_id=?1 and ((station_id=(select rowid from t_station where name='新今宮')) or (sales_km=(select max(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3)))))+(select min(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))+(select max(sales_km)-min(sales_km) from t_lines where line_id=(select rowid from t_line where name='関西線') and station_id in (select rowid from t_station where name='新今宮' or name='天王寺'))");
#ifdef UNICODE
	CT2A usql(tsql, CP_UTF8);
	DBO dbo = DBS::getInstance()->compileSql(usql);
#else
	DBO dbo = DBS::getInstance()->compileSql(tsql);
#endif
#else
	// 新今宮までの距離(近いほう) + 天王寺までの距離(近いほう) + 新今宮～天王寺

    const char tsql[] =
    u8"select"
    u8" (select max(sales_km)-min(sales_km) from t_lines where line_id=?1 and "
    u8"  ((station_id=(select rowid from t_station where name='新今宮')) or"
    u8"   (sales_km=(select max(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3)))))+"
    u8" (select min(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))+"
    u8" (select max(sales_km)-min(sales_km) from t_lines where line_id="
    u8"  (select rowid from t_line where name='関西線') and station_id in (select rowid from t_station where name='新今宮' or name='天王寺'))";
	DBO dbo = DBS::getInstance()->compileSql(tsql);
#endif
	int32_t km;

	if (dbo.isvalid()) {
		dbo.setParam(1, line_id);
		dbo.setParam(2, station_id1);
		dbo.setParam(3, station_id2);
		if (dbo.moveNext()) {
			km = dbo.getInt(0);
			ASSERT(0 < km);
			return km;
		}
	}
	ASSERT(FALSE);
	return 0;
}

//static
//	運賃計算キロを返す(最短距離算出用)
//
//	@param [in] line_id     路線
//	@param [in] station_id1 発
//	@param [in] station_id2 至
//
//	@retuen 営業キロ or 計算キロ
//
int32_t Route::Get_node_distance(int32_t line_id, int32_t station_id1, int32_t station_id2)
{
	const char tsql[] =
"select case when sum(calc_km)=0 then max(sales_km)-min(sales_km) else max(calc_km)-min(calc_km) end"
" from	t_lines"
" where line_id=?1 and (lflg&(1<<31))=0 and	(station_id=?2 or station_id=?3)";

#if 0	/* 140416 */
"select case when sum(calc_km)=0 then max(sales_km)-min(sales_km) else max(calc_km)-min(calc_km) end"
" from	t_lines"
" where line_id=?1"
" and	(lflg&(1<<31))=0"
" and	sales_km>=(select min(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))"
" and	sales_km<=(select max(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))";

/* old #if 0 */
"select case when (l1.calc_km-l2.calc_km)=0 then l1.sales_km - l2.sales_km else  l1.calc_km - l2.calc_km end"
" from t_lines l1"
" left join t_lines l2"
" where l1.line_id=?1"
" and l2.line_id=?1"
" and l1.sales_km>l2.sales_km"
" and ((l1.station_id=?2 and l2.station_id=?3)"
" or (l1.station_id=?3 and l2.station_id=?2))";
#endif
	DBO dbo = DBS::getInstance()->compileSql(tsql);
	if (dbo.isvalid()) {
		dbo.setParam(1, line_id);
		dbo.setParam(2, station_id1);
		dbo.setParam(3, station_id2);
		if (dbo.moveNext()) {
			return dbo.getInt(0);
		}
	}
	return 0;
}



//	70条適用
//	route_listを適用してout_route_listに設定する
//	showFare() => calcFare() => checkOfRuleSpecificCoreLine() =>
//
//	out_route_listは表示用ではなく計算用(路線が70条専用がある)
//
//	@param [in]   in_route_list  経路
//	@param [out]  out_route_list 経路
//	@retval 0: done
//	@retval -1: N/A
//  @note used member variable is route_flag at isBulletInRouteOfRule70()
//
int32_t CalcRoute::reRouteRule70j(const vector<RouteItem>& in_route_list, vector<RouteItem>* out_route_list)
{
	int32_t stage;
	int32_t stationId_o70 = 0;
    int32_t stationId_e70 = 0;
	int32_t flag = 0;
    int32_t station_id1 = 0;
    vector<PAIRIDENT> bullet_use;
	vector<RouteItem>::const_iterator route_item;

	stage = 0;

	for (route_item = in_route_list.cbegin(); route_item != in_route_list.cend(); route_item++) {
		bool skip = false;
		RouteItem ri = *route_item;

		if (stage == 0) {
			if ((route_item->flag & (1 << BCRULE70)) == 0) {
				stage = 1;					/* 1: off */
			} else {
				break;		/* 70条適用エリア内発は非適用(86、87条) */
			}
		} else if (stage == 1) {
			if ((route_item->flag & (1 << BCRULE70)) != 0) {
				if (IS_COMPANY_LINE(route_item->lineId)) {
					stage = 999;	// 千代田線通った規70条は適用しない
					break;
				}
				stage = 2;					/* 2: on */ /* 外から進入した */
								/* 路線より最外側の大環状線内(70条適用)駅を得る */
				stationId_o70 = CalcRoute::RetrieveOut70Station(route_item->lineId);
				ASSERT(0 < stationId_o70);
                station_id1 = ri.stationId;         /* 新幹線判定用 */
				ri.stationId = stationId_o70;
				flag = route_item->flag;
			} else {	// 外のまま
				/* do nothing */
			}
		} else if (stage == 2) {
			if ((route_item->flag & (1 << BCRULE70)) == 0) {
				if (IS_COMPANY_LINE(route_item->lineId)) {	//
					stage = 999;	// 千代田線通った規70条は適用しない
					break;
				}
				stage = 3;					/* 3: off: !70 -> 70 -> !70 (applied) */
								/* 進入して脱出した */
								/* 路線より最外側の大環状線内(70条適用)駅を得る */
				stationId_e70 = CalcRoute::RetrieveOut70Station(route_item->lineId);
				ASSERT(0 < stationId_e70);
				if (stationId_e70 != stationId_o70) {
					out_route_list->push_back(RouteItem(ID_L_RULE70, stationId_e70, flag));
				}
			} else {	// 中のまま
				skip = true;
				flag = route_item->flag;
                if (IS_SHINKANSEN_LINE(ri.lineId)) {
                    /* 70条経路上の新幹線乗車は大都市近郊区間適用外 */
                    bullet_use.push_back(MAKEPAIR(ri.stationId, station_id1));
                }
                station_id1 = ri.stationId;     /* 新幹線判定用 */
			}
		} else if (stage == 3) {
			/* 4 */
			if ((route_item->flag & (1 << BCRULE70)) != 0) {
				stage = 4;				// 進入して脱出して(通過して)また進入したら70条非適用
										/* !70 -> 70 -> !70 -> 70 (exclude) */
				break;
			} else {
				/* 70条通過(通過して外に出ている */
				/* do nothing */
			}
		} else {	/* -1: 発駅から70条適用駅 */
			/* do nothing */
		}
		if (!skip) {
			out_route_list->push_back(ri);
		}
	}

    if (3 == stage) {
        /* Normal */
        /* b#19081602 */
        // テーブルにあれば70条の経路に沿った新幹線乗車で、rule70bullet is True
        // (近郊区間無効となる新幹線乗車あり)
        // なければ,checkIsBulletInUrbanOnSpecificTerm()でbullet_lineを設定
        // 70経路内の無効な新幹線乗車は、ID_L_RULE70 で新幹線乗車そのものが消えて無くなる(乗っていないことになり近郊区間有効。経路は自由なので乗ることもできる)

        std::vector<PAIRIDENT>::const_iterator it = bullet_use.cbegin();
        while (it != bullet_use.cend()) {
            int32_t station_id1 = IDENT1(*it);
            int32_t station_id2 = IDENT2(*it);
                                    // ex.   品川           東京  　　　  品川             赤羽
            if (isBulletInRouteOfRule70(station_id1, station_id2, stationId_o70, stationId_e70)) {
                TRACE("Ride of Shinkansen in route 70.\n");
                route_flag.rule70bullet = true;
            }
            it++;
        }
    }

	switch (stage) {
	case 0:
		// 非適合
		// 0: 70条適用駅発
		out_route_list->assign(in_route_list.cbegin(), in_route_list.cend());
		break;
	case 1:		// 1: 70条適用駅なし(都内を通らない多くのルート)
		return -1;
		break;
	case 3:		/* 70条適用駅を通過 */
		return 0;	// done
		break;
	case 2:		// 2: 70条適用駅着(発駅は非適用駅)
	case 4:		// 4: 70条適用駅区間を通過して再度進入した場合
	default:
		/* cancel */
		out_route_list->clear();
		out_route_list->assign(in_route_list.cbegin(), in_route_list.cend());
		break;
	}
	return -1;
}

// 新幹線乗車は70条の経路内での乗車か？
// station_id1, station_id2  新幹線乗車区間
// stationId_o70, stationId_e70 70条入り口、出口駅
// @retval true テーブルあり(有効: 70経路内での新幹線乗車)
//  @note used member variable is route_flag at isBulletInRouteOfRule70()
//
bool CalcRoute::isBulletInRouteOfRule70(int32_t station_id1, int32_t station_id2, int32_t stationId_o70, int32_t stationId_e70)
{
    static const char tsql[] =
    "select count(*) from t_r70bullet "
    " where "
    " ((station_id1=?1 and station_id2=?2) or "
    "  (station_id1=?2 and station_id2=?1)) and "
    "   ((station70_id1=?3 and station70_id2=?4) or"
    "    (station70_id1=?4 and station70_id2=?3));";

    DBO dbo(DBS::getInstance()->compileSql(tsql));
    dbo.setParam(1, station_id1);
    dbo.setParam(2, station_id2);
    dbo.setParam(3, stationId_o70);
    dbo.setParam(4, stationId_e70);

    if (dbo.moveNext()) {
        return 1 == dbo.getInt(0);
    }
    return false;
}

//static
//	69条適用駅を返す
//	端駅1～端駅2のペア×Nをかえす.
//	results[]配列は必ずn*2を返す
//
//	showFare() => calcFare() => checkOfRuleSpecificCoreLine() => ReRouteRule69j()=>
//
//	coninue_flag: false 続き無し
//                true  続き
//	return: false 続き無し
//          true  続き
bool CalcRoute::Query_a69list(int32_t line_id, int32_t station_id1, int32_t station_id2, vector<PAIRIDENT>* results, bool continue_flag)
{
	const static char tsql[] =
//" select station_id, (lflg>>0)&15, (lflg>>24)&1,  (lflg>>23)&1"
" select station_id, lflg"
" from t_lines"
" where line_id=?1"
" and (lflg&((1<<31)|(1<<24)))=(1<<24)"
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

	int32_t prev_flag = 0;
	int32_t prev_stid = 0;
	int32_t cur_flag;
	int32_t cur_stid;
	bool next_continue = false;

	vector<PAIRIDENT> pre_list;
	vector<PAIRIDENT>::const_iterator it;

	results->clear();

	TRACE(_T("c69 line_id=%d, station_id1=%d, station_id2=%d\n"), line_id, station_id1, station_id2);

	DBO ctx = DBS::getInstance()->compileSql(tsql);
	if (ctx.isvalid()) {
		ctx.setParam(1, line_id);
		ctx.setParam(2, station_id1);
		ctx.setParam(3, station_id2);
		while (ctx.moveNext()) {
			cur_stid = ctx.getInt(0);
			cur_flag = ctx.getInt(1);
			// bit23 -> bit15 | bit3-0
			cur_flag = ((cur_flag >> (BSR69CONT - 15)) & (1 << 15)) | (cur_flag & 0x0f);
			pre_list.push_back(MAKEPAIR(IDENT1(cur_stid), cur_flag));
		}
		cur_stid = 0;
		cur_flag = 0;
		for (it = pre_list.cbegin(); it != pre_list.cend(); it++) {
			cur_stid = IDENT1(*it);
			cur_flag = IDENT2(*it);

			if ((prev_flag & 0x0f) == (cur_flag & 0x0f)) {
				// OK
				if (((cur_flag & (1 << 15)) != 0) && //((it + 1) == pre_list.cend()) &&
					 (station_id2 == cur_stid)) { /* 最後でBSR69CONT=1 */
					next_continue = true;
				} else if ((cur_flag & (1 << 15)) != 0) {
					// NG
					prev_flag = 0;
					continue;
				}
				results->push_back(MAKEPAIR(IDENT1(prev_stid), (prev_flag & 0x0f)));
				results->push_back(MAKEPAIR(IDENT1(cur_stid), (cur_flag & 0x0f)));
				prev_flag = 0;

				TRACE(_T("c69             station_id1=%s(%d), station_id2=%s(%d)\n"), SNAME(prev_stid), prev_stid, SNAME(cur_stid), cur_stid);

				if (((cur_flag & (1 << 15)) != 0) &&
				   // ((it + 1) == pre_list.cend()) &&
				    (station_id2 == cur_stid)) { /* 最後でBSR69CONT=1 */
					next_continue = true;
				} else {

				}
			} else {
				/* 継続以外で継続フラグ付BSR69TERMは無効*/
				if ((continue_flag && ((cur_flag & (1 << 15)) != 0) && (station_id1 == cur_stid)) ||
				   (!continue_flag && ((cur_flag & (1 << 15)) == 0))) {
					// OK
					prev_flag = cur_flag;
					prev_stid = cur_stid;
				} else {
					// NG skip
					prev_flag = 0;
					prev_stid = 0;
				}
				continue_flag = false;	/* 1回目以外はOff */
			}
		}
	}
	return next_continue;
}

//static
//	69条置換路線、駅を返す
//
bool CalcRoute::Query_rule69t(const vector<RouteItem>& in_route_list, const RouteItem& cur, int32_t ident, vector<vector<PAIRIDENT>>* results)
{
	int32_t lineId;
	int32_t stationId1;
	int32_t stationId2;
	int32_t in;
	int32_t in1;
	int32_t s1;
	int32_t s2;
	const static char tsql[] =
	" select line_id, station_id1, station_id2 from t_rule69 where id=? order by ord";

	results->clear();

	DBO ctx = DBS::getInstance()->compileSql(tsql);
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
			if (s1 != 0) { /* 2回目以降 ? */
				if (!cur.is_equal(*route_item)) {
					it = results->cbegin();
					while (it != results->cend()) {
						lineId = it->at(0);
						stationId1 = it->at(1);
						stationId2 = it->at(2);
						in = RouteUtil::InStation(s2, lineId, stationId1, stationId2);
						in1 = RouteUtil::InStation(s1, lineId, stationId1, stationId2);
						/* 2駅とも置換路線上にあれば置換しない
						 * 1駅のみ置換路線上にある場合、その駅が端駅でなければ置換しない
						 */
						if (results->size() == 1) {
                            if (((in != 0) && (in1 != 0)) ||
                                ((0 != RouteUtil::InStation(stationId1, lineId, s1, s2)) &&
                                 (0 != RouteUtil::InStation(stationId2, lineId, s1, s2)))) {
								results->clear();	/* 置換対象外とする */
								return false;
							}
						} else if (it == results->cbegin()) {
							if (((in != 0) && (in1 != 0)) ||
						        ((in  != 0) && (s2 != stationId1)) ||
						        ((in1 != 0) && (s1 != stationId1))) {
								results->clear();	/* 置換対象外とする */
								return false;
							}
						} else if ((it + 1) == results->cend()) {
							if (((in != 0) && (in1 != 0)) ||
						        ((in  != 0) && (s2 != stationId2)) ||
						        ((in1 != 0) && (s1 != stationId2))) {
								results->clear();	/* 置換対象外とする */
								return false;
							}
						} else {
                            // Don't come here.
							if ((in != 0) || (in1 != 0)) {
								results->clear();	/* 置換対象外とする */
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
//	69条適用
//	in_route_listを適用してroute_list_cookedに設定する
//	out_route_listは表示用ではなく計算用
//	showFare() => calcFare() => checkOfRuleSpecificCoreLine() =>
//
//	@param [in] in_route_list  変換前のチェック経路
//	@param [out] out_route_list 変換後のチェック経路
//	@retval 0: N/A
//	@retval 0<: replace for number of route
//
int32_t CalcRoute::ReRouteRule69j(const vector<RouteItem>& in_route_list, vector<RouteItem>* out_route_list)
{
	int32_t i;
	int32_t change;
	bool continue_flag;
	int32_t station_id1;
	int32_t a69_line_id;
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
			continue_flag = CalcRoute::Query_a69list(a69_line_id, station_id1, route_item->stationId, &dbrecord, continue_flag);
			for (i = 0; i < (int32_t)(dbrecord.size() - (continue_flag ? 2 : 0)); i += 2) {
				if ((dbrecord.size() - i) < 2) {
					ASSERT(FALSE);
					break;
				}
				a69list.push_back(MAKEPAIR(IDENT1(dbrecord[i]), IDENT1(dbrecord[i + 1])));
				// 置換処理
				CalcRoute::Query_rule69t(in_route_list, *route_item, IDENT2(dbrecord[i]), &trule69list);
				if (1 == trule69list.size()) {
					if (0 == a69list.size()) {
						ASSERT(FALSE);
					} else if (1 == a69list.size()) {
						/* 置換処理1 */
						if (route_item->stationId == IDENT2(a69list.at(0))) { /* a69list.station2 ? */
							route_item->lineId = trule69list.at(0).at(0);
							if (station_id1 != IDENT1(a69list.at(0))) {       /* a69list.station1 ? */
								/* 乗車路線, 駅2を挿入 */
								route_item = out_route_list->insert(route_item, RouteItem(a69_line_id, IDENT1(a69list.at(0))));
								route_item++;
							}
						} else {
							if (station_id1 != IDENT1(a69list.at(0))) { /* a69list.station1 ? */
								/* 乗車路線, 駅1を挿入 */
								route_item = out_route_list->insert(route_item, RouteItem(a69_line_id, IDENT1(a69list.at(0))));
								route_item++;
							}
							/* 計算路線、駅2を挿入 */
							route_item = out_route_list->insert(route_item, RouteItem(trule69list.at(0).at(0), IDENT2(a69list.at(0))));
							route_item++;
						}
					} else if (2 <= a69list.size()) {
						/* 置換処理3 */
						if (route_item->stationId == IDENT2(a69list.back())) {
							route_item->lineId = trule69list.at(0).at(0);
							route_item->stationId = IDENT2(a69list.back());
							if ((route_item - a69list.size())->stationId == IDENT1(a69list.front())) {
								// c) in-in
#if 1
								int32_t j = (int32_t)a69list.size() - 1;
								route_item -= j;
								for ( ; 0 < j; j--) {
									route_item = out_route_list->erase(route_item);
								}
#else	/* こっちはNG(返されるiteratorが無効で飛ぶ) */
								route_item = out_route_list->erase(route_item - (a69list.size() - 1),
								                                   route_item - (a69list.size() - 1) + (a69list.size() - 1));
#endif
							} else { // d) out-in
								if (2 < a69list.size()) {
									int32_t j = (int32_t)a69list.size() - 2;
									route_item -= j;
									for ( ; 0 < j; j--) {
										route_item = out_route_list->erase(route_item);
									}

									route_item = out_route_list->erase(route_item - (a69list.size() - 2),
									                                   route_item - (a69list.size() - 2) + (a69list.size() - 2));
									// 3経路以上は現状なし
								}
								(route_item - 1)->stationId = IDENT1(a69list.front());
							}
						} else if ((route_item - a69list.size())->stationId == IDENT1(a69list.front())) {
							/* b) in -> out */
							(route_item - 1)->lineId = trule69list.at(0).at(0);
							(route_item - 1)->stationId = IDENT2(a69list.back());
							if (2 < a69list.size()) {
#if 1
								int32_t j = (int32_t)a69list.size() - 2;
								route_item -= j;
								for ( ; 0 < j; j--) {
									route_item = out_route_list->erase(route_item);
								}
#else	/* こっちはNG(iteratorが無効) */
								route_item = out_route_list->erase(route_item - (a69list.size() - 1),
								                                   route_item - (a69list.size() - 1) + (a69list.size() - 2));
#endif
								// 3経路以上は現状なし
							}
						} else {
							/* a) out -> out */
							route_item = out_route_list->insert(route_item, RouteItem(trule69list.at(0).at(0), IDENT2(a69list.back())));
							route_item++;
							(route_item - a69list.size())->stationId = IDENT1(a69list.front());
							if (2 < a69list.size()) {
#if 1
								int32_t j = (int32_t)a69list.size() - 1;
								route_item -= j;
								j--;
								for ( ; 0 < j; j--) {
									route_item = out_route_list->erase(route_item);
								}
#else	/* こっちはNG(iteratorが無効) */
								route_item = out_route_list->erase(route_item - (a69list.size() - 1),
								                                   route_item - (a69list.size() - 1) + (a69list.size() - 2));
#endif
								// 3経路以上は現状なし
							}
						}
					} else {
						ASSERT(FALSE);	/* not implement for 3 <= a69list.size() */
					}
				} else if (0 == trule69list.size()) {
					// ASSERT(FALSE);
					// 69条 「…かつこ内の両線路にまたがる場合を除いて」の場合
				} else if (2 == trule69list.size()) {
					/* 置換処理2 */
					if ((PAIRIDENT)IDENT1(a69list.at(0)) == trule69list.at(0).at(1)) {
						// 下り線
						route_item->lineId = trule69list.at(1).at(0);
						route_item = out_route_list->insert(route_item, RouteItem(trule69list.at(0).at(0), trule69list.at(0).at(2)));
						route_item++;
					} else {
						// 上り線
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
 				ASSERT((2 <= dbrecord.size()) && ((size_t)i == (dbrecord.size() - 2)));
				a69list.push_back(MAKEPAIR(dbrecord[i], dbrecord[i + 1]));
 			} else {
				a69list.clear();
			}
		}
		station_id1 = route_item->stationId;
	}
	CalcRoute::RemoveDupRoute(out_route_list);
	TRACE(change == 0 ? "noapplid rule69(%d)\n" : "applied rule69 count=%d\n", change);
	return change;
}



//static
//	重複ルート除去
//
//	       勝浦
//  外房線 蘇我 <- remove
//  外房線 千葉
//	総武線 東京
//
int32_t CalcRoute::RemoveDupRoute(vector<RouteItem> *routeList)
{
	int32_t affects = 0;
#if 1	/* erase(iterator)しかないので */
	vector<RouteItem>::iterator pos;

	for (pos = routeList->begin(); pos != routeList->end() && (pos + 1) != routeList->end(); ) {
		if (pos->lineId == (pos + 1)->lineId) {
			pos = routeList->erase(pos);
			affects++;
		} else {
			pos++;
		}
	}

#else	/* erase(index)があればこっちでも可 */
	int32_t last = routeList->size() - 2;
	int32_t idx = 0;

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
//	路線の駅間に都区市内駅はあるか？
//	     東海道新幹線 京都 米原間に草津駅は存在するとして返します.
//
//	@param [in] cityno     検索する都区市内No
//	@param [in] lineId     路線
//	@param [in] stationId1 開始駅
//	@param [in] stationId2 終了駅
//	@return 0: not found / not 0: ocunt of found.
//
int32_t CalcRoute::InCityStation(int32_t cityno, int32_t lineId, int32_t stationId1, int32_t stationId2)
{
	static const char tsql[] =
"select count(*)"
"	from t_lines"
"	where line_id=?1"
"	and (lflg&(1<<31))=0"
"	and exists (select * from t_station where rowid=station_id and (sflg&15)=?4)"
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
	DBO dbo = DBS::getInstance()->compileSql(tsql);
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
//	86条適用されるかのチェック
//	showFare() => calcFare() => checkOfRuleSpecificCoreLine() =>
//
//	@param [in]  in_route_list 経路
//  @param [in]  route_flag     フラグ(BLF_JRTOKAISTOCK_APPLIED)
//	@param [out] exit          脱出路線・駅
//	@param [out] entr          進入路線・駅
//	@param [out] cityId_pair   IDENT1(発駅特定都区市内Id), IDENT2(着駅特定都区市内Id)
//
//	@return	0: 非適応(両端単駅)              -
//			1: 発駅が特定都区市内            exit有効
//			2: 着駅が特定都区市内            entr有効
//			3: 発駅≠着駅とも特定都区市内    entr, exit 有効
//		 0x83: 発駅=着駅=特定都区市内        entr, exit 有効
//			4: 全駅同一特定都区市内          -
//		0x8000_0000 : BLF_JRTOKAISTOCK_ENABLE JR東海株主優待券使用メニュー有効可否フラグ
//
//	 [九] 山陽新幹線
//	 [福] 鹿児島線
//	 [神] 山陽新幹線
//	 [札] 函館線
//	 [仙] 東北新幹線、東北線
//	 [広] 山陽新幹線、山陽線
//	 [阪] 東海道線
//	 [京] 東海道線
//	 [名] 東海道線
//	 [浜] 東海道線
//	 と、都区内以外はみんなあり得る
// 同一線で都区市内駅(o)のパターン
//	--o-- 上記一覧
//	ooooo あり(問題なし)
//	o---- 通常
//	----o 通常
//	o---o なし(乗車駅または分岐駅～分岐駅または降車駅が都区市内だが間に非都区市内が含まれる例はなし。
//
uint32_t CalcRoute::CheckOfRule86(const vector<RouteItem>& in_route_list, const RouteFlag& rRoute_flag, Station* exit, Station* entr, PAIRIDENT* cityId_pair)
{
	vector<RouteItem>::const_iterator fite;
	vector<RouteItem>::const_reverse_iterator rite;
	SPECIFICFLAG city_no_s;
	SPECIFICFLAG city_no_e;
	uint32_t c;
	uint32_t r;
	Station in_line;
	Station out_line;
	int32_t stationId = 0;
	int32_t lineId = 0;

	r = 0;
	fite = in_route_list.cbegin();
	if (fite == in_route_list.cend()) {
		return 0;	/* empty */
	}
	city_no_s = MASK_CITYNO(fite->flag);

	// 発駅が尼崎の場合大阪市内発ではない　基153-2
	if ((city_no_s == CITYNO_OOSAKA) && (STATION_ID(_T("尼崎")) == fite->stationId)) {
		city_no_s = 0;
	}
	else if (city_no_s != 0) {
		/* "JR東海株主優待券使用"指定のときは適用条件可否適用 */
		r |= 0x80000000; // BIT_ON(route_flag, BLF_JRTOKAISTOCK_ENABLE); // for UI
		if ((rRoute_flag.jrtokaistock_applied) && (city_no_s != CITYNO_NAGOYA)) { /* by user */
			city_no_s = 0;
		}
		/* 会社線 JR東海許可で 東海道新幹線駅 発 */
		if (rRoute_flag.tokai_shinkansen) {
		//	city_no_s = 0;
		}
	}

	rite = in_route_list.crbegin();
	if (rite == in_route_list.crend()) {
		ASSERT(FALSE);
		return 0;	/* fatal error */
	}
	city_no_e = MASK_CITYNO(rite->flag);
	// 着駅が尼崎の場合大阪市内着ではない　基153-2
	if ((city_no_e == CITYNO_OOSAKA) && (STATION_ID(_T("尼崎")) == rite->stationId)) {
		city_no_e = 0;
	}
	else if (city_no_e != 0) {
		/* "JR東海株主優待券使用"指定のときは適用条件可否適用 */
		r |= 0x80000000; // BIT_ON(route_flag, BLF_JRTOKAISTOCK_ENABLE); // for UI
		if ((rRoute_flag.jrtokaistock_applied) && (city_no_e != CITYNO_NAGOYA)) {
			city_no_e = 0;
		}
		/* 会社線 JR東海許可で 東海道新幹線駅 着 */
		if (rRoute_flag.tokai_shinkansen) {
		//	city_no_e = 0;
		}
	}

	*cityId_pair = MAKEPAIR(city_no_s, city_no_e);

	if (city_no_s != 0) {
		// 発駅=特定都区市内
		c = 0;
		stationId = fite->stationId;	// 発
		for (fite++; fite != in_route_list.cend(); fite++) {
			uint32_t cno = MASK_CITYNO(fite->flag);
			if (c == 0) {
				if (cno != city_no_s) {
					c = 1;			// 抜けた
					out_line.set(*fite);
				}
			} else {
				if (cno == city_no_s) {
					if (c == 1) {
						c = 2;		// 戻ってきた
						in_line.set(*fite);
					} else {
						ASSERT(c == 2);
					}
				} else {
					if ((c == 2) || (0 < InCityStation(city_no_s, fite->lineId, stationId, fite->stationId))) {
						c = 3;		// 戻ってまた抜けた
						break;
					}
					ASSERT(c == 1);
				}
			}
			stationId = fite->stationId;	// 発
		}
		switch (c) {
		case 0:
			return 4;	/* 全駅特定都区市内 */
			break;
		case 1:
			r &= ~0xffff;
			r |= 1;		/* 発駅特定都区市内 */
			*exit = out_line;
			break;
		case 2:
			*exit = out_line;
			*entr = in_line;
			return 0x83;	/* 発駅=着駅=特定都区市内 */
			break;
		case 3:
			// 8の字(いったん離れて戻ってきてまた離れた) (単駅(特定都区市内無効))
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}

	if (city_no_e != 0) {
		// 着駅=特定都区市内
		c = 0;
		in_line.set(*rite);
		for (rite++; rite != in_route_list.crend(); rite++) {
			uint32_t cno = MASK_CITYNO(rite->flag);
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
			stationId = rite->stationId;	// 発
			lineId = rite->lineId;
		}
		switch (c) {
		case 1:
			*entr = in_line;
			r |= 0x02;	/* 着駅特定都区市内(発駅≠着駅) */
			break;
		case 3:
			// 8の字(いったん離れて戻ってきてまた離れた) (単駅(特定都区市内無効))
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}
	return r;
}

//static
// 山手線内発着かをチェック
//	showFare() => calcFare() => checkOfRuleSpecificCoreLine() =>
//
//	@param [in] in_route_list  route
//	@return 0  : 発着ではない
//			1  : 発駅は山手線内
//			2  : 着駅は山手線内
//			3  : (返すことはない)
//		 0x83  : 発着駅山手線内
//		    4  : 全駅駅山手線内
//
uint32_t CalcRoute::CheckOfRule87(const vector<RouteItem>& in_route_list)
{
	vector<RouteItem>::const_iterator fite;
	vector<RouteItem>::const_reverse_iterator rite;
	uint32_t c;
	uint32_t r;

	r = 0;

	fite = in_route_list.cbegin();
	if ((fite != in_route_list.cend()) && /* 発駅適用 ？ */
	    (((fite->flag & ((1 << 10) | (1 << 5))) == ((1 << 10)|(1 << 5))))) {
		// start=山手線
		c = 0;
		for (fite++; fite != in_route_list.cend(); fite++) {
			if (c == 0) {
				if (((fite->flag & ((1 << 10) | (1 << 5))) != ((1 << 10)|(1 << 5)))) {
					c = 1;	// 抜けた
				}
			} else {
				if (((fite->flag & ((1 << 10) | (1 << 5))) == ((1 << 10)|(1 << 5)))) {
							// 山手線内
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
			return 4;		/* 全駅山手線内 */
			break;
		case 1:
			r = 1;			/* 発駅山手線内 */
			break;
		case 2:
			return 0x83;	/* 発駅=着駅=山手線内 */
			break;
		case 3:
			// 8の字(いったん離れて戻ってきてまた離れた) (単駅(特定都区市内無効))
			return 0;		/* 単駅とする */
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}
	rite = in_route_list.crbegin();
	if ((rite != in_route_list.crend()) && /* 着駅適用 ? */
		(((rite->flag & ((1 << 10) | (1 << 5))) == ((1 << 10)|(1 << 5))))) {
		// 着駅=山手線内
		c = 0;
		for (rite++; rite != in_route_list.crend(); rite++) {
			if (((rite->flag & ((1 << 10) | (1 << 5))) == ((1 << 10)|(1 << 5)))) {
						/* 山手線内 */
				if (c == 0) {
					// continue (c == 0)
				} else {
					c = 3;
					break;
				}
			} else {	/* 山手線外 */
				if (c == 0) {
					c = 1;		/* take */
				} else {	// c == 1
					// continue
				}
			}
		}

		switch (c) {
		case 1:
			ASSERT(r == 0);		/* 上で0x83で返しているはずなので */
			r |= 0x02;			/* 着駅山手線内 */
			break;
		case 3:
			// 8の字(いったん離れて戻ってきてまた離れた) (単駅(特定都区市内無効))
			break;				/* 単駅 */
		default:
			ASSERT(FALSE);
			break;
		}
	}
	return r;
}


//static
//	進入/脱出路線から86条中心駅から最初の乗換駅までの路線と乗換駅を返す
//	showFare() => calcFare() => checkOfRuleSpecificCoreLine() => ReRouteRule86j87j() =>
//
//  @param [in] lineId : 進入／脱出路線
//	@param [in] cityId : 都区市内Id
//
//  @retval vector<Station> : 返却配列(lineId, stationId)
//
//	例：lineId:常磐線, cityId:[区] -> 東北線、日暮里
//
vector<Station> CalcRoute::SpecificCoreAreaFirstTransferStationBy(int32_t lineId, int32_t cityId)
{
	const static char tsql[] =
"select station_id, line_id2 from t_rule86 where"
" line_id1=?1 and (city_id & 255)=?2 order by city_id";

	int32_t station_id;
	int32_t line_id;
	vector<Station> firstTransferStation;

	DBO dbo = DBS::getInstance()->compileSql(tsql);
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
//	86条中心駅を返す
//	showFare() => calcFare() => checkOfRuleSpecificCoreLine() => ReRouteRule86j87j() =>
//
//  @param [in]  cityId : 都区市内Id
//
//  @return 中心駅
//
int32_t CalcRoute::Retrieve_SpecificCoreStation(int32_t cityId)
{
	int32_t stationId;

	const static char tsql[] =
"select rowid from t_station where"
" (sflg & (1 << 4))!=0 and (sflg & 15)=?1";

	DBO dbo = DBS::getInstance()->compileSql(tsql);
	if (dbo.isvalid()) {
		dbo.setParam(1, cityId);
		if (dbo.moveNext()) {
			stationId = dbo.getInt(0);
			return stationId;
		}
	}
	return 0;
}

//	Route 営業キロを計算
//	showFare() => calcFare() => checkOfRuleSpecificCoreLine() =>
//	checkOfRule114j() =>
//
//	@param [in]  route_flag 大阪環状線通過方向(BLF_OSAKAKAN_1DIR, BLF_OSAKAKAN_2DIR, BLF_OSAKAKAN_1PASS)
//                         * BLF_OSAKAKAN_1PASS はwork用に使用可
//	@param [in]  route     計算ルート
//	@retuen 営業キロ[0] ／ 計算キロ[1] ／ 会社線キロ[2]　／ BRTキロ[3]
//
vector<int32_t> CalcRoute::Get_route_distance(const RouteFlag& rRoute_flag, const vector<RouteItem>& route)
{
	vector<RouteItem>::const_iterator it;
	int32_t total_sales_km;
	int32_t total_calc_km;
	int32_t total_company_km;
	int32_t total_brt_km;
	int32_t stationId;
	RouteFlag oskk_flag;

	it = route.cbegin();
	total_sales_km = 0;
	total_calc_km = 0;
	stationId = 0;
	total_company_km = 0;
	total_brt_km = 0;
	/* 大阪環状線flag */

	oskk_flag = rRoute_flag;
	oskk_flag.setOsakaKanPass(false);

	while (it != route.cend()) {
		if (stationId != 0) {
			vector<int32_t> vkms;
			if (it->lineId == LINE_ID(_T("大阪環状線"))) {
				vkms = RouteUtil::GetDistance(oskk_flag, it->lineId, stationId, it->stationId);
                oskk_flag.setOsakaKanPass(true);
			} else if (it->lineId == ID_L_RULE70) {
				vkms.push_back(FARE_INFO::Retrieve70Distance(stationId, it->stationId));
				vkms.push_back(vkms.at(0));
			} else {
				vkms = RouteUtil::GetDistance(it->lineId, stationId, it->stationId);
			}
			ASSERT(vkms.size() == 2);
			total_sales_km += vkms.at(0);
			total_calc_km  += vkms.at(1);
			if (IS_COMPANY_LINE(it->lineId)) {
				total_company_km += vkms.at(0);
			} else if (IS_BRT_LINE(it->lineId)) {
				total_brt_km += vkms.at(0);
			}
		}
		stationId = it->stationId;
		it++;
	}
	vector<int32_t> v;
	v.push_back(total_sales_km);
	v.push_back(total_calc_km);
	v.push_back(total_company_km);
	v.push_back(total_brt_km);
	return v;
}

//static
//	86, 87条適用変換
//	showFare() => calcFare() => checkOfRuleSpecificCoreLine() =>
//	route_list_raw => route_list_cooked
//
//	@param [in]     cityId      core id
//
//	@param [in]     mode
//						0: 非適応(両端単駅)              steady
//						1: 発駅が特定都区市内            modified
//						2: 着駅が特定都区市内            modified
//						3: 発駅≠着駅とも特定都区市内    modified
//					 0x83: 発駅=着駅=特定都区市内        modified
//						4: 全駅同一特定都区市内          steady
//
//	@param [in]     exit            脱出路線・駅
//	@param [in]     entr            進入路線・駅
//	@param [in/out] out_route_list  変換元・変換先経路
//
//	@remark このあと69条適用が必要(69条適用後でも可)
//	@remark ルール未適用時はroute_list_cooked = route_list_rawである
//
void  CalcRoute::ReRouteRule86j87j(PAIRIDENT cityId, int32_t mode, const Station& exit, const Station& enter, vector<RouteItem>* out_route_list)
{
	int32_t coreStationId;
	bool skip;
	int32_t lineId;
	vector<RouteItem>::const_iterator itr;
	vector<RouteItem> work_route_list;
	vector<Station> firstTransferStation;
	int32_t c;   // work counter
	int32_t n;   // work counter

	if ((mode & 1) != 0) {
		/* 発駅-脱出: exit 有効*/		/* ex) [[東北線、日暮里]] = (常磐線, [区]) */
		firstTransferStation = CalcRoute::SpecificCoreAreaFirstTransferStationBy(exit.lineId, IDENT1(cityId));
		if (firstTransferStation.size() <= 0) {
	        /* テーブル未定義 */
			work_route_list.assign(out_route_list->cbegin(), out_route_list->cend());
		} else {
			vector<Station>::const_reverse_iterator sta_ite;
			sta_ite = firstTransferStation.crbegin();
			if (exit.lineId == sta_ite->lineId) {
				work_route_list.push_back(RouteItem(0, sta_ite->stationId));	/* 発駅:都区市内代表駅 */
				// ASSERT(sta_ite->stationId == CalcRoute::Retrieve_SpecificCoreStation(IDENT(cityId))); (新横浜とか新神戸があるので)
			} else {
				coreStationId = CalcRoute::Retrieve_SpecificCoreStation(IDENT1(cityId));
				ASSERT(0 < coreStationId);
				work_route_list.push_back(RouteItem(0, coreStationId));			/* 発駅:都区市内代表駅 */
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
				}/* else  脱出 路線:駅の前の経路をスキップ*/
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
		/* 着駅-進入: entr 有効 */
		itr = work_route_list.cbegin();
		n  = 0;
		for (c = 1; itr != work_route_list.cend(); ++itr, ++c) {
			if (enter.is_equal(*itr)) {
				// 京都 東海道線 山科 湖西線 近江塩津 北陸線 富山 高山線 岐阜 東海道線 山科で着駅が[京]にならない不具合
				n = c;
			}
		}
		itr = work_route_list.cbegin();
		for (c = 1; itr != work_route_list.cend(); ++itr, ++c) {
			if ((n == 0) || (c < n)) {
				out_route_list->push_back(*itr);
			}
		}
		firstTransferStation = CalcRoute::SpecificCoreAreaFirstTransferStationBy(enter.lineId, IDENT2(cityId));
		if (firstTransferStation.size() <= 0) {
			// 博多南線で引っかかる ASSERT(FALSE);
			out_route_list->assign(work_route_list.cbegin(), work_route_list.cend());
		} else {
			vector<Station>::const_iterator sta_ite;
			sta_ite = firstTransferStation.cbegin();
			ASSERT(sta_ite != firstTransferStation.cend());
			if (enter.lineId == sta_ite->lineId) {
				out_route_list->push_back(RouteItem(sta_ite->lineId, sta_ite->stationId));
				// ASSERT(sta_ite->stationId == CalcRoute::Retrieve_SpecificCoreStation(IDENT2(cityId))); 新横浜とかあるので
			} else {
				out_route_list->push_back(RouteItem(enter.lineId, sta_ite->stationId));
				lineId = sta_ite->lineId;
				while (++sta_ite != firstTransferStation.cend()) {
					out_route_list->push_back(RouteItem(lineId, sta_ite->stationId));
					lineId = sta_ite->lineId;
				}
				coreStationId = CalcRoute::Retrieve_SpecificCoreStation(IDENT2(cityId));
				ASSERT(0 < coreStationId);
				out_route_list->push_back(RouteItem(lineId, coreStationId));
			}
			TRACE("end station is re-route rule86/87\n");
		}
	} else {
		out_route_list->assign(work_route_list.cbegin(), work_route_list.cend());
	}
}

#if 0
//static
//	経路は近郊区間内にあるか(115条2項check)
//  経路すべての駅が同一近郊間都市名なら、その都市名を返す(東京、新潟、仙台、福岡、関西）
//  (Nout used)
uint8_t CalcRoute::InRouteUrban(const vector<RouteItem>& route_list)
{
	uint16_t urban;

	vector<RouteItem>::const_iterator ite = route_list.cbegin();
	urban = (uint16_t)(ite->flag & MASK_URBAN);
	while (route_list.cend() != ite) {
		if (urban != (MASK_URBAN & ite->flag)) {
			urban = 0;
			break;
		}
		++ite;
	}
	return URBAN_ID(urban);
}
#endif


/* 近郊区間ではない条件となる新幹線乗車があるか */
  //  経路はJR東海管内のみか？
  //  b_jrtokaiOnly true: JR東海管内のみ / false=以外
  //  @note 東京ー熱海間はJR東日本エリアだけど新幹線はJR東海エリアなのでその判定をやる
  /*
      b:発駅が境界駅ならtrue
      f:着駅が境界駅ならtrue
      -:true
      n:新幹線ならtrue
      x:false
  */
void CalcRoute::checkIsJRTokaiOnly()
{
    IDENT station_id1 = 0;		/* station_id1, (station_id2=ite->stationId) */
	vector<RouteItem>::const_iterator ite = route_list_raw.cbegin();
    int32_t cid1 = 0;
	int32_t cid_s1;
	int32_t cid_e1;
	int32_t cid_s2;
	int32_t cid_e2;
	int32_t id_line_tokaido_shinkansen = LINE_ID(_T("東海道新幹線"));
    bool bJrTokaiOnly = true;

	for (ite = route_list_raw.cbegin(); ite != route_list_raw.cend(); ite++) {
        int32_t cid = RouteUtil::CompanyIdFromStation(ite->stationId);
		if (station_id1 != 0) {
            /* JR東海以外 and 東海道新幹線でない場合false */
            if (ite->lineId != id_line_tokaido_shinkansen) {
                cid_e1 = IDENT1(cid);
                cid_s1 = IDENT1(cid1);
                cid_e2 = IDENT2(cid);
                cid_s2 = IDENT2(cid1);
				if (((cid_s1 == cid_e1) && (JR_CENTRAL != cid_e1)) ||   /* 塩尻-甲府 */
					((cid_s1 != JR_CENTRAL) && (cid_s2 != JR_CENTRAL)) ||
					((cid_e1 != JR_CENTRAL) && (cid_e2 != JR_CENTRAL))) {
					bJrTokaiOnly = false;
                    break;
				}
            }
		}
		station_id1 = ite->stationId;
        cid1 = cid;
    }
    route_flag.bJrTokaiOnly = bJrTokaiOnly;
}

//public
//	86、87条、69条判定＆経路算出
//	showFare() => calcFare() =>
//	route_list_raw => route_list_cooked
//
//	後半でBLF_TER_xxx(route[0].lineId)を設定します
//
//	@param [in]  route_flag   BLF_MEIHANCITYFLAG = 発駅:着駅 無効(0)/有効(1)
//	@param [out] rule114	 [0] = 運賃, [1] = 営業キロ, [2] = 計算キロ
//	@return false : rule 114 no applied. true: rule 114 applied(available for rule114[] )
//	@remark ルール未適用時はroute_list_cooked = route_list_rawである
//
#define RULE114_SALES_KM_86	1700
#define RULE114_SALES_KM_87	800
void CalcRoute::checkOfRuleSpecificCoreLine(bool isCheckRule114 /* =false */)
{
	PAIRIDENT cityId;
	int32_t jsales_km;
	vector<RouteItem> route_list_tmp;
	vector<RouteItem> route_list_tmp2;
	vector<RouteItem> route_list_tmp3;
	vector<RouteItem> route_list_tmp4;
	Station enter;
	Station exit;
	int32_t n;
	int32_t sk;         /* 114 check 90km or 190km */
	int32_t sk2 = 0;	/* begin and arrive point as city, 101km or 201km */
	uint32_t chk;        /* 86 applied flag */
	uint32_t rtky;       /* 87 applied flag */
	uint32_t flg;
	int32_t aply88;
	vector<int32_t> km_raw;

    route_flag.terCityReset();
    route_flag.optionFlagReset();

    checkIsJRTokaiOnly();   // JR東海株主有効可否

	// 69を適用したものをroute_list_tmpへ
	n = CalcRoute::ReRouteRule69j(route_list_raw, &route_list_tmp);	/* 69条適用(route_list_raw->route_list_tmp) */
	TRACE("Rule 69 applied %dtimes.\n", n);
    if (0 < n) {
        route_flag.rule69 = true;      // applied rule
    }
	// route_list_tmp2 = route_list_tmp
	// 70を適用したものをroute_list_tmp2へ
	n = CalcRoute::reRouteRule70j(route_list_tmp, &route_list_tmp2);
	TRACE(0 == n ? "Rule70 applied.\n" : "Rule70 not applied.\n");
	if (0 == n) {
		route_flag.rule70 = true;      // applied rule
	}

	// 88を適用したものをroute_list_tmpへ
	aply88 = CalcRoute::CheckOfRule88j(&route_list_tmp2);
	if (0 != aply88) {
		if ((aply88 & 1) != 0) {
			TRACE("Apply to rule88 for start.\n");
			route_flag.ter_begin_oosaka = true;
		} else if ((aply88 & 2) != 0) {
			TRACE("Apply to rule88 for arrive.\n");
			route_flag.ter_fin_oosaka = true;
		}
        route_flag.rule88 = true;          // applied rule
	}

	/* 特定都区市内発着可否判定 */
	chk = CalcRoute::CheckOfRule86(route_list_tmp2, route_flag, &exit, &enter, &cityId);
	TRACE("RuleSpecific:chk 0x%0x, %d -> %d\n", chk, IDENT1(cityId), IDENT2(cityId));
	if (BIT_CHK(chk, 31)) {
		route_flag.jrtokaistock_enable = true; // for UI
	}
	else {
		route_flag.jrtokaistock_enable = false; // for UI
	}
	chk &= ~(1 << 31);
	/* 86, 87適用可能性None？ */
	if ((chk == 4) || (chk == 0)) {  /* 全駅特定都区市内駅 or 発着とも特定都区市内駅でない場合 */
		/* 未変換 */
		TRACE("no applied for rule86/87\n");
		route_list_cooked.assign(route_list_tmp2.cbegin(), route_list_tmp2.cend());
		return;			// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	}

	/* (発駅=都区市内 or 着駅=都区市内)
	 */
	/* route_list_tmp = route_list_tmp2 */
	route_list_tmp.assign(route_list_tmp2.cbegin(), route_list_tmp2.cend());

	/* 変換 -> route_list_tmp:86適用(仮)
	   88変換したものは対象外(=山陽新幹線 新大阪着時、非表示フラグが消えてしまうのを避ける効果あり) */
    if (route_flag.isEnableRule86or87()) {
        CalcRoute::ReRouteRule86j87j(cityId, chk & ~aply88, exit, enter, &route_list_tmp);
    }

	// 88を適用(新大阪発は大阪発に補正)
	/*aply88 = */CalcRoute::CheckOfRule88j(&route_list_tmp);
#if 0
	if (0 != aply88) {
		if ((aply88 & 1) != 0) {
			TRACE("Apply to rule88(2) for start.\n");
			route_flag &= ~LF_TER_CITY_MASK;
			BIT_ON(route_flag, BLF_TER_BEGIN_OOSAKA);
		} else if ((aply88 & 2) != 0) {
			TRACE("Apply to rule88(2) for arrive.\n");
			route_flag &= ~LF_TER_CITY_MASK;
			BIT_ON(route_flag, BLF_TER_FIN_OOSAKA);
		}
        BIT_ON(route_flag, BLF_RULE_EN);    // applied rule
	}
#endif


	// 69を適用したものをroute_list_tmp3へ
	n = CalcRoute::ReRouteRule69j(route_list_tmp, &route_list_tmp3);	/* 69条適用(route_list_tmp->route_list_tmp3) */
	TRACE("Rule 69(2) applied %dtimes.\n", n);

	/* route_list_tmp	70-88-69-86適用
	 * route_list_tmp2	70-88-69適用
	 * route_list_tmp3	70-88-69-86-69適用
	 */
	/* compute of sales_km by route_list_cooked */
	km_raw = CalcRoute::Get_route_distance(route_flag, route_list_tmp3);
			//	営業キロ[0] ／ 計算キロ[1] ／ 会社線キロ[2]　／ BRTキロ[3]
	jsales_km = km_raw[0] - km_raw[2] - km_raw[3]; // except BRT and company line
	//115-2の誤った解釈
    //km_raw = CalcRoute::Get_route_distance(route_flag, route_list_raw);
	//skm = km_raw[0] - km_raw[2];
	if ((2000 < jsales_km) /*&& ((CalcRoute::InRouteUrban(route_list_raw) != URB_TOKYO) ||
	    (2000 < skm))*/) {
		/* <<<都区市内適用>>> */
		/* 201km <= jsales_km */
		/* enable */
        route_flag.terCityReset();
        switch (chk & 0x03) {
            case 0:
                break;
            case 1:
                route_flag.rule86or87 |= 1;
                break;
            case 2:
                route_flag.rule86or87 |= 2;
                break;
            case 3:
            default:
                route_flag.rule86or87 |= 3;
                break;
        }
		TRACE("applied for rule86(%d)\n", chk & 0x03);

		// route_list_cooked = route_list_tmp3
		route_list_cooked.assign(route_list_tmp3.cbegin(), route_list_tmp3.cend());

		return;			// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	}

	/* 101km - 200km : 山手線発着 or 200km以下の都区市内間(名古屋-大阪など)チェック */
	rtky = CalcRoute::CheckOfRule87(route_list_tmp2);
	if ((3 & rtky) != 0) {
		/* apply to 87 */  /* 都区内に限り最短が100km以下は非適用(基115-2) */
		if ((1000 < jsales_km) /*&& ((CalcRoute::InRouteUrban(route_list_raw) != URB_TOKYO) ||
		    (1000 < skm))*/) {
			/* 山手線内発着 enable */
            if (!route_flag.jrtokaistock_enable || !route_flag.jrtokaistock_applied) {
                route_flag.terCityReset();
                switch (rtky & 0x03) {
                    case 0:
                        break;
                    case 1:
                        route_flag.rule86or87 |= 4;
                        break;
                    case 2:
                        route_flag.rule86or87 |= 8;
                        break;
                    case 3:
                    default:
                        route_flag.rule86or87 |= 0x0c;
                        break;
                }
				TRACE("applied for rule87\n");
			} else {
                TRACE("no applied rule87 reson the JR tokai stock enable.\n");
            }

            flg = 0;
            if (((chk & 0x01) != 0) && ((rtky & 0x01) == 0) && (CITYNO_TOKYO == IDENT1(cityId))) {
                // 都区内発で山手線内発ではない場合には、着駅が山手線内と言うことで着駅だけ東京に
                flg = 2;
            }
            if (((chk & 0x02) != 0) && ((rtky & 0x02) == 0) && (CITYNO_TOKYO == IDENT2(cityId))) {
                // 都区内着で山手線内着ではない
                flg = 1;
            }

            if (flg != 0) {
                // 要らない。(ReRouteRule86j87j()が第2引数で制御できるので）cityId = MAKEPAIR(0, IDENT2(cityId));
                /* route_list_tmp = route_list_tmp2 */
                route_list_tmp.assign(route_list_tmp2.cbegin(), route_list_tmp2.cend());
                CalcRoute::ReRouteRule86j87j(cityId, flg, exit, enter, &route_list_tmp);
                // 69を適用したものをroute_list_tmp3へ
                n = CalcRoute::ReRouteRule69j(route_list_tmp, &route_list_tmp3);	/* 69条適用(route_list_tmp->route_list_tmp3) */
                TRACE("Rule 69(2) applied %dtimes.\n", n);
                route_flag.rule69 = true;
            } else {
                // 東京,京葉線,蘇我,外房線,勝浦 -> [山]外房線,勝浦
            }
  				// route_list_cooked = route_list_tmp3
  			route_list_cooked.assign(route_list_tmp3.cbegin(), route_list_tmp3.cend());

            ///// route_flag.rule86or87 |= ((rtky | chk) & 0x03);    // applied rule

            return;			// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		}
		sk = RULE114_SALES_KM_87;	/* 80km for rule114 check */
	} else {
		/* can't apply to 87 */
		sk = RULE114_SALES_KM_86;	/* 170km for rule114 check */
	}

	// route_list_tmp4 = route_list_tmp3
	route_list_tmp4.assign(route_list_tmp3.cbegin(), route_list_tmp3.cend());

	/* 発着とも都区市内? */
	if ((0x03 & (rtky | chk)) == 3) { /* 名古屋市内-大阪市内など([名]-[阪]、[九]-[福]、[区]-[浜]) */
							/*  [区]-[区], [名]-[名], [山]-[区], ... */
        for (sk2 = 2000; true ; sk2 = 1000) {
			flg = 0;

			/* route_list_tmp = route_list_tmp2 */
			route_list_tmp.assign(route_list_tmp2.cbegin(), route_list_tmp2.cend());
			/* 発駅のみ特定都区市内着経路に変換 */
			CalcRoute::ReRouteRule86j87j(cityId, 1, exit, enter, &route_list_tmp);

			// 69を適用したものをroute_list_tmp3へ
			n = CalcRoute::ReRouteRule69j(route_list_tmp, &route_list_tmp3);	/* 69条適用(route_list_tmp->route_list_tmp3) */
			TRACE("Rule 69(3) applied %dtimes.\n", n);

			/* 発駅のみ都区市内にしても201/101km以上か？ */
			km_raw = CalcRoute::Get_route_distance(route_flag, route_list_tmp3);
			// except BRT and company line
			if (sk2 < (km_raw[0] - km_raw[2] - km_raw[3])) {
				// 発 都区市内有効
				flg = 0x01;
			}

			/* route_list_tmp = route_list_tmp2 */
			route_list_tmp.assign(route_list_tmp2.cbegin(), route_list_tmp2.cend());
			/* 着駅のみ特定都区市内着経路に変換仮適用 */
			CalcRoute::ReRouteRule86j87j(cityId, 2, exit, enter, &route_list_tmp);

			// 69を適用したものをroute_list_tmp3へ
			n = CalcRoute::ReRouteRule69j(route_list_tmp, &route_list_tmp3);	/* 69条適用(route_list_tmp->route_list_tmp3) */
			TRACE("Rule 69(4) applied %dtimes.\n", n);

			/* 着駅のみ都区市内にしても201/101km以上か？ */
			km_raw = CalcRoute::Get_route_distance(route_flag, route_list_tmp3);
			// except BRT and company line
			if (sk2 < (km_raw[0] - km_raw[2] - km_raw[3])) {
				// 着 都区市内有効
				flg |= 0x02;
			}
			if (flg == 0x03) {	/* 発・着とも200km越えだが、都区市内間は200km以下 */
                route_flag.meihan_city_enable = true;
				if (route_flag.meihan_city_flag) {
					/* 発のみ都区市内適用 */
					/* route_list_tmp = route_list_tmp2 */
					route_list_tmp.assign(route_list_tmp2.cbegin(), route_list_tmp2.cend());
					/* 発駅のみ特定都区市内着経路に変換 */
					CalcRoute::ReRouteRule86j87j(cityId, 1, exit, enter, &route_list_tmp);

					// 69を適用したものをroute_list_tmp3へ
					n = CalcRoute::ReRouteRule69j(route_list_tmp, &route_list_tmp3);	/* 69条適用(route_list_tmp->route_list_tmp3) */
					TRACE("Rule 69(5) applied %dtimes.\n", n);
                    if (0 < n) {
                        route_flag.rule69 = true;
                    }
					/* 発駅・着駅特定都区市内だが発駅のみ都区市内適用 */
					if (sk == RULE114_SALES_KM_87) {
						TRACE("applied for rule87(start)\n");
                        route_flag.terCityReset();
                        route_flag.rule86or87 |= 0x04;
					} else {
						TRACE("applied for rule86(start)\n");
                        route_flag.terCityReset();
                        route_flag.rule86or87 |= 0x01;
					}
					// route_list_cooked = route_list_tmp3
					route_list_cooked.assign(route_list_tmp3.cbegin(), route_list_tmp3.cend());
                    return;			// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
				} else {
					/* 着のみ都区市内適用 */
					/* 発駅・着駅特定都区市内だが着駅のみ都区市内適用 */
					if (sk == RULE114_SALES_KM_87) {
						TRACE("applied for rule87(end)\n");
                        route_flag.terCityReset();
                        route_flag.rule86or87 |= 0x08;
					} else {
						TRACE("applied for rule86(end)\n");
                        route_flag.terCityReset();
                        route_flag.rule86or87 = 0x02;
					}
					// route_list_cooked = route_list_tmp3
					route_list_cooked.assign(route_list_tmp3.cbegin(), route_list_tmp3.cend());
					return;			// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
				}
			} else if (flg == 0x01) {
				/* route_list_tmp = route_list_tmp2 */
				route_list_tmp.assign(route_list_tmp2.cbegin(), route_list_tmp2.cend());
				/* 発駅のみ特定都区市内着経路に変換 */
				CalcRoute::ReRouteRule86j87j(cityId, 1, exit, enter, &route_list_tmp);

				// 69を適用したものをroute_list_tmp3へ
				n = CalcRoute::ReRouteRule69j(route_list_tmp, &route_list_tmp3);	/* 69条適用(route_list_tmp->route_list_tmp3) */
				TRACE("Rule 69(6) applied %dtimes.\n", n);
                if (0 < n) {
                    route_flag.rule69 = true;
                }

				/* 発駅・着駅特定都区市内だが発駅のみ都区市内適用 */
				if (sk == RULE114_SALES_KM_87) {
					TRACE("applied for rule87(start)\n");
                    route_flag.terCityReset();
                    route_flag.rule86or87 |= 0x04;
				} else {
					TRACE("applied for rule86(start)\n");
                    route_flag.terCityReset();
                    route_flag.rule86or87 |= 0x01;
				}
				// route_list_cooked = route_list_tmp3
				route_list_cooked.assign(route_list_tmp3.cbegin(), route_list_tmp3.cend());
				return;			// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
			} else if (flg == 0x02) {
				// TODO 上のflg == 0x01の実装と違うのは何故？同じにすべきでは？？？ 名阪絡みで不要か？
				/* 発駅・着駅特定都区市内だが着駅のみ都区市内適用 */
				if (sk == RULE114_SALES_KM_87) {
					TRACE("applied for rule87(end)\n");
                    route_flag.terCityReset();
                    route_flag.rule86or87 |= 0x08;
				} else {
					TRACE("applied for rule86(end)\n");
                    route_flag.terCityReset();
                    route_flag.rule86or87 |= 0x2;
				}
				// route_list_cooked = route_list_tmp3
				route_list_cooked.assign(route_list_tmp3.cbegin(), route_list_tmp3.cend());
                route_flag.rule86or87 |= 0x02;    // applied rule   TODOこれもおかしくね？
				return;			// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
			}
			/* flg == 0 */
			if ((sk != RULE114_SALES_KM_87) || (sk2 == 1000)) {
				break;
			}
		} /* sk2= 2000, 1000 */
		/* passthru */
	} /* ((0x03 & (rtky | chk)) == 3)  名古屋市内-大阪市内など([名]-[阪]、[九]-[福]、[区]-[浜]) */

	/* route_list_tmp	x
	 * route_list_tmp2	70-88-69適用
	 * route_list_tmp3	x
	 * route_list_tmp4	70-88-69-86-69適用
	 * route_list_cooked 空
	 */

	/* 未変換 */
	TRACE("no applied for rule86/87(jsales_km=%d)\n", jsales_km);

	if (isCheckRule114 && (sk <= jsales_km)) {
			/* 114条適用かチェック */
		CalcRoute::CRule114 rule114;
		if (rule114.check(route_flag, chk, sk, route_list_tmp2, route_list_tmp4, cityId, enter, exit)) {
			rule114Info.set(Rule114Info(rule114.fare, rule114.apply_terminal_station));
		}
	} else {
		;
	}
	/* 86-87非適用 */
	// route_list_cooked = route_list_tmp2
	route_list_cooked.assign(route_list_tmp2.cbegin(), route_list_tmp2.cend());
	return;
}

//static:
//	旅客営業取扱基準規定43条の2（小倉、西小倉廻り）
//	@param [out] fare_inf  営業キロ(sales_km, kyusyu_sales_km, kyusyu_calc_km)
//
//	showFare() => calc_fare() ->
//	checkOfRule114j() => calc_fare ->
//
int32_t FARE_INFO::CheckAndApplyRule43_2j(const vector<RouteItem> &route)
{
	int32_t stage;
	int32_t c;
	int32_t rl;
	int32_t km;
	int32_t kagoshima_line_id;
	vector<RouteItem>::const_iterator ite = route.cbegin();

	stage = 0;
	c = 0;
	rl = 0;
	while (ite != route.cend()) {
		switch (stage) {
		case 0:
			if ((0 < ite->lineId) && (ite->stationId == STATION_ID(_T("西小倉")))) {
				c = 1;
				stage = 1;
			} else if (ite->stationId == STATION_ID(_T("博多"))) {
				if (ite->lineId == LINE_ID(_T("山陽新幹線"))) {
					c = 4;			// 山陽新幹線
				} else {
					c = 2;
				}
				stage = 1;
			} else if ((0 < ite->lineId) && (ite->stationId == STATION_ID(_T("吉塚")))) {
				c = 3;				// 吉塚
				stage = 1;
				break;
			}
			break;
		case 1:
			switch (c) {
			case 1:
				if ((ite->stationId == STATION_ID(_T("小倉"))) &&
				((ite + 1) != route.cend()) &&
				((ite + 1)->lineId == LINE_ID(_T("山陽新幹線"))) &&
				((ite + 1)->stationId == STATION_ID(_T("博多")))) {
					rl |= 1;	// 小倉 山陽新幹線 博多
				}
				break;
			case 2:
				if ((ite->stationId == STATION_ID(_T("小倉"))) &&
				(ite->lineId == LINE_ID(_T("山陽新幹線"))) &&
				((ite + 1) != route.cend()) &&
				((ite + 2) != route.cend()) &&	// 西小倉止まりまでなら非適用
				((ite + 1)->stationId == STATION_ID(_T("西小倉")))) {
					rl |= 1;
				}
				break;
			case 3:
				if ((ite->stationId == STATION_ID(_T("博多"))) &&
				((ite + 1) != route.cend()) &&
				((ite + 1)->lineId == LINE_ID(_T("山陽新幹線")))) {
					rl |= 2;
				}
				break;
			case 4:
				if ((ite->stationId == STATION_ID(_T("吉塚"))) &&
				((ite + 1) != route.cend())) {	// 吉塚止まりまでなら非適用
					rl |= 2;
				}
				break;
			default:
				break;
			}
			c = 0;
			stage = 0;
			break;
		default:
			ASSERT(FALSE);
			break;
		}
		ite++;
	}
	if (rl != 0) {
		km = 0;
		kagoshima_line_id = LINE_ID(_T("鹿児島線"));
		if ((rl & 1) != 0) {
			km = RouteUtil::GetDistance(kagoshima_line_id, STATION_ID(_T("小倉")), STATION_ID(_T("西小倉")))[0];
			TRACE(_T("applied 43-2(西小倉)\n"));
		}
		if ((rl & 2) != 0) {
			km += RouteUtil::GetDistance(kagoshima_line_id, STATION_ID(_T("博多")), STATION_ID(_T("吉塚")))[0];
			TRACE(_T("applied 43-2(吉塚)\n"));
		}
		return km;
	}
	return 0;
}

//static:
//	88条のチェックと変換
//	新大阪発（着）-[東海道線]-神戸-[山陽線]-姫路以遠なら、新大阪→大阪置換
//	大阪-[東海道線]-新大阪-[山陽新幹線]-
//	の場合、大阪-[東海道線]-新大阪の区間を外す（201km以上なら外さない)
//	(この関数へは201km以上の86条適用時には呼ばれることはない)
//	BLF_TER_xxx(route[0].lineId)の設定前であることを前提(0を設定)とします
//
//	@param [in/out] route    route
//
//	@retval 0: no-convert
//	@retval 1: change start
//	@retval 2: change arrived
//
int32_t CalcRoute::CheckOfRule88j(vector<RouteItem> *route)
{
	int32_t lastIndex;
	static int32_t distance_koube_himeji = 0;  // 神戸-姫路
	static int32_t distance_shinoosaka_himeji = 0; // 新大阪-姫路

	lastIndex = (int32_t)route->size() - 1;

	if (!distance_koube_himeji) {	/* chk_distance: 山陽線 神戸-姫路間営業キロ, 新幹線 新大阪-姫路 */
		distance_koube_himeji = RouteUtil::GetDistance(LINE_ID(_T("山陽線")),
                                               STATION_ID(_T("神戸")),
                                               STATION_ID(_T("姫路")))[0];
		distance_shinoosaka_himeji = RouteUtil::GetDistance(LINE_ID(_T("山陽新幹線")),
                                               STATION_ID(_T("新大阪")),
                                               STATION_ID(_T("姫路")))[0];
	}

	if (2 <= lastIndex) {
		    // 新大阪 発 東海道線 - 山陽線
		if ((route->front().stationId == STATION_ID(_T("新大阪"))) &&
			(route->at(1).lineId == LINE_ID(_T("東海道線"))) &&
		    (route->at(2).lineId == LINE_ID(_T("山陽線"))) &&
		    (distance_koube_himeji <= RouteUtil::GetDistance(LINE_ID(_T("山陽線")),
                                                     STATION_ID(_T("神戸")),
                                                     route->at(2).stationId)[0])) {

			ASSERT(route->at(1).stationId == STATION_ID(_T("神戸")));
			/*	新大阪発東海道線-山陽線-姫路以遠なら発駅を新大阪->大阪へ */
			route->front() = RouteItem(0, STATION_ID(_T("大阪")));	// 新大阪->大阪

			return 1;
		}	// 新大阪 着 山陽線 - 東海道線
		else if ((route->back().stationId == STATION_ID(_T("新大阪"))) &&
				 (route->back().lineId == LINE_ID(_T("東海道線"))) &&
				 (route->at(lastIndex - 1).lineId == LINE_ID(_T("山陽線"))) &&
		    	 (distance_koube_himeji <= RouteUtil::GetDistance(LINE_ID(_T("山陽線")),
                                                          STATION_ID(_T("神戸")),
                                                          route->at(lastIndex - 2).stationId)[0])) {

			ASSERT(route->at(lastIndex - 1).stationId == STATION_ID(_T("神戸")));
			/*	新大阪着東海道線-山陽線-姫路以遠なら着駅を新大阪->大阪へ */
			route->back() = RouteItem(LINE_ID(_T("東海道線")),
                                      STATION_ID(_T("大阪")));	// 新大阪->大阪

			return 2;
		}
		    // 大阪 発 新大阪 経由 山陽新幹線
		if ((route->front().stationId == STATION_ID(_T("大阪"))) &&
			(route->at(2).lineId == LINE_ID(_T("山陽新幹線"))) &&
			(route->at(1).stationId == STATION_ID(_T("新大阪"))) &&
			(distance_shinoosaka_himeji <= RouteUtil::GetDistance(LINE_ID(_T("山陽新幹線")),
                                                     STATION_ID(_T("新大阪")),
                                                     route->at(2).stationId)[0])) {

			ASSERT(route->at(1).lineId == LINE_ID(_T("東海道線")));

			/* 大阪発-東海道線上り-新大阪-山陽新幹線 姫路以遠の場合、大阪発-東海道線-山陽線 西明石経由に付け替える */

			route->at(1) = RouteItem(LINE_ID(_T("東海道線")),
                                     STATION_ID(_T("神戸")));
			route->at(1).flag |= (FLG_HIDE_LINE | FLG_HIDE_STATION);

			vector<RouteItem>::iterator ite = route->begin();
			ite += 2;	// at(2)						// 山陽線-西明石
			ite = route->insert(ite, RouteItem(LINE_ID(_T("山陽線")),
                                               STATION_ID(_T("西明石"))));
			ite->flag |= (FLG_HIDE_LINE | FLG_HIDE_STATION);

			return 1;
		}	// 山陽新幹線 新大阪 経由 大阪 着
		else if ((route->back().stationId == STATION_ID(_T("大阪"))) &&
				 (route->at(lastIndex - 1).stationId == STATION_ID(_T("新大阪"))) &&
				 (route->at(lastIndex - 1).lineId == LINE_ID(_T("山陽新幹線"))) &&
				 (distance_shinoosaka_himeji <= RouteUtil::GetDistance(LINE_ID(_T("山陽新幹線")),
                                                          STATION_ID(_T("新大阪")),
                                                          route->at(lastIndex - 2).stationId)[0])) {

			ASSERT(route->back().lineId == LINE_ID(_T("東海道線")));

			/* 山陽新幹線 姫路以遠～新大阪乗換東海道線-大阪着の場合、最後の東海道線-大阪 を西明石 山陽線、東海道線に付け替える */

			route->at(lastIndex - 1) = RouteItem(LINE_ID(_T("山陽新幹線")),
                                                 STATION_ID(_T("西明石")));	// 新大阪->西明石
			route->at(lastIndex - 1).flag |= FLG_HIDE_STATION;
			route->at(lastIndex).flag |= FLG_HIDE_LINE;	// 東海道線 非表示
			vector<RouteItem>::iterator ite = route->end();
			ite--;
			ite = route->insert(ite, RouteItem(LINE_ID(_T("山陽線")),
                                               STATION_ID(_T("神戸"))));
			ite->flag |= (FLG_HIDE_LINE | FLG_HIDE_STATION);

			return 2;
		}
	}
	if (1 <= lastIndex) {
		    // 新大阪 発 山陽新幹線
		if ((route->front().stationId == STATION_ID(_T("新大阪"))) &&
			(route->at(1).lineId == LINE_ID(_T("山陽新幹線"))) &&
			(distance_shinoosaka_himeji <= RouteUtil::GetDistance(LINE_ID(_T("山陽新幹線")),
                                                     STATION_ID(_T("新大阪")),
                                                     route->at(1).stationId)[0])) {

			/* 大阪発-東海道線上り-新大阪-山陽新幹線 姫路以遠の場合、大阪発-東海道線-山陽線 西明石経由に付け替える */

			vector<RouteItem>::iterator ite = route->begin();
			*ite = RouteItem(0, STATION_ID(_T("大阪")));
			ite++;
			ite = route->insert(ite, RouteItem(LINE_ID(_T("山陽線")),
                                               STATION_ID(_T("西明石"))));
			ite->flag |= (FLG_HIDE_LINE | FLG_HIDE_STATION);

			ite = route->insert(ite, RouteItem(LINE_ID(_T("東海道線")),
                                               STATION_ID(_T("神戸"))));
			ite->flag |= (FLG_HIDE_LINE | FLG_HIDE_STATION);

			return 1;

		}	// 山陽新幹線 大阪 着
		else if ((route->back().stationId == STATION_ID(_T("新大阪"))) &&
				 (route->back().lineId == LINE_ID(_T("山陽新幹線"))) &&
				 (distance_shinoosaka_himeji <= RouteUtil::GetDistance(LINE_ID(_T("山陽新幹線")),
                                                          STATION_ID(_T("新大阪")),
                                                          route->at(lastIndex - 1).stationId)[0])) {

			/* 山陽新幹線 姫路以遠～新大阪乗換東海道線-大阪着の場合、最後の東海道線-大阪 を西明石 山陽線、東海道線に付け替える */

			route->back() = RouteItem(LINE_ID(_T("山陽新幹線")),
                                      STATION_ID(_T("西明石")));	// 新大阪->西明石
			route->back().flag |= FLG_HIDE_STATION;

			route->push_back(RouteItem(LINE_ID(_T("山陽線")),
                                       STATION_ID(_T("神戸"))));	// add 山陽線-神戸
			route->back().flag |= (FLG_HIDE_LINE | FLG_HIDE_STATION);

			route->push_back(RouteItem(LINE_ID(_T("東海道線")),
                                       STATION_ID(_T("大阪"))));	// add 東海道線-大阪
			route->back().flag |= FLG_HIDE_LINE;

			return 2;
		}
	}
	return 0;
}


//static:
//	showFare() => calc_fare() =>
//	checkOfRule114j() => calc_fare() =>
//
//	89条のチェックと変換
//	北新地発（着）-[JR東西線]-尼崎なら、北新地→大阪置換
//
//	@param [in] route    route
//
//	@retval 0: no-convert
//	@retval <0: 大阪-尼崎の営業キロ - 北新地-尼崎の営業キロ(負数)
//
int32_t FARE_INFO::CheckOfRule89j(const vector<RouteItem>& route)
{
	int32_t lastIndex;
	static int32_t distance = 0;

	lastIndex = (int32_t)route.size() - 1;
	if (lastIndex < 2) {
		return 0;
	}

				// 北新地発やで
	if (((route.front().stationId == STATION_ID(_T("北新地"))) &&
		(route.at(1).stationId == STATION_ID(_T("尼崎")))) &&
		((lastIndex <= 1) || (route.at(2).lineId != LINE_ID(_T("東海道線"))) ||
			(RouteUtil::LDIR_ASC == RouteUtil::DirLine(LINE_ID(_T("東海道線")),
                                                       STATION_ID(_T("尼崎")),
                                                       route.at(2).stationId)))) {
		/* 北新地-(JR東西線)-尼崎 の場合、発駅（北新地）は大阪や */
		// route.front().stationId = STATION_ID(_T("大阪");
		if (distance == 0) {
			distance = RouteUtil::GetDistance(LINE_ID(_T("東海道線")),
                                              STATION_ID(_T("大阪")),
                                              STATION_ID(_T("尼崎")))[0] -
					   RouteUtil::GetDistance(route.at(1).lineId,
                                              STATION_ID(_T("尼崎")),
                                              STATION_ID(_T("北新地")))[0];
		}
		ASSERT(distance < 0);
		return distance;
	}			// 北新地終着やねん
	else if (((route.back().stationId == STATION_ID(_T("北新地"))) &&
			 (route.at(lastIndex - 1).stationId == STATION_ID(_T("尼崎")))) &&
			 ((lastIndex <= 1) || (route.at(lastIndex - 1).lineId != LINE_ID(_T("東海道線"))) ||
		(RouteUtil::LDIR_DESC == RouteUtil::DirLine(LINE_ID(_T("東海道線")),
                                                    route.at(lastIndex - 2).stationId,
                                                    STATION_ID(_T("尼崎")))))) {
		//route.back().stationId = STATION_ID(_T("大阪");
		if (distance == 0) {
			distance = RouteUtil::GetDistance(LINE_ID(_T("東海道線")),
                                              STATION_ID(_T("大阪")),
                                              STATION_ID(_T("尼崎")))[0] -
					   RouteUtil::GetDistance(route.back().lineId,
                                              STATION_ID(_T("尼崎")),
                                              STATION_ID(_T("北新地")))[0];
		}
		ASSERT(distance < 0);
		return distance;
	} else {
		return 0;
	}
}

CalcRoute::CRule114::CRule114()
{
	normal_fare = 0;
}

//	Rule114 check
//	@param [in] rRouteFlag	route flag(refer only)
//	@param [in] chk		86or86 applied flag.
//	@param [in] sk		86 or 87
//	@param [in] rRoute_list_no_applied_86or87
//	@param [in] rRoute_list_applied_86or87
//	@param [in] cityId
//	@param [in] enter
//	@param [in] exit   
//
bool CalcRoute::CRule114::check(const RouteFlag& rRouteFlag, uint32_t chk, uint32_t sk, 
	                      const vector<RouteItem>& rRoute_list_no_applied_86or87, 
	                      const vector<RouteItem>& rRoute_list_applied_86or87, 
						  const PAIRIDENT cityId, const Station& enter, const Station& exit)
{
	this->route_flag.setAnotherRouteFlag(rRouteFlag);

	if ((0x03 & chk) == 3) {

		route_list.assign(rRoute_list_no_applied_86or87.cbegin(), rRoute_list_no_applied_86or87.cend());
		/* 発駅のみ特定都区市内着経路に変換 */
		CalcRoute::ReRouteRule86j87j(cityId, 1, exit, enter, &route_list);

		// 69を適用したものをroute_list_special へ
		CalcRoute::ReRouteRule69j(route_list, &route_list_special);	/* 69条適用(route_list->route_list_special) */
		CalcRoute::CRule114::ConvertShinkansen2ZairaiFor114Judge(&route_list_special);

		route_list.assign(rRoute_list_no_applied_86or87.cbegin(), rRoute_list_no_applied_86or87.cend());
		CalcRoute::CRule114::ConvertShinkansen2ZairaiFor114Judge(&route_list);
							/* 86,87適用前,   86,87適用後 */
		if (!checkOfRule114j(0x01 | ((sk == RULE114_SALES_KM_86) ? 0 : 0x8000))) {
			route_list.assign(rRoute_list_no_applied_86or87.cbegin(), rRoute_list_no_applied_86or87.cend());
			/* 着駅のみ特定都区市内着経路に変換 */
			CalcRoute::ReRouteRule86j87j(cityId, 2, exit, enter, &route_list);

			// 69を適用したものをroute_list_specialへ
			CalcRoute::ReRouteRule69j(route_list, &route_list_special);	/* 69条適用(route_list->route_list_special) */
			CalcRoute::CRule114::ConvertShinkansen2ZairaiFor114Judge(&route_list_special);

			route_list.assign(rRoute_list_no_applied_86or87.cbegin(), rRoute_list_no_applied_86or87.cend());
			CalcRoute::CRule114::ConvertShinkansen2ZairaiFor114Judge(&route_list);
			return checkOfRule114j(0x02 | ((sk == RULE114_SALES_KM_86) ? 0 : 0x8000));
		}
	} else {
		route_list.assign(rRoute_list_no_applied_86or87.cbegin(), rRoute_list_no_applied_86or87.cend());
		route_list_special.assign(rRoute_list_applied_86or87.cbegin(), rRoute_list_applied_86or87.cend());
		CalcRoute::CRule114::ConvertShinkansen2ZairaiFor114Judge(&route_list);
		CalcRoute::CRule114::ConvertShinkansen2ZairaiFor114Judge(&route_list_special);
		ASSERT(((0x03 & chk) == 1) || ((0x03 & chk) == 2));
		return checkOfRule114j((chk & 0x03) | ((sk == RULE114_SALES_KM_86) ? 0 : 0x8000));
	}
	return false;
}

//static
//	並行在来線へ変換
//	114条判定用
//  都区市内駅から200kmを超える場合は結果が正しくない場合がある
//	(東北新幹線 仙台 八戸 など)この場合は前段でチェックされるので
//  ここにくることはない(114条チェック候補から外れるため)。
//
//	@return true if changed.
//
bool CalcRoute::CRule114::ConvertShinkansen2ZairaiFor114Judge(vector<RouteItem>* route)
{
	vector<RouteItem>::iterator ite = route->begin();
	int32_t station_id1 = 0;
	int32_t station_id1n = 0;
	int32_t station_id2 = 0;
	int32_t j_station_id = 0;
	int32_t z_station_id = 0;
	int32_t cline_id = 0;
	int32_t bline_id = 0;
	int32_t zline_id = 0;
	uint32_t i;
	vector<uint32_t> zline;
	vector<uint32_t> zroute;
	int32_t replace = 0;

	while (ite != route->end()) {
		station_id1n = ite->stationId;
		if ((station_id1 != 0) && IS_SHINKANSEN_LINE(ite->lineId)) {
			zline = RouteUtil::EnumHZLine(ite->lineId, station_id1, station_id1n);
//TRACE(_T("?%d?%d %d %d"), zline.size(), zline[0], zline[1], zline[2]);
			if (3 <= zline.size()) {
				// 並行在来線
				cline_id = 0;
				zline_id = 0;
				j_station_id = 0;
				zroute.clear();
				for (i = 1; i < (zline.size() - 1); i++) {
					zline_id = (0xffff & zline[i]);
					station_id2 = (zline[i] >> 16);
#if 0
if (i == 0) {
TRACE(_T("++%15s(%d)\t%s(%d)\n"), LNAME(zline_id), zline_id, SNAME(station_id2), station_id2);
} else if (i == (zline.size() - 1)) {
TRACE(_T("--%15s(%d)\t%s(%d)\n"), LNAME(zline_id), zline_id, SNAME(station_id2), station_id2);
} else {
TRACE(_T("::%15s(%d)\t%s(%d)\n"), LNAME(zline_id), zline_id, SNAME(station_id2), station_id2);
}
#endif
					if ((0 < zline_id) && (cline_id != zline_id)) {
						cline_id = zline_id;
					} else {
						zline_id = 0;
					}

					if ((zroute.size() % 2) == 0) {
						// 高崎着
						if (zline_id != 0) {
							zroute.push_back(zline_id);
							if (station_id2 != 0) {
								// 境界駅通過
								zroute.push_back(station_id2);
							}
						} else {
							// 高崎 - 新潟 /
							j_station_id = station_id2;
						}
					} else {
						if (station_id2 != 0) {
							// 境界駅通過
							zroute.push_back(station_id2);
							if (zline_id != 0) {
								zroute.push_back(zline_id);
							} else {
								// 大宮－高崎 / 0 - 高崎
							}
						} else {
							// 大宮-新潟 / 信越線 - 0
							ASSERT(zroute.back() == (0xffff & zline[i]));
						}
					}
				}
				// 上毛高原-高崎-xxや、本庄早稲田-高崎-xxはj_station_id=高崎 else j_station_id=0
				// (xxは高崎かその上毛高原か本庄早稲田)
				if (j_station_id == 0) {
					if (0 < zroute.size()) {
						bline_id = ite->lineId;
						if (0xffffffff == zline.front()) {
							z_station_id = RouteUtil::NextShinkansenTransferTermInRange(bline_id, station_id1, station_id1n);
							if (0 < z_station_id) {
								ite->stationId = z_station_id;
								ite->refresh();
								++ite;
								ite = route->insert(ite, RouteItem(zroute[0], station_id1n));
							} else {
								// 新横浜→品川、本庄早稲田→熊谷
								goto n1;
							}
						} else {
							z_station_id = 0;
							ite->lineId = zroute[0];
						}

						if (1 < zroute.size()) {
							ite->stationId = zroute[1];
							ite->refresh();
							for (i = 2; i < zroute.size() - 1; i += 2) {
								ite++;
								ite = route->insert(ite, RouteItem(zroute[i], zroute[i + 1]));
							}
							if (i < zroute.size()) {
								ite++;
								ite = route->insert(ite, RouteItem(zroute[i], station_id1n));
							}
						}
						if (0xffffffff == zline.back()) {
							station_id2 = RouteUtil::NextShinkansenTransferTermInRange(bline_id, station_id1n, station_id1);
							if (0 < station_id2) {
								if (z_station_id == station_id2) {
									// いわて沼宮内 - 新花巻
									ite = route->erase(ite - 1);
									*ite = RouteItem(bline_id, station_id1n);
									--replace;
								} else {
									ite->stationId = station_id2;
									ite->refresh();
									ite++;
									ite = route->insert(ite, RouteItem(bline_id, station_id1n));
								}
							} else {
								// 品川-新横浜
								*ite = RouteItem(bline_id, station_id1n);
								ite->refresh();
							}
						} else {
							ite->stationId = station_id1n;
							ite->refresh();
						}
						++replace;
					} else {
						ASSERT(FALSE);
					}
				} else {
#if 1
					if (0 < zroute.size()) {
						if (station_id1 == j_station_id) {
							// 高崎発
							// k
							ite->lineId = zroute[0];
							if (1 < zroute.size()) {
								ite->stationId = zroute[1];
							}
							i = 2;
						} else {
							// 高崎の隣の非在来線駅発高崎通過
							//  x, y, ag
							ite->stationId = j_station_id;
							i = 0;
						}
						ite->refresh();
						for (; i < zroute.size() - 1; i += 2) {
							ite++;
							ite = route->insert(ite, RouteItem(zroute[i], zroute[i + 1]));
						}
						if (i < zroute.size()) {
							ite++;
							ite = route->insert(ite, RouteItem(zroute[i], station_id1n));
						}
						++replace;
						// n, o
						// DO NOTHING
						// l, m,
						// DO NOTHING
					}
#else
					if (station_id1 == j_station_id) {
						// 高崎発
						if (0 < zroute.size()) {
							// k
							ite->lineId = zroute[0];
							if (1 < zroute.size()) {
								ite->stationId = zroute[1];
								ite->refresh();
								for (i = 2; i < zroute.size() - 1; i += 2) {
									ite++;
									ite = route->insert(ite, RouteItem(zroute[i], zroute[i + 1]));
								}
								if (i < zroute.size()) {
									ite++;
									ite = route->insert(ite, RouteItem(zroute[i], station_id1n));
								}
								++replace;
							} else {
								// n, o
								// DO NOTHING
							}
						} else {
							// l, m,
							// DO NOTHING
						}
					} else {
						// 高崎の隣の非在来線駅発高崎通過
						if (0 < zroute.size()) {
							//  x, y, ag
							ite->stationId = j_station_id;
							ite->refresh();
							for (i = 0; i < zroute.size() - 1; i += 2) {
								ite++;
								ite = route->insert(ite, RouteItem(zroute[i], zroute[i + 1]));
							}
							if (i < zroute.size()) {
								ite++;
								ite = route->insert(ite, RouteItem(zroute[i], station_id1n));
							}
							++replace;
						} // else // r, t, v, w
					}
#endif
				}
#if 0
TRACE(_T("\n;;%15s(%d)\t%s(%d)\n"), LNAME(route->back().lineId),
                                    route->back().lineId,
                                    SNAME(route->back().stationId),
                                    route->back().stationId);
 TRACE(_T("@"));
 for (i = 0; i < zroute.size(); i++) {
 TRACE(_T("%s, "), ((i % 2) == 0) ? LNAME(zroute[i]) : SNAME(zroute[i]));
 }
 TRACE(_T("\n"));
 if (j_station_id != 0) { TRACE(_T("/%s/\n"), SNAME(j_station_id)); }
#endif
			} else {
				// 整備新幹線
				// DO NOTHING
			}
//TRACE(_T("---------------------------------\n"));
		}
n1:
		station_id1 = station_id1n;
		ite++;
	}
	return 0 < replace;
}

//static
//public
//	114条のチェック
//
//	@param [in][ro] route_flag    経路フラグ
//	@param [in][ro] route_list         経路(86, 87変換前)
//	@param [in][ro] route_list_special  経路(86, 87変換後)
//	@param [in] kind       種別
//							bit0:発駅が特定都区市内または山手線内
//							bit1:着駅が特定都区市内または山手線内
//							bit15:OFF=特定都区市内
//							      ON =山手線内
//	@param [out] rule114   (戻り値True時のみ有効) [0]適用運賃(未適用時、0), [1]営業キロ, [2]計算キロ)
//
//	@note 86/87適用後の営業キロが200km/100km以下であること.
//
bool CalcRoute::CRule114::checkOfRule114j(int32_t kind)
{
	int32_t km;				// 100km or 200km
	int32_t aSales_km;		// 86/87 applied
	int32_t line_id;
	int32_t station_id1;
	int32_t station_id2;
	int32_t last_arrive_sales_km;

	vector<int32_t> km_raw;		// 86 or 87 適用後 [0]:営業キロ、[1]計算キロ
	vector<int32_t> km_spe;		// 86 or 87 適用後 [0]:営業キロ、[1]計算キロ

	/* 経路は乗換なしの単一路線 */
	if (route_list.size() <= 2) {
		return false;
	}

	km_raw = CalcRoute::Get_route_distance(route_flag, route_list); 			/* 経路距離 */
	ASSERT(km_raw.size() == 4);		// 営業キロ[0] ／ 計算キロ[1] ／ 会社線キロ[2]　／ BRTキロ[3]

	km_spe = CalcRoute::Get_route_distance(route_flag, route_list_special); 	/* 経路距離(86,87適用後) */
	ASSERT(km_spe.size() == 4);

	sales_km_special = km_spe.at(0) - km_spe.at(2) - km_spe.at(3); // sales_km as except BRT and company line;

	printf("checkOfRule114j: raw = %d, cook = %d (%d)\n", (km_raw.at(0) - km_raw.at(2) - km_raw.at(3)),
                                                          (km_spe.at(0) - km_spe.at(2) - km_spe.at(3)),
		((km_raw.at(0) - km_raw.at(2) - km_raw.at(3)) - ((km_spe.at(0) - km_spe.at(2) - km_spe.at(3)))));
	if (((km_raw.at(0) - km_raw.at(2) - km_raw.at(3)) - sales_km_special) < 100) {
		return false;
	}
	/* 中心駅～目的地は、180(90) - 200(100)km未満であるのが前提 */
	if ((0x8000 & kind) == 0) {
		// 200km
		km = 2000;
	} else {
		// 100km
		km = 1000;
	}

	this->is_start_city = ((kind & 1) != 0);
	this->is100km = (km == 1000);

	/* 距離があと86、87条適用距離-10kmの範囲内ではない */

	if ((kind & 1) != 0) {		/* 発駅が特定都区市内 */
		line_id = route_list.at(route_list.size() - 1).lineId;			// 着 路線 発-着
		station_id1 = route_list.at(route_list.size() - 2).stationId;
		station_id2 = route_list.at(route_list.size() - 1).stationId;
	} else if ((kind & 2) != 0) { /* 着駅が特定都区市内 */
		line_id = route_list.at(1).lineId;								// 発 路線.発-着
		station_id1 = route_list.at(1).stationId;
		station_id2 = route_list.at(0).stationId;
	} else {
		ASSERT(FALSE);
		return false;					// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	}

	// ex. 国母-横浜-長津田の場合、身延線.富士-国母 間の距離を引く
	//                            | 富士         国母
	// City ------------||--------+------------/ Arrive
	//  +--------------------------------------/ sales_km_special
	//  |                         +------------/ last_arrive_sales_km
	//  +-------------------------| aSalesKm               
	last_arrive_sales_km = RouteUtil::GetDistance(line_id, station_id1, station_id2).at(0);
	aSales_km = sales_km_special - last_arrive_sales_km;	/* 発駅から初回乗換駅までの営業キロを除いた営業キロ */

	if (aSales_km < 0) {
		return false;					// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	}
	ASSERT(0 <= aSales_km);
	ASSERT(0 < last_arrive_sales_km);

	if (RouteUtil::LDIR_ASC != RouteUtil::DirLine(line_id, station_id1, station_id2)) {
		/* 上り */
		km = -km;
	}
	/* 中心駅から目的地方向に最初に200(100)kmに到達する駅を得る */
	/* 富士から身延線で甲府方向に */
	route_list_replace.clear();
	collectCheckedJunction.clear();
	deep_count = 0;
	/* (86, 87条適用前されなかった)運賃その駅までの運賃より高ければ、その駅までの運賃とする */
	get86or87firstPoint(km, aSales_km, line_id, station_id1);
	if (fare.fare != 0) {
		return true;
	} else {
		TRACE("Rule 114 no applied\n");
		return false;
	}
}

// 運賃計算して比較する
// 最安運賃更新
//
//	@param [in] arrive_station_id	    last(arrive) station
//	@param [in] base_line_id    last line
//	@param [in] base_station_id last start station
//	@retval applied/non-applied of rule114
//
void CalcRoute::CRule114::judgementOfFare(int32_t arrive_station_id, int32_t base_line_id, int32_t base_station_id)
{
	FARE_INFO fi;
	int32_t fare_applied;
	
	RouteFlag rRoute_flag_ref;
	rRoute_flag_ref.setAnotherRouteFlag(route_flag);

	/* 通常運賃を得る */
	if (normal_fare == 0) {
		if (!fi.calc_fare(&rRoute_flag_ref, route_list)) {
			ASSERT(FALSE);
			return ;					// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
		}
		normal_fare = fi.getFareForJR(); /* 200(100)km以下により, 86 or 87 非適用の通常計算運賃(長津田-横浜-国母) */
		locost_fare = normal_fare;
	}

	vector<RouteItem> route_work;	// <- route_list_special
	route_work.assign(route_list_special.cbegin(), route_list_special.cend());

	if (is_start_city) {			/* 発駅が特定都区市内 */
		/* 最終着駅を置き換える */
#if defined _DEBUG
		printf("@@@ Down %lu\n",route_list_replace.size());
		for (int i = 0; i < (int)route_work.size(); i++ ) {
			printf("  D(%d)route[%s-%s]\n", i, LNAME(route_work.at(i).lineId), SNAME(route_work.at(i).stationId));
		}
		for (int i = 0; i < (int)route_list_replace.size(); i++ ) {
			printf("  D(%d)strage[%s-%s]\n", i, LNAME(route_list_replace.at(i).lineId), SNAME(route_list_replace.at(i).stationId));
		}
#endif
		route_work.pop_back();
		for (int i = 0; i < (int)route_list_replace.size(); i++) {
			if (i < (route_list_replace.size() - 1)) {
				route_work.push_back(RouteItem(route_list_replace.at(i).lineId,
 			                                   route_list_replace.at(i + 1).stationId));
			} else {
				route_work.push_back(RouteItem(route_list_replace.at(i).lineId,
			                               base_station_id));
			}
		}
		route_work.push_back(RouteItem(base_line_id, arrive_station_id));
	} else {	/* 着駅が特定都区市内 */
#if defined _DEBUG
		printf("@@@ Up %lu\n",route_list_replace.size());
		for (int i = 0; i < (int)route_work.size(); i++ ) {
			printf("  U(%d)route[%s-%s]\n", i, LNAME(route_work.at(i).lineId), SNAME(route_work.at(i).stationId));
		}
		for (int i = 0; i < (int)route_list_replace.size(); i++ ) {
			printf("  U(%d)strage[%s-%s]\n", i, LNAME(route_list_replace.at(i).lineId), SNAME(route_list_replace.at(i).stationId));
		}
#endif
		/* 始発駅を置き換える */
		route_work.front().stationId = arrive_station_id;
		for (int i = 1; i < route_list_replace.size(); i++) {
			route_work.insert(route_work.begin(), route_work.front());
			route_work.at(1).stationId = route_list_replace.at(i).stationId;
			route_work.at(1).lineId = route_list_replace.at(i).lineId;
		}
		route_work.insert(route_work.begin(), route_work.front());
		route_work.at(1).stationId = base_station_id;
		route_work.at(1).lineId = base_line_id;
	}
#if defined _DEBUG
	for (int i = 0; i < (int)route_work.size(); i++ ) {
		printf("  >>[%s-%s]\n", LNAME(route_work.at(i).lineId), SNAME(route_work.at(i).stationId));
	}
#endif
	/* 86,87適用した最短駅の運賃を得る(上例では甲斐住吉-横浜間) */
	if (!fi.calc_fare(&rRoute_flag_ref, route_work)) {
		ASSERT(FALSE);
		return ;					// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	}
	fare_applied = fi.getFareForJR();			/* より遠い駅までの都区市内発着の仮適用運賃(横浜-甲斐住吉) */

	if (fare_applied < normal_fare) {
		/* 114条適用 */
		TRACE("Rule 114 Applied(%d->%d)\n", normal_fare, fare_applied);
		TRACE("      saleskm=%d / calckm=%d\n", fi.getJRSalesKm(), fi.getJRCalcKm());

		TRACE("    *** update lowcost %d-> %d ***\n", locost_fare, fare_applied);
		if (locost_fare != normal_fare && locost_fare != fare_applied) {
			// ここに来ることはないので（若松-佐伯除く）、114運賃は通常一つ検出できれば以降検索しなくても良い。
			ASSERT(FALSE);
		}
		if ((0 == fare.sales_km) || (fi.getJRSalesKm() < fare.sales_km)) {
			locost_fare = fare_applied;
			fare.fare = fare_applied;		/* 先の駅の86,87適用運賃 */
			fare.sales_km = fi.getJRSalesKm();
			fare.calc_km = fi.getJRCalcKm();
			apply_terminal_station = arrive_station_id;
		}
	}
}

//	駅の所属する路線一覧を返す（新幹線除く）
//
vector<int32_t> CalcRoute::CRule114::ArrayOfLinesOfStationId(int32_t junction_station_id)
{
	vector<int32_t> results;

	DBO lines = RouteUtil::Enum_line_of_stationId(junction_station_id);

	if (lines.isvalid()) {
		while (lines.moveNext()) {
			int lflg = lines.getInt(2);
			if (((1<<31) & lflg) == 0) {
				int line_id = lines.getInt(1);
				if (!IS_SHINKANSEN_LINE(line_id) && !IS_COMPANY_LINE(line_id)
				   && !IS_BRT_LINE(line_id)) {
					results.push_back(line_id);
				}
			}
		}
	} else {
		TRACE("Could execute query Enum_line_of_stationId()\n");
		ASSERT(FALSE);
	}
	return results;	
}

//	指定路線・駅からの101/201km到達地点の駅を得る
//	checkOfRule114j() =>
//
//	@param [in] cond_km  下り: 100=1000, 200=2000(上り: -1000 / -2000)
//	@param [in] base_sales_km     都区市内駅からみて最後の路線の起点駅までの営業キロ
//                                都区市内着の場合、最初の路線の降車駅から都区市内駅までの営業キロ
//	@param [in] base_line_id      都区市内駅からみて最後の路線
//	@param [in] base_station_id   都区市内駅から最後の路線の起点駅
//	member: is100km, is_start_city
//	@retval なし
//
void CalcRoute::CRule114::get86or87firstPoint(int32_t cond_km, uint32_t base_sales_km, uint32_t base_line_id, uint32_t base_station_id)
{
	vector<IntPair> junctions;

	deep_count++;
	TRACE(_T("[get86or87firstPoint]: dept%d: cond_km=%d, base km=%d, %s %s @@@\n"), deep_count, cond_km, base_sales_km, LNAME(base_line_id), SNAME(base_station_id));
	
	int32_t arrive8687_station_id = retreive_SpecificCoreAvailablePoint(cond_km, base_sales_km, base_line_id, base_station_id);
  	if (0 != arrive8687_station_id) {  // with junction もあり得るのでelseにしていない
		// 運賃計算して比較する
		// 最安運賃更新
		TRACE(_T("judgementOfFare(%s, %s, %s)\n"), SNAME(arrive8687_station_id), LNAME(base_line_id), SNAME(base_station_id));
		judgementOfFare(arrive8687_station_id, base_line_id, base_station_id);
  	}
	// 100/200km までの分岐駅一覧
#if !defined C114NOFASTJUNCCHEK
	if (deep_count == 1) {
		// 井原市 芸備線 広島 山陽線 幡生
		// enumJunctionRange start:山陽線-櫛ケ浜 cond_km=2000, base_salles_km=851(広島〜櫛ケ浜)
		// 幡生から先200
		// base_station_id ではなく、幡生から先の200未満までの分岐駅

		// 広島-幡生 1982=sales_km_special base_sales 1131 	
		int arrive_station_id;
		if (is_start_city) {
			arrive_station_id = route_list_special.back().stationId;   // 幡生
		} else {
			arrive_station_id = route_list_special.front().stationId;  // 
		}
		if (STATION_IS_JUNCTION(arrive_station_id)) {
			TRACE(_T("  terminal was junction. sales_km_special:%d from:%s\n"), sales_km_special, SNAME(arrive_station_id));

			IntPair v(sales_km_special, arrive_station_id);
			junctions.push_back(v);
			base_sales_km = 0;
		} else {
			TRACE(_T("  terminal was non-junction. sales_km_special:%d from:%s\n"), sales_km_special, SNAME(arrive_station_id));
			junctions = enumJunctionRange(cond_km, sales_km_special, base_line_id, arrive_station_id);
			base_sales_km = sales_km_special;
		}
	  //       幡生から(2000-1982=18)kmまでの分岐駅
	} else {
#endif
		junctions = enumJunctionRange(cond_km, base_sales_km, base_line_id, base_station_id);
#if !defined C114NOFASTJUNCCHEK
	}
#endif
	for (vector<IntPair>::const_iterator jct_ite = junctions.cbegin(); jct_ite != junctions.cend(); jct_ite++) {
		int32_t offset_sales_km = jct_ite->one + base_sales_km;
		int32_t last_station_id = jct_ite->two;
		TRACE(_T("Enum_line_of_stationId start %s from %s-%s:\n"), SNAME(last_station_id), LNAME(base_line_id), SNAME(base_station_id));
		if (collectCheckedJunction.find(last_station_id) == collectCheckedJunction.end()) {
			collectCheckedJunction[last_station_id] = last_station_id;
			// 分岐駅から分かれる路線一覧
			// DBO lines = RouteUtil::Enum_line_of_stationId(last_station_id);
			vector<int32_t> lines = ArrayOfLinesOfStationId(last_station_id);
			for (vector<int32_t>::const_iterator ite = lines.cbegin(); ite != lines.cend(); ite++ ) {
				int32_t jct_line_id = *ite;
				TRACE(_T("found junction:%s(%s)\n"), LNAME(jct_line_id), SNAME(last_station_id));
				if (base_line_id != jct_line_id) {
					route_list_replace.push_back(RouteItem(base_line_id, base_station_id)); // 1st station will not used.
					get86or87firstPoint(cond_km, offset_sales_km, jct_line_id, last_station_id);
					get86or87firstPoint(-cond_km, offset_sales_km, jct_line_id, last_station_id);
					route_list_replace.pop_back();
				}
			}
		}
	}
	deep_count--;
}

//	指定路線・駅から、指定方面へ100/200kmまでの分岐駅を得る
//	@param [in]	cond_km		上り ／ 下り, 86(200km) or 87(100km)
//	@param [in] base_sales_km 86,87条中心駅からの指定駅までの営業キロ
//	@param [in] base_line_id 指定路線
//	@param [in] base_station_id 指定駅
//	@retval DBO(0:int(指定駅からの営業キロ), 1:int(分岐駅))
//
vector<IntPair> CalcRoute::CRule114::enumJunctionRange(int32_t cond_km, int32_t base_sales_km, int32_t base_line_id, int32_t base_station_id)
{
	// 上り方向
	static const char tsql_desc[] =
"select l1.sales_km-l2.sales_km, l2.station_id from t_lines l1 left join t_lines l2 on l1.line_id=l2.line_id"
" left join t_station t on t.rowid=l2.station_id "
"where l1.line_id=?1 and l1.station_id=?2 and l1.sales_km>l2.sales_km and (l1.sales_km-%u)<=l2.sales_km"
" and (l2.lflg&(1<<17))=0 and (l2.lflg&(1<<31))=0 and (l2.lflg&(1<<15))!=0 and (sflg&(1<<12))!=0"
" order by l2.sales_km desc";

	// 下り方向
	static const char tsql_asc[] =
"select l2.sales_km-l1.sales_km, l2.station_id from t_lines l1 left join t_lines l2 on l1.line_id=l2.line_id"
" left join t_station t on t.rowid=l2.station_id "
"where l1.line_id=?1 and l1.station_id=?2 and l1.sales_km<l2.sales_km and (l1.sales_km+%u)>=l2.sales_km"
" and (l2.lflg&(1<<17))=0 and (l2.lflg&(1<<31))=0 and (l2.lflg&(1<<15))!=0 and (sflg&(1<<12))!=0"
" order by l2.sales_km";

	char sql_buf[512];
	vector<IntPair> result;

	if (cond_km < 0) {
		/* 上り */
		sqlite3_snprintf(sizeof(sql_buf), sql_buf, tsql_desc, -cond_km - base_sales_km);
	} else {		/* 下り */
		sqlite3_snprintf(sizeof(sql_buf), sql_buf, tsql_asc, cond_km - base_sales_km);
	}
	TRACE(_T("enumJunctionRange start:%s-%s cond_km=%d, base_salles_km=%d\n"), LNAME(base_line_id), SNAME(base_station_id), cond_km, base_sales_km);

	DBO dbo = DBS::getInstance()->compileSql(sql_buf);
	dbo.setParam(1, base_line_id);
	dbo.setParam(2, base_station_id);

	while (dbo.moveNext()) {
		IntPair value;
		value.one = dbo.getInt(0);
		value.two = dbo.getInt(1);
		TRACE(_T("enumJunctionRange found %d %s\n"), value.one, SNAME(value.two));
		result.push_back(value);
	}
	return result;
}

//	指定路線・駅からの100/200km到達地点の駅を得る
//	checkOfRule114j() =>
//
//	@param [in] km  下り: 100=1000, 200=2000(上り: -1000 / -2000)
//	@param [in] line_id   路線
//	@param [in] station_id   起点駅
//	@retval 到達駅id. 0はなし
//
int32_t CalcRoute::CRule114::retreive_SpecificCoreAvailablePoint(int32_t cond_km, int32_t km_offset, int32_t line_id, int32_t station_id)
{
	// 上り方向で200km到達地点
	static const char tsql_desc[] =
	"select station_id from t_lines where line_id=?1 and (lflg&(1<<17))=0 and (lflg&(1<<31))=0 and sales_km<"
	"(select sales_km-%u from t_lines where line_id=?1 and station_id=?2) order by sales_km desc limit 1";

	// 下り方向で200km到達地点
	static const char tsql_asc[] =
	"select station_id from t_lines where line_id=?1 and (lflg&(1<<17))=0 and (lflg&(1<<31))=0 and sales_km>"
	"(select sales_km+%u from t_lines where line_id=?1 and station_id=?2) order by sales_km limit(1)";

	char sql_buf[512];

	if (cond_km < 0) {
		cond_km = -cond_km;	/* 上り */
		sqlite3_snprintf(sizeof(sql_buf), sql_buf, tsql_desc, cond_km - km_offset);
	} else {		/* 下り */
		sqlite3_snprintf(sizeof(sql_buf), sql_buf, tsql_asc, cond_km - km_offset);
	}
	//int32_t akm;
	int32_t aStationId;
	DBO dbo(DBS::getInstance()->compileSql(sql_buf));

	dbo.setParam(1, line_id);
	dbo.setParam(2, station_id);

	if (dbo.moveNext()) {
		aStationId = dbo.getInt(0);
		return aStationId;
	}
	return 0;
}


//static
//	路線は新幹線の在来線か?
//
//	@param [in] line_id1		比較元路線(在来線)
//	@param [in] line_id2		路線(新幹線)
//	@param [in] station_id1		駅1 (発) 在来線接続駅であること
//	@param [in] station_id2		駅2 (着) 在来線接続駅でなくても可
//
//	@return	true 並行在来線
//
bool Route::IsAbreastShinkansen(int32_t line_id1, int32_t line_id2, int32_t station_id1, int32_t station_id2)
{
	int32_t i;
	int32_t w;

	if (!IS_SHINKANSEN_LINE(line_id2)) {
		return false;
	}
	vector<uint32_t> hzl = RouteUtil::EnumHZLine(line_id2, station_id1, station_id2);
	if (hzl.size() < 3) {
		if (hzl.size() < 1) {
			ASSERT(FALSE);
		} else {
			if (hzl[0] == (uint32_t)line_id1) {
				return 0 < InStationOnLine(line_id1, station_id2);
			}
		}
		return false;
	}
	for (i = 0; i < (int32_t)hzl.size(); i++) {
		if (0x10000 < hzl[i]) {
			w = 0xffff & hzl[i];
		} else {
			w = hzl[i];
		}
		if (w == line_id1) {
			return true;
		} else if (0 != w) {
			return false;
		}
	}
	return false;
}


//static
//	並行在来線を得る
//
//	@param [in] line_id     路線(新幹線)
//	@param [in] station_id	駅(並行在来線駅(新幹線接続駅)
//	@param [in] (optional)station_id2 至駅(方向)
//	@retval not 0 並行在来線
//	@retval 0xffff 並行在来線は2つあり、その境界駅である(上越新幹線 高崎)
//
//
int32_t RouteUtil::GetHZLine(int32_t line_id, int32_t station_id, int32_t station_id2 /* =-1 */)
{
	int32_t i;
	int32_t w;
	vector<uint32_t> hzl = RouteUtil::EnumHZLine(line_id, station_id, station_id2);

	if (hzl.size() < 3) {
		//ASSERT(FALSE);
		return 0;
	}
	for (i = 0; i < (int32_t)hzl.size(); i++) {
		if (0x10000 < hzl[i]) {
			w = 0xffff & hzl[i];
		} else {
			w = hzl[i];
		}
		if (0 != w) {
			return w;
		}
		/* 着駅までは関知しない */
	}
	return 0;

	// 山陽新幹線 新大阪 姫路 は東海道線と山陽線だが東海道線を返す
	// 山陽新幹線 姫路 新大阪なら山陽線を返す
}



vector<uint32_t> RouteUtil::EnumHZLine(int32_t line_id, int32_t station_id, int32_t station_id2)
{
// 新幹線-並行在来線取得クエリ
const char tsql_hzl[] =
	"select case when(select line_id from t_hzline where rowid=("
	"	select ((lflg>>19)&15) from t_lines where line_id=?1 and station_id=?2)) > 0 then"
    "(select line_id from t_hzline where rowid=("
	"	select ((lflg>>19)&15) from t_lines where line_id=?1 and station_id=?2))"
    " else 0 end,"
    "(select count(*) from t_lines where line_id=?1 and station_id=?2 and 0=(lflg&((1<<31)|(1<<17))))"
	" union all"
	" select distinct line_id, 0 from t_hzline h join ("
	"	select (lflg>>19)&15 as x from t_lines"
	"	where ((lflg>>19)&15)!=0 and (lflg&((1<<31)|(1<<17)))=0	and line_id=?1 and "
	"	case when (select sales_km from t_lines where line_id=?1 and station_id=?2)<"
	"	          (select sales_km from t_lines where line_id=?1 and station_id=?3)"
	"	then"
	"	sales_km>=(select sales_km from t_lines where line_id=?1 and station_id=?2) and"
	"	sales_km<=(select sales_km from t_lines where line_id=?1 and station_id=?3) "
	"	else"
	"	sales_km<=(select sales_km from t_lines where line_id=?1 and station_id=?2) and"
	"	sales_km>=(select sales_km from t_lines where line_id=?1 and station_id=?3)"
	"	end"
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
	" end asc"
	") as y on y.x=h.rowid "
	" union all"
	" select case when(select line_id from t_hzline where rowid=("
	"	select ((lflg>>19)&15) from t_lines where line_id=?1 and station_id=?3)) > 0 then"
    " (select line_id from t_hzline where rowid=("
	"	select ((lflg>>19)&15) from t_lines where line_id=?1 and station_id=?3))"
    " else 0 end,   "
    " (select count(*) from t_lines where line_id=?1 and station_id=?3 and 0=(lflg&((1<<31)|(1<<17))))";
/*
 名古屋－＞新横浜の場合
 	東海道線	1
 	東海道線	0
 	0			1

 新潟->大宮の場合
   信越線	1
   信越線	0
   信越線/宮内	0	loword(信越線) + hiword(宮内)
   上越線	0
   0/高崎	0		loword(0) + hiword(高崎)
   高崎線	0
   高崎線	1

   (着駅、発駅がどそっぽの駅の場合(新幹線にない駅）、
   0		0
   となる
*/

	vector<uint32_t> rslt;
	int32_t lineId;
	int32_t flg;

	DBO dbo(DBS::getInstance()->compileSql(tsql_hzl));

	ASSERT(IS_SHINKANSEN_LINE(line_id));

	dbo.setParam(1, line_id);
	dbo.setParam(2, station_id);
	dbo.setParam(3, station_id2);

	while (dbo.moveNext()) {
		lineId = dbo.getInt(0);
		flg = dbo.getInt(1);
		if ((flg == 1) && (lineId == 0)) {
			lineId = -1;	/* 新幹線駅だが在来線接続駅でない */
		} /* else if ((flg == 0) && (lineId == 0)) 不正(新幹線にない駅) */
		rslt.push_back(lineId);
	}
	return rslt;
}


//static
//	新幹線乗換可否のチェック
//	add() =>
//
//	@param [in] line_id1  前路線
//	@param [in] line_id2  今路線
//	@param [in] station_id1  前回接続(発)駅
//	@param [in] station_id2  接続駅
//	@param [in] station_id3  着駅
//
//	@retval 0: N/A(OK)
//	@retval -1: NG
//	@retval 1<: 新幹線接続駅の（乗ってきた、乗る予定の）次の在来線接続駅
//                 国府津 s1                東京
// l1 東海道線     小田原 s2 東海道新幹線   静岡
// l2 東海道新幹線 名古屋 s3 東海道線       草薙
//
int Route::CheckTransferShinkansen(int32_t line_id1, int32_t line_id2, int32_t station_id1, int32_t station_id2, int32_t station_id3)
{
	int32_t bullet_line;
	int32_t local_line;
	int32_t dir;
	int32_t hzl;
    int32_t flgbit;
    int32_t opposite_bullet_station;

	if (IS_SHINKANSEN_LINE(line_id2)) {
		bullet_line = line_id2;		// 在来線->新幹線乗換
		local_line = line_id1;
		opposite_bullet_station = station_id3;
		hzl = RouteUtil::GetHZLine(bullet_line, station_id2, station_id3);

	} else if (IS_SHINKANSEN_LINE(line_id1)) {
		bullet_line = line_id1;		// 新幹線->在来線乗換
		local_line = line_id2;
		hzl = RouteUtil::GetHZLine(bullet_line, station_id2, station_id1);
		opposite_bullet_station = station_id1;

	} else {
		return 0;				// それ以外は対象外
	}
	if (local_line != hzl) {
		return 0;
	}

	// table.A
	dir = RouteUtil::DirLine(line_id1, station_id1, station_id2);
	if (dir == RouteUtil::DirLine(line_id2, station_id2, station_id3)) {
		return 0;		// 上り→上り or 下り→下り
	}
	if (dir == RouteUtil::LDIR_ASC) {	// 下り→上り
        flgbit = 0x01;
    } else {
        flgbit = 0x02;
    }
	if (((RouteUtil::AttrOfStationOnLineLine(local_line, station_id2) >> BSRSHINKTRSALW) & flgbit) != 0) {
		int chk_station = RouteUtil::NextShinkansenTransferTerm(bullet_line, station_id2, opposite_bullet_station);
		TRACE(_T("shinzai: %s -> %s, %s(%d)\n"), SNAME(station_id2), SNAME(opposite_bullet_station), SNAME(chk_station), chk_station);
		ASSERT(0 < chk_station);
        return chk_station;
    } else {
        return -1;
    }
}


//static
//	新幹線の駅1から駅2方面の隣の在来線接続駅を返す
//
//	@param [in] line_id1  新幹線
//	@param [in] station_id1  駅1
//	@param [in] station_id2  駅2(方向)
//
//	@return 駅id 0を返した場合、隣駅は駅2またはそれより先の駅
//
int32_t RouteUtil::NextShinkansenTransferTermInRange(int32_t line_id, int32_t station_id1, int32_t station_id2)
{
	const static char tsql[] =
	"select station_id from t_lines where line_id=?1 and"
	" case when"
	"(select sales_km from t_lines where line_id=?1 and station_id=?3)<"
	"(select sales_km from t_lines where line_id=?1 and station_id=?2) then"
	" sales_km=(select max(sales_km) from t_lines where line_id=?1 and"
	"	((lflg>>19)&15)!=0 and (lflg&((1<<17)|(1<<31)))=0 and"
	"	sales_km<(select sales_km from t_lines where line_id=?1 and station_id=?2) and"
	"	sales_km>(select sales_km from t_lines where line_id=?1 and station_id=?3))"
	" else"
	" sales_km=(select min(sales_km) from t_lines where line_id=?1 and"
	"	((lflg>>19)&15)!=0 and (lflg&((1<<17)|(1<<31)))=0 and"
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

int32_t RouteUtil::NextShinkansenTransferTerm(int32_t line_id, int32_t station_id1, int32_t station_id2)
{
	const static char tsql[] =
	"select station_id from t_lines where line_id=?1 and"
	" case when"
	"(select sales_km from t_lines where line_id=?1 and station_id=?3)<"
	"(select sales_km from t_lines where line_id=?1 and station_id=?2) then"
	" sales_km=(select max(sales_km) from t_lines where line_id=?1 and"
	"	((lflg>>19)&15)!=0 and (lflg&((1<<17)|(1<<31)))=0 and"
	"	sales_km<(select sales_km from t_lines where line_id=?1 and station_id=?2))"
	" else"
	" sales_km=(select min(sales_km) from t_lines where line_id=?1 and"
	"	((lflg>>19)&15)!=0 and (lflg&((1<<17)|(1<<31)))=0 and"
	"	sales_km>(select sales_km from t_lines where line_id=?1 and station_id=?2))"
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
//	両隣の分岐駅を得る(非分岐駅指定、1つか2つ)
//	changeNeerest() =>
//
//	@param [in] station_id    駅(非分岐駅)
//	@return PAIRIDENT[2] or PAIRIDENT[1] 近隣の分岐駅(盲腸線の場合1、以外は2)
//			IDENT1: station_id, IDENT2: calc_km
//
vector<PAIRIDENT> RouteUtil::GetNeerNode(int32_t station_id)
{
	vector<PAIRIDENT> result;
	DBO dbo = RouteUtil::Enum_neer_node(station_id);
	while (dbo.moveNext()) {
		int32_t stationId = dbo.getInt(0);
		int32_t cost = dbo.getInt(1);
		result.push_back(MAKEPAIR(stationId, cost));
	}
	return result;
}

//static
//	二つの駅は、同一ノード内にあるか
//
//	@param [in] line_id    line
//	@param [in] station_id1  station_id 1
//	@param [in] station_id2  station_id 2
//	@return true : 同一ノード
//	@return fase : 同一ノードではない
//
bool Route::IsSameNode(int32_t line_id, int32_t station_id1, int32_t station_id2)
{
	const static char tsql[] =
	" select count(*)"
	" from t_lines"
	" where line_id=?1"
	" and (lflg&((1<<31)|(1<<17)|(1<<15)))=(1<<15)"
	" and sales_km>"
	" 		(select min(sales_km)"
	" 		from t_lines"
	" 		where line_id=?1"
	" 		and (station_id=?2 or"
	" 			 station_id=?3))"
	" and sales_km<"
	" 		(select max(sales_km)"
	" 		from t_lines"
	" 		where line_id=?1"
	" 		and (station_id=?2 or"
	" 			 station_id=?3));";

	DBO dbo(DBS::getInstance()->compileSql(tsql));

	dbo.setParam(1, line_id);
	dbo.setParam(2, station_id1);
	dbo.setParam(3, station_id2);

	if (dbo.moveNext()) {
		return dbo.getInt(0) <= 0;
	}
	return false;
}

//static
//	路線の駅1から駅2方向の最初の分岐駅
//  (neerestで使おうと思ったが不要になった-残念)
//	@param [in] line_id    line
//	@param [in] station_id1  station_id 1(from)
//	@param [in] station_id2  station_id 2(to)
//	@return station_id : first function(contains to station_id1 or station_id2)
//
int32_t Route::NeerJunction(int32_t line_id, int32_t station_id1, int32_t station_id2)
{
	const static char tsql[] =
"select station_id from t_lines where line_id=?1 and"
" case when"
" (select sales_km from t_lines where line_id=?1 and station_id=?3)<"
" (select sales_km from t_lines where line_id=?1 and station_id=?2) then"
" sales_km=(select max(sales_km) from t_lines where line_id=?1 and (lflg&((1<<17)|(1<<31)))=0 and"
" sales_km<=(select sales_km from t_lines where line_id=?1 and station_id=?2) and (lflg&((1<<17)|(1<<31)|(1<<15)))=(1<<15))"
" else"
" sales_km=(select min(sales_km) from t_lines where line_id=?1 and (lflg&((1<<17)|(1<<31)))=0 and "
" sales_km>=(select sales_km from t_lines where line_id=?1 and station_id=?3) and (lflg&((1<<17)|(1<<31)|(1<<15)))=(1<<15))"
" end;";

	DBO dbo(DBS::getInstance()->compileSql(tsql));

	dbo.setParam(1, line_id);
	dbo.setParam(2, station_id1);
	dbo.setParam(3, station_id2);

	if (dbo.moveNext()) {
		return dbo.getInt(0);
	}
	return 0;
}

//public:
//
//	最短経路に変更(raw immidiate)
//
//	@param [in] useBulletTrain 0 在来線のみ
//                             1 新幹線を利用
//                             2 会社線を利用
//                             3 新幹線も会社線も利用
//                           100 地方交通線を覗く（在来線のみ)
//
//
//	@retval true success
//	@retval 1 : success
//	@retval 0 : loop end.
//	@retval 4 : already routed
//	@retval 5 : already finished
//	@retval -n: add() error(re-track)
//	@retval -32767 unknown error(DB error or BUG)
//
class Dijkstra	{
	struct NODE_JCT {
		int32_t minCost;
		IDENT fromNode;
		bool done_flg;
		IDENT line_id;
	} *d;
public:
	Dijkstra() {
		int i;
		d = new NODE_JCT [MAX_JCT];
		/* ダイクストラ変数初期化 */
		for (i = 0; i < MAX_JCT; i++) {
			d[i].minCost = -1;
			d[i].fromNode = 0;
			d[i].done_flg = false;
			d[i].line_id = 0;
		}
	}
	~Dijkstra() {
		delete [] d;
	}
	void setMinCost(int index, int32_t value) { d[index].minCost = value; }
	void setFromNode(int index, IDENT value) { d[index].fromNode = value; }
	void setDoneFlag(int index, bool value) { d[index].done_flg = value; }
	void setLineId(int index, IDENT value) { d[index].line_id = value; }

	int32_t minCost(int index) { return d[index].minCost; }
	IDENT fromNode(int index) { return d[index].fromNode; }
	bool doneFlag(int index) { return d[index].done_flg; }
	IDENT lineId(int index) { return d[index].line_id; }
};

int32_t Route::changeNeerest(uint8_t useBulletTrain, int end_station_id)
{
	ASSERT(0 < departureStationId());
	//ASSERT(0 < end_station_id);
	//ASSERT(departureStationId() != end_station_id);

	IDENT startNode;
	IDENT lastNode = 0;
	IDENT excNode1 = 0;
	IDENT excNode2 = 0;
	IDENT lastNode1 = 0;
	IDENT lastNode1_distance = 0;
	IDENT lastNode2 = 0;
	IDENT lastNode2_distance = 0;
	int32_t i;
	bool loopRoute;
	//int32_t km;
	Dijkstra dijkstra;
	int32_t a = 0;
	int32_t b = 0;
	int32_t doneNode;
	int32_t cost;
	int32_t id;
	int32_t lid;
	int32_t stationId;
	int32_t nLastNode;
	vector<PAIRIDENT> neer_node;
    bool except_local;

    if (useBulletTrain == 100) {
        useBulletTrain = 0;
        except_local = true;
    } else {
        except_local = false;
    }
	/* 途中追加か、最初からか */
	if (1 < route_list_raw.size()) {
		do {
			stationId = route_list_raw.back().stationId;
		    if (0 == Route::Id2jctId(stationId)) {
		    	removeTail();
				route_flag.jctsp_route_change = true;	/* route modified */
			} else {
		    	break;
		    }
		} while (1 < route_list_raw.size());

		stationId = route_list_raw.back().stationId;

	} else {
		stationId = departureStationId();
	}

	if ((stationId == end_station_id) || (end_station_id <= 0)) {
		return 4;			/* already routed */
	}

    if (route_flag.end) {
		return 5;	/* already finished */
    }

	// dijkstra initial

	startNode = Route::Id2jctId(stationId);
	lastNode = Route::Id2jctId(end_station_id);
	if (startNode == 0) { /* 開始駅は非分岐駅 */
		// 駅の両隣の最寄分岐駅IDとその駅までの計算キロを得る
		neer_node = RouteUtil::GetNeerNode(stationId);
        lid = Route::LineIdFromStationId(stationId);
		// 発駅～最初の分岐駅までの計算キロを最初の分岐駅までの初期コストとして初期化
		a = Route::Id2jctId(IDENT1(neer_node.at(0)));
		if (!IsJctMask(jct_mask, a)) {
			dijkstra.setMinCost(a - 1, IDENT2(neer_node.at(0)));
			dijkstra.setFromNode(a - 1, -1);	// from駅を-1(分岐駅でないので存在しない分岐駅)として初期化
			dijkstra.setLineId(a - 1, lid);
		}
		if (2 <= neer_node.size()) {
			b = Route::Id2jctId(IDENT1(neer_node.at(1)));
			if (!IsJctMask(jct_mask, b)) {
				dijkstra.setMinCost(b - 1, IDENT2(neer_node.at(1)));
				dijkstra.setFromNode(b - 1, -1);
				dijkstra.setLineId(b - 1, lid);
			} else if (IsJctMask(jct_mask, a)) {
				TRACE(_T("Autoroute:発駅の両隣の分岐駅は既に通過済み"));
				return -10;								// >>>>>>>>>>>>>>>>>>>>>>>
			}
		} else {
			/* 盲腸線 */
			if (IsJctMask(jct_mask, a)) {
				TRACE(_T("Autoroute:盲腸線で通過済み."));
				return -11;								// >>>>>>>>>>>>>>>>>>>>>>>>>>
			}
			b = 0;
		}
	} else {
		dijkstra.setMinCost(startNode - 1, 0);
		a = b = 0;
	}

	loopRoute = false;

	if (lastNode == 0) { /* 終了駅は非分岐駅 ? */
		// 駅の両隣の最寄分岐駅IDとその駅までの計算キロを得る
		neer_node = RouteUtil::GetNeerNode(end_station_id);

		// dijkstraのあとで使用のために変数に格納
		// 終了駅の両隣の分岐駅についてはadd()でエラーとなるので不要(かどうか？）

		lastNode1 = Route::Id2jctId(IDENT1(neer_node.at(0)));
		lastNode1_distance = IDENT2(neer_node.at(0));
		nLastNode = (int32_t)neer_node.size();
		ASSERT((nLastNode == 1) || (nLastNode == 2));  /* 野辺地の大湊線はASSERT*/
		if (2 <= nLastNode) {
			lastNode2 = Route::Id2jctId(IDENT1(neer_node.at(1)));
			lastNode2_distance = IDENT2(neer_node.at(1));
			nLastNode = 2;
		} else {
			/* 着駅=閉塞線 */
			lastNode2 = 0;
			lastNode2_distance = (IDENT)0xffff;
			nLastNode = 1;
		}
		TRACE(_T("Last target=%s, %s\n"), SNAME(Jct2id(lastNode1)), SNAME(Jct2id(lastNode2)));
	} else {
		nLastNode = 0;
		lastNode1 = lastNode2 = 0;
	}

	if ((2 == route_list_raw.size()) &&
				Route::IsSameNode(route_list_raw.back().lineId,
				                  route_list_raw.front().stationId,
				                  route_list_raw.back().stationId)) {
		id = Route::Id2jctId(departureStationId());
		if (id == 0) {
			neer_node = RouteUtil::GetNeerNode(departureStationId());
			if (neer_node.size() == 2) {
				excNode1 = Route::Id2jctId(IDENT1(neer_node.at(0)));		/* 渋谷 品川 代々木 */
				excNode2 = Route::Id2jctId(IDENT1(neer_node.at(1)));		/* 渋谷 品川 新宿 */
TRACE(_T("******** loopRouteX **%s, %s******\n"), SNAME(Jct2id(excNode1)), SNAME(Jct2id(excNode2)));
				loopRoute = true;
			} else {
				/* 逗子 大船 磯子 */
			}
		} else {
			excNode1 = id;
			excNode2 = Id2jctId(route_list_raw.back().stationId);
			if (excNode2 == 0) {
				neer_node = RouteUtil::GetNeerNode(route_list_raw.back().stationId);
				if (neer_node.size() == 2) {
					excNode1 = Route::Id2jctId(IDENT1(neer_node.at(0)));
					excNode2 = Route::Id2jctId(IDENT1(neer_node.at(1)));
					ASSERT((id == excNode1) || (id == excNode2));
					loopRoute = true;		/* 代々木 品川 代々木 */
				} else {
					ASSERT(FALSE);	/* 先頭で途中追加の最終ノードは分岐駅のみとしているのであり得ない */
					                /* 大船 鎌倉 横須賀などは、鎌倉がremoveTail() され大船 横須賀となる*/
				}
			} else {
				loopRoute = true;
			}
TRACE(_T("******** loopRouteY **%s, %s******\n"), SNAME(Jct2id(excNode1)), SNAME(Jct2id(excNode2)));
		}
	}

	/* dijkstra */
	for (;;) {
		doneNode = -1;
		for (i = 0; i < MAX_JCT; i++) {
			// ノードiが確定しているとき
			// ノードiまでの現時点での最小コストが不明の時
			if (dijkstra.doneFlag(i) || (dijkstra.minCost(i) < 0)) {
				continue;
			}
			/*  確定したノード番号が-1かノードiの現時点の最小コストが小さいとき
			 *  確定ノード番号更新する
			 */
			if ((doneNode < 0) || (!IsJctMask(jct_mask, i + 1) && (dijkstra.minCost(i) < dijkstra.minCost(doneNode)))) {
				doneNode = i;
			}
		}
		if (doneNode == -1) {
			break;	/* すべてのノードが確定したら終了 */
		}
		dijkstra.setDoneFlag(doneNode, true);	// Enter start node

		TRACE(_T("[%s]"), SNAME(Jct2id(doneNode + 1)));
		if (nLastNode == 0) {
			if ((doneNode + 1) == lastNode) {
				break;	/* 着ノードが完了しても終了可 */
			}
		} else if (nLastNode == 1) {
			if ((doneNode + 1) == lastNode1) {
				break;	/* 着ノードが完了しても終了可 */
			}
		} else if (nLastNode == 2) {
			if (dijkstra.doneFlag(lastNode1 - 1) &&
				dijkstra.doneFlag(lastNode2 - 1)) {
				break;	/* 着ノードが完了しても終了可 */
			}
		}

		vector<vector<int32_t>> nodes = Route::Node_next(doneNode + 1, except_local);
		vector<vector<int32_t>>::const_iterator ite;

		for (ite = nodes.cbegin(); ite != nodes.cend(); ite++) {

			a = ite->at(0) - 1;	// jctId

			if ((!IsJctMask(jct_mask, a + 1) /**/|| ((nLastNode == 0) && (lastNode == (a + 1))) ||
                 ((0 < nLastNode) && (lastNode1 == (a + 1))) ||
                 ((1 < nLastNode) && (lastNode2 == (a + 1)))) &&
                ((((0x01 & useBulletTrain) != 0) || !IS_SHINKANSEN_LINE(ite->at(2))) &&
                 (((0x02 & useBulletTrain) != 0) || !IS_COMPANY_LINE(ite->at(2))))) {
                /** コメント化しても同じだが少し対象が減るので無駄な比較がなくなる */
				/* 新幹線でない */
				cost = dijkstra.minCost(doneNode) + ite->at(1); // cost

				// ノードtoはまだ訪れていないノード
				// またはノードtoへより小さいコストの経路だったら
				// ノードtoの最小コストを更新
				if ((((dijkstra.minCost(a) < 0) || (cost <= dijkstra.minCost(a))) &&
					((cost != dijkstra.minCost(a)) || IS_SHINKANSEN_LINE(ite->at(2))))
					&&
					(!loopRoute ||
						((((doneNode + 1) != excNode1) && ((doneNode + 1) != excNode2)) ||
						 ((excNode1 != (a + 1)) && (excNode2 != (a + 1)))))) {
					/* ↑ 同一距離に2線ある場合新幹線を採用 */
					dijkstra.setMinCost(a, cost);
					dijkstra.setFromNode(a, doneNode + 1);
					dijkstra.setLineId(a, ite->at(2));
					TRACE( _T("+<%s(%s)>"), SNAME(Jct2id(a + 1)), LNAME(dijkstra.lineId(a)));
				} else {
					TRACE(_T("-<%s>"), SNAME(Jct2id(a + 1)));
				}
			} else {
TRACE(_T("x(%s)"), SNAME(Jct2id(a + 1)));
			}
		}
		TRACE("\n");
	}

	vector<IDENT> route;
	int32_t lineid = 0;
	int32_t idb;

	if (lastNode == 0) { /* 終了駅は非分岐駅 ? */
		// 最後の分岐駅の決定：
		// 2つの最後の分岐駅候補(終了駅(非分岐駅）の両隣の分岐駅)～終了駅(非分岐駅)までの
		// 計算キロ＋2つの最後の分岐駅候補までの計算キロは、
		// どちらが短いか？
		if ((2 == nLastNode) &&
            (!IsJctMask(jct_mask, lastNode2) && ((dijkstra.minCost(lastNode2 - 1) + lastNode2_distance) <
              (dijkstra.minCost(lastNode1 - 1) + lastNode1_distance)))) {
            id = lastNode2;		// 短い方を最後の分岐駅とする
        } else {
            id = lastNode1;
        }
	} else {
		id = lastNode;
	}

TRACE(_T("Last target=%s, <-- %s(%d), (%d, %d, %d)\n"), SNAME(Jct2id(id)), SNAME(Jct2id(dijkstra.fromNode(id - 1))), dijkstra.fromNode(id - 1), (int)lastNode, (int)lastNode1, (int)lastNode2);

	//fromNodeが全0で下のwhileループで永久ループに陥る
	if (dijkstra.fromNode(id - 1) == 0) {
	    if ((lastNode == 0) &&
			(((nLastNode == 2) && (lastNode2 == startNode)) ||
	                              (lastNode1 == startNode))) {
      		/* 品川―大森 */
      		lid = Route::LineIdFromStationId(end_station_id);
			if (0 < Route::InStationOnLine(lid, stationId)) {
				TRACE( _T("short-cut route.###\n"));
           		a = add(lid, /*stationId,*/ end_station_id);
//           		if (0 <= a) {
//           			route_list_cooked.clear();
//           		}
				route_flag.jctsp_route_change = true;	/* route modified */
				return a;	//>>>>>>>>>>>>>>>>>>>>>>>>>
			}
        }
		TRACE( _T("can't lowcost route.###\n"));
		return -1002;
	}
//------------------------------------------
	// 発駅(=分岐駅)でなく最初の分岐駅(-1+1=0)でない間
	// 最後の分岐駅からfromをトレース >> route[]
	while ((id != startNode) && (0 < id)) {
		TRACE( _T("  %s, %s, %s."), LNAME(lineid), LNAME(dijkstra.lineId(id - 1)), SNAME(Jct2id(id)));
		if (lineid != dijkstra.lineId(id - 1)) {
			if (IS_SHINKANSEN_LINE(lineid)) {
                /* 新幹線→並行在来線 */
				int32_t zline = RouteUtil::GetHZLine(lineid, Route::Jct2id(id));
				for (idb = id; (idb != startNode) && (dijkstra.lineId(idb - 1) == zline);
				     idb = dijkstra.fromNode(idb - 1)) {
					TRACE( _T("    ? %s %s/"),  LNAME(dijkstra.lineId(idb - 1)), SNAME(Jct2id(idb)));
					;
				}
				if (dijkstra.lineId(idb - 1) == lineid) { /* もとの新幹線に戻った ? */
					//TRACE(".-.-.-");
					id = idb;
					continue;
				} else if (idb == startNode) { /* 発駅？ */
					if (zline == RouteUtil::GetHZLine(lineid, Route::Jct2id(idb))) {
						id = idb;
						continue;
					}
					/* thru */
					//TRACE("*-*-*-");
				} else if (idb != id) { /* 他路線の乗り換えた ? */
					TRACE( _T("%sはそうだが、%sにも新幹線停車するか?"), SNAME(Jct2id(id)), SNAME(Jct2id(idb)));
					if (zline == RouteUtil::GetHZLine(lineid, Route::Jct2id(idb))) {
						id = idb;
						continue;
					}
					/* thru */
					TRACE( _T("+-+-+-: %s(%s) : "), LNAME(dijkstra.lineId(idb - 1)), LNAME(lineid));
				} else {
					//TRACE("&");
				}
			} else { /* 前回新幹線でないなら */
				/* thru */
				//TRACE("-=-=-=");
			}
            // 次の新幹線でも並行在来線でもない路線への分岐駅に新幹線分岐駅でない場合(金山)最初の在来線切り替えを有効にする（三河安城）
            // 新幹線に戻ってきている場合(花巻→盛岡）、花巻まで（北上から）無効化にする
            //
			route.push_back(id - 1);
			lineid = dijkstra.lineId(id - 1);
			TRACE( _T("  o\n"));
		} else {
			TRACE( _T("  x\n"));
		}
		id = dijkstra.fromNode(id - 1);
	}

	//// 発駅=分岐駅

	TRACE( _T("----------[(%d)%s]------\n"), id, SNAME(Jct2id(id)));

	vector<IDENT> route_rev;
	vector<IDENT>::const_reverse_iterator ritr = route.crbegin();
	int32_t bid = -1;
	while (ritr != route.crend()) {
		TRACE(_T("> %s %s\n"), LNAME(dijkstra.lineId(*ritr)), SNAME(Jct2id(*ritr + 1)));
		if (0 < bid && IS_SHINKANSEN_LINE(dijkstra.lineId(bid))) {
			if (RouteUtil::GetHZLine(dijkstra.lineId(bid), Route::Jct2id(*ritr + 1)) == dijkstra.lineId(*ritr)) {
				dijkstra.setLineId(*ritr, dijkstra.lineId(bid));	/* local line -> bullet train */
				route_rev.pop_back();
			}
		}
		route_rev.push_back(*ritr);
		bid = *ritr;
		++ritr;
	}
	/* reverse(route_rev->route) */
	route.assign(route_rev.cbegin(), route_rev.cend());
	route_rev.clear();	/* release */

	if (lastNode == 0) {	// 着駅は非分岐駅?
TRACE(_T("last: %s\n"), LNAME(dijkstra.lineId(route.back())));
		lid = Route::LineIdFromStationId(end_station_id); // 着駅所属路線ID
		// 最終分岐駅～着駅までの営業キロ、運賃計算キロを取得
		//km = Route::Get_node_distance(lid, end_station_id, Route::Jct2id(a));
		//km += minCost[route.back()];	// 最後の分岐駅までの累積計算キロを更新

        bool isAbreastShinkansen = IsAbreastShinkansen(lid, dijkstra.lineId(route.back()),
                                                       Jct2id(route.back() + 1),
                                                       end_station_id);
        if ((lid == dijkstra.lineId(route.back())) ||
        ((0 < Route::InStationOnLine(dijkstra.lineId(route.back()), end_station_id)) &&
		  isAbreastShinkansen ) ||
          (isAbreastShinkansen &&  (0 < RouteUtil::InStation(end_station_id, lid, Jct2id(route.back() + 1), Jct2id(id))))) {
			route.pop_back();	// if   着駅の最寄分岐駅の路線=最後の分岐駅?
								//      (渋谷-新宿-西国分寺-国立)
                                //     渋谷 山手線 品川 → 有楽町
								// or   平行在来線の新幹線 #141002001
            // else 渋谷-新宿-三鷹
		}
	} else {
		lid = 0;
	}

	if ((1 < route_list_raw.size()) && (1 < route.size()) && (route_list_raw.back().lineId == dijkstra.lineId(route[0]))) {
TRACE(_T("###return return!!!!!!!! back!!!!!! %s:%s#####\n"), LNAME(route_list_raw.back().lineId), SNAME(route_list_raw.back().stationId));
		removeTail();
		ASSERT(0 < route_list_raw.size()); /* route_list_raw.size() は0か2以上 */
		//stationId = route_list_raw.back().stationId;
	}

	a = 1;
	for (i = 0; i < (int32_t)route.size(); i++) {
TRACE(_T("route[] add: %s\n"), SNAME(Route::Jct2id(route[i] + 1)));
		a = add(dijkstra.lineId(route[i]), /*stationId,*/ Route::Jct2id(route[i] + 1));
		route_flag.jctsp_route_change = true;/* route modified */
		if ((a <= 0) || (a == 5)) {
			//ASSERT(FALSE);
TRACE(_T("####%d##%d, %lu##\n"), a, i, route.size());
			if ((a < 0) || ((i + 1) < (int32_t)route.size())) {
//				route_list_cooked.clear();
				return a;	/* error */
			} else {
				break;
			}
		}
		//stationId = Route::Jct2id(route[i] + 1);
	}

//	route_list_cooked.clear();

	if (lastNode == 0) {
TRACE(_T("fin last:%s\n"), LNAME(lid));
		ASSERT(0 < lid);
		if (a == 0) {
			// 立川 八王子 拝島 西国立
			return -1000;
		}
		a = add(lid, /*stationId,*/ end_station_id);
		route_flag.jctsp_route_change = true;/* route modified */
		if ((a <= 0) || (a == 5)) {
			//ASSERT(0 == a);
			return a;
		}
	}
	return a;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// static
int32_t FARE_INFO::tax = 0;

/**	往復運賃を返す(会社線含む総額)(JR分は601km以上で1割引)
 *
 *	@param discount [out]  true=割引あり
 *	@retval [円]
 */
FARE_INFO::FareResult 	FARE_INFO::roundTripFareWithCompanyLine() const
{
	FareResult fareW;

	if (6000 < total_jr_sales_km) {	/* 往復割引 */
		fareW.fare = fare_discount(jr_fare, 1) * 2 + company_fare * 2;
		fareW.isDiscount = true;
		ASSERT(this->roundTripDiscount == true);
	} else {
		fareW.isDiscount = false;
		fareW.fare = jrFare() * 2 + company_fare * 2;
		ASSERT(this->roundTripDiscount == false);
	}
	return fareW;
}

/**	114条適用前の往復運賃を返す(会社線含む総額)(JR分は601km以上で1割引)
 *
 *	@retval [円]
 */
int32_t 	FARE_INFO::roundTripFareWithCompanyLinePriorRule114() const
{
    int32_t fareW;

    if (6000 < total_jr_sales_km) {	/* 往復割引 */
        ASSERT(FALSE);
    }
    if (!isRule114()) {
        ASSERT(FALSE);
    }
    fareW = getFareForJR() * 2 + company_fare * 2;
    return fareW;
}


/**	総営業キロを返す
 *
 *	@retval 営業キロ
 */
int32_t 	FARE_INFO::getTotalSalesKm()  const
{
	return sales_km;
}

/**	JR線の営業キロを返す
 *
 *	@retval 計算キロ
 */
int32_t		FARE_INFO::getJRSalesKm() const
{
	return total_jr_sales_km;
}

/**	JR線の計算キロを返す
 *
 *	@retval 計算キロ
 */
int32_t		FARE_INFO::getJRCalcKm() const
{
	return total_jr_calc_km;
}

/**	会社線の営業キロ総数を返す
 *
 *	@retval 営業キロ
 */
int32_t		FARE_INFO::getCompanySalesKm() const
{
	return sales_km - total_jr_sales_km;
}

/** BRT 営業キロ
 *
 *  @retval 営業キロ
 */
int32_t     FARE_INFO::getBRTSalesKm() const
{
    return brt_sales_km;
}

/**	JR北海道の営業キロを返す
 *
 *	@retval 営業キロ
 */
int32_t		FARE_INFO::getSalesKmForHokkaido() const
{
	return hokkaido_sales_km;
}

/**	JR四国の営業キロを返す
 *
 *	@retval	営業キロ
 */
int32_t		FARE_INFO::getSalesKmForShikoku() const
{
	return shikoku_sales_km;
}

/**	JR九州の営業キロを返す
 *
 *	@retval	営業キロ
 */
int32_t		FARE_INFO::getSalesKmForKyusyu() const
{
	return kyusyu_sales_km;
}

/**	JR北海道の計算キロを返す
 *
 *	@retval	計算キロ
 */
int32_t		FARE_INFO::getCalcKmForHokkaido() const
{
	return hokkaido_calc_km;
}

/**	JR四国の計算キロを返す
 *
 *	@retval	計算キロ
 */
int32_t		FARE_INFO::getCalcKmForShikoku() const
{
	return shikoku_calc_km;
}

/**	JR九州の計算キロを返す
 *
 *	@retval	計算キロ
 */
int32_t		FARE_INFO::getCalcKmForKyusyu() const
{
	return kyusyu_calc_km;
}

/**	乗車券の有効日数を返す
 *
 *	@retval	有効日数[日]
 */
int32_t		FARE_INFO::getTicketAvailDays() const
{
	return avail_days;
}

/**	会社線の運賃額を返す
 *
 *	@retval	[円]
 */
int32_t		FARE_INFO::getFareForCompanyline() const
{
	return company_fare;
}

/**	JR線＋会社線の小児運賃額を返す
 *
 *	@retval	[円]
 */
int32_t		FARE_INFO::getChildFareForDisplay() const
{
	return company_fare_child + fare_discount(jrFare(), 5);
}

/** BRT 運賃
 *
 *  @retval BRT運賃
 */
int32_t     FARE_INFO::getFareForBRT() const
{
    return brt_fare;    // BRT割引は含まれない
}

/**	JR線の運賃額を返す(114条未適用計算値)
 *
 *	@retval	[円]
 */
int32_t		FARE_INFO::getFareForJR() const
{
	return jr_fare;    // BRT（割引も）含む
}

/**	JR線＋会社線の運賃額を返す
 *
 *	@retval	[円]
 */
int32_t		FARE_INFO::getFareForDisplay() const
{
	return getFareForCompanyline() + jrFare();
}

/**	JR線＋会社線の運賃額を返す(114条非適用運賃)
 *
 *	@retval	[円]
 */
int32_t		FARE_INFO::getFareForDisplayPriorRule114() const
{
    if (isRule114()) {
        return getFareForCompanyline() + jr_fare;
    } else {
//      ASSERT(FALSE);
        return 0;
    }
}

/**	JR線の運賃額を返す(含114条適用／非適用判定)
 *
 *	@retval	[円]
 */
int32_t		FARE_INFO::jrFare() const
{
    if (isRule114()) {
        return rule114Info.fare();
    } else {
		return jr_fare;
    }
}

/**	株主優待割引有効数を返す
 *
 *	@param index      [in]  0から1 JR東海のみ
 *	@param idCompany [out]  0:JR東海1割/1:JR西日本5割/2:JR東日本2割/3:JR東日本4割
 *	@retval	[円](無割引、無効は0)
 */
int32_t FARE_INFO::countOfFareStockDiscount() const
{
	// 通過連絡運輸も株優は有効らしい

    switch (getStockDiscountCompany()) {
    case JR_CENTRAL:
		return 2;
		break;
    case JR_EAST:
    case JR_WEST:
    case JR_KYUSYU:
        return 1;
        break;
    default:
       break;
    }
	return 0;
}

/**	株主優待割引運賃を返す(会社線運賃は含まない)
 *
 *	@param index      [in]   0から1 JR東日本のみ 0は2割引、1は4割引を返す
 *	@param title      [out]  項目表示文字列
 *	@param applied_r114  [in]  true = 114条適用 / false = 114条適用前
 *	@retval	[円](無割引、無効は0)
 */
int32_t FARE_INFO::getFareStockDiscount(int32_t index, tstring& title, bool applied_r114 /* =false*/)
const
{
	const TCHAR* const titles[] = {
		_T("JR東日本 株主優待2割"), // 0 (2020.6より無効)
		_T("JR東日本 株主優待4割"), // 1
		_T("JR西日本 株主優待5割"), // 2
		_T("JR東海   株主優待1割"), // 3
		_T("JR東海   株主優待2割"), // 4
		_T("JR九州   株主優待5割"), // 5
	};

	int32_t cfare;
	int32_t brtfare;

	brtfare = brt_fare - brt_discount_fare;

	if (applied_r114) {
		if (isRule114()) {
			cfare = rule114Info.fare() - brtfare;
		} else {
			ASSERT(FALSE);
			return 0;		// >>>>>>>>>
		}
	} else {
		cfare = jr_fare - brtfare;
	}

    switch (getStockDiscountCompany()) {
    case JR_EAST:
		if (index == 0) {
			title = titles[1];	// JR東日本　株主優待4割
			return fare_discount(cfare, 4) + fare_discount(brtfare, 4);
		}
        break;
    case JR_WEST:
        if (index == 0) {
            title = titles[2];
            return fare_discount(cfare, 5);
        }
    case JR_KYUSYU:
        if (index == 0) {
            title = titles[5];
    		return fare_discount(cfare, 5);
        }
        break;
    case JR_CENTRAL:
		if (index == 0) {
			title = titles[3];
			return fare_discount(cfare, 1);
		}
		else if (index == 1) {
			/* JR海2割(2枚使用) */
			title = titles[4];
			return fare_discount(cfare, 2);
		}
		break;
	default:
        break;
    }
	return 0;

	// 通過連絡運輸も株優は有効らしい
}

//  株主優待可否／種別を返す
//  @retval JR_EAST = JR東日本
//  @retval JR_WEST = JR西日本
//  @retval JR_CENTRAL = JR東海
//  @retval JR_KYUSYU = JR九州
//  @retval 0 = 無効
//	会社線も含んでも良い(計算時は除外されるため)
//
int32_t FARE_INFO::getStockDiscountCompany() const
{
	if ((enableTokaiStockSelect == 1) || (enableTokaiStockSelect == 3)) {
		// 1: 品川 東海道新幹線 名古屋
		// 1: 品川 東海道新幹線 三島 東海道線 富士 身延線 甲府
		// 3: 品川 東海道新幹線 新横浜
		return JR_CENTRAL;
	}
	if ((JR_GROUP_MASK & companymask) == (1 << (JR_EAST - 1))) {
		return JR_EAST;
	}
	if ((JR_GROUP_MASK & companymask) == (1 << (JR_WEST - 1))) {
		return JR_WEST;
	}
    if ((JR_GROUP_MASK & companymask) == (1 << (JR_KYUSYU - 1))) {
        return JR_KYUSYU;
    }
	if ((JR_GROUP_MASK & companymask) == (1 << (JR_CENTRAL - 1))) {
        return JR_CENTRAL;
	}
	return 0;
}


/**	学割運賃を返す(会社線+JR線=全線)
 *
 *	@retval	学割運賃[円]
 *	@retval 0 非適用
 */
int32_t		FARE_INFO::getAcademicDiscountFare() const
{
	int32_t result_fare;
	int32_t fareBrt = brt_fare - brt_discount_fare;

	if ((1000 < total_jr_sales_km) || (0 < company_fare_ac_discount)) {
		if (1000 < total_jr_sales_km) {
			result_fare = (fare_discount(jrFare() - fareBrt, 2)
						   + fare_discount(fareBrt, 2));
		} else {
			result_fare = jrFare();
		}
		return result_fare + (company_fare - company_fare_ac_discount);
	} else {
		/* 学割非適用 */
		return 0;
	}
}

/**	学割往復運賃を返す(会社線含む総額)(JR分は601km以上で1割引)
 *
 *	@retval [円]
 */
int32_t 	FARE_INFO::roundTripAcademicFareWithCompanyLine() const
{
	int32_t fareS;
	int32_t fareBrt = brt_fare - brt_discount_fare;

	// JR

	if (6000 < total_jr_sales_km) {	/* 往復割引かつ学割 */
	// 最初に往復割引で１割引いた後に、学割分の２割をJR線 BRT線 どちらもそれぞれ引く x 2
		fareS = fare_discount(fare_discount(jrFare() - fareBrt, 1), 2)
              + fare_discount(fare_discount(fareBrt, 1), 2);
		ASSERT(this->roundTripDiscount == true);
	} else {
	// 学割分の２割をJR線 BRT線 どちらもそれぞれ引く x2
		if (1000 < total_jr_sales_km) {
			// Academic discount
			ASSERT(this->roundTripDiscount == false);
			fareS = fare_discount((jrFare() - fareBrt), 2)
				  + fare_discount(fareBrt, 2);
		} else {
			fareS = jrFare();
		}
	}

	// company

	fareS += (company_fare - company_fare_ac_discount);
	return fareS * 2;
}

/**	小児往復運賃を返す(会社線含む総額)(JR分は601km以上で1割引)
 *
 *	@paramm discount [out]  true=割引あり
 *	@retval [円]
 */
int32_t 	FARE_INFO::roundTripChildFareWithCompanyLine() const
{
	int32_t fareW;

	if (6000 < total_jr_sales_km) {	/* 往復割引 */
		fareW = fare_discount(fare_discount(jr_fare, 5), 1) * 2 + company_fare_child * 2;
	} else {
		fareW = fare_discount(jrFare(), 5) * 2 + company_fare_child * 2;
	}
	return fareW;
}


/**	IC運賃を返す
 *
 *	@retval IC運賃(x10[円])
 */
int32_t		FARE_INFO::getFareForIC() const
{
	//ASSERT(((fare_ic != 0) && ((companymask == (1 << (JR_CENTRAL - 1))) || (companymask == (1 << (JR_EAST - 1))))) || (fare_ic == 0));
	ASSERT(((fare_ic != 0) && (0 == (companymask & ~((1 << (JR_CENTRAL - 1)) | ((1 << (JR_EAST - 1))))))) || (fare_ic == 0));
	if (0 < company_fare) {
		return 0;
	} else {
		return fare_ic;
	}
}

/* static */
/*	路線の2点間営業キロ、計算キロ、会社(JR-Gr.)境界を得る
 *	calc_fare() => aggregate_fare_info() =>
 *
 *	@param [in] line_id   	 路線ID
 *	@param [in] station_id1  駅1
 *	@param [in] station_id2  駅2
 *
 *	@return vector<int32_t> [0] 営業キロ
 *	@return vector<int32_t> [1] 計算キロ
 *	@return vector<int32_t> [2] 駅1の会社区間部の営業キロ(駅1の会社ID≠駅2の会社ID時のみ有効)
 *                          駅1が境界駅なら-1を返す, 境界駅が駅1～駅2間になければ、Noneを返す
 *	@return vector<int32_t> [3] 駅2の会社区間部の計算キロ(駅1の会社ID≠駅2の会社ID時のみ有効)
 *                          駅2が境界駅なら-1を返す, 境界駅が駅1～駅2間になければ、Noneを返す
 *	@return vector<int32_t> [4] IDENT1(駅1の会社ID) + IDENT2(駅2の会社ID)
 *	@return vector<int32_t> [5] IDENT1(駅1のsflg) / IDENT2(駅2のsflg(MSB=bit15除く)
*/
vector<int32_t> FARE_INFO::getDistanceEx(int32_t line_id, int32_t station_id1, int32_t station_id2)
{
	vector<int32_t> result;

	DBO ctx = DBS::getInstance()->compileSql(
"select"
"	(select max(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))-"
"	(select min(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3)),"		// [0]
"	(select max(calc_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))-"
"	(select min(calc_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3)),"		// [1]
"	case when exists (select * from t_lines	where line_id=?1 and (lflg&((1<<16)|(1<<31)))=(1<<16) and station_id=?2)"
"	then 0 else"
"	abs((select sales_km from t_lines where line_id=?1 and (lflg&((1<<16)|(1<<31)))=(1<<16)"
"	and	sales_km>(select min(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))"
"	and	sales_km<(select max(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3)))-"
"	(select sales_km from t_lines where line_id=?1 and station_id=?2)) end,"						// [2]
"	case when exists (select * from t_lines	where line_id=?1 and (lflg&((1<<16)|(1<<31)))=(1<<16) and station_id=?3)"
"	then 0 else"
"	abs((select calc_km from t_lines where line_id=?1 and (lflg&((1<<16)|(1<<31)))=(1<<16)"
"	and	sales_km>(select min(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))"
"	and	sales_km<(select max(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3)))-"
"	(select calc_km from t_lines where line_id=?1 and station_id=?2)) end,"							// [3]
"	(select company_id from t_station where rowid=?2),"				// [4](4)
"   (select company_id from t_station where rowid=?3),"	            // [4](5)
"	(select sub_company_id from t_station where rowid=?2),"			// [4](6)
"	(select sub_company_id from t_station where rowid=?3),"         // [4](7)
"	((select sflg&4095 from t_station where rowid=?2) + (select sflg&4095 from t_station where rowid=?3) * 65536)"		// [5](8)
);
	uint32_t company_id1;
	uint32_t company_id2;
	uint32_t sub_company_id1;
	uint32_t sub_company_id2;
//	2147483648 = 0x80000000
	if (ctx.isvalid()) {
		ctx.setParam(1, line_id);
		ctx.setParam(2, station_id1);
		ctx.setParam(3, station_id2);

		if (ctx.moveNext()) {
			result.push_back(ctx.getInt(0));	// sales_km
			result.push_back(ctx.getInt(1));	// calc_km
			result.push_back(ctx.getInt(2));	// sales_km for in company as station_id1
			result.push_back(ctx.getInt(3));	// calc_km  for in company as station_id1
			company_id1 = ctx.getInt(4);
			company_id2 = ctx.getInt(5);
			sub_company_id1 = ctx.getInt(6);
			sub_company_id2 = ctx.getInt(7);
			result.push_back(0);				// IDENT1(駅ID1の会社ID) + IDENT2(駅ID2の会社ID)
			result.push_back(ctx.getInt(8));	// bit31:1=JR以外の会社線／0=JRグループ社線 / IDENT1(駅1のsflg) / IDENT2(駅2のsflg(MSB=bit15除く))

			if ((line_id == LINE_ID(_T("博多南線"))) ||
				(line_id == LINE_ID(_T("山陽新幹線")))) { //山陽新幹線、博多南線はJ九州内でもJR西日本
				result[4] = MAKEPAIR(JR_WEST, JR_WEST);
			}
			else if (line_id == LINE_ID(_T("北海道新幹線"))) {
				result[4] = MAKEPAIR(JR_HOKKAIDO, JR_HOKKAIDO);
			}
			else if ((line_id == LINE_ID(_T("東海道新幹線"))) &&
				     ((company_id1 == company_id2) && (company_id1 != JR_CENTRAL))) {
				/*
					東 西 -> あとで救われる
					東 海
					海 東
					海 海
					東 東 -> 東 海
					西 西 -> 西 海 にする　上り、下りの情報は不要
					西 東
					西 海
					海 西
				*/
				result[4] = MAKEPAIR(company_id1, JR_CENTRAL);
			}
			else {
 TRACE("company_id1=%d, sub_company_id1=%d, company_id2=%d, sub_company_id2=%d\n", company_id1, sub_company_id1, company_id2, sub_company_id2);
 TRACE("s1km=%d, c1km=%d, s2km=%d, c2km=%d\n", result[0], result[1], result[2], result[3]);

				if (company_id1 != company_id2) {
                    if (IS_JR_MAJOR_COMPANY(company_id1) && IS_JR_MAJOR_COMPANY(company_id2)) {
						// 本州内
						// (company)(subcompany) - (company)(subcompany)
                        // 猪谷(4)(3) 富山(4)
                        // 猪谷(4)(3) 名古屋(3)
                        // 神戸(4)　門司(3)(4)
                        // 品川(2)-新大阪(4)
                        // 長野(2)-金沢(4)
                         // 同一路線で会社A-会社B-会社Aなどというパターンはあり得ない>紀勢線 亀山 新宮 以西間にあり
                         // A-B-Cはあり得る。熱海 新幹線 京都　の場合、JR海-西であるべきだが　東-西となる(現在は本州3社は同一と見て問題ない)
						// 亀山(3)(4)-和歌山(4)
						// 亀山(3)(4)-新宮(4)(3)
						// 松阪(3)-新宮(4)(3)
						// 松阪(3)-亀山(3)(4)
						if ((line_id == LINE_ID(_T("紀勢線")))
						&& (((company_id1 == JR_CENTRAL)
						&& (sub_company_id1 == JR_WEST) && (company_id2 == JR_WEST) && (sub_company_id2 == 0))
						|| ((company_id2 == JR_CENTRAL)
						&& (sub_company_id2 == JR_WEST) && (company_id1 == JR_WEST) && (sub_company_id1 == 0)))) {
							// do-noghint
							TRACE("detect kisyu-sen kameyama-(wakayama-miwasaki)\n");
						} else if (sub_company_id1 == company_id2) {
							// 猪谷-富山、神戸-門司
    						company_id1 = sub_company_id1;
    					}
    					else if (company_id1 == sub_company_id2) {
    						company_id2 = sub_company_id2;
    					}
                    } else {
						// 本州-離島
                        if ((sub_company_id1 != 0 || sub_company_id2 != 0) &&
                            ((result[2] == 0) && (result[3] == 0))) {
                            /* 門司(5) - 下関(4)(5) */
                            /* 児島 - 宇多津 */
                            if (company_id1 < company_id2) {
                                /* 四国、 九州 にする */
                                company_id1 = company_id2;
                            } else {
                                company_id2 = company_id1;
                            }
                        } else {
                            /* 新山口 - 門司 とか */
                        }
                    }
                }
				result[4] = MAKEPAIR(company_id1, company_id2);
			}
		}
	}
	return result;
}

/*static
 *   case 大阪環状線
 *
 *	calc_fare() => aggregate_fare_info() =>
 *
 *	@param [in] route flag 	 大阪環状線通過フラグ
 *	@param [in] line_id   	 路線ID
 *	@param [in] station_id1  駅1
 *	@param [in] station_id2  駅2
 *
 *	@return vector<int32_t> [0] 営業キロ
 *	@return vector<int32_t> [1] 計算キロ
 *	@return vector<int32_t> [2] 駅1の会社区間部の営業キロ(駅1の会社ID≠駅2の会社ID時のみ有効)
 *                          駅1が境界駅なら-1を返す, 境界駅が駅1～駅2間になければ、Noneを返す
 *	@return vector<int32_t> [3] 駅1の会社区間部の計算キロ(駅1の会社ID≠駅2の会社ID時のみ有効)
 *                          駅2が境界駅なら-1を返す, 境界駅が駅1～駅2間になければ、Noneを返す
 *	@return vector<int32_t> [4] IDENT1(駅1の会社ID) + IDENT2(駅2の会社ID)
 *	@return vector<int32_t> [5] IDENT1(駅1のsflg) / IDENT2(駅2のsflg(MSB=bit15除く))
 *
 */
vector<int32_t> FARE_INFO::GetDistanceEx(const RouteFlag& osakakan_aggregate, int32_t line_id, int32_t station_id1, int32_t station_id2)
{
	vector<int32_t> result;
	int32_t rslt = 0;

	result = RouteUtil::GetDistance(osakakan_aggregate, line_id, station_id1, station_id2); // [0][1]
	result.push_back(0);	// sales_km for in company as station_id1 [2]
	result.push_back(0);	// calc_km  for in company as station_id1 [3]
	result.push_back(MAKEPAIR(JR_WEST, JR_WEST));	// IDENT1(駅ID1の会社ID) + IDENT2(駅ID2の会社ID) [4]
	DBO ctx = DBS::getInstance()->compileSql("select"
" (select sflg&4095 from t_station where rowid=?1) + ((select sflg&4095 from t_station where rowid=?2) * 65536)"		// [5]
		);
	if (ctx.isvalid()) {
		ctx.setParam(1, station_id1);
		ctx.setParam(2, station_id2);
		if (ctx.moveNext()) {
			rslt = ctx.getInt(0);
		}
	}
	result.push_back(rslt);	// bit31:1=JR以外の会社線／0=JRグループ社線 = 0 / IDENT1(駅1のsflg) / IDENT2(駅2のsflg(MSB=bit15除く))
    TRACE("oskkan:s1km=%d, c1km=%d\n", result[0], result[1]);

	return result;
}


// static
//		@brief 70条通過の営業キロを得る
//
//	@param [in] station_id1		駅1
//	@param [in] station_id2		駅2
//
//	@return station_id1, station_id2間の営業キロ
//
int32_t FARE_INFO::Retrieve70Distance(int32_t station_id1, int32_t station_id2)
{
	static const char tsql[] =
"select sales_km from t_rule70"
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
//	@brief 近郊区間でない条件となる新幹線乗車があるか？
//
//	@param [in] line_id
//	@param [in] station_id1		駅1
//	@param [in] station_id2		駅2
//
//	@return true 新幹線
//
// 大都市近郊区間に含まれる新幹線は米原駅 - 新大阪駅間と西明石駅 - 相生駅間のみ
//
bool FARE_INFO::IsBulletInUrban(int32_t line_id, int32_t station_id1, int32_t station_id2, bool isRule88)
{
	static const char tsql[] =
" select count(*) - sum(sflg>>13&1) from t_station t join t_lines l on l.station_id=t.rowid"
"	where line_id=?1"
"	and (lflg&((1<<31)|(1<<17)))=0"
"	and sales_km>=(select min(sales_km) from t_lines"
"			where line_id=?1 and (station_id=?2 or station_id=?3))"
"	and sales_km<=(select max(sales_km) from t_lines"
"			where line_id=?1 and (station_id=?2 or station_id=?3))";
// 13:近郊区間、17:新幹線仮想分岐駅
// 新幹線乗車でも13がONなら近郊区間内とみなせる(新幹線乗車ではない)

	int32_t rsd = -1;

	if (!IS_SHINKANSEN_LINE(line_id)) {
		return false;     /* 非新幹線はFalse */
	}

    /* 88条 によって大阪に書き換えられていたら、新大阪-山陽新幹線 乗車なので
     * 新幹線乗車とする
     */
    // ref. CheckOfRule88j()
    if (isRule88) {
        TRACE("IsBulletInUrban is true(rule88 applied)\n");
        return true;
    }

	DBO dbo = DBS::getInstance()->compileSql(tsql);
	if (dbo.isvalid()) {
		dbo.setParam(1, line_id);
		dbo.setParam(2, station_id1);
		dbo.setParam(3, station_id2);

		if (dbo.moveNext()) {
			rsd = dbo.getInt(0);
		}
	}
    TRACE("IsBulletInUrban=%d\n", rsd); // rsd=0なら新幹線乗車とはみなされない
	return 0 < rsd;    // 新幹線乗車とみなすならTrue
}


/**	近郊区間内かを返す(有効日数を1にする)
 *
 *	@retval	true 近郊区間
 */
bool 	FARE_INFO::isUrbanArea() const
{
	return ((MASK_URBAN & flag) != 0);
}

//static
bool FARE_INFO::IsIC_area(int32_t urban_id)
{
	return ((urban_id == URB_TOKYO) ||
		(urban_id == URB_NIGATA) ||
		(urban_id == URB_SENDAI));
}

// 近郊区間内で新幹線乗車があるか？
// 新幹線乗車は86or87経路か？(大都市近郊区間かはまだわからんのであとで判定)
// 86,87中の新幹線乗車は新幹線乗車とみなさない(品川〜東京〜宇都宮として品川から新幹線乗っても運賃計算は東京[山]
// からなので新幹線乗車はできても近郊区間特例は解かれない)
// 70条が適用されている場合、その計算経路に新幹線が含まれていれば、大都市近郊区間ルールから外せるが、
// 70条の最短経路以外の新幹線乗車は無効である(経路指定ができない)
// 八王子から都内を通って宇都宮へいく場合、代々木〜品川(新幹線)東京〜宇都宮 としても新幹線を指定することは
// できないので(70条の計算経路ではない)新幹線乗車は無効である(大都市近郊区間特例は解かれない)
// 70条は、 reRouteRule70j()で仮設定している
//
// 70は、ID_L_RULE70 路線がある前提(cooked経路であること)
void FARE_INFO::CheckIsBulletInUrbanOnSpecificTerm(const vector<RouteItem>& routeList, RouteFlag* pRoute_flag)
{
    IDENT station_id1 = 0;		/* station_id1, (station_id2=ite->stationId) */
	vector<RouteItem>::const_iterator ite;
    uint16_t cityId = 0;    // [区], [横]
    uint16_t cityId_c = 0;    // [区], [横]
    bool enabled = false;

	for (ite = routeList.cbegin(); ite != routeList.cend(); ite++) {
		if (station_id1 != 0) {
            cityId_c = (uint16_t)MASK_CITYNO(ite->flag);

//            printf("@@@>>>>%s, lid=%d, 70=%d, 8687=%d, city=%d - %d, IsBulletInUrban=%d\n", ite->lineId == ID_L_RULE70 ? "true":"false", ite->lineId, pRoute_flag->rule70, pRoute_flag->isAvailableRule86or87(), cityId, cityId_c,
//                IsBulletInUrban(ite->lineId, station_id1, ite->stationId, pRoute_flag->rule88));

            if (!(pRoute_flag->isAvailableRule86or87()
                  && (cityId != 0) && (cityId_c != 0)
                  && (cityId == cityId_c)) &&
                IsBulletInUrban(ite->lineId, station_id1, ite->stationId, pRoute_flag->rule88)) {
                TRACE("Use bullet line.\n");
                enabled = true; // ONの場合大都市近郊区間特例無効(新幹線乗車している)
                break;
			}
		}
		station_id1 = ite->stationId;
        cityId = (uint16_t)MASK_CITYNO(ite->flag);
    }
    pRoute_flag->bullet_line = enabled;
}




int FARE_INFO::aggregate_fare_company(bool first_company,   /* 1回目の会社線 */
                                      const RouteFlag& rRoute_flag,
                                      int32_t station_id_0,
                                      int32_t station_id,
                                      int32_t station_id1)
{
    FARE_INFO::CompanyFare comfare;	//[0]fare, [1]children, [2]academic, [3]no-connect-discount

    if (0 < station_id_0) {
        /* 2回以上連続で会社線の場合(タンゴ鉄道とか) */
            /* a+++b+++c : */
        if (!FARE_INFO::Fare_company(station_id_0, station_id, &comfare)) {
            /* 乗継割引なし */
            if (!FARE_INFO::Fare_company(station_id1, station_id, &comfare)) {
                ASSERT(FALSE);
            }
            station_id_0 = station_id1;
        } else {
            FARE_INFO::CompanyFare comfare_1;	// a+++b

            // if ex. 氷見-金沢 併算割引非適用
            if (comfare.is_connect_non_discount_fare() &&
                (!rRoute_flag.compnend || !rRoute_flag.compnbegin)) {
                /* 乗継割引なし */
                if (!FARE_INFO::Fare_company(station_id1, station_id, &comfare)) {
                    ASSERT(FALSE);
                }
            } else {
                /* normal or 併算割引適用 */
                if (!FARE_INFO::Fare_company(station_id_0, station_id1, &comfare_1)) {
                    ASSERT(FALSE);
                }
                if (0 < comfare_1.fareAcademic) {
                    /* 学割ありの場合は割引額を戻す */
                    ASSERT(comfare_1.fareAcademic < comfare_1.fare);
                    this->company_fare_ac_discount -= (comfare_1.fare - comfare_1.fareAcademic);
                }
                this->company_fare -= comfare_1.fare;	// 戻す(直前加算分を取消)
                this->company_fare_child -= comfare_1.fareChild;
            }
        }
        if (0 < comfare.fareAcademic) {
            /* 学割ありの場合は割引額を加算 */
            ASSERT(comfare.fareAcademic < comfare.fare);
            this->company_fare_ac_discount += (comfare.fare - comfare.fareAcademic);
        }
        this->company_fare += comfare.fare;
        this->company_fare_child += comfare.fareChild;
    } else {
        /* 1回目の会社線 */
        if (first_company) {
            result_flag.comapany_first = true;
        } else if ((0 < this->company_fare) &&
                   !result_flag.comapany_end) {
            /* 会社線 2回以上通過 */
            result_flag.company_incorrect = true;
            ASSERT(FALSE);
        }
        if (!FARE_INFO::Fare_company(station_id1, station_id, &comfare)) {
            ASSERT(FALSE);
        }
        if (0 < comfare.fareAcademic) {
            /* 学割ありの場合は割引額を加算 */
            ASSERT(comfare.fareAcademic < comfare.fare);
            this->company_fare_ac_discount += (comfare.fare - comfare.fareAcademic);
        }
        this->company_fare += comfare.fare;
        this->company_fare_child += comfare.fareChild;

        station_id_0 = station_id1;

        result_flag.comapany_end = true;
    }
    return station_id_0;
}

//	1経路の営業キロ、計算キロを集計
//	calc_fare() =>
//
//	@retval 0 < Success(特別加算区間割増運賃額.通常は0)
//	@retval -1 Fatal error
//  @note route_flag update bit was BLF_JRTOKAISTOCK_ENABLE only.
//  @note isCityterminalWoTokai()を呼ぶので、setTerminal()を読んでおく必要がある
//
int32_t FARE_INFO::aggregate_fare_info(RouteFlag* pRoute_flag, const vector<RouteItem>& routeList)
{
	IDENT station_id1;		/* station_id1, (station_id2=ite->stationId) */
	IDENT station_id_0;		/* last station_id1(for Company line) */
	vector<RouteItem>::const_iterator ite;
	RouteFlag   osakakan_aggregate;	// 大阪環状線通過フラグ bit0: 通過フラグ

    CheckIsBulletInUrbanOnSpecificTerm(routeList, pRoute_flag); // routeListはcooked経路であること
	                                //
	int32_t fare_add = 0;						// 運賃特別区間加算値

	this->local_only = true;
	this->local_only_as_hokkaido = true;
	osakakan_aggregate = *pRoute_flag;
	osakakan_aggregate.setOsakaKanPass(false);

    enableTokaiStockSelect = 0;

	pRoute_flag->jrtokaistock_enable = false ;	// b#20090901 未使用になった

	station_id_0 = station_id1 = 0;

	for (ite = routeList.cbegin(); ite != routeList.cend(); ite++) {

//ASSERT((ite->flag) == 0);
		if (station_id1 != 0) {
							/* 会社別営業キロ、計算キロの取得 */

			if (ite->lineId == ID_L_RULE70) {
				int32_t sales_km;
				sales_km = FARE_INFO::Retrieve70Distance(station_id1, ite->stationId);
				ASSERT(0 < sales_km);
				this->sales_km += sales_km;			// total 営業キロ(会社線含む、有効日数計算用)
				this->base_sales_km	+= sales_km;
				this->base_calc_km += sales_km;
				this->local_only = false;		// 幹線

			} else {
				int32_t company_id1;
				int32_t company_id2;
				int32_t flag;
				vector<int32_t> d;

				if (ite->lineId != LINE_ID(_T("大阪環状線"))) {
					d = this->getDistanceEx(ite->lineId, station_id1, ite->stationId);
				} else {
					d = FARE_INFO::GetDistanceEx(osakakan_aggregate, ite->lineId, station_id1, ite->stationId);
					osakakan_aggregate.setOsakaKanPass(true);
				}

				if (6 != d.size()) {
					ASSERT(FALSE);
					return -1;	/* failure abort end. >>>>>>>>> */
				}
				company_id1 = IDENT1(d.at(4));
				company_id2 = IDENT2(d.at(4));

				TRACE(_T("multicompany line none detect X: %d, %d, comp1,2=%d, %d, %s:%s-%s\n"),
				      d.at(2),
				      d.at(3),
				      company_id1,
				      company_id2,
				      LNAME(ite->lineId),
				      SNAME(station_id1),
				      SNAME(ite->stationId));

				this->companymask |= ((1 << (company_id1 - 1)) | ((1 << (company_id2 - 1))));

				if ((company_id1 == JR_CENTRAL) || (company_id1 == JR_WEST)) {
					company_id1 = JR_EAST;	/* 本州三社としてJR東海、JR西は、JR東とする */
				}
				if ((company_id2 == JR_CENTRAL) || (company_id2 == JR_WEST)) {
					company_id2 = JR_EAST;	/* 本州三社としてJR東海、JR西は、JR東とする */
				}

				this->sales_km += d.at(0);			// total 営業キロ(会社線含む、有効日数計算用)
				if (IS_COMPANY_LINE(ite->lineId)) {	/* 会社線 */
                    station_id_0 = aggregate_fare_company(this->sales_km == d.at(0),
                                                            *pRoute_flag,
                                                            station_id_0,
                                                            ite->stationId,
                                                            station_id1);
				} else {
					/* JR線 */
					station_id_0 = 0;

					result_flag.comapany_end = false;

					/* JR Group */
					if (IS_SHINKANSEN_LINE(ite->lineId)) {
						/* 山陽新幹線 広島-徳山間を通る場合の1経路では、fare(l, i) とされてしまうけんのぉ
						 */
						this->local_only = false;		// 幹線
					}
                    if (this->aggregate_fare_jr(IS_BRT_LINE(ite->lineId), company_id1, company_id2, d) < 0) {
						return -1;	// error >>>>>>>>>>>>>>>>>>>>>>>>>>>
					}
				}
				if ((this->flag & FLAG_FARECALC_INITIAL) == 0) { // b15が0の場合最初なので駅1のフラグも反映
					// 保持bit(既にflagのbitのみにはcheckIsBulletInUrbanOnSpecificTerm()でセットしているから)=いまはないので0
				    this->flag &= MASK_FARECALC_INITIAL;
									// 次回以降から駅1不要、駅1 sflgの下12ビット,
									// bit12以上はGetDistanceEx()のクエリでOxfffしているので不要
					this->flag |= (FLAG_FARECALC_INITIAL | (/*~(1<<BCBULURB) & */IDENT1(d.at(5))));
				}
				flag = (FLAG_FARECALC_INITIAL | MASK_FARECALC_INITIAL | IDENT2(d.at(5)));
				if ((flag & MASK_URBAN) != (this->flag & MASK_URBAN)) {/* 近郊区間(b7-9) の比較 */
					flag &= ~MASK_URBAN;				/* 近郊区間 OFF */
				}
				this->flag &= flag;	/* b11,10,5(大阪/東京電車特定区間, 山手線／大阪環状線内) */
									/* ~(反転）不要 */
				/* flag(sflg)は、b11,10,5, 7-9 のみ使用で他はFARE_INFOでは使用しない */

								// 特別加算区間
				fare_add += FARE_INFO::CheckSpecificFarePass(ite->lineId, station_id1, ite->stationId);
			}
		}
		station_id1 = ite->stationId;
	}
    if (pRoute_flag->bJrTokaiOnly) {
		// JR東海のみ

		// [名]以外の都区市内・山手線が発着のいずれかにあるか
		if (isCityterminalWoTokai()) {
			enableTokaiStockSelect = 1;	// JR東海株主優待券使用可
            pRoute_flag->jrtokaistock_enable = true;
            TRACE("Enable use the JR Tokai stock option.\n");
		}
		else {
			if (this->companymask == (1 << (JR_CENTRAL - 1))) {
                enableTokaiStockSelect = 2; // JR東海TOICA有効
                TRACE("Enable use the JR Tokai TOICA.\n");
            }
            else {
				TRACE("could you used stock JR East and Tokai.\n");
				enableTokaiStockSelect = 3;	// JR東海株主優待券使用可
          	}
//			TRACE("Set only the JR Tokai.\n");
//			this->companymask = (1 << (JR_CENTRAL - 1));
//			if (pRoute_flag->jrtokaistock_applied) {
//				pRoute_flag->jrtokaistock_enable = true;
//			}
		}
	}
	return fare_add;
}

//
//	calc_fare() => aggregate_fare_info() -> *
//	距離値積上集計(JRグループ)
//
//	@param [in] is_brt         BRT?
//	@param [in] company_id1    会社1
//	@param [in] company_id2    会社2
//	@param [in] distance       GetDistanceEx()の戻り
//
//	@retval 0 success
//	@retval -1 failure
//
int32_t FARE_INFO::aggregate_fare_jr(bool is_brt, int32_t company_id1, int32_t company_id2, const vector<int32_t>& distance)
{
	if (company_id1 == company_id2) {		// 同一 1社
        if (is_brt) {
            this->brt_sales_km 	+= distance.at(0);
            this->brt_calc_km 	+= distance.at(1);
        } else {
			/* 単一 JR Group */
    		switch (company_id1) {
    		case JR_HOKKAIDO:
    			this->hokkaido_sales_km 	+= distance.at(0);
    			if (0 == distance.at(1)) { 		/* 幹線? */
    				this->hokkaido_calc_km 	+= distance.at(0);
    				this->local_only_as_hokkaido = false;
    				this->local_only = false;	// 幹線
    			} else {
    				this->hokkaido_calc_km 	+= distance.at(1);
    			}
    			break;
    		case JR_SHIKOKU:
    			this->shikoku_sales_km 		+= distance.at(0);
    			if (0 == distance.at(1)) { 		/* 幹線? */
    				this->shikoku_calc_km 	+= distance.at(0);
    				this->local_only = false;	// 幹線
    			} else {
    				this->shikoku_calc_km 	+= distance.at(1);
    			}
    			break;
    		case JR_KYUSYU:
    			this->kyusyu_sales_km 		+= distance.at(0);
    			if (0 == distance.at(1)) { 		/* 幹線? */
    				this->kyusyu_calc_km 	+= distance.at(0);
    				this->local_only = false;	// 幹線
    			} else {
    				this->kyusyu_calc_km 	+= distance.at(1);
    			}
    			break;
    		case JR_EAST:
    		case JR_CENTRAL:
    		case JR_WEST:
    			this->base_sales_km 		+= distance.at(0);
    			if (0 == distance.at(1)) { 		/* 幹線? */
    				this->base_calc_km 		+= distance.at(0);
    				this->local_only = false;	// 幹線
    			} else {
    				this->base_calc_km 		+= distance.at(1);
    			}
    			break;
    		default:
    			ASSERT(FALSE);	/* あり得ない */
    			break;
    		}
	   } /* no BRT */
   } else {								// 会社跨り?
		switch (company_id1) {
		case JR_HOKKAIDO:					// JR北海道->本州3社(JR東)
//			ASSERT(FALSE);		// 北海道新幹線が開通するまでお預け
			this->hokkaido_sales_km 	+= distance.at(2);
			this->base_sales_km 		+= (distance.at(0) - distance.at(2));
			if (0 == distance.at(1)) { /* 幹線? */
				this->hokkaido_calc_km  += distance.at(2);
				this->base_calc_km  	+= (distance.at(0) - distance.at(2));
				this->local_only_as_hokkaido = false;
			} else {
				this->hokkaido_calc_km  += distance.at(3);
				this->base_calc_km  	+= (distance.at(1) - distance.at(3));
			}
			break;
		case JR_SHIKOKU:					// JR四国->本州3社(JR西)
			this->shikoku_sales_km 		+= distance.at(2);
			this->base_sales_km 		+= (distance.at(0) - distance.at(2));
			if (0 == distance.at(1)) { 			/* 幹線? */
				this->shikoku_calc_km 	+= distance.at(2);
				this->base_calc_km  	+= (distance.at(0) - distance.at(2));
				this->local_only = false;	// 幹線
			} else {
				this->shikoku_calc_km 	+= distance.at(3);
				this->base_calc_km  	+= (distance.at(1) - distance.at(3));
			}
			break;
		case JR_KYUSYU:					// JR九州->本州3社(JR西)
			this->kyusyu_sales_km 		+= distance.at(2);
			this->base_sales_km 		+= (distance.at(0) - distance.at(2));
			if (0 == distance.at(1)) { /* 幹線? */
				this->kyusyu_calc_km 	+= distance.at(2);
				this->base_calc_km 		+= (distance.at(0) - distance.at(2));
				this->local_only = false;	// 幹線
			} else {
				this->kyusyu_calc_km 	+= distance.at(3);
				this->base_calc_km 		+= (distance.at(1) - distance.at(3));
			}
			break;
		default:
			this->base_sales_km 		+= distance.at(2);
			if (0 == distance.at(1)) { /* 幹線? */
				this->base_calc_km 		+= distance.at(2);
				this->local_only = false;	// 幹線
			} else {
				this->base_calc_km 		+= distance.at(3);
			}
			switch (company_id2) {
			case JR_HOKKAIDO:			// 本州3社(JR東)->JR北海道
//				ASSERT(FALSE);	// 北海道新幹線が来るまでお預け
				this->hokkaido_sales_km		+= (distance.at(0) - distance.at(2));
				if (0 == distance.at(1)) { /* 幹線? */
					this->hokkaido_calc_km	+= (distance.at(0) - distance.at(2));
					this->local_only_as_hokkaido = false;
				} else {
					this->hokkaido_calc_km	+= (distance.at(1) - distance.at(3));
				}
				break;
			case JR_SHIKOKU:			// 本州3社(JR西)->JR四国
				this->shikoku_sales_km		+= (distance.at(0) - distance.at(2));
				if (0 == distance.at(1)) { /* 幹線? */
					this->shikoku_calc_km	+= (distance.at(0) - distance.at(2));
				} else {
					this->shikoku_calc_km	+= (distance.at(1) - distance.at(3));
				}
				break;
			case JR_KYUSYU:				// 本州3社(JR西)->JR九州
				this->kyusyu_sales_km		+= (distance.at(0) - distance.at(2));
				if (0 == distance.at(1)) { /* 幹線? */
					this->kyusyu_calc_km	+= (distance.at(0) - distance.at(2));
				} else {
					this->kyusyu_calc_km	+= (distance.at(1) - distance.at(3));
				}
				break;
			default:
				ASSERT(FALSE);
				return -1;	/* failure abort end. >>>>>>>>> */
				break;	/* あり得ない */
			}
			break;
		} /* JR 2company */
	} /* JR Group */
	return 0;
}


// Private:
//	showFare() =>
//	checkOfRule114j() =>
//
//	運賃計算
//
//	@param [in] routeList    経路
//	@param [in] applied_rule ルール適用(デフォルトTrue)
//	@return 異常の時はfalse
//  @note route_flag update bit was BLF_JRTOKAISTOCK_ENABLE only.
//  @note aggregate_fare_info()で、isCityterminalWoTokai()を呼ぶので、setTerminal()を読んでおく必要がある(JR東海のみ)
//
bool FARE_INFO::calc_fare(RouteFlag* pRoute_flag, const vector<RouteItem>& routeList)
{
	int32_t fare_add;		/* 特別加算区間 */
	int32_t adjust_km;

	reset();

	if ((fare_add = FARE_INFO::aggregate_fare_info(pRoute_flag, routeList)) < 0) {
		ASSERT(FALSE);
        reset();
        result_flag.fatal_error = true;
		return false;
        //goto err;		/* >>>>>>>>>>>>>>>>>>> */
	}

	/* 旅客営業取扱基準規定43条の2（小倉、西小倉廻り） */
	if (!pRoute_flag->no_rule && !pRoute_flag->osakakan_detour) {
		adjust_km = FARE_INFO::CheckAndApplyRule43_2j(routeList);
		this->sales_km			-= adjust_km * 2;
		this->base_sales_km		-= adjust_km;
		this->base_calc_km		-= adjust_km;
		this->kyusyu_sales_km	-= adjust_km;
		this->kyusyu_calc_km	-= adjust_km;
	}

	/* 旅客営業規則89条適用 */
	if (!pRoute_flag->no_rule && !pRoute_flag->osakakan_detour) {
		this->base_calc_km += FARE_INFO::CheckOfRule89j(routeList);
	}

    /* 運賃計算 */
    calc_brt_fare(routeList);

	/* 幹線のみ ? */
	this->major_only = ((this->base_sales_km == this->base_calc_km) &&
						(this->kyusyu_sales_km == this->kyusyu_calc_km) &&
						(this->hokkaido_sales_km == this->hokkaido_calc_km) &&
						(this->shikoku_sales_km == this->shikoku_calc_km));

	retr_fare(pRoute_flag->isUseBullet());

    if ((0 != this->brt_sales_km) || (0 < this->total_jr_sales_km)) {
        /* JR or BRT */

		int32_t special_fare;

		if (pRoute_flag->compncheck && (this->sales_km < 1500)) {
										/* 大聖寺-和倉温泉は106kmある */
										/* 大聖寺-米原-岐阜-富山-津端-和倉温泉 を弾く為 */
	        	TRACE("specific fare section replace for IR-ishikawa change continue discount\n");
				special_fare = FARE_INFO::SpecificFareLine(routeList.front().stationId, routeList.back().stationId, 2);
				if (0 < special_fare) {
		            this->jr_fare = special_fare - this->company_fare;	/* IRいしかわ 乗継割引 */
				}
		} else if ( //!pRoute_flag->isUseBullet() &&           /* b#18111401: 新幹線乗車なく、 */
		            (((MASK_URBAN & this->flag) != 0) || (this->sales_km < 500))
					&& !pRoute_flag->isIncludeCompanyLine()) { // 東京メトロは適用外
			special_fare = FARE_INFO::SpecificFareLine(routeList.front().stationId, routeList.back().stationId, 1);
			if (0 < special_fare) {
	        	TRACE("specific fare section replace for Metro or Shikoku-Big-bridge\n");
                if (!pRoute_flag->no_rule && !pRoute_flag->osakakan_detour) {
                    // 品川-青森-横浜 なども適用されてはいけないので,近郊区間内なら適用するように。
    				// 品川-横浜などの特別区間は近郊区間内の場合遠回り指定でも特別運賃を表示
    				// 名古屋は近郊区間でないので距離(尾頭橋-岡崎 37.7km 名古屋-岡崎 40.1km)50km以下として条件に含める
    				// またIRいしかわの乗継割引区間も同様50km以下が条件

    		        if (URB_TOKYO == URBAN_ID(this->flag)) {      /* 東京、新潟、仙台 近郊区間(最短距離で算出可能) */
    		                                                      /* 新幹線乗車も特別運賃適用 */
    		                                                      /* ---> b#18111401: しないようにした(上で弾いた)*/

    		            if ((companymask & (1 << (JR_CENTRAL - 1))) != 0) {
    		                /* 新幹線乗車 はIC運賃適用しないが大都市特例運賃は適用 */
    		                ASSERT(0 == (companymask & ~((1 << (JR_EAST - 1)) | (1 << (JR_CENTRAL - 1)))));
    		            } else {
    		                ASSERT(companymask == (1 << (JR_EAST - 1)));  /* JR East only  */
    		                this->fare_ic = special_fare;
    		            }
    		            this->jr_fare = round_up(special_fare);	/* 大都市特定区間運賃(東京)(\10単位切り上げ) */
    		        } else {
    		            this->jr_fare = special_fare;	/* 大都市特定区間運賃(大阪、名古屋), 本四備讃線ローカル */
    		        }
                }
                pRoute_flag->special_fare_enable = true; // 私鉄競合区間特別運賃適用
	        }
			ASSERT(this->company_fare == 0);	// 会社線は通っていない
		}

        // 特定区間は加算しない
        if (!pRoute_flag->special_fare_enable) {
            // 特別加算区間分
    		this->jr_fare += fare_add;
        }

        if (isUrbanArea() && !pRoute_flag->isUseBullet()) {
            this->avail_days = 1;	/* 当日限り */
        } else {
            /* 乗車券の有効日数 */
            this->avail_days = FARE_INFO::days_ticket(this->sales_km);
        }

        if (6000 < total_jr_sales_km) {	/* 往復割引 */
            this->roundTripDiscount = true;
        } else {
            this->roundTripDiscount = false;
        }
		return true;

	} else {
		/* 会社線のみ */
		//this->reset();
		this->avail_days = 1;
        this->roundTripDiscount = false;
        this->companymask &= ~JR_GROUP_MASK;
		return true;
		//return false;
	}
}


// static private
//
//  @return [0][0] station_id1
//          [0][1] station_id2
//          [0][2] type 0=normal, 1=BRT
//          [1] :
//          :
std::vector<std::vector<int>> FARE_INFO::getBRTrecord(int32_t line_id)
{
    static const  char tsql[] =
    "select station_id1, station_id2, type from t_brtsp where line_id=?1";
    std::vector<std::vector<int>> results;

    DBO dbo = DBS::getInstance()->compileSql(tsql);
    dbo.setParam(1, line_id);

    while (dbo.moveNext()) {
        std::vector<int> v;
        v.push_back(dbo.getInt(0));
        v.push_back(dbo.getInt(1));
        v.push_back(dbo.getInt(2));
        results.push_back(v);
    }
    return results;
}

//private
//
void FARE_INFO::calc_brt_fare(const vector<RouteItem>& routeList)
{
    unsigned i;
    vector<RouteItem>::const_iterator ite;
    int32_t station_id1 = 0;
    int32_t lineId = 0;
    std::vector<std::vector<int>> brt_tbl;
	bool bBRTPassed = false;
	uint8_t passc = 0;

    if (brt_sales_km <= 0) {
        return;             /* not exists BRT */
    }
    for (ite = routeList.cbegin(); ite != routeList.cend(); ite++) {
		if (station_id1 != 0) {
            if (lineId == 0) {
                /* first line */
    			if (IS_BRT_LINE(ite->lineId)) {
    				bBRTPassed = true;
    			}
    			else {
    				bBRTPassed = false;
    			}
            }
            lineId = ite->lineId;

            brt_tbl = getBRTrecord(lineId);

            if (IS_BRT_LINE(lineId)) {
                if (!bBRTPassed) {
                   // Train -> BRT
                   bBRTPassed = true;
                   passc++;
               }
                for (i = 0; i < brt_tbl.size(); i++) {
                    std::vector<int> v = RouteUtil::getIntersectOnLine(lineId,
                                                                       brt_tbl[i][0], brt_tbl[i][1],
                                                                       station_id1, ite->stationId);
                     if (v[0] != 0) {
                         /* 前谷地-柳津はBRTだけど鉄道線として扱う */
                         brt_sales_km -= v[0];
                         base_sales_km += v[0];
                         brt_calc_km -= v[1];
                         base_calc_km += v[1];
                         break;
                     }
                 }
				// 前谷地―柳津を通ってたら
                 if (i < brt_tbl.size()) {
                     if (0 < RouteUtil::InStation(ite->stationId, lineId, brt_tbl[i][0], brt_tbl[i][1])) {
                         bBRTPassed = false;
                         passc++;
                     }
                 } else {
                 }
            } else {
				if (bBRTPassed) {
					// BRT -> Train
					bBRTPassed = false;
					passc++;
				}
                if (passc <= 1) {
                    for (i = 0; i < brt_tbl.size(); i++) {
                        // 乗り継ぎ割引区間内か？
                        if (RouteUtil::inlineOnline(lineId,
                                                    brt_tbl[i][0], brt_tbl[i][1],
                                                    station_id1, ite->stationId)) {
                            break;
                        }
                    }
                    if (!(i < brt_tbl.size())) {
						passc = 2;	/* 2以上は割引無効 */
                    }
                }
            }
		}
        station_id1 = ite->stationId;
    }
    if (((base_sales_km != 0) && (brt_sales_km != 0)) && (passc <= 1)) {
        brt_discount_fare = BRT_DISCOUNT_FARE;
    } else {
        brt_discount_fare = 0;
    }
}


//	経路設定
//	calcFare() =>
//
//	@param [in]	begin_station_id  開始駅
//	@param [in] end_station_id    終了駅
//	@param [in] routeList         経路（規則適用時は変換後、規則非適用なら変換前）
//	@param [in]           route flag(LF_OSAKAKAN_MASK:大阪環状線関連フラグのみ).
//
void FARE_INFO::setRoute(const vector<RouteItem>& routeList,
                         const RouteFlag& rRoute_flag)
{
    route_for_disp = RouteUtil::Show_route(routeList, rRoute_flag);
}

// TOICA Neerest route
//
void FARE_INFO::setTOICACalcRoute(const vector<RouteItem>& routeList,
                        const RouteFlag& rRoute_flag)
{
    calc_route_for_disp = RouteUtil::Show_route(routeList, rRoute_flag);
}


/*
          XXXX          XXXX
    xxxx  xxxx    xxxx  xxxx
    xxxx  拝島     xxxx  八王子
    八高線 八王子   八高線 拝島
    xxxx xxxx    　XXXX XXXX

    　　XXXX          XXXX
xxxx  　xxxx    xxxx  xxxx
xxxx 　 拝島     xxxx  八王子
青梅線   立川   　中央東線 立川
中央東線 八王子 　青梅線 拝島
xxxx xxxx 　   　XXXX XXXX


  最短経路化した経路で拝島 八王子間を八高線を経由したいた場合、営業キロが長い
  青梅線 拝島ー立川+中央東線 立川ー八王子 を経由した方が電車特定区間の例外のため
  安くなる場合がありえるため。

  static
  @param [in] route_list    The route
  @retval Chaned route or Empty route(if N/A)
 */
std::vector<RouteItem> FARE_INFO::IsHachikoLineHaijima(const std::vector<RouteItem>& route_list)
{
    vector<RouteItem> out_route_list;
    vector<RouteItem>::const_iterator ite;
    int32_t sid1;
    bool b;
    bool bb;
    int32_t id_hachiko_line = LINE_ID(_T("八高線"));
    int32_t id_cyuou_line = LINE_ID(_T("中央東線"));
    int32_t id_oume_line = LINE_ID(_T("青梅線"));
    int32_t id_haijima = STATION_ID(_T("拝島"));
    int32_t id_hachiouji = STATION_ID(_T("八王子"));
    int32_t id_tachikawa = STATION_ID(_T("立川"));
    int32_t cid = 0;

    bb = b = false;
    for (ite = route_list.cbegin(); ite != route_list.cend(); ite++) {
        sid1 = ite->stationId;
        if (!b) {
            if (id_haijima == sid1) {
                b = true;
                cid = id_hachiouji;
            } else if (id_hachiouji == sid1) {
                b = true;
                cid = id_haijima;
            }
            out_route_list.push_back(*ite);
        } else if ((ite->lineId == id_hachiko_line) && (cid == sid1)) {
            if (cid == id_hachiouji) {
            	// 拝島 八高線 八王子 -> 拝島 青梅線 立川 中央線 八王子
                out_route_list.push_back(RouteItem(id_oume_line, id_tachikawa));
                out_route_list.push_back(RouteItem(id_cyuou_line, id_hachiouji));
            } else {
            	// 八王子 八高線 拝島 -> 八王子 中央線 立川 青梅線 拝島
                out_route_list.push_back(RouteItem(id_cyuou_line, id_tachikawa));
                out_route_list.push_back(RouteItem(id_oume_line, id_haijima));
            }
            bb = true;
            b = false;
        } else {
            b = false;
            out_route_list.push_back(*ite);
        }
    }
    if (!bb) {
        out_route_list.clear();
    }
    return out_route_list;
}

//  JR東海 IC運賃用の最短経路をだす
//
//
bool FARE_INFO::reCalcFareForOptiomizeRouteForToiCa(const RouteList& route_original)
{
    FARE_INFO fare_info_shorts;         // 最短経路

    // JR東海(TOICA)
    Route shortRoute(reRouteForToica(route_original));

    fare_info_shorts.setTerminal(route_original.departureStationId(), route_original.arriveStationId());
    RouteFlag short_route_flag = shortRoute.getRouteFlag();
	if (!fare_info_shorts.calc_fare(&short_route_flag, shortRoute.routeList())) {
        ASSERT(FALSE);
        return false;
    }

    if (fare_info_shorts.getFareForJR() < this->getFareForJR()) {
        //
        fare_ic = fare_info_shorts.getFareForJR();
        setTOICACalcRoute(shortRoute.routeList(), short_route_flag);
        return true;
    } else {
        clrTOICACalcRoute();
        return false;
    }
}

/*
 *  大都市近郊区間またはJR東海IC運賃計算のための最適化再計算
 *
 *  @param [in] route_original  経路(Cooked)
 *  @param [in] begin_id 開始駅(86・87・88条適用／非適用)
 *  @param [in] term_id  終了駅(86・87・88条適用／非適用)
 *  @retval true 適用
 *  @retval false 非適用(通常経路)
 */
bool FARE_INFO::reCalcFareForOptiomizeRoute(RouteList& route_original)
{
    FARE_INFO fare_info_shorts;         // 最短経路
    FARE_INFO fare_info_nolocal_short;  // 地方交通線を避けた経路
    FARE_INFO fare_info_specific_short; // 都区市内発着最短
    bool b_change_route = false;
    int8_t decision = 0;   // this or via_tachikawa or short

    // 大都市近郊区間内ではない、or 新幹線乗車している or 同一駅(単駅ベースで)発着 なら対象外
    if ( !isUrbanArea() || route_original.getRouteFlag().isUseBullet()
	     || route_original.getRouteFlag().isIncludeCompanyLine()
         || (route_original.departureStationId() == route_original.arriveStationId())) {
        // usualy or loop route */
        TRACE("No reCalcFareForOptiomizeRoute.\n");
        return false;
    }
    TRACE("begin reCalcFareForOptiomizeRoute.urban=%d, bullet=%d, specific term=%d\n", isUrbanArea(), route_original.getRouteFlag().isUseBullet(), route_original.getRouteFlag().rule86or87);

    /* 大都市近郊区間 */

    // 最短経路算出(8687 applied)
    // 近郊区間内で指定経路が8687適用で、
    //   A. 最短中心が8687適用ならその運賃を採用（最短中心駅で算出）1, 3, 5,6,7,8
    //      蒲田 東神奈川 上諏訪 とか、高崎 池袋とか
    //   B. 最短中心が8687適用できないなら単駅の最安経路(規程115)
    //       代々木 立川 用土 や 蒲田 立川 茅野 とか
    // 規程115 当該中心駅からの最も短い営業キロが200km以内となるときに限り、規則第86条の規定を適用しないで、
    // 発駅から実際の営業キロ又は運賃計算キロによつて計算することができる。
    std::vector<RouteItem> specificRoute_List;
    RouteFlag specificRouteFlag;

    if (route_original.getRouteFlag().isAvailableRule86or87()) {
        int stid;
        int edid;

        if (0 != (0x05 & route_original.getRouteFlag().rule86or87)) {
            stid = FARE_INFO::CenterStationIdFromCityId(route_original.coreAreaIDByCityId(CSTART));
        } else {
            stid = route_original.departureStationId();
        }
        if (0 != (0x0a & route_original.getRouteFlag().rule86or87)) {
            edid = FARE_INFO::CenterStationIdFromCityId(route_original.coreAreaIDByCityId(CEND));
        } else {
            edid = route_original.arriveStationId();
        }
        ASSERT(stid != 0 && edid != 0);

        if (!fare_info_specific_short.reCalcFareForOptiomizeRoute(&specificRoute_List,
                                                                   stid, edid,
                                                                   &specificRouteFlag)) {
            ASSERT(FALSE);
            return false;
        }

        if ((route_original.getRouteFlag().isAvailableRule86() && specificRouteFlag.isAvailableRule86()) ||
            (route_original.getRouteFlag().isAvailableRule87() && specificRouteFlag.isAvailableRule87())) {
            // [山],[区] or [横] applied
            // A.近郊区間内で指定経路が8687適用で、最短中心が8687適用ならその運賃を採用（最短中心駅で算出）
            // 　高崎-池袋、蒲田-上諏訪  など
            TRACE("neerest specific terminal over 200.0km or 100.0km\n");
            decision = 20;
        } else {
            // B. 規程115 近郊区間内で指定経路が8687適用で、最短中心が8687適用できないなら単駅の最安経路
            //     代々木ー用土、蒲田-茅野　など
            TRACE("could apply rule115.\n");
            if (0 <= route_original.getRouteFlag().rule115) {
                fare_info_specific_short.reset(); // 86崩れの87が適用されては困るんで(この下の処理で)
                decision = 15;
                route_original.refRouteFlag().rule115 = 1; //notify to user
            } else {
                TRACE("set specific term apply rule115.\n");
                return false;
            }
        }
	}

    // 最短経路算出
    std::vector<RouteItem> shortRoute_List;
    RouteFlag short_route_flag;
    std::vector<RouteItem> route_nolocal_short;

    if (decision != 20) {
        ASSERT(decision == 0 || decision == 15);
        short_route_flag.setDisableRule86or87();
        if (!fare_info_shorts.reCalcFareForOptiomizeRoute(&shortRoute_List,
                                                           route_original.departureStationId(),
                                                           route_original.arriveStationId(),
                                                           &short_route_flag)) {
            ASSERT(FALSE);
            return false;
        }

        // 最短経路との差が、50km 越えならそのまま指定経路で一旦提示
		int difference = (getTotalSalesKm() - fare_info_shorts.getTotalSalesKm());
        if (0 < difference) {
            TRACE("The appoint route and neerest route was different.\n");
			if (500 < difference) {
	            TRACE("          over the 50.0km(cancel lowcost route)\n");
    	        route_original.refRouteFlag().rule115 = 0; // 大回り指定の場合、115条は無効（打ち消す)
			}
			if (route_original.refRouteFlag().no_rule) {
				/* 非適用ではrule115 変数のみが欲しいので */
				return false;
			}
            if (route_original.getRouteFlag().urban_neerest < 0) {
                TRACE("Foreced choice appint route.\n");
                return false;
            }
            route_original.refRouteFlag().meihan_city_enable = 0;   // 名阪のあれも。
	        route_original.refRouteFlag().urban_neerest = 1; // 近郊区間内ですので最短経路の運賃で利用可能です
        } else {
            TRACE("already neerest route.\n");
			if (route_original.refRouteFlag().no_rule) {
				/* 非適用ではrule115 変数のみが欲しいので */
				return false;
			}
      	    route_original.refRouteFlag().urban_neerest = 0; // すでに最安になってます(ので指定経路へ云々の選択肢なし)
        }

        short_route_flag.rule86or87 = 0;
        fare_info_shorts.setRoute(shortRoute_List, short_route_flag);

    // 最短経路の運賃算出:fare_info_shorts
//                          書き換えたroute_flagはJR東海株主使用可否Optionだけなので無視してよし
//                          学割、小児、株主運賃は既存どおりなので、fare_infoのic運賃のみfare_info_shortsのic運賃へ書き換える
//                            と拝島問題
        if (fare_info_shorts.didHaveLocalLine()
            && (decision == 0) /* 86or87 適用絡みはやらん (decision == 15 or 20) */
            && fare_info_nolocal_short.reCalcFareForOptiomizeRoute(&route_nolocal_short,
                                                               route_original.departureStationId(),
                                                               route_original.arriveStationId(),
                                                               &short_route_flag,
                                                               true)) {
            // 八高線 拝島ー八王子を通過している場合、立川経由の方が電特により安い場合がある(b#18122802)
            // 地方交通線を避けた方が最安
            fare_info_nolocal_short.setTerminal(route_original.departureStationId(),
                                                route_original.arriveStationId());
            if (fare_info_nolocal_short.calc_fare(&short_route_flag, route_nolocal_short)) {
                if (fare_info_nolocal_short.getFareForJR() < fare_info_shorts.getFareForJR()) {
                    /* 地方交通線を避けた方が最安 */
                    TRACE("Found lowcost route DENSHA special.\n");
                    decision = 2;
                }
            } else {
                ASSERT(FALSE);
            }
        }
        ASSERT(fare_info_shorts.total_jr_calc_km != 0 && 0 != total_jr_calc_km);
        if ((decision != 2) && (fare_info_shorts.jr_fare < jr_fare)) {
            /* ユーザ指定は最短経路ではない */ /* 経路長くても運賃同じなら入れ替えない */
            TRACE("Found neerest lowcost route.%dyen->%dyen\n", jr_fare, fare_info_shorts.jr_fare);
            decision = 1;
        }
    }
    if (route_original.refRouteFlag().no_rule) {
		/* 非適用ではrule115 変数のみが欲しいので */
		return false;
	}
    ASSERT(decision == 20 || decision == 1 || decision == 2 || decision == 0 || decision == 15);

TRACE("reCalc(urban): decision=%d, this=%s->%s(%d)(%dyen),\n                          short=%s->%s(%d)(%dyen)\n", decision, CalcRoute::BeginOrEndStationName(this->getBeginTerminalId()).c_str(), CalcRoute::BeginOrEndStationName(this->getEndTerminalId()).c_str(), this->getTotalSalesKm(), jr_fare, CalcRoute::BeginOrEndStationName(fare_info_shorts.getBeginTerminalId()).c_str(), CalcRoute::BeginOrEndStationName(fare_info_shorts.getEndTerminalId()).c_str(), fare_info_shorts.getTotalSalesKm(), fare_info_shorts.jr_fare);

    if ((decision == 1/*最短経路*/) ||
        (route_original.getRouteFlag().special_fare_enable == true)) {
        // 私鉄競合区間特別運賃適用の場合経路が大回りのままで不自然なので最短経路を表示するように
        if (getStockDiscountCompany() != JR_CENTRAL) {
            // 東海道新幹線(新大阪-米原間は大都市近郊区間適用)利用は
            // 新幹線を使用しているけど、最短経路は新幹線を使わないで算出しているので
            // 新幹線経路が在来線経路に勝手に書き換わってしまうので。
            // 最短経路=最安
			*this = fare_info_shorts;
            setRoute(shortRoute_List, short_route_flag);
        } else {
            jr_fare = fare_info_shorts.jr_fare;
        }
        b_change_route = true;
    } else if (decision == 2) {
        // 地方交通線を避けた方が最安
        if (getStockDiscountCompany() != JR_CENTRAL) {
            *this = fare_info_nolocal_short;
            setRoute(route_nolocal_short, short_route_flag);
        } else {
            jr_fare = fare_info_nolocal_short.jr_fare;
        }
        b_change_route = true;  // 特例非適用オプション選択可
    } else if (decision == 20) {
        // 86 or 87 applied
        TRACE("optimized 86or87: applied type A. %s->%s(%dyen), %s->%s(%dyen)\n", CalcRoute::BeginOrEndStationName(this->getBeginTerminalId()).c_str(), CalcRoute::BeginOrEndStationName(this->getEndTerminalId()).c_str(), jr_fare, CalcRoute::BeginOrEndStationName(fare_info_specific_short.getBeginTerminalId()).c_str(), CalcRoute::BeginOrEndStationName(fare_info_specific_short.getEndTerminalId()).c_str(), fare_info_specific_short.jr_fare);
        *this = fare_info_specific_short;
        setRoute(specificRoute_List, specificRouteFlag);
        b_change_route = true;
    }
    TRACE("reCalcFareForOptiomizeRoute: applied.\n");

    return b_change_route;
}

//  最短経路を算出して運賃計算する
//
bool FARE_INFO::reCalcFareForOptiomizeRoute(std::vector<RouteItem> *pShortRoute_list,
                                            int32_t start_station_id,
                                            int32_t end_station_id,
                                            RouteFlag* pShort_route_flag,
                                            bool except_local /* =false */ )
{
    Route shortRoute;
    int32_t rc = shortRoute.add(start_station_id);
    ASSERT(rc == 1);
    rc = shortRoute.changeNeerest(except_local ? 100 : 0, end_station_id);
    if (rc < 0) {
        ASSERT(except_local);
        return false;
    }

    CalcRoute shortCalcRoute(shortRoute);
    shortCalcRoute.refRouteFlag().setAnotherRouteFlag(*pShort_route_flag);
    shortCalcRoute.checkOfRuleSpecificCoreLine();

    setTerminal(shortCalcRoute.beginStationId(), shortCalcRoute.endStationId());
    *pShort_route_flag = shortCalcRoute.getRouteFlag();

    if (!calc_fare(pShort_route_flag, shortCalcRoute.routeList())) {
        ASSERT(FALSE);
        return false;
    }

    pShortRoute_list->assign(shortCalcRoute.routeList().cbegin(),
                             shortCalcRoute.routeList().cend());
    return true;
}


//  経路をJR東海 IC運賃用 最短経路にする
//
//  @param [in/out]   route  経路
//
RouteList FARE_INFO::reRouteForToica(const RouteList& route)
{
    Route shortRoute;
    int32_t rc;
    int32_t id;
    bool bNeer = false;
    vector<RouteItem>::const_iterator pos;
    const std::vector<RouteItem>& route_list = route.routeList();

    if (route_list.size() <= 1) {
		return route;
	}

    pos = route_list.cbegin();

    rc = shortRoute.add(pos->stationId);
    // add() の開始駅追加時removeAll()が呼ばれroute_flagがリセットされるため)
	// route_flagに((1 << BLF_OSAKAKAN_DETOUR) | (1 << BLF_NOTSAMEKOKURAHAKATASHINZAI))を設定する必要があるが
    // JR東海内のみの経路のため、関係ない。

    for (++pos; pos != route_list.cend(); pos++) {
        id = pos->stationId;
        if ((id == STATION_ID(_T("金山(中)")))
        || (id == STATION_ID(_T("岐阜")))
        || (id == STATION_ID(_T("美濃太田")))
        || (id == STATION_ID(_T("多治見")))) {
            bNeer = true;
        } else {
            if (bNeer == true) {
                rc = shortRoute.changeNeerest(0, id);
                if (rc < 0) {
                    ASSERT(FALSE);
                    /* error */
                    return route;
                }
            } else {
                rc = shortRoute.add(pos->lineId, id);
                if (rc != ADDRC_OK) {
                    break;
                }
            }
            bNeer = false;
        }
    }
    if ((rc < 0) || ((rc != ADDRC_OK) && ((rc == ADDRC_LAST) && (pos != route_list.cend())))) {
        /* error */
        return route;
    }
    if (bNeer == true) {
        rc = shortRoute.changeNeerest(0, route.arriveStationId());
        if (rc < 0) {
            ASSERT(FALSE);
            return route;
        }
    }
    return shortRoute;
}

void FARE_INFO::setIsRule16_5_route(RouteList& route_original)
{
	int32_t arriveStationId = route_original.arriveStationId();
	int32_t departureStationId = route_original.departureStationId();

	if ((((arriveStationId == STATION_ID(_T("綾瀬")))
	&& (departureStationId == STATION_ID(_T("北千住"))))
	|| ((arriveStationId == STATION_ID(_T("北千住")))
	&& (departureStationId == STATION_ID(_T("綾瀬")))))
	&& getTotalSalesKm() < 50) {
		route_original.refRouteFlag().rule16_5 = true;
		TRACE("applied Rule16-5.\n");
	} else {
		route_original.refRouteFlag().rule16_5 = false;
	}
}

//static
//	集計された営業キロ、計算キロより運賃額を算出(運賃計算最終工程)
//	(JRのみ)
//	calc_fare() =>
//
//	@retval true Success
//	@retval false Fatal error(会社線のみJR無し)
//
void FARE_INFO::retr_fare(bool useBullet)
{
	int32_t fare_tmp;
    int32_t _total_jr_sales_km;
    int32_t _total_jr_calc_km;
    int32_t _total_jr_fare = 0;

	_total_jr_sales_km = this->base_sales_km +
						 this->kyusyu_sales_km +
						 this->hokkaido_sales_km +
						 this->shikoku_sales_km;

	_total_jr_calc_km = this->base_calc_km +
						this->kyusyu_calc_km +
						this->hokkaido_calc_km +
						this->shikoku_calc_km;

    // brt
    if (0 < brt_sales_km) {
        fare_tmp = FARE_INFO::Fare_sub_f(this->brt_sales_km);
        this->brt_fare = round(fare_tmp);
    }

				// 本州3社あり？
	if (0 < (this->base_sales_km + this->base_calc_km)) {
		if (IS_OSMSP(this->flag)) {
			/* 大阪電車特定区間のみ */
			ASSERT(_total_jr_fare == 0); /* 特別加算区間を通っていないはずやねん */
			ASSERT(this->company_fare == 0);	// 会社線は通ってへん
			ASSERT(this->base_sales_km == _total_jr_sales_km);
			ASSERT(this->base_sales_km == this->sales_km);
            ASSERT(this->base_calc_km == _total_jr_calc_km);
			if (IS_YAMATE(this->flag) && FARE_INFO::tax != 10) {
                                        // 2025.4.1 大阪環状線特例廃止 
				TRACE("fare(osaka-kan)\n");
				_total_jr_fare = FARE_INFO::Fare_osakakan(_total_jr_sales_km);
			} else {
				TRACE("fare(osaka)\n");
				_total_jr_fare = FARE_INFO::Fare_osaka(_total_jr_sales_km);
			}
		} else if ((_total_jr_sales_km == _total_jr_calc_km) &&   /* 地方交通線(八高線)含まず:b#18122801 */
                   (IS_TKMSP(this->flag) && (IS_YAMATE(this->flag) || (((1 << (JR_CENTRAL - 1)) & companymask) == 0)))) {
			/* 東京電車特定区間のみ (東海道新幹線ではない) )*/ /* b#18083101, b#19051701 */
			ASSERT(_total_jr_fare == 0); /* 特別加算区間を通っていないはずなので */
//#20200726			ASSERT(this->company_fare == 0);	// 会社線は通っていない
			ASSERT(this->base_sales_km == _total_jr_sales_km);
//#20200726			ASSERT(this->base_sales_km == this->sales_km);
			ASSERT(this->base_calc_km == _total_jr_calc_km);

			if (IS_YAMATE(this->flag)) {
				TRACE("fare(yamate)\n");
				fare_tmp = FARE_INFO::Fare_yamate_f(_total_jr_sales_km);
			} else {
				TRACE("fare(tokyo)\n");
				fare_tmp = FARE_INFO::Fare_tokyo_f(_total_jr_sales_km);
			}
			if (FARE_INFO::tax == 5) {
				_total_jr_fare = round(fare_tmp);
			} else {
				/* 新幹線乗車はIC運賃適用外(東北新幹線も) */
				if (!useBullet) {
					this->fare_ic = fare_tmp;
				}
				_total_jr_fare = round_up(fare_tmp);
			}
		} else if (this->local_only || (!this->major_only && (_total_jr_sales_km <= 100))) {
			/* 本州3社地方交通線のみ or JR東+JR北 */
			/* 幹線+地方交通線でトータル営業キロが10km以下 */
			// (i)<s>
			TRACE("fare(sub)\n");

			fare_tmp = FARE_INFO::Fare_sub_f(_total_jr_sales_km);

			if ((FARE_INFO::tax != 5) &&
			    IsIC_area(URBAN_ID(this->flag)) &&   /* 近郊区間(最短距離で算出可能) */
				!useBullet) {             /* 新幹線乗車はIC運賃適用外 */
				//ASSERT(companymask == (1 << (JR_EAST - 1)));  /* JR East only  */

				this->fare_ic = fare_tmp;
			}
			_total_jr_fare = round(fare_tmp);

		} else { /* 幹線のみ／幹線+地方交通線 */
			// (a) + this->calc_kmで算出
			TRACE("fare(basic)\n");

			fare_tmp = FARE_INFO::Fare_basic_f(_total_jr_calc_km);

			if ((FARE_INFO::tax != 5) && /* IC運賃導入 */
			    IsIC_area(URBAN_ID(this->flag)) &&   /* 近郊区間(最短距離で算出可能) */
				!useBullet) {            /* 新幹線乗車はIC運賃適用外 */
				//ASSERT(companymask == (1 << (JR_EAST - 1)));  /* JR East only  */

				this->fare_ic = fare_tmp;
			}
			_total_jr_fare = round(fare_tmp);
		}

		// JR北あり?
		if (0 < (this->hokkaido_sales_km + this->hokkaido_calc_km)) {
			/* JR東 + JR北 */
			if (this->local_only_as_hokkaido ||
			  (!this->major_only && (_total_jr_sales_km <= 100))) {
				/* JR北は地方交通線のみ */
				/* or 幹線+地方交通線でトータル営業キロが10km以下 */
				// (r) sales_km add
				TRACE("fare(hla)\n");		// TRACE("fare(r,r)\n");
				_total_jr_fare += FARE_INFO::Fare_table("hla", "ha",
				                                    this->hokkaido_sales_km);
                //過去DBもつかうんで。。。ASSERT(FALSE); // 北海道新幹線ができてから不要
			} else { /* 幹線のみ／幹線+地方交通線で10km越え */
				// (o) calc_km add
				TRACE("fare(add, ha)\n");	// TRACE("fare(opq, o)\n");
				_total_jr_fare += FARE_INFO::Fare_table("add", "ha",
				                                    this->hokkaido_calc_km);
			}
		}				// JR九あり？
		if (0 < (this->kyusyu_sales_km + this->kyusyu_calc_km)) {
			/* JR西 + JR九 */
			/* 幹線のみ、幹線+地方交通線 */

			// JR九州側(q)<s><c> 加算
			TRACE("fare(add, ka)\n");	// TRACE("fare(opq, q)\n");
			_total_jr_fare += FARE_INFO::Fare_table("add", "ka",
			                                    this->kyusyu_calc_km);
		}				// JR四あり?
		if (0 < (this->shikoku_sales_km + this->shikoku_calc_km)) {
			/* JR西 + JR四 */
			/* 幹線のみ、幹線+地方交通線 */

			// JR四国側(p)<s><c> 加算
			TRACE("fare(add, sa)\n");	// TRACE("fare(opq, p)\n");
			_total_jr_fare += FARE_INFO::Fare_table("add", "sa",
			                                    this->shikoku_calc_km);
		}				// JR北
	} else if (0 < (this->hokkaido_sales_km + this->hokkaido_calc_km)) {
		/* JR北海道のみ */
		ASSERT(_total_jr_sales_km == this->hokkaido_sales_km);
		ASSERT(_total_jr_calc_km == this->hokkaido_calc_km);
		ASSERT(_total_jr_fare == 0);

		if (this->local_only_as_hokkaido
			|| (!this->major_only && (_total_jr_sales_km <= 100))) {
			/* JR北海道 地方交通線のみ */
			// (j)<s>
			TRACE("fare(hokkaido_sub)\n");
			_total_jr_fare = FARE_INFO::Fare_hokkaido_sub(_total_jr_sales_km);
		} else {
			/* JR北海道 幹線のみ、幹線+地方交通線 */
			// (f)<c>
			TRACE("fare(hokkaido-basic)\n");
			_total_jr_fare = FARE_INFO::Fare_hokkaido_basic(_total_jr_calc_km);
		}				// JR九
	} else if (0 < (this->kyusyu_sales_km + this->kyusyu_calc_km)) {
		/* JR九州のみ */
		ASSERT(_total_jr_sales_km == this->kyusyu_sales_km);
		//ASSERT(_total_jr_calc_km == this->kyusyu_calc_km);
		ASSERT(_total_jr_fare == 0);

		if (this->local_only) {
			/* JR九州 地方交通線 */
			TRACE("fare(ls)'k'\n");
			/* (l) */
			_total_jr_fare = FARE_INFO::Fare_table(_total_jr_calc_km, _total_jr_sales_km, 'k');

		}
		if (_total_jr_fare == 0) {
			/* JR九州 幹線のみ、幹線＋地方交通線、地方交通線のみ の(l), (n)非適用 */
			// (h)<s><c>
			TRACE("fare(kyusyu)\n");			// TRACE("fare(m, h)[9]\n");
			_total_jr_fare = FARE_INFO::Fare_kyusyu(_total_jr_sales_km,
			                                    _total_jr_calc_km);
		}

	} else if (0 < (this->shikoku_sales_km + this->shikoku_calc_km)) {
		/* JR四国のみ */
		ASSERT(_total_jr_sales_km == this->shikoku_sales_km);
		ASSERT(_total_jr_calc_km == this->shikoku_calc_km);
		ASSERT(_total_jr_fare == 0);

		if (this->local_only) {
			/* JR四国 地方交通線 */
			TRACE("fare(ls)'s'\n");
			/* (k) */
			_total_jr_fare = FARE_INFO::Fare_table(_total_jr_calc_km, _total_jr_sales_km, 's');

		}
		if (_total_jr_fare == 0) {
			/* JR四国 幹線のみ、幹線＋地方交通線、地方交通線のみ の(l), (n)非適用 */
			// (g)<s><c>
			TRACE("fare(shikoku)[4]\n");		// TRACE("fare(m, g)[4]\n");
			_total_jr_fare = FARE_INFO::Fare_shikoku(_total_jr_sales_km,
			                                     _total_jr_calc_km);
		}

	} else {
        /* 会社線のみ or BRTのみ */
		//ASSERT(FALSE);   青森始発からIGRで盛岡方面へ
	}
    this->total_jr_sales_km = _total_jr_sales_km + this->brt_sales_km;
    this->total_jr_calc_km = _total_jr_calc_km + this->brt_calc_km;
    this->jr_fare = _total_jr_fare + this->brt_fare - this->brt_discount_fare;
}

// 営業キロから有効日数を得る
//	calc_fare() =>
//
//	@param [in] km    営業キロ
//	@return 有効日数
//
int32_t FARE_INFO::days_ticket(int32_t sales_km)
{
	if (sales_km < 1001) {
		return 1;
	}
	return (sales_km + 1999) / 2000 + 1;
}

//static
//	会社線の運賃を得る
//	calc_fare() => aggregate_fare_info() =>
//
//	@param [in]     station_id1   駅1
//	@param [in]     station_id2   駅2
//	@param [in/out] compantFare   [0]区間運賃 / [1]小児運賃 / [2]学割運賃(非適用は0) / [3]併算割引運賃有無(1=無)
//	@return true : success / false : failuer
//
bool FARE_INFO::Fare_company(int32_t station_id1, int32_t station_id2, CompanyFare* campanyFare)
{
	char sql[256];
	int32_t fare_work;
	static const char tsql[] =
"select fare, academic, flg from t_clinfar"
" where tax=%d and "
" ((station_id1=?1 and station_id2=?2) or"
"  (station_id1=?2 and station_id2=?1))";

	sqlite3_snprintf(sizeof(sql), sql, tsql, FARE_INFO::tax);
	DBO dbo(DBS::getInstance()->compileSql(sql, false));
	dbo.setParam(1, station_id1);
	dbo.setParam(2, station_id2);

	if (dbo.moveNext()) {
		fare_work = campanyFare->fare = dbo.getInt(0);	// fare
		campanyFare->fareAcademic = dbo.getInt(1);	// academic
		campanyFare->passflg = dbo.getInt(2);	// flg

		// (0=5円は切り捨て, 1=5円未満切り上げ)
		if (campanyFare->is_round_up_children_fare()) {
			campanyFare->fareChild = round_up(fare_work / 2);
		} else {
			campanyFare->fareChild = round_down(fare_work / 2);
		}
		return true;
	}
	return false;
}

//static
//	運賃テーブル参照
//	calc_fare() => retr_fare() =>
//
//	@param [in] tbl  table name postfix"
//	@param [in] field name of column for retrieve fare in table.
//	@param [in] km
//	@return fare [yen]
//
int32_t	FARE_INFO::Fare_table(const char* tbl, const char* field, int32_t km)
{
	char sql[128];
	static const char tsql[] =
"select %s%x from t_fare%s where 0<=%s%x and km<=? order by km desc limit(1)";

    TRACE("Fare_table(%s, %s, %d)\n", tbl, field, km);

	sqlite3_snprintf(sizeof(sql), sql, tsql, field, FARE_INFO::tax, tbl, field, FARE_INFO::tax);
	DBO dbo(DBS::getInstance()->compileSql(sql));
	dbo.setParam(1, KM(km));
	if (dbo.moveNext()) {
		return dbo.getInt(0);
	}
	return 0;
}

//static
//	運賃テーブル参照
//	calc_fare() => retr_fare() =>
//
//	@param [in] c   'h'=hokkaido, 'e'= east or central or west
//	@param [in] km
//	@retval 0 db error
//	@retval Number of negative ; -fare
//	@retval Positive of negative ; c_km
//
int32_t	FARE_INFO::Fare_table(const char* tbl, char c, int32_t km)
{
	char* sql;
	int32_t ckm;
	int32_t fare;

    TRACE("Fare_table(%s, %c, %d)\n", tbl, c, km);

    sql = sqlite3_mprintf(
	"select ckm, %c%x from t_fare%s where km<=? order by km desc limit(1)",
	                      c, FARE_INFO::tax, tbl);

	DBO dbo(DBS::getInstance()->compileSql(sql));
	sqlite3_free(sql);
	dbo.setParam(1, KM(km));
	if (dbo.moveNext()) {
		ckm = dbo.getInt(0);
		fare = dbo.getInt(1);
		if (fare == 0) {
			return ckm;
		} else {
			return -fare;
		}
	}
	return 0;
}

//static
//	運賃テーブル参照(ls) JR四国、JR九州 地方交通線のみ
//    JR四国は2023.5より地方交通線特別運賃は廃止されたので、t_farels テーブルは0を返す
//	calc_fare() => retr_fare() =>
//
//	@param [in] dkm   擬制キロ
//	@param [in] skm	  営業キロ
//	@param [in] c     's': 四国 / 'k': 九州
//	@return value
//
int32_t FARE_INFO::Fare_table(int32_t dkm, int32_t skm, char c)
{
	int32_t fare;

    TRACE("Fare_table(%d, %d, %c)\n", dkm, skm, c);

    char* sql = sqlite3_mprintf(
#if 1
	"select %c%x from t_farels where dkm=?2 and (skm=-1 or skm=?1)",
	c, FARE_INFO::tax);
#else
"select k,l from t_farels where "
"(-1=(select skm from t_farels where skm=(select max(skm) from t_farels where skm<=?1))"
"	or"
" skm=(select skm from t_farels where skm=(select max(skm) from t_farels where skm<=?1))"
") and dkm="
"(select dkm from t_farels where dkm=(select max(dkm) from t_farels where dkm<=?2))";
#endif
	DBO dbo(DBS::getInstance()->compileSql(sql));
	sqlite3_free(sql);

	dbo.setParam(1, KM(skm));
	dbo.setParam(2, KM(dkm));
	if (dbo.moveNext()) {
		fare = dbo.getInt(0);
	} else {
		fare = 0;
	}
	return fare;
}


//static
//	特別通過運賃区間か判定し通過していたら加算運賃額を返す
//	calc_fare() => aggregate_fare_info() =>
//
//	@param [in] line_id     路線
//	@param [in] station_id1 駅1
//	@param [in] station_id2 駅2
//
//	@return 駅1～駅2に運賃と区別区間が含まれている場合その加算額を返す
//
int32_t FARE_INFO::CheckSpecificFarePass(int32_t line_id, int32_t station_id1, int32_t station_id2)
{
	char* sql = sqlite3_mprintf(
"select station_id1, station_id2, fare%up from t_farespp f where kind=0 and exists ("
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
" order by fare%up desc"
" limit(1)", FARE_INFO::tax, FARE_INFO::tax);

	DBO dbo(DBS::getInstance()->compileSql(sql));
	dbo.setParam(1, line_id);
	dbo.setParam(2, station_id1);
	dbo.setParam(3, station_id2);
	sqlite3_free(sql);
	if (dbo.moveNext()) {
		int32_t fare = dbo.getInt(2);
		TRACE(_T("CheckSpecificFarePass found: %s, %s, +%d\n"), SNAME(dbo.getInt(0)), SNAME(dbo.getInt(1)), fare);
		/* found, return values is add fare */
		return fare;
	}
	return 0;	/* not found */
}

//static
//	特別運賃区間か判定し該当していたら運賃額を返す
//	calc_fare() =>
//
//	@param [in] station_id1 駅1(departureStationId)
//	@param [in] station_id2 駅2(arriveStationId)
//
//	@return 特別区間運賃
//
int32_t FARE_INFO::SpecificFareLine(int32_t station_id1, int32_t station_id2, int32_t kind)
{
	char sql[256];

	static const char tsql[] =
	"select fare%up from t_farespp where kind=?3 and"
	" ((station_id1=?1 and station_id2=?2) or"
	"  (station_id1=?2 and station_id2=?1))";

	sqlite3_snprintf(sizeof(sql), sql, tsql, FARE_INFO::tax);

	DBO dbo(DBS::getInstance()->compileSql(sql));
	dbo.setParam(1, station_id1);
	dbo.setParam(2, station_id2);
	dbo.setParam(3, kind);
	if (dbo.moveNext()) {
		int32_t fare = dbo.getInt(0);
		TRACE(_T("SpecificFareLine found: %s - %s, +%d\n"), SNAME(station_id1), SNAME(station_id2), fare);
		return fare;	/* fare */
	}
	return 0;	/* not found */
}


//	運賃計算(基本)
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    計算キロ
//	@return 運賃額
//
int32_t FARE_INFO::Fare_basic_f(int32_t km)
{
	int32_t fare;
	int32_t c_km;

	fare = FARE_INFO::Fare_table("bspekm", "b", km);
	if (0 != fare) {
		return fare;
	}
    /* After 2025, less than 101 km doesn't pass  the following block. */
	if (km < 31) {							// 1 to 3km
        if (FARE_INFO::tax == 10) {
            return 147;
		} else if (FARE_INFO::tax == 5) {
			return 140;
		} else {
			return 144;
		}
	}
	if (km < 61) {							// 4 to 6km
        if (FARE_INFO::tax == 10) {
            return 189;
		} else if (FARE_INFO::tax == 5) {
			return 180;
		} else {
			return 185;
		}
	}
	if (km < 101) {							// 7 to 10km
        if (FARE_INFO::tax == 10) {
            return 199;
        } else if (FARE_INFO::tax == 5) {
			return 190;
		} else {
			return 195;
		}
	}
	if (6000 < km) {						// 600km越えは40キロ刻み
		c_km = (km - 1) / 400 * 400 + 200;
	} else if (1000 < km) {					// 100.1-600kmは20キロ刻み
		c_km = (km - 1) / 200 * 200 + 100;
	} else if (500 < km) {					// 50.1-100kmは10キロ刻み
		c_km = (km - 1) / 100 * 100 + 50;
	} else if (100 < km) {					// 10.1-50kmは5キロ刻み
		c_km = (km - 1) / 50 * 50 + 30;
	} else {
		ASSERT(FALSE);
		c_km = 0;
	}
	if (6000 <= c_km) {
		fare = 1620 * 3000 + 1285 * (6000 - 3000) + 705 * (c_km - 6000);
	} else if (3000 < c_km) {
		fare = 1620 * 3000 + 1285 * (c_km - 3000);
	} else {
		fare = 1620 * c_km;
	}
	if (c_km <= 1000) {						// 100km以下は切り上げ
		// 1の位を切り上げ
		fare = (fare + 9999) / 10000 * 10;
	} else {								// 100㎞越えは四捨五入
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd_ic(fare, FARE_INFO::tax);	// tax = +5%, 四捨五入
}

//	sub: 地方交通線
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    営業キロ
//	@return 運賃額
//
int32_t FARE_INFO::Fare_sub_f(int32_t km)
{
	int32_t fare;
	int32_t c_km;

    if (FARE_INFO::tax != 10) {
        if (km < 31) {							// 1 to 3km
            if (FARE_INFO::tax == 5) {
                return 140;
            } else {
                return 144;
            }
        }
        if (km < 61) {							// 4 to 6km
            if (FARE_INFO::tax == 5) {
                return 180;
            } else {  // 8%
                return 185;
            }
        }
        if (km < 101) {							// 7 to 10km
            if (FARE_INFO::tax == 5) {
                return 200;
            } else {
                return 206;
            }
        }
    }
	if (12000 < km) {		// 1200km越えは別表第2号イの4にない
		ASSERT(FALSE);
		return -1;
	}

	c_km = Fare_table("lspekm", 'e', km);
	if (c_km == 0) {
		ASSERT(FALSE);
		return -1;
	}
	if (c_km < 0) {
		return -c_km;		/* fare 第77条の5の3*/
	}

    TRACE("Fare_sub_f: c_km=%d\n", c_km);

	/* c_km : 第77条の5の2 */
	c_km *= 10;

	/* 第77条の5 */
	if (5460 <= c_km) {
		fare = 1780 * 2730 + 1410 * (5460 - 2730) + 770 * (c_km - 5460);
	} else if (2730 < c_km) {
		fare = 1780 * 2730 + 1410 * (c_km - 2730);
	} else {
		fare = 1780 * c_km;
	}
	if (c_km <= 1000) {						// 100km以下は切り上げ
		// 1の位を切り上げ
		fare = (fare + 9999) / 10000 * 10;
	} else {				// 100㎞越えは四捨五入
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd_ic(fare, FARE_INFO::tax);
}

//	b: 電車特定区間(東京)
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    営業キロ
//	@return 運賃額
//
int32_t FARE_INFO::Fare_tokyo_f(int32_t km)
{
	int32_t fare;
	int32_t c_km;

	fare = FARE_INFO::Fare_table("bspekm", "t", km);
	if (0 != fare) {
		return fare;
	}
    /* After 2025, less than 101 km doesn't pass  the following block. */
	if (km < 31) {							// 1 to 3km
        if (FARE_INFO::tax == 10) {
            return 136;
        } else if (FARE_INFO::tax == 5) {
			return 130;
		} else {
			return 133;
		}
	}
	if (km < 61) {							// 4 to 6km
        if (FARE_INFO::tax == 10) {
            return 157;
        } else if (FARE_INFO::tax == 5) {
			return 150;
		} else {
			return 154;
		}
	}
	if (km < 101) {							// 7 to 10km
        if (FARE_INFO::tax == 10) {
            return 168;
        } else if (FARE_INFO::tax == 5) {
			return 160;
		} else {
			return 165;
		}
	}

	if (6000 < km) {						// 600km越えは40キロ刻み
		ASSERT(FALSE);
		return -1;
	} else if (1000 < km) {					// 100.1-600kmは20キロ刻み
		c_km = (km - 1) / 200 * 200 + 100;
	} else if (500 < km) {					// 50.1-100kmは10キロ刻み
		c_km = (km - 1) / 100 * 100 + 50;
	} else if (100 < km) {					// 10.1-50kmは5キロ刻み
		c_km = (km - 1) / 50 * 50 + 30;
	} else {
		ASSERT(FALSE);
		c_km = 0;
	}
	if (3000 < c_km) {
		fare = 1530 * 3000 + 1215 * (c_km - 3000);
	} else {
		fare = 1530 * c_km;
	}
	if (c_km <= 1000) {						// 100km以下は切り上げ
		// 1の位を切り上げ
		fare = (fare + 9999) / 10000 * 10;
	} else {				// 100㎞越えは四捨五入
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd_ic(fare, FARE_INFO::tax);
}

//	C: 電車特定区間(大阪)
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    営業キロ
//	@return 運賃額
//
int32_t FARE_INFO::Fare_osaka(int32_t km)
{
	int32_t fare;
	int32_t c_km;

	fare = FARE_INFO::Fare_table("bspekm", "o", km);
	if (0 != fare) {
		return fare;
	}
    /* After 2025, less than 101 km doesn't pass  the following block. */
    if (km < 31) {							// 1 to 3km
        if (FARE_INFO::tax == 10) {
            return 140;
        } else if (FARE_INFO::tax == 5) {
			return 120;
		} else {
			return 120;
		}
	}
	if (km < 61) {							// 4 to 6km
        if (FARE_INFO::tax == 10) {
            return 170;
        } else if (FARE_INFO::tax == 5) {
			return 160;
		} else {
			return 160;
		}
	}
	if (km < 101) {							// 7 to 10km
        if (FARE_INFO::tax == 10) {
            return 190;
        } else if (FARE_INFO::tax == 5) {
			return 170;
		} else {
			return 180;
		}
	}
	if (6000 < km) {						// 600km越えは40キロ刻み
		ASSERT(FALSE);
		return -1;
	} else if (1000 < km) {					// 100.1-600kmは20キロ刻み
		c_km = (km - 1) / 200 * 200 + 100;
	} else if (500 < km) {					// 50.1-100kmは10キロ刻み
		c_km = (km - 1) / 100 * 100 + 50;
	} else if (100 < km) {					// 10.1-50kmは5キロ刻み
		c_km = (km - 1) / 50 * 50 + 30;
	} else {
		ASSERT(FALSE);
		c_km = 0;
	}

    /* 2025 賃率変更しているので(2023.3改訂) tax5,8は不正な金額を返すで(このロジック使用する他もそうだが)*/
	if (3000 < c_km) {
		fare = 1550 * 3000 + 1230 * (c_km - 3000);
	} else {
		fare = 1550 * c_km;
	}
	if (c_km <= 1000) {						// 100km以下は切り上げ
		// 1の位を切り上げ
		fare = (fare + 9999) / 10000 * 10;
	} else {								// 100㎞越えは四捨五入
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd(fare, FARE_INFO::tax);	// tax = +5%, 四捨五入
}

//	d: 電車特定区間(山手線)
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    営業キロ
//	@return 運賃額
//
int32_t FARE_INFO::Fare_yamate_f(int32_t km)
{
	int32_t fare;
	int32_t c_km;

	fare = FARE_INFO::Fare_table("bspekm", "y", km);
	if (0 != fare) {
		return fare;
	}
    /* After 2025, less than 101 km doesn't pass  the following block. */
	if (km < 31) {							// 1 to 3km
        if (FARE_INFO::tax == 10) {
            return 146;
        } else if (FARE_INFO::tax == 5) {
			return 130;
		} else {
			return 133;
		}
	}
	if (km < 61) {							// 4 to 6km
        if (FARE_INFO::tax == 10) {
            return 167;
        } else if (FARE_INFO::tax == 5) {
			return 150;
		} else {
			return 154;
		}
	}
	if (km < 101) {							// 7 to 10km
        if (FARE_INFO::tax == 10) {
            return 178;
        } else if (FARE_INFO::tax == 5) {
			return 160;
		} else {
			return 165;
		}
	}
	if (3000 < km) {						// 300km越えは未定義
		ASSERT(FALSE);
		return -1;
	} else if (1000 < km) {					// 100.1-300kmは20キロ刻み
		c_km = (km - 1) / 200 * 200 + 100;
	} else if (500 < km) {					// 50.1-100kmは10キロ刻み
		c_km = (km - 1) / 100 * 100 + 50;
	} else if (100 < km) {					// 10.1-50kmは5キロ刻み
		c_km = (km - 1) / 50 * 50 + 30;
	} else {
		ASSERT(FALSE);
		c_km = 0;
	}
	fare = 1325 * c_km;

	if (c_km <= 1000) {	// 100km以下は切り上げ
		// 1の位を切り上げ
		fare = (fare + 9999) / 10000 * 10;
	} else {								// 100㎞越えは四捨五入
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd_ic(fare, FARE_INFO::tax);	// tax = +5%, 四捨五入
}

//	e: 電車特定区間(大阪環状線)
//	calc_fare() => retr_fare() =>
//  2025.4.1 廃止
//
//	@param [in] km    営業キロ
//	@return 運賃額
//
int32_t FARE_INFO::Fare_osakakan(int32_t km)
{
	int32_t fare;
	int32_t c_km;

	if (km < 31) {							// 1 to 3km
        if (FARE_INFO::tax == 10) {
            return 130;
        } else if (FARE_INFO::tax == 5) {
			return 120;
		} else {
			return 120;
		}
	}
	if (km < 61) {							// 4 to 6km
        if (FARE_INFO::tax == 10) {
            return 160;
        } else if (FARE_INFO::tax == 5) {
			return 160;
		} else {
			return 160;
		}
	}
	if (km < 101) {							// 7 to 10km
        if (FARE_INFO::tax == 10) {
            return 180;
        } else if (FARE_INFO::tax == 5) {
			return 170;
		} else {
			return 180;
		}
	}
	if (3000 < km) {						// 300km越えは未定義
		ASSERT(FALSE);
		return -1;
	} else if (1000 < km) {					// 100.1-300kmは20キロ刻み
		c_km = (km - 1) / 200 * 200 + 100;
	} else if (500 < km) {					// 50.1-100kmは10キロ刻み
		c_km = (km - 1) / 100 * 100 + 50;
	} else if (100 < km) {					// 10.1-50kmは5キロ刻み
		c_km = (km - 1) / 50 * 50 + 30;
	} else {
		ASSERT(FALSE);
		c_km = 0;
	}
	fare = 1325 * c_km;

	if (c_km <= 1000) {						// 100km以下は切り上げ
		// 1の位を切り上げ
		fare = (fare + 9999) / 10000 * 10;
	} else {								// 100㎞越えは四捨五入
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd(fare, FARE_INFO::tax);	// tax = +5%, 四捨五入
}

//	f: JR北海道幹線
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    計算キロ
//	@return 運賃額
//
int32_t FARE_INFO::Fare_hokkaido_basic(int32_t km)
{
	int32_t fare;
	int32_t c_km;

	fare = FARE_INFO::Fare_table("bspekm", "h", km);
	if (0 != fare) {
		return fare;
	}

    TRACE(_T("Fare_hokkaido_basic: Calculate ckm for %d\n"), km);

    /* 第77条の2 */
    if (6000 < km) {						// 600km越えは40キロ刻み
        c_km = (km - 1) / 400 * 400 + 200;
    } else {
        ASSERT(FALSE);
        return 99999;
    }
    fare = 2116 * 2000 + 1636 * (3000 - 2000) + 1283 * (6000 - 3000) + 705 * (c_km - 6000);

    // 100㎞越えは四捨五入
    fare = (fare + 50000) / 100000 * 100;

    return taxadd(fare, FARE_INFO::tax);	// tax = +5%, 四捨五入
}

//	JR北海道地方交通線
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    営業キロ
//	@return 運賃額
//
int32_t FARE_INFO::Fare_hokkaido_sub(int32_t km)
{
	int32_t fare;
	int32_t c_km;

	if (12000 < km) {		// 1200km越えは別表第2号イの4にない
		ASSERT(FALSE);
		return -1;
	}

	c_km = Fare_table("lspekm", 'h', km); /* under 800km */
	if (c_km == 0) {
		ASSERT(FALSE);
		return -1;
	}
	if (c_km < 0) {
		return -c_km;		/* fare */
	}

    TRACE(_T("Fare_hokkaido_sub: Calculate ckm=%d\n"), c_km);

    c_km *= 10;
    /* 第77条の6(2) */
	if (5460 <= c_km) {
		fare = 2311 * 1820 + 1835 * (2730 - 1820) + 1402 * (5460 - 2730) + 772 * (c_km - 5460);
	} else if (2730 < c_km) { /* Normally It doesn't pass below here. */
		fare = 2311 * 1820 + 1835 * (2730 - 1820) + 1402 * (c_km - 2730);
	} else if (1820 < c_km) {
		fare = 2311 * 1820 + 1835 * (c_km - 1820);
	} else { /* <= 182km */
		fare = 2311 * c_km;
	}
	if (c_km <= 1000) {						// 100km以下は切り上げ
		// 1の位を切り上げ
		fare = (fare + 9999) / 10000 * 10;
	} else {				// 100㎞越えは四捨五入
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd(fare, FARE_INFO::tax);
}

//	g: JR四国 幹線
//	calc_fare() => retr_fare() =>
//
//	@param [in] skm    営業キロ
//	@param [in] ckm    計算キロ
//	@return 運賃額
//
int32_t FARE_INFO::Fare_shikoku(int32_t skm, int32_t ckm)
{
	int32_t fare;
	int32_t c_km;

	/* JTB時刻表 C-3表 */
	if (ckm != skm) {
        if (FARE_INFO::tax == 10) {
                /* JR四国 幹線+地方交通線 */
                /* (m) */
#if 0 /* (2023.4 廃止) */
            if ((KM(ckm) == 4) && (KM(skm) == 3)) {
                return 170;	/* \ */
            } else if ((KM(ckm) == 11) && (KM(skm) == 10)) {
                return 240;	/* \ */
            }
#endif
        } else if (FARE_INFO::tax == 5) {
			/* JR四国 幹線+地方交通線 */
			/* (m) */
			if ((KM(ckm) == 4) && (KM(skm) == 3)) {
				return 160;	/* \ */
			} else if ((KM(ckm) == 11) && (KM(skm) == 10)) {
				return 230;	/* \ */
			}
		} else {
				/* JR四国 幹線+地方交通線 */
				/* (m) */
			if ((KM(ckm) == 4) && (KM(skm) == 3)) {
				return 160;	/* \ */
			} else if ((KM(ckm) == 11) && (KM(skm) == 10)) {
				return 230;	/* \ */
			}
			/*** JR四国は消費税8%でもここは据え置きみたい ***/
		}
	}

	fare = FARE_INFO::Fare_table("bspekm", "s", ckm);
	if (0 != fare) {
		return fare;
	}
    TRACE(_T("Fare_shikoku: Calculate ckm=%d\n"), ckm);

	if (6000 < ckm) {						// 600km越えは40キロ刻み
		c_km = (ckm - 1) / 400 * 400 + 200;
	} else if (1000 < ckm) {					// 100.1-600kmは20キロ刻み
		c_km = (ckm - 1) / 200 * 200 + 100;
	} else if (500 < ckm) {					// 50.1-100kmは10キロ刻み
		c_km = (ckm - 1) / 100 * 100 + 50;
	} else if (100 < ckm) {					// 10.1-50kmは5キロ刻み
		c_km = (ckm - 1) / 50 * 50 + 30;
	} else {
		/* 0-10kmは上で算出return済み */
		ASSERT(FALSE);
		c_km = 0;
	}
    if (FARE_INFO::tax != 10) {
            /* JR四国 幹線+地方交通線 */
            /* (m) */
        if (6000 <= c_km) {
            fare = 1821 * 1000 + 1620 * (3000 - 1000) + 1285 * (6000 - 3000) + 705 * (c_km - 6000);
        } else if (3000 < c_km) {
            fare = 1821 * 1000 + 1620 * (3000 - 1000) + 1285 * (c_km - 3000);
        } else if (1000 < c_km) {
            fare = 1821 * 1000 + 1620 * (c_km - 1000);
        } else {
            /* 10.1km - 100.0 km */
            fare = 1821 * c_km;
        }
    } else {
        /* 2023.5 update */
        if (6000 <= c_km) {
            fare = 1920 * 1000 + 1620 * (3000 - 1000) + 1285 * (6000 - 3000) + 705 * (c_km - 6000);
        } else if (3000 < c_km) {
            fare = 1920 * 1000 + 1620 * (3000 - 1000) + 1285 * (c_km - 3000);
        } else if (1000 < c_km) {
            fare = 1920 * 1000 + 1620 * (c_km - 1000);
        } else {
            /* 10.1km - 100.0 km */
            fare = 1920 * c_km;
        }
    }
	if (c_km <= 1000) {						// 100km以下は切り上げ
		// 1の位を切り上げ
		fare = (fare + 9999) / 10000 * 10;
	} else {								// 100㎞越えは四捨五入
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd(fare, FARE_INFO::tax);	// tax = +5%, 四捨五入
}

//	h: JR九州 幹線
//	calc_fare() => retr_fare() =>
//
//	@param [in] skm    営業キロ
//	@param [in] ckm    計算キロ
//	@return 運賃額
//
int32_t FARE_INFO::Fare_kyusyu(int32_t skm, int32_t ckm)
{
	int32_t fare;
	int32_t c_km;

	/* JTB時刻表 C-3表 */
	if (ckm != skm) {
        if (FARE_INFO::tax == 10) {
            /* JR九州 幹線+地方交通線 */
            /* (n) */
            if ((KM(ckm) == 4) && (KM(skm) == 3)) {
                return 210;	/* \ */
            } else if ((KM(ckm) == 11) && (KM(skm) == 10)) {
                return 320;	/* \ */
            }
        } else if (FARE_INFO::tax == 5) {
			/* JR九州 幹線+地方交通線 */
			/* (n) */
			if ((KM(ckm) == 4) && (KM(skm) == 3)) {
				return 170;	/* \ */
			} else if ((KM(ckm) == 11) && (KM(skm) == 10)) {
				return 240;	/* \ */
			}
		} else {
				/* JR九州 幹線+地方交通線 */
				/* (n) */
			if ((KM(ckm) == 4) && (KM(skm) == 3)) {
				return 180;	/* \ */
			} else if ((KM(ckm) == 11) && (KM(skm) == 10)) {
				return 250;	/* \ */
			}
		}
	}

	fare = FARE_INFO::Fare_table("bspekm", "k", ckm);
	if (0 != fare) {
		return fare;
	}

    TRACE(_T("Fare_kyusyu: Calculate ckm=%d\n"), ckm);

	if (6000 < ckm) {						// 600km越えは40キロ刻み
		c_km = (ckm - 1) / 400 * 400 + 200;
	} else if (1000 < ckm) {				// 100.1-600kmは20キロ刻み
		c_km = (ckm - 1) / 200 * 200 + 100;
	} else if (500 < ckm) {					// 50.1-100kmは10キロ刻み
		c_km = (ckm - 1) / 100 * 100 + 50;
	} else if (100 < ckm) {					// 10.1-50kmは5キロ刻み
		c_km = (ckm - 1) / 50 * 50 + 30;
	} else {
		/* 0-10kmは上で算出return済み */
		ASSERT(FALSE);
		c_km = 0;
	}

    if (FARE_INFO::tax != 10) {
        if (6000 <= c_km) {
            fare = 1775 * 3000 + 1285 * (6000 - 3000) + 705 * (c_km - 6000);
        } else if (3000 < c_km) {
            fare = 1775 * 3000 + 1285 * (c_km - 3000);
        } else if (1000 < c_km) {
            fare = 1775 * c_km;
        } else {
            /* 10.1-100.0kmはDBで定義 */
            ASSERT(FALSE);
            fare = 0;
        }
    } else {
        /* 2023.5 update */
        if (6000 <= c_km) {
            fare = 1975 * 3000 + 1285 * (6000 - 3000) + 705 * (c_km - 6000);
        } else if (3000 < c_km) {
            fare = 1975 * 3000 + 1285 * (c_km - 3000);
        } else if (1000 < c_km) {
            fare = 1975 * c_km;
        } else {
            /* 10.1-100.0kmはDBで定義 */
            ASSERT(FALSE);
            fare = 0;
        }
    }
	if (c_km <= 1000) {							// 100km以下は切り上げ
		// 1の位を切り上げ
		fare = (fare + 9999) / 10000 * 10;
	} else {									// 100㎞越えは四捨五入
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd(fare, FARE_INFO::tax);	// tax = +5%, 四捨五入
}
