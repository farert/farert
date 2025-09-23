#!/bin/bash

# DevContainer セットアップスクリプト

echo "Setting up Farert GCOV Development Environment..."

# 必要なファイルへのシンボリックリンクを作成
echo "Creating symbolic links to necessary files..."
ln -sf Makefile /workspace/test/unix/gcov/Makefile 2>/dev/null || echo "Warning: Could not link Makefile from ../all"
ln -sf ../all/testmain.cpp /workspace/test/unix/gcov/testmain.cpp 2>/dev/null || echo "Warning: Could not link testmain.cpp from ../all"

# 権限を設定
chmod +x /workspace/test/unix/gcov/farert 2>/dev/null || echo "farert binary not found (will be built)"

# データベースファイルの存在を確認
if [ -f "$farertDB" ]; then
    echo "Database file found: $farertDB"
else
    echo "Warning: Database file not found at $farertDB"
    echo "Make sure the database exists before running tests"
fi

# gcov設定の確認
echo "Checking gcov installation..."
gcov --version

# make環境の確認
echo "Checking build environment..."
cd /workspace/test/unix/gcov
make clean

echo ""
echo "=== Development Environment Ready ==="
echo "Database: $farertDB"
echo "To build and run coverage:"
echo "  make clean && make coverage"
echo ""
echo "To view coverage results:"
echo "  cat *.gcov | head -20"
echo "  or use VS Code Coverage Gutters extension"
echo "=================================="
