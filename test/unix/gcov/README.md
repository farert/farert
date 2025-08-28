# 📊 Farert GCOV Development Environment

FarertプロジェクトのC++コードカバレッジを測定するための開発環境です。

内容は、../all/ とおなじです。その環境にカバレッジを追加しただけのものとなります。

## 🎯 2つの使い方から選択

| 方法 | 推奨度 | 特徴 |
|------|--------|------|
| **[🐳 Docker編](README-Docker.md)** | ⭐⭐⭐ | 環境構築不要、VSCode統合 |
| **[📊 カバレッジ編](README-Coverage.md)** | ⭐⭐ | 詳細な測定・解析方法 |

## 🚀 クイックスタート

### 👑 推奨：DevContainer環境（全OS対応）
```bash
# 1. VSCodeでこのディレクトリを開く
code .  # Windows/Mac/Linux共通

# 2. DevContainer起動
# Ctrl+Shift+P (Windows/Linux) または Cmd+Shift+P (Mac)
# → "Dev Containers: Reopen in Container"

# 3. カバレッジ測定
make coverage-html

# 4. 結果確認
# Windows: start coverage_html/index.html
# Mac:     open coverage_html/index.html  
# Linux:   xdg-open coverage_html/index.html
```

## 🖥️ OS別：手動環境構築

DevContainerを使わない場合の環境構築方法：

### 🪟 Windows
```powershell
# 1. WSL2 + Ubuntu推奨
wsl --install Ubuntu

# 2. WSL内でツールインストール
sudo apt-get update
sudo apt-get install build-essential gcc g++ make gcov lcov

# 3. カバレッジ測定
$env:farertDB="../../../app/win_mfc/fjr_mfc/alps_mfc/res/jrdbnewest.db"
make coverage-html
start coverage_html/index.html
```

### 🍎 Mac
```bash
# 1. Homebrewでツールインストール
brew install gcc make lcov

# 2. カバレッジ測定
export farertDB=../../../app/win_mfc/fjr_mfc/alps_mfc/res/jrdbnewest.db
make coverage-html
open coverage_html/index.html
```

### 🐧 Linux (Ubuntu/Debian)
```bash
# 1. パッケージインストール
sudo apt-get update
sudo apt-get install build-essential gcc g++ make gcov lcov

# 2. カバレッジ測定
export farertDB=../../../app/win_mfc/fjr_mfc/alps_mfc/res/jrdbnewest.db
make coverage-html
xdg-open coverage_html/index.html
```

## 🎮 VSCodeタスク（全OS対応）

**ショートカット**:
- Windows/Linux: `Ctrl+Shift+P`
- Mac: `Cmd+Shift+P`

**利用可能なタスク**:
- **Build with Coverage**: gcovフラグ付きでビルド
- **Run Coverage Tests**: カバレッジ測定実行  
- **Generate LCOV Report**: HTMLレポート生成
- **Show Coverage Summary**: 結果サマリー表示

**デバッグ**: `F5`キーでテスト実行のデバッグが可能

## 📄 詳細情報

各詳細については、以下の専用ドキュメントをご覧ください：

### 🐳 [Docker編](README-Docker.md)
- DevContainer環境構築
- VSCode統合設定
- Dockerfile詳細
- トラブルシューティング

### 📊 [カバレッジ編](README-Coverage.md)
- カバレッジ測定の詳細解説
- HTMLレポートの見方
- 結果分析テクニック
- テストケース改善方法

## 🆘 よくある質問

### ❓ どちらの方法を選ぶべき？
- **初心者・環境構築が面倒**: 🐳 Docker編推奨
- **詳細な設定を理解したい**: 📊 カバレッジ編推奨
- **チーム開発**: DevContainer使用で環境統一

### ❓ OS固有の問題
| OS | 注意点 | 解決方法 |
|----|--------|----------|
| **Windows** | パス区切り文字 | WSL2使用推奨 |
| **Mac** | Xcodeツール必要 | `xcode-select --install` |
| **Linux** | 権限エラー | `sudo apt-get install` |

### ❓ 環境変数が効かない
```bash
# 現在の設定確認
echo $farertDB  # Linux/Mac
echo $env:farertDB  # Windows PowerShell

# ファイル存在確認（全OS共通）
ls -la ../../../app/win_mfc/fjr_mfc/alps_mfc/res/jrdbnewest.db
```

## 📋 次のステップ

1. **🐳 Docker編で環境構築** → [README-Docker.md](README-Docker.md)
2. **📊 カバレッジ測定開始** → [README-Coverage.md](README-Coverage.md)
3. **継続的な品質改善** → 定期的なカバレッジ測定
