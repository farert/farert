#include "stdafx.h"
#include <alpdb.h>
#include <cstdio>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <utility>
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

namespace {
struct StationCandidate {
    std::string name;
    std::string kana;
    std::vector<std::string> samename;
    int score = 99;
    std::string matchedBy = "name";
};

static void replace_all(std::string& text, const std::string& from, const std::string& to)
{
    if (from.empty()) return;
    std::size_t start = 0;
    while ((start = text.find(from, start)) != std::string::npos) {
        text.replace(start, from.length(), to);
        start += to.length();
    }
}

static std::size_t utf8_char_length(unsigned char lead)
{
    if ((lead & 0x80) == 0x00) return 1;
    if ((lead & 0xE0) == 0xC0) return 2;
    if ((lead & 0xF0) == 0xE0) return 3;
    if ((lead & 0xF8) == 0xF0) return 4;
    return 1;
}

static std::string extract_kana_only(const std::string& text)
{
    std::string out;
    for (std::size_t i = 0; i < text.size();) {
        const std::size_t cs = utf8_char_length(static_cast<unsigned char>(text[i]));
        if (i + cs > text.size()) break;
        const std::string token = text.substr(i, cs);
        if (isKanaString(token.c_str())) {
            out += token;
        }
        i += cs;
    }
    if (!out.empty()) {
        conv_to_kana2hira(out);
    }
    return out;
}

static std::string tail_utf8_chars(const std::string& text, std::size_t char_count)
{
    if (text.empty() || char_count == 0) return "";
    std::vector<std::size_t> starts;
    for (std::size_t i = 0; i < text.size();) {
        starts.push_back(i);
        const std::size_t cs = utf8_char_length(static_cast<unsigned char>(text[i]));
        if (cs == 0) break;
        i += cs;
    }
    if (starts.empty()) return "";
    const std::size_t begin_index = (starts.size() > char_count) ? starts[starts.size() - char_count] : 0;
    return text.substr(begin_index);
}

static std::string normalize_route_token(std::string text)
{
    replace_all(text, " ", "");
    replace_all(text, "\t", "");
    replace_all(text, "\r", "");
    replace_all(text, "\n", "");
    replace_all(text, "　", "");
    replace_all(text, "（", "(");
    replace_all(text, "）", ")");
    return text;
}

static std::string route_token_base_name(const std::string& text)
{
    const std::string normalized = normalize_route_token(text);
    const std::size_t pos = normalized.find('(');
    if (pos == std::string::npos) {
        return normalized;
    }
    return normalized.substr(0, pos);
}

static bool contains_int(const std::vector<int32_t>& values, int32_t value)
{
    return std::find(values.cbegin(), values.cend(), value) != values.cend();
}

static void push_unique_int(std::vector<int32_t>& values, int32_t value)
{
    if ((value > 0) && !contains_int(values, value)) {
        values.push_back(value);
    }
}

static bool line_contains_station(int32_t line_id, int32_t station_id)
{
    static const char tsql[] =
        "select count(*) from t_lines where (lflg&((1<<31)|(1<<17)))=0 and line_id=?1 and station_id=?2";

    DBO dbo = DBS::getInstance()->compileSql(tsql);
    if (dbo.isvalid()) {
        dbo.setParam(1, line_id);
        dbo.setParam(2, station_id);
        if (dbo.moveNext()) {
            return dbo.getInt(0) > 0;
        }
    }
    return false;
}

static std::vector<int32_t> resolve_line_candidates_from_station(
    int32_t current_station_id,
    const std::string& input_line,
    const std::vector<int32_t>& target_station_candidates)
{
    std::vector<int32_t> line_ids;
    const int32_t exact_line_id = RouteUtil::GetLineId(input_line.c_str());
    const std::string normalized_input = normalize_route_token(input_line);
    const std::string input_base = route_token_base_name(input_line);

    if ((exact_line_id > 0) && line_contains_station(exact_line_id, current_station_id)) {
        push_unique_int(line_ids, exact_line_id);
    }

    DBO dbo = RouteUtil::Enum_line_of_stationId(current_station_id);
    while (dbo.moveNext()) {
        const std::string line_name = dbo.getText(0);
        const int32_t line_id = dbo.getInt(1);
        const std::string normalized_line_name = normalize_route_token(line_name);
        const std::string line_base = route_token_base_name(line_name);

        if ((normalized_line_name != normalized_input) && (line_base != input_base)) {
            continue;
        }

        if (!target_station_candidates.empty()) {
            bool reachable = false;
            for (int32_t station_id : target_station_candidates) {
                if (line_contains_station(line_id, station_id)) {
                    reachable = true;
                    break;
                }
            }
            if (!reachable) {
                continue;
            }
        }

        push_unique_int(line_ids, line_id);
    }

    return line_ids;
}

static std::vector<int32_t> resolve_station_candidates_on_line(int32_t line_id, const std::string& input_station)
{
    std::vector<int32_t> station_ids;
    const int32_t exact_station_id = RouteUtil::GetStationId(input_station.c_str());
    const std::string normalized_input = normalize_route_token(input_station);
    const std::string input_base = route_token_base_name(input_station);

    if (exact_station_id > 0) {
        push_unique_int(station_ids, exact_station_id);
    }

    DBO dbo = RouteUtil::Enum_station_of_lineId(line_id);
    while (dbo.moveNext()) {
        const int32_t station_id = dbo.getInt(1);
        const std::string station_name = RouteUtil::StationNameEx(station_id);
        const std::string normalized_station_name = normalize_route_token(station_name);
        const std::string station_base = route_token_base_name(station_name);

        if ((normalized_station_name == normalized_input) || (station_base == input_base)) {
            push_unique_int(station_ids, station_id);
        }
    }

    return station_ids;
}

static std::vector<int32_t> resolve_station_candidates_for_start(const std::string& input_station, const std::string& next_line)
{
    std::vector<int32_t> station_ids;
    const int32_t exact_station_id = RouteUtil::GetStationId(input_station.c_str());
    if (exact_station_id > 0) {
        push_unique_int(station_ids, exact_station_id);
    }

    const int32_t next_line_id = RouteUtil::GetLineId(next_line.c_str());
    const std::string normalized_input = normalize_route_token(input_station);
    const std::string input_base = route_token_base_name(input_station);

    if (next_line_id <= 0) {
        return station_ids;
    }

    static const char tsql[] =
        "select t.rowid, t.name, t.samename"
        " from t_lines l"
        " join t_station t on t.rowid=l.station_id"
        " where l.line_id=?1 and (l.lflg&((1<<31)|(1<<17)))=0"
        " order by l.sales_km";

    DBO dbo = DBS::getInstance()->compileSql(tsql);
    dbo.setParam(1, next_line_id);
    while (dbo.moveNext()) {
        const int32_t station_id = dbo.getInt(0);
        const std::string station_name = std::string(dbo.getText(1)) + std::string(dbo.getText(2));
        const std::string normalized_station_name = normalize_route_token(station_name);
        const std::string station_base = route_token_base_name(station_name);

        if ((normalized_station_name == normalized_input) || (station_base == input_base)) {
            push_unique_int(station_ids, station_id);
        }
    }

    return station_ids;
}

static int try_add_route_candidate(az_route& route, int32_t line_id, int32_t station_id)
{
    az_route snapshot;
    snapshot.assign(route, route.get_route_count());
    const int rc = route.add(line_id, station_id);
    if (rc < 0) {
        route.assign(snapshot, snapshot.get_route_count());
    }
    return rc;
}

static int try_auto_route_candidate(az_route& route, int use_bullet_train, int32_t station_id)
{
    az_route snapshot;
    snapshot.assign(route, route.get_route_count());
    const int rc = route.changeNeerest(use_bullet_train, station_id);
    if (rc < 0) {
        route.assign(snapshot, snapshot.get_route_count());
    }
    return rc;
}

static std::vector<int32_t> resolve_station_candidates_anywhere(const std::string& input_station)
{
    std::vector<int32_t> station_ids;
    const int32_t exact_station_id = RouteUtil::GetStationId(input_station.c_str());
    const std::string normalized_input = normalize_route_token(input_station);
    const std::string input_base = route_token_base_name(input_station);

    if (exact_station_id > 0) {
        push_unique_int(station_ids, exact_station_id);
    }

    static const char tsql[] =
        "select rowid, name, samename from t_station where (sflg&(1<<18))=0";
    DBO dbo = DBS::getInstance()->compileSql(tsql);
    while (dbo.moveNext()) {
        const int32_t station_id = dbo.getInt(0);
        const std::string station_name = std::string(dbo.getText(1)) + std::string(dbo.getText(2));
        const std::string normalized_station_name = normalize_route_token(station_name);
        const std::string station_base = route_token_base_name(station_name);

        if ((normalized_station_name == normalized_input) || (station_base == input_base)) {
            push_unique_int(station_ids, station_id);
        }
    }

    return station_ids;
}

static std::vector<std::string> tokenize_route_string(const std::string& route_str)
{
    std::vector<std::string> tokens;
    std::string token;

    auto flush = [&]() {
        if (!token.empty()) {
            tokens.push_back(token);
            token.clear();
        }
    };

    for (char c : route_str) {
        if (c == ',' || c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            flush();
            continue;
        }
        token.push_back(c);
    }
    flush();
    return tokens;
}

static int build_route_from_tokens(az_route& route, const std::vector<std::string>& tokens, std::string& fail_item, int& offset)
{
    if (tokens.empty()) {
        fail_item.clear();
        offset = 0;
        return -1;
    }

    route.remove_all();

    int result = -999;
    for (std::size_t i = 0; i < tokens.size();) {
        if (route.get_route_count() == 0) {
            if (i + 1 < tokens.size()) {
                result = route.add_start_route(tokens[i], tokens[i + 1]);
            } else {
                result = route.add_start_route(tokens[i]);
            }
            if (result < 0) {
                fail_item = tokens[i];
                offset = static_cast<int>(i);
                return result;
            }
            i += 1;
            continue;
        }

        if (i + 1 < tokens.size()) {
            result = route.add_route(tokens[i], tokens[i + 1]);
            if (result < 0) {
                fail_item = tokens[i + 1];
                offset = static_cast<int>(i + 1);
                return result;
            }
            i += 2;
            continue;
        }

        const std::vector<int32_t> station_candidates = resolve_station_candidates_anywhere(tokens[i]);
        if (station_candidates.empty()) {
            fail_item = tokens[i];
            offset = static_cast<int>(i);
            return -200;
        }

        result = -200;
        for (int32_t station_id : station_candidates) {
            const int rc = try_auto_route_candidate(route, 1, station_id);
            if (rc >= 0) {
                result = rc;
                break;
            }
            result = rc;
        }
        if (result < 0) {
            fail_item = tokens[i];
            offset = static_cast<int>(i);
            return result;
        }
        i += 1;
    }

    fail_item.clear();
    offset = 0;
    return result >= 0 ? result : 0;
}

static const char* stock_discount_company_name(int32_t company)
{
    switch (company) {
    case JR_EAST:
        return "JR東日本";
    case JR_WEST:
        return "JR西日本";
    case JR_CENTRAL:
        return "JR東海";
    case JR_KYUSYU:
        return "JR九州";
    default:
        return "";
    }
}

static int32_t stock_discount_rate(int32_t company, int32_t index)
{
    switch (company) {
    case JR_EAST:
        return index == 0 ? 40 : 0;
    case JR_WEST:
        return index == 0 ? 50 : 0;
    case JR_CENTRAL:
        if (index == 0) return 10;
        if (index == 1) return 20;
        return 0;
    case JR_KYUSYU:
        return index == 0 ? 50 : 0;
    default:
        return 0;
    }
}

static const char* stock_discount_kind(int32_t company, int32_t index)
{
    if (company == JR_CENTRAL && index == 1) {
        return "double";
    }
    return "single";
}

// 仮実装: UTF-8完全正規化は行わず、主要な表記ゆれのみ吸収する
static std::string normalize_station_token(std::string text)
{
    // 空白・改行
    replace_all(text, " ", "");
    replace_all(text, "\t", "");
    replace_all(text, "\r", "");
    replace_all(text, "\n", "");
    replace_all(text, "　", "");

    // 括弧・中点・長音などの記号ゆれ
    replace_all(text, "（", "(");
    replace_all(text, "）", ")");
    replace_all(text, "・", "");
    replace_all(text, "ｰ", "");
    replace_all(text, "ー", "");
    replace_all(text, "-", "");
    replace_all(text, "−", "");

    // カタカナ/ひらがなの主要ゆれ（仮実装）
    replace_all(text, "カ", "か");
    replace_all(text, "ガ", "が");
    replace_all(text, "ケ", "け");
    replace_all(text, "ゲ", "げ");
    replace_all(text, "ツ", "つ");
    replace_all(text, "ッ", "つ");
    replace_all(text, "ノ", "の");
    replace_all(text, "ヂ", "じ");
    replace_all(text, "ヅ", "ず");

    // かな表記ゆれ
    replace_all(text, "ぢ", "じ");
    replace_all(text, "づ", "ず");
    replace_all(text, "ゔ", "う");

    // 漢字・異体字ゆれ
    replace_all(text, "ノ", "の");
    replace_all(text, "之", "の");
    replace_all(text, "ヶ", "が");
    replace_all(text, "ケ", "が");
    replace_all(text, "け", "が");
    replace_all(text, "龍", "竜");
    replace_all(text, "總", "総");
    replace_all(text, "澤", "沢");
    replace_all(text, "齊", "斉");
    replace_all(text, "斎", "斉");
    replace_all(text, "亘", "渡");
    replace_all(text, "冨", "富");
    replace_all(text, "﨑", "崎");
    replace_all(text, "嵜", "崎");
    replace_all(text, "溪", "渓");
    return text;
}

static std::vector<std::string> split_samename(const std::string& samename)
{
    std::vector<std::string> result;
    if (samename.empty()) return result;
    std::string normalized = samename;
    replace_all(normalized, "、", ",");
    replace_all(normalized, "，", ",");
    replace_all(normalized, "；", ";");
    replace_all(normalized, "｜", "|");
    std::string token;
    auto push_token = [&]() {
        if (!token.empty()) {
            result.push_back(token);
            token.clear();
        }
    };
    for (std::size_t i = 0; i < normalized.size(); i++) {
        const char c = normalized[i];
        if (c == ',' || c == ';' || c == '|') {
            push_token();
            continue;
        }
        token.push_back(c);
    }
    push_token();
    if (result.empty()) {
        result.push_back(normalized);
    }
    return result;
}

static std::string make_station_candidate_key(const StationCandidate& candidate)
{
    std::ostringstream oss;
    oss << candidate.name << "\t";
    for (std::size_t i = 0; i < candidate.samename.size(); i++) {
        if (i > 0) {
            oss << "/";
        }
        oss << candidate.samename[i];
    }
    oss << "\t" << candidate.kana;
    return oss.str();
}

static void append_keyword_match_candidates(
    std::vector<StationCandidate>& candidates,
    const std::string& keyword,
    int score,
    const std::string& matchedBy)
{
    if (keyword.empty()) return;
    DBO dbo = RouteUtil::Enum_station_match(keyword.c_str());
    if (!dbo.isvalid()) return;

    while (dbo.moveNext()) {
        const std::string name = dbo.getText(0);
        if (name.empty()) continue;
        const std::string samename = dbo.getText(2);

        StationCandidate item;
        item.name = name;
        item.kana = RouteUtil::GetKanaFromStationId(RouteUtil::GetStationId(name.c_str()));
        item.samename = split_samename(samename);
        item.score = score;
        item.matchedBy = matchedBy;
        candidates.push_back(item);
    }
}

static void append_kana_suffix_candidates(
    std::vector<StationCandidate>& candidates,
    const std::string& key,
    int score,
    const std::string& matchedBy)
{
    const std::string kanaOnly = extract_kana_only(normalize_station_token(key));
    // 2文字以上のかなが取れるときだけ末尾一致を試す
    const std::string suffix = tail_utf8_chars(kanaOnly, 2);
    if (suffix.empty() || suffix == kanaOnly) return;

    static const char tsql[] = "select name,kana,samename from t_station where kana like ? order by kana";
    DBO dbo = DBS::getInstance()->compileSql(tsql, false);
    if (!dbo.isvalid()) return;

    const std::string likeParam = "%" + suffix + "%";
    dbo.setParam(1, likeParam.c_str());

    while (dbo.moveNext()) {
        const std::string name = dbo.getText(0);
        if (name.empty()) continue;
        StationCandidate item;
        item.name = name;
        item.kana = dbo.getText(1);
        item.samename = split_samename(dbo.getText(2));
        item.score = score;
        // 「◯ノ巣/◯の巣」の駅を優先（例: 鳩ノ巣）
        if (name.find("ノ巣") != std::string::npos || name.find("の巣") != std::string::npos) {
            item.score = std::max(0, score - 1);
        }
        item.matchedBy = matchedBy;
        candidates.push_back(item);
    }
}

static int score_station_candidate(
    const std::string& normalizedKey,
    const std::string& name,
    const std::string& kana,
    const std::vector<std::string>& sameNames,
    std::string& matchedBy)
{
    const std::string normalizedName = normalize_station_token(name);
    const std::string normalizedKana = normalize_station_token(kana);
    const std::string kanaOnlyKey = extract_kana_only(normalizedKey);
    const std::string kanaOnlyStation = extract_kana_only(normalizedKana);
    if (!normalizedKey.empty()) {
        if (normalizedName == normalizedKey) {
            matchedBy = "name";
            return 0;
        }
        if (normalizedName.find(normalizedKey) == 0) {
            matchedBy = "name";
            return 1;
        }
        if (normalizedName.find(normalizedKey) != std::string::npos) {
            matchedBy = "name";
            return 2;
        }
        if (!normalizedKana.empty()) {
            if (normalizedKana.find(normalizedKey) == 0) {
                matchedBy = "kana";
                return 3;
            }
            if (normalizedKana.find(normalizedKey) != std::string::npos) {
                matchedBy = "kana";
                return 4;
            }
        }
        // ひらがな部分のみでの照合（例: おちゃの水 -> おちゃの）
        if (kanaOnlyKey.size() >= 6 && !kanaOnlyStation.empty()) {
            if (kanaOnlyStation.find(kanaOnlyKey) == 0) {
                matchedBy = "kana";
                return 7;
            }
            if (kanaOnlyStation.find(kanaOnlyKey) != std::string::npos) {
                matchedBy = "kana";
                return 8;
            }
        }
        for (const auto& alias : sameNames) {
            const std::string normalizedAlias = normalize_station_token(alias);
            if (normalizedAlias.find(normalizedKey) == 0) {
                matchedBy = "samename";
                return 5;
            }
            if (normalizedAlias.find(normalizedKey) != std::string::npos) {
                matchedBy = "samename";
                return 6;
            }
        }
    }
    return 99;
}
} // namespace


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
		return "{ \"result\": false, \"reason\": \"farertDB environment variable not set.\" }";
	}
