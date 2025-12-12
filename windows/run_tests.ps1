# PowerShell script to build and run encoding tests
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Building Encoding Tests" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

$ErrorActionPreference = "Stop"
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $scriptDir

# Create build directory
$buildDir = Join-Path "tests" "build"
if (-not (Test-Path $buildDir)) {
    New-Item -ItemType Directory -Path $buildDir | Out-Null
}

Set-Location $buildDir

# Configure with CMake
Write-Host "`nConfiguring with CMake..." -ForegroundColor Yellow

$generators = @(
    "Visual Studio 17 2022",
    "Visual Studio 16 2019",
    "Visual Studio 15 2017"
)

$configured = $false
foreach ($generator in $generators) {
    try {
        Write-Host "Trying $generator..." -ForegroundColor Gray
        & cmake -G $generator -A x64 -S ".." -B "." -DCMAKE_BUILD_TYPE=Release 2>&1 | Out-Null
        if ($LASTEXITCODE -eq 0) {
            Write-Host "✓ Configured with $generator" -ForegroundColor Green
            $configured = $true
            break
        }
    }
    catch {
        continue
    }
}

if (-not $configured) {
    Write-Host "Trying without specific generator..." -ForegroundColor Gray
    & cmake -A x64 -S ".." -B "." -DCMAKE_BUILD_TYPE=Release
    if ($LASTEXITCODE -ne 0) {
        Write-Host "✗ Failed to configure CMake" -ForegroundColor Red
        Write-Host "Please ensure you have Visual Studio and CMake installed." -ForegroundColor Red
        Set-Location $scriptDir
        Read-Host "Press Enter to exit"
        exit 1
    }
}

# Build
Write-Host "`nBuilding..." -ForegroundColor Yellow
& cmake --build . --config Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "✗ Build failed" -ForegroundColor Red
    Set-Location $scriptDir
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host "✓ Build successful" -ForegroundColor Green

Set-Location $scriptDir

# Run tests
Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "Running Tests" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

$exePaths = @(
    "tests\build\Release\encoding_test.exe",
    "tests\build\encoding_test.exe",
    "tests\build\Debug\encoding_test.exe"
)

$exeFound = $false
foreach ($exePath in $exePaths) {
    if (Test-Path $exePath) {
        & $exePath
        $exeFound = $true
        break
    }
}

if (-not $exeFound) {
    Write-Host "✗ Test executable not found!" -ForegroundColor Red
    Write-Host "Checked locations:" -ForegroundColor Yellow
    foreach ($path in $exePaths) {
        Write-Host "  - $path" -ForegroundColor Gray
    }
    Write-Host "`nLooking for any .exe files:" -ForegroundColor Yellow
    Get-ChildItem -Path "tests\build" -Recurse -Filter "*.exe" | ForEach-Object {
        Write-Host "  Found: $($_.FullName)" -ForegroundColor Gray
    }
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "Tests completed" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Read-Host "Press Enter to exit"
