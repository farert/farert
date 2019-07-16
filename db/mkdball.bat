@echo off
@echo ==== mkdb jrdb2014 2014 ====
call mkdb jrdb2014 2014
if errorlevel 1 goto err
@echo ==== mkdb jrdb2015 2015 ====
call mkdb jrdb2015 2015
if errorlevel 1 goto err
@echo ==== mkdb jrdb2017 2017 ====
call mkdb jrdb2017 2017
if errorlevel 1 goto err
@echo ==== mkdb jrdb2019 2019 ====
call mkdb jrdb2019 2019
if errorlevel 1 goto err
@echo **** complete *****
@echo .
goto fin
:err
@echo !!! failed !!!
:fin
pause