#endif
    if (DBS::getInstance()->open(dbpath) && RouteUtil::DbVer(&dbsys)) {
        return "{ \"result\": true, \"dbName\": \"" + std::string(dbsys.name)
         + "\", \"createdate\": \"" + std::string(dbsys.createdate) + "\", \"tax\": " + std::to_string(g_tax) + " }"  ;
    } else {
        return "{ \"result\": false, \"reason\": \"failed to open database.\" }";
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
         + "\", \"createdate\": \"" + std::string(dbsys.createdate) + "\", \"tax\": " + std::to_string(g_tax) + " }"  ;
    } else {
        return "{ \"result\": false, \"reason\": \"failed to open database.\" }";
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
    CalcRoute crt(*this);
    crt.calcFare(&fi);
    int rc = fi.resultCode();
    if ((rc != 0) && (rc != -1)) {
        return std::string("{ \"fareResultCode\": -2 }"); // -2: empty or -3: fail
    }
    const RouteFlag& refRouteFlag = crt.getRouteFlag();

    auto jsjoin = [&](const std::vector<std::string>& items) -> std::string {
        std::ostringstream oss;
        int i = 0;
        for (auto& item : items) {
            if (0 < i) {
                oss << ",";
            }
            oss << item;
            i++;
        }
        return oss.str();
    };
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
            const int32_t stockCompany = fi.getStockDiscountCompany();

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
                oss << ",";
                oss << json_encoder::pair("company", stock_discount_company_name(stockCompany));
                oss << ",";
                oss << json_encoder::pair("discountRate", stock_discount_rate(stockCompany, i));
                oss << ",";
                oss << json_encoder::pair("discountKind", stock_discount_kind(stockCompany, i));
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
            std::vector<std::string> messages;
            oss << json_encoder::begin_array("messages");
            
            if (refRouteFlag.no_rule &&
                    fi.isUrbanArea() && !refRouteFlag.isUseBullet()) {
                if (fi.getBeginTerminalId() == fi.getEndTerminalId()) {
                    // messages.add(msgCantMetroTicket)
                } else if (!refRouteFlag.isEnableRule115()
                        || !refRouteFlag.isRule115specificTerm()) {
                    if (refRouteFlag.isLongRoute()) {
                        messages.push_back(json_encoder::value("近郊区間内ですので最短経路の運賃で利用可能です"));
                    } else {
                        messages.push_back(json_encoder::value("近郊区間内ですので最安運賃の経路で計算"));
                    }
                }

                // 大回り指定では115適用はみない
                if (refRouteFlag.isEnableRule115() && !refRouteFlag.isEnableLongRoute()) {
                    if (refRouteFlag.isRule115specificTerm()) {
                        messages.push_back(json_encoder::value("「単駅最安」で単駅発着が選択可能です"));
                    } else {
                        messages.push_back(json_encoder::value("「特定都区市内発着」で特定都区市内発着が選択可能です"));
                    }
                }
            }

            // 私鉄競合特例運賃(大都市近郊区間)
            if (!refRouteFlag.no_rule && refRouteFlag.special_fare_enable) {
                messages.push_back(json_encoder::value("特定区間割引運賃適用"));
            }

            if (fi.isBeginEndCompanyLine()) {
                messages.push_back(json_encoder::value("会社線発着のため一枚の乗車券として発行されない場合があります."));
            }
            if (fi.isMultiCompanyLine()) {
                /* 2017.3 以降 ここに来ることはない */
                messages.push_back(json_encoder::value("複数の会社線を跨っているため乗車券は通し発券できません. 運賃額も異なります."));
            }
            if (fi.isEnableTokaiStockSelect()) {
                messages.push_back(json_encoder::value("JR東海株主優待券使用オプション選択可"));
            }
            if (fi.getIsBRT_discount()) {
                messages.push_back(json_encoder::value("BRT乗り継ぎ割引適用"));
            }

            if (refRouteFlag.no_rule && refRouteFlag.special_fare_enable) {
                messages.push_back(json_encoder::value("特定区間割引運賃を適用していません"));
            }
            if (refRouteFlag.no_rule && refRouteFlag.isAvailableRule86()) {
                messages.push_back(json_encoder::value("旅客営業規則第86条を適用していません"));
            }
            if (refRouteFlag.no_rule && refRouteFlag.isAvailableRule87()) {
                messages.push_back(json_encoder::value("旅客営業規則第87条を適用していません"));
            }
            if (refRouteFlag.no_rule && refRouteFlag.isAvailableRule88()) {
                messages.push_back(json_encoder::value("旅客営業規則第88条を適用していません"));
            }
            if (refRouteFlag.no_rule && refRouteFlag.isAvailableRule160_4()) {
                messages.push_back(json_encoder::value("旅客営業規則第160条第4項を適用していません"));
            }
            if (refRouteFlag.no_rule && refRouteFlag.isAvailableRule69()) {
                messages.push_back(json_encoder::value("旅客営業規則第69条を適用していません"));
            }
            if (refRouteFlag.no_rule && refRouteFlag.isAvailableRule70()) {
                messages.push_back(json_encoder::value("旅客営業規則第70条を適用していません"));
            }
            if (refRouteFlag.no_rule && refRouteFlag.isAvailableRule115()) {
                messages.push_back(json_encoder::value("旅客営業取扱基準規程第115条を適用していません"));
            }
            if (refRouteFlag.isAvailableRule16_5()) {
                messages.push_back(json_encoder::value("この乗車券はJRで発券されません. 東京メトロでのみ発券されます"));
            }
            if (fi.isRule114()) {
                messages.push_back(json_encoder::value("旅客営業取扱基準規程第114条適用営業キロ計算駅:" + fi.getRule114apply_terminal_station()));
            }
            if (refRouteFlag.compnterm) {
                messages.push_back(json_encoder::value("この経路の会社線通過連絡は許可されていません."));
            }
            oss << jsjoin(messages);
            oss << json_encoder::end_array();
            return oss.str();
        }(),
    };

    std::ostringstream oss;
    oss << "{";
    oss << jsjoin(fields);
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

