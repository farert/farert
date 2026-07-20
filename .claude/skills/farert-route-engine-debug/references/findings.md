# Farert 経路エンジン: removeTail / 151条 / 会社線 調査知見

2026-07 に実施した `add() → removeTail() → add()` 整合性調査の詳細記録。
SKILL.md の背景資料。数値・行番号は当時のもので、参照時は `git blame`/`grep` で現状を確認すること。

## 目次
1. 調査の発端と結論
2. 検証ハーネスの4系統と判定語
3. モード別の詳細(正しい挙動 vs バグ)
4. 会社線バグの根本原因(フラグダンプの読み方)
5. 修正の設計判断
6. 回帰検証の落とし穴(__LINE__ シフト / ベースラインの取り方)
7. 移植と各プラットフォームの検証(C++/Java/Kotlin)

## 1. 調査の発端と結論

発端: 「`add() → removeTail() → add()` で重複エラーになるバグがある」との報告。151条関連との示唆あり。

結論: 報告された症状のうち、
- **新幹線在来線同一視の -1** と **151条圧縮後の removeTail 全消し** は**正しい挙動**(ユーザ確認済み)。
- 真のバグは**会社線 同一会社線継ぎ足し**の1件。`companyPassCheck` 会社線b枝の
  `ASSERT(back().lineId != line_id)` が、直通運転で同一会社線が分岐駅を挟んで続く経路を弾いていた。
  → b枝で同一会社線継続を早期 return する修正で解消(CPPのみ)。

## 2. 検証ハーネスの4系統と判定語

`test/unix/all/removetail-151-repro/repro.cpp`。DB は `jrdbNewest.db`。

| モード | コマンド | 何を検証するか | 判定語 |
|---|---|---|---|
| 単発 | `repro S1 L1 S2 L2 S3` | add/removeTail/re-add で rc・route_script・end が一致するか | `OK`/`NG**` |
| フラグダンプ | `repro -flags S1 L1 S2 L2 S3` | add(L2,S3)前の期待状態 B0 と removeTail後 B1 の会社線フラグ比較 | 目視 |
| 状態diff | `repro -diff "route_script"` | route_script を素に切詰め構築 vs removeTail後の tail item/flag/再add rc | 目視 |
| 実経路undo | `repro -undo routes.txt` | コーパスを多段 removeTail→再add し route_script 往復整合 | `NGUNDO` |
| 総当たり | `repro -brute` | 全分岐駅×通過路線ペアの2区間 add/removeTail/re-add | `NG**` |
| 大阪環状線 | `repro -osaka` | ループ上駅への進入×環状線区間の全組合せ | `NG**` |
| ファジング | `repro -fuzz seed iters` | 乱択churn中、setup_route 再構築との等価性 | `NGSTATE`/`NGPURE` |

`routes.txt` は `test/unix/common/test_exec.cpp` から抽出した実経路コーパス(約1783経路、x/e/p マーカー除去済み)。

当時の主な数値: brute 2533件中 NG 47〜51件、undo 2904件中 10件(全て同一視)、fuzz 90k手中 state乖離18件。
NG の大半は下記モードA(折りたたみ→removeAll)で、会社線ASSERTは16件→修正後0件。

## 3. モード別の詳細

### モード1: 新幹線在来線同一視 → 重複エラー(-1)【正しい】
例: `新大村,西九州新幹線,長崎,長崎線,浦上`。
- 諫早-長崎は西九州新幹線と在来線(長崎線)が新幹線在来線同一視区間。
  長崎まで新幹線で行き在来線で浦上/諫早へ折り返すのは同一区間の重複乗車。
- `add(長崎線,浦上)` は直前に西九州新幹線があると `F-3b`(`add()` 内の新幹線折返しチェック、
  `CheckTransferShinkansen` 由来)で -1。
- `setup_route` はトークン `長崎線` を長与経由(183)に解決して構築できるが、`route_script()` は
  折りたたみ後の表示ID 長崎線(182)を出力するため、182 の再add は同一視で弾かれる。
  → route_script が往復しないのは**同一視の帰結**であり、修正対象外。

### モードA: 151条水平型圧縮後の removeTail 全消し【正しい, A:No】
例: `東京,東海道線,東神奈川,横浜線,横浜`。
- `add(横浜線,横浜)` が水平型分岐特例(`JCT: F1, H, E11-14`)で経路を2要素 `東京,東海道線,横浜` に圧縮
  (中間駅 東神奈川が route_list_raw から消滅)。
- `removeTail()` は `list_num<=2 → removeAll(false)` に落ち経路全体を `東京` に戻す。
- 中間駅情報は破壊されており原理的に `東京,東海道線,東神奈川` は復元不能。
  ユーザ判断で**正しい挙動**(削除=圧縮セグメント丸ごと戻し)。brute の rc1=1/4→rc2=-2 群はこれ。

### モードB: 会社線 同一会社線継ぎ足し → ASSERT/-4【バグ, 修正済み】
例: `金沢 七尾線 津幡 IRいしかわ 倶利伽羅`(七尾線は金沢から直通、金沢-津幡は IRいしかわ)。
- 詳細は次節。

## 4. 会社線バグの根本原因(フラグダンプの読み方)

同じ可視経路 `金沢,IRいしかわ,津幡` が、到達経路で会社線フラグが食い違うのが決定的証拠。

| 到達方法 | compncheck | compnbegin | compnend | 再add(IRいしかわ,倶利伽羅) |
|---|---|---|---|---|
| `七尾線` トークン経由(直通) | 0 | 0 | 0 | 第1枝(preCompanyPassCheck)→ 成功 |
| `IRいしかわ` を直接1区間目 | 1 | 1 | 1 | b枝 → ASSERT発火(begin=1でも ASSERTが先) |
| **removeTail後** | 1 | 0 | 1 | b枝 → CompanyConnectCheck(津幡)→ -4 |

