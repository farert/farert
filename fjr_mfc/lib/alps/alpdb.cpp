#include "stdafx.h"

/*!	@file alpdb.cpp core logic implement.
 *	Copyright(c) sutezo9@me.com 2012.
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



// 経路マスクビットパターンマスク
#define JctMaskOn(jctid)  	jct_mask[(jctid) / 8] |= (1 << ((jctid) % 8))
#define JctMaskOff(jctid) 	jct_mask[(jctid) / 8] &= ~(1 << ((jctid) % 8))
#define JctMaskClear()   	memset(jct_mask, 0, sizeof(jct_mask))
#define IsJctMask(jctid)	((jct_mask[(jctid) / 8] & (1 << ((jctid) % 8))) != 0)


////////////////////////////////////////////
//	static member

/*static */ int DbidOf::StationIdOf_SHINOSAKA = 0;		// 新大阪
/*static */ int DbidOf::StationIdOf_OSAKA = 0;    		// 大阪
/*static */ int DbidOf::StationIdOf_KOUBE = 0;     		// 神戸
/*static */ int DbidOf::StationIdOf_HIMEJI = 0;    		// 姫路
/*static */ int DbidOf::StationIdOf_NISHIAKASHI = 0;    // 西明石
/*static */ int DbidOf::LineIdOf_TOKAIDO = 0;       	// 東海道線
/*static */ int DbidOf::LineIdOf_SANYO = 0;        		// 山陽線
/*static */ int DbidOf::LineIdOf_SANYOSHINKANSEN = 0; 	// 山陽新幹線
/*static */ int DbidOf::LineIdOf_HAKATAMINAMISEN = 0; 	// 博多南線

/*static */ int DbidOf::StationIdOf_KITASHINCHI = 0;  	// 北新地
/*static */ int DbidOf::StationIdOf_AMAGASAKI = 0;  	// 尼崎

/*static */ int DbidOf::StationIdOf_KOKURA = 0;  		// 小倉
/*static */ int DbidOf::StationIdOf_NISHIKOKURA = 0;  	// 西小倉
/*static */ int DbidOf::StationIdOf_HAKATA = 0;  		// 博多
/*static */ int DbidOf::StationIdOf_YOSHIZUKA = 0;  	// 吉塚


