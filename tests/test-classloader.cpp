//
// Test for KiVM ClassLoader functionality
// Created for independent C++ testing
//

#include <kivm/classpath/classLoader.h>
#include <kivm/classpath/classPathManager.h>
#include <kivm/oop/instanceKlass.h>
#include <kivm/memory/universe.h>
#include <shared/string.h>
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
        
        // Set default classpath
        ClassPathManager::get()->addClassPaths(strings::fromStdString("./rt.jar"));
        printSuccess("Classpath initialized");
        
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

int main() {
    std::cout << "=== KiVM ClassLoader Test ===" << std::endl;
    
    try {
        if (!initializeMinimalVM()) {
            return 1;
        }
        
        // Test bootstrap class loader
        auto bootstrapCL = BootstrapClassLoader::get();
        printSuccess("Bootstrap ClassLoader obtained");
        
        // Test loading a basic class (Object)
        auto objectClass = bootstrapCL->loadClass(L"java/lang/Object");
        if (objectClass != nullptr) {
            printSuccess("Successfully loaded java.lang.Object");
            printInfo("  Class name: " + wstringToString(objectClass->getName().c_str()));
            printInfo("  Class state: " + std::to_string((int)objectClass->getClassState()));
        } else {
            printError("Failed to load java.lang.Object");
            return 1;
        }
        
        // Test loading String class
        auto stringClass = bootstrapCL->loadClass(L"java/lang/String");
        if (stringClass != nullptr) {
            printSuccess("Successfully loaded java.lang.String");
            printInfo("  Class name: " + wstringToString(stringClass->getName().c_str()));
        } else {
            printError("Failed to load java.lang.String");
            return 1;
        }
        
        // Test loading a non-existent class
        auto nonExistentClass = bootstrapCL->loadClass(L"java/lang/NonExistentClass");
        if (nonExistentClass == nullptr) {
            printSuccess("Correctly failed to load non-existent class");
        } else {
            printError("Unexpectedly loaded non-existent class");
            return 1;
        }
        
        printSuccess("All ClassLoader tests passed!");
        return 0;
        
    } catch (const std::exception& e) {
        printError("Exception in testClassLoader: " + std::string(e.what()));
        return 1;
    } catch (...) {
        printError("Unknown exception in testClassLoader");
        return 1;
    }
}