読み方:
- `-flags` の B0(期待)と B1(removeTail後)が食い違えば状態復元バグ。
- `companyPassCheck`(`alpdb.cpp`)の分岐:
  - 冒頭: `compnda || (会社線 && compnpass)` → -4。
  - 第1枝: 会社線 && `!compncheck && !compnpass` → compncheck=compnend=true, `preCompanyPassCheck`。
  - 後段枝: 非会社線 && compncheck → `postCompanyPassCheck`(会社線→JR)。
  - **b枝**: 会社線(compncheck 有効中)→ 新規会社線接続とみなし `CompanyConnectCheck`。
    ここが同一会社線継続を弾いていた。
- `CompanyConnectCheck(station_id)` は `t_compnconc` に登録された会社線接続許可駅(市振・目時・
  妙高高原・倶利伽羅 等)でのみ 0(許可)を返す。津幡は未登録なので -4。同一会社線継続には不適。

## 5. 修正の設計判断

3案を検討:
- 案1(採用): b枝で `back().lineId == line_id`(同一会社線継続)を早期 `return 0`、
  旧 ASSERT を削除。最小変更で根本解決。removeTail経由・IRいしかわ直接構築の両方が直る。
- 案2: removeTail のフラグを残存経路から再導出(reBuild相当)。ただし stored lineId が IRいしかわ
  なので再導出しても check=1 になり、b枝ASSERTは案1なしでは残る → 単独では不十分。
- 案3: 案1+案2 併用。範囲最大。

案1を採用。会社線b枝(`companyPassCheck`)に同一会社線継続の早期returnを追加し、
`compnend=true` は維持(末尾は会社線のまま)。異会社線接続(`back().lineId != line_id`)は
従来どおり `CompanyConnectCheck`。

なぜ安全か: `-exec`(通常経路のみ)全出力が実質不変=通常運賃計算に副作用なし。同一会社線継続の
早期returnは removeTail後の再add という特定状況でしか到達しない(通常経路は該当せず)。

## 6. 回帰検証の落とし穴(__LINE__ シフト / ベースラインの取り方)

### __LINE__ シフト
`alpdb.cpp` の行数が変わると、TRACE 中の `__LINE__` 由来の数値(例: `TOICA not enabled 10690`)が
一斉にずれ、`-exec` の生 diff が数千〜万行になる。これは見かけ上の差分。
`sed -E 's/ [0-9]{3,6}$//'` で末尾数値を正規化してから diff すると実質差分だけが残る。
今回は正規化後の差分が「プロセス実行時間の1行」のみ = 副作用ゼロと確認できた。
TRACE を消す(`ALPS_LOG` を外す)手もあるが reference ログ比較が効かなくなるので行番号正規化が実務的。

### ベースライン取り違え(実際にやらかした)
`git checkout -- alpdb.cpp` で「修正前」を得るのは修正が**未コミット**のときだけ正しい。
本件は途中で C++ 修正が `bug/takeDupRoute` の `e030467` に**コミット済み**になっており、
checkout が修正版を復元 → **修正版どうしを比較して「一致」と誤判定**した。
- 危険信号: 生 diff が **2行しか出ない**(本来 `__LINE__` シフトで数千行出るはず)。
- 正しい手順: コミット済みなら親から取り出す `git show e030467^:app/alps/alpdb.cpp`。
  これで生 diff 10,982行 → 正規化後2行(実行時間のみ)= 真の一致を確認できた。
- 教訓: 「生 diff の行数」で別バイナリを比較できているか必ず検算する。

## 7. 移植と各プラットフォームの検証(C++/Java/Kotlin)

会社線b枝の修正は全エンジンコピー共通。各コピーの差は表層のみ(下記)。

- **C++ `app/alps/alpdb.cpp`**(Windows/iOS/CLI 共有): `route_flag` / `ASSERT` / `IS_COMPANY_LINE` /
  `route_list_raw.back()`。検証は `removetail-151-repro` ハーネス + `-exec` 前後 diff。
- **Java 本番 `org.sutezo.alps/Route.java`**: `route_flag` だが `route_list_raw.get(size-1)`。
  検証は `run_java_regression.sh`(Java `-exec` = C++ `test_result.txt`)+ `compileDebugJavaWithJavac`。
  → 一致確認済み。手順の正典はリポジトリ直下 `javatest.md`。
- **Java テスト `sutezo.routetest.alps/Route.java`**: `last_flag` / `RouteUtil.ASSERT` /
  `RouteUtil.IS_COMPANY_LINE`。b枝は preCompanyPassCheck を呼ばない簡易版だが同型に修正済み。
- **Kotlin `test/RouteTest.kotolin/.../Route.kt`**(未対応): フラグが `lastFlag` の bit 表現
  (`BLF_COMPNEND` 等、`BIT_ON`/`BIT_CHK`)。`:2626` 付近の `ASSERT(back().lineId != line_id)` を
  削り、`BIT_ON(lastFlag, BLF_COMPNEND)` 直後に `if (back().lineId.toInt() == line_id) return 0`。

ポイント: `-exec` 回帰は C++/Java とも removeTail を通らないので「通常運賃を壊していない」担保。
修正挙動そのもの(add→removeTail→add)の実証は C++ ハーネスが引き受ける。Java で動的挙動まで
見るなら `JavaTestMain` に removeTail シナリオを追加するか Android インストルメンテッドテストが必要。
