#include <windows.h>
#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include "include/encoding.h"

struct TestCase {
    std::wstring input;
    std::string description;
};

void PrintUtf8Bytes(const std::string& str) {
    std::cout << "  Bytes: ";
    for (unsigned char c : str) {
        printf("%02X ", c);
    }
    std::cout << std::endl;
}

bool IsValidUtf8(const std::string& str) {
    size_t i = 0;
    while (i < str.length()) {
        unsigned char c = str[i];

        if (c <= 0x7F) {
            i++;
        } else if ((c & 0xE0) == 0xC0) {
            if (i + 1 >= str.length()) return false;
            if ((str[i + 1] & 0xC0) != 0x80) return false;
            i += 2;
        } else if ((c & 0xF0) == 0xE0) {
            if (i + 2 >= str.length()) return false;
            if ((str[i + 1] & 0xC0) != 0x80) return false;
            if ((str[i + 2] & 0xC0) != 0x80) return false;
            i += 3;
        } else if ((c & 0xF8) == 0xF0) {
            if (i + 3 >= str.length()) return false;
            if ((str[i + 1] & 0xC0) != 0x80) return false;
            if ((str[i + 2] & 0xC0) != 0x80) return false;
            if ((str[i + 3] & 0xC0) != 0x80) return false;
            i += 4;
        } else {
            return false;
        }
    }
    return true;
}

void TestWideToUtf8() {
    std::cout << "\n=== Testing WideToUtf8 ===" << std::endl;

    std::vector<TestCase> testCases = {
        {L"", "Empty string"},
        {L"Hello World", "ASCII only"},
        {L"Café", "French accent (é)"},
        {L"Crème brûlée", "Multiple French accents"},
        {L"Zürich", "German umlaut (ü)"},
        {L"Münchën", "Multiple umlauts"},
        {L"Москва", "Russian Cyrillic"},
        {L"日本語", "Japanese characters"},
        {L"Hello 世界", "Mixed ASCII and Japanese"},
        {L"😀🎉🔥", "Emojis"},
        {L"Test™®©", "Special symbols"},
        {L"Ñoño", "Spanish ñ"},
        {L"Αλφάβητο", "Greek alphabet"},
        {L"Device™ (HD Audio®)", "Realistic device name with symbols"},
        {L"{0.0.0.00000000}.{f3456789-1234-5678-90ab-cdef12345678}", "GUID-like string"}
    };

    int passCount = 0;
    int failCount = 0;

    for (const auto& test : testCases) {
        std::cout << "\nTest: " << test.description << std::endl;
        std::wcout << L"  Input: " << test.input << std::endl;

        std::string result = Encoding::WideToUtf8(test.input);
        std::cout << "  Output length: " << result.length() << " bytes" << std::endl;

        if (!result.empty() || test.input.empty()) {
            PrintUtf8Bytes(result);
        }

        bool isValid = IsValidUtf8(result);
        std::cout << "  Valid UTF-8: " << (isValid ? "YES" : "NO") << std::endl;

        if (!isValid && !test.input.empty()) {
            std::cout << "  ❌ FAILED: Invalid UTF-8 encoding" << std::endl;
            failCount++;
        } else {
            std::cout << "  ✓ PASSED" << std::endl;
            passCount++;
        }
    }

    std::cout << "\n--- WideToUtf8 Summary ---" << std::endl;
    std::cout << "Passed: " << passCount << "/" << testCases.size() << std::endl;
    std::cout << "Failed: " << failCount << "/" << testCases.size() << std::endl;
}

void TestRoundTrip() {
    std::cout << "\n=== Testing Round-trip Conversion ===" << std::endl;

    std::vector<TestCase> testCases = {
        {L"Café", "French"},
        {L"Zürich", "German"},
        {L"日本語", "Japanese"},
        {L"😀", "Emoji"},
        {L"Device™ Audio®", "Device name"}
    };

    int passCount = 0;
    int failCount = 0;

    for (const auto& test : testCases) {
        std::cout << "\nTest: " << test.description << std::endl;
        std::wcout << L"  Original: " << test.input << std::endl;

        std::string utf8 = Encoding::WideToUtf8(test.input);
        std::wstring roundTrip = Encoding::Utf8ToWide(utf8);

        std::wcout << L"  Round-trip: " << roundTrip << std::endl;

        bool matches = (test.input == roundTrip);
        std::cout << "  Match: " << (matches ? "YES" : "NO") << std::endl;

        if (!matches) {
            std::cout << "  ❌ FAILED: Round-trip conversion failed" << std::endl;
            failCount++;
        } else {
            std::cout << "  ✓ PASSED" << std::endl;
            passCount++;
        }
    }

    std::cout << "\n--- Round-trip Summary ---" << std::endl;
    std::cout << "Passed: " << passCount << "/" << testCases.size() << std::endl;
    std::cout << "Failed: " << failCount << "/" << testCases.size() << std::endl;
}

