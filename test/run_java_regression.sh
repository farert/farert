#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
CLASS_DIR="$(mktemp -d "${TMPDIR:-/tmp}/farert-java-regression.XXXXXX")"

cleanup() {
  rm -rf "$CLASS_DIR"
}
trap cleanup EXIT

RESOURCE_DIR="$ROOT_DIR/app/Farert.android/app/src/test/resources"
CPP_RESULT="$ROOT_DIR/test/unix/all/test_result.txt"
JAVA_RESULT="$RESOURCE_DIR/test_result.txt"

SQLITE_JDBC_JAR="$RESOURCE_DIR/sqlite-jdbc.jar"
SLF4J_API_JAR="$RESOURCE_DIR/slf4j-api.jar"
SLF4J_NOP_JAR="$RESOURCE_DIR/slf4j-nop.jar"

for required in \
  "$ROOT_DIR/db/jrdbNewest.db" \
  "$CPP_RESULT" \
  "$SQLITE_JDBC_JAR" \
  "$SLF4J_API_JAR" \
  "$SLF4J_NOP_JAR"; do
  if [[ ! -f "$required" ]]; then
    echo "missing required file: $required" >&2
    exit 1
  fi
done

echo "[1/3] Java テスト用クラスをコンパイルします"
java_files=()
while IFS= read -r line; do
  java_files+=("$ROOT_DIR/$line")
done < <(
  cd "$ROOT_DIR" &&
  rg --files -g '*.java' app/Farert.android/app/src/main/java/org/sutezo/alps app/Farert.android/app/src/test/java |
  rg -v 'JavaRouteRegressionTest\.java$|SimpleCursor\.java$'
)

if [[ "${#java_files[@]}" -eq 0 ]]; then
  echo "no Java files found to compile" >&2
  exit 1
fi

javac -encoding UTF-8 -d "$CLASS_DIR" "${java_files[@]}"

echo "[2/3] JavaTestMain -exec を実行します"
java -cp "$CLASS_DIR:$SQLITE_JDBC_JAR:$SLF4J_API_JAR:$SLF4J_NOP_JAR" org.sutezo.alps.JavaTestMain -exec

echo "[3/3] C++ の test_result.txt と比較します"
diff -u "$CPP_RESULT" "$JAVA_RESULT"

echo "一致しました: $JAVA_RESULT"
