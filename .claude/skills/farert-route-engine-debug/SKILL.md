---
name: farert-route-engine-debug
description: >
  Farert 鉄道運賃計算エンジン(app/alps/alpdb.cpp)の経路構築・運賃計算まわりの
  デバッグと回帰検証を行うスキル。add() / removeTail() / setup_route() の状態整合、
  会社線通過連絡運輸(compncheck/compnpass 等)、分岐特例(151条 段差型/水平型)、
  新幹線在来線同一視、大阪環状線ループなど、経路エンジンの不具合調査・修正・回帰確認・
  他プラットフォーム移植を行うときは必ずこのスキルを使うこと。alpdb.cpp / RouteFlag /
  companyPassCheck / 通過連絡運輸 / 分岐特例 / removeTail / 重複エラー といった語が出たら
  たとえ明示的に「スキルを使って」と言われなくても発動する。検証ハーネス
  test/unix/all/removetail-151-repro/ の使い方、C++回帰(farert -exec)と Java回帰
  (javatest.md / run_java_regression.sh)の手順、既知バグ分類(同一視・折りたたみは
  正しい挙動、会社線b枝ASSERTは C++/Java とも修正済み)を含む。
---

# Farert 経路エンジン デバッグ・検証スキル

`app/alps/alpdb.cpp`(約12,400行の C++ 経路・運賃計算エンジン、`org.sutezo.alps` 相当)の
経路構築・運賃計算の不具合を調査し、安全に修正・回帰確認するためのスキル。
sonnet/opus など別セッションが本作業を引き継ぐための知識と手順を凝縮している。

このエンジンは Windows(MFC)・iOS・CLI が共有し、Android は同ロジックの Java 版を持つ。
**運賃(金額)に直結する共有コードなので、変更は最小・外科的に行い、必ず回帰差分を取ること。**

## いつ何をするか(最初に読む)

1. まず「本当にバグか」を疑う。JR 運賃規則には、直感に反するが**正しい**挙動が多い。
   下記「バグ分類」で、報告された症状が既知の正しい挙動(同一視・折りたたみ)でないか確認する。
2. 症状を**検証ハーネス**(`test/unix/all/removetail-151-repro/`)で機械的に再現・分類する。
   単発・総当たり・ファジング・実経路コーパスの4系統がある。
3. 根本原因を**フラグダンプ**(`-flags`)や**状態diff**(`-diff`)で特定する。
   「同じ可視経路が到達経路によって内部フラグが食い違う」= 状態復元バグの典型。
4. 修正は最小変更。修正後は必ず回帰を取る(下記「回帰検証」)。C++ は `-exec` 全出力の
   **修正前後 diff**(`__LINE__` 正規化)、Java は `run_java_regression.sh`(Java `-exec` と
   C++ `test_result.txt` の一致)。通常経路に影響がないことを担保する。
5. **移植は下記「移植と検証マトリクス」の各エンジンコピーへ同型で展開**する。運賃(金額)に
   直結する共有ロジックなので、プラットフォームごとに回帰を回してから完了とする。段階的に
   進める場合は「まず C++ のみ」等とユーザが範囲を切ることがあるので、指示に従うこと。

## 検証ハーネスの使い方

場所: `test/unix/all/removetail-151-repro/`(`repro.cpp` + `README.md` + 各種 `*.log` 証跡)。
ビルドと主要スイープは同梱スクリプトで一括実行できる:

```bash
# リポジトリルートから
bash .claude/skills/farert-route-engine-debug/scripts/build_and_verify.sh
```

手動で使う場合:

