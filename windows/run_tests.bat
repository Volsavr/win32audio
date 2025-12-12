@echo off
echo ========================================
echo Building Encoding Tests
echo ========================================

REM Create build directory if it doesn't exist
if not exist "test_build" mkdir test_build
cd test_build

REM Configure with CMake
cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release ../.. -C ../test_CMakeLists.txt
if errorlevel 1 (
    echo Failed to configure with Visual Studio 17
    echo Trying Visual Studio 16...
    cmake -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Release ../.. -C ../test_CMakeLists.txt
)
if errorlevel 1 (
    echo Failed to configure with Visual Studio 16
    echo Trying Visual Studio 15...
    cmake -G "Visual Studio 15 2017" -A x64 -DCMAKE_BUILD_TYPE=Release ../.. -C ../test_CMakeLists.txt
)
if errorlevel 1 (
    echo Failed to configure CMake. Please ensure you have Visual Studio and CMake installed.
    cd ..
    pause
    exit /b 1
)

REM Build the project
cmake --build . --config Release
if errorlevel 1 (
    echo Build failed
    cd ..
    pause
    exit /b 1
)

cd ..

echo.
echo ========================================
echo Running Tests
echo ========================================
echo.

REM Run the test executable
if exist "test_build\bin\Release\encoding_test.exe" (
    test_build\bin\Release\encoding_test.exe
) else if exist "test_build\Release\encoding_test.exe" (
    test_build\Release\encoding_test.exe
) else if exist "test_build\encoding_test.exe" (
    test_build\encoding_test.exe
) else (
    echo Test executable not found!
    pause
    exit /b 1
)

echo.
echo ========================================
echo Tests completed
echo ========================================
pause
