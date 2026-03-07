#include "stdafx.h"
#include <alpdb.h>
#include <sstream>
#include <azusa.h>

#if 0

'Farert'
Copyright (C) 2025 Sutezo (sutezo666@gmail.com)

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

/*
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
*/

#endif

// int g_tax = 10; // Default 10% tax rate (can be changed at runtime)

static int get_prefect_id(std::string prefecture);
static int get_company_id(std::string company);


// open database
// return JSON string with DB info
std::string open_database()
{
    DBsys dbsys;

#if defined(__EMSCRIPTEN__)
    const char* dbpath = "/data/jrdbNewest.db"; // for WASM default path (case-sensitive in MEMFS)
#else
	char* dbpath = getenv("farertDB");

	if (!dbpath) {
		fprintf(stderr, "Should be set environment variable the 'farertDB' for use database.\n");
		return "{ \"result\": false, \"reson\": \"farertDB environment variable not set.\" }";
	}
#endif
    if (DBS::getInstance()->open(dbpath) && RouteUtil::DbVer(&dbsys)) {
        return "{ \"result\": true, \"dbName\": \"" + std::string(dbsys.name)
         + "\", \"createdate\": \"" + std::string(dbsys.createdate) + "\" }"  ;
    } else {
        return "{ \"result\": false, \"reson\": \"failued to open database.\" }";
    }
}

// close database
void close_database()
{
    DBS::getInstance()->close();
}

// DB Information
std::string database_info()
{
    DBsys dbsys;

    if (RouteUtil::DbVer(&dbsys)) {
        return "{ \"result\": true, \"dbName\": \"" + std::string(dbsys.name)
         + "\", \"createdate\": \"" + std::string(dbsys.createdate) + "\" }"  ;
    } else {
        return "{ \"result\": false, \"reson\": \"failued to open database.\" }";
    }
}

// 運賃計算結果出力 for Shared export
//
std::string az_route::show_fare()
{
    FARE_INFO fi;
    CalcRoute crt(*this);

    crt.calcFare(&fi);
    std::string result = fi.showFare(crt.getRouteFlag());
    result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());

    return result;
}

