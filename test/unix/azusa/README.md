# Azusa API CLIツール

## 概要

`farecli` は、azusa API を使用した運賃計算コマンドラインツールです。

## ビルド方法

```bash
cd test/unix/azusa
make
```

## 実行方法

### データベース環境変数の設定

```bash
source ../all/start.sh
```

### 使い方

```bash
./farecli [options] <arguments>
```

## オプション

- `-f <filename>` - ファイルから経路を読み込み（1行1経路）
- `-j` - JSON形式で運賃情報を出力
- `-h` - ヘルプを表示

## 使用例

### 1. カンマ区切りの経路文字列

```bash
./farecli "東京,東海道線,品川,東海道線,横浜"
```

**出力例:**
```
東京 -> 横浜
経由：[東海道線]
近郊区間内ですので最安運賃の経路にしました(途中下車不可、有効日数当日限り)
営業キロ： 28.8 km
運賃(IC)： ¥490(¥483)    往復： ¥980(¥966)
```

### 2. 駅名と路線名を個別指定（奇数個）

```bash
./farecli 東京 東海道線 品川 山手線 新宿 中央東線 甲府 身延線 国母
```

この形式でも、内部ではトークン列をスペース結合して `az_route::build_route()` に渡し、共通の経路パーサで構築します。

### 3. 自動経路検索（偶数個）

```bash
./farecli 東京 新大阪
```

偶数個の引数を指定すると、内部では `az_route::build_route()` を経由し、最後の単独駅を `auto_route()` 相当として解釈します（新幹線使用）。

### 4. JSON出力

```bash
./farecli -j 東京 東海道線 横浜
```

**出力例:**
```json
{
  "fareResultCode": 0,
  "isMeihanCityStartTerminalEnable": false,
  "isRuleApplied": true,
  "totalSalesKm": 28.8,
  "fare": 490,
  "fareForIC": 483,
  "childFare": 240,
  ...
}
```

### 5. ファイルから経路を読み込み

経路ファイル（例: `sample_routes.txt`）:
```
# サンプル経路ファイル
# # で始まる行はコメント

# 東京-横浜
東京 東海道線 横浜

# 新大阪-姫路（Rule88適用テスト）
新大阪 山陽新幹線 姫路

# 東京-名古屋
東京 東海道新幹線 名古屋
```

実行:
```bash
./farecli -f sample_routes.txt
```

ファイル内の各行が順次処理され、それぞれの運賃が表示されます。

## 引数の処理ロジック

### 1. 1トークン + カンマ区切り

引数が1つで、カンマを含む場合:
- `az_route::build_route()` を使用して経路を構築

```bash
./farecli "東京,東海道線,横浜"
```

### 2. 奇数個のトークン

引数が奇数個の場合:
- トークン列をスペース区切りの経路文字列として `az_route::build_route()` に渡す
- `setup_route()` 側で、出発駅 + 路線/駅ペアの経路として解釈する

```bash
./farecli 東京 東海道線 品川 山手線 新宿
#        ^^^^ ^^^^^^^ ^^^ ^^^^^ ^^^^
#        出発  路線1    駅1 路線2  駅2
```

### 3. 偶数個のトークン

引数が偶数個の場合:
- トークン列をスペース区切りの経路文字列として `az_route::build_route()` に渡す
- `setup_route()` 側で、最後の単独駅を `auto_route()` 相当として解釈する

```bash
./farecli 東京 新大阪
#        ^^^^ ^^^^^^^
#        出発  到着（自動検索）
```

## 出力形式

### 通常出力

`az_route::show_fare()` を使用して、人間が読みやすい形式で運賃情報を表示:

```
東京 -> 横浜
経由：[東海道線]
営業キロ： 28.8 km
運賃(IC)： ¥490(¥483)
小児運賃： ¥240
有効日数： 1日
```

### JSON出力（`-j` オプション）

`az_route::get_fare_info_object_json()` を使用して、構造化されたJSON形式で出力:

```json
{
  "fareResultCode": 0,
  "totalSalesKm": 28.8,
  "fare": 490,
  "fareForIC": 483,
  "childFare": 240,
  "ticketAvailDays": 1,
  "routeList": "[東海道線]"
}
```

## エラーハンドリング

- データベースが開けない場合: エラーメッセージを表示して終了
- 経路構築に失敗した場合: エラーメッセージとビルド結果を表示
- ファイルが開けない場合: エラーメッセージを表示

## 使用しているazusa API

### データベース操作
- `open_database()` - データベースのオープン
- `close_database()` - データベースのクローズ

### 経路構築
- `az_route::build_route(const std::string& route_str)` - 経路文字列からの構築（`farecli` の全入力形式で使用）
- `az_route::add_start_route(std::string station)` - 出発駅の追加
- `az_route::add_route(std::string line, std::string station)` - 経路の追加
- `az_route::auto_route(int useBulletTrain, std::string destinationStation)` - 自動経路検索

### 運賃計算・表示
- `az_route::show_fare()` - 人間が読みやすい形式で運賃表示
- `az_route::get_fare_info_object_json()` - JSON形式で運賃情報取得

## ファイル構成

- `farecli.cpp` - メインプログラム
- `Makefile` - ビルド設定
- `sample_routes.txt` - サンプル経路ファイル
- `README.md` - このファイル

## 注意事項

1. データベース環境変数 `farertDB` が設定されている必要があります
2. `source ../all/start.sh` でデータベースパスを設定してください
3. グローバル変数 `g_tax = 10` で消費税率10%を設定しています

## ライセンス

このプログラムは Farert プロジェクトの一部です。GPL v3 ライセンスに従います。
