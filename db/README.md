# Farert Database

## 利用許諾 / End User License Agreement (EULA)

本データを営利目的および二次的著作物の作成に使用することを禁じます。
このデータは、個人的な学習および非営利の研究目的に限り、自由にご利用いただけます。
ただし、以下の行為を禁止します。
- データの全部または一部を複製し、第三者に配布、公開（転載）すること。
- 営利目的で利用すること。 
- データを改変し、独自の成果物として発表・公開すること。

Copyright (C) 2025 Sutezo. All Rights Reserved. 
Do not reproduce or redistribute without permission. 
Use for commercial purposes or for creating derivative works is prohibited.

---

経路運賃営業キロ計算アプリ Farert データベース


## 概要

このプロジェクトは、Farertアプリケーション用のJR路線・運賃データベースを管理・作成するためのツールセットです。GoogleスプレッドシートをデータソースとしてSQLiteデータベースを生成し、Windows、iOS、Android各プラットフォーム向けにデータベースファイルを配布します。

## 必要環境

- macOS / Linux / Windows (WSL)
- Python 3
- curl
- GNU Make
- bash または zsh

## データベース作成方法

### 1. Googleスプレッドシートの準備

以下のGoogleスプレッドシートを共有設定で「リンクを知っているすべてのユーザーが閲覧可能」にしてください。

https://docs.google.com/spreadsheets/d/1tVIkbCGtK4HGdeOli3dYZ4K__0h4ySCMl0XaTuKrHmY

### 2. データベース作成

```bash
make update
```

このコマンドにより以下の処理が実行されます：

1. Googleスプレッドシートからデータをダウンロード
2. SQLiteデータベースファイルを生成
3. 各開発環境にデータベースファイルをコピー

### 3. 部分的な更新

特定の処理のみを実行したい場合：

```bash
# ダウンロードからやり直す場合
make update

# データベース生成以降のみ実行する場合
make
```

## ファイル構成

### 主要ファイル

- `Makefile` - データベース生成メイクファイル
- `How-To-Makeing-db.txt` - データベース作成手順（詳細版）
- `down_spr_as_text.sh` - Googleスプレッドシートダウンロードスクリプト
- `SPREADSHEET` - スプレッドシートURL・シートID設定

### 生成ファイル

- `jrdb*.db` - SQLiteデータベースファイル
- `jrdb*.txt` - 中間テキストファイル
- `*.tmp` - ダウンロード一時ファイル

### ディレクトリ

- `ddm/` - データベーススキーマ（参考用）
- `scripts/` - データベース生成・処理スクリプト
- `misc/` - その他参考ファイル・テストデータ

## 対象プラットフォーム

### iOS
- 配布先: `../app/Farert.ios/Farert/`
- ファイル形式: `.db` ファイル

### Android
- 配布先: `../app/Farert.android/app/src/main/assets/routeDB/jrdb.dat`
- ファイル形式: ZIP圧縮ファイル

### Windows
- 配布先: `../app/win_mfc/fjr_mfc/alps_mfc/res/`
- ファイル形式: `.db` ファイル

## データベース年版

- `jrdb2014.db` - 2014年版
- `jrdb2015.db` - 2015年版
- `jrdb2017.db` - 2017年版
- `jrdbNewest.db` - 最新版

## 注意事項

- スプレッドシートのURLを変更する場合は `SPREADSHEET` ファイルを編集してください
- データベース作成でエラーが発生した場合は、`*.db` ファイルを削除してから再実行してください
- 各プラットフォームでのDB名変更は別途手動で行う必要があります

## ファイル一覧

- `How-To-Makeing-db.txt` - データベース作成手順（詳細版）
- `Makefile` - データベース生成メイクファイル
- `dbtest_start.bat` - Windows環境でのDB利用環境セットアップ
- `ddm/` - データベーススキーマ（参考用）
- `down_spr_as_text.sh` - Googleスプレッドシートダウンロードスクリプト（Makefileで使用）
- `jr_db_reg.jse` - 未使用（Windows環境用）
- `jrdb.xlsx` - DBソース（2021.4以前に使用、現在は使用せず）
  - 現在はGoogleスプレッドシートを使用
- `jrdb2014.db` / `jrdb2014.txt` - 2014年版データベース
- `jrdb2015.db` / `jrdb2015.txt` - 2015年版データベース
- `jrdb2017.db` / `jrdb2017.txt` - 2017年版データベース
- `jrdb2019.db` / `jrdb2019.txt` - 2019年版データベース
- `mkdb-android.sh` - 未使用（Windows環境で使用する場合）
- `mkdb.bat` - 未使用（Windows環境用）
- `mkdball.bat` - 未使用（Windows環境用）
- `scripts/` - データベース生成・処理スクリプト
- `spr2tsv.gs` - 未使用
- `利用許諾.txt` - 利用許諾書

## ライセンス

詳細は `利用許諾.txt` および `EULA.txt` を参照してください。