// 運賃計算結果出力 for JSON Object
// 
std::string az_route::get_fare_info_object_json() {
    FARE_INFO fi;
    std::ostringstream message;

    CalcRoute crt(*this);
    crt.calcFare(&fi);
    int rc = fi.resultCode();
    if ((rc != 0) && (rc != -1)) {
        return std::string("{ \"fareResultCode\": -2 }"); // -2: empty or -3: fail
    }
    const RouteFlag& refRouteFlag = crt.getRouteFlag();

    auto fields = {
        json_encoder::pair("fareResultCode",rc == 0 ? 0 : 1), /* 0: success, 1: KOKURA-pending */
        json_encoder::pair("isMeihanCityStartTerminalEnable", refRouteFlag.isMeihanCityEnable()),
        json_encoder::pair("isMeihanCityStart", refRouteFlag.isStartAsCity()),
        json_encoder::pair("isMeihanCityTerminal", refRouteFlag.isArriveAsCity()),
        json_encoder::pair("isRuleAppliedEnable", refRouteFlag.rule_en()),
        json_encoder::pair("isRuleApplied", !refRouteFlag.no_rule),
        json_encoder::pair("isJRCentralStockEnable", refRouteFlag.jrtokaistock_enable),
        json_encoder::pair("isJRCentralStock", refRouteFlag.jrtokaistock_applied),
        json_encoder::pair("isEnableLongRoute", refRouteFlag.isEnableLongRoute()),
        json_encoder::pair("isLongRoute", refRouteFlag.isLongRoute()),
        json_encoder::pair("isRule115specificTerm", refRouteFlag.isRule115specificTerm()),
        json_encoder::pair("isEnableRule115", refRouteFlag.isEnableRule115()),
        json_encoder::pair("isResultCompanyBeginEnd", fi.isBeginEndCompanyLine()),
        json_encoder::pair("isResultCompanyMultipassed", fi.isMultiCompanyLine()),
        json_encoder::pair("isEnableTokaiStockSelect", fi.isEnableTokaiStockSelect()),
        json_encoder::pair("beginStation", CalcRoute::BeginOrEndStationName(fi.getBeginTerminalId())),
        json_encoder::pair("endStation", CalcRoute::BeginOrEndStationName(fi.getEndTerminalId())),
        json_encoder::pair("isBeginInCity", FARE_INFO::IsCityId(fi.getBeginTerminalId())),
        json_encoder::pair("isEndInCity", FARE_INFO::IsCityId(fi.getEndTerminalId())),
        /* Rule114 */
        json_encoder::pair("isRule114Applied", fi.isRule114()),
        json_encoder::pair("rule114SalesKm", fi.getRule114SalesKm()),
        json_encoder::pair("rule114CalcKm",  fi.getRule114CalcKm()),
        json_encoder::pair("rule114ApplyTerminal", fi.getRule114apply_terminal_station()),
        [&]() -> std::string {
            std::ostringstream oss;

            oss << json_encoder::begin_array("stockDiscounts");
            for (int32_t i = 0; true; i++) {
                tstring title;
                tstring dummy;
                int32_t fareStock = fi.getFareStockDiscount(i, title);

                if (fareStock <= 0) {
                    break;
                }
                oss << "{";
                if (fi.isRule114()) {
                    oss << json_encoder::pair("rule114StockFare", 
                            fi.getFareStockDiscount(i, dummy, true));
                    oss << ",";
                }
                oss << json_encoder::pair("stockDiscountFare",
                        (fareStock + fi.getFareForCompanyline()));
                oss << ",";
                oss << json_encoder::pair("stockDiscountTitle", title);
                oss << "},";
            }
            std::string str = oss.str();
            if (str.back() == ',') {
                str.pop_back(); // remove last comma
            }
            return str + json_encoder::end_array();
        }(),
        json_encoder::pair("isSpecificFare", refRouteFlag.special_fare_enable),
        json_encoder::pair("totalSalesKm", fi.getTotalSalesKm()),
        json_encoder::pair("jrCalcKm", fi.getJRCalcKm()),
        json_encoder::pair("jrSalesKm", fi.getJRSalesKm()),
        json_encoder::pair("companySalesKm", fi.getCompanySalesKm()),
        json_encoder::pair("salesKmForHokkaido", fi.getSalesKmForHokkaido()),
        json_encoder::pair("calcKmForHokkaido", fi.getCalcKmForHokkaido()),
        json_encoder::pair("brtSalesKm", fi.getBRTSalesKm()),
        json_encoder::pair("salesKmForEast", fi.getSalesKmForEast()),
        json_encoder::pair("calcKmForEast", fi.getCalcKmForEast()),
        json_encoder::pair("salesKmForShikoku", fi.getSalesKmForShikoku()),
        json_encoder::pair("calcKmForShikoku", fi.getCalcKmForShikoku()),
        json_encoder::pair("salesKmForKyusyu", fi.getSalesKmForKyusyu()),
        json_encoder::pair("calcKmForKyusyu", fi.getCalcKmForKyusyu()),
        json_encoder::pair("isRoundtrip", refRouteFlag.isRoundTrip()),
        json_encoder::pair("isRoundtripDiscount", fi.isRoundTripDiscount()),
        json_encoder::pair("fareForCompanyline", fi.getFareForCompanyline()),
        json_encoder::pair("fare", fi.getFareForDisplay()),
        json_encoder::pair("farePriorRule114", fi.getFareForDisplayPriorRule114()),
        json_encoder::pair("fareForBRT", fi.getFareForBRT()),
        json_encoder::pair("isBRTdiscount", fi.getIsBRT_discount()),
        json_encoder::pair("roundTripFareWithCompanyLine", fi.roundTripFareWithCompanyLine().fare),
        [&]() -> std::string {
            if (fi.isRule114()) {
                return json_encoder::pair("roundTripFareWithCompanyLinePriorRule114", fi.roundTripFareWithCompanyLinePriorRule114())
                + ","
                + json_encoder::pair("fareForIC", fi.getFareForIC());
            }
            return json_encoder::pair("fareForIC", fi.getFareForIC());
        }(),
        [&]() -> std::string {
            std::ostringstream oss;
            int fare = fi.getAcademicDiscountFare();
            if (0 < fare) {
                oss << json_encoder::pair("academicFare", fare) + ","
                     + json_encoder::pair("isAcademicFare", true) + ","
                     + json_encoder::pair("roundtripAcademicFare", fi.roundTripAcademicFareWithCompanyLine());
            } else {
                oss << json_encoder::pair("academicFare", 0) + ","
                     + json_encoder::pair("isAcademicFare", false) + ","
                     + json_encoder::pair("roundtripAcademicFare", 0);
            }
            return oss.str();
        }(),
        json_encoder::pair("childFare", fi.getChildFareForDisplay()),
        json_encoder::pair("roundtripChildFare", fi.roundTripChildFareWithCompanyLine()),
        json_encoder::pair("ticketAvailDays", fi.getTicketAvailDays()),
        json_encoder::pair("routeList", fi.getRoute_string()),
        json_encoder::pair("routeListForTOICA", fi.getTOICACalcRoute_string()),

        // UI結果オプションメニュー
        json_encoder::pair("isFareOptEnabled", 
                                 refRouteFlag.rule_en()
                              || refRouteFlag.jrtokaistock_enable
                              || refRouteFlag.isEnableRule115()
                              || refRouteFlag.isEnableLongRoute()
                              || refRouteFlag.special_fare_enable),
        [&]() -> std::string {
            std::ostringstream oss;

            oss << json_encoder::begin_array("messages");
            
            if (refRouteFlag.no_rule &&
                    fi.isUrbanArea() && !refRouteFlag.isUseBullet()) {
                if (fi.getBeginTerminalId() == fi.getEndTerminalId()) {
                    // messages.add(msgCantMetroTicket)
                } else if (!refRouteFlag.isEnableRule115()
                        || !refRouteFlag.isRule115specificTerm()) {
                    if (refRouteFlag.isLongRoute()) {
                        oss << json_encoder::value("近郊区間内ですので最短経路の運賃で利用可能です");
                    } else {
                        oss << json_encoder::value("近郊区間内ですので最安運賃の経路で計算");
                    }
                }

                // 大回り指定では115適用はみない
                if (refRouteFlag.isEnableRule115() && !refRouteFlag.isEnableLongRoute()) {
                    if (refRouteFlag.isRule115specificTerm()) {
                        oss << json_encoder::value("「単駅最安」で単駅発着が選択可能です");
                    } else {
                        oss << json_encoder::value("「特定都区市内発着」で特定都区市内発着が選択可能です");
                    }
                }
            }

            // 私鉄競合特例運賃(大都市近郊区間)
            if (refRouteFlag.special_fare_enable) {
                oss << json_encoder::value("特定区間割引運賃適用");
            }

            if (fi.isBeginEndCompanyLine()) {
                oss << json_encoder::value("会社線発着のため一枚の乗車券として発行されない場合があります.");
            }
            if (fi.isMultiCompanyLine()) {
                /* 2017.3 以降 ここに来ることはない */
                oss << json_encoder::value("複数の会社線を跨っているため乗車券は通し発券できません. 運賃額も異なります.");
            }
            if (fi.isEnableTokaiStockSelect()) {
                oss << json_encoder::value("JR東海株主優待券使用オプション選択可");
            }
            if (fi.getIsBRT_discount()) {
                oss << json_encoder::value("BRT乗り継ぎ割引適用");
            }

            if (refRouteFlag.no_rule && refRouteFlag.special_fare_enable) {
                oss << json_encoder::value("特定区間割引運賃を適用していません");
            }
            if (refRouteFlag.no_rule && refRouteFlag.isAvailableRule86()) {
                oss << json_encoder::value("旅客営業規則第86条を適用していません");
            }
            if (refRouteFlag.no_rule && refRouteFlag.isAvailableRule87()) {
                oss << json_encoder::value("旅客営業規則第87条を適用していません");
            }
            if (refRouteFlag.no_rule && refRouteFlag.isAvailableRule88()) {
                oss << json_encoder::value("旅客営業規則第88条を適用していません");
            }
            if (refRouteFlag.no_rule && refRouteFlag.isAvailableRule160_4()) {
                oss << json_encoder::value("旅客営業規則第160条第4項を適用していません");
            }
            if (refRouteFlag.no_rule && refRouteFlag.isAvailableRule69()) {
                oss << json_encoder::value("旅客営業規則第69条を適用していません");
            }
            if (refRouteFlag.no_rule && refRouteFlag.isAvailableRule70()) {
                oss << json_encoder::value("旅客営業規則第70条を適用していません");
            }
            if (refRouteFlag.no_rule && refRouteFlag.isAvailableRule115()) {
                oss << json_encoder::value("旅客営業取扱基準規程第115条を適用していません");
            }
            if (refRouteFlag.isAvailableRule16_5()) {
                oss << json_encoder::value("この乗車券はJRで発券されません. 東京メトロでのみ発券されます");
            }
            if (fi.isRule114()) {
                oss << json_encoder::value("旅客営業取扱基準規程第114条適用営業キロ計算駅:" + fi.getRule114apply_terminal_station());
            }
            if (refRouteFlag.compnterm) {
                oss << json_encoder::value("この経路の会社線通過連絡は許可されていません.");
            }
            oss << json_encoder::end_array();
            return oss.str();
        }(),
    };

    std::ostringstream oss;
    oss << "{";
    int i = 0;
    for (auto& field : fields) {
        if (0 < i) {
            oss << ",";
        }
        oss << field;
        i++;
    }
    oss << "}";
    return oss.str();
}

