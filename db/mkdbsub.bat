rem @echo off
rem jrdb.xls のlines, clinfar, rule69, rule70_new, rule86, fare2シートの
rem 全内容(テキスト)を すべて jrdb.txt へ貼り付け
rem 本バッチファイルを実行する

rem mkdb jrdb2014 2014
rem mkdb jrdb2015 2015

python scripts\jr_db_reg.py %1.txt %2 8
if errorlevel 1 goto err
python scripts\node_list.py %1.db > scripts\node_list.txt
if errorlevel 1 goto err
scripts\crypt_tofish.exe enc %1.db
if errorlevel 1 goto err
move %1.db.enc ..\app\win_mfc\fjr_mfc\alps_mfc\res
if errorlevel 1 goto err
rem popd
@echo **** successfully *****
goto fin
:err
@echo !!! failed !!!
:fin
