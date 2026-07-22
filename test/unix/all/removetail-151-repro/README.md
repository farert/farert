# removeTail / 151条(段差型・水平型分岐特例)再現ドライバ

`add() → removeTail() → add()` が 151条区間で破綻する問題の調査用ドライバとログ一式。

## 内容

- `repro.cpp` — 再現ドライバ本体
- `routes.txt` — `test_exec.cpp` から抽出した実経路コーパス(1783経路)。
  `test/unix/all/.gitignore` の `*.txt` 対象で **未コミット**。`-undo` を使う前に下記で再生成する。
- `*.log` — 調査時の実行ログ(証跡、未コミット)

### routes.txt の再生成

`-undo` モードで使う実経路コーパスは `test_exec.cpp` から生成する:

```bash
cd test/unix/all
python3 - <<'PY'
import re
src = open('../common/test_exec.cpp', encoding='utf-8').read()
routes, seen = [], set()
for m in re.finditer(r'_T\("([^"]+)"\)', src):
    s = m.group(1)
    if s and s[0] in 'Ccs' and not s[1:2].isascii():
        s = s[1:]
    toks = [t for t in re.split(r'[ ,\t]+', s) if t]
    if len(toks) < 5 or (len(toks) % 2) == 0:
        continue
    clean, ok = [], True
    for t in toks:
        while t and t[0] in 'xep':
            t = t[1:]
        if not t:
            ok = False; break
        clean.append(t)
    if not ok:
        continue
    r = ' '.join(clean)
    if r not in seen:
        seen.add(r); routes.append(r)
open('removetail-151-repro/routes.txt', 'w', encoding='utf-8').write('\n'.join(routes) + '\n')
print(len(routes), 'routes')
PY
```

## ビルド

`test/unix/all/` で一度 `make`(CLI `farert`)してオブジェクトを生成した後、
同ディレクトリで:

```sh
g++ -Wall -D_DEBUG -D_CONSOLE_TEST -g -O0 \
  -I. -I../common -I../../../app/win_mfc/fjr_mfc/lib/db -I../../../app/alps \
  -std=c++0x removetail-151-repro/repro.cpp alpdb.o db.o sqlite3.o ../common/stdafx.o \
  -o removetail-151-repro/repro -lpthread -ldl
```

## 実行(要 `farertDB` 環境変数)

```sh
source start.sh   # export farertDB=.../db/jrdbNewest.db

# 単発: add(S1)->add(L1,S2)->add(L2,S3)->removeTail()->add(L2,S3) の一致検証
./removetail-151-repro/repro 長崎 長崎線 喜々津 長崎線\(長与経由\) 諫早

# 素の切詰め vs removeTail後 の状態比較(路線ID/フラグ/再add rc)
./removetail-151-repro/repro -diff "新大村,西九州新幹線,長崎,長崎線,浦上"

# 実経路を多段 removeTail→再add して route_script 往復整合を検証
./removetail-151-repro/repro -undo removetail-151-repro/routes.txt

# 全分岐駅×路線 総当たり(2区間)
./removetail-151-repro/repro -brute

# 大阪環状線ループ特化スキャン
./removetail-151-repro/repro -osaka

# 乱択ファジング(setup_route再構築との等価性)  seed iterations
./removetail-151-repro/repro -fuzz 7 30000
```

TRACE ログは stderr へ出るので、内部挙動を見るときは `2>trace.log` で分離する。

## 判明している破綻パターン

1. **重複エラー(-1)**: `新大村,西九州新幹線,長崎,長崎線,浦上` 型。
   `setup_route` は路線トークンを `長崎線(長与経由)=183` に解決して構築するが、
   `route_script()` は折りたたみ後の表示ID `長崎線=182` を出力する。
   この 182 を再add すると新幹線折返しチェック(`alpdb.cpp` の `F-3b`)で -1。
   → route_script の往復不能。

2. **経路乖離(rc=1だが別経路)**: `長崎,長崎線,喜々津,長崎線(長与経由),諫早` 型。
   水平型変換で可視経路が2要素に折りたたまれると removeTail が
   `list_num<=2 → removeAll(false)` に落ち、経路全体を消してしまう。

3. **会社線 -4 + ASSERT(151条ではない派生)**: `金沢,IRいしかわ,…` 型。
   removeTail で通過連絡運輸フラグ(`compnpass` 等)が復元されず、
   `alpdb.cpp` の会社線 ASSERT が発火し -4。