// add start station
int az_route::add_start_route(std::string station)
{
    int32_t station_id = RouteUtil::GetStationId(station.c_str());

    if (0 < station_id) {
        return add(station_id);
    } else {
        return -200;
    }
}

// add end station
int az_route::add_route(std::string line, std::string station)
{
    int line_id = RouteUtil::GetLineId(line.c_str());
    int station_id = RouteUtil::GetStationId(station.c_str());
    if (line_id <= 0) {
        return -300;
    }
    if (station_id <= 0) {
        return -200;
    }
    return add(line_id, station_id);
}

// auto route from current to destinationStation
int az_route::auto_route(int useBulletTrain, std::string destinationStation)
{
    int station_id = RouteUtil::GetStationId(destinationStation.c_str());

    if (station_id <= 0) {
        return -200;
    }
    return changeNeerest(useBulletTrain, station_id);
}

// get routes as JSON array
std::string az_route::get_routes_json()
{
    std::ostringstream oss;
    int num = 0;
    oss << json_encoder::begin_array();

    for (const RouteItem& item : routeList()) {
        if (0 < num++) {
            oss << ",";
        }
        oss << "{ "
            << json_encoder::pair("line", RouteUtil::LineName(item.lineId))
            << "," 
            << json_encoder::pair("station", RouteUtil::StationNameEx(item.stationId))
            << " }";
    }
    oss << json_encoder::end_array();
    return "";
}