void TestEdgeCases() {
    std::cout << "\n=== Testing Edge Cases ===" << std::endl;

    std::cout << "\n1. Empty string" << std::endl;
    std::string empty = Encoding::WideToUtf8(L"");
    std::cout << "  Result length: " << empty.length() << std::endl;
    std::cout << "  " << (empty.empty() ? "✓ PASSED" : "❌ FAILED") << std::endl;

    std::cout << "\n2. Very long string with special characters" << std::endl;
    std::wstring longStr;
    for (int i = 0; i < 100; i++) {
        longStr += L"Café ";
    }
    std::string longResult = Encoding::WideToUtf8(longStr);
    bool longValid = IsValidUtf8(longResult);
    std::cout << "  Length: " << longResult.length() << " bytes" << std::endl;
    std::cout << "  Valid UTF-8: " << (longValid ? "YES" : "NO") << std::endl;
    std::cout << "  " << (longValid ? "✓ PASSED" : "❌ FAILED") << std::endl;

    std::cout << "\n3. Single special character" << std::endl;
    std::string single = Encoding::WideToUtf8(L"é");
    bool singleValid = IsValidUtf8(single);
    std::cout << "  Result: é" << std::endl;
    std::cout << "  Length: " << single.length() << " bytes" << std::endl;
    PrintUtf8Bytes(single);
    std::cout << "  Valid UTF-8: " << (singleValid ? "YES" : "NO") << std::endl;
    std::cout << "  " << (singleValid ? "✓ PASSED" : "❌ FAILED") << std::endl;
}

void TestRealisticDeviceNames() {
    std::cout << "\n=== Testing Realistic Audio Device Names ===" << std::endl;

    std::vector<TestCase> deviceNames = {
        {L"Haut-parleurs (Realtek® HD Audio)", "French device with ®"},
        {L"Périphérique audio haute définition", "French audio device"},
        {L"Lautsprecher (Realtek® HD Audio)", "German device"},
        {L"Auriculares (Conexant™ Audio)", "Spanish with ™"},
        {L"Headphones™ (HD Audio®) - Öko Edition", "Mixed special chars"},
        {L"Microphone (Realtek® HD Audiö™)", "Multiple symbols"},
        {L"扬声器 (HD Audio)", "Chinese characters"},
        {L"スピーカー (HD Audio)", "Japanese characters"},
    };

    int passCount = 0;
    int failCount = 0;

    for (const auto& test : deviceNames) {
        std::cout << "\nDevice: " << test.description << std::endl;
        std::wcout << L"  Name: " << test.input << std::endl;

        std::string utf8 = Encoding::WideToUtf8(test.input);
        bool isValid = IsValidUtf8(utf8);

        std::cout << "  UTF-8 length: " << utf8.length() << " bytes" << std::endl;
        std::cout << "  Valid UTF-8: " << (isValid ? "YES" : "NO") << std::endl;

        if (isValid) {
            std::wstring roundTrip = Encoding::Utf8ToWide(utf8);
            bool matches = (test.input == roundTrip);
            std::cout << "  Round-trip OK: " << (matches ? "YES" : "NO") << std::endl;

            if (matches) {
                std::cout << "  ✓ PASSED" << std::endl;
                passCount++;
            } else {
                std::cout << "  ❌ FAILED: Round-trip mismatch" << std::endl;
                failCount++;
            }
        } else {
            std::cout << "  ❌ FAILED: Invalid UTF-8" << std::endl;
            failCount++;
        }
    }

    std::cout << "\n--- Realistic Device Names Summary ---" << std::endl;
    std::cout << "Passed: " << passCount << "/" << deviceNames.size() << std::endl;
    std::cout << "Failed: " << failCount << "/" << deviceNames.size() << std::endl;
}

int main() {
    std::cout << "==================================" << std::endl;
    std::cout << "Encoding Functions Test Suite" << std::endl;
    std::cout << "==================================" << std::endl;

    try {
        TestWideToUtf8();
        TestRoundTrip();
        TestEdgeCases();
        TestRealisticDeviceNames();

        std::cout << "\n==================================" << std::endl;
        std::cout << "All tests completed!" << std::endl;
        std::cout << "==================================" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
