@echo off
setlocal enabledelayedexpansion

:: Timing wrapper for compilation commands
:: Usage: time_wrapper.bat <command>

:: Get start time in milliseconds
for /f "tokens=*" %%i in ('powershell -Command "& { (Get-Date).ToFileTime() }"') do set START_TIME=%%i

:: Execute the command (suppress output)
%* >nul 2>&1

:: Get end time in milliseconds
for /f "tokens=*" %%i in ('powershell -Command "& { (Get-Date).ToFileTime() }"') do set END_TIME=%%i

:: Calculate elapsed time in seconds
for /f "tokens=*" %%i in ('powershell -Command "& { [math]::Round((!END_TIME! - !START_TIME!) / 10000000.0, 2) }"') do set ELAPSED=%%i

:: Extract just the source filename from the command
for %%a in (%*) do (
    if "%%~xa"==".cpp" echo %%~na.cpp: !ELAPSED!s
    if "%%~xa"==".c" echo %%~na.c: !ELAPSED!s
)