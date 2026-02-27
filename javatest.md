
# Android 版のテストコード

## TL;DR
- [2) Java のコンパイル](#java-compile)
- [3) 実行](#java-run)
- [4) 結果](#java-result)


## ソース
- app/Farert.android/app/src/main/java/org/sutezo/alps

## 参考C＋＋ソース

- app/test/unix/all/Makefile
- app/test/unix/all/testmain.cpp
- app/test/unix/common/test_exec.cpp
- その他は、Makefile の中身を参照
- C++でのテストパターンは、 app/test/unix/common/test_exec.cpp にある。
- C++でのテスト結果出力は、 app/test/unix/all/test_result.txt 
- C++の実行は、 ./farert -exec つまり、testmain.cpp の argv[1]が、'-exec' です。それ以外は実装しなくて結構です

## TODO
- C++でのテスト結果出力を模倣すること

## 注意
- C++版、Javaソースとも変更しないでください。
- 変更して良いのは成果物のディレクトリ以下のものだけに限ります。
- 使用するDBは、db/jrdbNewest.db
- app/test/unix/all/test_result.txt は 最新で正 とみなしてください
- 結果出力は、app/Farert.android/app/src/test/resources/ へ書き出してください.

## 成果物
- CLIで実行できるようにする
- java -cp コマンドで起動する。
- 結果は, C++と同様 test_result.txt を生成する。
- 結果をC++版とdiffコマンドで比較し、一致していれば完成となる
- テストソースは、app/Farert.android/app/src/test に置く。
  
- C++の test_result.txt と Javaの test_result.txt で diff 比較します。
- その結果が一致すればテストがパスしたことになります。

## 追加ファイルの説明（理由と内容）
### テスト実行と出力生成
- `app/Farert.android/app/src/test/java/org/sutezo/alps/JavaTestMain.java`
  - 追加理由: C++ の `test_exec.cpp` と同等のテスト実行を Java で再現し、`-exec` 指定で `test_result.txt` を生成するため。
  - 記述内容: C++ のテスト配列をパース、ルート構築・`FARE_INFO` 実行、結果の整形と書き出し、DB 参照の初期化。

### DB 参照ユーティリティ
- `app/Farert.android/app/src/test/java/org/sutezo/alps/DbIdOf.java`
  - 追加理由: テスト内で DB ID を引く処理をまとめ、`JavaTestMain` から利用するため。
  - 記述内容: JDBC 経由の ID 参照ロジック。

### Android 依存のスタブ
- `app/Farert.android/app/src/test/java/android/database/Cursor.java`
  - 追加理由: Android API の `Cursor` を JVM テストで動かすため。
  - 記述内容: 必要最小限のメソッドを持つ簡易実装。
- `app/Farert.android/app/src/test/java/android/database/sqlite/SQLiteDatabase.java`
  - 追加理由: Android の `SQLiteDatabase` を JDBC で代替するため。
  - 記述内容: JDBC で `rawQuery` / `execSQL` 相当を実装。
- `app/Farert.android/app/src/test/java/android/app/Application.java`
  - 追加理由: `Application` 参照を伴うクラスの初期化を通すため。
  - 記述内容: 空のスタブ。
- `app/Farert.android/app/src/test/java/android/content/Context.java`
  - 追加理由: `Context` 参照を伴うクラスの初期化を通すため。
  - 記述内容: 空のスタブ。

### Farert 側のスタブ
- `app/Farert.android/app/src/test/java/org/sutezo/farert/BuildConfig.java`
  - 追加理由: `BuildConfig` 参照の解決。
  - 記述内容: テスト用の最小定義。
- `app/Farert.android/app/src/test/java/org/sutezo/farert/DatabaseOpenHelper.java`
  - 追加理由: Android 版の DB 初期化を JVM で置き換えるため。
  - 記述内容: JDBC で DB を開くための簡易実装。
- `app/Farert.android/app/src/test/java/org/sutezo/farert/FarertApp.java`
  - 追加理由: `FarertApp` 参照の解決。
  - 記述内容: `Context` 参照の最低限実装。

### 依存 JAR（テスト実行用）
- `app/Farert.android/app/src/test/resources/sqlite-jdbc.jar`
  - 追加理由: JVM から SQLite を扱うための JDBC ドライバ。
  - 記述内容: `org.xerial` の SQLite JDBC 本体。
- `app/Farert.android/app/src/test/resources/slf4j-api.jar`
  - 追加理由: sqlite-jdbc のログ依存を満たすため。
  - 記述内容: SLF4J API。
- `app/Farert.android/app/src/test/resources/slf4j-nop.jar`
  - 追加理由: ログ出力を抑制するため。
  - 記述内容: SLF4J の NOP 実装。

## セットアップ / 使い方（JDBC含む）
### 1) JDBC 依存を配置
以下を `app/Farert.android/app/src/test/resources/` に置く。

- `sqlite-jdbc.jar`
- `slf4j-api.jar`
- `slf4j-nop.jar`

例:
```
app/Farert.android/app/src/test/resources/sqlite-jdbc.jar
app/Farert.android/app/src/test/resources/slf4j-api.jar
app/Farert.android/app/src/test/resources/slf4j-nop.jar
```

#### JAR のセットアップ方法
- 取得した JAR を上記パスに **そのまま配置**する。
- 既存の JAR がある場合は **同名で置き換え**れば OK。
- 実行時は `java -cp ...` に 3 つの JAR を含める（下記実行コマンド参照）。

#### JAR のアップデート方法
- 新しいバージョンの `sqlite-jdbc.jar` を入手し、同じファイル名で置き換える。
- 依存に合わせて `slf4j-api.jar` / `slf4j-nop.jar` も更新する。
- 置き換え後は `javac` → `java -cp ...` の順で再実行して動作確認する。

#### JAR の入手方法
入手元は Maven Central を想定。`sqlite-jdbc` と `slf4j` を **同じフォルダ**に配置する。

バージョン確認:
- `sqlite-jdbc` は MvnRepository の versions ページで最新版を確認する。
- `slf4j` は SLF4J 公式の download ページで stable を確認する。

参照ページ:
```
https://mvnrepository.com/artifact/org.xerial/sqlite-jdbc/versions
https://mvnrepository.com/artifact/org.slf4j/slf4j-api/versions
https://mvnrepository.com/artifact/org.slf4j/slf4j-nop/versions
https://www.slf4j.org/download.html
```

例: 直接ダウンロード（バージョンは適宜変更）
```bash
curl -L -o app/Farert.android/app/src/test/resources/sqlite-jdbc.jar \
  https://repo1.maven.org/maven2/org/xerial/sqlite-jdbc/<VERSION>/sqlite-jdbc-<VERSION>.jar

curl -L -o app/Farert.android/app/src/test/resources/slf4j-api.jar \
  https://repo1.maven.org/maven2/org/slf4j/slf4j-api/<VERSION>/slf4j-api-<VERSION>.jar

curl -L -o app/Farert.android/app/src/test/resources/slf4j-nop.jar \
  https://repo1.maven.org/maven2/org/slf4j/slf4j-nop/<VERSION>/slf4j-nop-<VERSION>.jar
```

補足:
- `sqlite-jdbc` は `org.xerial` のアーティファクト。
- `slf4j` は `org.slf4j` の `slf4j-api` と `slf4j-nop`。

<a id="java-compile"></a>
### 2) Java のコンパイル
```
find app/Farert.android/app/src/main/java/org/sutezo/alps app/Farert.android/app/src/test/java -name \
  '*.java' -print0 | xargs -0 javac -encoding UTF-8 -d /tmp/farert_test_classes
or
rg --files -g '*.java' app/Farert.android/app/src/main/java/org/sutezo/alps app/Farert.android/app/src/test/java | xargs javac -encoding UTF-8 -d /tmp/farert_test_classes


# 確認
  ls -l /tmp/farert_test_classes/org/sutezo/alps
```

<a id="java-run"></a>
### 3) 実行
```
java -cp /tmp/farert_test_classes:app/Farert.android/app/src/test/resources/sqlite-jdbc.jar:app/Farert.android/app/src/test/resources/slf4j-api.jar:app/Farert.android/app/src/test/resources/slf4j-nop.jar org.sutezo.alps.JavaTestMain -exec
```

<a id="java-result"></a>
### 4) 結果
- 生成ファイル: `app/Farert.android/app/src/test/resources/test_result.txt`
- 比較: `diff -u test/unix/all/test_result.txt app/Farert.android/app/src/test/resources/test_result.txt`
