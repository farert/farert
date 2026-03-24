/**
 * fuzzsearch.cpp
 *
 * fare_ui::search_station_fuzzy() を使って駅名あいまい検索を行うCLI
 *
 * 使い方:
 *   ./fuzzsearch <keyword> [limit]
 *
 * 例:
 *   ./fuzzsearch "竜が崎"
 *   ./fuzzsearch "横川(陽)" 30
 */

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

#include "azusa.h"

int g_tax = 10;

struct FuzzyItem {
    std::string name;
    std::string kana;
    std::vector<std::string> samename;
};

static void usage(const char* program) {
    std::cerr << "使い方: " << program << " <keyword> [limit]\n";
}

static bool parse_json_string_at(const std::string& text, std::size_t quote_pos, std::string& out, std::size_t& next_pos) {
    if (quote_pos >= text.size() || text[quote_pos] != '"') {
        return false;
    }

    out.clear();
    for (std::size_t i = quote_pos + 1; i < text.size(); i++) {
        const char c = text[i];
        if (c == '\\') {
            if (i + 1 >= text.size()) return false;
            const char esc = text[++i];
            switch (esc) {
                case '"': out.push_back('"'); break;
                case '\\': out.push_back('\\'); break;
                case 'n': out.push_back('\n'); break;
                case 'r': out.push_back('\r'); break;
                case 't': out.push_back('\t'); break;
                default: out.push_back(esc); break;
            }
            continue;
        }
        if (c == '"') {
            next_pos = i + 1;
            return true;
        }
        out.push_back(c);
    }

    return false;
}

static std::vector<std::string> parse_json_string_array(const std::string& text, std::size_t array_pos, std::size_t& next_pos) {
    std::vector<std::string> values;
    if (array_pos >= text.size() || text[array_pos] != '[') return values;

    std::size_t pos = array_pos + 1;
    while (pos < text.size()) {
        while (pos < text.size() && (text[pos] == ' ' || text[pos] == '\n' || text[pos] == '\r' || text[pos] == '\t' || text[pos] == ',')) {
            pos++;
        }
        if (pos >= text.size()) break;
        if (text[pos] == ']') {
            next_pos = pos + 1;
            return values;
        }
        if (text[pos] != '"') {
            break;
        }

        std::string value;
        std::size_t str_next = pos;
        if (!parse_json_string_at(text, pos, value, str_next)) {
            break;
        }
        values.push_back(value);
        pos = str_next;
    }

    return values;
}

static std::vector<FuzzyItem> parse_fuzzy_results(const std::string& json) {
    std::vector<FuzzyItem> items;
    std::size_t pos = 0;

    while (true) {
        const std::size_t name_key = json.find("\"name\":\"", pos);
        if (name_key == std::string::npos) break;

        const std::size_t name_quote = name_key + 7;
        std::string name;
        std::size_t after_name = name_quote;
        if (!parse_json_string_at(json, name_quote, name, after_name)) {
            pos = name_key + 1;
            continue;
        }

        const std::size_t kana_key = json.find("\"kana\":\"", after_name);
        if (kana_key == std::string::npos) {
            pos = after_name;
            continue;
        }
        const std::size_t kana_quote = kana_key + 7;
        std::string kana;
        std::size_t after_kana = kana_quote;
        if (!parse_json_string_at(json, kana_quote, kana, after_kana)) {
            pos = after_name;
            continue;
        }

        std::vector<std::string> same_names;
        const std::size_t same_key = json.find("\"samename\":[", after_kana);
        if (same_key != std::string::npos) {
            const std::size_t array_pos = same_key + 11;
            std::size_t after_array = array_pos;
            same_names = parse_json_string_array(json, array_pos, after_array);
            pos = after_array;
        } else {
            pos = after_kana;
        }

        FuzzyItem item;
        item.name = name;
        item.kana = kana;
        item.samename = same_names;
        items.push_back(item);
    }

    return items;
}

static int parse_limit(const char* value) {
    if (!value) return 20;
    const int parsed = std::atoi(value);
    if (parsed <= 0) return 20;
    if (parsed > 200) return 200;
    return parsed;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        usage(argv[0]);
        return 1;
    }

    const std::string keyword = argv[1];
    const int limit = (argc >= 3) ? parse_limit(argv[2]) : 20;

    const std::string open_result = open_database();
    if (open_result.find("\"result\": true") == std::string::npos) {
        std::cerr << "DBオープン失敗: " << open_result << "\n";
        return 2;
    }

    const std::string result_json = fare_ui::search_station_fuzzy(keyword, limit);
    const std::vector<FuzzyItem> items = parse_fuzzy_results(result_json);

    std::unordered_set<std::string> printed;
    for (const auto& item : items) {
        std::string kana = item.kana;
        if (kana.empty()) {
            kana = fare_ui::get_kana_by_station(item.name);
        }

        if (item.samename.empty()) {
            if (printed.insert(item.name).second) {
                std::cout << item.name << "\t" << kana << "\n";
            }
            continue;
        }

        bool emitted_alias = false;
        for (const auto& alias : item.samename) {
            if (alias.empty()) continue;
            const std::string display = item.name + alias;
            std::string display_kana = kana;
            if (display_kana.empty()) {
                display_kana = fare_ui::get_kana_by_station(display);
            }
            if (printed.insert(display).second) {
                std::cout << display << "\t" << display_kana << "\n";
                emitted_alias = true;
            }
        }
        if (!emitted_alias && printed.insert(item.name).second) {
            std::cout << item.name << "\t" << kana << "\n";
        }
    }

    close_database();
    return 0;
}
