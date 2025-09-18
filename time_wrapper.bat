@echo off
setlocal EnableDelayedExpansion

:: Get the start time
for /f "tokens=*" %%i in ('powershell -Command "& { (Get-Date).Ticks }"') do set START_TIME=%%i

:: Execute the original command and capture output
%*
set EXIT_CODE=%errorlevel%

:: Get the end time and calculate duration
for /f "tokens=*" %%i in ('powershell -Command "& { $endTime = (Get-Date).Ticks; $elapsed = [math]::Round((%START_TIME% - $endTime) / -10000000.0, 3); $elapsed }"') do set ELAPSED=%%i

:: Extract the source file from the command line arguments
set SOURCE_FILE=
set COMMAND_LINE=%*
for %%a in (%*) do (
    set "arg=%%a"
    if "!arg:~-4!"==".cpp" set "SOURCE_FILE=!arg!"
    if "!arg:~-3!"==".cc" set "SOURCE_FILE=!arg!"
    if "!arg:~-2!"==".c" set "SOURCE_FILE=!arg!"
    if "!arg:~-4!"==".cxx" set "SOURCE_FILE=!arg!"
)

:: If no direct source file found, try to extract from /Fo or other patterns
if "!SOURCE_FILE!"=="" (
    echo !COMMAND_LINE! | findstr /i "\.cpp\|\.cc\|\.c\|\.cxx" >nul
    if !errorlevel! equ 0 (
        for /f "tokens=*" %%f in ('echo !COMMAND_LINE! ^| powershell -Command "& { $input = $input -replace '/Fo.*?\s', ''; if ($input -match '([^\s]+\.(cpp|cc|c|cxx))') { $matches[1] } }"') do set "SOURCE_FILE=%%f"
    )
)

:: Log the timing if we found a source file
if not "!SOURCE_FILE!"=="" (
    echo !ELAPSED! !SOURCE_FILE! >> build_times.tmp
)

exit /b %EXIT_CODE%