int az_route::add_start_route(std::string station, std::string next_line)
{
    const std::vector<int32_t> station_candidates = resolve_station_candidates_for_start(station, next_line);
    for (int32_t station_id : station_candidates) {
        az_route snapshot;
        snapshot.assign(*this, this->get_route_count());
        const int rc = add(station_id);
        if (rc >= 0) {
            return rc;
        }
        assign(snapshot, snapshot.get_route_count());
    }
    return -200;
}

// add end station
int az_route::add_route(std::string line, std::string station)
{
    const int32_t current_station_id = RouteList::routeList().empty() ? 0 : RouteList::routeList().back().stationId;
    const int32_t exact_station_id = RouteUtil::GetStationId(station.c_str());
    if (exact_station_id <= 0 && current_station_id <= 0) {
        return -200;
    }

    std::vector<int32_t> target_station_candidates;
    if (exact_station_id > 0) {
        push_unique_int(target_station_candidates, exact_station_id);
    }

    std::vector<int32_t> line_candidates = resolve_line_candidates_from_station(current_station_id, line, target_station_candidates);
    if (line_candidates.empty()) {
        const int32_t exact_line_id = RouteUtil::GetLineId(line.c_str());
        if (exact_line_id <= 0) {
            return -300;
        }
        push_unique_int(line_candidates, exact_line_id);
    }

    if (exact_station_id <= 0) {
        for (int32_t line_id : line_candidates) {
            const std::vector<int32_t> station_candidates = resolve_station_candidates_on_line(line_id, station);
            for (int32_t station_id : station_candidates) {
                push_unique_int(target_station_candidates, station_id);
            }
        }
    }

    if (target_station_candidates.empty()) {
        return -200;
    }

    int fallback_rc = -200;
    for (int32_t line_id : line_candidates) {
        const std::vector<int32_t> station_candidates = resolve_station_candidates_on_line(line_id, station);
        for (int32_t station_id : station_candidates) {
            const int rc = try_add_route_candidate(*this, line_id, station_id);
            if (rc >= 0) {
                return rc;
            }
            fallback_rc = rc;
        }
    }

    return fallback_rc;
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
    return oss.str();
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
    if (rc < 0) {
        std::string fallback_fail_item;
        int fallback_offset = 0;
        const int fallback_rc = build_route_from_tokens(*this, tokenize_route_string(route_str), fallback_fail_item, fallback_offset);
        if (fallback_rc >= 0) {
            rc = fallback_rc;
            error_buf[0] = '\0';
            offset = fallback_offset;
        } else if (!fallback_fail_item.empty()) {
            std::snprintf(error_buf, sizeof(error_buf), "%s", fallback_fail_item.c_str());
            offset = fallback_offset;
        }
    }
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
    return "{}";
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
    return "{}";
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
    return "{}";
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
    return "{}";
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
    return "{}";
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
    return "{}";
}

