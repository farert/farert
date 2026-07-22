#!/usr/bin/env bash
# Farert 経路エンジンの検証ハーネスをビルドし、主要スイープを実行する。
# 使い方: リポジトリルート(または任意)から `bash <this>`  ; 引数 --quick で軽量スイープのみ。
set -u

# --- リポジトリルート特定(このスクリプトの4階層上) ---
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../../../.." && pwd)"
ALL_DIR="$REPO_ROOT/test/unix/all"
REPRO_DIR="$ALL_DIR/removetail-151-repro"
DB="$REPO_ROOT/db/jrdbNewest.db"

if [ ! -f "$REPRO_DIR/repro.cpp" ]; then
  echo "error: $REPRO_DIR/repro.cpp が見つかりません" >&2
  exit 1
fi
if [ ! -f "$DB" ]; then
  echo "error: DB $DB が見つかりません" >&2
  exit 1
fi

cd "$ALL_DIR" || exit 1

echo "== 1) make (farert 本体 + オブジェクト生成) =="
make >/tmp/farert_make.log 2>&1 || { echo "make 失敗。/tmp/farert_make.log を参照"; tail -5 /tmp/farert_make.log; exit 1; }

echo "== 2) repro をリンク =="
g++ -Wall -D_DEBUG -D_CONSOLE_TEST -g -O0 \
  -I. -I../common -I../../../app/win_mfc/fjr_mfc/lib/db -I../../../app/alps \
  -std=c++0x removetail-151-repro/repro.cpp alpdb.o db.o sqlite3.o ../common/stdafx.o \
  -o removetail-151-repro/repro -lpthread -ldl \
  || { echo "repro リンク失敗"; exit 1; }

export farertDB="$(cd "$(dirname "$DB")" && pwd)/$(basename "$DB")"
R="$REPRO_DIR/repro"

echo
echo "== 3) 会社線サニティ(修正が効いていれば OK rc1=rc2) =="
printf '\n' | "$R" 金沢 七尾線 津幡 IRいしかわ 倶利伽羅 </dev/null 2>/dev/null | grep -E 'OK|NG' || echo "(出力なし=前提区間で早期skip)"

echo
echo "== 4) 本来の通過連絡運輸(回帰、OK 期待) =="
printf '\n' | "$R" 直江津 "えちごトキめき鉄道（日本海ひすい）" 市振 あいの風とやま 高岡 </dev/null 2>/dev/null | grep -E 'OK|NG'

if [ "${1:-}" = "--quick" ]; then
  echo; echo "--quick 指定のため総当たり/ファジングはスキップ"; exit 0
fi

echo
echo "== 5) brute 総当たり(ASSERT発火が無いこと) =="
"$R" -brute </dev/null >/tmp/brute.out 2>/tmp/brute.err
echo "  $(tail -1 /tmp/brute.out)"
echo "  ASSERT発火件数: $(grep -c 'Assertion error' /tmp/brute.err)"
echo "  会社線NG(同一視/折りたたみ以外があれば要確認):"
grep -A3 'NG\*\*' /tmp/brute.out | grep -cE 'IRいしかわ|ハピライン|あいの風|えちごトキ|しなの鉄道|松浦|トキめき' \
  | sed 's/^/    company-related NG: /'

echo
echo "== 6) 実経路コーパス undo(NGUNDO は同一視のみが期待) =="
"$R" -undo "$REPRO_DIR/routes.txt" </dev/null >/tmp/undo.out 2>/tmp/undo.err
echo "  $(tail -1 /tmp/undo.out)"
echo "  undo中ASSERT発火件数: $(grep -c 'Assertion error' /tmp/undo.err)"

echo
echo "== 7) Java 回帰(任意, javatest.md: Java -exec と C++ test_result.txt の一致) =="
if [ -x "$REPO_ROOT/test/run_java_regression.sh" ]; then
  if "$REPO_ROOT/test/run_java_regression.sh" >/tmp/java_reg.out 2>&1; then
    echo "  Java 回帰: 一致(PASS)"
  else
    echo "  Java 回帰: 不一致 or 実行失敗(/tmp/java_reg.out を参照)"
    tail -5 /tmp/java_reg.out | sed 's/^/    /'
  fi
else
  echo "  run_java_regression.sh なし。スキップ(javatest.md 参照)"
fi

echo
echo "完了。詳細は /tmp/brute.out /tmp/undo.out /tmp/java_reg.out を参照。"
echo "C++ 通常運賃の回帰は SKILL.md「回帰検証」の -exec 前後 diff(ベースラインは修正コミットの親)を実施のこと。"
