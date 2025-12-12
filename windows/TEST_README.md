# Encoding Functions Test Suite

This directory contains tests for the UTF-8 encoding fix that resolves the issue with special characters in Windows audio device names.

## The Problem

The original implementation had a bug where `WideToUtf8()` used inconsistent parameters in its two calls to `WideCharToMultiByte()`:
- First call: used `wstr.length()` to calculate buffer size
- Second call: used `-1` (null-terminated) to perform the conversion

This inconsistency caused malformed UTF-8 sequences when device names contained special characters (é, ü, ™, ®, etc.), leading to Flutter crashes.

## The Fix

Both calls now consistently use `wstr.length()` for proper UTF-8 conversion:
```cpp
int count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
    static_cast<int>(wstr.length()), nullptr, 0, nullptr, nullptr);

std::string str(count, 0);
WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
    static_cast<int>(wstr.length()), &str[0], count, nullptr, nullptr);
```

Additional improvements:
- Added empty string checks
- Added error handling for failed conversions
- Fixed the same issue in `WideToAnsi()` function

## Running the Tests

### Windows (Recommended)

Simply run the batch file:
```cmd
run_tests.bat
```

This will:
1. Create a build directory
2. Configure CMake with Visual Studio
3. Build the test executable
4. Run all tests
5. Display results

### Manual Build

If you prefer to build manually:

```cmd
cd tests
mkdir build
cd build
cmake -G "Visual Studio 17 2022" -A x64 ..
cmake --build . --config Release
.\Release\encoding_test.exe
```

## Test Coverage

The test suite includes:

### 1. **Basic UTF-8 Tests**
- Empty strings
- ASCII-only strings
- French accents (é, è, ê, à)
- German umlauts (ü, ö, ä)
- Spanish characters (ñ)
- Greek alphabet (Α, β, γ)

### 2. **Complex Character Tests**
- Cyrillic (Їжак)
- Japanese characters (日本語)
- Chinese characters (扬声器)
- Emojis (😀, 🎉, 🔥)
- Special symbols (™, ®, ©)

### 3. **Round-trip Conversion Tests**
Tests that converting Wide → UTF-8 → Wide returns the original string

### 4. **Realistic Device Names**
Tests actual audio device name patterns:
- `Haut-parleurs (Realtek® HD Audio)` (French)
- `Lautsprecher (Realtek® HD Audio)` (German)
- `Auriculares (Conexant™ Audio)` (Spanish)
- `扬声器 (HD Audio)` (Chinese)
- `スピーカー (HD Audio)` (Japanese)

### 5. **Edge Cases**
- Very long strings with special characters
- Single special characters
- Empty strings
- GUIDs with special formatting

## Expected Output

All tests should pass with output like:
```
=== Testing WideToUtf8 ===
Test: French accent (é)
  Input: Café
  Output length: 5 bytes
  Bytes: 43 61 66 C3 A9
  Valid UTF-8: YES
  ✓ PASSED
```

## UTF-8 Validation

The test suite includes a UTF-8 validator that checks:
- Proper byte sequences (1-4 bytes per character)
- Correct continuation bytes (0x80-0xBF)
- Valid UTF-8 encoding patterns
- No malformed sequences

## Integration

These tests verify that the fix will work correctly when:
- Windows enumerates audio devices
- Device names are sent to Flutter via method channels
- Flutter's UTF-8 decoder receives the strings
- Device state changes are monitored

## Requirements

- Windows OS (uses Windows-specific APIs)
- CMake 3.14 or later
- Visual Studio 2017 or later with C++ tools
- Windows SDK

## Notes

- Tests must be run on Windows (uses `WideCharToMultiByte` API)
- The test executable is standalone and doesn't require Flutter
- All string conversions are tested for valid UTF-8 encoding
- Failed tests will display hex dumps of malformed bytes
