/**
 * test_azusa.cpp
 *
 * azusa.h/azusa.cpp の全関数を網羅した動作確認サンプルコード
 *
 * コンパイル方法:
 *   cd test/unix/all
 *   make test_azusa
 *   source start.sh
 *   ./test_azusa
 */

#include <iostream>
#include <iomanip>
#include "azusa.h"

// グローバル変数（税率: 10%）
int g_tax = 10;

// テスト結果の表示用マクロ
#define TEST_SECTION(name) \
    std::cout << "\n========================================\n" \
              << "  " << name << "\n" \
              << "========================================\n"

#define TEST_RESULT(name, result) \
    std::cout << "[" << std::setw(40) << std::left << name << "] " \
              << result << std::endl

#define TEST_INT(name, value) \
    std::cout << "[" << std::setw(40) << std::left << name << "] " \
              << value << std::endl

#define TEST_BOOL(name, value) \
    std::cout << "[" << std::setw(40) << std::left << name << "] " \
              << (value ? "true" : "false") << std::endl

int main() {
    std::cout << "===========================================\n"
              << "  Azusa API 全関数動作確認テスト\n"
              << "===========================================\n";

    // ========================================
    // 1. データベースのオープン/クローズ
    // ========================================
    TEST_SECTION("1. データベース操作");

    std::string db_result = open_database();
    TEST_RESULT("open_database()", db_result);

    // ========================================
    // 2. fare_ui 名前空間の関数テスト
    // ========================================
    TEST_SECTION("2. fare_ui - マスターデータ取得");

    std::string db_info = database_info();
    TEST_RESULT("database_info()", db_info);

    // 2-1. 都道府県一覧
    std::string prefects = fare_ui::get_prefects();
    TEST_RESULT("get_prefects()", prefects.substr(0, 100) + "...");

    // 2-2. JR会社一覧
    std::string companies = fare_ui::get_companys();
    TEST_RESULT("get_companys()", companies);

    // ========================================
    // 3. fare_ui - 路線データ取得
    // ========================================
    TEST_SECTION("3. fare_ui - 路線データ取得");

    // 3-1. 都道府県の路線一覧
    std::string lines_tokyo = fare_ui::get_lines_by_prefect("静岡県");
    TEST_RESULT("get_lines_by_prefect('静岡県')", lines_tokyo.substr(0, 100) + "...");

    // 3-2. JR会社の路線一覧
    std::string lines_jre = fare_ui::get_lines_by_company("JR東日本");
    TEST_RESULT("get_lines_by_company('JR東日本')", lines_jre.substr(0, 100) + "...");

    // 3-3. 駅の所属路線
    std::string lines_tokyo_st = fare_ui::get_lines_by_station("東京");
    TEST_RESULT("get_lines_by_station('東京')", lines_tokyo_st);

    // ========================================
    // 4. fare_ui - 駅データ取得
    // ========================================
    TEST_SECTION("4. fare_ui - 駅データ取得");

    // 4-1. 路線の全駅一覧
    std::string stations_tokaido = fare_ui::get_stations_by_line("東海道線");
    TEST_RESULT("get_stations_by_line('東海道線')", stations_tokaido.substr(0, 100) + "...");

    // 4-2. 会社と路線の駅一覧
    std::string stations_jre_tokaido = fare_ui::get_stations_by_company_and_line("JR東日本", "東海道線");
    TEST_RESULT("get_stations_by_company_and_line()", stations_jre_tokaido.substr(0, 100) + "...");

    // 4-3. 都道府県と路線の駅一覧
    std::string stations_tokyo_yamanote = fare_ui::get_stations_by_prefecture_and_line("東京都", "山手線");
    TEST_RESULT("get_stations_by_prefecture_and_line()", stations_tokyo_yamanote.substr(0, 100) + "...");

    // 4-4. 分岐駅一覧
    std::string branch_stations = fare_ui::get_branch_stations_by_line("東海道線", "東京");
    TEST_RESULT("get_branch_stations_by_line()", branch_stations);

    // 4-5. 駅の都道府県
    std::string prefecture = fare_ui::get_prefecture_by_station("東京");
    TEST_RESULT("get_prefecture_by_station('東京')", prefecture);

    // 4-6. 駅名のかな
    std::string kana = fare_ui::get_kana_by_station("東京");
    TEST_RESULT("get_kana_by_station('東京')", kana);

    // 4-7. 駅の検索
    std::string search_result = fare_ui::search_station_by_keyword("新宿");
    TEST_RESULT("search_station_by_keyword('新宿')", search_result);

    // 4-8. 駅のあいまい検索
    std::string fuzzy_result_ochanomizu = fare_ui::search_station_fuzzy("おち ゃの水", 20);
    TEST_RESULT("search_station_fuzzy('おち ゃの水',20)", fuzzy_result_ochanomizu);

    std::string fuzzy_result_ryugasaki = fare_ui::search_station_fuzzy("竜が崎", 20);
    TEST_RESULT("search_station_fuzzy('竜が崎',20)", fuzzy_result_ryugasaki);

    std::string fuzzy_result_yokokawa = fare_ui::search_station_fuzzy("横川(陽)", 20);
    TEST_RESULT("search_station_fuzzy('横川(陽)',20)", fuzzy_result_yokokawa);

    std::string fuzzy_result_futa = fare_ui::search_station_fuzzy("冨", 20);
    TEST_RESULT("search_station_fuzzy('冨',20)", fuzzy_result_futa);

    // ========================================
    // 5. az_route - 基本的な経路作成
    // ========================================
    TEST_SECTION("5. az_route - 基本的な経路作成");

    az_route route1;

    // 5-1. 出発駅追加
    int result = route1.add_start_route("東京");
    TEST_INT("add_start_route('東京')", result);

    // 5-2. 経路追加
    result = route1.add_route("東海道線", "品川");
    TEST_INT("add_route('東海道線', '品川')", result);

    result = route1.add_route("山手線", "新宿");
    TEST_INT("add_route('山手線', '新宿')", result);

    // 5-3. 経路数取得
    TEST_INT("get_route_count()", route1.get_route_count());

    // 5-4. 出発駅名・到着駅名取得
    TEST_RESULT("departure_station_name()", route1.departure_station_name());
    TEST_RESULT("arriveval_station_name()", route1.arriveval_station_name());

    // 5-5. 経路スクリプト取得
    std::string script = route1.route_script();
    TEST_RESULT("route_script()", script);

    // 5-6. 経路をJSON配列で取得
    std::string routes_json = route1.get_routes_json();
    TEST_RESULT("get_routes_json()", routes_json);

    // 5-7. index番目の経路レコード取得
    if (route1.get_route_count() > 0) {
        std::string record = route1.get_route_record(0);
        TEST_RESULT("get_route_record(0)", record);
    }

    // ========================================
    // 6. az_route - 運賃計算
    // ========================================
    TEST_SECTION("6. az_route - 運賃計算");

    // 6-1. 運賃表示
    std::string fare_display = route1.show_fare();
    TEST_RESULT("show_fare()", fare_display.substr(0, 200) + "...");

    // 6-2. 運賃情報オブジェクトJSON取得
    std::string fare_json = route1.get_fare_info_object_json();
    TEST_RESULT("get_fare_info_object_json()", fare_json.substr(0, 200) + "...");

    // ========================================
    // 7. az_route - フラグ設定
    // ========================================
    TEST_SECTION("7. az_route - フラグ設定");
#if 0
    route1.set_long_route(true);
    TEST_RESULT("set_long_route(true)", "設定完了");

    route1.set_jr_tokai_stock_apply(true);
    TEST_RESULT("set_jr_tokai_stock_apply(true)", "設定完了");

    route1.set_start_as_city();
    TEST_RESULT("set_start_as_city()", "設定完了");

    route1.set_arrival_as_city();
    TEST_RESULT("set_arrival_as_city()", "設定完了");

    route1.set_specific_term_rule115(true);
    TEST_RESULT("set_specific_term_rule115(true)", "設定完了");

    route1.set_no_rule(false);
    TEST_RESULT("set_no_rule(false)", "設定完了");

    route1.set_detour(true);
    TEST_INT("set_detour(true)", result);

    route1.set_not_same_kokura_hakata_shin_zai(true);
    TEST_RESULT("set_not_same_kokura_hakata_shin_zai(true)", "設定完了");
#endif
    // ========================================
    // 8. az_route - 状態確認
    // ========================================
    TEST_SECTION("8. az_route - 状態確認");

    TEST_BOOL("is_not_same_kokura_hakata_shin_zai()", route1.is_not_same_kokura_hakata_shin_zai());
    TEST_BOOL("is_available_reverse()", route1.is_available_reverse());
    TEST_BOOL("is_osakakan_detour_enable()", route1.is_osakakan_detour_enable());
    TEST_BOOL("is_osakakan_detour()", route1.is_osakakan_detour());

    // ========================================
    // 9. az_route - 経路操作
    // ========================================
    TEST_SECTION("9. az_route - 経路操作");

    // 9-1. 最後の経路を削除
    route1.remove_tail();
    TEST_INT("remove_tail() 後の経路数", route1.get_route_count());

    // 9-2. 経路の逆転
    int reverse_result = route1.reverse();
    TEST_INT("reverse()", reverse_result);
    TEST_RESULT("reverse() 後の出発駅", route1.departure_station_name());
    TEST_RESULT("reverse() 後の到着駅", route1.arriveval_station_name());

    // 9-3. 通過路線タイプ取得
    if (route1.get_route_count() > 0) {
        TEST_INT("type_of_passed_line(0)", route1.type_of_passed_line(0));
    }

    // ========================================
    // 9.5. az_route - assign() テスト
    // ========================================
    TEST_SECTION("9.5. az_route - assign() で経路をコピー");

    // 元の経路を作成（東京→品川→横浜）
    az_route source_route;
    source_route.add_start_route("東京");
    source_route.add_route("東海道線", "品川");
    source_route.add_route("東海道線", "横浜");

    TEST_INT("元の経路数", source_route.get_route_count());
    TEST_RESULT("元の出発駅", source_route.departure_station_name());
    TEST_RESULT("元の到着駅", source_route.arriveval_station_name());
    TEST_RESULT("元の経路スクリプト", source_route.route_script());

    // 新しい経路オブジェクトに assign() でコピー
    az_route dest_route;
    dest_route.assign(source_route, source_route.get_route_count());

    TEST_INT("コピー後の経路数", dest_route.get_route_count());
    TEST_RESULT("コピー後の出発駅", dest_route.departure_station_name());
    TEST_RESULT("コピー後の到着駅", dest_route.arriveval_station_name());
    TEST_RESULT("コピー後の経路スクリプト", dest_route.route_script());

    // コピーされた経路の運賃計算も確認
    std::string copied_fare = dest_route.show_fare();
    TEST_RESULT("コピーした経路の運賃", copied_fare.substr(0, 150) + "...");

    // ========================================
    // 9.6. az_route - assign(route, 3) テスト（部分コピー）
    // ========================================
    TEST_SECTION("9.6. az_route - assign(route, 3) で経路の一部をコピー");

    // 長い経路を作成（東京→新青森→大館→好摩→目時）
    az_route long_route;
    long_route.add_start_route("東京");
    long_route.add_route("東北新幹線", "新青森");
    long_route.add_route("奥羽線", "大館");
    long_route.add_route("花輪線", "好摩");
    long_route.add_route("IGRいわて銀河", "目時");

    TEST_INT("元の経路数（長い経路）", long_route.get_route_count());
    TEST_RESULT("元の出発駅", long_route.departure_station_name());
    TEST_RESULT("元の到着駅", long_route.arriveval_station_name());
    TEST_RESULT("元の経路スクリプト", long_route.route_script());

    // 最初の3駅分だけコピー（東京、新青森、大館）
    az_route partial_route;
    partial_route.assign(long_route, 3);

    TEST_INT("部分コピー後の経路数", partial_route.get_route_count());
    TEST_RESULT("部分コピー後の出発駅", partial_route.departure_station_name());
    TEST_RESULT("部分コピー後の到着駅", partial_route.arriveval_station_name());
    TEST_RESULT("部分コピー後の経路スクリプト", partial_route.route_script());

    // 部分コピーされた経路の運賃計算も確認
    std::string partial_fare = partial_route.show_fare();
    TEST_RESULT("部分コピーした経路の運賃", partial_fare.substr(0, 200) + "...");

    // ========================================
    // 10. az_route - 自動経路検索
    // ========================================
    TEST_SECTION("10. az_route - 自動経路検索");

    az_route route2;
    route2.add_start_route("東京");

    // 10-1. 新幹線を使用して大阪まで自動経路検索
    int auto_result = route2.auto_route(1, "新大阪");
    TEST_INT("auto_route(1, '新大阪')", auto_result);
    TEST_RESULT("自動検索後の経路", route2.route_script());

    // ========================================
    // 11. az_route - 経路文字列からの構築
    // ========================================
    TEST_SECTION("11. az_route - 経路文字列からの構築");

    az_route route3;
    std::string route_str = "直江津,信越線(直江津-新潟),犀潟,ほくほく線,十日町,飯山線,飯山,北陸新幹線,高崎,上越新幹線,大宮,東北新幹線,上野,東北線,秋葉原,総武線(錦糸町-御茶ノ水),御茶ノ水";
    std::string build_result = route3.build_route(route_str);
    TEST_RESULT("build_route(route_str)", build_result);
    TEST_RESULT("構築後の経路", route3.route_script());

    build_result = route3.build_route("加島,JR東西線,京橋,大阪環状線,大阪c,東海道線,神戸,山陽線,西明石,山陽新幹線,厚狭");
    TEST_RESULT("build_route(route_str)", build_result);
    TEST_RESULT("構築後の経路", route3.route_script());

    // ========================================
    // 11.5. add_route() と build_route() の同等性テスト（簡単な経路）
    // ========================================
    TEST_SECTION("11.5. add_route() と build_route() の運賃情報同等性テスト");

    // 方法1: add_start_route() + add_route() で構築
    // 経路: 武蔵小杉 → 府中本町 → 西船橋 → 東京 → 品川
    az_route route_by_add;
    route_by_add.add_start_route("武蔵小杉");
    route_by_add.add_route("南武線", "府中本町");
    route_by_add.add_route("武蔵野線", "西船橋");
    route_by_add.add_route("総武線", "東京");
    route_by_add.add_route("東海道新幹線", "品川");

    TEST_INT("add_route()で構築した経路数", route_by_add.get_route_count());
    TEST_RESULT("add_route()で構築した経路", route_by_add.route_script());

    // 方法2: build_route() で構築
    az_route route_by_build;
    std::string simple_route = "武蔵小杉,南武線,府中本町,武蔵野線,西船橋,総武線,東京,東海道新幹線,品川";
    std::string build_result2 = route_by_build.build_route(simple_route);
    TEST_RESULT("build_route()の結果", build_result2);
    TEST_INT("build_route()で構築した経路数", route_by_build.get_route_count());
    TEST_RESULT("build_route()で構築した経路", route_by_build.route_script());

    // 両方の方法で構築した経路の運賃情報JSONを取得
    std::string fare_json_by_add = route_by_add.get_fare_info_object_json();
    std::string fare_json_by_build = route_by_build.get_fare_info_object_json();

    // 比較結果を表示
    if (fare_json_by_add == fare_json_by_build) {
        TEST_RESULT("運賃情報JSON比較結果", "✓ 一致（両方の方法で同じ結果）");
    } else {
        TEST_RESULT("運賃情報JSON比較結果", "✗ 不一致（異なる結果）");
        TEST_RESULT("add_route()のJSON", fare_json_by_add.substr(0, 300) + "...");
        TEST_RESULT("build_route()のJSON", fare_json_by_build.substr(0, 300) + "...");
    }

    // 参考: 運賃表示も確認
    std::string fare_musako_shinagawa = route_by_add.show_fare();
    TEST_RESULT("武蔵小杉→品川の運賃", fare_musako_shinagawa.substr(0, 200) + "...");

    // ========================================
    // 11.6. build_route() スペース区切り + route_script() + assign() 照合テスト
    // ========================================
    TEST_SECTION("11.6. build_route()スペース区切り + route_script() + assign()照合");

    // 1. add_route() で基準経路を作成
    az_route reference_route;
    reference_route.add_start_route("武蔵小杉");
    reference_route.add_route("南武線", "府中本町");
    reference_route.add_route("武蔵野線", "西船橋");
    reference_route.add_route("総武線", "東京");
    reference_route.add_route("東海道新幹線", "品川");

    std::string ref_script = reference_route.route_script();
    TEST_RESULT("【基準】add_route()の経路", ref_script);

    // 2. build_route() でスペース区切り文字列から構築
    az_route space_separated_route;
    std::string space_route_str = "武蔵小杉 南武線 府中本町 武蔵野線 西船橋 総武線 東京 東海道新幹線 品川";
    std::string space_build_result = space_separated_route.build_route(space_route_str);
    TEST_RESULT("build_route(スペース区切り)結果", space_build_result);

    std::string space_script = space_separated_route.route_script();
    TEST_RESULT("build_route(スペース区切り)の経路", space_script);

    // 3. route_script() の照合
    if (ref_script == space_script) {
        TEST_RESULT("route_script()照合結果", "✓ 一致");
    } else {
        TEST_RESULT("route_script()照合結果", "✗ 不一致");
        TEST_RESULT("  基準", ref_script);
        TEST_RESULT("  スペース区切り", space_script);
    }

    // 4. assign() で経路をコピーして照合
    az_route assigned_route;
    assigned_route.assign(reference_route, reference_route.get_route_count());

    std::string assigned_script = assigned_route.route_script();
    TEST_RESULT("assign()後の経路", assigned_script);

    if (ref_script == assigned_script) {
        TEST_RESULT("assign()照合結果", "✓ 一致");
    } else {
        TEST_RESULT("assign()照合結果", "✗ 不一致");
    }

    // 5. 3つの方法すべてのroute_script()が一致するか確認
    if (ref_script == space_script && ref_script == assigned_script) {
        TEST_RESULT("【総合】全方法の照合結果", "✓ 完全一致（add_route/build_route/assign すべて同じ）");
    } else {
        TEST_RESULT("【総合】全方法の照合結果", "✗ 不一致あり");
    }

    // 6. 運賃情報JSONも照合
    std::string ref_fare_json = reference_route.get_fare_info_object_json();
    std::string space_fare_json = space_separated_route.get_fare_info_object_json();
    std::string assigned_fare_json = assigned_route.get_fare_info_object_json();

    bool all_json_match = (ref_fare_json == space_fare_json) && (ref_fare_json == assigned_fare_json);
    if (all_json_match) {
        TEST_RESULT("【運賃JSON】全方法の照合結果", "✓ 完全一致");
    } else {
        TEST_RESULT("【運賃JSON】全方法の照合結果", "✗ 不一致あり");
        if (ref_fare_json != space_fare_json) {
            TEST_RESULT("  add vs build", "不一致");
        }
        if (ref_fare_json != assigned_fare_json) {
            TEST_RESULT("  add vs assign", "不一致");
        }
    }

    // ========================================
    // 11.65. build_route() 省略・曖昧・大阪環状線遠回りの回帰テスト
    // ========================================
    TEST_SECTION("11.65. build_route() 回帰テスト");

    az_route regression_route_1;
    std::string regression_result_1 = regression_route_1.build_route("千歳 千歳線 白石 函館線 岩見沢 室蘭線 追分");
    TEST_RESULT("build_route(千歳→追分)", regression_result_1);
    TEST_RESULT("千歳→追分の経路", regression_route_1.route_script());

    az_route regression_route_2;
    std::string regression_result_2 = regression_route_2.build_route("長崎 西九州新幹線 諫早 長崎線 長与");
    TEST_RESULT("build_route(長崎→長与)", regression_result_2);
    TEST_RESULT("長崎→長与の経路", regression_route_2.route_script());

    az_route regression_route_3;
    std::string regression_result_3 = regression_route_3.build_route("大阪 r大阪環状線 京橋");
    TEST_RESULT("build_route(大阪→京橋 detour)", regression_result_3);
    TEST_RESULT("大阪→京橋 detour の経路", regression_route_3.route_script());

    az_route regression_route_4;
    std::string regression_result_4 = regression_route_4.build_route("長崎 西九州新幹線 諫早 長崎線 新鳥栖 九州新幹線 博多 山陽新幹線 新大阪 東海道線 大阪 r大阪環状線 京橋 片町線 木津");
    TEST_RESULT("build_route(長崎→木津 detour)", regression_result_4);
    TEST_RESULT("長崎→木津 detour の経路", regression_route_4.route_script());

    az_route regression_route_5;
    std::string regression_result_5 = regression_route_5.build_route("上越妙高 えちごトキめき鉄道（妙高はねうま） 直江津");
    TEST_RESULT("build_route(上越妙高→直江津 tokimeki)", regression_result_5);
    TEST_RESULT("上越妙高→直江津 tokimeki の経路", regression_route_5.route_script());

    az_route regression_route_6;
    std::string regression_result_6 = regression_route_6.build_route("直江津 えちごトキめき鉄道（日本海ひすい） 糸魚川");
    TEST_RESULT("build_route(直江津→糸魚川 tokimeki)", regression_result_6);
    TEST_RESULT("直江津→糸魚川 tokimeki の経路", regression_route_6.route_script());

    az_route regression_route_7;
    std::string regression_result_7 = regression_route_7.build_route("上越妙高　えちごトキめき鉄道（妙高はねうま）　直江津");
    TEST_RESULT("build_route(上越妙高→直江津 full-width spaces)", regression_result_7);
    TEST_RESULT("上越妙高→直江津 full-width spaces の経路", regression_route_7.route_script());

    az_route regression_route_8;
    std::string regression_result_8 = regression_route_8.build_route("上越妙高，えちごトキめき鉄道（妙高はねうま），直江津");
    TEST_RESULT("build_route(上越妙高→直江津 full-width commas)", regression_result_8);
    TEST_RESULT("上越妙高→直江津 full-width commas の経路", regression_route_8.route_script());

    // ========================================
    // 11.7. build_route() 失敗パターンのテスト
    // ========================================
    TEST_SECTION("11.7. build_route() 失敗パターンのテスト");

    // 経路: 蒲田→川崎→立川→八王子→倉賀野→高崎→新前橋→小山→友部→我孫子
    // この経路は途中でエラーになるはず
    az_route route_fail;
    std::string fail_route_str = "蒲田 東海道線 川崎 南武線 立川 中央東線 八王子 八高 倉賀野 高崎線 高崎 上越線 新前橋 両毛線 小山 水戸線 友部 常磐線 我孫子";
    std::string fail_build_result = route_fail.build_route(fail_route_str);
    TEST_RESULT("build_route(失敗パターン)結果", fail_build_result);

    // 失敗時の詳細情報を表示
    // build_route() は {"rc": エラーコード, "failItem": "失敗箇所", "offset": オフセット} を返す
    TEST_INT("失敗パターンの経路数", route_fail.get_route_count());
    if (route_fail.get_route_count() > 0) {
        TEST_RESULT("失敗パターンの出発駅", route_fail.departure_station_name());
        TEST_RESULT("失敗パターンの到着駅", route_fail.arriveval_station_name());
        TEST_RESULT("失敗パターンの経路スクリプト", route_fail.route_script());
    }

    // ========================================
    // 12. az_route - Rule88 適用例（新大阪-姫路）
    // ========================================
    TEST_SECTION("12. Rule88 適用テスト（新大阪→姫路）");

    az_route route4;
    route4.add_start_route("新大阪");
    route4.add_route("山陽新幹線", "姫路");

    std::string fare4 = route4.show_fare();
    TEST_RESULT("新大阪→姫路の運賃", fare4);

    std::string fare_json4 = route4.get_fare_info_object_json();
    TEST_RESULT("運賃JSON", fare_json4.substr(0, 300) + "...");

    // ========================================
    // 13. az_route - 長距離経路例（東京-大阪）
    // ========================================
    TEST_SECTION("13. 長距離経路テスト（東京→大阪）");

    az_route route5;
    route5.add_start_route("東京");
    route5.add_route("東海道線", "品川");
    route5.add_route("山手線", "新宿");
    route5.add_route("中央東線", "八王子");
    route5.add_route("横浜線", "東神奈川");
    route5.add_route("東海道線", "国府津");
    route5.add_route("御殿場線", "沼津");
    route5.add_route("東海道線", "豊橋");
    route5.add_route("飯田線", "辰野");

    std::string fare5 = route5.show_fare();
    TEST_RESULT("東京→辰野（在来線）の運賃", fare5);
    std::cout << route5.route_script() << endl;

    // ========================================
    // 14. az_route - すべての経路削除
    // ========================================
    TEST_SECTION("14. 経路の完全削除");

    route5.remove_all();
    TEST_INT("remove_all() 後の経路数", route5.get_route_count());

    // ========================================
    // 15. データベースのクローズ
    // ========================================
    TEST_SECTION("15. データベースクローズ");

    close_database();
    TEST_RESULT("close_database()", "実行完了");

    // ========================================
    // テスト完了
    // ========================================
    std::cout << "\n===========================================\n"
              << "  すべてのテストが完了しました\n"
              << "===========================================\n";

    return 0;
}