// get route record at index
std::string az_route::get_route_record(int index)
{
    return "{ " +
        json_encoder::pair("line", RouteUtil::LineName(routeList().at(index).lineId))
        + "," + 
        json_encoder::pair("station", RouteUtil::StationNameEx(routeList().at(index).stationId))
        + " }";
}

/* build route from string
    for Import route
 { 
   "failItem": "金山",
   "offset": 4,
   "rc": -2
 }
*/
std::string az_route::build_route(const std::string& route_str)
{
    std::ostringstream oss;
    char error_buf[256] = {0};
    int offset = 0;

    int rc = setup_route(route_str.c_str(), error_buf, sizeof(error_buf), &offset);
    oss << "{" << json_encoder::pair("rc", rc) << ",";
    oss << json_encoder::pair("failItem", std::string(error_buf)) << ",";
    oss << json_encoder::pair("offset", offset);
    oss << "}";

    return oss.str();
}

//  Output Routes
//  for Export route
std::string az_route::route_script()
{
    return RouteList::route_script(); 
}

///////////////////////

    // 都道府県の列挙
std::string fare_ui::get_prefects()
{
    std::vector<std::string> prefects;

    DBO dbo = RouteUtil::Enum_company_prefect();
    int ident;
    int num = 0;

    if (dbo.isvalid()) {
        std::ostringstream oss;

        while (dbo.moveNext()) {
            ident = dbo.getInt(1);
            if (0x10000 <= ident) {
                prefects.push_back(dbo.getText(0));
            }
        }
        oss << "{" << json_encoder::begin_array("prefectures");
        for (std::string& pref : prefects) {
            if (0 < num++) {
                oss << ",";
            }
            if (!pref.empty()) {
                oss << "\"" << pref << "\"";
            }
        }
        oss << json_encoder::end_array() << "}";
        return oss.str();
    }
    return "{}";
}

