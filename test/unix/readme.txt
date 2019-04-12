testmain.cpp を、exp/ autoroute/ all/
この下の各ディレクトリ内でMake実行

exp/ コマンドラインで実行
autoroute/ コマンドラインで自動経路
all/ test_exec.cpp の全テストパターン実行



Unixでは、make
Windowsコマンドプロンプトでは、vcvars32.batの実行で環境を整えてから、
nmake /f Makefile.nmk

2019.2.24 all/test を仕様拡張し、exp, autoroute を不要とした。
以降は、all/test だけを使用すれば良い。
使い方は、all/test 内で、

./test
  引数なしではすべてのtestパターン(app/win_mfc/fjr_mfc/test_exec.cpp)を実行する.
  .test >log.txt
  として実行し、結果は、test_result.txt として作成される。
  (現状どおり)

./test 引数1つ
　引数のテキストファイルを1行ずつ逐次処理する


./test 引数奇数個
　指定した経路を実行してその結果を返す
 行頭に's'を記載すると、新幹線在来線別線扱い（小倉-博多)

 例；
  　./test 新山口 山陽線 神戸

 P型経路などではAssertエラーになるので、以下のように指定するとよい
   ./test 久留米 久大線 大分 日豊線 城野 日田彦山線 e夜明


./test 引数偶数個
  　指定した経路の最短経路を実行してその結果を返す
 例：
  　./test 新山口 神戸
  　./test 新山口 山陽線 神戸 横浜

* データベースは、環境変数 farertDB にパスを指定する。
   例)
    $ export farertDB=/Users/ntake/exports/devel/farert.repos/db/jrdb2018.db

   (../../db/scripts/jrdb.py と同様)
　　
 --