```bash
cd test/unix/all
make                                   # farert 本体 + オブジェクト(alpdb.o 等)を生成
# repro を現行 alpdb.o でリンク
g++ -Wall -D_DEBUG -D_CONSOLE_TEST -g -O0 \
  -I. -I../common -I../../../app/win_mfc/fjr_mfc/lib/db -I../../../app/alps \
  -std=c++0x removetail-151-repro/repro.cpp alpdb.o db.o sqlite3.o ../common/stdafx.o \
  -o removetail-151-repro/repro -lpthread -ldl
source start.sh                        # export farertDB=.../db/jrdbNewest.db

R=./removetail-151-repro/repro
# 1) 単発: add(S1)->add(L1,S2)->add(L2,S3)->removeTail()->add(L2,S3) の一致検証
printf '\n' | $R 金沢 七尾線 津幡 IRいしかわ 倶利伽羅
# 2) 会社線フラグダンプ: add(L2,S3)前の期待状態 B0 と removeTail後 B1 を比較
printf '\n' | $R -flags 金沢 七尾線 津幡 IRいしかわ 倶利伽羅
# 3) 状態diff: route_script を素に切り詰め構築 vs removeTail後
$R -diff "新大村,西九州新幹線,長崎,長崎線,浦上"
# 4) 実経路コーパス多段 removeTail→再add(route_script 往復整合)
$R -undo removetail-151-repro/routes.txt
# 5) 全分岐駅×路線 総当たり(2区間)
$R -brute
# 6) 大阪環状線ループ特化
$R -osaka
# 7) 乱択ファジング(setup_route 再構築との等価性)  seed iterations
$R -fuzz 7 30000
```

- TRACE は stderr。内部挙動を見るときは `2>trace.log` で分離。
- テストハーネスの `ASSERT` は `scanf` で待つので、バッチ実行時は `</dev/null` を渡す
  (EOF で継続)。リリースビルドでは `ASSERT` は無効化され、そのまま戻り値になる。
- 判定語: `OK`/`NG**`(単発・brute・osaka)、`NGSTATE`/`NGPURE`(fuzz)、`NGUNDO`(undo)。

## バグ分類(報告症状をここに当てる)

| 症状 | 例 | 判定 |
|---|---|---|
| **新幹線在来線同一視で重複エラー(-1)** | `新大村,西九州新幹線,長崎,長崎線,浦上` | **正しい挙動**。諫早-長崎は新幹線/在来線同一視区間。`F-3b`(`alpdb.cpp` 新幹線折返しチェック、`add()` 内)で -1。route_script は再add で復元できないが、それは同一視の帰結。修正対象外。 |
| **151条圧縮後の removeTail=全消し** | `東京,東海道線,東神奈川,横浜線,横浜` → 2要素に圧縮 | **正しい挙動(ユーザ確認済み A:No)**。水平型変換で可視経路が2要素に折りたたまれると removeTail が `list_num<=2 → removeAll(false)` に落ち経路全体を破棄。中間駅情報は route_list_raw から消えており原理的に復元不能。修正対象外。 |
| **会社線同一線継ぎ足しで ASSERT/-4** | `金沢 七尾線 津幡 IRいしかわ 倶利伽羅` の removeTail 後再add | **バグ → 修正済み**(下記)。 |

判断に迷う NG は、まず「新幹線/同一視が絡むか」「経路が2要素に折りたたまれたか(rc=4/1→removeAll)」で
上2つの正しい挙動を除外し、残りを真のバグ候補とする。

## 修正済みバグ: 会社線 同一会社線継ぎ足し(2026-07 対応, C++/Java)

