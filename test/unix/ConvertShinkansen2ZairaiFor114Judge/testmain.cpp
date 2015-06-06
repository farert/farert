#include "stdafx.h"
FILE *out = stderr;

static void result_text(const TCHAR* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	static bool b = false;
	if (!b) {
		fopen_s(&out, "test_exp_result.txt", "w");
		b = true;
	}
	_vftprintf(out, fmt, ap);
}

#define TRACE result_text

int g_tax = 8;
extern Route route;

static const TCHAR *test_tbl[] = {
	_T("上野 東北新幹線 盛岡"),								// a			@東北線, 
	_T("福島(北) 東北新幹線 郡山(北)"),						// b			@東北線, 
	_T("大宮 上越新幹線 熊谷"),								// c			@高崎線, 
	_T("大宮 上越新幹線 本庄早稲田"),                       // d			@高崎線, 
	_T("大宮 上越新幹線 高崎"),                             // e			@高崎線, 高崎, 
	_T("大宮 上越新幹線 上毛高原"),                         // f			@高崎線, 高崎, 
	_T("大宮 上越新幹線 浦佐"),                             // g 			@高崎線, 高崎, 上越線, 
	_T("大宮 上越新幹線 長岡"),                             // h			@高崎線, 高崎, 上越線, 宮内, 信越線(直江津-新潟), 
	_T("大宮 上越新幹線 燕三条"),                           // i			@高崎線, 高崎, 上越線, 宮内, 信越線(直江津-新潟), 
	_T("大宮 上越新幹線 新潟"),                             // j			@高崎線, 高崎, 上越線, 宮内, 信越線(直江津-新潟), 
	_T("高崎 上越新幹線 新潟"),                             // k			/@上越線, 宮内, 信越線(直江津-新潟), 
	_T("高崎 上越新幹線 本庄早稲田"),                       // l			/@
	_T("高崎 上越新幹線 上毛高原"),                         // m			/@
	_T("高崎 上越新幹線 越後湯沢"),                         // n			/@上越線, 
	_T("高崎 上越新幹線 熊谷"),                             // o			/@高崎線, 
	_T("浦佐 上越新幹線 大宮"),                             // p			@上越線, 高崎, 高崎線, 
	_T("長岡 上越新幹線 高崎"),                             // q			@信越線(直江津-新潟), 宮内, 上越線, 高崎, 
	_T("上毛高原 上越新幹線 高崎"),                         // r o			/@
	_T("熊谷 上越新幹線 高崎"),                             // s 			@高崎線, 高崎, 
	_T("上毛高原 上越新幹線 本庄早稲田"),                   // t o			/@
	_T("本庄早稲田 上越新幹線 大宮"),                       // u		x	@高崎線, 
	_T("本庄早稲田 上越新幹線 高崎"),                       // v o			/@
	_T("本庄早稲田 上越新幹線 上毛高原"),                   // w o			/@
	_T("本庄早稲田 上越新幹線 越後湯沢"),                   // x o			/@上越線, 
	_T("上毛高原 上越新幹線 大宮"),                         // y o			/@高崎線, 
	_T("燕三条 上越新幹線 大宮"),                           // z 		x	@信越線(直江津-新潟), 宮内, 上越線, 高崎, 高崎線, 
	_T("長岡 上越新幹線 大宮"),                             // aa			@信越線(直江津-新潟), 宮内, 上越線, 高崎, 高崎線, 
	_T("名古屋 東海道新幹線 新大阪 山陽新幹線 博多"),       // ab			@東海道線, 
															//				@東海道線, 神戸, 山陽線, 門司, 鹿児島線, 
	_T("博多 山陽新幹線 新大阪 東海道新幹線 名古屋"),       // ac		x	@鹿児島線, 門司, 山陽線, 神戸, 東海道線, 
															//				@鹿児島線, 門司, 山陽線, 神戸, 東海道線, 
	_T("土合 上越線 高崎 北陸新幹線 長野"),                 // ad			-
	_T("長野 北陸新幹線 金沢"),                             // ae			-
	_T("長岡 上越新幹線 上毛高原"),                         // af			@信越線(直江津-新潟), 宮内, 上越線, 
	_T("本庄早稲田 上越新幹線 新潟"),   	                // ag			@上越線, 宮内, 信越線(直江津-新潟), 
	_T("白石蔵王 東北新幹線 盛岡"),							// ah		x	@東北線, 
	_T("白石蔵王 東北新幹線 水沢江刺"),						// ai		x	@東北線, 
	_T("福島(北) 東北新幹線 水沢江刺"),						// aj			@東北線, 
	_T("くりこま高原 東北新幹線 二戸"),						// aj			@東北線, 
	_T("二戸 東北新幹線 くりこま高原"),						// aj			@東北線, 
	_T("新青森 東北新幹線 東京"),						// aj			@東北線, 
	_T("東京 東北新幹線 新青森"),						// aj			@東北線, 
	_T("新花巻 東北新幹線 いわて沼宮内"),						// aj			@東北線, 
	_T("いわて沼宮内 東北新幹線 新花巻"),						// aj			@東北線, 
	_T("水沢江刺 東北新幹線 くりこま高原"),						// aj			@東北線, 
	_T(""),
};

