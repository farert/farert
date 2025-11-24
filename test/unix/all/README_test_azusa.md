# Azusa API 動作確認テスト

## 概要

`test_azusa.cpp` は、`app/alps/azusa.h` と `azusa.cpp` で定義されている全関数を網羅的にテストするサンプルコードです。

## ビルド方法

```bash
cd test/unix/all
make test_azusa
```

## 実行方法

```bash
# データベース環境変数を設定
source start.sh

# 実行
./test_azusa
```

## テスト内容

### 1. データベース操作
- `open_database()` - データベースのオープン
- `close_database()` - データベースのクローズ

### 2. fare_ui::マスターデータ取得
- `get_prefects()` - 都道府県一覧の取得
- `get_companys()` - JR会社一覧の取得
- `get_prefect_id()` - 都道府県IDの取得
- `get_company_id()` - JR会社IDの取得

### 3. fare_ui::路線データ取得
- `get_lines_by_prefect()` - 都道府県の路線一覧
- `get_lines_by_company()` - JR会社の路線一覧
- `get_lines_by_station()` - 駅の所属路線一覧

### 4. fare_ui::駅データ取得
- `get_stations_by_line()` - 路線の全駅一覧
- `get_stations_by_company_and_line()` - 会社と路線の駅一覧
- `get_stations_by_prefecture_and_line()` - 都道府県と路線の駅一覧
- `get_branch_stations_by_line()` - 路線の分岐駅一覧
- `get_prefecture_by_station()` - 駅の都道府県
- `get_kana_by_station()` - 駅名のかな
- `search_station_by_keyword()` - キーワードによる駅検索

### 5. az_route::基本的な経路作成
- `add_start_route()` - 出発駅の追加
- `add_route()` - 経路の追加（路線と駅）
- `get_route_count()` - 経路数の取得
- `departure_station_name()` - 出発駅名の取得
- `arriveval_station_name()` - 到着駅名の取得
- `route_script()` - 経路スクリプトの取得
- `get_routes_json()` - 経路のJSON配列取得
- `get_route_record()` - 個別経路レコードの取得

### 6. az_route::運賃計算
- `show_fare()` - 運賃の表示
- `get_fare_info_object_json()` - 運賃情報オブジェクトのJSON取得

### 7. az_route::フラグ設定
- `set_long_route()` - 長距離経路の設定
- `set_jr_tokai_stock_apply()` - JR東海株主優待券適用の設定
- `set_start_as_city()` - 出発を都市内として設定
- `set_arrival_as_city()` - 到着を都市内として設定
- `set_specific_term_rule115()` - 特定期間ルール115の設定
- `set_no_rule()` - ルールなしの設定
- `set_detour()` - 迂回の設定
- `set_not_same_kokura_hakata_shin_zai()` - 小倉博多新幹線/在来線同一でない設定

### 8. az_route::状態確認
- `is_not_same_kokura_hakata_shin_zai()` - 小倉博多新幹線/在来線同一チェック
- `is_available_reverse()` - 逆転可能かチェック
- `is_osakakan_detour_enable()` - 大阪環状線迂回可能かチェック
- `is_osakakan_detour()` - 大阪環状線迂回かチェック

### 9. az_route::経路操作
- `remove_tail()` - 最後の経路の削除
- `reverse()` - 経路の逆転
- `type_of_passed_line()` - 通過路線タイプの取得
- `remove_all()` - すべての経路の削除

### 10. az_route::自動経路検索
- `auto_route()` - 自動経路検索（新幹線使用/不使用の指定可能）

### 11. az_route::経路文字列からの構築
- `build_route()` - 経路文字列からの経路構築

### 12-13. 実践例
- Rule88適用テスト（新大阪→姫路）
- 長距離経路テスト（東京→大阪）

## 出力例

```
===========================================
  Azusa API 全関数動作確認テスト
===========================================

========================================
  1. データベース操作
========================================
[open_database()                         ] { "result": true, "dbName": "2025", "createdate": "2025-11-12 13:11:18" }

========================================
  2. fare_ui - マスターデータ取得
========================================
[get_prefects()                          ] "prefectures":["青森県","秋田県",...
[get_companys()                          ] "companies":["JR北海道","JR東日本","JR東海","JR西日本","JR九州","JR四国"]
...

========================================
  6. az_route - 運賃計算
========================================
[show_fare()                             ] 東京 -> 横浜
経由：[東海道線]
近郊区間内ですので最安運賃の経路にしました
営業キロ： 28.8 km
運賃(IC)： ¥490(¥480)
...
```

## 使用しているグローバル変数

- `int g_tax = 10` - 消費税率（10%）

## 注意事項

1. データベース環境変数 `farertDB` が設定されている必要があります（`start.sh` で設定）
2. データベースファイルは `test/unix/all/jrdbnewest.db` に配置されている必要があります
3. コンパイル時には以下のオブジェクトファイルが必要です：
   - `azusa.o`
   - `alpdb.o`
   - `db.o`
   - `sqlite3.o`
   - `stdafx.o`

## トラブルシューティング

### データベースが開けない場合

```bash
# 環境変数が設定されているか確認
echo $farertDB

# start.sh を実行
source start.sh

# データベースファイルが存在するか確認
ls -l jrdbnewest.db
```

### コンパイルエラーが出る場合

```bash
# クリーンビルドを試す
make clean
make test_azusa
```

## ライセンス

このテストコードは Farert プロジェクトの一部です。GPL v3 ライセンスに従います。