コミット `bug/takeDupRoute` の `e030467`("fix: allow re-adding same company line after
removeTail (through-service)")が C++ 本体。`git show e030467` で正確な差分を参照できる。

**症状**: 会社線直通運転区間(七尾線→IRいしかわ、ハピラインふくい直通 等)で
`add → removeTail → add` すると、デバッグ時 `alpdb.cpp` 会社線b枝の
`ASSERT(back().lineId != line_id)` が発火、リリース時は -4(「許可されていない会社線通過です」)。

**根本原因**: `Route::companyPassCheck()` の会社線b枝は「compncheck 有効中に会社線が現れる=
新規会社線間接続」を前提に `CompanyConnectCheck`(市振・倶利伽羅等の乗継可否)を行う。しかし
**同一会社線が分岐駅を挟んで継続する場合**(金沢→津幡→倶利伽羅 が全て IRいしかわ)は新規接続では
ないため、この枝で弾かれるのが誤り。removeTail 後に末尾が同一会社線になった状態で再addすると顕在化。

**修正**: b枝で `back().lineId == line_id`(同一会社線継続)を `compnend=true` の直後に早期
`return 0` で継続扱いにし、旧 `ASSERT(back().lineId != line_id)` を削除。異会社線接続
(`back().lineId != line_id`)は従来どおり `CompanyConnectCheck` を通す。

## 移植と検証マトリクス

同じ b枝1箇所の修正を各エンジンコピーへ同型で展開し、プラットフォームごとに回帰を回す。

| エンジンコピー | 用途 | 修正 | 検証 |
|---|---|---|---|
| `app/alps/alpdb.cpp` | C++ 本体(Windows/iOS/CLI 共有) | **済** | `-exec` 前後 diff 実質差分ゼロ、ハーネス全スイープ ASSERT 0件、会社線 add/removeTail/add rc一致 |
| `app/Farert.android/.../org/sutezo/alps/Route.java` | Android 本番 | **済** | `run_java_regression.sh` で Java `-exec` が C++ `test_result.txt` と**一致**、`compileDebugJavaWithJavac` 通過 |
| `test/RouteTest/.../sutezo/routetest/alps/Route.java` | Java テストハーネス | **済** | (本番と同型。`last_flag`/`RouteUtil.` 前置) |
| `test/RouteTest.kotolin/.../sutezo/routetest/alps/Route.kt` | Kotlin テスト | **未** | b枝 `ASSERT(back().lineId != line_id)` 相当(`:2626` 付近)を削り、`BIT_ON(lastFlag, BLF_COMPNEND)` の後に `if (back().lineId.toInt() == line_id) return 0` を挿入で同型移植可 |

Java コピーは `route_flag`/`last_flag` の別、`ASSERT`/`IS_COMPANY_LINE` の `RouteUtil.` 前置、
`route_list_raw.get(size-1)` の書き方が違うだけで、ロジックは C++ と1対1。

## 回帰検証(修正時は必ず実施)

`-exec`(約1783経路の運賃計算)は removeTail を通らないので、**通常の運賃計算に影響がないこと**を
ここで担保する。C++ と Java の2本を回す。

### C++ 回帰: 修正前後の -exec 全出力 diff

出力は TRACE(stderr)込みで比較する。alpdb.cpp の行数が変わると TRACE 中の `__LINE__` 由来の
数値が一斉にずれ大量の見かけ差分が出るので、**末尾の数値を正規化してから diff** する。

**重要な落とし穴 — ベースラインの取り方**: `git checkout -- alpdb.cpp` で「修正前」を得るのは、
修正が**まだコミットされていない**場合のみ正しい。修正が既にコミット済み(例: `e030467`)だと
checkout は修正版を復元してしまい、**修正版どうしを比較して偽の一致**になる(生 diff が2行しか
出ない=危険信号)。**正しい修正なら生 diff は数千行出る**(`__LINE__` シフトの証拠)。
コミット済みなら親コミットから取り出す:

```bash
cd test/unix/all
export farertDB=$(readlink -f ../../../db/jrdbNewest.db)
FIX=e030467   # 修正コミット。未コミットなら FIX=HEAD で working が fixed 側
# 修正前 = 親コミットの alpdb.cpp
git -C ../../../ show ${FIX}^:app/alps/alpdb.cpp > /tmp/alpdb_pre.cpp
cp /tmp/alpdb_pre.cpp ../../../app/alps/alpdb.cpp && make >/dev/null 2>&1
./farert -exec > /tmp/exec_pre.txt 2>&1 </dev/null
# 修正版へ復元(コミット済みなら git checkout がクリーン。未コミットなら退避コピーから戻す)
git -C ../../../ checkout -- app/alps/alpdb.cpp && make >/dev/null 2>&1
./farert -exec > /tmp/exec_fix.txt 2>&1 </dev/null
# 検算: 生 diff は数千行出るはず(=別バイナリの証拠)
diff /tmp/exec_pre.txt /tmp/exec_fix.txt | grep -cE '^[<>]'
# __LINE__ 正規化後の実質差分
sed -E 's/ [0-9]{3,6}$//' /tmp/exec_pre.txt > /tmp/pre.txt
sed -E 's/ [0-9]{3,6}$//' /tmp/exec_fix.txt > /tmp/fix.txt
diff /tmp/pre.txt /tmp/fix.txt | grep -E '^[<>]'   # 期待: 実行時間の1行のみ、または対象経路のみ
```

実質差分が「意図した対象経路のみ」または「実行時間だけ」なら、通常経路への副作用なし。

### Java 回帰: Java -exec を C++ 基準と一致比較(javatest.md)

Android 本番エンジン `org.sutezo.alps` を JVM 上で走らせ、C++ の `test_result.txt` と一致するかを見る。
手順の正典は `javatest.md`。日常運用は下記スクリプト一発:

```bash
./test/run_java_regression.sh   # exit 0 = Java と C++ の test_result.txt が一致
```

内部で `org.sutezo.alps` + テスト用スタブを `javac` でコンパイルし、`JavaTestMain -exec` を実行して
`app/Farert.android/app/src/test/resources/test_result.txt` を再生成、`test/unix/all/test_result.txt`
(C++ 基準・最新で正)と `diff`。JDBC 依存(`sqlite-jdbc.jar` 等、resources 配下)が要る。
Java 側で構文確認だけしたいときは `cd app/Farert.android && ./gradlew compileDebugJavaWithJavac`。

注意: `-exec` は C++/Java とも removeTail を通らないため、会社線バグ修正**そのものの挙動**
(add→removeTail→add)は C++ ハーネス(`removetail-151-repro`)で実証する。Java の -exec 一致は
「修正で通常運賃計算を壊していない」ことの担保であって、修正挙動の直接テストではない。

## エンジンの要点(背景知識)

- **経路は分岐駅ビットマスク(`jct_mask`, 最大350)で重複通過を管理**。`add()` は `on`(OR)で
  マスクを立て、`removeTail()` は `off`(AND-NOT)で落とす。OR と AND-NOT は重なりに非対称なので、
  境界駅が両区間に属すると removeTail が過剰に落とすことがある(状態復元バグの温床)。
- **`RouteFlag`** に適用規則・大阪環状線通過状態・会社線状態(compncheck/compnpass/compnbegin/
  compnend/compnda/compnterm)を保持。会社線バグはこれらの復元漏れが原因。
- **151条(分岐特例)** は `add()` 内で経路を破壊的に折りたたみ/展開し、格納 lineId を表示用に
  付け替える。このため `route_script()` の出力が `setup_route()`/再add で必ずしも往復しない。
- **路線ID名前空間**: `LID_SHINKANZEN 0x1000` / `LID_COMPANY 0x2000` / `LID_BRT 0x4000`。
  `IS_COMPANY_LINE(id)` で会社線判定。
- 主要関数: `Route::add`(経路追加・妥当性検証)、`Route::removeTail`(末尾除去・状態巻き戻し)、
  `Route::companyPassCheck`/`preCompanyPassCheck`/`postCompanyPassCheck`(通過連絡運輸)、
  `CalcRoute::calcFare`→`FARE_INFO::calc_fare`(運賃集計)。

## さらに詳しく

- 詳細な調査ログ・フラグ表・3モードの具体例・今回のハマりどころ: `references/findings.md`
- ハーネスの各モード仕様と過去の実行ログ: `test/unix/all/removetail-151-repro/README.md` と同ディレクトリの `*.log`
- Java 回帰テストの正典(スタブ・JDBC・実行方法): リポジトリ直下 `javatest.md`
- エンジン全体像: `specs/Core/architecture.md`、JR規則: `specs/Core/rule-88.md`、`specs/Core/Rule114.md`
