#include "stdafx.h"
#include <alpdb.h>
#include <sstream>
#include <azusa.h>

// int g_tax = 10; // Default 10% tax rate (can be changed at runtime)

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


// fare info object to JSON
std::string az_route::get_fare_info_object_json() {
    FARE_INFO fi;
    std::ostringstream message;

    CalcRoute crt(*this);
    crt.calcFare(&fi);
    int rc = fi.resultCode();
    if ((rc != 0) && (rc != -1)) {
        return std::string("{ \"fareResultCode\": -2 }"); // -2: empty or -3: fail
    }
    auto fields = {
        json_encoder::pair("fareResultCode",rc == 0 ? 0 : 1), /* 0: success, 1: KOKURA-pending */
        json_encoder::pair("isMeihanCityStartTerminalEnable", route_flag.isMeihanCityEnable()),
        json_encoder::pair("isMeihanCityStart", route_flag.isStartAsCity()),
        json_encoder::pair("isMeihanCityTerminal", route_flag.isArriveAsCity()),
        json_encoder::pair("isRuleAppliedEnable", route_flag.rule_en()),
        json_encoder::pair("isRuleApplied", !route_flag.no_rule),
        json_encoder::pair("isJRCentralStockEnable", route_flag.jrtokaistock_enable),
        json_encoder::pair("isJRCentralStock", route_flag.jrtokaistock_applied),
        json_encoder::pair("isEnableLongRoute", route_flag.isEnableLongRoute()),
        json_encoder::pair("isLongRoute", route_flag.isLongRoute()),
        json_encoder::pair("isRule115specificTerm", route_flag.isRule115specificTerm()),
        json_encoder::pair("isEnableRule115", route_flag.isEnableRule115()),
        json_encoder::pair("isResultCompanyBeginEnd", fi.isBeginEndCompanyLine()),
        json_encoder::pair("isResultCompanyMultipassed", fi.isMultiCompanyLine()),
        json_encoder::pair("isEnableTokaiStockSelect", fi.isEnableTokaiStockSelect()),
        json_encoder::pair("beginStationId", fi.getBeginTerminalId()),
        json_encoder::pair("endStationId", fi.getEndTerminalId()),
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
                if (fi.isRule114()) {
                    oss << json_encoder::pair("rule114StockFare", 
                            fi.getFareStockDiscount(i, dummy, true)
                        + fi.getFareForCompanyline());
                }
                oss << json_encoder::pair("stockDiscountFare",
                        (fareStock + fi.getFareForCompanyline()));
                oss << json_encoder::pair("stockDiscountTitle", title);
                oss << ",";
            }
            std::string str = oss.str();
            str.pop_back(); // remove last comma
            return str + json_encoder::end_array();
        }(),
        json_encoder::pair("isSpecificFare", route_flag.special_fare_enable),
        json_encoder::pair("totalSalesKm", fi.getTotalSalesKm()),
        json_encoder::pair("jrCalcKm", fi.getJRCalcKm()),
        json_encoder::pair("jrSalesKm", fi.getJRSalesKm()),
        json_encoder::pair("companySalesKm", fi.getCompanySalesKm()),
        json_encoder::pair("salesKmForHokkaido", fi.getSalesKmForHokkaido()),
        json_encoder::pair("calcKmForHokkaido", fi.getCalcKmForHokkaido()),
        json_encoder::pair("brtSalesKm", fi.getBRTSalesKm()),
        json_encoder::pair("salesKmForShikoku", fi.getSalesKmForShikoku()),
        json_encoder::pair("calcKmForShikoku", fi.getCalcKmForShikoku()),
        json_encoder::pair("salesKmForKyusyu", fi.getSalesKmForKyusyu()),
        json_encoder::pair("calcKmForKyusyu", fi.getCalcKmForKyusyu()),
        json_encoder::pair("isRoundtrip", route_flag.isRoundTrip()),
        json_encoder::pair("isRoundtripDiscount", fi.isRoundTripDiscount()),
        json_encoder::pair("fareForCompanyline", fi.getFareForCompanyline()),
        json_encoder::pair("fare", fi.getFareForDisplay()),
        json_encoder::pair("farePriorRule114", fi.getFareForDisplayPriorRule114()),
        json_encoder::pair("fareForBRT", fi.getFareForBRT()),
        json_encoder::pair("isBRTdiscount", fi.getIsBRT_discount()),
        json_encoder::pair("roundTripFareWithCompanyLine", fi.roundTripFareWithCompanyLine().fare),
        fi.isRule114() ? json_encoder::pair("roundTripFareWithCompanyLinePriorRule114", fi.roundTripFareWithCompanyLinePriorRule114()) : "",
        json_encoder::pair("fareForIC", fi.getFareForIC()),
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
                                 route_flag.rule_en()
                              || route_flag.jrtokaistock_enable
                              || route_flag.isEnableRule115()
                              || route_flag.isEnableLongRoute()
                              || route_flag.special_fare_enable),
        [&]() -> std::string {
            std::ostringstream oss;

            oss << json_encoder::begin_array("messages");
            
            if (route_flag.no_rule &&
                    fi.isUrbanArea() && !route_flag.isUseBullet()) {
                if (fi.getBeginTerminalId() == fi.getEndTerminalId()) {
                    // messages.add(msgCantMetroTicket)
                } else if (!route_flag.isEnableRule115()
                        || !route_flag.isRule115specificTerm()) {
                    if (route_flag.isLongRoute()) {
                        oss << "近郊区間内ですので最短経路の運賃で利用可能です";
                    } else {
                        oss << "近郊区間内ですので最安運賃の経路で計算";
                    }
                }

                // 大回り指定では115適用はみない
                if (route_flag.isEnableRule115() && !route_flag.isEnableLongRoute()) {
                    if (route_flag.isRule115specificTerm()) {
                        oss << "「単駅最安」で単駅発着が選択可能です";
                    } else {
                        oss << "「特定都区市内発着」で特定都区市内発着が選択可能です";
                    }
                }
            }

            // 私鉄競合特例運賃(大都市近郊区間)
            if (route_flag.special_fare_enable) {
                oss << "特定区間割引運賃適用";
            }

            if (fi.isBeginEndCompanyLine()) {
                oss << "会社線発着のため一枚の乗車券として発行されない場合があります.";
            }
            if (fi.isMultiCompanyLine()) {
                /* 2017.3 以降 ここに来ることはない */
                oss << "複数の会社線を跨っているため乗車券は通し発券できません. 運賃額も異なります.";
            }
            if (fi.isEnableTokaiStockSelect()) {
                oss << "JR東海株主優待券使用オプション選択可";
            }
            if (fi.getIsBRT_discount()) {
                oss << "BRT乗り継ぎ割引適用";
            }

            if (route_flag.no_rule && route_flag.special_fare_enable) {
                oss << "特定区間割引運賃を適用していません";
            }
            if (route_flag.no_rule && route_flag.isAvailableRule86()) {
                oss << "旅客営業規則第86条を適用していません";
            }
            if (route_flag.no_rule && route_flag.isAvailableRule87()) {
                oss << "旅客営業規則第87条を適用していません";
            }
            if (route_flag.no_rule && route_flag.isAvailableRule88()) {
                oss << "旅客営業規則第88条を適用していません";
            }
            if (route_flag.no_rule && route_flag.isAvailableRule69()) {
                oss << "旅客営業規則第69条を適用していません";
            }
            if (route_flag.no_rule && route_flag.isAvailableRule70()) {
                oss << "旅客営業規則第70条を適用していません";
            }
            if (route_flag.no_rule && route_flag.isAvailableRule115()) {
                oss << "旅客営業取扱基準規程第115条を適用していません";
            }
            if (route_flag.isAvailableRule16_5()) {
                oss << "この乗車券はJRで発券されません. 東京メトロでのみ発券されます";
            }
            if (fi.isRule114()) {
                oss << "旅客営業取扱基準規程第114条適用営業キロ計算駅:" << [&]() -> std::string {
                    std::ostringstream oss;
                    oss << fi.getRule114apply_terminal_station();
                    return oss.str();
                }();
            }
            if (route_flag.compnterm) {
                oss << "この経路の会社線通過連絡は許可されていません.";
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
    return add(RouteUtil::GetStationId(station.c_str()));
}

// add end station
int az_route::add_route(std::string line, std::string station)
{
    return add(RouteUtil::GetLineId(line.c_str()), RouteUtil::GetStationId(station.c_str()));
}

// auto route from current to destinationStation
int az_route::auto_route(int useBulletTrain, std::string destinationStation)
{
    return changeNeerest(useBulletTrain, RouteUtil::GetStationId(destinationStation.c_str()));
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
            if (0x10000 < ident) {
                prefects.push_back(dbo.getText(0));
            }
        }
        oss << json_encoder::begin_array("prefectures");
        for (std::string& pref : prefects) {
            if (0 < num++) {
                oss << ",";
            }
            if (!pref.empty()) {
                oss << "\"" << pref << "\"";
            }
        }
        oss << json_encoder::end_array();
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
        oss << json_encoder::begin_array("companies");
        for (std::string& company : companies) {
            if (0 < num++) {
                oss << ",";
            }
            if (!company.empty()) {
                oss << "\"" << company << "\"";
            }
        }
        oss << json_encoder::end_array();
        return oss.str();
	}
    return "{}";
}

