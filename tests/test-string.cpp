//
// Test for KiVM String Operations functionality
// Created for independent C++ testing
//

#include <shared/string.h>
#include <kivm/memory/universe.h>
#include <kivm/classpath/classPathManager.h>
#include <iostream>
#include <locale>
#include <codecvt>

using namespace kivm;

// Helper function to convert wide string to std::string
std::string wstringToString(const wchar_t* wstr) {
    if (!wstr) return "";
    try {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.to_bytes(wstr);
    } catch (...) {
        // Fallback: simple conversion (may lose characters)
        std::wstring ws(wstr);
        return std::string(ws.begin(), ws.end());
    }
}

void printSuccess(const std::string& message) {
    std::cout << "✓ " << message << std::endl;
}

void printError(const std::string& message) {
    std::cerr << "✗ " << message << std::endl;
}

void printInfo(const std::string& message) {
    std::cout << "  " << message << std::endl;
}

bool initializeMinimalVM() {
    try {
        std::cout << "Initializing minimal KiVM state..." << std::endl;
        
        // Initialize Universe
        Universe::initialize();
        printSuccess("Universe initialized");
        
        return true;
    } catch (const std::exception& e) {
        printError("Failed to initialize minimal VM: " + std::string(e.what()));
        return false;
    } catch (...) {
        printError("Failed to initialize minimal VM: Unknown error");
        return false;
    }
}

void testStringConversion() {
    std::cout << "\n=== Testing String Conversion ===" << std::endl;
    
    try {
        // Test std::string to KiVM String conversion
        std::string testStdString = "Hello KiVM";
        String kivmString = strings::fromStdString(testStdString);
        printSuccess("Converted std::string to KiVM String: " + wstringToString(kivmString.c_str()));
        
        // Test various string contents
        std::vector<std::string> testStrings = {
            "Simple ASCII",
            "Numbers: 123456789",
            "Special chars: !@#$%^&*()",
            "Path/separator\\test",
            ""  // Empty string
        };
        
        for (const auto& str : testStrings) {
            String converted = strings::fromStdString(str);
            std::string back = wstringToString(converted.c_str());
            if (back == str) {
                printSuccess("Round-trip conversion: '" + str + "'");
            } else {
                printError("Round-trip failed for: '" + str + "' -> '" + back + "'");
            }
        }
        
    } catch (const std::exception& e) {
        printError("Exception in string conversion test: " + std::string(e.what()));
    } catch (...) {
        printError("Unknown exception in string conversion test");
    }
}

void testStringReplacement() {
    std::cout << "\n=== Testing String Replacement ===" << std::endl;
    
    try {
        // Test basic replacement
        String testStr = L"com.example.Test";
        String replaced = strings::replaceAll(testStr, L".", L"/");
        printSuccess("String replacement test: " + wstringToString(replaced.c_str()));
        
        // Test multiple replacements
        struct TestCase {
            std::wstring input;
            std::wstring find;
            std::wstring replace;
            std::wstring expected;
        };
        
        std::vector<TestCase> testCases = {
            {L"hello world hello", L"hello", L"hi", L"hi world hi"},
            {L"aaa", L"a", L"bb", L"bbbbbb"},
            {L"no change", L"xyz", L"abc", L"no change"},
            {L"", L"test", L"replace", L""},
            {L"test", L"", L"x", L"test"},  // Empty find string should not change
            {L"multiple...dots...here", L"...", L".", L"multiple.dots.here"}
        };
        
        for (const auto& testCase : testCases) {
            String result = strings::replaceAll(testCase.input, testCase.find, testCase.replace);
            std::wstring resultStr(result.c_str());
            
            if (resultStr == testCase.expected) {
                printSuccess("Replace test: '" + wstringToString(testCase.input.c_str()) + "' -> '" + 
                           wstringToString(result.c_str()) + "'");
            } else {
                printError("Replace test failed: '" + wstringToString(testCase.input.c_str()) + 
                          "' expected '" + wstringToString(testCase.expected.c_str()) + 
                          "' but got '" + wstringToString(result.c_str()) + "'");
            }
        }
        
    } catch (const std::exception& e) {
        printError("Exception in string replacement test: " + std::string(e.what()));
    } catch (...) {
        printError("Unknown exception in string replacement test");
    }
}

void testStringUtilities() {
    std::cout << "\n=== Testing String Utilities ===" << std::endl;
    
    try {
        // Test string length
        String testStr = strings::fromStdString("Hello");
        if (testStr.length() == 5) {
            printSuccess("String length test passed");
        } else {
            printError("String length test failed: expected 5, got " + std::to_string(testStr.length()));
        }
        
        // Test empty string
        String emptyStr = strings::fromStdString("");
        if (emptyStr.length() == 0) {
            printSuccess("Empty string test passed");
        } else {
            printError("Empty string test failed: expected 0, got " + std::to_string(emptyStr.length()));
        }
        
        // Test string comparison
        String str1 = strings::fromStdString("test");
        String str2 = strings::fromStdString("test");
        String str3 = strings::fromStdString("different");
        
        if (str1 == str2) {
            printSuccess("String equality test passed");
        } else {
            printError("String equality test failed");
        }
        
        if (str1 != str3) {
            printSuccess("String inequality test passed");
        } else {
            printError("String inequality test failed");
        }
        
    } catch (const std::exception& e) {
        printError("Exception in string utilities test: " + std::string(e.what()));
    } catch (...) {
        printError("Unknown exception in string utilities test");
    }
}

int main() {
    std::cout << "=== KiVM String Operations Test ===" << std::endl;
    
    if (!initializeMinimalVM()) {
        return 1;
    }
    
    testStringConversion();
    testStringReplacement();
    testStringUtilities();
    
    printSuccess("All String Operations tests completed!");
    return 0;
}