// JRグループの会社一覧を配列で返す
std::string fare_ui::get_companys()
{
    std::vector<std::string> companies;

    DBO dbo = RouteUtil::Enum_company_prefect();
    int ident;
    int num = 0;

	if (dbo.isvalid()) {
        std::ostringstream oss;

		while (dbo.moveNext()) {
            ident = dbo.getInt(1);
            if (ident < 0x10000) {
                companies.push_back(dbo.getText(0));
            }
		}
        oss << "{" << json_encoder::begin_array("companies");
        for (std::string& company : companies) {
            if (0 < num++) {
                oss << ",";
            }
            if (!company.empty()) {
                oss << "\"" << company << "\"";
            }
        }
        oss << json_encoder::end_array() << "}";
        return oss.str();
	}
    return "{}";
}

// line-selection-screen

// 都道府県IDを返す
static int get_prefect_id(std::string prefecture)
{
    static const char tsql[] = "select rowid from t_prefect where name=?";
    DBO dbo = DBS::getInstance()->compileSql(tsql);
    dbo.setParam(1, prefecture.c_str());

    if (dbo.moveNext()) {
        return dbo.getInt(0) * 0x10000;
    } else {
        return -1;
    }
}

// JRグループの会社IDを返す
static int get_company_id(std::string company)
{
    static const char tsql[] = "select rowid from t_company where name=?";
    DBO dbo = DBS::getInstance()->compileSql(tsql);
    dbo.setParam(1, company.c_str());

    if (dbo.moveNext()) {
        return dbo.getInt(0);
    } else {
        return -1;
    }
}

// 都道府県の路線一覧を配列で返す
std::string fare_ui::get_lines_by_prefect(std::string prefecture)
{
    std::vector<std::string> lines;
    int num = 0;

    DBO dbo = RouteUtil::Enum_lines_from_company_prefect(get_prefect_id(prefecture.c_str()));

    if (dbo.isvalid()) {
        std::ostringstream oss;
        while (dbo.moveNext()) {
            lines.push_back(dbo.getText(0));
        }
        oss << "{" << json_encoder::begin_array("lines");
        for (std::string& line : lines) {
            if (0 < num++) {
                oss << ",";
            }
            if (!line.empty()) {
                oss << "\"" << line << "\"";
            }
        }
        oss << json_encoder::end_array() << "}";
        return oss.str();
    }
    return "";
}

