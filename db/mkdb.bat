rem jrdb.xls のlines, clinfar, rule69, rule70_new, rule86, fare2シートの
rem 全内容(テキスト)を すべて jrdb.txt へ貼り付け
rem 本バッチファイルを実行する

python scripts\jr_db_reg.py
if errorlevel 1 goto err
python scripts\node_list.py >scripts\node_list.txt
if errorlevel 1 goto err
rem -- git入れたので保留 copy jr.db devel\fjr_mfc
rem -- git入れたので保留 if errorlevel 1 goto err
rem -- git入れたので保留 pushd devel\fjr_mfc
rem -- git入れたので保留 if errorlevel 1 goto err
rem -- git入れたので変更 >
rem -- git入れたので変更 <
scripts\crypt_tofish.exe enc jr.db
if errorlevel 1 goto err
move jr.db.enc ..\app\win_mfc\fjr_mfc\alps_mfc\res
if errorlevel 1 goto err
rem popd
@echo **** successfully *****
goto fin
:err
rem popd
@echo !!! failed !!!
:fin
