@echo off
setlocal

set "input=build_times.txt"

if not exist "%input%" (
    echo File not found: %input%
    pause
    exit /b
)

echo ==== Debug: Raw Input ====
type "%input%"
echo ==========================
echo.

:: Sum times using PowerShell
for /f "usebackq tokens=* delims=" %%A in (`powershell -NoLogo -NoProfile -Command ^
    "$sum = 0; Get-Content '%input%' | ForEach-Object { $_ = $_.Trim(); if ($_ -match '^\d+(\.\d+)?s') { $num = ($_ -split '\s+')[0] -replace 's',''; $sum += [double]$num } }; Write-Output $sum"`) do (
    set "total=%%A"
)

echo Total Time: %total%s
pause
