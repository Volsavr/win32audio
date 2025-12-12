@echo off
setlocal enabledelayedexpansion

echo ========================================
echo Building Encoding Tests
echo ========================================

REM Get the directory where this script is located
set "SCRIPT_DIR=%~dp0"
cd /d "%SCRIPT_DIR%"

REM Create build directory if it doesn't exist
if not exist "tests\build" mkdir tests\build
cd tests\build

REM Configure with CMake
echo Configuring with CMake...
cmake -G "Visual Studio 17 2022" -A x64 -S .. -B . -DCMAKE_BUILD_TYPE=Release 2>nul
if errorlevel 1 (
    echo Failed to configure with Visual Studio 17 2022
    echo Trying Visual Studio 16 2019...
    cmake -G "Visual Studio 16 2019" -A x64 -S .. -B . -DCMAKE_BUILD_TYPE=Release 2>nul
    if errorlevel 1 (
        echo Failed to configure with Visual Studio 16 2019
        echo Trying without generator specification...
        cmake -A x64 -S .. -B . -DCMAKE_BUILD_TYPE=Release
        if errorlevel 1 (
            echo Failed to configure CMake. Please ensure you have Visual Studio and CMake installed.
            cd ..\..
            pause
            exit /b 1
        )
    )
)

echo.
echo Building...
REM Build the project
cmake --build . --config Release
if errorlevel 1 (
    echo Build failed
    cd ..\..
    pause
    exit /b 1
)

cd ..\..

echo.
echo ========================================
echo Running Tests
echo ========================================
echo.

REM Run the test executable - check all possible locations
if exist "tests\build\Release\encoding_test.exe" (
    tests\build\Release\encoding_test.exe
) else if exist "tests\build\encoding_test.exe" (
    tests\build\encoding_test.exe
) else if exist "tests\build\Debug\encoding_test.exe" (
    tests\build\Debug\encoding_test.exe
) else (
    echo Test executable not found!
    echo Checked locations:
    echo   - tests\build\Release\encoding_test.exe
    echo   - tests\build\encoding_test.exe
    echo   - tests\build\Debug\encoding_test.exe
    echo.
    echo Looking for any .exe files:
    dir /s /b tests\build\*.exe 2>nul
    pause
    exit /b 1
)

echo.
echo ========================================
echo Tests completed
echo ========================================
pause
