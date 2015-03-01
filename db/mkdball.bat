call mkdb jrdb2014 2014
if errorlevel 1 goto err
call mkdb jrdb2015 2015
if errorlevel 1 goto err
@echo **** successfully *****
goto fin
:err
@echo !!! failed !!!
:fin
