
// azusa.h

#pragma once
#include "alpdb.h"
#include <string>
#include <sstream>
#include <algorithm>

std::string open_database();
void close_database();
std::string database_info();

class az_route : public Route {
public:
    // main view

    int add_start_route(std::string station);
    int add_route(std::string line, std::string station);
    int auto_route(int useBulletTrain, std::string destinationStation);

    int get_route_count() const { return RouteList::routeList().size(); }

    std::string departure_station_name() const { return RouteUtil::StationName(RouteList::departureStationId()); }
    std::string arriveval_station_name() const { return RouteUtil::StationName(RouteList::arriveStationId()); }    

    void remove_all() { Route::removeAll(); }
    void remove_tail() { Route::removeTail(); }
    int reverse() { return Route::reverse(); }

    void assign(const RouteList& source_route, int count) { Route::assign(source_route, count); } // Use for result details page and try auto route.

    int type_of_passed_line(int offset) { return Route::typeOfPassedLine(offset); }  // use for auto route
    int set_detour(bool enabled) { return Route::setDetour(enabled); }
    void set_no_rule(bool no_rule) { route_flag.setNoRule(no_rule); }


    // route_flag
    void set_long_route(bool flag) { route_flag.setLongRoute(flag); }
    void set_jr_tokai_stock_apply(bool flag) { route_flag.setJrTokaiStockApply(flag); } 
    void set_start_as_city() { route_flag.setStartAsCity(); }
    void set_arrival_as_city() { route_flag.setArriveAsCity(); }
    void set_specific_term_rule115(bool flag) { route_flag.setSpecificTermRule115(flag); }  

    bool is_not_same_kokura_hakata_shin_zai() { return isNotSameKokuraHakataShinZai(); }
    bool is_available_reverse() const { return RouteList::isAvailableReverse(); }
    bool is_osakakan_detour_enable() const { return route_flag.is_osakakan_1pass(); }
    bool is_osakakan_detour() const { return route_flag.osakakan_detour; }   
    void set_not_same_kokura_hakata_shin_zai(bool enabled) { Route::setNotSameKokuraHakataShinZai(enabled); }

    // 運賃計算結果出力
    std::string show_fare(); // for Shared export
    std::string get_fare_info_object_json(); // for JSON object

    // Changed from std::string& to std::string for WASM binding compatibility
    std::string build_route(const std::string& route_str);
    std::string route_script();

    // 内部配列を JSON にシリアライズして返す
    std::string get_routes_json();
    // index 番目の路線＋駅データを JSON で返す
    std::string get_route_record(int index);
};


namespace fare_ui {
    // global function
    // 都道府県の列挙
    std::string get_prefects();
    // JRグループの会社一覧を配列で返す
    std::string get_companys();

    // line-selection-screen

    // 都道府県の路線一覧を配列で返す
    std::string get_lines_by_prefect(std::string prefecture);
    // JRグループの路線一覧を配列で返す
    std::string get_lines_by_company(std::string jrgoutp);
    // 駅の所属路線を返す
    std::string get_lines_by_station(std::string station);
    // 路線の会社管轄内駅一覧を返す
    std::string get_stations_by_company_and_line(std::string jrgroup, std::string line_name);
    // 路線の都道府県内の駅一覧を返す
    std::string get_stations_by_prefecture_and_line(std::string prefecture, std::string line_name);

    // 駅のある都道府県
    std::string get_prefecture_by_station(const std::string& station_name);

    // 検索
    std::string search_station_by_keyword(std::string key);

    // 指定した駅も含め路線の分岐駅一覧を返す)
    std::string get_branch_stations_by_line(std::string line_name, std::string station_name);

    // 路線の全駅一覧を返す
    std::string get_stations_by_line(std::string line_name);

    // 駅名のかなを得る
    std::string get_kana_by_station(const std::string& station_name);
}

// Developer tools namespace
namespace dev {
    // Execute arbitrary SQL and return results as JSON
    // For debugging and development only
    std::string execute_sql(const std::string& sql);
}

namespace json_encoder {

    // string
    inline std::string pair(const std::string& key, const std::string& value, bool quote = true) {
        std::ostringstream oss;
        oss << "\"" << key << "\":";
        if (quote) {
            oss << "\"" << value << "\"";
        } else {
            oss << value;
        }
        return oss.str();
    }

    // bool
    inline std::string pair(const std::string& key, bool value) {
        std::ostringstream oss;
        oss << "\"" << key << "\":" << (value ? "true" : "false");
        return oss.str();
    }

    // int
    inline std::string pair(const std::string& key, int value) {
        std::ostringstream oss;
        oss << "\"" << key << "\":" << value;
        return oss.str();
    }

    inline std::string begin_array(const std::string& key) {
        std::ostringstream oss;
        oss << "\"" << key << "\":[";
        return oss.str();
    }

    inline std::string begin_array() {
        return "[";
    }

    inline std::string end_array() {
        return "]";
    }
    inline std::string value(const std::string & value) {
        return "\"" + value + "\"";
    }
}

//////////////////////////////////////////////////////////////