//高久 東北線 東京 東海道新幹線 新横浜 横浜線 東神奈川  東海道線 横浜 根岸線 本郷台 
static bool ConvertShinkansen2ZairaiFor114Judge(vector<RouteItem>* route)
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
	bool replaced = false;

	while (ite != route->end()) {
		station_id1n = ite->stationId;
		if ((station_id1 != 0) && IS_SHINKANSEN_LINE(ite->lineId)) {
			zline = Route::EnumHZLine(ite->lineId, station_id1, station_id1n);
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
TRACE(_T("++%15s(%d)\t%s(%d)\n"), Route::LineName(zline_id).c_str(), zline_id, Route::StationName(station_id2).c_str(), station_id2);
} else if (i == (zline.size() - 1)) {
TRACE(_T("--%15s(%d)\t%s(%d)\n"), Route::LineName(zline_id).c_str(), zline_id, Route::StationName(station_id2).c_str(), station_id2);
} else {
TRACE(_T("::%15s(%d)\t%s(%d)\n"), Route::LineName(zline_id).c_str(), zline_id, Route::StationName(station_id2).c_str(), station_id2);
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
							z_station_id = Route::NextShinkansenTransferTerm(bline_id, station_id1, station_id1n);
							ASSERT(0 < z_station_id);
							ite->stationId = z_station_id;
							++ite;
							ite = route->insert(ite, RouteItem(zroute[0], station_id1n));
						} else {
							ite->lineId = zroute[0];
						}

						ite->stationId = zroute[1];
						for (i = 2; i < zroute.size() - 1; i += 2) {
							ite++;
							ite = route->insert(ite, RouteItem(zroute[i], zroute[i + 1]));
						}
						if (i < zroute.size()) {
							ite++;
							ite = route->insert(ite, RouteItem(zroute[i], station_id1n));
						}
						if (0xffffffff == zline.back()) {
							ite->stationId = Route::NextShinkansenTransferTerm(bline_id, station_id1n, station_id1);
							if (z_station_id == ite->stationId) {
								ite = route->erase(ite - 1);
								*ite = RouteItem(bline_id, station_id1n);
							} else {
								ite++;
								ite = route->insert(ite, RouteItem(bline_id, station_id1n));
							}
						} else {
							ite->stationId = station_id1n;
						}
					} else {
						ASSERT(FALSE);
					}
				} else {
					if (station_id1 == j_station_id) {
						// 高崎発
						if (0 < zroute.size()) {
							// k
							ite->lineId = zroute[0];
							if (1 < zroute.size()) {
								ite->stationId = zroute[1];
								for (i = 2; i < zroute.size() - 1; i += 2) {
									ite++;
									ite = route->insert(ite, RouteItem(zroute[i], zroute[i + 1]));
								}
								if (i < zroute.size()) {
									ite++;
									ite = route->insert(ite, RouteItem(zroute[i], station_id1n));
								}
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
							for (i = 0; i < zroute.size() - 1; i += 2) {
								ite++;
								ite = route->insert(ite, RouteItem(zroute[i], zroute[i + 1]));
							}
							if (i < zroute.size()) {
								ite++;
								ite = route->insert(ite, RouteItem(zroute[i], station_id1n));
							}
						} // else // r, t, v, w
					}
				}
#if 0
TRACE(_T("\n;;%15s(%d)\t%s(%d)\n"), Route::LineName(route->back().lineId).c_str(), 
                                    route->back().lineId, 
                                    Route::StationName(route->back().stationId).c_str(), 
                                    route->back().stationId);
 TRACE(_T("@"));
 for (i = 0; i < zroute.size(); i++) {
 TRACE(_T("%s, "), ((i % 2) == 0) ? Route::LineName(zroute[i]).c_str() : Route::StationName(zroute[i]).c_str());
 }
 TRACE(_T("\n"));
 if (j_station_id != 0) { TRACE(_T("/%s/\n"), Route::StationName(j_station_id).c_str()); }
#endif
			} else {
				// 整備新幹線
				// DO NOTHING
			}
