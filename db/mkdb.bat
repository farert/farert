python scripts\jr_db_reg.py
if errorlevel 1 goto err
python scripts\node_list.py >scripts\node_list.txt
if errorlevel 1 goto err
rem -- git“ü‚ê‚½‚Ì‚Å•Û—¯ copy jr.db devel\fjr_mfc
rem -- git“ü‚ê‚½‚Ì‚Å•Û—¯ if errorlevel 1 goto err
rem -- git“ü‚ê‚½‚Ì‚Å•Û—¯ pushd devel\fjr_mfc
rem -- git“ü‚ê‚½‚Ì‚Å•Û—¯ if errorlevel 1 goto err
rem -- git“ü‚ê‚½‚Ì‚Å•ÏX >
rem -- git“ü‚ê‚½‚Ì‚Å•ÏX <
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
