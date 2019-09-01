テスト用ディレクトリ
旧バージョンのソースを持ってきて動作してみたい場合、
git show xxx:path/to/file.cpp>file.cpp とかしてここに配置してmakeすれば、
ここに、..と同じ環境が作れるので、.. の./test と比較できます
.. 以上の最新ソースツリーに影響を与えることなく。
以下のソースが、ここにあればOKです

alpdb.cpp          ../../../../app/alps/
alpdb.h            ../../../../app/alps/
test_exec.cpp      ../../../../app/win_mfc/fjr_mfc/alps_mfc/
testmain.cpp       ..からコピーするだけ

