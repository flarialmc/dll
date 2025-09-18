@echo off
setlocal enabledelayedexpansion

:: Timing wrapper for compilation commands
:: Usage: time_wrapper.bat <command>

:: Find the source file first
set SOURCE_FILE=
for %%a in (%*) do (
    if "%%~xa"==".cpp" set SOURCE_FILE=%%~na.cpp
    if "%%~xa"==".c" set SOURCE_FILE=%%~na.c
)

:: Use PowerShell Measure-Command for accurate timing with single invocation
for /f "tokens=*" %%i in ('powershell -Command "& { $result = Measure-Command { cmd /c '%*' 2>$null | Out-Null }; [math]::Round($result.TotalSeconds, 2) }"') do set ELAPSED=%%i

:: Output timing and save to file if we found a source file
if defined SOURCE_FILE (
    echo !SOURCE_FILE!: !ELAPSED!s
    echo !ELAPSED! !SOURCE_FILE! >> build_times.tmp
)