// line-selection-screen

// 都道府県IDを返す
int fare_ui::get_prefect_id(std::string prefecture)
{
    static const char tsql[] = "select rowid from t_prefect where name=?";
    DBO dbo = DBS::getInstance()->compileSql(tsql);
    dbo.setParam(1, prefecture.c_str());

    if (dbo.moveNext()) {
        return dbo.getInt(0) + 0x10000;
    } else {
        return -1;
    }
}

// JRグループの会社IDを返す
int fare_ui::get_company_id(std::string company)
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

    DBO dbo = RouteUtil::Enum_lines_from_company_prefect(get_company_id(prefecture.c_str()));

    if (dbo.isvalid()) {
        std::ostringstream oss;
		while (dbo.moveNext()) {
            lines.push_back(dbo.getText(0));
        }
        oss << json_encoder::begin_array("lines");
        for (std::string& line : lines) {
            if (0 < num++) {
                oss << ",";
            }
            if (!line.empty()) {
                oss << "\"" << line << "\"";
            }
        }
        oss << json_encoder::end_array();
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
        oss << json_encoder::begin_array("lines");
        for (std::string& line : lines) {
            if (0 < num++) {
                oss << ",";
            }
            if (!line.empty()) {
                oss << "\"" << line << "\"";
            }
        }
        oss << json_encoder::end_array();
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
        oss << json_encoder::begin_array("lines");
        for (std::string& line : lines) {
            if (0 < num++) {
                oss << ",";
            }
            if (!line.empty()) {
                oss << "\"" << line << "\"";
            }
        }
        oss << json_encoder::end_array();
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
        oss << json_encoder::begin_array("stations");
        for (std::string& line : stations) {
            if (0 < num++) {
                oss << ",";
            }
            if (!line.empty()) {
                oss << "\"" << line << "\"";
            }
        }
        oss << json_encoder::end_array();
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
                get_company_id(prefecture.c_str()), 
                               RouteUtil::GetLineId(line_name.c_str()));
    if (dbo.isvalid()) {
        std::ostringstream oss;
		while (dbo.moveNext()) {
            stations.push_back(dbo.getText(0));
        }
        oss << json_encoder::begin_array("stations");
        for (std::string& line : stations) {
            if (0 < num++) {
                oss << ",";
            }
            if (!line.empty()) {
                oss << "\"" << line << "\"";
            }
        }
        oss << json_encoder::end_array();
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
        oss << "{";
        oss << json_encoder::begin_array("stations");
        for (std::string& line : stations) {
            if (0 < num++) {
                oss << ",";
            }
            if (!line.empty()) {
                oss << "\"" << line << "\"";
            }
        }
        oss << json_encoder::end_array();
        oss << "}";
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
        oss << json_encoder::begin_array("stations");
        for (std::string& line : stations) {
            if (0 < num++) {
                oss << ",";
            }
            if (!line.empty()) {
                oss << "\"" << line << "\"";
            }
        }
        oss << json_encoder::end_array();
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
        oss << json_encoder::begin_array("stations");
        for (std::string& line : stations) {
            if (0 < num++) {
                oss << ",";
            }
            if (!line.empty()) {
                oss << "\"" << line << "\"";
            }
        }
        oss << json_encoder::end_array();
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

/* build route from string

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
    oss << "{ \"rc\": " << rc << ", ";
    oss << json_encoder::pair("failItem", std::string(error_buf)) << ", ";
    oss << json_encoder::pair("offset", offset);
    oss << " }";

    return oss.str();
}
