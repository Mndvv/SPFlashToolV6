rem @set WORKING_DIR=%cd%
rem @set TOOLS_DIR=%WORKING_DIR%\Build\Tools\Windows
@rem %1: build type(debug or release) %2: clean or not
@set BUILD_FLAG=%1
@if not defined BUILD_FLAG @set BUILD_FLAG=debug
make.exe HOST=windows BUILD_TYPE=%BUILD_FLAG% %2
@echo off
rem if %errorlevel%==0 (@echo off) else (exit %errorlevel%)