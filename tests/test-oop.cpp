//
// Test for KiVM Object-Oriented Programming structures
// Created for independent C++ testing
//

#include <kivm/classpath/classLoader.h>
#include <kivm/classpath/classPathManager.h>
#include <kivm/oop/instanceKlass.h>
#include <kivm/oop/method.h>
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
        
        // Set classpath
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

void testClassStructure() {
    std::cout << "\n=== Testing Class Structure ===" << std::endl;
    
    try {
        auto bootstrapCL = BootstrapClassLoader::get();
        
        // Test Object class
        auto objectClass = (InstanceKlass*)bootstrapCL->loadClass(L"java/lang/Object");
        if (objectClass != nullptr) {
            printSuccess("Loaded java.lang.Object as InstanceKlass");
            
            // Check basic properties
            printInfo("  Class name: " + wstringToString(objectClass->getName().c_str()));
            printInfo("  Is interface: " + std::string(objectClass->isInterface() ? "true" : "false"));
            printInfo("  Is abstract: " + std::string(objectClass->isAbstract() ? "true" : "false"));
            printInfo("  Is final: " + std::string(objectClass->isFinal() ? "true" : "false"));
            
            // Check class hierarchy
            auto superClass = objectClass->getSuperClass();
            printInfo("  Super class: " + std::string(superClass ? "exists" : "null (expected for Object)"));
            
            if (superClass == nullptr) {
                printSuccess("Object class correctly has no superclass");
            } else {
                printError("Object class unexpectedly has a superclass");
            }
            
        } else {
            printError("Failed to load java.lang.Object for OOP testing");
            return;
        }
        
        // Test String class
        auto stringClass = (InstanceKlass*)bootstrapCL->loadClass(L"java/lang/String");
        if (stringClass != nullptr) {
            printSuccess("Loaded java.lang.String as InstanceKlass");
            
            printInfo("  Class name: " + wstringToString(stringClass->getName().c_str()));
            printInfo("  Is interface: " + std::string(stringClass->isInterface() ? "true" : "false"));
            printInfo("  Is abstract: " + std::string(stringClass->isAbstract() ? "true" : "false"));
            printInfo("  Is final: " + std::string(stringClass->isFinal() ? "true" : "false"));
            
            // Check inheritance
            auto stringSuperClass = stringClass->getSuperClass();
            if (stringSuperClass != nullptr) {
                printSuccess("String class has a superclass: " + wstringToString(stringSuperClass->getName().c_str()));
            } else {
                printError("String class unexpectedly has no superclass");
            }
            
        } else {
            printError("Failed to load java.lang.String for OOP testing");
        }
        
    } catch (const std::exception& e) {
        printError("Exception in class structure test: " + std::string(e.what()));
    } catch (...) {
        printError("Unknown exception in class structure test");
    }
}

void testMethodAccess() {
    std::cout << "\n=== Testing Method Access ===" << std::endl;
    
    try {
        auto bootstrapCL = BootstrapClassLoader::get();
        auto objectClass = (InstanceKlass*)bootstrapCL->loadClass(L"java/lang/Object");
        
        if (objectClass != nullptr) {
            // Try to find common Object methods
            std::vector<std::pair<std::wstring, std::wstring>> methodsToFind = {
                {L"toString", L"()Ljava/lang/String;"},
                {L"equals", L"(Ljava/lang/Object;)Z"},
                {L"hashCode", L"()I"},
                {L"getClass", L"()Ljava/lang/Class;"}
            };
            
            for (const auto& methodInfo : methodsToFind) {
                auto method = objectClass->getThisClassMethod(methodInfo.first, methodInfo.second);
                if (method != nullptr) {
                    printSuccess("Found method: " + wstringToString(methodInfo.first.c_str()));
                    printInfo("  Signature: " + wstringToString(methodInfo.second.c_str()));
                    printInfo("  Is native: " + std::string(method->isNative() ? "true" : "false"));
                    printInfo("  Is static: " + std::string(method->isStatic() ? "true" : "false"));
                    printInfo("  Is public: " + std::string(method->isPublic() ? "true" : "false"));
                } else {
                    printError("Failed to find method: " + wstringToString(methodInfo.first.c_str()));
                }
            }
            
        } else {
            printError("Failed to load Object class for method testing");
        }
        
    } catch (const std::exception& e) {
        printError("Exception in method access test: " + std::string(e.what()));
    } catch (...) {
        printError("Unknown exception in method access test");
    }
}

void testClassStates() {
    std::cout << "\n=== Testing Class States ===" << std::endl;
    
    try {
        auto bootstrapCL = BootstrapClassLoader::get();
        
        // Test different classes and their states
        std::vector<std::wstring> classesToTest = {
            L"java/lang/Object",
            L"java/lang/String",
            L"java/lang/Class",
            L"java/lang/System"
        };
        
        for (const auto& className : classesToTest) {
            auto klass = bootstrapCL->loadClass(className);
            if (klass != nullptr) {
                printSuccess("Loaded class: " + wstringToString(className.c_str()));
                
                ClassState state = klass->getClassState();
                std::string stateStr;
                switch (state) {
                    case ClassState::ALLOCATED:
                        stateStr = "ALLOCATED";
                        break;
                    case ClassState::LOADED:
                        stateStr = "LOADED";
                        break;
                    case ClassState::LINKED:
                        stateStr = "LINKED";
                        break;
                    case ClassState::BEING_INITIALIZED:
                        stateStr = "BEING_INITIALIZED";
                        break;
                    case ClassState::FULLY_INITIALIZED:
                        stateStr = "FULLY_INITIALIZED";
                        break;
                    case ClassState::INITIALIZATION_ERROR:
                        stateStr = "INITIALIZATION_ERROR";
                        break;
                    default:
                        stateStr = "UNKNOWN(" + std::to_string((int)state) + ")";
                        break;
                }
                printInfo("  State: " + stateStr);
                
            } else {
                printError("Failed to load class: " + wstringToString(className.c_str()));
            }
        }
        
    } catch (const std::exception& e) {
        printError("Exception in class states test: " + std::string(e.what()));
    } catch (...) {
        printError("Unknown exception in class states test");
    }
}

int main() {
    std::cout << "=== KiVM OOP Structures Test ===" << std::endl;
    
    if (!initializeMinimalVM()) {
        return 1;
    }
    
    testClassStructure();
    testMethodAccess();
    testClassStates();
    
    printSuccess("All OOP Structures tests completed!");
    return 0;
}
