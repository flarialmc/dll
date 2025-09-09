@echo off
setlocal EnableDelayedExpansion

:: Build script for Flarial project
:: Usage: build.bat [R|D] where R=Release, D=Debug

echo.
echo ===============================================
echo             Flarial Build Script
echo ===============================================
echo.

:: Get build type from user input
set BUILD_TYPE=
if "%1"=="R" set BUILD_TYPE=Release
if "%1"=="r" set BUILD_TYPE=Release
if "%1"=="D" set BUILD_TYPE=Debug
if "%1"=="d" set BUILD_TYPE=Debug

:: If no argument provided, ask user
if "%BUILD_TYPE%"=="" (
    echo Please select build type:
    echo   [R] Release
    echo   [D] Debug
    echo.
    set /p choice="Enter your choice (R/D): "
    if "!choice!"=="R" set BUILD_TYPE=Release
    if "!choice!"=="r" set BUILD_TYPE=Release
    if "!choice!"=="D" set BUILD_TYPE=Debug
    if "!choice!"=="d" set BUILD_TYPE=Debug
)

:: Validate build type
if "%BUILD_TYPE%"=="" (
    echo ERROR: Invalid build type selected!
    echo Please use R for Release or D for Debug
    pause
    exit /b 1
)

echo Selected build type: %BUILD_TYPE%
echo.

:: Set build directory (CLion style)
set BUILD_DIR=cmake-build-%BUILD_TYPE%-ninja
if "%BUILD_TYPE%"=="Debug" set BUILD_DIR=cmake-build-debug-ninja
if "%BUILD_TYPE%"=="Release" set BUILD_DIR=cmake-build-release-ninja

echo Build directory: %BUILD_DIR%
echo.

:: Find Visual Studio installation and setup environment
echo Setting up MSVC environment...

:: Try to find vcvarsall.bat in common locations
set VCVARSALL_PATH=
for %%v in (2022 2019) do (
    for %%e in (Enterprise Professional Community BuildTools) do (
        if exist "C:\Program Files\Microsoft Visual Studio\%%v\%%e\VC\Auxiliary\Build\vcvarsall.bat" (
            set "VCVARSALL_PATH=C:\Program Files\Microsoft Visual Studio\%%v\%%e\VC\Auxiliary\Build\vcvarsall.bat"
            goto :found_vs
        )
        if exist "C:\Program Files (x86)\Microsoft Visual Studio\%%v\%%e\VC\Auxiliary\Build\vcvarsall.bat" (
            set "VCVARSALL_PATH=C:\Program Files (x86)\Microsoft Visual Studio\%%v\%%e\VC\Auxiliary\Build\vcvarsall.bat"
            goto :found_vs
        )
    )
)

:found_vs
if "%VCVARSALL_PATH%"=="" (
    echo ERROR: Could not find Visual Studio installation!
    echo Please install Visual Studio 2019 or 2022 with C++ tools
    pause
    exit /b 1
)

echo Found Visual Studio at: %VCVARSALL_PATH%

:: Setup MSVC environment for x64
echo Initializing MSVC x64 environment...
call "%VCVARSALL_PATH%" x64 >nul 2>&1
if errorlevel 1 (
    echo ERROR: Failed to initialize MSVC environment!
    pause
    exit /b 1
)

:: Check for Ninja
ninja --version >nul 2>&1
if errorlevel 1 (
    echo.
    echo WARNING: Ninja not found in PATH!
    echo Trying to use Ninja from Visual Studio...
    
    :: Try to find Ninja in VS installation
    for %%v in (2022 2019) do (
        for %%e in (Enterprise Professional Community BuildTools) do (
            if exist "C:\Program Files\Microsoft Visual Studio\%%v\%%e\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe" (
                set "PATH=C:\Program Files\Microsoft Visual Studio\%%v\%%e\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja;!PATH!"
                goto :ninja_found
            )
            if exist "C:\Program Files (x86)\Microsoft Visual Studio\%%v\%%e\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe" (
                set "PATH=C:\Program Files (x86)\Microsoft Visual Studio\%%v\%%e\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja;!PATH!"
                goto :ninja_found
            )
        )
    )
    
    echo ERROR: Ninja not found! Please install Ninja or use Visual Studio with CMake tools
    pause
    exit /b 1
)

:ninja_found
echo Ninja version: 
ninja --version

:: Create build directory
echo.
echo Creating build directory: %BUILD_DIR%
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

:: Configure CMake with Ninja generator
echo.
echo ===============================================
echo              Configuring CMake
echo ===============================================
echo.

cd "%BUILD_DIR%"

cmake .. ^
    -G "Ninja" ^
    -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
    -DCMAKE_C_COMPILER=cl ^
    -DCMAKE_CXX_COMPILER=cl ^
    -DCMAKE_MAKE_PROGRAM=ninja ^
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ^
    -DCMAKE_VERBOSE_MAKEFILE=OFF

if errorlevel 1 (
    echo.
    echo ERROR: CMake configuration failed!
    cd ..
    pause
    exit /b 1
)

:: Build the project
echo.
echo ===============================================
echo                Building Project
echo ===============================================
echo.

:: Use all available CPU cores for building
set /a "NUM_CORES=%NUMBER_OF_PROCESSORS%"
echo Building with %NUM_CORES% parallel jobs...

ninja -j %NUM_CORES%

if errorlevel 1 (
    echo.
    echo ERROR: Build failed!
    cd ..
    pause
    exit /b 1
)

cd ..

echo.
echo ===============================================
echo              Build Completed Successfully!
echo ===============================================
echo.
echo Build type: %BUILD_TYPE%
echo Output directory: %BUILD_DIR%
echo Binary location: %BUILD_DIR%\Flarial.dll
echo.

:: Show build artifacts
if exist "%BUILD_DIR%\Flarial.dll" (
    echo Build artifacts:
    dir "%BUILD_DIR%\*.dll" "%BUILD_DIR%\*.exe" "%BUILD_DIR%\*.lib" 2>nul
)

echo.
echo Build completed! Press any key to exit...
pause >nul