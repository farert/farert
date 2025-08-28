# 🐳 Docker編: VSCode DevContainer環境構築

Farert GCOV開発環境をVSCode DevContainerで構築する手順です。

## 🚀 クイックスタート

### 1. 前提条件
- [Visual Studio Code](https://code.visualstudio.com/)
- [Docker Desktop](https://www.docker.com/products/docker-desktop/)
- VSCode拡張機能: [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers)

### 2. 環境立ち上げ
```bash
# 1. このディレクトリをVSCodeで開く
code .

# 2. コマンドパレット (Ctrl+Shift+P) から
# "Dev Containers: Reopen in Container"を実行

# 3. 自動的にDockerコンテナが構築され、開発環境が準備されます
```

## 📁 DevContainer設定ファイル

### `.devcontainer/devcontainer.json`
DevContainer全体の設定を管理：
```json
{
  "name": "Farert GCOV Development",
  "build": {
    "dockerfile": "Dockerfile"
  },
  "workspaceMount": "source=${localWorkspaceFolder}/../../../,target=/workspace,type=bind",
  "workspaceFolder": "/workspace/test/unix/gcov",
  "remoteEnv": {
    "farertDB": "/workspace/app/win_mfc/fjr_mfc/alps_mfc/res/jrdbnewest.db"
  }
}
```

**重要な設定**:
- プロジェクト全体をマウント（ソースコードとDBファイルへのアクセス）
- 環境変数`farertDB`を自動設定
- 必要なVSCode拡張機能を自動インストール

### `.devcontainer/Dockerfile`
開発環境の構築：
```dockerfile
FROM ubuntu:24.04

# 開発ツールのインストール
RUN apt-get update && apt-get install -y \
    build-essential \
    gcc g++ make cmake \
    gdb gcov lcov \
    git curl wget \
    sqlite3 libsqlite3-dev
```

### `.devcontainer/setup.sh`
初回起動時の自動セットアップスクリプト

## 🔧 自動インストールされるVSCode拡張機能

| 拡張機能 | 用途 |
|---------|------|
| **C/C++** | IntelliSense、コンパイル、デバッグ |
| **C/C++ Extension Pack** | C++開発の統合パック |
| **CMake Tools** | CMakeサポート |
| **Makefile Tools** | Makefileサポート |
| **Coverage Gutters** | エディタ内でカバレッジ表示 |
| **GCOV Viewer** | gcovファイルビューアー |

## ⚙️ コンテナ内の環境設定

### 環境変数
```bash
CC=gcc
CXX=g++
CFLAGS="-g -fprofile-arcs -ftest-coverage"
CXXFLAGS="-g -fprofile-arcs -ftest-coverage"
LDFLAGS="-lgcov"
farertDB="/workspace/app/win_mfc/fjr_mfc/alps_mfc/res/jrdbnewest.db"
```

### ディレクトリ構造
```
/workspace/                           # プロジェクトルート
├── test/unix/gcov/                  # 作業ディレクトリ
│   ├── .devcontainer/               # DevContainer設定
│   ├── .vscode/                     # VSCode設定
│   └── Makefile                     # ビルド設定
├── app/                             # アプリケーションソース
└── ...
```

## 🎯 VSCodeタスク

`Ctrl+Shift+P` → "Tasks: Run Task"で実行可能：

### ビルドタスク
- **Clean Build**: オブジェクトファイルをクリーン
- **Build with Coverage**: gcovフラグ付きでビルド

### テストタスク
- **Run Coverage Tests**: カバレッジ測定実行
- **Generate LCOV Report**: HTMLレポート生成
- **Show Coverage Summary**: 簡単な結果表示

## 🔍 デバッグ設定

`F5`キーまたはデバッグビューから実行：

### デバッグ構成
1. **Debug Test Executable**: 標準テスト実行のデバッグ
2. **Debug Test with Custom Route**: カスタム路線でのデバッグ

### デバッグ機能
- ブレークポイント設定
- 変数監視
- スタックトレース
- ステップ実行

## 💡 Tips & トラブルシューティング

### 🔄 コンテナの再構築
設定を変更した場合：
```bash
# コマンドパレット (Ctrl+Shift+P) から
"Dev Containers: Rebuild Container"
```

### 📦 依存関係の問題
Dockerfileに追加パッケージを追加：
```dockerfile
RUN apt-get install -y [パッケージ名]
```

### 🔒 権限問題
コンテナ内でのファイル権限を確認：
```bash
ls -la /workspace/test/unix/gcov/
```

### 🖥️ ポート転送
Webサーバーを起動する場合：
```json
"forwardPorts": [3000, 8080]
```

### 💾 永続化ボリューム
データを永続化したい場合：
```json
"mounts": [
  "source=project-data,target=/data,type=volume"
]
```

## 🔧 高度な設定

### カスタムDockerfile
特別な要件がある場合：
```dockerfile
# 独自のベースイメージを使用
FROM ubuntu:22.04

# 特定のgccバージョンを指定
RUN apt-get install -y gcc-11 g++-11
```

### 複数環境対応
```json
"build": {
  "dockerfile": "Dockerfile",
  "args": {
    "VARIANT": "ubuntu-22.04"
  }
}
```

## 📋 次のステップ

DevContainer環境が準備できたら、[カバレッジ編](README-Coverage.md)でコードカバレッジの測定方法を学習しましょう！