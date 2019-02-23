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
すなわち、all/test 内で、

./test
  引数なしではすべてのtestパターン(app/win_mfc/fjr_mfc/test_exec.cpp)を実行する.
  .test >log.txt
  として実行し、結果は、test_result.txt として作成される。
  (現状どおり)

./test 引数奇数個
　指定した経路を実行してその結果を返す
 行頭に's'を記載すると、新幹線在来線別線扱い（小倉-博多)

 例；
  　./test 新山口 山陽線 神戸

./test 引数偶数個
  　指定した経路の最短経路を実行してその結果を返す
 例：
  　./test 新山口 神戸
  　./test 新山口 山陽線 神戸 横浜


 --
