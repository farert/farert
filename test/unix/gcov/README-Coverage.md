# 📊 カバレッジ編: gcov/lcovでコードカバレッジ測定

FarertプロジェクトのC++コードのカバレッジをgcovとlcovで測定する完全ガイドです。

## 🎯 カバレッジ測定の基本

### カバレッジとは？
**コードカバレッジ**は、テスト実行時にソースコードのどの部分が実行されたかを測定する指標です。

| カバレッジの種類 | 説明 |
|----------------|------|
| **行カバレッジ** | 実行された行の割合 |
| **分岐カバレッジ** | 条件分岐の実行割合 |
| **関数カバレッジ** | 実行された関数の割合 |

## 🚀 クイックスタート

### 基本的な測定手順
```bash
# 1. 環境変数を設定
export farertDB=../../../app/win_mfc/fjr_mfc/alps_mfc/res/jrdbnewest.db

# 2. gcovカバレッジ測定
make coverage

# 3. HTMLレポート生成（推奨）
make coverage-html

# 4. 結果確認
open coverage_html/index.html  # macOS
xdg-open coverage_html/index.html  # Linux
```

## 📁 出力ファイルの説明

### gcovファイル（`*.gcov`）
各ソースファイルのライン別実行回数情報：
```
        -:    0:Source:testmain.cpp
        -:    1:#include "stdafx.h"
        1:    2:int main() {
        3:    3:    printf("Hello");
        -:    4:    return 0;
    #####:    5:    unreachable_code();
```

**記号の意味**:
- `数字`: そのラインが実行された回数
- `-`: 実行不可能な行（コメント、宣言など）
- `#####`: 実行されていない行（要注意！）

### HTMLレポート（`coverage_html/`）
lcovが生成する視覚的なレポート：
```
coverage_html/
├── index.html          # プロジェクト概要
├── [ファイル名].cpp.gcov.html  # 各ファイルの詳細
└── gcov.css            # スタイル
```

### LCOVデータファイル（`coverage.info`）
lcovの中間ファイル（バイナリ形式）

## 🛠️ Makefileターゲット詳解

### `make coverage`
基本のgcovカバレッジ測定：
```makefile
coverage: test
	./test -exec           # テスト実行
	gcov *.cpp            # カバレッジ情報生成
```

### `make coverage-html`
HTMLレポート付きの完全測定：
```makefile
coverage-html: coverage
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/*' --output-file coverage.info    # システムファイル除外
	lcov --remove coverage.info '*/sqlite3.c' --output-file coverage.info  # 外部ライブラリ除外
	genhtml coverage.info --output-directory coverage_html
```

### `make clean`
全ての生成ファイルを削除：
```makefile
clean:
	rm -f *.o test *.gcda *.gcno *.gcov
	rm -rf coverage_html coverage.info
```

## 📊 カバレッジ結果の解析

### 良いカバレッジ指標
| カバレッジ | 評価 | 対応 |
|-----------|------|------|
| **80%以上** | 🟢 優秀 | 維持・改善 |
| **60-80%** | 🟡 普通 | テスト追加検討 |
| **60%未満** | 🔴 要改善 | テスト強化必須 |

### HTMLレポートの見方

#### 1. プロジェクト概要（index.html）
```
Summary Coverage Report
=======================
Lines:    1,234 of 2,000 (61.7%)
Functions: 456 of 600 (76.0%)
Branches:  789 of 1,200 (65.8%)
```

#### 2. ファイル別詳細
- 🟢 **緑色**: 実行された行
- 🔴 **赤色**: 実行されなかった行  
- ⚪ **白色**: 実行不可能な行

#### 3. 関数別カバレッジ
各関数の実行回数と未実行部分を確認

## 🔍 詳細解析のテクニック

### 1. 未実行コードの特定
```bash
# 未実行行のみ表示
grep "^\s*#####" *.gcov

# 未実行関数の特定
grep -B2 -A2 "^\s*#####" *.gcov
```

### 2. 実行回数の分析
```bash
# 最も実行回数の多い行
grep -E "^\s*[0-9]+" *.gcov | sort -nr

# ホットスポットの特定
grep -E "^\s*[1-9][0-9]{3,}" *.gcov
```

### 3. 複雑な条件分岐の確認
```bash
# if文の実行状況
grep -n "if\|else\|switch" *.gcov
```

## 🧪 テストケース改善

### カバレッジ向上のアプローチ

#### 1. エラーハンドリングのテスト
```cpp
// 対象：未テストのエラー処理
if (database_open_error) {
    return ERROR_DB_FAIL;  // ← この行が未実行
}
```

#### 2. 境界値テスト
```cpp
// 対象：境界条件
if (distance > MAX_DISTANCE) {  // ← MAX_DISTANCE付近をテスト
    return ERROR_TOO_FAR;
}
```

#### 3. 例外的なルート
```cpp
// 対象：まれにしか通らないパス
switch (station_type) {
    case STATION_SPECIAL:       // ← 特殊駅のテストケース追加
        handle_special_case();
        break;
}
```

## 🚨 トラブルシューティング

### データベース関連エラー
```bash
# エラー例
Can't db open
Database can't open(14)

# 解決方法
export farertDB=../../../app/win_mfc/fjr_mfc/alps_mfc/res/jrdbnewest.db
ls -la $farertDB  # ファイル存在確認
```

### 権限エラー
```bash
# 実行権限付与
chmod +x farert
chmod +x .devcontainer/setup.sh
```

### gcovファイルが生成されない
```bash
# コンパイラフラグ確認
grep "fprofile-arcs\|ftest-coverage" ../common/Makefile

# ビルドからやり直し
make clean && make
```

### lcovエラー
```bash
# lcovがインストールされていない場合
sudo apt-get install lcov

# DevContainer使用推奨
# または手動インストール
```

## 📈 継続的な改善

### 1. カバレッジ目標設定
```bash
# 現在のカバレッジ確認
grep "lines" coverage_html/index.html

# 目標：前回から+5%向上
```

### 2. 定期的な測定
```bash
# 週次カバレッジレポート生成
make coverage-html
cp coverage_html/index.html reports/coverage_$(date +%Y%m%d).html
```

### 3. 重要な機能の重点測定
- 運賃計算ロジック → 必須：90%以上
- ルート検索機能 → 推奨：80%以上
- エラーハンドリング → 最低：70%以上

## 🔧 高度な使用法

### カスタムテストケースの実行
```bash
# 特定のルートでテスト
./farert "東京 新宿 渋谷"

# デバッグ情報付き
./farert -exec  # 全テストケース実行
```

### 差分カバレッジ
```bash
# 前回との比較
lcov --diff coverage_old.info coverage.info --output-file diff.info
genhtml diff.info --output-directory coverage_diff
```

### フィルタリング
```bash
# 特定ディレクトリのみ
lcov --extract coverage.info "*/app/alps/*" --output-file alps_only.info

# 特定ファイルを除外
lcov --remove coverage.info "*/test/*" --output-file no_tests.info
```

## 📋 次のステップ

1. **基本測定**: `make coverage-html`でHTMLレポートを確認
2. **カバレッジ分析**: 未実行部分を特定
3. **テストケース追加**: 重要な未実行パスをカバー
4. **継続改善**: 定期的な測定で品質向上

詳しい環境構築は[Docker編](README-Docker.md)をご覧ください！