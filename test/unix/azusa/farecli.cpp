/**
 * farecli.cpp
 *
 * azusa API を使った運賃計算CLIツール
 *
 * 使い方:
 *   # ファイルから経路を読み込み（1行1経路）
 *   ./farecli -f routes.txt
 *
 *   # カンマ区切りの経路文字列
 *   ./farecli "東京,東海道線,品川,東海道線,横浜"
 *
 *   # 駅名と路線名を個別に指定
 *   ./farecli 東京 東海道線 品川 東海道線 横浜
 *
 *   # 自動経路検索（偶数個の引数）
 *   ./farecli 東京 新大阪
 *
 *   # JSON出力オプション
 *   ./farecli -j 東京 東海道線 横浜
 *
 * コンパイル方法:
 *   cd test/unix/azusa
 *   make
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <unistd.h>
#include "azusa.h"

// グローバル変数（税率: 10%）
int g_tax = 10;

// 使い方表示
void usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [options] <arguments>\n"
              << "\n"
              << "Options:\n"
              << "  -f <filename>  ファイルから経路を読み込み（1行1経路）\n"
              << "  -j             JSON形式で運賃情報を出力\n"
              << "  -h             このヘルプを表示\n"
              << "\n"
              << "Arguments:\n"
              << "  1. カンマ区切りの経路文字列:\n"
              << "     例: ./farecli \"東京,東海道線,品川,東海道線,横浜\"\n"
              << "\n"
              << "  2. 駅名と路線名を個別指定（奇数個）:\n"
              << "     例: ./farecli 東京 東海道線 品川 東海道線 横浜\n"
              << "\n"
              << "  3. 自動経路検索（偶数個）:\n"
              << "     例: ./farecli 東京 新大阪\n"
              << "\n"
              << "Examples:\n"
              << "  " << program_name << " -f routes.txt\n"
              << "  " << program_name << " \"東京,東海道線,横浜\"\n"
              << "  " << program_name << " 東京 東海道線 横浜\n"
              << "  " << program_name << " -j 東京 新大阪\n"
              << std::endl;
}

// カンマ区切り文字列を分割
std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        // 前後の空白を削除
        size_t start = token.find_first_not_of(" \t\r\n");
        size_t end = token.find_last_not_of(" \t\r\n");

        if (start != std::string::npos && end != std::string::npos) {
            tokens.push_back(token.substr(start, end - start + 1));
        } else if (start != std::string::npos) {
            tokens.push_back(token.substr(start));
        }
    }

    return tokens;
}

// 経路文字列から経路を構築（build_route使用）
bool build_route_from_string(az_route& route, const std::string& route_str, bool json_mode) {
    std::string result = route.build_route(route_str);

    // build_route は JSON を返す: { "rc": 0, "failItem": "", "offset": 0 }
    // rc が 0 なら成功
    if (result.find("\"rc\": 0") == std::string::npos) {
        std::cerr << "Error: Failed to build route from string: " << route_str << std::endl;
        std::cerr << "Result: " << result << std::endl;
        return false;
    }

    // 結果表示
    if (json_mode) {
        std::cout << route.get_fare_info_object_json() << std::endl;
    } else {
        std::cout << route.show_fare() << std::endl;
    }

    return true;
}

// トークンリストから経路を構築（add_start_route/add_route使用）
bool build_route_from_tokens(az_route& route, const std::vector<std::string>& tokens, bool json_mode) {
    if (tokens.empty()) {
        std::cerr << "Error: No tokens provided" << std::endl;
        return false;
    }

    // 最初の駅を追加
    int result = route.add_start_route(tokens[0]);
    if (result < 0) {
        std::cerr << "Error: Failed to add start station: " << tokens[0] << std::endl;
        return false;
    }

    // 残りの路線と駅を追加（路線、駅のペア）
    for (size_t i = 1; i + 1 < tokens.size(); i += 2) {
        result = route.add_route(tokens[i], tokens[i + 1]);
        if (result < 0) {
            std::cerr << "Error: Failed to add route: " << tokens[i] << " -> " << tokens[i + 1] << std::endl;
            return false;
        }
    }

    // 結果表示
    if (json_mode) {
        std::cout << route.get_fare_info_object_json() << std::endl;
    } else {
        std::cout << route.show_fare() << std::endl;
    }

    return true;
}

// 自動経路検索（偶数個のトークン）
bool auto_route_search(az_route& route, const std::vector<std::string>& tokens, bool json_mode) {
    if (tokens.size() != 2) {
        std::cerr << "Error: Auto route search requires exactly 2 stations" << std::endl;
        return false;
    }

    // 出発駅を追加
    int result = route.add_start_route(tokens[0]);
    if (result < 0) {
        std::cerr << "Error: Failed to add start station: " << tokens[0] << std::endl;
        return false;
    }

    // 自動経路検索（新幹線使用: 1）
    result = route.auto_route(1, tokens[1]);
    if (result < 0) {
        std::cerr << "Error: Failed to auto route to: " << tokens[1] << std::endl;
        return false;
    }

    // 結果表示
    if (json_mode) {
        std::cout << route.get_fare_info_object_json() << std::endl;
    } else {
        std::cout << route.show_fare() << std::endl;
    }

    return true;
}

// ファイルから経路を処理
bool process_file(const char* filename, bool json_mode) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file: " << filename << std::endl;
        return false;
    }

    std::string line;
    int line_number = 0;
    bool success = true;

    while (std::getline(file, line)) {
        line_number++;

        // 空行やコメント行をスキップ
        if (line.empty() || line[0] == '#') {
            continue;
        }

        // 前後の空白を削除
        size_t start = line.find_first_not_of(" \t\r\n");
        size_t end = line.find_last_not_of(" \t\r\n");

        if (start == std::string::npos) {
            continue;
        }

        line = line.substr(start, end - start + 1);

        std::cout << "\n========================================" << std::endl;
        std::cout << "Line " << line_number << ": " << line << std::endl;
        std::cout << "========================================" << std::endl;

        // 経路を構築
        az_route route;

        // スペース区切りでトークンに分解
        std::vector<std::string> tokens = split(line, ' ');

        if (tokens.empty()) {
            continue;
        }

        // トークン数に応じて処理
        if (tokens.size() % 2 == 0) {
            // 偶数個: 自動経路検索
            if (!auto_route_search(route, tokens, json_mode)) {
                success = false;
            }
        } else {
            // 奇数個: 手動経路構築
            if (!build_route_from_tokens(route, tokens, json_mode)) {
                success = false;
            }
        }
    }

    file.close();
    return success;
}

int main(int argc, char** argv) {
    bool json_mode = false;
    char* filename = nullptr;
    int opt;

    // オプション解析
    while ((opt = getopt(argc, argv, "jf:h")) != -1) {
        switch (opt) {
        case 'j':
            json_mode = true;
            break;
        case 'f':
            filename = optarg;
            break;
        case 'h':
            usage(argv[0]);
            return 0;
        default:
            usage(argv[0]);
            return 1;
        }
    }

    // データベースオープン
    std::string db_result = open_database();
    // JSON出力モードでない場合のみデータベース情報を表示
    if (!json_mode) {
        std::cout << "Database: " << db_result << std::endl;
    }

    // データベースが開けているか確認
    if (db_result.find("\"result\": true") == std::string::npos) {
        std::cerr << "Error: Failed to open database" << std::endl;
        close_database();
        return 1;
    }

    bool success = false;

    // ファイル指定の場合
    if (filename != nullptr) {
        success = process_file(filename, json_mode);
    }
    // コマンドライン引数の場合
    else if (optind < argc) {
        az_route route;

        // 引数が1つで、カンマを含む場合
        if (argc - optind == 1 && strchr(argv[optind], ',') != nullptr) {
            // カンマで分割してトークンリストに変換
            std::vector<std::string> tokens = split(argv[optind], ',');

            // トークン数に応じて処理
            if (tokens.size() % 2 == 0) {
                // 偶数個: 自動経路検索
                success = auto_route_search(route, tokens, json_mode);
            } else {
                // 奇数個: 手動経路構築
                success = build_route_from_tokens(route, tokens, json_mode);
            }
        }
        // 引数が偶数個の場合は自動経路検索
        else if ((argc - optind) % 2 == 0) {
            std::vector<std::string> tokens;
            for (int i = optind; i < argc; i++) {
                tokens.push_back(argv[i]);
            }
            success = auto_route_search(route, tokens, json_mode);
        }
        // 引数が奇数個の場合は手動経路構築
        else {
            std::vector<std::string> tokens;
            for (int i = optind; i < argc; i++) {
                tokens.push_back(argv[i]);
            }
            success = build_route_from_tokens(route, tokens, json_mode);
        }
    }
    // 引数なしの場合
    else {
        std::cerr << "Error: No arguments provided" << std::endl;
        usage(argv[0]);
        close_database();
        return 1;
    }

    // データベースクローズ
    close_database();

    return success ? 0 : 1;
}