////////////////////////////////////////////
//	DbidOf
//
DbidOf::DbidOf()
{
	if (!DbidOf::StationIdOf_SHINOSAKA) {	/* 前処理 キャッシュ処理 */
		DbidOf::StationIdOf_SHINOSAKA 	 = Route::GetStationId(_T("新大阪"));
		DbidOf::StationIdOf_OSAKA 		 = Route::GetStationId(_T("大阪"));
		DbidOf::StationIdOf_KOUBE 		 = Route::GetStationId(_T("神戸"));
		DbidOf::StationIdOf_HIMEJI 		 = Route::GetStationId(_T("姫路"));
		DbidOf::StationIdOf_NISHIAKASHI  = Route::GetStationId(_T("西明石"));
		DbidOf::LineIdOf_TOKAIDO 		 = Route::GetLineId(_T("東海道線"));
		DbidOf::LineIdOf_SANYO 			 = Route::GetLineId(_T("山陽線"));
		DbidOf::LineIdOf_SANYOSHINKANSEN = Route::GetLineId(_T("山陽新幹線"));
		DbidOf::LineIdOf_HAKATAMINAMISEN = Route::GetLineId(_T("博多南線"));

		DbidOf::StationIdOf_KITASHINCHI = Route::GetStationId(_T("北新地"));
		DbidOf::StationIdOf_AMAGASAKI = Route::GetStationId(_T("尼崎"));

		DbidOf::StationIdOf_KOKURA = Route::GetStationId(_T("小倉"));
		DbidOf::StationIdOf_NISHIKOKURA = Route::GetStationId(_T("西小倉"));
		DbidOf::StationIdOf_HAKATA = Route::GetStationId(_T("博多"));
		DbidOf::StationIdOf_YOSHIZUKA = Route::GetStationId(_T("吉塚"));
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

	ASSERT(0 < DbidOf::StationIdOf_KOKURA);
	ASSERT(0 < DbidOf::StationIdOf_NISHIKOKURA);
	ASSERT(0 < DbidOf::StationIdOf_HAKATA);
	ASSERT(0 < DbidOf::StationIdOf_YOSHIZUKA);
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

//	if (lineId <= 0) {
		flag = Route::AttrOfStationId((int)stationId_) & MASK_ROUTE_FLAG_SFLG;
//	} else {
//		flag = Route::AttrOfStationOnLineLine((int)lineId_, (int)stationId_);
//	}
}

//	constructor
//	@param [in] lineId_    路線
//	@param [in] stationId_ 駅
//	@param [in] flag_      mask(bit31-16のみ有効)
//
RouteItem::RouteItem(IDENT lineId_, IDENT stationId_, SPECIFICFLAG flag_) 
{
	lineId = lineId_;
	stationId = stationId_;
	flag = Route::AttrOfStationId((int)stationId_) & MASK_ROUTE_FLAG_SFLG;
	flag |= (flag_ & MASK_ROUTE_FLAG_LFLG);
}

//////////////////////////////////////////////////////

// 	文字列は「漢字」か「かな」か？
//	「かな」ならTrueを返す
// 
// 	@param [in] szStr    文字列
// 	@retval true  文字列はひらがなまたはカタカナのみである
// 	@retval false 文字列はひらがな、カタカナ以外の文字が含まれている
// 
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

//	カナをかなに
//	
//	@param [in][out] kana_str  変換文字列
//
void conv_to_kana2hira(tstring& kana_str)
{
	tstring::iterator i = kana_str.begin();
	while (i != kana_str.end()) {
		if (iskana(*i)) {
			*i = *i - _T('ァ') + _T('ぁ');
		}
		i++;
	}
}

//	3桁毎にカンマを付加した数値文字列を作成
//	(1/10した小数点付き営業キロ表示用)
//
//	@param [in] num  数値
//	@return 変換された文字列(ex. 43000 -> "4,300.0", 25793 -> "2,579.3")
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

//	3桁毎にカンマを付加した数値文字列を作成
//	金額表示用
//
//	@param [in] num  数値
//	@return 変換された文字列(ex. 43000 -> "\43,000", 3982003 -> "3,982,003")
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
//	Routeクラス インプリメント
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
//	会社&都道府県の列挙
//
//	@return DBクエリ結果オブジェクト(会社(JRのみ)、都道府県一覧)
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
//	都道府県or会社に属する路線の列挙(JRのみ)
//
//	@return DBクエリ結果オブジェクト(路線)
//
DBO Route::Enum_lines_from_company_prefect(int id)
{
	char sql[300];
	const char tsql[] = 
"/**/select n.name, line_id, lflg from t_line n"
" left join t_lines l on n.rowid=l.line_id"
" left join t_station t on t.rowid=l.station_id"
" where %s=%d"
" and (l.lflg&((1<<18)|(1<<31)|(1<<17)))=0"
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
//	駅名のパターンマッチの列挙(ひらがな、カタカナ、ひらがなカタカナ混じり、漢字）
//
//	@param [in] station   えきめい
//	@return DBクエリ結果オブジェクト(駅名)
//
DBO Route::Enum_station_match(LPCTSTR station)
{
	char sql[256];
	const char tsql[] = "/**/select name, rowid, samename from t_station where (sflg&(1<<18))=0 and %s like '%q%%'";
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
//	駅(Id)の都道府県を得る 
//
//	@param [in] stationId   駅ident
//	@return 都道府県名
//
tstring Route::GetPrefectByStationId(int stationId)
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
DBO Route::Enum_station_located_in_prefect_or_company_and_line(int prefectOrCompanyId, int lineId)
{
	static const  char tsql[] =
//"select t.name, station_id from t_lines l left join t_station t on t.rowid=l.station_id "
//" where line_id=? and %s=? order by sales_km";
"/**/select t.name, station_id from t_station t left join t_lines l on t.rowid=l.station_id"
" where line_id=? and %s=? and (l.lflg&((1<<18)|(1<<31)|(1<<16)))=0 order by sales_km";

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
//	駅名のよみを得る
//
//	@param [in] stationId   駅ident
//	@return えきめい
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
//	駅の所属路線のイテレータを返す
//
//	@param [in] stationId   駅ident
//	@return DBクエリ結果オブジェクト(路線)
//	@return field0(text):路線名, field1(int):路線id, field2(int):lflg(bit31のみ)
//
DBO Route::Enum_line_of_stationId(int stationId)
{
	static const  char tsql[] =
"select n.name, line_id, lflg from t_line n left join t_lines l on n.rowid=l.line_id"
//" where station_id=? and (lflg&((1<<31)|(1<<17)))=0 and sales_km>=0";
" where station_id=? and (lflg&(1<<17))=0";

	DBO dbo = DBS::getInstance()->compileSql(tsql, true);
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
DBO Route::Enum_junction_of_lineId(int lineId, int stationId)
{
	static const  char tsql[] =
#if 1	// 分岐特例のみの乗換駅(山形とか)を含む
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
" where line_id=?1 and ((sflg & (1<<12))<>0 or station_id=?2)"
//" and (lflg&((1<<31)|(1<<17)))=0 and sales_km>=0"
" and (lflg&(1<<17))=0"
" order by l.sales_km";
#endif


	DBO dbo = DBS::getInstance()->compileSql(tsql, true);
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
DBO Route::Enum_station_of_lineId(int lineId)
{
	static const  char tsql[] =
"select t.name, station_id, sflg&(1<<12)"
" from t_lines l left join t_station t on t.rowid=l.station_id"
" where line_id=?"
" and (lflg&((1<<31)|(1<<17)))=0"
" order by l.sales_km";

	DBO dbo = DBS::getInstance()->compileSql(tsql, true);
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
DBO Route::Enum_neer_node(int stationId)
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
"					  and	0<=y.sales_km and (1<<15)=(y.lflg&((1<<31)|(1<<17)|(1<<15)))"
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
	DBO dbo = DBS::getInstance()->compileSql(tsql, true);
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
int Route::NumOfNeerNode(int stationId)
{
	if (STATION_IS_JUNCTION(stationId)) {
		return 2;	// 2以上あることもあるが嘘つき
	}
	DBO dbo =  Route::Enum_neer_node(stationId);
	int c;
	
	c = 0;
	while (dbo.moveNext()) {
		++c;
	}
	return c;
}

// 駅ID(分岐駅)の最寄りの分岐駅を得る(全路線）
// [jct_id, calc_km, line_id][N] = f(jct_id)
//
//	@param [in] jctId   分岐駅
//	@return 分岐駅[0]、計算キロ[1]、路線[2]
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
//	路線のbegin_station_id駅からto_station_id駅までの分岐駅リストを返す
//
//	注： lflg&(1<<17)を含めていないため、新幹線内分岐駅、たとえば、
//	     東海道新幹線 京都 米原間に草津駅は存在するとして返します.
//
//	@param [in] line_id          路線
//	@param [in] begin_station_id 発
//	@param [in] to_station_id    至
//
//	@return DBO& iterator:int 分岐ID
//
DBO Route::Enum_junctions_of_line(int line_id, int begin_station_id, int to_station_id)
{
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

	DBO dbo = DBS::getInstance()->compileSql(tsql, true);
	if (dbo.isvalid()) {
		dbo.setParam(1, line_id);
		dbo.setParam(2, begin_station_id);
		dbo.setParam(3, to_station_id);
	}
	return dbo;
}

//static
//	70条進入路線、脱出路線から進入、脱出境界駅と営業キロ、路線IDを返す
//
//	@param [in] line_id    大環状線進入／脱出路線
//	@return 一番外側の大環状線内(70条適用)駅
//
//	@note 東京都区内なので、営業キロMAX固定(下り即ち東京から一番遠い70条適用駅)とする
//
int Route::RetrieveOut70Station(int line_id)
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
	DBO dbo = DBS::getInstance()->compileSql(tsql, true);
	if (dbo.isvalid()) {
		dbo.setParam(1, line_id);
	}
	if (dbo.moveNext()) {
		return dbo.getInt(0);
	}
	return 0;
}


bool Route::chk_jctsb_b(int kind, int num)
{
	DbidOf dbid;

	switch (kind) {
	case JCTSP_B_NISHIKOKURA:
		/* 博多-新幹線-小倉*/
		return (2 <= num) && 
			(dbid.LineIdOf_SANYOSHINKANSEN == route_list_raw.at(num - 1).lineId) &&
			(dbid.StationIdOf_KOKURA == route_list_raw.at(num - 1).stationId) &&
			(dbid.StationIdOf_HAKATA == route_list_raw.at(num - 2).stationId);
		break;
	case JCTSP_B_YOSHIZUKA:
		/* 小倉-新幹線-博多 */
		return (2 <= num) && 
			(dbid.LineIdOf_SANYOSHINKANSEN == route_list_raw.at(num - 1).lineId) &&
			(dbid.StationIdOf_HAKATA == route_list_raw.at(num - 1).stationId) &&
			(dbid.StationIdOf_KOKURA == route_list_raw.at(num - 2).stationId);
		break;
	default:
		break;
	}
	return false;
}

/*	経路追加
 *
 *	@param [in] line_id      路線ident
 *	@param [in] stationId1   駅1 ident
 *	@param [in] stationId2   駅2 ident
 *	@param [in] ctlflg       デフォルト0(All Off)
 *							 bit8: 新幹線乗換チェックしない
 *
 *  @retval 0 = OK(last)
 *  @retval 1 = OK
 *  //@retval 2 = OK(re-route)
 *  @retval -1 = overpass(復乗エラー)
 *  @retval -2 = 経路エラー(stationId1 or stationId2はline_id内にない)
 *  @retval -3 = operation error(開始駅未設定)
 *  @retval -100 DB error
 *	@retval last_flag bit4-0 : reserve
 *	@retval last_flag bit5=1 : 次にremoveTailでlastItemの通過マスクをOffする(typeOでもPでもないので)
 *	@retval last_flag bit5=0 : 次にremoveTailでlastItemの通過マスクをOffする(typeOでもPでもないので)
 *	@retval last_flag bit6=1 : 分岐特例区間指定による経路変更あり
 */
#define ADD_BULLET_NC	(1<<8)
int Route::add(int stationId)
{
	removeAll(true, false);
	route_list_raw.push_back(RouteItem(0, stationId));
	TRACE(_T("add-begin %s(%d)\n"), Route::StationName(stationId).c_str(), stationId);
	return 1;
}

int Route::add(int line_id, int stationId2, int ctlflg)
{
	DbidOf dbid;
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
	bool replace_flg = false;	// 経路追加ではなく置換
	SPECIFICFLAG jct_flg_on = 0;   // 水平型検知(D-2) / BSRNOTYET_NA
	int type = 0;
	JCTSP_DATA jctspdt;
#ifdef _DEBUG
	int original_line_id = line_id;
	int first_station_id1;
#endif
	if (BIT_CHK(last_flag, BLF_END)) {
		return	0;		/* already terminated. */
	}

	last_flag &= ~((1 << BLF_TRACKMARKCTL) | (1 << BLF_JCTSP_ROUTE_CHANGE));

	num = route_list_raw.size();
	if (num <= 0) {
		ASSERT(FALSE);	// bug. must be call to add(station);
		return -3;
	}
	start_station_id = route_list_raw.front().stationId;
	stationId1 = route_list_raw.back().stationId;
#ifdef _DEBUG
first_station_id1 = stationId1;
#endif
	/* 発駅 */
	lflg1 = Route::AttrOfStationOnLineLine(line_id, stationId1);
	if (BIT_CHK(lflg1, BSRNOTYET_NA)) {
		return -2;		/* 不正経路(line_idにstationId1は存在しない) */
	}

	/* 着駅が発駅〜最初の分岐駅間にあるか? */
	/* (着駅未指定, 発駅=着駅は除く) */
	if ((num == 1) && (0 < endStationId) && (endStationId != start_station_id) && 
	(endStationId != stationId2) &&
	(0 != Route::InStation(endStationId, line_id, stationId1, stationId2))) {
		return -1;	/* <t> already passed error  */
	}

	ASSERT(BIT_CHK(lflg1, BSRJCTHORD) || route_list_raw.at(num - 1).stationId == stationId1);

	lflg2 = Route::AttrOfStationOnLineLine(line_id, stationId2);
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
	if ((route_list_raw.at(num - 1).lineId == line_id) && (2 <= num) &&
	(Route::DirLine(line_id, route_list_raw.at(num - 2).stationId, stationId1) !=
	 Route::DirLine(line_id, stationId1, stationId2))) {
		TRACE(_T("re-route error.\n"));
		TRACE(_T("add_abort\n"));
		return -1;		//  >>>>>>>>>>>>>>>>>>
	}

	// 分岐特例B(BSRJCTSP_B)水平型検知
	if (BIT_CHK(lflg2, BSRJCTSP_B) && chk_jctsb_b((type = getBsrjctSpType(line_id, stationId2)), num)) {
		TRACE("JCT: h_(B)detect\n");
		BIT_ON(jct_flg_on, BSRNOTYET_NA);	/* 不完全経路フラグ */
	} else {
		/* 新幹線在来線同一視区間の重複経路チェック(lastItemのflagがBSRJCTHORD=ONがD-2ケースである */
		if (!BIT_CHK(ctlflg, 8) && 
			(1 < num) && !BIT_CHK2(route_list_raw.at(num - 1).flag, BSRJCTHORD, BSRJCTSP_B) && 
			!Route::CheckTransferShinkansen(route_list_raw.at(num - 1).lineId, line_id,
										route_list_raw.at(num - 2).stationId, stationId1, stationId2)) {
			TRACE("JCT: F-3b\n");
			return -1;		// F-3b
		}
	}
	TRACE(_T("add %s(%d)-%s(%d), %s(%d)\n"), Route::LineName(line_id).c_str(), line_id, Route::StationName(stationId1).c_str(), stationId1, Route::StationName(stationId2).c_str(), stationId2);

	if (BIT_CHK(route_list_raw.at(num - 1).flag, BSRJCTSP_B)) {
		 /* 信越線上り(宮内・直江津方面) ? (フラグけちってるので
		  * t_jctspcl.type retrieveJunctionSpecific()で吉塚、小倉廻りと区別しなければならない) */
		if ((LDIR_DESC == Route::DirLine(line_id, route_list_raw.at(num - 1).stationId, stationId2)) &&
		    ((num < 2) || ((2 <= num) && 
		    (LDIR_ASC  == Route::DirLine(route_list_raw.at(num - 1).lineId,
		                                 route_list_raw.at(num - 2).stationId, 
		                                 route_list_raw.at(num - 1).stationId)))) &&
			(JCTSP_B_NAGAOKA == retrieveJunctionSpecific(route_list_raw.at(num - 1).lineId, 
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
				BIT_ON(last_flag, BLF_JCTSP_ROUTE_CHANGE);	/* route modified */
				if (rc != 1) {
					TRACE(_T("junction special (JSBS001) error.\n"));
					TRACE(_T("add_abort\n"));
					return rc;			// >>>>>>>>>>>>>>>>>>>>>
				}
				num++;
				stationId1 = jctspdt.jctSpStationId2; // 宮内
				// line_id : 信越線
				// stationId2 : 宮内〜長岡
			}
		} else if ((2 <= num) && (dbid.LineIdOf_SANYOSHINKANSEN == line_id)) { /* b#14021205 add */
			JctMaskOff(Route::Id2jctId(route_list_raw.at(num - 2).stationId));
			TRACE("b#14021205-1\n");
		}
	} else if ((1 <= num) && BIT_CHK(lflg2, BSRJCTSP_B) &&	/* b#14021205 add */
		(dbid.LineIdOf_SANYOSHINKANSEN == route_list_raw.at(num - 1).lineId)) {
		JctMaskOff(Route::Id2jctId(route_list_raw.at(num - 1).stationId));
		TRACE("b#14021205-2\n");
		BIT_ON(jct_flg_on, BSRNOTYET_NA);	/* 不完全経路フラグ */
	}

	// 水平型検知
	if (BIT_CHK(route_list_raw.at(num - 1).flag, BSRJCTHORD)) {
		TRACE("JCT: h_detect 2 (J, B, D)\n");
		if (Route::IsAbreastShinkansen(route_list_raw.at(num - 1).lineId, line_id, stationId1, stationId2)) {
			// 	line_idは新幹線
			//	route_list_raw.at(num - 1).lineIdは並行在来線
			// 
			ASSERT(IS_SHINKANSEN_LINE(line_id));
			if (0 != Route::InStation(route_list_raw.at(num - 2).stationId, line_id, stationId1, stationId2)) {
				TRACE("JCT: D-2\n");
				j = Route::NextShinkansenTransferTerm(line_id, stationId1, stationId2);
				if (j <= 0) {	// 隣駅がない場合
ASSERT(original_line_id = line_id);
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
					ASSERT(rc == 1);
					stationId1 = j;
				}
				BIT_ON(last_flag, BLF_JCTSP_ROUTE_CHANGE);	/* route modified */
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
ASSERT(original_line_id = line_id);
			type = retrieveJunctionSpecific(line_id, stationId2, &jctspdt); // update jctSpMainLineId(b), jctSpStation(c)
			ASSERT(0 < type);
			TRACE("JCT: detect step-horiz:%u\n", type);
			if (jctspdt.jctSpStationId2 != 0) {
				BIT_OFF(lflg1, BSRJCTSP);				// 別に要らないけど
				break;
			}
		}
ASSERT(original_line_id = line_id);
ASSERT(first_station_id1 = stationId1);
		// retrieve from a, d to b, c 
		type = retrieveJunctionSpecific(line_id, stationId1, &jctspdt); // update jctSpMainLineId(b), jctSpStation(c)
		ASSERT(0 < type);
		TRACE("JCT: detect step:%u\n", type);
		if (stationId2 != jctspdt.jctSpStationId) {
			if (route_list_raw.at(num - 1).lineId == jctspdt.jctSpMainLineId) {
				ASSERT(stationId1 == route_list_raw.at(num - 1).stationId);
				if (0 < Route::InStation(jctspdt.jctSpStationId, 
										 route_list_raw.at(num - 1).lineId, 
										 route_list_raw.at(num - 2).stationId, 
										 stationId1)) {
					TRACE("JCT: C-1\n");
					routePassOff(jctspdt.jctSpMainLineId, jctspdt.jctSpStationId, stationId1);	// C-1
				} else { // A-1
					TRACE("JCT: A-1\n");
				}
				if ((2 <= num) && (jctspdt.jctSpStationId == route_list_raw.at(num - 2).stationId)) {
					removeTail();
					TRACE(_T("JCT: A-C\n"));		// 3, 4, 8, 9, g,h
					--num;
				} else {
					route_list_raw.at(num - 1) = RouteItem(route_list_raw.at(num - 1).lineId, 
														   jctspdt.jctSpStationId);
					TRACE("JCT: %d\n", __LINE__);
				}
				if (jctspdt.jctSpStationId2 != 0) {		// 分岐特例路線2
					TRACE("JCT: step_(2)detect\n");
					rc = add(jctspdt.jctSpMainLineId2, jctspdt.jctSpStationId2, ADD_BULLET_NC);	//**************
					ASSERT(rc == 1);
					num++;
					if (rc != 1) {			// safety
						BIT_ON(last_flag, BLF_JCTSP_ROUTE_CHANGE);	/* route modified */
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
					stationId1 = jctspdt.jctSpStationId;
				}
			} else {
ASSERT(first_station_id1 = stationId1);
				if ((num < 2) || 
				!Route::IsAbreastShinkansen(jctspdt.jctSpMainLineId, 
											route_list_raw.at(num - 1).lineId, 
											stationId1, 
											route_list_raw.at(num - 2).stationId)
				|| (jctspdt.jctSpStationId == dbid.StationIdOf_NISHIKOKURA) // KC-2
				|| (jctspdt.jctSpStationId == dbid.StationIdOf_YOSHIZUKA) // KC-2
				|| (Route::InStation(jctspdt.jctSpStationId, 
									 route_list_raw.at(num - 1).lineId, 
									 route_list_raw.at(num - 2).stationId, 
									 stationId1) <= 0)) {
					// A-0, I, A-2
					TRACE("JCT: A-0, I, A-2\n");	//***************
					
					if ((jctspdt.jctSpStationId == dbid.StationIdOf_NISHIKOKURA) // KC-2
					   || (jctspdt.jctSpStationId == dbid.StationIdOf_YOSHIZUKA)) { // KC-2
						routePassOff(jctspdt.jctSpMainLineId, 
									 stationId1, jctspdt.jctSpStationId);
						TRACE("JCT: KC-2\n");
					}
					rc = add(jctspdt.jctSpMainLineId, 
							 /*route_list_raw.at(num - 1).stationId,*/ jctspdt.jctSpStationId, 
							 ADD_BULLET_NC);
					ASSERT(rc == 1);
					num++;
					if (rc != 1) {				// safety
						BIT_ON(last_flag, BLF_JCTSP_ROUTE_CHANGE);	/* route modified */
						TRACE(_T("A-0, I, A-2\n"));
						TRACE(_T("add_abort\n"));
						return -1;					//>>>>>>>>>>>>>>>>>>>>>>>>>>
					}
					if (jctspdt.jctSpStationId2 != 0) {		// 分岐特例路線2
						rc = add(jctspdt.jctSpMainLineId2, jctspdt.jctSpStationId2, ADD_BULLET_NC);	//**************
						num++;
						ASSERT(rc == 1);
						if (rc != 1) {			// safety
							BIT_ON(last_flag, BLF_JCTSP_ROUTE_CHANGE);	/* route modified */
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
ASSERT(first_station_id1 = stationId1);
					// C-2
					TRACE("JCT: C-2\n");
					ASSERT(IS_SHINKANSEN_LINE(route_list_raw.at(num - 1).lineId));
					routePassOff(jctspdt.jctSpMainLineId, 
								 jctspdt.jctSpStationId, stationId1);
					i = Route::NextShinkansenTransferTerm(route_list_raw.at(num - 1).lineId, stationId1, route_list_raw.at(num - 2).stationId);
					if (i <= 0) {	// 隣駅がない場合
						TRACE("JCT: C-2(none next station on bullet line)\n");
						// 新幹線の発駅には並行在来線(路線b)に所属しているか?
						if (0 == Route::InStationOnLine(jctspdt.jctSpMainLineId, 
														route_list_raw.at(num - 2).stationId)) {
							BIT_ON(last_flag, BLF_JCTSP_ROUTE_CHANGE);	/* route modified */
							TRACE(_T("next station is not found in shinkansen.\n"));
							TRACE(_T("add_abort\n"));
							return -1;			// >>>>>>>>>>>>>>>>>>>
						} else {
							removeTail();
							rc = add(jctspdt.jctSpMainLineId, 
									 jctspdt.jctSpStationId, ADD_BULLET_NC);	//**************
							ASSERT(rc == 1);
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
			BIT_ON(last_flag, BLF_JCTSP_ROUTE_CHANGE);	/* route modified */
		} else {
			// E, G		(stationId2 == jctspdt.jctSpStationId)
			TRACE("JCT: E, G\n");
			if (jctspdt.jctSpStationId2 != 0) {
				TRACE("JCT: KE0-4\n");
				BIT_OFF(lflg2, BSRJCTSP);
			}
			line_id = jctspdt.jctSpMainLineId;
ASSERT(first_station_id1 = stationId1);
			if ((2 <= num) && 
//			!BIT_CHK(Route::AttrOfStationOnLineLine(line_id, stationId2), BSRJCTSP_B) &&
			(0 < Route::InStation(stationId2, jctspdt.jctSpMainLineId, 
			route_list_raw.at(num - 2).stationId, stationId1))) {
				TRACE(_T("E-3:duplicate route error.\n"));
				TRACE(_T("add_abort\n"));
				return -1;	// Duplicate route error >>>>>>>>>>>>>>>>>
			}
			if (BIT_CHK(Route::AttrOfStationOnLineLine(line_id, stationId2), BSRJCTSP_B)) {
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
			BIT_ON(last_flag, BLF_JCTSP_ROUTE_CHANGE);	/* route modified */
		}
		break;
	}
	if (BIT_CHK(lflg2, BSRJCTSP)) {
		// 水平型
			// a(line_id), d(stationId2) -> b(jctSpMainLineId), c(jctSpStationId)
ASSERT(original_line_id = line_id);
ASSERT(first_station_id1 = stationId1);
		type = retrieveJunctionSpecific(line_id, stationId2, &jctspdt);
		ASSERT(0 < type);
		TRACE("JCT:%u\n", type);
		if (stationId1 == jctspdt.jctSpStationId) {
			// E10-, F, H
			TRACE("JCT: E10-, F, H/KI0-4\n");
			line_id = jctspdt.jctSpMainLineId;	// a -> b
			if (route_list_raw.at(num - 1).lineId == jctspdt.jctSpMainLineId) {
				if ((2 <= num) && 
				(0 < Route::InStation(stationId2, jctspdt.jctSpMainLineId, 
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
				if (rc != 1) {
					BIT_ON(last_flag, BLF_JCTSP_ROUTE_CHANGE);	/* route modified */
					TRACE(_T("junction special (KJ) error.\n"));
					TRACE(_T("add_abort\n"));
					return rc;			// >>>>>>>>>>>>>>>>>>>>>
				}
			} else {
				if (jctspdt.jctSpStationId2 != 0) {	// 分岐特例路線2
					TRACE(_T("JCT: KH0-4(J, B, D) add(日田彦山線, 城野c')\n"));
					rc = add(line_id, /*stationId1,*/ jctspdt.jctSpStationId2, ADD_BULLET_NC);	//**************
					num++;
					if (rc == 1) {
						TRACE(_T("JCT: add(日豊線b', 西小倉c)\n"));
						rc = add(jctspdt.jctSpMainLineId2, jctspdt.jctSpStationId, ADD_BULLET_NC);	//**************
						num++;
					}
					if (rc != 1) {
						BIT_ON(last_flag, BLF_JCTSP_ROUTE_CHANGE);	/* route modified */
						TRACE(_T("junction special horizen type convert error.\n"));
						TRACE(_T("add_abort\n"));
						return rc;			// >>>>>>>>>>>>>>>>>>>>>
					}
				} else {
					TRACE("JCT: J, B, D\n");
					rc = add(line_id, /*stationId1,*/ jctspdt.jctSpStationId, ADD_BULLET_NC);	//**************
					num++;
					if (rc != 1) {
						BIT_ON(last_flag, BLF_JCTSP_ROUTE_CHANGE);	/* route modified */
						TRACE(_T("junction special horizen type convert error.\n"));
						TRACE(_T("add_abort\n"));
						return rc;			// >>>>>>>>>>>>>>>>>>>>>
					}
					BIT_ON(jct_flg_on, BSRJCTHORD);	//b#14021202
				}
			}
			// b#14021202 BIT_ON(jct_flg_on, BSRJCTHORD);
			line_id = jctspdt.jctSpMainLineId;
			stationId1 = jctspdt.jctSpStationId;
		}
		BIT_ON(last_flag, BLF_JCTSP_ROUTE_CHANGE);	/* route modified */
	}
	
	// 長岡周りの段差型
	if ((2 <= num) && BIT_CHK(lflg1, BSRJCTSP_B)) {
		if (JCTSP_B_NAGAOKA == retrieveJunctionSpecific(line_id, 
		                                                route_list_raw.at(num - 1).stationId, &jctspdt)) {
		 	/* 信越線下り(直江津→長岡方面) && 新幹線|上越線上り(長岡-大宮方面)? */
			if ((LDIR_ASC == Route::DirLine(route_list_raw.at(num - 1).lineId, 
		                                    route_list_raw.at(num - 2).stationId, 
		                                    route_list_raw.at(num - 1).stationId)) &&
			    (LDIR_DESC == Route::DirLine(line_id, 
		                                    route_list_raw.at(num - 1).stationId, 
		                                    stationId2))) {
				/* 宮内発 */
				if (route_list_raw.at(num - 2).stationId == jctspdt.jctSpStationId2) {
					TRACE(_T("junction special 2(JSBS004) error.\n"));
					TRACE(_T("add_abort\n"));
					return -1;			// >>>>>>>>>>>>>>>>>>>>>
				}
				// 長岡Off
				JctMaskOff(Route::Id2jctId(route_list_raw.at(num - 1).stationId));

				// 長岡->宮内へ置換
				route_list_raw.at(num - 1) = RouteItem(route_list_raw.at(num - 1).lineId,
				                                       jctspdt.jctSpStationId2);
			             
                // 上越線 宮内→浦佐
				rc = add(jctspdt.jctSpMainLineId, jctspdt.jctSpStationId, ADD_BULLET_NC);		//****************
				BIT_ON(last_flag, BLF_JCTSP_ROUTE_CHANGE);	/* route modified */
				if (1 != rc) {
					TRACE(_T("junction special 2(JSBH001) error.\n"));
					TRACE(_T("add_abort\n"));
					return rc;			// >>>>>>>>>>>>>>>>>>>>>
				}
				stationId1 = jctspdt.jctSpStationId;
				num += 1;
        	}
	    }
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
		if (i != 0) {	/* 開始駅(i=0)は前路線着駅(or 発駅)なのでチェックしない*/
			if (IsJctMask(jct_on)) {
				// 既に通過済み
				break;	/* already passed error */
			}
		}
		JctMaskOn(jct_on);
		TRACE(_T("  add-mask on: %s(%d,%d)\n"), Route::JctName(junctions[i]).c_str(), Route::Jct2id(junctions[i]), junctions[i]);
	}

	if (jnum <= i) {	// 復乗なし
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
	} else {	// 復乗
		if ((jnum - 1) == i) { /* last */
			if (
			(!STATION_IS_JUNCTION(stationId2)) ||
			((2 <= num) && (start_station_id != stationId2) && 
			 (0 != Route::InStation(start_station_id, line_id, stationId1, stationId2))) ||
			(((0 < endStationId) && (endStationId != stationId2) && (2 <= num)) &&
			(0 != Route::InStation(endStationId, line_id, stationId1, stationId2))) 
			// ||
			// /*BIT_CHK(lflg1, BSRJCTSP) || */ BIT_CHK(lflg2, BSRJCTSP) ||	// E-12
			// BIT_CHK(route_list_raw.back().flag, BSRJCTHORD) ||				// E-14
			// (0 < junctionStationExistsInRoute(stationId2)) ||
		//	((2 <= num) && (0 < Route::InStation(stationId2, line_id, route_list_raw.at(num - 2).stationId, stationId1)))
			) {
				rc = -1;	/* <k> <e> <r> <x> <u> <m> */
			} else  {
				rc = 1;		/* <b> <j> <h> */
			}
		} else {
			rc = -1;
		}
	}
	
	if (rc < 0) {
		// 不正ルートなのでmaskを元に戻す
		for (j = 1; j < i; j++) {
			jct_on = junctions[j];
			JctMaskOff(jct_on);	// off
			TRACE(_T("  add_mask off: %s\n"), Route::JctName(jct_on).c_str());
		}
		TRACE(_T("add_abort\n"));
		BIT_OFF(last_flag, BLF_TRACKMARKCTL);
		// E-12, 6, b, c, d, e 
		return -1;	/* already passed error >>>>>>>>>>>>>>>>>>>> */
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
	lflg1 = Route::AttrOfStationOnLineLine(line_id, stationId1);
	lflg2 = Route::AttrOfStationOnLineLine(line_id, stationId2);

	lflg2 |= (lflg1 & 0xff000000);
	lflg2 &= (0xff00ffff & ~(1<<BSRJCTHORD));	// 水平型検知(D-2);
	lflg2 |= jct_flg_on;	// BSRNOTYET_NA:BSRJCTHORD
	route_list_raw.push_back(RouteItem(line_id, stationId2, lflg2));
	++num;

	if (rc == 0) {
		TRACE(_T("added continue.\n"));
		BIT_OFF(last_flag, BLF_TRACKMARKCTL);
	} else if (rc == 1) {
		BIT_ON(last_flag, BLF_TRACKMARKCTL);
	} else if (rc = 2) {
		BIT_OFF(last_flag, BLF_TRACKMARKCTL);	/* 次にremoveTailでlastItemの通過マスクをOffする(typeOでもPでもないので) */
	} else {
		ASSERT(FALSE);
		BIT_ON(last_flag, BLF_TRACKMARKCTL);
	}

	TRACE(_T("added last(%d).\n"), rc);

	if (rc != 0) {
		if (BIT_CHK(lflg2, BSRNOTYET_NA)) {
			TRACE(_T("？？？西小倉・吉塚.\n"), rc);
			return 1;	/* 西小倉、吉塚 */
		} else {
			return 0;
		}
	} else {
		return 1;	/* OK - Can you continue */
	}
}


//	経路の末尾を除去
//
//	@param [in] begin_off    最終ノードの始点もOffするか(デフォルト:Offしない)
//
void Route::removeTail(bool begin_off/* = false*/)
{
	int line_id;
	int begin_station_id;
	int to_station_id;
	int jct_num;
	int route_num;
	int i;
	vector<int> junctions;	// 分岐駅リスト

	route_num = route_list_raw.size();
	if (route_num < 2) {
		ASSERT(FALSE);
		BIT_OFF(last_flag, BLF_TRACKMARKCTL);
		return;
	}
	
	/* 発駅〜最初の乗換駅時 */
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
		jct_num -= 1;		/* 開始駅はOffしない(前路線の着駅なので) */
	}
	for (i = 0; i < jct_num; i++) {
		/* i=0:最近分岐駅でO型経路、P型経路の着駅の場合は除外 */
		if ((i != 0) || !BIT_CHK(last_flag, BLF_TRACKMARKCTL)) {
			JctMaskOff(junctions[i]);
			TRACE(_T("removed   : %s\n"), Route::JctName(junctions[i]).c_str());
		}
	}

	last_flag &= ((1 << BLF_TRACKMARKCTL) | (1 << BLF_END));
	route_list_raw.pop_back();
}

//public:
//	運賃表示
//	@param [in]  cooked : bit15=0/1  = 規則適用/非適用
//						: bit0=0/1 = 発・着が特定都区市内で発-着が100/200km以下のとき、着のみ都区市内有効(APPLIED_TERMINAL)
//						: bit1=0/1 = 発・着が特定都区市内で発-着が100/200km以下のとき、発のみ都区市内有効(APPLIED_START)
//							(本関数が抜けた後、開始駅のlineIdのビット7, 6を参照してUIにより決める)
//	@return 運賃、営業キロ情報 表示文字列
//
tstring Route::showFare(int cooked)
{
#define MAX_BUF	1024
	TCHAR cb[MAX_BUF];
	tstring sResult;
	tstring sWork;
	tstring sExt;
	int rule114[3];	// [0] = 運賃, [1] = 営業キロ, [2] = 計算キロ

	if (route_list_raw.size() <= 1) {
		return tstring(_T(""));
	}

	if (BIT_CHK(route_list_raw.back().flag, BSRNOTYET_NA)) {
		return tstring(_T("--------------"));	//この経路の片道乗車券は購入できません."));
	}
	
	if (RULE_APPLIED == (RULE_NO_APPLIED & cooked)) {
		/* 規則適用 */
		/* 86, 87, 69, 70条 114条適用かチェック */
		if (!checkOfRuleSpecificCoreLine(cooked, rule114)) {	// route_list_raw -> route_list_cooked
			rule114[0] = 0;
			sExt = _T("");
		} else {
			// rule 114 applied
			_sntprintf_s(cb, MAX_BUF, _T("規程114条適用運賃：\\%s 営業キロ：%s km / 計算キロ：%s km\r\n"), 
						 num_str_yen(rule114[0]).c_str(),
						 num_str_km(rule114[1]).c_str(),
						 num_str_km(rule114[2]).c_str());
			sExt = cb;
		}
		// 仮↑

		if (route_list_cooked.size() <= 1) {
ASSERT(FALSE);
			if (!fare_info.calc_fare(route_list_raw)) {
				return tstring(_T(""));
			}
		} else {
			/* 経路変更 */
			sResult = _T("");

			/* 発駅 */
			sWork = Route::CoreAreaNameByCityId(CSTART, 
						 						  route_list_cooked.front().lineId, 
						 						  route_list_cooked.front().flag);
			if (sWork == _T("")) {
				/* 単駅 */
				sWork = Route::StationNameEx(route_list_cooked.front().stationId);
			}
			sResult += sWork;
			sResult += _T(" -> ");

			/* 着駅 */
			sWork = Route::CoreAreaNameByCityId(CEND, 
						 						route_list_cooked.front().lineId, 
						 						route_list_cooked.back().flag);
			if (sWork == _T("")) {
				/* 単駅 */
				sWork = Route::StationNameEx(route_list_cooked.back().stationId);
			}
			sResult += sWork;
			sResult += _T("\r\n経由：");
			sResult += Route::show_route(true);
			
			if (!fare_info.calc_fare(route_list_cooked)) {
				return tstring(_T(""));
			}
		}
	} else {
		/* 規則非適用 */
		sResult = _T("");

		/* 単駅 */
		sWork = Route::StationNameEx(route_list_raw.front().stationId);
		sResult += sWork;
		sResult += _T(" -> ");
		sWork = Route::StationNameEx(route_list_raw.back().stationId);
		sResult += sWork;
		sResult += _T("\r\n経由：");
		sResult += Route::show_route(false);
		
		if (!fare_info.calc_fare(route_list_raw, false)) {
			return tstring(_T(""));
		}
	}

	ASSERT(100<=fare_info.getFareForJR());

	if (fare_info.isUrbanArea()) { 
		 /* 全経路が近郊区間内で新幹線未利用のとき最短距離で算出可能 */
		_sntprintf_s(cb, MAX_BUF,
		_T("近郊区間内ですので最短経路の運賃で利用可能です(途中下車不可、有効日数当日限り)\r\n"));
		sResult += cb;
		//ASSERT(fare_info.getTicketAvailDays() <= 2);
	} else {
		ASSERT(fare_info.getFareForIC() == 0);
	}

	_sntprintf_s(cb, MAX_BUF,
				_T("営業キロ： %s km    計算キロ： %s km\r\n"), 
				num_str_km(fare_info.getTotalSalesKm()).c_str(),
				num_str_km(fare_info.getJRCalcKm()).c_str());
	sResult += cb;

	/* 会社線(通過連絡運輸)あり */
	if (fare_info.getJRSalesKm() != fare_info.getTotalSalesKm()) {
		ASSERT(0 != fare_info.getCompanySalesKm());
		_sntprintf_s(cb, MAX_BUF,
				_T("(JR線営業キロ： %-6s km   会社線営業キロ： %s km)\r\n"), 
				num_str_km(fare_info.getJRSalesKm()).c_str(),
				num_str_km(fare_info.getCompanySalesKm()).c_str());
		sResult += cb;
	} else {
		ASSERT(0 == fare_info.getCompanySalesKm());
	}

	if (0 < fare_info.getSalesKmForHokkaido()) {
		_sntprintf_s(cb, MAX_BUF, _T("JR北海道営業キロ： %-6s km 計算キロ： %s km\r\n"),
						num_str_km(fare_info.getSalesKmForHokkaido()).c_str(),
						num_str_km(fare_info.getCalcKmForHokkaido()).c_str());
		sResult += cb;
	}
	if (0 < fare_info.getSalesKmForShikoku()) {
		if (0 < fare_info.getCalcKmForShikoku()) {
			_sntprintf_s(cb, MAX_BUF, _T("JR四国営業キロ： %-6s km 計算キロ： %s km\r\n"),
							num_str_km(fare_info.getSalesKmForShikoku()).c_str(),
							num_str_km(fare_info.getCalcKmForShikoku()).c_str());
		} else {
			_sntprintf_s(cb, MAX_BUF, _T("JR四国営業キロ： %-6s km \r\n"),
							num_str_km(fare_info.getSalesKmForShikoku()).c_str());
		}
		sResult += cb;
	}
	if (0 < fare_info.getSalesKmForKyusyu()) {
		if (0 < fare_info.getCalcKmForKyusyu()) {
			_sntprintf_s(cb, MAX_BUF, _T("JR九州営業キロ： %-6s km  計算キロ： %-6s km\r\n"),
							num_str_km(fare_info.getSalesKmForKyusyu()).c_str(),
							num_str_km(fare_info.getCalcKmForKyusyu()).c_str());
		} else {
			_sntprintf_s(cb, MAX_BUF, _T("JR九州営業キロ： %-6s km \r\n"),
							num_str_km(fare_info.getSalesKmForKyusyu()).c_str());
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

	bool return_discount_flg;
	int company_fare = fare_info.getFareForCompanyline();
	int normal_fare = fare_info.getFareForJR() + company_fare;
	int fareW = fare_info.roundTripFareWithComapnyLine(return_discount_flg);
	int fare_ic = fare_info.getFareForIC();

	if (0 < company_fare) {
		_sntprintf_s(cb, MAX_BUF,
					_T(" (うち会社線： \\%s)"), num_str_yen(company_fare).c_str());
		sWork = cb;
	} else {
		sWork = _T("");
	}

	if (fare_ic == 0) {
		_sntprintf_s(cb, MAX_BUF, _T("運賃： \\%-5s   %s     往復： \\%-5s"),
		                              num_str_yen(normal_fare).c_str(),
		                              sWork.c_str(),
		                              num_str_yen(fareW).c_str());
		if (return_discount_flg) {
			_tcscat_s(cb, NumOf(cb), _T("(割)"));
		}
		_tcscat_s(cb, NumOf(cb), _T("\r\n"));

	} else {
		ASSERT(!return_discount_flg);
		ASSERT(company_fare == 0);
		ASSERT(normal_fare  *  2 == fare_info.roundTripFareWithComapnyLine(return_discount_flg));
		_sntprintf_s(cb, MAX_BUF, _T("運賃(IC)： \\%s(\\%s)  %s    往復： \\%s(\\%s)\r\n"), 
		             num_str_yen(normal_fare).c_str(), num_str_yen(fare_ic).c_str(),
	                 sWork.c_str(),
		             num_str_yen(normal_fare * 2).c_str(), num_str_yen(fare_ic * 2).c_str());
	}

	sWork = _T("");
	for (int i = 0; true; i++) {
		tstring s;
		TCHAR sb[64];
		int fareStock = fare_info.getFareStockDistount(i, s);
		if (fareStock <= 0) {
			break;
		}
		if (0 < i) {
			sWork += _T("\r\n");
		}
		_sntprintf_s(sb, NumOf(sb), _T("%s \\%s"), s.c_str(), num_str_yen(fareStock + company_fare).c_str());
		sWork += sb;
	}

	sWork = cb + sWork;
	
	_sntprintf_s(cb, MAX_BUF,
				_T("\r\n有効日数：%4u日\r\n"),
						fare_info.getTicketAvailDays());
	return sExt + sResult + sWork + cb;
}

//public:
//	運賃計算オプションを得る
//	@return 0 : 無し(通常)(発・着が特定都区市内駅で特定都区市内間が100/200km以下ではない)
//			 (以下、発・着が特定都区市内駅で特定都区市内間が100/200kmを越える)
//			1 : 結果表示状態は{特定都区市内 -> 単駅} (「発駅を単駅に指定」と表示)
//			2 : 結果表示状態は{単駅 -> 特定都区市内} (「着駅を単駅に指定」と表示)
//
int Route::fareCalcOption()
{
	if ((route_list_raw.size() <= 1) || (route_list_cooked.size() <= 1)) {
		return 0;
	}
	if ((B1LID_MARK | (1 << B1LID_BEGIN_CITY_OFF)) == (route_list_cooked.at(0).lineId & 
	    (B1LID_MARK | (1 << B1LID_BEGIN_CITY_OFF)))) {
		return 1;
	}
	if ((B1LID_MARK | (1 << B1LID_FIN_CITY_OFF)) == (route_list_cooked.at(0).lineId & 
	    (B1LID_MARK | (1 << B1LID_FIN_CITY_OFF)))) {
		return 2;
	}
	return 0;
}

//public:
//	経路を逆転
//
//	@retval 1   sucess
//	@retval 0   sucess(empty)
//	@retval -1	failure(6の字を逆転すると9になり経路重複となるため)
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
				rc = add(rev_pos->lineId, /*station_id,*/ rev_pos->stationId, 1<<8);
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
			if (rc <= 0) {
				break;
			}
			lineId = 0;
			stationId1 = stationId2;
		}
	}
	delete [] rstr;

	return rc;
}

/*	ルート表示
 *	@param [in]  cooked : true/false  = 規則適用/非適用
 *	@return ルート文字列
 *
 *	@remark showFare()の呼び出し後にのみ有効
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
		routeList = &route_list_cooked;	/* 規則適用 */
	} else {
		routeList = &route_list_raw;	/* 規則非適用 */
	}
	
	if (routeList->size() == 0) {	/* 経路なし(AutoRoute) */
		return _T("");
	}
	
	vector<RouteItem>::const_iterator pos = routeList->cbegin();

	result_str = _T("");

	for (pos++; pos != routeList->cend() ; pos++) {

		lineName = LineName(pos->lineId);

		if ((pos + 1) != routeList->cend()) {
			/* 中間駅 */
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
			/* 着駅 */
			if (!IS_FLG_HIDE_LINE(pos->flag)) {
				result_str += _T("<");
				result_str += lineName;
				result_str += _T(">");
			}
			//result_str += stationName;	// 着駅
			result_str += _T("\r\n");
		}
		//result_str += buf;
	}
	return result_str;
}

/*	ルート表示
 *	@return ルート文字列
 *
 *	@remark showFare()の呼び出し後にのみ有効
 */
tstring Route::route_script()
{
	const vector<RouteItem>* routeList;
	tstring result_str;
	
	routeList = &route_list_raw;	/* 規則非適用 */

	if (routeList->size() == 0) {	/* 経路なし(AutoRoute) */
		return _T("");
	}
	
	vector<RouteItem>::const_iterator pos = routeList->cbegin();

	result_str = Route::StationNameEx(pos->stationId);

	for (pos++; pos != routeList->cend() ; pos++) {
		result_str += _T(",");
		result_str += LineName(pos->lineId);
		result_str += _T(",");
		result_str += Route::StationNameEx(pos->stationId);
	}
	result_str += _T("\n\n");
	return result_str;
}

//private:
//	@brief	分岐マークOff
//
//	@param [in]  line_id          路線
//	@param [in]  to_station_id    開始駅(含まない)
//	@param [in]  begin_station_id 終了駅(含む)
//
void Route::routePassOff(int line_id, int to_station_id, int begin_station_id)
{
	int jct_num;
	int i;
	vector<int> junctions;	// 分岐駅リスト

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

//	経路設定中 経路重複発生時
//	経路設定キャンセル
//
//	@param [in]  bWithStart  開始駅もクリアするか(デフォルトクリア)
//	@param [in]  bWithEnd    終了駅もクリアするか(デフォルトクリア)
//	@note
//	x stop_jctId > 0 : 指定分岐駅前までクリア(指定分岐駅含まず)
//	x stop_jctId = 0 : (default)指定分岐駅指定なし
//	x stop_jctId < 0 : 開始地点も削除
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

	last_flag = LASTFLG_OFF;

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

//	分岐駅が経路内に含まれているか？
//
//	@param [in] stationId   駅ident
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

//	経路内を着駅で終了する(以降の経路はキャンセル)
//
//	@param [in] stationId   駅ident
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
			/* stationIdはroute_list_raw[i].lineId内のstationIdFromから
			              route_list_raw[i].statonIdの間にあるか? */
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
//	路線が上り／下りかを得る
//
//	@param [in] line_id     路線
//	@param [in] station_id1 発
//	@param [in] k 至
//
//	@retval 1 下り(Route::LDIR_ASC)
//	@retval 2 上り(Route::LDIR_DESC)
//
//  @node 同一駅の場合下り(0)を返す
//
//
int Route::DirLine(int line_id, int station_id1, int station_id2)
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
//	駅ID→分岐ID
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
//	分岐ID→駅名
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
//	駅ID→駅名
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
//	駅ID→駅名(同名駅区別)
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
//	路線ID→路線名
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
//	駅の属性を得る
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
//	路線駅の属性を得る
//
SPECIFICFLAG Route::AttrOfStationOnLineLine(int line_id, int station_id)
{
	DBO ctx = DBS::getInstance()->compileSql(
	//  "select lflg, sflg from t_lines where line_id=?1 and station_id=?2", true);
		"select sflg, lflg from t_station t left join t_lines on t.rowid=station_id where line_id=?1 and station_id=?2", true);
	if (ctx.isvalid()) {
		int s;
		int l;
		ctx.setParam(1, line_id);
		ctx.setParam(2, station_id);

		if (ctx.moveNext()) {
			s = 0x0000ffff & ctx.getInt(0);
			l = 0xffff0000 & ctx.getInt(1);
			return s | l;
		}
	}
	return (1<<30);
}

//static 
//	駅は路線内にあるか否か？
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


// 駅(station_id)の所属する路線IDを得る. 
// 駅は非分岐駅で, 路線は1つしかないと解っていることを前提とする.
//
// line_id = f(station_id)
//
int Route::LineIdFromStationId(int station_id)
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


// 駅名より駅IDを返す(私鉄含)
//
// station_id = f("駅名")
//
int Route::GetStationId(LPCTSTR station)
{
	const char tsql[] = "select rowid from t_station where (sflg&(1<<18))=0 and name=?1 and samename=?2";

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

// 線名より線IDを返す
//
// station_id = f("線名")
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


//	@brief 分岐特例の分岐路線a+乗換駅dから本線bと分岐駅cを得る
//
//	@param [in]  jctLineId         a 分岐路線
//	@param [in]  transferStationId d 乗換駅
//	@param [out] jctspdt   b 本線, c 分岐駅
//
//	@return type 0: nomal, 1-3:type B
//
int Route::retrieveJunctionSpecific(int jctLineId, int transferStationId, JCTSP_DATA* jctspdt)
{
	const char tsql[] =
	//"select calc_km>>16, calc_km&65535, (lflg>>16)&32767, lflg&32767 from t_lines where (lflg&(1<<31))!=0 and line_id=?1 and station_id=?2";
//	"select type,jctsp_line_id1, jctsp_station_id1, jctsp_line_id2, jctsp_station_id2"
//	" from t_jctspcl where id=("
//	"	select calc_km from t_lines where (lflg&(1<<31))!=0 and line_id=?1 and station_id=?2)";
	"select type,jctsp_line_id1, jctsp_station_id1, jctsp_line_id2, jctsp_station_id2" \
	" from t_jctspcl where id=(" \
	"	select lflg&255 from t_lines where (lflg&((1<<31)|(1<<29)))!=0 and line_id=?1 and station_id=?2)";
	int type = 0;

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
//	@return type 0: nomal, 1-3:type B
//
int Route::getBsrjctSpType(int line_id, int station_id)
{
	const char tsql[] =
	"select type from t_jctspcl where id=(select lflg&255 from t_lines where line_id=?1 and station_id=?2)";
	int type = -1;
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
//
//	@param [in] line_id     路線
//	@param [in] station_id1 発
//	@param [in] station_id2 至
//  
//	@retuen [0]:営業キロ, [1]:計算キロ
//
vector<int> Route::GetDistance(int line_id, int station_id1, int station_id2)
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
//	運賃計算キロを返す(最短距離算出用)
//
//	@param [in] line_id     路線
//	@param [in] station_id1 発
//	@param [in] station_id2 至
//  
//	@retuen 営業キロ or 計算キロ
//
int Route::Get_node_distance(int line_id, int station_id1, int station_id2)
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
//	特定都区市内駅名を返す
//
//	@param [in] startEndFlg     CSTART(発駅) / CEND(着駅)
//	@param [in] flg             RouteItem配列の先頭のlineIdに設定されたフラグ値(発駅、着駅の有無、山手線内か)
//	@param [in] flags           RouteItem配列の現位置(発駅または着駅)のflagに設定されたフラグ値(都区市内ID)
//
//	@return 特定都区市内
//
tstring Route::CoreAreaNameByCityId(int startEndFlg, int flg, SPECIFICFLAG flags)
{
	const static TCHAR *coreAreaName[] = {  _T("東京都区内[区]"),	// 1
											_T("横浜市内[浜]"),		// 2
											_T("名古屋市内[名]"),	// 3
											_T("京都市内[京]"),		// 4
											_T("大阪市内[阪]"),		// 5
											_T("神戸市内[神]"),		// 6
											_T("広島市内[広]"),		// 7
											_T("北九州市内[九]"),	// 8
											_T("福岡市内[福]"),		// 9
											_T("仙台市内[仙]"),		// 10
											_T("札幌市内[札]"),		// 11
											/*****************/
											_T("山手線内[山]"), 	// -2
											_T("大阪・新大阪"), 	// -1
										};
	int cityno;

	if (0 != (flg & B1LID_MARK)) {
		if (((startEndFlg == CSTART) && BIT_CHK(flg, B1LID_BEGIN_YAMATE)) ||
		    ((startEndFlg == CEND)   && BIT_CHK(flg, B1LID_FIN_YAMATE))) {
			return coreAreaName[NumOf(coreAreaName) - 2];	/* 山手線内*/
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
			return coreAreaName[NumOf(coreAreaName) - 1];	/* 大阪・新大阪 */
		}
		// else
		// thru
	}
	return _T("");
}

//static 
//	70条適用
//	route_listを適用してout_route_listに設定する
//	showFare() => checkOfRuleSpecificCoreLine() => 
//
//	out_route_listは表示用ではなく計算用(路線が70条専用がある)
//
//	@param [in]   in_route_list  経路
//	@param [out]  out_route_list 経路
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
				break;		/* 70条適用エリア内発は非適用(86、87条) */
			}
		} else if (stage == 1) {
			if ((route_item->flag & (1 << BCRULE70)) != 0) {
				stage = 2;					/* 2: on */ /* 外から進入した */
								/* 路線より最外側の大環状線内(70条適用)駅を得る */
				stationId_o70 = Route::RetrieveOut70Station(route_item->lineId);
				ASSERT(0 < stationId_o70);
				ri.stationId = stationId_o70;
				flag = route_item->flag;
			} else {	// 外のまま
				/* do nothing */
			}
		} else if (stage == 2) {
			if ((route_item->flag & (1 << BCRULE70)) == 0) {
				int stationId_tmp;
				stage = 3;					/* 3: off: !70 -> 70 -> !70 (applied) */
								/* 進入して脱出した */
								/* 路線より最外側の大環状線内(70条適用)駅を得る */
				stationId_tmp = Route::RetrieveOut70Station(route_item->lineId);
				ASSERT(0 < stationId_tmp);
				if (stationId_tmp != stationId_o70) {
					out_route_list->push_back(RouteItem(ID_L_RULE70, stationId_tmp, flag));
				}
			} else {	// 中のまま
				skip = true;
				flag = route_item->flag;
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


//static
//	69条適用駅を返す
//	端駅1〜端駅2のペア×Nをかえす.
//	results[]配列は必ずn*2を返す
//
//	showFare() => checkOfRuleSpecificCoreLine() => ReRouteRule69j()=>
//
//	coninue_flag: false 続き無し
//                true  続き
//	return: false 続き無し
//          true  続き
bool Route::Query_a69list(int line_id, int station_id1, int station_id2, vector<PAIRIDENT>* results, bool continue_flag)
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
				if (((cur_flag & (1 << 15)) != 0) && ((it + 1) == pre_list.cend()) && 
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

				TRACE(_T("c69             station_id1=%s(%d), station_id2=%s(%d)\n"), Route::StationName(prev_stid).c_str(), prev_stid, Route::StationName(cur_stid).c_str(), cur_stid);

				if (((cur_flag & (1 << 15)) != 0) && 
				    ((it + 1) == pre_list.cend()) && 
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
			if (s1 != 0) { /* 2回目以降 ? */
				if (!cur.is_equal(*route_item)) {
					it = results->cbegin();
					while (it != results->cend()) {
						lineId = it->at(0);
						stationId1 = it->at(1);
						stationId2 = it->at(2);
						in = Route::InStation(s2, lineId, stationId1, stationId2);
						in1 = Route::InStation(s1, lineId, stationId1, stationId2);
						/* 2駅とも置換路線上にあれば置換しない
						 * 1駅のみ置換路線上にある場合、その駅が端駅でなければ置換しない
						 */
						if (results->size() == 1) {
							if (((in != 0) && (in1 != 0)) ||
						        ((in  != 0) && ((s2 != stationId1) && (s2 != stationId2))) || 
						        ((in1 != 0) && ((s1 != stationId1) && (s1 != stationId2)))) {
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
//	showFare() => checkOfRuleSpecificCoreLine() => 
//
//	@param [in] in_route_list  変換前のチェック経路
//	@param [out] out_route_list 変換後のチェック経路
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
				// 置換処理
				Route::Query_rule69t(in_route_list, *route_item, IDENT2(dbrecord[i]), &trule69list);
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
					} else if (2 == a69list.size()) {
						/* 置換処理3 */
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
#else	/* こっちはNG(返されるiteratorが無効で飛ぶ) */
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
								int j = a69list.size() - 2;
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
								int j = a69list.size() - 1;
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
					if (IDENT1(a69list.at(0)) == trule69list.at(0).at(1)) {
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
//	重複ルート除去
//
//	       勝浦
//  外房線 蘇我 <- remove
//  外房線 千葉
//	総武線 東京
//
int Route::RemoveDupRoute(vector<RouteItem> *routeList)
{
	int affects = 0;
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
//	路線の駅間に都区市内駅はあるか？
//	     東海道新幹線 京都 米原間に草津駅は存在するとして返します.
//
//	@param [in] cityno     検索する都区市内No
//	@param [in] lineId     路線
//	@param [in] stationId1 開始駅
//	@param [in] stationId2 終了駅
//	@return 0: not found / not 0: ocunt of found.
//
int Route::InCityStation(int cityno, int lineId, int stationId1, int stationId2)
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
//	86条適用されるかのチェック
//	showFare() => checkOfRuleSpecificCoreLine() =>
//
//	@param [in]  in_route_list 経路
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
//	o---o なし(乗車駅または分岐駅〜分岐駅または降車駅が都区市内だが間に非都区市内が含まれる例はなし。
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
	// 発駅が尼崎の場合大阪市内発ではない　基153-2
	if ((city_no_s == CITYNO_OOSAKA) && (dbid.StationIdOf_AMAGASAKI == fite->stationId)) {
		city_no_s = 0;
	}
	
	rite = in_route_list.crbegin();
	if (rite == in_route_list.crend()) {
		ASSERT(FALSE);
		return 0;	/* fatal error */
	}
	city_no_e = MASK_CITYNO(rite->flag);
	// 着駅が尼崎の場合大阪市内着ではない　基153-2
	if ((city_no_e == CITYNO_OOSAKA) && (dbid.StationIdOf_AMAGASAKI == rite->stationId)) {
		city_no_e = 0;
	}

	*cityId_pair = MAKEPAIR(city_no_s, city_no_e);

	if (city_no_s != 0) {
		// 発駅=特定都区市内
		c = 0;
		stationId = fite->stationId;	// 発 
		for (fite++; fite != in_route_list.cend(); fite++) {
			int cno = MASK_CITYNO(fite->flag);
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
			r = 1;		/* 発駅特定都区市内 */
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
//	showFare() => checkOfRuleSpecificCoreLine() => 
//
//	@param [in] in_route_list  route
//	@return 0  : 発着ではない
//			1  : 発駅は山手線内
//			2  : 着駅は山手線内
//			3  : (返すことはない)
//		 0x83  : 発着駅山手線内
//		    4  : 全駅駅山手線内
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
	if ((rite != in_route_list.crend()) && (((rite->flag & ((1 << 10) | (1 << 5))) == ((1 << 10)|(1 << 5))))) {
		// 着駅=山手線内
		c = 0;
		for (rite++; rite != in_route_list.crend(); rite++) {
			if (c == 0) {
				if (((rite->flag & ((1 << 10) | (1 << 5))) != ((1 << 10)|(1 << 5)))) {
					c = 1;			// 離れた
				} else {
					//
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
//	showFare() => checkOfRuleSpecificCoreLine() => ReRouteRule86j87j() =>
//
//  @param [in] lineId : 進入／脱出路線
//	@param [in] cityId : 都区市内Id
//
//  @retval vector<Station> : 返却配列(lineId, stationId)
//
//	例：lineId:常磐線, cityId:[区] -> 東北線、日暮里
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
//	86条中心駅を返す
//	showFare() => checkOfRuleSpecificCoreLine() => ReRouteRule86j87j() =>
//	
//  @param [in]  cityId : 都区市内Id
//
//  @return 中心駅
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
//	Route 営業キロを計算
//	showFare() => checkOfRuleSpecificCoreLine() =>
//	CheckOfRule114j() =>
//
//	@param [in]  route     計算ルート
//	@retuen 営業キロ[0] ／ 計算キロ[1]
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
//	86, 87条適用変換
//	showFare() => checkOfRuleSpecificCoreLine() =>
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
void  Route::ReRouteRule86j87j(PAIRIDENT cityId, int mode, const Station& exit, const Station& enter, vector<RouteItem>* out_route_list)
{
	int coreStationId;
	bool skip;
	int lineId;
	vector<RouteItem>::const_iterator itr;
	vector<RouteItem> work_route_list;
	vector<Station> firstTransferStation;
	
	if ((mode & 1) != 0) {
		/* 発駅-脱出: exit 有効*/		/* ex) [[東北線、日暮里]] = (常磐線, [区]) */
		firstTransferStation = Route::SpecificCoreAreaFirstTransferStationBy(exit.lineId, IDENT1(cityId));
		if (firstTransferStation.size() <= 0) {
			ASSERT(FALSE);				/* テーブル未定義 */
			work_route_list.assign(out_route_list->cbegin(), out_route_list->cend());
		} else {
			vector<Station>::const_reverse_iterator sta_ite;
			sta_ite = firstTransferStation.crbegin();
			if (exit.lineId == sta_ite->lineId) {
				work_route_list.push_back(RouteItem(0, sta_ite->stationId));	/* 発駅:都区市内代表駅 */
				// ASSERT(sta_ite->stationId == Route::Retrieve_SpecificCoreStation(IDENT(cityId))); (新横浜とか新神戸があるので)
			} else {
				coreStationId = Route::Retrieve_SpecificCoreStation(IDENT1(cityId));
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
				// ASSERT(sta_ite->stationId == Route::Retrieve_SpecificCoreStation(IDENT2(cityId))); 新横浜とかあるので
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
//	86、87条、69条判定＆経路算出
//	showFare() =>
//	route_list_raw => route_list_cooked
//
//	後半でB1LID_xxx(route[0].lineId)を設定します
//
//	@param [in]  dis_cityflag     bit0:1 = 発駅:着駅 無効(0)/有効(1)
//	@param [out] rule114	 [0] = 運賃, [1] = 営業キロ, [2] = 計算キロ
//	@return false : rule 114 no applied. true: rule 114 applied(available for rule114[] )
//	@remark ルール未適用時はroute_list_cooked = route_list_rawである
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

	// 69を適用したものをroute_list_tmp2へ
	n = Route::ReRouteRule69j(route_list_raw, &route_list_tmp);	/* 69条適用(route_list_raw->route_list_tmp) */
	TRACE("Rule 69 applied %dtimes.\n", n);

	// route_list_tmp2 = route_list_tmp
	// 70を適用したものをroute_list_tmp2へ
	n = Route::ReRouteRule70j(route_list_tmp, &route_list_tmp2);
	TRACE(0 == n ? "Rule70 applied.\n" : "Rule70 not applied.\n");

	// 88を適用したものをroute_list_tmpへ
	aply88 = Route::CheckOfRule88j(&route_list_tmp2);
	if (0 != aply88) {
		if ((aply88 & 1) != 0) {
			TRACE("Apply to rule88 for start.\n");
			route_list_tmp2.at(0).lineId = short(B1LID_MARK | MASK(B1LID_BEGIN_OOSAKA));
		} else if ((aply88 & 2) != 0) {
			TRACE("Apply to rule88 for arrive.\n");
			route_list_tmp2.at(0).lineId = short(B1LID_MARK | MASK(B1LID_FIN_OOSAKA));
		}
	}

	/* 特定都区市内発着可否判定 */
	chk = Route::CheckOfRule86(route_list_tmp2, &exit, &enter, &cityId);
	TRACE("RuleSpecific:chk 0x%0x, %d -> %d\n", chk, IDENT1(cityId), IDENT2(cityId));
	
	/* 86, 87適用可能性あり？ */
	if ((chk == 4) || (chk == 0)) {	/* 全駅特定都区市内駅 or 発着とも特定都区市内駅でない場合 */
		/* 未変換 */
		TRACE("no applied for rule86/87\n");
		route_list_cooked.assign(route_list_tmp2.cbegin(), route_list_tmp2.cend());
		return false;			// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	}

	/* (発駅=都区市内 or 着駅=都区市内) 
	 */
	/* route_list_tmp = route_list_tmp2 */
	route_list_tmp.assign(route_list_tmp2.cbegin(), route_list_tmp2.cend());

	/* 変換 -> route_list_tmp:86適用(仮) 
	   88変換したものは対象外(=山陽新幹線 新大阪着時、非表示フラグが消えてしまうのを避ける効果あり) */
	Route::ReRouteRule86j87j(cityId, chk & ~aply88, exit, enter, &route_list_tmp);

	// 88を適用
	aply88 = Route::CheckOfRule88j(&route_list_tmp);
	if (0 != aply88) {
		if ((aply88 & 1) != 0) {
			TRACE("Apply to rule88(2) for start.\n");
			route_list_tmp.at(0).lineId = short(B1LID_MARK | MASK(B1LID_BEGIN_OOSAKA));
		} else if ((aply88 & 2) != 0) {
			TRACE("Apply to rule88(2) for arrive.\n");
			route_list_tmp.at(0).lineId = short(B1LID_MARK | MASK(B1LID_FIN_OOSAKA));
		}
	}

	// 69を適用したものをroute_list_tmp3へ
	n = Route::ReRouteRule69j(route_list_tmp, &route_list_tmp3);	/* 69条適用(route_list_tmp->route_list_tmp3) */
	TRACE("Rule 69(2) applied %dtimes.\n", n);

	/* route_list_tmp	70-88-69-86適用
	 * route_list_tmp2	70-88-69適用
	 * route_list_tmp3	70-88-69-86-69適用
	 */
	/* compute of sales_km by route_list_cooked */
	sales_km = Route::Get_route_distance(route_list_tmp3).at(0);
	
	if (2000 < sales_km) {
		/* <<<都区市内適用>>> */
		/* 201km <= sales_km */
		/* enable */
		route_list_tmp3.at(0).lineId = short(B1LID_MARK | (chk & 0x03));	// B1LID_BEGIN_CITY, B1LID_FIN_CITY
		TRACE("applied for rule86(%d)\n", chk & 0x03);

		// route_list_cooked = route_list_tmp3
		route_list_cooked.assign(route_list_tmp3.cbegin(), route_list_tmp3.cend());

		return false;			// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	}
	
	/* 101km - 200km : 山手線発着 or 200km以下の都区市内間(名古屋-大阪など)チェック */
	rtky = Route::CheckOfRule87(route_list_tmp2);
	if ((3 & rtky) != 0) {
		/* apply to 87 */
		if (1000 < sales_km) {
			/* 山手線内発着 enable */
			route_list_tmp3.at(0).lineId = short(B1LID_MARK | ((rtky & 0x03) << 2));	// B1LID_BEGIN_YAMATE, B1LID_FIN_YAMATE
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

	/* 発着とも都区市内? */
	if ((0x03 & (rtky | chk)) == 3) { /* 名古屋市内-大阪市内など([名]-[阪]、[九]-[福]、[区]-[浜]) */
							/*  [区]-[区], [名]-[名], [山]-[区], ... */
		for (sk2 = 2000; true; sk2 = 1000) {
			flg = 0;

			/* route_list_tmp = route_list_tmp2 */
			route_list_tmp.assign(route_list_tmp2.cbegin(), route_list_tmp2.cend());
			/* 発駅のみ特定都区市内着経路に変換 */
			Route::ReRouteRule86j87j(cityId, 1, exit, enter, &route_list_tmp);

			// 69を適用したものをroute_list_tmp3へ
			n = Route::ReRouteRule69j(route_list_tmp, &route_list_tmp3);	/* 69条適用(route_list_tmp->route_list_tmp3) */
			TRACE("Rule 69(3) applied %dtimes.\n", n);

			/* 発駅のみ都区市内にしても201/101km以上か？ */
			skm = Route::Get_route_distance(route_list_tmp3).at(0);
			if (sk2 < skm) {
				// 発 都区市内有効
				flg = 0x01;
			}

			/* route_list_tmp = route_list_tmp2 */
			route_list_tmp.assign(route_list_tmp2.cbegin(), route_list_tmp2.cend());
			/* 着駅のみ特定都区市内着経路に変換仮適用 */
			Route::ReRouteRule86j87j(cityId, 2, exit, enter, &route_list_tmp);

			// 69を適用したものをroute_list_tmp3へ
			n = Route::ReRouteRule69j(route_list_tmp, &route_list_tmp3);	/* 69条適用(route_list_tmp->route_list_tmp3) */
			TRACE("Rule 69(4) applied %dtimes.\n", n);

			/* 着駅のみ都区市内にしても201/101km以上か？ */
			skm = Route::Get_route_distance(route_list_tmp3).at(0);
			if (sk2 < skm) {
				// 着 都区市内有効
				flg |= 0x02;
			}
			if (flg == 0x03) {	/* 発・着とも200km越えだが、都区市内間は200km以下 */
				if (0 != (dis_cityflag & APPLIED_START)) {
					/* 発のみ都区市内適用 */
					/* route_list_tmp = route_list_tmp2 */
					route_list_tmp.assign(route_list_tmp2.cbegin(), route_list_tmp2.cend());
					/* 発駅のみ特定都区市内着経路に変換 */
					Route::ReRouteRule86j87j(cityId, 1, exit, enter, &route_list_tmp);

					// 69を適用したものをroute_list_tmp3へ
					n = Route::ReRouteRule69j(route_list_tmp, &route_list_tmp3);	/* 69条適用(route_list_tmp->route_list_tmp3) */
					TRACE("Rule 69(5) applied %dtimes.\n", n);

					/* 発駅・着駅特定都区市内だが発駅のみ都区市内適用 */
					if (sk == 900) {
						TRACE("applied for rule87(start)\n");
						route_list_tmp3.at(0).lineId = short(B1LID_MARK | MASK(B1LID_BEGIN_YAMATE) | MASK(B1LID_BEGIN_CITY_OFF));
					} else {
						TRACE("applied for rule86(start)\n");
						route_list_tmp3.at(0).lineId = short(B1LID_MARK | MASK(B1LID_BEGIN_CITY) | MASK(B1LID_BEGIN_CITY_OFF));
					}
					// route_list_cooked = route_list_tmp3
					route_list_cooked.assign(route_list_tmp3.cbegin(), route_list_tmp3.cend());
					return false;			// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
				} else {
					/* 着のみ都区市内適用 */
					/* 発駅・着駅特定都区市内だが着駅のみ都区市内適用 */
					if (sk == 900) {
						TRACE("applied for rule87(end)\n");
						route_list_tmp3.at(0).lineId = short(B1LID_MARK | MASK(B1LID_FIN_YAMATE) | MASK(B1LID_FIN_CITY_OFF));
					} else {
						TRACE("applied for rule86(end)\n");
						route_list_tmp3.at(0).lineId = short(B1LID_MARK | MASK(B1LID_FIN_CITY) | MASK(B1LID_FIN_CITY_OFF));
					}
					// route_list_cooked = route_list_tmp3
					route_list_cooked.assign(route_list_tmp3.cbegin(), route_list_tmp3.cend());
					return false;			// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
				}
			} else if (flg == 0x01) {
				/* route_list_tmp = route_list_tmp2 */
				route_list_tmp.assign(route_list_tmp2.cbegin(), route_list_tmp2.cend());
				/* 発駅のみ特定都区市内着経路に変換 */
				Route::ReRouteRule86j87j(cityId, 1, exit, enter, &route_list_tmp);

				// 69を適用したものをroute_list_tmp3へ
				n = Route::ReRouteRule69j(route_list_tmp, &route_list_tmp3);	/* 69条適用(route_list_tmp->route_list_tmp3) */
				TRACE("Rule 69(6) applied %dtimes.\n", n);

				/* 発駅・着駅特定都区市内だが発駅のみ都区市内適用 */
				if (sk == 900) {
					TRACE("applied for rule87(start)\n");
					route_list_tmp3.at(0).lineId = short(B1LID_MARK | MASK(B1LID_BEGIN_YAMATE));
				} else {
					TRACE("applied for rule86(start)\n");
					route_list_tmp3.at(0).lineId = short(B1LID_MARK | MASK(B1LID_BEGIN_CITY));
				}
				// route_list_cooked = route_list_tmp3
				route_list_cooked.assign(route_list_tmp3.cbegin(), route_list_tmp3.cend());
				return false;			// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
			} else if (flg == 0x02) {
				/* 発駅・着駅特定都区市内だが着駅のみ都区市内適用 */
				if (sk == 900) {
					TRACE("applied for rule87(end)\n");
					route_list_tmp3.at(0).lineId = short(B1LID_MARK | MASK(B1LID_FIN_YAMATE));
				} else {
					TRACE("applied for rule86(end)\n");
					route_list_tmp3.at(0).lineId = short(B1LID_MARK | MASK(B1LID_FIN_CITY));
				}
				// route_list_cooked = route_list_tmp3
				route_list_cooked.assign(route_list_tmp3.cbegin(), route_list_tmp3.cend());
				return false;			// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
			}
			/* flg == 0 */
			if ((sk != 900) || (sk2 == 1000)) {
				/* 87無効 or 営業キロ200km越え判定は済 */
				break;
			}
		}
		/* passthru */
	}

	/* route_list_tmp	x
	 * route_list_tmp2	70-88-69適用
	 * route_list_tmp3	x
	 * route_list_tmp4	70-88-69-86-69適用
	 * route_list_cooked 空
	 */

	/* 未変換 */
	TRACE("no applied for rule86/87(sales_km=%d)\n", sales_km);

	if (sk <= sales_km) {
			/* 114条適用かチェック */
		if ((0x03 & chk) == 3) {
			is114 =					/* 86,87適用前,   86,87適用後 */
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
	/* 86-87非適用 */
	// route_list_cooked = route_list_tmp2
	route_list_cooked.assign(route_list_tmp2.cbegin(), route_list_tmp2.cend());
	return is114;
}

//static:
//	規則43条の2
//	@param [out] fare_inf  営業キロ(sales_km, kyusyu_sales_km, kyusyu_calc_km)
//
//	showFare() => calc_fare() ->
//	CheckOfRule114j() => calc_fare ->
//
int Route::CheckAndApplyRule43_2j(const vector<RouteItem> &route)
{
	DbidOf dbid;
	int stage;
	int c;
	int rl;
	int km;
	int kagoshima_line_id;
	vector<RouteItem>::const_iterator ite = route.cbegin();

	stage = 0;
	c = 0;
	rl = 0;
	while (ite != route.cend()) {
		switch (stage) {
		case 0:
			if ((0 < ite->lineId) && (ite->stationId == dbid.StationIdOf_NISHIKOKURA)) {
				c = 1;
				stage = 1;
			} else if (ite->stationId == dbid.StationIdOf_HAKATA) {
				if (ite->lineId == dbid.LineIdOf_SANYOSHINKANSEN) {
					c = 4;
				} else {
					c = 2;
				}
				stage = 1;
			} else if ((0 < ite->lineId) && (ite->stationId == dbid.StationIdOf_YOSHIZUKA)) {
				c = 3;
				stage = 1;
				break;
			}
			break;
		case 1:
			switch (c) {
			case 1:
				if ((ite->stationId == dbid.StationIdOf_KOKURA) && 
				((ite + 1) != route.cend()) &&
				((ite + 1)->lineId == dbid.LineIdOf_SANYOSHINKANSEN) &&
				((ite + 1)->stationId == dbid.StationIdOf_HAKATA)) {
					rl |= 1;
				}
				break;
			case 2:
				if ((ite->stationId == dbid.StationIdOf_KOKURA) && 
				(ite->lineId == dbid.LineIdOf_SANYOSHINKANSEN) &&
				((ite + 1) != route.cend()) &&
				((ite + 2) != route.cend()) &&	// 西小倉止まりまでなら非適用
				((ite + 1)->stationId == dbid.StationIdOf_NISHIKOKURA)) {
					rl |= 1;
				}
				break;
			case 3:
				if ((ite->stationId == dbid.StationIdOf_HAKATA) && 
				((ite + 1) != route.cend()) &&
				((ite + 1)->lineId == dbid.LineIdOf_SANYOSHINKANSEN)) {
					rl |= 2;
				}
				break;
			case 4:
				if ((ite->stationId == dbid.StationIdOf_YOSHIZUKA) &&
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
		kagoshima_line_id = Route::GetLineId(_T("鹿児島線"));
		if ((rl & 1) != 0) {
			km = Route::GetDistance(kagoshima_line_id, dbid.StationIdOf_KOKURA, dbid.StationIdOf_NISHIKOKURA)[0];
			TRACE(_T("applied 43-2(西小倉)\n"));
		}
		if ((rl & 2) != 0) {
			km += Route::GetDistance(kagoshima_line_id, dbid.StationIdOf_HAKATA, dbid.StationIdOf_YOSHIZUKA)[0];
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
//	B1LID_xxx(route[0].lineId)の設定前であることを前提(0を設定)とします
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

	if (!chk_distance1) {	/* chk_distance: 山陽線 神戸-姫路間営業キロ, 新幹線 新大阪-姫路 */
		chk_distance1 = Route::GetDistance(dbid.LineIdOf_SANYO, dbid.StationIdOf_KOUBE, dbid.StationIdOf_HIMEJI)[0];
		chk_distance2 = Route::GetDistance(dbid.LineIdOf_SANYOSHINKANSEN, dbid.StationIdOf_SHINOSAKA, dbid.StationIdOf_HIMEJI)[0];
	}

	if (2 <= lastIndex) {
		    // 新大阪 発 東海道線 - 山陽線
		if ((route->front().stationId == dbid.StationIdOf_SHINOSAKA) && 
			(route->at(1).lineId == dbid.LineIdOf_TOKAIDO) &&
		    (route->at(2).lineId == dbid.LineIdOf_SANYO) &&
		    (chk_distance1 <= Route::GetDistance(dbid.LineIdOf_SANYO, dbid.StationIdOf_KOUBE, route->at(2).stationId)[0])) {

			ASSERT(route->at(1).stationId == dbid.StationIdOf_KOUBE);
			/*	新大阪発東海道線-山陽線-姫路以遠なら発駅を新大阪->大阪へ */
			route->front() = RouteItem(0, dbid.StationIdOf_OSAKA);	// 新大阪->大阪

			return 1;
		}	// 新大阪 着 山陽線 - 東海道線
		else if ((route->back().stationId == dbid.StationIdOf_SHINOSAKA) && 
				 (route->back().lineId == dbid.LineIdOf_TOKAIDO) &&
				 (route->at(lastIndex - 1).lineId == dbid.LineIdOf_SANYO) &&
		    	 (chk_distance1 <= Route::GetDistance(dbid.LineIdOf_SANYO, dbid.StationIdOf_KOUBE, route->at(lastIndex - 2).stationId)[0])) {

			ASSERT(route->at(lastIndex - 1).stationId == dbid.StationIdOf_KOUBE);
			/*	新大阪着東海道線-山陽線-姫路以遠なら着駅を新大阪->大阪へ */
			route->back() = RouteItem(dbid.LineIdOf_TOKAIDO, dbid.StationIdOf_OSAKA);	// 新大阪->大阪

			return 2;
		}
		    // 大阪 発 新大阪 経由 山陽新幹線
		if ((route->front().stationId == dbid.StationIdOf_OSAKA) && 
			(route->at(2).lineId == dbid.LineIdOf_SANYOSHINKANSEN) &&
			(route->at(1).stationId == dbid.StationIdOf_SHINOSAKA) &&
			(chk_distance2 <= Route::GetDistance(dbid.LineIdOf_SANYOSHINKANSEN, dbid.StationIdOf_SHINOSAKA, route->at(2).stationId)[0])) {

			ASSERT(route->at(1).lineId == dbid.LineIdOf_TOKAIDO);

			/* 大阪発-東海道線上り-新大阪-山陽新幹線 姫路以遠の場合、大阪発-東海道線-山陽線 西明石経由に付け替える */
			
			route->at(1) = RouteItem(dbid.LineIdOf_TOKAIDO, dbid.StationIdOf_KOUBE);
			route->at(1).flag |= (FLG_HIDE_LINE | FLG_HIDE_STATION);

			vector<RouteItem>::iterator ite = route->begin();
			ite += 2;	// at(2)						// 山陽線-西明石
			ite = route->insert(ite, RouteItem(dbid.LineIdOf_SANYO, dbid.StationIdOf_NISHIAKASHI));
			ite->flag |= (FLG_HIDE_LINE | FLG_HIDE_STATION);

			return 1;
		}	// 山陽新幹線 新大阪 経由 大阪 着
		else if ((route->back().stationId == dbid.StationIdOf_OSAKA) && 
				 (route->at(lastIndex - 1).stationId == dbid.StationIdOf_SHINOSAKA) &&
				 (route->at(lastIndex - 1).lineId == dbid.LineIdOf_SANYOSHINKANSEN) &&
				 (chk_distance2 <= Route::GetDistance(dbid.LineIdOf_SANYOSHINKANSEN, dbid.StationIdOf_SHINOSAKA, route->at(lastIndex - 2).stationId)[0])) {

			ASSERT((route->back().lineId == dbid.LineIdOf_TOKAIDO));

			/* 山陽新幹線 姫路以遠〜新大阪乗換東海道線-大阪着の場合、最後の東海道線-大阪 を西明石 山陽線、東海道線に付け替える */

			route->at(lastIndex - 1) = RouteItem(dbid.LineIdOf_SANYOSHINKANSEN, dbid.StationIdOf_NISHIAKASHI);	// 新大阪->西明石
			route->at(lastIndex - 1).flag |= FLG_HIDE_STATION;
			route->at(lastIndex).flag |= FLG_HIDE_LINE;	// 東海道線 非表示
			vector<RouteItem>::iterator ite = route->end();
			ite--;
			ite = route->insert(ite, RouteItem(dbid.LineIdOf_SANYO, dbid.StationIdOf_KOUBE));
			ite->flag |= (FLG_HIDE_LINE | FLG_HIDE_STATION);

			return 2;
		}
	}
	if (1 <= lastIndex) {
		    // 新大阪 発 山陽新幹線
		if ((route->front().stationId == dbid.StationIdOf_SHINOSAKA) && 
			(route->at(1).lineId == dbid.LineIdOf_SANYOSHINKANSEN) &&
			(chk_distance2 <= Route::GetDistance(dbid.LineIdOf_SANYOSHINKANSEN, dbid.StationIdOf_SHINOSAKA, route->at(1).stationId)[0])) {

			/* 大阪発-東海道線上り-新大阪-山陽新幹線 姫路以遠の場合、大阪発-東海道線-山陽線 西明石経由に付け替える */
			
			vector<RouteItem>::iterator ite = route->begin();
			*ite = RouteItem(0, dbid.StationIdOf_OSAKA);
			ite++;
			ite = route->insert(ite, RouteItem(dbid.LineIdOf_SANYO, dbid.StationIdOf_NISHIAKASHI));
			ite->flag |= (FLG_HIDE_LINE | FLG_HIDE_STATION);

			ite = route->insert(ite, RouteItem(dbid.LineIdOf_TOKAIDO, dbid.StationIdOf_KOUBE));
			ite->flag |= (FLG_HIDE_LINE | FLG_HIDE_STATION);

			return 1;

		}	// 山陽新幹線 大阪 着
		else if ((route->back().stationId == dbid.StationIdOf_SHINOSAKA) && 
				 (route->back().lineId == dbid.LineIdOf_SANYOSHINKANSEN) &&
				 (chk_distance2 <= Route::GetDistance(dbid.LineIdOf_SANYOSHINKANSEN, dbid.StationIdOf_SHINOSAKA, route->at(lastIndex - 1).stationId)[0])) {

			/* 山陽新幹線 姫路以遠〜新大阪乗換東海道線-大阪着の場合、最後の東海道線-大阪 を西明石 山陽線、東海道線に付け替える */

			route->back() = RouteItem(dbid.LineIdOf_SANYOSHINKANSEN, dbid.StationIdOf_NISHIAKASHI);	// 新大阪->西明石
			route->back().flag |= FLG_HIDE_STATION;

			route->push_back(RouteItem(dbid.LineIdOf_SANYO, dbid.StationIdOf_KOUBE));	// add 山陽線-神戸
			route->back().flag |= (FLG_HIDE_LINE | FLG_HIDE_STATION);

			route->push_back(RouteItem(dbid.LineIdOf_TOKAIDO, dbid.StationIdOf_OSAKA));	// add 東海道線-大阪
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
//	89条のチェックと変換
//	北新地発（着）-[JR東西線]-尼崎なら、北新地→大阪置換
//
//	@param [in] route    route
//	
//	@retval 0: no-convert
//	@retval <0: 大阪-尼崎の営業キロ - 北新地-尼崎の営業キロ(負数)
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

				// 北新地発
	if (((route.front().stationId == dbid.StationIdOf_KITASHINCHI) && 
		(route.at(1).stationId == dbid.StationIdOf_AMAGASAKI)) && 
		((lastIndex <= 1) || (route.at(2).lineId != dbid.LineIdOf_TOKAIDO) || 
			(LDIR_ASC == Route::DirLine(dbid.LineIdOf_TOKAIDO, dbid.StationIdOf_AMAGASAKI, route.at(2).stationId)))) {
		/* 北新地-(JR東西線)-尼崎 の場合、発駅（北新地）を大阪に */
		// route.front().stationId = dbid.StationIdOf_OSAKA;
		if (distance == 0) {
			distance = Route::GetDistance(dbid.LineIdOf_TOKAIDO,   dbid.StationIdOf_OSAKA, dbid.StationIdOf_AMAGASAKI)[0] -
					   Route::GetDistance(route.at(1).lineId, dbid.StationIdOf_AMAGASAKI, dbid.StationIdOf_KITASHINCHI)[0];
		}
		ASSERT(distance < 0);
		return distance;
	}			// 北新地終着
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
//	114条のチェック
//
//	@param [in] route         経路(86, 87変換前)
//	@param [in] routeSpecial  経路(86, 87変換後)
//	@param [in] kind       種別
//							bit0:発駅が特定都区市内または山手線内
//							bit1:着駅が特定都区市内または山手線内
//							bit15:OFF=特定都区市内
//							      ON =山手線内
//	@param [out] rule114   (戻り値True時のみ有効) [0]適用運賃(未適用時、0), [1]営業キロ, [2]計算キロ)
//	@retval true  114適用
//	@retval false 114非適用
//
//	@note 86/87適用後の営業キロが200km/100km以下であること.
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
	vector<int> km_raw;		// 86 or 87 適用前 [0]:営業キロ、[1]計算キロ
#endif
	vector<int> km_spe;		// 86 or 87 適用後 [0]:営業キロ、[1]計算キロ
	FARE_INFO fi;
	int fare_normal;			/* 200(100)km以下により, 86 or 87 非適用の通常計算運賃 */
	int fare_applied;			/* より遠い駅までの都区市内発着の仮適用運賃 */

	/* 経路は乗換なしの単一路線 */
	if (route.size() <= 2) {
		return 0;
	}

#ifdef _DEBUG
	km_raw = Route::Get_route_distance(route); 			/* 経路距離 */
	ASSERT(km_raw.size() == 2);			// [0]:営業キロ、[1]計算キロ
#endif
	km_spe = Route::Get_route_distance(routeSpecial); 	/* 経路距離(86,87適用後) */
	ASSERT(km_spe.size() == 2);
	
	aSales_km = km_spe.at(0);			// 営業キロ

	/* 中心駅〜目的地は、180(90) - 200(100)km未満であるのが前提 */
	if ((0x8000 & kind) == 0) {
		// 200km
		km = 2000;
	} else {
		// 100km
		km = 1000;
	}
	/* 距離があと86、87条適用距離-10kmの範囲内ではない */
	if ((aSales_km < (km * 9 / 10)) || (km < aSales_km)) {
		return 0;
	}

	if ((kind & 1) != 0) {		/* 発駅が特定都区市内 */
		line_id = route.at(route.size() - 1).lineId;				// 着 路線 発-着
		station_id1 = route.at(route.size() - 2).stationId;
		station_id2 = route.at(route.size() - 1).stationId;
	} else if ((kind & 2) != 0) { /* 着駅が特定都区市内 */
		line_id = route.at(1).lineId;								// 発 路線.発-着
		station_id1 = route.at(1).stationId;
		station_id2 = route.at(0).stationId;
	} else {
		ASSERT(FALSE);
		return false;					// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	}
	// ex. 国母-横浜-長津田の場合、身延線.富士-国母 間の距離を引く
	dkm = Route::GetDistance(line_id, station_id1, station_id2).at(0);
	aSales_km -= dkm;	/* 発駅から初回乗換駅までの営業キロを除いた営業キロ */

	ASSERT(0 <= aSales_km);
	ASSERT(0 < dkm);

	if (LDIR_ASC != Route::DirLine(line_id, station_id1, station_id2)) {
		/* 上り */
		km = -km;
	}
	/* 中心駅から目的地方向に最初に200(100)kmに到達する駅を得る */
	/* 富士から身延線で甲府方向に */
	station_id3 = Route::Retreive_SpecificCoreAvailablePoint(km, aSales_km, line_id, station_id1);
	
	//#2013.6.28:modified>> ASSERT(0 < station_id3);
	if (station_id3 <= 0) {
		return false;
	}
	//<<

	/* (86, 87条適用前されなかった)運賃その駅までの運賃より高ければ、その駅までの運賃とする */

	vector<RouteItem> route_work;	// <- routeSpecial
	route_work.assign(routeSpecial.cbegin(), routeSpecial.cend());

	if ((kind & 1) != 0) {			/* 発駅が特定都区市内 */
		route_work.back().stationId = station_id3;	/* 最終着駅を置き換える */
	} else if ((kind & 2) != 0) {	/* 着駅が特定都区市内 */
		route_work.front().stationId = station_id3;	/* 始発駅を置き換える */
	} else {
		ASSERT(FALSE);
		return false;					// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	}
	
	/* 通常運賃を得る */
	if (!fi.calc_fare(route)) {
		ASSERT(FALSE);
		return false;					// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	}
	fare_normal = fi.getFareForJR();			/* 200(100)km以下により, 86 or 87 非適用の通常計算運賃(長津田-横浜-国母) */

#ifdef _DEBUG
	ASSERT(km_raw[0] == fi.getJRSalesKm());
	ASSERT(km_raw[1] == fi.getJRCalcKm());
#endif
	/* 86,87適用した最短駅の運賃を得る(上例では甲斐住吉-横浜間) */
	if (!fi.calc_fare(route_work)) {
		ASSERT(FALSE);
		return false;					// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	} 
	fare_applied = fi.getFareForJR();			/* より遠い駅までの都区市内発着の仮適用運賃(横浜-甲斐住吉) */

	if (fare_applied < fare_normal) {	
		/* 114条適用 */
		TRACE("Rule 114 Applied(%d->%d)\n", fare_normal, fare_applied);
		result[0] = fare_applied;		/* 先の駅の86,87適用運賃 */
		result[1] = fi.getJRSalesKm();
		result[2] = fi.getJRCalcKm();
		return true;
	}
	TRACE("Rule 114 no applied\n");
	return false;
}


//static
//	指定路線・駅からの100/200km到達地点の駅を得る
//	CheckOfRule114j() =>
//
//	@param [in] km  下り: 100=1000, 200=2000(上り: -1000 / -2000)
//	@param [in] line_id   路線
//	@param [in] station_id   起点駅
//	@retval 到達駅id. 0はなし
//
int Route::Retreive_SpecificCoreAvailablePoint(int km, int km_offset, int line_id, int station_id)
{
	// 上り方向で200km到達地点
	static const char tsql_desc[] = 
	"select l2.sales_km - l1.sales_km+%u as sales_km, l1.station_id"
	" from  t_lines l1 left join t_lines l2"
	" where l1.line_id=?1 and l2.line_id=?1"
	" and   l2.sales_km>l1.sales_km"
	" and   l2.station_id=?2"
	" and	(l1.lflg&(1<<31))=0"
	" and   (l2.sales_km-l1.sales_km)>%u order by l1.sales_km desc limit(1)";

	// 下り方向で200km到達地点
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
		km = -km;	/* 上り */
		sqlite3_snprintf(sizeof(sql_buf), sql_buf, tsql_desc, km_offset, km - km_offset);
	} else {		/* 下り */
		sqlite3_snprintf(sizeof(sql_buf), sql_buf, tsql_asc, km_offset, km - km_offset);
	}
	int akm;
	int aStationId;
	DBO dbo(DBS::getInstance()->compileSql(sql_buf));

	dbo.setParam(1, line_id);
	dbo.setParam(2, station_id);

	if (dbo.moveNext()) {
		akm = dbo.getInt(0);		// 未使用(営業キロじゃ貰っても要らん)
		aStationId = dbo.getInt(1);
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
bool Route::IsAbreastShinkansen(int line_id1, int line_id2, int station_id1, int station_id2)
{
	if (!IS_SHINKANSEN_LINE(line_id2)) {
		return false;
	}
	return line_id1 == GetHZLine(line_id2, station_id1, station_id2);
}


//static
//	並行在来線を得る
//
//	@param [in] line_id     路線(新幹線)
//	@param [in] station_id	駅(並行在来線駅(新幹線接続駅)
//
//	@retval not 0 並行在来線
//	@retval 0xffff 並行在来線は2つあり、その境界駅である(上越新幹線 高崎)
//
int Route::GetHZLine(int line_id, int station_id, int station_id2 /* =-1 */)
{
// 新幹線-並行在来線取得クエリ(GetHZLine)
const char tsql_hzl1[] = "select line_id from t_hzline where rowid=("
	"	select ((lflg>>19)&15) from t_lines where line_id=?1 and station_id=?2"
	"	)";
const char tsql_hzl2[] = "select line_id from t_hzline where line_id<32767 and rowid in ("
	"	select ((lflg>>19)&15) from t_lines where ((lflg>>19)&15)!=0 and line_id=?1 and "
	"	case when (select sales_km from t_lines where line_id=?1 and station_id=?2)<"
	"	          (select sales_km from t_lines where line_id=?1 and station_id=?3)"
	"	then"
	"	sales_km>(select sales_km from t_lines where line_id=?1 and station_id=?2)"
	"	else"
	"	sales_km<(select sales_km from t_lines where line_id=?1 and station_id=?2)"
	"	end"
	") limit(1);";

	int lineId = 0;
	DBO dbo(DBS::getInstance()->compileSql(tsql_hzl1));

	ASSERT(IS_SHINKANSEN_LINE(line_id));

	dbo.setParam(1, line_id);
	dbo.setParam(2, station_id);

	if (dbo.moveNext()) {
		lineId = dbo.getInt(0);
		if (32767 < lineId) {
			DBO dbo2(DBS::getInstance()->compileSql(tsql_hzl2));
			dbo2.setParam(1, line_id);
			dbo2.setParam(2, station_id);
			dbo2.setParam(3, station_id2);
			if (dbo2.moveNext()) {
				lineId = dbo2.getInt(0);
			}
		}
	}
	return lineId;
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
//	@return true: OK / false: NG
//
//                 国府津 s1                東京
// l1 東海道線     小田原 s2 東海道新幹線   静岡
// l2 東海道新幹線 名古屋 s3 東海道線       草薙
//
bool Route::CheckTransferShinkansen(int line_id1, int line_id2, int station_id1, int station_id2, int station_id3)
{
	int bullet_line;
	int local_line;
	int dir;
	int hzl;
	
	if (IS_SHINKANSEN_LINE(line_id2)) {
		bullet_line = line_id2;		// 在来線->新幹線乗換
		local_line = line_id1;
	} else if (IS_SHINKANSEN_LINE(line_id1)) {
		bullet_line = line_id1;		// 新幹線->在来線乗換
		local_line = line_id2;
	} else {
		return true;				// それ以外は対象外
	}
	hzl = Route::GetHZLine(bullet_line, station_id2, 
			(bullet_line == line_id2) ? station_id2 : station_id1);
	if (local_line != hzl) {
		return true;
	}

	// table.A
	dir = Route::DirLine(line_id1, station_id1, station_id2);
	if (dir == Route::DirLine(line_id2, station_id2, station_id3)) {
		return true;		// 上り→上り or 下り→下り
	}
	if (dir == LDIR_ASC) {	// 下り→上り
		return (((Route::AttrOfStationOnLineLine(local_line, station_id2) >> 19) & 0x01) != 0);
	} else {				// 上り→下り
		return (((Route::AttrOfStationOnLineLine(local_line, station_id2) >> 19) & 0x02) != 0);
	}
	return true;
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
int Route::NextShinkansenTransferTerm(int line_id, int station_id1, int station_id2)
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

//static
//	両隣の分岐駅を得る(非分岐駅指定、1つか2つ)
//	changeNeerest() =>
//
//	@param [in] station_id    駅(非分岐駅)
//	@return PAIRIDENT[2] or PAIRIDENT[1] 近隣の分岐駅(盲腸線の場合1、以外は2)
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
//	最短経路に変更(raw immidiate)
//
//	@param [in] useBulletTrain (bool)新幹線使用有無
//	@retval true success
//	@retval 1 : success
//	@retval 0 : loop end.
//	@retval -n: add() error(re-track)
//	@retval -32767 unknown error(DB error or BUG)
//
int Route::changeNeerest(bool useBulletTrain)
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
	
	/* 途中追加か、最初からか */
	if (1 < route_list_raw.size()) {
		stationId = route_list_raw.back().stationId;
	} else {
		stationId = startStationId();
	}

	if (stationId == endStationId) {
		return 0;			/* already routed */
	}

	/* ダイクストラ変数初期化 */
	for (i = 0; i < MAX_JCT; i++) {
		minCost[i] = -1;
		fromNode[i] = 0;
		done_flg[i] = false;
		line_id[i] = 0;
	}

	startNode = Route::Id2jctId(stationId);
	lastNode = Route::Id2jctId(endStationId);
	if (startNode == 0) { /* 開始駅は非分岐駅 */
		lid = Route::LineIdFromStationId(stationId);
		if ((lastNode == 0) & (lid == Route::LineIdFromStationId(endStationId))) {  /* 発駅と着駅は同一路線 (A) */
		//if (0 < Route::InStationOnLine(lid, endStationId)) { /* 発駅と着駅は同一路線 (B) */
			// 大崎-田端は山手線より東海道・東北線経由のほうが近くそちらが選択される(A)にした
			// !!!だが不完全。他の例は？(大崎-駒込が山手線経由の方が短いのは偶然))
			///km = Route::Get_node_distance(lid, stationId, endStationId);
			// 渋谷-原宿など >>>>>>>>>>>>>>>>>>>>>
			if (1 < route_list_raw.size() && route_list_raw.back().lineId == lid) {
				removeTail();
			}
			return add(lid, /*stationId,*/ endStationId);
		}
		
		// 駅の両隣の最寄分岐駅IDとその駅までの計算キロを得る
		vector<PAIRIDENT> neer_node = Route::GetNeerNode(stationId);

		// 発駅〜最初の分岐駅までの計算キロを最初の分岐駅までの初期コストとして初期化
		a = Route::Id2jctId(IDENT1(neer_node.at(0)));
		if (!IsJctMask(a)) {
			minCost[a - 1] = IDENT2(neer_node.at(0));
			fromNode[a - 1] = -1;	// from駅を-1(分岐駅でないので存在しない分岐駅)として初期化
			line_id[a - 1] = lid;
		}
		if (2 <= neer_node.size()) {
			b = Route::Id2jctId(IDENT1(neer_node.at(1)));
			if (!IsJctMask(b)) {
				minCost[b - 1] = IDENT2(neer_node.at(1));
				fromNode[b - 1] = -1;
				line_id[b - 1] = lid;
			} else if (IsJctMask(a)) {
				TRACE("Autoroute:発駅の両隣の分岐駅は既に通過済み");
				return -10;
			}
		} else {
			/* 盲腸線 */
			if (IsJctMask(a)) {
				TRACE("Autoroute:盲腸線で通過済み.");
				return -11;
			}
		}
	} else {
		minCost[startNode - 1] = 0;
	}

	if (lastNode == 0) { /* 終了駅は非分岐駅 ? */
		// 駅の両隣の最寄分岐駅IDとその駅までの計算キロを得る
		vector<PAIRIDENT> neer_node = Route::GetNeerNode(endStationId);

		// dijkstraのあとで使用のために変数に格納
		// 終了駅の両隣の分岐駅についてはadd()でエラーとなるので不要(かどうか？）
	
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
			// ノードiが確定しているとき
			// ノードiまでの現時点での最小コストが不明の時
			if (done_flg[i] || (minCost[i] < 0)) {
				continue;
			}
			/*  確定したノード番号が-1かノードiの現時点の最小コストが小さいとき
			 *  確定ノード番号更新する
			 */
			if ((doneNode < 0) || (minCost[i] < minCost[doneNode])) {
				doneNode = i;
			}
		}
		if (doneNode == -1) {
			break;	/* すべてのノードが確定したら終了 */
		}
		done_flg[doneNode] = true;	// Enter start node

		TRACE(_T("[%s]"), Route::StationName(Jct2id(doneNode + 1)).c_str());
		if (nLastNode == 0) {
			if ((doneNode + 1) == lastNode) {
				break;	/* 着ノードが完了しても終了可 */
			}
		} else if (nLastNode == 1) {
			if ((doneNode + 1) == lastNode1) {
				break;	/* 着ノードが完了しても終了可 */
			}
		} else if (nLastNode == 2) {
			if (done_flg[lastNode1 - 1] && 
				done_flg[lastNode2 - 1]) {
				break;	/* 着ノードが完了しても終了可 */
			}
		}

		vector<vector<int>> nodes = Route::Node_next(doneNode + 1);
		vector<vector<int>>::const_iterator ite;

		for (ite = nodes.cbegin(); ite != nodes.cend(); ite++) {

			a = ite->at(0) - 1;	// jctId

			if ((!IsJctMask(a + 1) || ((nLastNode == 0) && (lastNode == (a + 1))) ||
			                          ((0 < nLastNode) && (lastNode1 == (a + 1))) || 
			                          ((1 < nLastNode) && (lastNode2 == (a + 1)))) && 
			    (useBulletTrain || !IS_SHINKANSEN_LINE(ite->at(2)))) {

				/* 新幹線でない */
				cost = minCost[doneNode] + ite->at(1); // cost

				// ノードtoはまだ訪れていないノード
				// またはノードtoへより小さいコストの経路だったら
				// ノードtoの最小コストを更新
				if (((minCost[a] < 0) || (cost <= minCost[a])) &&
					((cost != minCost[a]) || IS_SHINKANSEN_LINE(ite->at(2)))) {
					/* ↑ 同一距離に2線ある場合新幹線を採用 */
					minCost[a] = cost;
					fromNode[a] = doneNode;
					line_id[a] = ite->at(2);
					TRACE( _T("+<%s>"), Route::StationName(Jct2id(a + 1)).c_str());
				} else {
					TRACE(_T("-<%s>"), Route::StationName(Jct2id(a + 1)).c_str());
				}
			}
		}
		TRACE("\n");
	}


	vector<IDENT> route;
	int lineid = 0;
	int idb;
	
	if (lastNode == 0) { /* 終了駅は非分岐駅 ? */
		// 最後の分岐駅の決定：
		// 2つの最後の分岐駅候補(終了駅(非分岐駅）の両隣の分岐駅)〜終了駅(非分岐駅)までの
		// 計算キロ＋2つの最後の分岐駅候補までの計算キロは、
		// どちらが短いか？
	
		if ((2 == nLastNode) && 
		   ((minCost[lastNode2 - 1] + lastNode2_distance) < 
		    (minCost[lastNode1 - 1] + lastNode1_distance))) {
			id = lastNode2;		// 短い方を最後の分岐駅とする
		} else {
			id = lastNode1;
		}
	} else {
		id = lastNode;
	}

	if (fromNode[id - 1] == 0) {
		TRACE( _T("can't lowcost route.\n"));
		return -1002;
	}
	//これがあるとL字(綾瀬-東川口)で失敗。何故入れてたのか？？？
	//b#13091401

	// 発駅(=分岐駅)でなく最初の分岐駅(-1+1=0)でない間
	// 最後の分岐駅からfromをトレース >> route[]
	while ((id != startNode) && (0 < id)) {
		TRACE( _T("  %s, %s, %s."), Route::LineName(lineid).c_str(), Route::LineName(line_id[id - 1]).c_str(), Route::StationName(Jct2id(id)).c_str());
		if (lineid != line_id[id - 1]) {
			if (IS_SHINKANSEN_LINE(lineid)) {
				//printf("@@@@->%d\n", lineid);
				 	/* 新幹線→並行在来線 */
				int zline = Route::GetHZLine(lineid, Route::Jct2id(id));
				for (idb = id; (idb != startNode) && (line_id[idb - 1] == zline); 
				     idb = fromNode[idb - 1] + 1) {
					TRACE( _T("    ? %s %s/"),  Route::LineName(line_id[idb - 1]).c_str(), Route::StationName(Jct2id(idb)).c_str());
					;
				}
				if (line_id[idb - 1] == lineid) { /* もとの新幹線に戻った ? */
					//printf(".-.-.-");
					id = idb;
					continue;
				} else if (idb == startNode) { /* 発駅？ */
					if (zline == Route::GetHZLine(lineid, Route::Jct2id(idb))) {
						id = idb;
						continue;
					}
					/* thru */
					//printf("*-*-*-");
				} else if (idb != id) { /* 他路線の乗り換えた ? */
					TRACE( _T("%sはそうだが、%sにも新幹線停車するか?"), Route::StationName(Jct2id(id)).c_str(), Route::StationName(Jct2id(idb)).c_str());
					if (zline == Route::GetHZLine(lineid, Route::Jct2id(idb))) {
						id = idb;
						continue;
					}
					/* thru */
					TRACE( _T("+-+-+-: %s(%s) : "), Route::LineName(line_id[idb - 1]).c_str(), Route::LineName(lineid).c_str());
				} else {
					//printf("&");
				}
			} else { /* 前回新幹線でないなら */
				/* thru */
				//printf("-=-=-=");
			}
// 次の新幹線でも並行在来線でもない路線への分岐駅に新幹線分岐駅でない場合(金山)最初の在来線切り替えを有効にする（三河安城）
// 新幹線に戻ってきている場合(花巻→盛岡）、花巻まで（北上から）無効化にする
// 
			route.push_back(id - 1);
			lineid = line_id[id - 1];
			TRACE( _T("  o\n"));
		} else {
			TRACE( _T("  x\n"));
		}
		id = fromNode[id - 1] + 1;
	}
	
	//// 発駅=分岐駅


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

	if (lastNode == 0) {	// 着駅は非分岐駅?
		lid = Route::LineIdFromStationId(endStationId); // 着駅所属路線ID
		// 最終分岐駅〜着駅までの営業キロ、運賃計算キロを取得
		//km = Route::Get_node_distance(lid, endStationId, Route::Jct2id(a));
		//km += minCost[route.back()];	// 最後の分岐駅までの累積計算キロを更新
		if (lid == line_id[route.back()]) { // 着駅の最寄分岐駅の路線=最後の分岐駅?
			route.pop_back();	// if   渋谷-新宿-西国分寺-国立
								// else 渋谷-新宿-三鷹
		}
	} else {
		lid = 0;
	}
	
	if ((1 < route_list_raw.size()) && (1 < route.size()) && (route_list_raw.back().lineId == line_id[route[0]])) {
		removeTail();
		ASSERT(0 < route_list_raw.size()); /* route_list_raw.size() は0か2以上 */
		//stationId = route_list_raw.back().stationId;
	}

	for (i = 0; i < (int)route.size(); i++) {
		a = add(line_id[route[i]], /*stationId,*/ Route::Jct2id(route[i] + 1));
		BIT_ON(last_flag, BLF_JCTSP_ROUTE_CHANGE);	/* route modified */
		if (a <= 0) {
			//ASSERT(FALSE);
			if ((a < 0) || ((i + 1) < (int)route.size())) { 
				return a;	/* error */
			} else {
				break;
			}
		}
		//stationId = Route::Jct2id(route[i] + 1);
	}
	if (lastNode == 0) {
		ASSERT(0 < lid);
		if (a == 0) {
			return -1000;
		}
		a = add(lid, /*stationId,*/ endStationId);
		BIT_ON(last_flag, BLF_JCTSP_ROUTE_CHANGE);	/* route modified */
		if (a <= 0) {
			//ASSERT(FALSE);
			return a;
		}
	}
	route_list_cooked.clear();
	return 1;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// static
int FARE_INFO::tax = 0;

/**	往復運賃を返す(会社線含む総額)(JR分は601km以上で1割引)
 *
 *	@paramm discount [out]  true=割引あり
 *	@retval [円]
 */
int 	FARE_INFO::roundTripFareWithComapnyLine(bool& return_discount)
{
	int fareW;
	
	if (6000 < total_jr_calc_km) {	/* 往復割引 */
		fareW = fare_discount(fare, 1) * 2 + company_fare * 2;
		return_discount = true;
	} else {
		return_discount = false;
		fareW = fare * 2 + company_fare * 2;
	}
	return fareW;
}

/**	近郊区間内かを返す(有効日数を1にする)
 *
 *	@retval	true 近郊区間
 */
bool 	FARE_INFO::isUrbanArea()
{
	if (((MASK_URBAN & flag) != 0) && 
	     (MASK_FLAG_SHINKANSEN(flag) == 0)) {
	
		avail_days = 1;	/* 当日限り */

		ASSERT((IsIC_area(URBAN_ID(flag)) && (fare_ic != 0)) ||
	      (!FARE_INFO::IsIC_area(URBAN_ID(flag)) && (fare_ic == 0)));

		return true;
	} else {
		return false;
	}
}

/**	総営業キロを返す	
 *
 *	@retval 営業キロ
 */
int 	FARE_INFO::getTotalSalesKm()
{
	return sales_km;
}

/**	JR線の営業キロを返す
 *
 *	@retval 計算キロ
 */
int		FARE_INFO::getJRSalesKm()
{
	return total_jr_sales_km;
}

/**	JR線の計算キロを返す
 *
 *	@retval 計算キロ
 */
int		FARE_INFO::getJRCalcKm()
{
	return total_jr_calc_km;
}

/**	会社線の営業キロ総数を返す
 *
 *	@retval 営業キロ
 */
int		FARE_INFO::getCompanySalesKm()
{
	return sales_km - total_jr_sales_km;
}

/**	JR北海道の営業キロを返す
 *
 *	@retval 営業キロ
 */
int		FARE_INFO::getSalesKmForHokkaido()
{
	return hokkaido_sales_km;
}

/**	JR四国の営業キロを返す
 *
 *	@retval	営業キロ
 */
int		FARE_INFO::getSalesKmForShikoku()
{
	return shikoku_sales_km;
}

/**	JR九州の営業キロを返す
 *
 *	@retval	営業キロ
 */
int		FARE_INFO::getSalesKmForKyusyu()
{
	return kyusyu_sales_km;
}

/**	JR北海道の計算キロを返す
 *
 *	@retval	計算キロ
 */
int		FARE_INFO::getCalcKmForHokkaido()
{
	return hokkaido_calc_km;
}

/**	JR四国の計算キロを返す
 *
 *	@retval	計算キロ
 */
int		FARE_INFO::getCalcKmForShikoku()
{
	return shikoku_calc_km;
}

/**	JR九州の計算キロを返す
 *
 *	@retval	計算キロ
 */
int		FARE_INFO::getCalcKmForKyusyu()
{
	return kyusyu_calc_km;
}

/**	乗車券の有効日数を返す
 *
 *	@retval	有効日数[日]
 */
int		FARE_INFO::getTicketAvailDays()
{
	return avail_days;
}

/**	会社線の運賃額を返す
 *
 *	@retval	[円]
 */
int		FARE_INFO::getFareForCompanyline()
{
	return company_fare;
}

/**	JR線の運賃額を返す
 *
 *	@retval	[円]
 */
int		FARE_INFO::getFareForJR()
{
	return fare;
}

/**	株主優待割引運賃を返す
 *
 *	@param index      [in]  0から1 JR東日本のみ 0は2割引、1は4割引を返す
 *	@param idCompany [out]  0:JR東海1割/1:JR西日本5割/2:JR東日本2割/3:JR東日本4割
 *	@retval	[円](無割引、無効は0)
 */
int FARE_INFO::getFareStockDistount(int index, tstring& title)
{
	const TCHAR* const titles[] = {
		_T("JR東日本 株主優待2割："),
		_T("JR東日本 株主優待4割："),
		_T("JR西日本 株主優待5割："), 
		_T("JR東海   株主優待1割："), 
	};

	if ((JR_GROUP_MASK & companymask) == (1 << (JR_EAST - 1))) {
		if (index == 0) {
			title = titles[0];
			return fare_discount(fare, 2);
		} else if (index == 1) {
			/* JR東4割(2枚使用) */
			title = titles[1];
			return fare_discount(fare, 4);
		} else {
			return 0;
		}
	}
	if (index != 0) {
		return 0;
	}
	if ((JR_GROUP_MASK & companymask) == (1 << (JR_WEST - 1))) {
		title = titles[2];
		return fare_discount(fare, 5);

	} else if ((JR_GROUP_MASK & companymask) == (1 << (JR_CENTRAL - 1))) {
		title = titles[3];
		return fare_discount(fare, 1);
	} else {
		return 0;
	}
	// 通過連絡運輸も株優は有効らしい
}

/**	学割運賃を返す(会社線+JR線=全線)
 *
 *	@retval	学割運賃[円]
 */
int		FARE_INFO::getAcademicDiscount()
{
	int result_fare;

	// which ?
	result_fare = fare_discount(fare, 2) + fare_discount(company_fare, 2);
	
	//result_fare = fare_discount(fare + company_fare, 2);
	
	return result_fare;
}

/**	IC運賃を返す
 *
 *	@retval IC運賃(x10[円])
 */
int		FARE_INFO::getFareForIC()
{
	ASSERT(((fare_ic != 0) && (companymask == (1 << (JR_EAST - 1)))) || (fare_ic == 0));
	return fare_ic;
}

/* static */
/*	路線の2点間営業キロ、計算キロ、会社(JR-Gr.)境界を得る
 *	calc_fare() => aggregate_fare_info() =>
 *
 *	@param [in] line_id   	 路線ID
 *	@param [in] station_id1  駅1
 *	@param [in] station_id2  駅2
 *
 *	@return vector<int> [0] 営業キロ
 *	@return vector<int> [1] 計算キロ
 *	@return vector<int> [2] 駅1の会社区間部の営業キロ(駅1の会社ID≠駅2の会社ID時のみ有効)
 *                          駅1が境界駅なら-1を返す, 境界駅が駅1〜駅2間になければ、Noneを返す
 *	@return vector<int> [3] 駅1の会社区間部の計算キロ(駅1の会社ID≠駅2の会社ID時のみ有効)
 *                          駅2が境界駅なら-1を返す, 境界駅が駅1〜駅2間になければ、Noneを返す
 *	@return vector<int> [4] IDENT1(駅1の会社ID) + IDENT2(駅2の会社ID)
 *	@return vector<int> [5] bit31:1=JR以外の会社線／0=JRグループ社線 / IDENT1(駅1のsflg) / IDENT2(駅2のsflg(MSB=bit15除く))
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
"	case when exists (select * from t_lines	where line_id=?1 and (lflg&((1<<16)|(1<<31)))=(1<<16) and station_id=?2)"
"	then -1 else"
"	abs((select sales_km from t_lines where line_id=?1 and (lflg&((1<<16)|(1<<31)))=(1<<16)"
"	and	sales_km>(select min(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))"
"	and	sales_km<(select max(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3)))-"
"	(select sales_km from t_lines where line_id=?1 and station_id=?2)) end,"						// 2
"	case when exists (select * from t_lines"
"	where line_id=?1 and (lflg&((1<<16)|(1<<31)))=(1<<16) and station_id=?3)"
"	then -1 else"
"	abs((select calc_km from t_lines where line_id=?1 and (lflg&((1<<16)|(1<<31)))=(1<<16)"
"	and	sales_km>(select min(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))"
"	and	sales_km<(select max(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3)))-"
"	(select calc_km from t_lines where line_id=?1 and station_id=?2)) end,"							// 3
"	((select company_id from t_station where rowid=?2) + (65536 * (select company_id from t_station where rowid=?3))),"	// 4
"	((select 2147483648*(1&(lflg>>18)) from t_lines where line_id=?1 and station_id=?3) + "
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
			result.push_back(ctx.getInt(4));	// IDENT1(駅ID1の会社ID) + IDENT2(駅ID2の会社ID)
			result.push_back(ctx.getInt(5));	// bit31:1=JR以外の会社線／0=JRグループ社線 / IDENT1(駅1のsflg) / IDENT2(駅2のsflg(MSB=bit15除く))

			if ((line_id == dbid.LineIdOf_HAKATAMINAMISEN) || 
			    (dbid.LineIdOf_SANYOSHINKANSEN == line_id)) {	// 山陽新幹線
				result[2] = 0;
				result[3] = 0;
				result[4] = MAKEPAIR(JR_WEST, JR_WEST);
			}
		}
	}
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


//	1経路の営業キロ、計算キロを集計
//	calc_fare() =>
//
//	@param [in] line_id      路線
//	@param [in] station_id1  駅1
//	@param [in] station_id2  駅2
//	@param [in] station_id0  前回の駅1
//	@retval 0 < Success(会社線の場合、今回の駅1、JR線の場合0)
//	@retval -1 Fatal error
//
int FARE_INFO::aggregate_fare_info(int line_id, int station_id1, int station_id2, int station_id_0)
{
	int company_id1;
	int company_id2;
	int flag;

	if (line_id == ID_L_RULE70) {
		int sales_km;
		sales_km = FARE_INFO::Retrieve70Distance(station_id1, station_id2);
		ASSERT(0 < sales_km);
		this->sales_km += sales_km;			// total 営業キロ(会社線含む、有効日数計算用)
		this->base_sales_km	+= sales_km;
		this->base_calc_km += sales_km;
		this->local_only = false;		// 幹線

		return 0;
	}

	vector<int> d = FARE_INFO::GetDistanceEx(line_id, station_id1, station_id2);
	
	if (6 != d.size()) {
		ASSERT(FALSE);
		return -1;	/* failure abort end. >>>>>>>>> */
	}
	company_id1 = IDENT1(d.at(4));
	company_id2 = IDENT2(d.at(4));

TRACE(_T("multicompany line none detect X: %d, %d, comp1,2=%d, %d, %s:%s-%s\n"), d.at(2), d.at(3), company_id1, company_id2, Route::LineName(line_id).c_str(), Route::StationName(station_id1).c_str(), Route::StationName(station_id2).c_str());
	if (d.at(2) == -1) {		/* station_id1が境界駅の場合 */
		TRACE("multicompany line detect 1: %d, %d(com1 <- com2)\n", d.at(2), d.at(3));
		company_id1 = company_id2;
	}
	if (d.at(3) == -1) {		/* station_id2が境界駅の場合 */
		TRACE("multicompany line detect 2: %d, %d(com2 <- com1)\n", d.at(2), d.at(3));
		company_id2 = company_id1;
	}

	/* 通過会社 */
	companymask |= ((1 << (company_id1 - 1)) | ((1 << (company_id2 - 1))));

	if ((company_id1 == JR_CENTRAL) || (company_id1 == JR_WEST)) {
		company_id1 = JR_EAST;	/* 本州三社としてJR東海、JR西は、JR東とする */
	}
	if ((company_id2 == JR_CENTRAL) || (company_id2 == JR_WEST)) {
		company_id2 = JR_EAST;	/* 本州三社としてJR東海、JR西は、JR東とする */
	}

	this->sales_km += d.at(0);			// total 営業キロ(会社線含む、有効日数計算用)
	if (IS_COMPANY_LINE(d.at(5))) {	/* 会社線 */
		if (0 < station_id_0) {
			/* 2回連続で会社線の場合 */
			this->company_fare += FARE_INFO::Fare_company(station_id_0, station_id2);
			this->company_fare -= FARE_INFO::Fare_company(station_id_0, station_id1);
		} else {
			this->company_fare += FARE_INFO::Fare_company(station_id1, station_id2);
		}
		station_id_0 = station_id1;
	} else {
		station_id_0 = 0;
		/* JR Group */
		if (IS_SHINKANSEN_LINE(line_id)) {
			/* 山陽新幹線 広島-徳山間を通る場合の1経路では、fare(l, i) とされてしまうから.
			 */
			this->local_only = false;		// 幹線
		}
		if (company_id1 == company_id2) {		// 同一 1社
				/* 単一 JR Group */
			switch (company_id1) {
			case JR_HOKKAIDO:
				this->hokkaido_sales_km 	+= d.at(0);
				if (0 == d.at(1)) { 		/* 幹線? */
					this->hokkaido_calc_km 	+= d.at(0);
					this->local_only_as_hokkaido = false;
					this->local_only = false;	// 幹線
				} else {
					this->hokkaido_calc_km 	+= d.at(1);
				}
				break;
			case JR_SHIKOKU:
				this->shikoku_sales_km 		+= d.at(0);
				if (0 == d.at(1)) { 		/* 幹線? */
					this->shikoku_calc_km 	+= d.at(0);
					this->local_only = false;	// 幹線
				} else {
					this->shikoku_calc_km 	+= d.at(1);
				}
				break;
			case JR_KYUSYU:
				this->kyusyu_sales_km 		+= d.at(0);
				if (0 == d.at(1)) { 		/* 幹線? */
					this->kyusyu_calc_km 	+= d.at(0);
					this->local_only = false;	// 幹線
				} else {
					this->kyusyu_calc_km 	+= d.at(1);
				}
				break;
			case JR_EAST:
			case JR_CENTRAL:
			case JR_WEST:
				this->base_sales_km 		+= d.at(0);
				if (0 == d.at(1)) { 		/* 幹線? */
					this->base_calc_km 		+= d.at(0);
					this->local_only = false;	// 幹線
				} else {
					this->base_calc_km 		+= d.at(1);
				}
				break;
			default:
				ASSERT(FALSE);	/* あり得ない */
				break;
			}
		} else {								// 会社跨り?
			switch (company_id1) {
			case JR_HOKKAIDO:					// JR北海道->本州3社(JR東)
				ASSERT(FALSE);		// 北海道新幹線が開通するまでお預け
				this->hokkaido_sales_km 	+= d.at(2);
				this->base_sales_km 		+= (d.at(0) - d.at(2));
				if (0 == d.at(1)) { /* 幹線? */
					ASSERT(FALSE);	/* ありえない */
					this->hokkaido_calc_km  += d.at(2);
					this->base_calc_km  	+= (d.at(0) - d.at(2));
					this->local_only_as_hokkaido = false;
				} else {
					this->hokkaido_calc_km  += d.at(3);
					this->base_calc_km  	+= (d.at(1) - d.at(3));
				}
				break;
			case JR_SHIKOKU:					// JR四国->本州3社(JR西)
				this->shikoku_sales_km 		+= d.at(2);
				this->base_sales_km 		+= (d.at(0) - d.at(2));
				if (0 == d.at(1)) { 			/* 幹線? */
					this->shikoku_calc_km 	+= d.at(2);
					this->base_calc_km  	+= (d.at(0) - d.at(2));
					this->local_only = false;	// 幹線
				} else {
					this->shikoku_calc_km 	+= d.at(3);
					this->base_calc_km  	+= (d.at(1) - d.at(3));
				}
				break;
			case JR_KYUSYU:					// JR九州->本州3社(JR西)
				this->kyusyu_sales_km 		+= d.at(2);
				this->base_sales_km 		+= (d.at(0) - d.at(2));
				if (0 == d.at(1)) { /* 幹線? */
					this->kyusyu_calc_km 	+= d.at(2);
					this->base_calc_km 		+= (d.at(0) - d.at(2));
					this->local_only = false;	// 幹線
				} else {
					this->kyusyu_calc_km 	+= d.at(3);
					this->base_calc_km 		+= (d.at(1) - d.at(3));
				}
				break;
			default:
				this->base_sales_km 		+= d.at(2);
				if (0 == d.at(1)) { /* 幹線? */
					this->base_calc_km 		+= d.at(2);
					this->local_only = false;	// 幹線
				} else {
					this->base_calc_km 		+= d.at(3);
				}
				switch (company_id2) {
				case JR_HOKKAIDO:			// 本州3社(JR東)->JR北海道
					ASSERT(FALSE);	// 北海道新幹線が来るまでお預け
					this->hokkaido_sales_km		+= (d.at(0) - d.at(2));
					if (0 == d.at(1)) { /* 幹線? */
						this->hokkaido_calc_km	+= (d.at(0) - d.at(2));
						this->local_only_as_hokkaido = false;
					} else {
						this->hokkaido_calc_km	+= (d.at(1) - d.at(3));
					}
					break;
				case JR_SHIKOKU:			// 本州3社(JR西)->JR四国
					this->shikoku_sales_km		+= (d.at(0) - d.at(2));
					if (0 == d.at(1)) { /* 幹線? */
						this->shikoku_calc_km	+= (d.at(0) - d.at(2));
					} else {
						this->shikoku_calc_km	+= (d.at(1) - d.at(3));
					}
					break;
				case JR_KYUSYU:				// 本州3社(JR西)->JR九州
					this->kyusyu_sales_km		+= (d.at(0) - d.at(2));
					if (0 == d.at(1)) { /* 幹線? */
						this->kyusyu_calc_km	+= (d.at(0) - d.at(2));
					} else {
						this->kyusyu_calc_km	+= (d.at(1) - d.at(3));
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
	}
	if ((this->flag & FLAG_FARECALC_INITIAL) == 0) { // b15が0の場合最初なので駅1のフラグも反映
		this->flag = IDENT1(d.at(5));		// 駅1 sflgの下13ビット
	}
	flag = IDENT2(d.at(5));					// 駅2 sflgの下13ビット
	if ((flag & MASK_URBAN) != (this->flag & MASK_URBAN)) {/* 近郊区間(b7-9) の比較 */
		flag &= ~MASK_URBAN;				/* 近郊区間 OFF */
	}
	this->flag &= (flag | FLAG_SHINKANSEN);	/* b11,10,5(大阪/東京電車特定区間, 山手線／大阪環状線内) */
	this->flag |= FLAG_FARECALC_INITIAL;	/* 次回から駅1は不要 */
	if (IS_SHINKANSEN_LINE(line_id)) {
		this->flag |= ((line_id & MASK_SHINKANSEN) << BSHINKANSEN);	/* make flag for MASK_FLAG_SHINKANSEN() */
	}
	/* flag(sflg)は、b11,10,5, 7-9 のみ使用で他はFARE_INFOでは使用しない */
	return station_id_0;
}

// Private:
//	showFare() =>
//	CheckOfRule114j() =>
//
//	運賃計算
//
//	@param [in] routeList    経路
//	@param [in] applied_rule ルール適用(デフォルトTrue)
//	@return 異常の時はfalse
//
bool FARE_INFO::calc_fare(const vector<RouteItem>& routeList, bool applied_rule/* = true*/)
{
	vector<RouteItem>::const_iterator ite;
	int station_id1;
	int b_station_id;
	int fare_add;		/* 特別加算区間 */
	int adjust_km;
	
	reset();
	fare_add = 0;

	b_station_id = station_id1 = 0;
	this->local_only = true;
	this->local_only_as_hokkaido = true;
	for (ite = routeList.cbegin(); ite != routeList.cend(); ite++) {

//ASSERT((ite->flag) == 0);
		if (station_id1 != 0) {
							/* 会社別営業キロ、計算キロの取得 */
			b_station_id = FARE_INFO::aggregate_fare_info(ite->lineId, station_id1, ite->stationId, b_station_id);
			if (b_station_id < 0) {
				ASSERT(FALSE);
				goto err;		/* >>>>>>>>>>>>>>>>>>> */
			}
							// 特別加算区間
			fare_add += FARE_INFO::CheckSpecficFarePass(ite->lineId, station_id1, ite->stationId);
		}
		station_id1 = ite->stationId;
	}
	/* 43-2 */
	if (applied_rule) {
		adjust_km = Route::CheckAndApplyRule43_2j(routeList);
		this->sales_km			-= adjust_km * 2;
		this->base_sales_km		-= adjust_km;
		this->base_calc_km		-= adjust_km;
		this->kyusyu_sales_km	-= adjust_km;
		this->kyusyu_calc_km	-= adjust_km;
	}
	/* 乗車券の有効日数 */
	this->avail_days = FARE_INFO::days_ticket(this->sales_km);

	/* 89条適用 */
	if (applied_rule) {
		this->base_calc_km += Route::CheckOfRule89j(routeList);
	}
	/* 幹線のみ ? */
	this->major_only = ((this->base_sales_km == this->base_calc_km) && 
						(this->kyusyu_sales_km == this->kyusyu_calc_km) && 
						(this->hokkaido_sales_km == this->hokkaido_calc_km) && 
						(this->shikoku_sales_km == this->shikoku_calc_km));

	int special_fare = FARE_INFO::SpecficFareLine(routeList.front().stationId, routeList.back().stationId);
	if (0 < special_fare) {
		TRACE("specific fare section replace for Metro or Shikoku-Big-bridge\n");

		if ((FARE_INFO::tax != 5) && 
		    IsIC_area(URBAN_ID(this->flag)) &&     /* 東京 近郊区間(最短距離で算出可能) */
           (MASK_FLAG_SHINKANSEN(this->flag) == 0)) {

			ASSERT(companymask == (1 << (JR_EAST - 1)));  /* JR East only  */

			this->fare_ic = special_fare;
			this->fare = round_up(special_fare);	/* 大都市特定区間運賃(東京)(\10単位切り上げ) */

		} else {
			this->fare = special_fare;	/* 大都市特定区間運賃 */
		}

		this->total_jr_sales_km = this->base_sales_km;
		this->total_jr_calc_km = this->base_calc_km;

		ASSERT(this->company_fare == 0);	// 会社線は通っていない
		
		return true;	// >>>>>>>>>>>>>>>>>>>>
	}
	/* 運賃計算 */
	if (retr_fare()) {
		// 特別加算区間分
		this->fare += fare_add;
		return true;
	}
	
err:
	ASSERT(FALSE);
	this->reset();
	return false;
}

//static
//	集計された営業キロ、計算キロより運賃額を算出(運賃計算最終工程)
//	(JRのみ)
//	calc_fare() =>
//
//	@retval true Success
//	@retval false Fatal error
//
bool FARE_INFO::retr_fare()
{
	int fare_tmp;
	this->total_jr_sales_km = this->base_sales_km +
								this->kyusyu_sales_km +
								this->hokkaido_sales_km +
								this->shikoku_sales_km;

	this->total_jr_calc_km =this->base_calc_km +
								this->kyusyu_calc_km +
								this->hokkaido_calc_km +
								this->shikoku_calc_km;

				// 本州3社あり？
	if (0 < (this->base_sales_km + this->base_calc_km)) {
		if (IS_OSMSP(this->flag)) {
			/* 大阪電車特定区間のみ */
			ASSERT(this->fare == 0); /* 特別加算区間を通っていないはずなので */
			ASSERT(this->company_fare == 0);	// 会社線は通っていない
			ASSERT(this->base_sales_km == this->total_jr_sales_km);
			ASSERT(this->base_sales_km == this->sales_km);
			if (IS_YAMATE(this->flag)) {
				TRACE("fare(osaka-kan)\n");
				this->fare = FARE_INFO::Fare_osakakan(this->total_jr_sales_km);
			} else {
				TRACE("fare(osaka)\n");
				this->fare = FARE_INFO::Fare_osaka(this->total_jr_sales_km);
			}
		} else if (IS_TKMSP(this->flag)) { 
			/* 東京電車特定区間のみ */
			ASSERT(this->fare == 0); /* 特別加算区間を通っていないはずなので */
			ASSERT(this->company_fare == 0);	// 会社線は通っていない
			ASSERT(this->base_sales_km == this->total_jr_sales_km);
			ASSERT(this->base_sales_km == this->sales_km);

			if (IS_YAMATE(this->flag)) {
				TRACE("fare(yamate)\n");
				fare_tmp = FARE_INFO::Fare_yamate_f(this->total_jr_sales_km);
			} else {
				TRACE("fare(tokyo)\n");
				fare_tmp = FARE_INFO::Fare_tokyo_f(this->total_jr_sales_km);
			}
			if (FARE_INFO::tax == 5) {
				this->fare = round(fare_tmp);
			} else {
				if (MASK_FLAG_SHINKANSEN(this->flag) == 0) {
					this->fare_ic = fare_tmp;
				}
				this->fare = round_up(fare_tmp);
			}
		} else if (this->local_only || (!this->major_only && (this->total_jr_sales_km <= 100))) {
			/* 本州3社地方交通線のみ or JR東+JR北 */
			/* 幹線+地方交通線でトータル営業キロが10km以下 */
			// (i)<s>
			TRACE("fare(sub)\n");

			fare_tmp = FARE_INFO::Fare_sub_f(this->total_jr_sales_km);
			
			if ((FARE_INFO::tax != 5) && 
			    IsIC_area(URBAN_ID(this->flag)) &&   /* 近郊区間(最短距離で算出可能) */
				(MASK_FLAG_SHINKANSEN(this->flag) == 0)) {
               
				ASSERT(companymask == (1 << (JR_EAST - 1)));  /* JR East only  */

				this->fare_ic = fare_tmp;
			}
			this->fare = round(fare_tmp);

		} else { /* 幹線のみ／幹線+地方交通線 */
			// (a) + this->calc_kmで算出
			TRACE("fare(basic)\n");

			fare_tmp = FARE_INFO::Fare_basic_f(this->total_jr_calc_km);

			if ((FARE_INFO::tax != 5) && 
			    IsIC_area(URBAN_ID(this->flag)) &&   /* 近郊区間(最短距離で算出可能) */
				(MASK_FLAG_SHINKANSEN(this->flag) == 0)) {

				ASSERT(companymask == (1 << (JR_EAST - 1)));  /* JR East only  */

				this->fare_ic = fare_tmp;
			}
			this->fare = round(fare_tmp);
		}

		// JR北あり?
		if (0 < (this->hokkaido_sales_km + this->hokkaido_calc_km)) {
			/* JR東 + JR北 */
			if (this->local_only_as_hokkaido || 
			  (!this->major_only && (this->total_jr_sales_km <= 100))) {
				/* JR北は地方交通線のみ */
				/* or 幹線+地方交通線でトータル営業キロが10km以下 */
				// (r) sales_km add
				TRACE("fare(hla)\n");		// TRACE("fare(r,r)\n");
				this->fare += FARE_INFO::Fare_table((FARE_INFO::tax == 5) ? "hla5p" : "hla", "ha", 
				                                    this->hokkaido_sales_km);

			} else { /* 幹線のみ／幹線+地方交通線で10km越え */
				// (o) calc_km add
				TRACE("fare(add, ha)\n");	// TRACE("fare(opq, o)\n");
				this->fare += FARE_INFO::Fare_table((FARE_INFO::tax == 5) ? "add5p" : "add", "ha", 
				                                    this->hokkaido_calc_km);
			}
		}				// JR九あり？
		if (0 < (this->kyusyu_sales_km + this->kyusyu_calc_km)) {
			/* JR西 + JR九 */
			/* 幹線のみ、幹線+地方交通線 */

			// JR九州側(q)<s><c> 加算 
			TRACE("fare(add, ka)\n");	// TRACE("fare(opq, q)\n");
			this->fare += FARE_INFO::Fare_table((FARE_INFO::tax == 5) ? "add5p" : "add", "ka", 
			                                    this->kyusyu_calc_km);
		}				// JR四あり?
		if (0 < (this->shikoku_sales_km + this->shikoku_calc_km)) {
			/* JR西 + JR四 */
			/* 幹線のみ、幹線+地方交通線 */

			// JR四国側(p)<s><c> 加算 
			TRACE("fare(add, sa)\n");	// TRACE("fare(opq, p)\n");
			this->fare += FARE_INFO::Fare_table((FARE_INFO::tax == 5) ? "add5p" : "add", "sa", 
			                                    this->shikoku_calc_km);
		}				// JR北
	} else if (0 < (this->hokkaido_sales_km + this->hokkaido_calc_km)) {
		/* JR北海道のみ */
		ASSERT(this->total_jr_sales_km == this->hokkaido_sales_km);
		ASSERT(this->total_jr_calc_km == this->hokkaido_calc_km);
		ASSERT(this->fare == 0);

		if (this->local_only_as_hokkaido) {
			/* JR北海道 地方交通線のみ */
			// (j)<s>
			TRACE("fare(hokkaido_sub)\n");
			this->fare = FARE_INFO::Fare_hokkaido_sub(this->total_jr_sales_km);
		} else {
			/* JR北海道 幹線のみ、幹線+地方交通線 */
			// (f)<c>	
			TRACE("fare(hokkaido-basic)\n");
			this->fare = FARE_INFO::Fare_hokkaido_basic(this->total_jr_calc_km);

		}				// JR九
	} else if (0 < (this->kyusyu_sales_km + this->kyusyu_calc_km)) {
		/* JR九州のみ */
		ASSERT(this->total_jr_sales_km == this->kyusyu_sales_km);
		ASSERT(this->total_jr_calc_km == this->kyusyu_calc_km);
		ASSERT(this->fare == 0);

		if (this->local_only) {
			/* JR九州 地方交通線 */
			TRACE("fare(ls)'k'\n");
			/* (l) */
			this->fare = FARE_INFO::Fare_table(this->total_jr_calc_km, this->total_jr_sales_km, 'k');

		}
		if (this->fare == 0) {
			/* JR九州 幹線のみ、幹線＋地方交通線、地方交通線のみ の(l), (n)非適用 */
			// (h)<s><c>
			TRACE("fare(kyusyu)\n");			// TRACE("fare(m, h)[9]\n");
			this->fare = FARE_INFO::Fare_kyusyu(this->total_jr_sales_km, 
			                                    this->total_jr_calc_km);
		}
		
	} else if (0 < (this->shikoku_sales_km + this->shikoku_calc_km)) {
		/* JR四国のみ */
		ASSERT(this->total_jr_sales_km == this->shikoku_sales_km);
		ASSERT(this->total_jr_calc_km == this->shikoku_calc_km);
		ASSERT(this->fare == 0);

		if (this->local_only) {
			/* JR四国 地方交通線 */
			TRACE("fare(ls)'s'\n");
			/* (k) */
			this->fare = FARE_INFO::Fare_table(this->total_jr_calc_km, this->total_jr_sales_km, 's');

		}
		if (this->fare == 0) {
			/* JR四国 幹線のみ、幹線＋地方交通線、地方交通線のみ の(l), (n)非適用 */
			// (g)<s><c>
			TRACE("fare(shikoku)[4]\n");		// TRACE("fare(m, g)[4]\n");
			this->fare = FARE_INFO::Fare_shikoku(this->total_jr_sales_km, 
			                                     this->total_jr_calc_km);
		}

	} else {
		ASSERT(FALSE);
		return false;
	}
	return true;
}

// 営業キロから有効日数を得る
//	calc_fare() => 
//
//	@param [in] km    営業キロ
//	@return 有効日数
//
int FARE_INFO::days_ticket(int sales_km)
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
//	@param [in] station_id1   駅1
//	@param [in] station_id2   駅2
//	@return 運賃額
//
int	FARE_INFO::Fare_company(int station_id1, int station_id2)
{
	char sql[256];
	static const char tsql[] = 
"select fare from %s"
" where "
" ((station_id1=?1 and station_id2=?2) or" 
"  (station_id1=?2 and station_id2=?1))";
	char* tbl[] = { "t_clinfar", "t_clinfar5p"};

	sqlite3_snprintf(sizeof(sql), sql, tsql, tbl[(FARE_INFO::tax == 5) ? 1 : 0]);
	DBO dbo(DBS::getInstance()->compileSql(sql));
	dbo.setParam(1, station_id1);
	dbo.setParam(2, station_id2);

	if (dbo.moveNext()) {
		return dbo.getInt(0);
	}
	return 0;
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
//	運賃テーブル参照
//	calc_fare() => retr_fare() =>
//
//	@param [in] c   'h'=hokkaido, 'e'= east or central or west
//	@param [in] km  
//	@retval 0 db error
//	@retval Number of negative ; -fare
//	@retval Positive of negative ; c_km
//
int	FARE_INFO::Fare_table(const char* tbl, char c, int km)
{
	char* sql;
	int ckm;
	int fare;

	sql = sqlite3_mprintf(
	"select ckm, %c%u from t_fare%s where km<=? order by km desc limit(1)",
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
//	運賃テーブル参照(ls)
//	calc_fare() => retr_fare() =>
//
//	@param [in] dkm   擬制キロ
//	@param [in] skm	  営業キロ
//	@param [in] c     's': 四国 / 'k': 九州
//	@return value
//
int FARE_INFO::Fare_table(int dkm, int skm, char c)
{
	int fare;

	char* sql = sqlite3_mprintf(
#if 1
	"select %c%u from t_farels where dkm=?2 and (skm=-1 or skm=?1)",
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
//	@return 駅1〜駅2に運賃と区別区間が含まれている場合その加算額を返す
//
int FARE_INFO::CheckSpecficFarePass(int line_id, int station_id1, int station_id2)
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
		int fare = dbo.getInt(2);
		TRACE(_T("CheckSpecificFarePass found: %s, %s, +%d\n"), Route::StationName(dbo.getInt(0)).c_str(), Route::StationName(dbo.getInt(1)).c_str(), fare);
		/* found, return values is add fare */
		return fare;
	}
	return 0;	/* not found */
}

//static 
//	特別運賃区間か判定し該当していたら運賃額を返す
//	calc_fare() =>
//
//	@param [in] station_id1 駅1(startStationId)
//	@param [in] station_id2 駅2(endStationId)
//
//	@return 特別区間運賃
//
int FARE_INFO::SpecficFareLine(int station_id1, int station_id2)
{
	char sql[256];
	
	static const char tsql[] = 
	"select fare%up from t_farespp where kind=1 and"
	" ((station_id1=?1 and station_id2=?2) or" 
	"  (station_id1=?2 and station_id2=?1))";

	sqlite3_snprintf(sizeof(sql), sql, tsql, FARE_INFO::tax);

	DBO dbo(DBS::getInstance()->compileSql(sql));
	dbo.setParam(1, station_id1);
	dbo.setParam(2, station_id2);
	if (dbo.moveNext()) {
		int fare = dbo.getInt(0);
		TRACE(_T("SpecficFareLine found: %s - %s, +%d\n"), Route::StationName(station_id1).c_str(), Route::StationName(station_id2).c_str(), fare);
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
int FARE_INFO::Fare_basic_f(int km)
{
	int fare;
	int c_km;

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
		} else {
			return 185;
		}
	}
	if (km < 101) {							// 7 to 10km
		if (FARE_INFO::tax == 5) {
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
	} else {								// 100�q越えは四捨五入
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
int FARE_INFO::Fare_sub_f(int km)
{
	int fare;
	int c_km;

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
		} else {
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
		return -c_km;		/* fare */
	}
	
	c_km *= 10;

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
	} else {				// 100�q越えは四捨五入
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
int FARE_INFO::Fare_tokyo_f(int km)
{
	int fare;
	int c_km;

	if (km < 31) {							// 1 to 3km
		if (FARE_INFO::tax == 5) {
			return 130;
		} else {
			return 133;
		}
	}
	if (km < 61) {							// 4 to 6km
		if (FARE_INFO::tax == 5) {
			return 150;
		} else {
			return 154;
		}
	}
	if (km < 101) {							// 7 to 10km
		if (FARE_INFO::tax == 5) {
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
	} else {				// 100�q越えは四捨五入
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
int FARE_INFO::Fare_osaka(int km)
{
	int fare;
	int c_km;

	if (km < 31) {							// 1 to 3km
		if (FARE_INFO::tax == 5) {
			return 120;
		} else {
			return 120;
		}
	}
	if (km < 61) {							// 4 to 6km
		if (FARE_INFO::tax == 5) {
			return 160;
		} else {
			return 160;
		}
	}
	if (km < 101) {							// 7 to 10km
		if (FARE_INFO::tax == 5) {
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
	
	if (3000 < c_km) {
		fare = 1530 * 3000 + 1215 * (c_km - 3000);
	} else {
		fare = 1530 * c_km;
	}
	if (c_km <= 1000) {						// 100km以下は切り上げ
		// 1の位を切り上げ
		fare = (fare + 9999) / 10000 * 10;
	} else {								// 100�q越えは四捨五入
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
int FARE_INFO::Fare_yamate_f(int km)
{
	int fare;
	int c_km;

	if (km < 31) {							// 1 to 3km
		if (FARE_INFO::tax == 5) {
			return 130;
		} else {
			return 133;
		} 
	}
	if (km < 61) {							// 4 to 6km
		if (FARE_INFO::tax == 5) {
			return 150;
		} else {
			return 154;
		}
	}
	if (km < 101) {							// 7 to 10km
		if (FARE_INFO::tax == 5) {
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
	} else {								// 100�q越えは四捨五入
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd_ic(fare, FARE_INFO::tax);	// tax = +5%, 四捨五入
}

//	e: 電車特定区間(大阪環状線)
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    営業キロ
//	@return 運賃額
//
int FARE_INFO::Fare_osakakan(int km)
{
	int fare;
	int c_km;

	if (km < 31) {							// 1 to 3km
		if (FARE_INFO::tax == 5) {
			return 120;
		} else {
			return 120;
		}
	}
	if (km < 61) {							// 4 to 6km
		if (FARE_INFO::tax == 5) {
			return 160;
		} else {
			return 160;
		}
	}
	if (km < 101) {							// 7 to 10km
		if (FARE_INFO::tax == 5) {
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
	} else {								// 100�q越えは四捨五入
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
int FARE_INFO::Fare_hokkaido_basic(int km)
{
	int fare;
	int c_km;
	char tbl[16];

	if (km < 31) {							// 1 to 3km
		if (FARE_INFO::tax == 5) {
			return 160;
		} else {
			return 170;
		}
	}
	if (km < 61) {							// 4 to 6km
		if (FARE_INFO::tax == 5) {
			return 200;
		} else {
			return 210;
		}
	}
	if (km < 101) {							// 7 to 10km
		if (FARE_INFO::tax == 5) {
			return 210;
		} else {
			return 220;
		}
	}

	sprintf_s(tbl, NumOf(tbl), "h%u", FARE_INFO::tax);
	fare = FARE_INFO::Fare_table("bspekm", tbl, km);
	if (0 != fare) {
		return fare;
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
	
	if (6000 < c_km) {
		fare = 1785 * 2000 + 1620 * (3000 - 2000) + 1285 * (6000 - 3000) + 705 * (c_km - 6000);
	} else if (3000 < c_km) {
		fare = 1785 * 2000 + 1620 * (3000 - 2000) + 1285 * (c_km - 3000);
	} else if (2000 < c_km) {
		fare = 1785 * 2000 + 1620 * (c_km - 2000);
	} else {
		fare = 1785 * c_km;
	}
	if (c_km <= 1000) {						// 100km以下は切り上げ
		// 1の位を切り上げ
		fare = (fare + 9999) / 10000 * 10;
	} else {								// 100�q越えは四捨五入
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd(fare, FARE_INFO::tax);	// tax = +5%, 四捨五入
}

//	JR北海道地方交通線
//	calc_fare() => retr_fare() =>
//
//	@param [in] km    営業キロ
//	@return 運賃額
//
int FARE_INFO::Fare_hokkaido_sub(int km)
{
	int fare;
	int c_km;

	if (km < 31) {							// 1 to 3km
		if (FARE_INFO::tax == 5) {
			return 160;
		} else {
			return 170;
		}
	}
	if (km < 61) {							// 4 to 6km
		if (FARE_INFO::tax == 5) {
			return 200;
		} else {
			return 210;
		}
	}
	if (km < 101) {							// 7 to 10km
		if (FARE_INFO::tax == 5) {
			return 220;
		} else {
			return 230;
		}
	}
	
	if (12000 < km) {		// 1200km越えは別表第2号イの4にない
		ASSERT(FALSE);
		return -1;
	}
	
	c_km = Fare_table("lspekm", 'h', km);
	if (c_km == 0) {
		ASSERT(FALSE);
		return -1;
	}
	if (c_km < 0) {
		return -c_km;		/* fare */
	}
	
	c_km *= 10;

	if (5460 <= c_km) {
		fare = 1960 * 1820 + 1780 * (2730 - 1820) + 1410 * (5460 - 2730) + 770 * (c_km - 5460);
	} else if (2730 < c_km) {
		fare = 1960 * 1820 + 1780 * (2730 - 1820) + 1410 * (c_km - 2730);
	} else if (1820 < c_km) {
		fare = 1960 * 1820 + 1780 * (c_km - 1820);
	} else { /* <= 182km */
		fare = 1960 * c_km;
	}
	if (c_km <= 1000) {						// 100km以下は切り上げ
		// 1の位を切り上げ
		fare = (fare + 9999) / 10000 * 10;
	} else {				// 100�q越えは四捨五入
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
int FARE_INFO::Fare_shikoku(int skm, int ckm)
{
	int fare;
	int c_km;

	/* JTB時刻表 C-3表 */
	if (ckm != skm) {
		if (FARE_INFO::tax == 5) {
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

	if (ckm < 31) {							// 1 to 3km
		if (FARE_INFO::tax == 5) {
			return 160;
		} else {
			return 160;
		}
	}
	if (ckm < 61) {							// 4 to 6km
		if (FARE_INFO::tax == 5) {
			return 200;
		} else {
			return 210;
		}
	}
	if (ckm < 101) {						// 7 to 10km
		if (FARE_INFO::tax == 5) {
			return 210;
		} else {
			return 220;
		}
	}

	fare = FARE_INFO::Fare_table("bspekm", (FARE_INFO::tax == 5) ? "s5" : "s8", ckm);
	if (0 != fare) {
		return fare;
	}

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
	
	if (c_km <= 1000) {						// 100km以下は切り上げ
		// 1の位を切り上げ
		fare = (fare + 9999) / 10000 * 10;
	} else {								// 100�q越えは四捨五入
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
int FARE_INFO::Fare_kyusyu(int skm, int ckm)
{
	int fare;
	int c_km;

	/* JTB時刻表 C-3表 */
	if (ckm != skm) {
		if (FARE_INFO::tax == 5) {
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

	if (ckm < 31) {							// 1 to 3km
		if (FARE_INFO::tax == 5) {
			return 160;
		} else {
			return 160;
		}
	}
	if (ckm < 61) {							// 4 to 6km
		if (FARE_INFO::tax == 5) {
			return 200;
		} else {
			return 220;
		}
	}
	if (ckm < 101) {						// 7 to 10km
		if (FARE_INFO::tax == 5) {
			return 220;
		} else {
			return 230;
		}
	}

	fare = FARE_INFO::Fare_table("bspekm", (FARE_INFO::tax == 5) ? "k5" : "k8", ckm);
	if (0 != fare) {
		return fare;
	}

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

	if (c_km <= 1000) {							// 100km以下は切り上げ
		// 1の位を切り上げ
		fare = (fare + 9999) / 10000 * 10;
	} else {									// 100�q越えは四捨五入
		fare = (fare + 50000) / 100000 * 100;
	}
	return taxadd(fare, FARE_INFO::tax);	// tax = +5%, 四捨五入
}


//static
bool FARE_INFO::IsIC_area(int urban_id)
{
	return ((urban_id == URB_TOKYO) ||
		(urban_id == URB_NIGATA) ||
		(urban_id == URB_SENDAI));
}
	