// JRグループの路線一覧を配列で返す
std::string fare_ui::get_lines_by_company(std::string jrgroup)
{
    std::vector<std::string> lines;
    int num = 0;

    DBO dbo = RouteUtil::Enum_lines_from_company_prefect(get_company_id(jrgroup.c_str()));

    if (dbo.isvalid()) {
        std::ostringstream oss;
        while (dbo.moveNext()) {
            lines.push_back(dbo.getText(0));
        }
        oss << "{" << json_encoder::begin_array("lines");
        for (std::string& line : lines) {
            if (0 < num++) {
                oss << ",";
            }
            if (!line.empty()) {
                oss << "\"" << line << "\"";
            }
        }
        oss << json_encoder::end_array() << "}";
        return oss.str();
    }
    return "";
}

// 駅の所属路線を返す
std::string fare_ui::get_lines_by_station(std::string station)
{
    std::vector<std::string> lines;
    int num = 0;

    DBO dbo = RouteUtil::Enum_line_of_stationId(RouteUtil::GetStationId(station.c_str()));

    if (dbo.isvalid()) {
        std::ostringstream oss;
        while (dbo.moveNext()) {
            lines.push_back(dbo.getText(0));
        }
        oss << "{" << json_encoder::begin_array("lines");
        for (std::string& line : lines) {
            if (0 < num++) {
                oss << ",";
            }
            if (!line.empty()) {
                oss << "\"" << line << "\"";
            }
        }
        oss << json_encoder::end_array() << "}";
        return oss.str();
    }
    return "";
}


// 路線の会社管轄内駅一覧を返す
std::string fare_ui::get_stations_by_company_and_line(std::string jrgroup, std::string line_name)
{
    std::vector<std::string> stations;
    int num = 0;

    DBO dbo = RouteUtil::Enum_station_located_in_prefect_or_company_and_line(
                get_company_id(jrgroup.c_str()), 
                               RouteUtil::GetLineId(line_name.c_str()));
    if (dbo.isvalid()) {
        std::ostringstream oss;
        while (dbo.moveNext()) {
            stations.push_back(dbo.getText(0));
        }
        oss << "{" << json_encoder::begin_array("stations");
        for (std::string& line : stations) {
            if (0 < num++) {
                oss << ",";
            }
            if (!line.empty()) {
                oss << "\"" << line << "\"";
            }
        }
        oss << json_encoder::end_array() << "}";
        return oss.str();
    }
    return "";
}


// 路線の都道府県内の駅一覧を返す
std::string fare_ui::get_stations_by_prefecture_and_line(std::string prefecture, std::string line_name)
{
    std::vector<std::string> stations;
    int num = 0;

    DBO dbo = RouteUtil::Enum_station_located_in_prefect_or_company_and_line(
                get_prefect_id(prefecture.c_str()), 
                               RouteUtil::GetLineId(line_name.c_str()));
    if (dbo.isvalid()) {
        std::ostringstream oss;
        while (dbo.moveNext()) {
            stations.push_back(dbo.getText(0));
        }
        oss << "{" << json_encoder::begin_array("stations");
        for (std::string& line : stations) {
            if (0 < num++) {
                oss << ",";
            }
            if (!line.empty()) {
                oss << "\"" << line << "\"";
            }
        }
        oss << json_encoder::end_array() << "}";
        return oss.str();
    }
    return "";
}

// 駅のある都道府県
std::string fare_ui::get_prefecture_by_station(const std::string& station_name)
{
    return RouteUtil::GetPrefectByStationId(RouteUtil::GetStationId(station_name.c_str()));
}

// 検索
std::string fare_ui::search_station_by_keyword(std::string key)
{
    std::vector<std::string> stations;
    int num = 0;

    DBO dbo = RouteUtil::Enum_station_match(key.c_str());
    if (dbo.isvalid()) {
        std::ostringstream oss;
        while (dbo.moveNext()) {
            stations.push_back(dbo.getText(0));
        }
        oss << "{" << json_encoder::begin_array("stations");
        for (std::string& line : stations) {
            if (0 < num++) {
                oss << ",";
            }
            if (!line.empty()) {
                oss << "\"" << line << "\"";
            }
        }
        oss << json_encoder::end_array() << "}";
        return oss.str();
    }
    return "";
}

