@echo off
rem jrdb.xls ��lines, clinfar, rule69, rule70_new, rule86, fare2�V�[�g��
rem �S���e(�e�L�X�g)�� ���ׂ� jrdb.txt �֓\��t��
rem �{�o�b�`�t�@�C�������s����

python scripts\jr_db_reg.py
if errorlevel 1 goto err
python scripts\node_list.py >scripts\node_list.txt
if errorlevel 1 goto err
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