//TRACE(_T("---------------------------------\n"));
		}
		station_id1 = station_id1n;
		ite++;
	}
	return (0 < zline_id) && (zline_id < 32767);
}

static tstring cr_remove(tstring s)
{
	tstring::size_type idx = 0;
	while (tstring::npos != (idx = s.find(_T('\r'), idx))) {
		s.replace(idx, 1, _T(" "), 1, 1);
	}
	return s;
}

static void xxx(vector<RouteItem>& route)
{
	vector<RouteItem>::const_iterator ite = route.cbegin();
	tstring s;
	while (ite != route.end()) {
		TRACE(_T("<%s> %s "), Route::LineName(ite->lineId).c_str(), Route::StationName(ite->stationId).c_str());
		ite++;
	}
	TRACE(_T("\n"));
}

/////////////////////////////////////////////////////////////////
void test(void)
{
	int32_t rc;
	int32_t i;
	tstring s;
	
	for (i = 0; '\0' != *test_tbl[i]; i++) {
		rc = route.setup_route(test_tbl[i]);
		if ((rc != 0) && (rc != 1)) {
			TRACE(_T("Error!!!!(%d) test_tbl[%d](%s)\n"), rc, i, test_tbl[i]);
			return;
		}
		TRACE(_T("/////////////////////////////////////////////////////////////////////////\n"));
		TRACE(_T("%s\n"), test_tbl[i]);
		s = route.showFare();
		s = cr_remove(s);
		TRACE(_T("%s\n"), s.c_str());

		vector<RouteItem>& r = route.routeList();
		ConvertShinkansen2ZairaiFor114Judge(&r);
//		s = Route::Show_route(r, 0);
//		s = cr_remove(s);
//		TRACE(_T("\n%s-------------\n"), s.c_str());
		s = route.showFare();
		s = cr_remove(s);
//		xxx(route.routeList());
		TRACE(_T("---------------\n%s\n"), s.c_str());
	}
}

////////////////////////////////////////////////////////////////


#ifdef _WINDOWS
int _tmain(int argc, TCHAR** argv)
#else
int main(int argc, char** argv)
#endif
{
#if defined _WINDOWS
	_tsetlocale(LC_ALL, _T(""));	// tstring
#endif
	if (! DBS::getInstance()->open(_T("../../db/jrdb2015.db"))) {
		printf("Can't db open\n");
		return -1;
	}
	printf("----------");
	test();
	return 0;
}