// 指定した駅も含め路線の分岐駅一覧を返す)
std::string fare_ui::get_branch_stations_by_line(std::string line_name, std::string station_name)
{
    std::vector<std::string> stations;
    int num = 0;

    DBO dbo = RouteUtil::Enum_junction_of_lineId(RouteUtil::GetLineId(line_name.c_str()),
                                                 RouteUtil::GetStationId(station_name.c_str()));
    if (dbo.isvalid()) {
        std::ostringstream oss;
        while (dbo.moveNext()) {
            stations.push_back(dbo.getText(0));
        }
        oss << "{" << json_encoder::begin_array("stations");
        for (std::string& line : stations) {
            if (0 < num++) {
                oss << ",";
            }
            if (!line.empty()) {
                oss << "\"" << line << "\"";
            }
        }
        oss << json_encoder::end_array() << "}";
        return oss.str();
    }
    return "";
}

// 路線の全駅一覧を返す
std::string fare_ui::get_stations_by_line(std::string line_name)
{
    std::vector<std::string> stations;
    int num = 0;

    DBO dbo = RouteUtil::Enum_station_of_lineId(RouteUtil::GetLineId(line_name.c_str()));
    if (dbo.isvalid()) {
        std::ostringstream oss;
        while (dbo.moveNext()) {
            stations.push_back(dbo.getText(0));
        }
        oss << "{" << json_encoder::begin_array("stations");
        for (std::string& line : stations) {
            if (0 < num++) {
                oss << ",";
            }
            if (!line.empty()) {
                oss << "\"" << line << "\"";
            }
        }
        oss << json_encoder::end_array() << "}";
        return oss.str();
    }
    return "";
}

// 駅名のかなを得る
std::string fare_ui::get_kana_by_station(const std::string& station_name)
{
    return RouteUtil::GetKanaFromStationId(RouteUtil::GetStationId(station_name.c_str()));
}

// Developer tools implementation
std::string dev::execute_sql(const std::string& sql)
{
    DBO dbo = DBS::getInstance()->compileSql(sql.c_str(), false); // no cache for dev queries

    if (!dbo.isvalid()) {
        std::ostringstream oss;
        oss << "{\"error\":\"" << DBS::getInstance()->errmsg() << "\"}";
        return oss.str();
    }

    std::ostringstream oss;
    std::vector<std::string> rows;

    // Get column count
    int colCount = dbo.getNumOfCol();

    // Build JSON array of results
    oss << "{\"columns\":[";
    // Note: SQLite doesn't provide column names easily from sqlite3_stmt
    // We'll use column indices instead
    for (int i = 0; i < colCount; i++) {
        if (i > 0) oss << ",";
        oss << "\"col" << i << "\"";
    }
    oss << "],\"rows\":[";

    int rowNum = 0;
    while (dbo.moveNext()) {
        if (rowNum > 0) oss << ",";
        oss << "[";
        for (int i = 0; i < colCount; i++) {
            if (i > 0) oss << ",";

            int colType = dbo.colType(i);
            if (colType == SQLITE_INTEGER) {
                oss << dbo.getInt(i);
            } else if (colType == SQLITE_NULL) {
                oss << "null";
            } else {
                // TEXT or BLOB - treat as string
                std::string text = dbo.getText(i);
                // Escape quotes in JSON
                oss << "\"";
                for (char c : text) {
                    if (c == '"') oss << "\\\"";
                    else if (c == '\\') oss << "\\\\";
                    else if (c == '\n') oss << "\\n";
                    else if (c == '\r') oss << "\\r";
                    else if (c == '\t') oss << "\\t";
                    else oss << c;
                }
                oss << "\"";
            }
        }
        oss << "]";
        rowNum++;
    }

    oss << "],\"rowCount\":" << rowNum << "}";
    return oss.str();
}

