@echo off
rem jrdb.xls のlines, clinfar, rule69, rule70_new, rule86, fare2シートの
rem 全内容(テキスト)を すべて jrdb.txt へ貼り付け
rem または、jr_db_reg.jse jrdb.xlsx を実行する
rem 本バッチファイルを実行する

rem mkdb jrdb2014 2014
rem mkdb jrdb2015 2015

if %1/==/ goto usage
if %2/==/ goto usage
goto next
:usage
echo ex.) mkdb jrdb2015 2015
goto fin
:next
@echo python scripts\jr_db_reg.py %1.txt %2
python scripts\jr_db_reg.py %1.txt %2
if errorlevel 1 goto err
@echo python scripts\node_list.py %1.db > scripts\node_list.txt
python scripts\node_list.py %1.db > scripts\node_list.txt
if errorlevel 1 goto err
@echo scripts\crypt_tofish.exe enc %1.db
scripts\crypt_tofish.exe enc %1.db
if errorlevel 1 goto err
@echo move %1.db.enc ..\app\win_mfc\fjr_mfc\alps_mfc\res
move %1.db.enc ..\app\win_mfc\fjr_mfc\alps_mfc\res
if errorlevel 1 goto err
@echo copy %1.db ..\app\Farert.ios\Farert
copy %1.db ..\app\Farert.ios\Farert
if errorlevel 1 goto err
rem popd
@echo **** successfully *****
@echo .
goto fin
:err
@echo !!! failed !!!
:fin