// あいまい検索（仮実装）
std::string fare_ui::search_station_fuzzy(std::string key, int limit)
{
    std::vector<StationCandidate> candidates;
    if (limit <= 0) {
        limit = 50;
    } else if (limit > 200) {
        limit = 200;
    }
    const std::string normalizedKey = normalize_station_token(key);

    DBO dbo = DBS::getInstance()->compileSql("select name,kana,samename from t_station");
    if (!dbo.isvalid()) {
        return "{\"results\":[]}";
    }

    while (dbo.moveNext()) {
        const std::string name = dbo.getText(0);
        const std::string kana = dbo.getText(1);
        const std::string samename = dbo.getText(2);
        const std::vector<std::string> aliases = split_samename(samename);

        std::string matchedBy = "name";
        const int score = score_station_candidate(normalizedKey, name, kana, aliases, matchedBy);
        if (score >= 99) {
            continue;
        }

        StationCandidate item;
        item.name = name;
        item.kana = kana;
        item.samename = aliases;
        item.score = score;
        item.matchedBy = matchedBy;
        candidates.push_back(item);
    }

    // 補助: 既存の駅名検索（name/kana prefix）で候補を追加
    append_keyword_match_candidates(candidates, key, 10, "keyword");
    if (normalizedKey != key) {
        append_keyword_match_candidates(candidates, normalizedKey, 11, "keyword");
    }
    append_kana_suffix_candidates(candidates, key, 12, "kana_suffix");

    std::sort(candidates.begin(), candidates.end(), [](const StationCandidate& a, const StationCandidate& b) {
        if (a.score != b.score) return a.score < b.score;
        return a.name < b.name;
    });

    std::unordered_set<std::string> seen;
    std::ostringstream oss;
    oss << "{" << json_encoder::begin_array("results");
    int out = 0;
    for (const auto& candidate : candidates) {
        if (out >= limit) break;
        if (candidate.name.empty()) continue;
        if (!seen.insert(make_station_candidate_key(candidate)).second) continue;
        if (out++ > 0) {
            oss << ",";
        }
        oss << "{"
            << json_encoder::pair("name", candidate.name) << ","
            << json_encoder::pair("kana", candidate.kana) << ","
            << json_encoder::pair("score", candidate.score) << ","
            << json_encoder::pair("matchedBy", candidate.matchedBy) << ","
            << json_encoder::begin_array("samename");
        for (std::size_t i = 0; i < candidate.samename.size(); i++) {
            if (i > 0) {
                oss << ",";
            }
            oss << json_encoder::value(candidate.samename[i]);
        }
        oss << json_encoder::end_array() << "}";
    }
    oss << json_encoder::end_array() << "}";
    return oss.str();
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
    return "{}";
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
    return "{}";
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
