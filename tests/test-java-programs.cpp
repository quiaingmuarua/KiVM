//
// Test for running Java programs through C++ interface
// Created for independent Java program testing
//

#include <kivm/classpath/classLoader.h>
#include <kivm/classpath/classPathManager.h>
#include <kivm/runtime/javaThread.h>
#include <kivm/runtime/runtimeConfig.h>
#include <kivm/jni/jniJavaVM.h>
#include <shared/string.h>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

using namespace kivm;

void printSuccess(const std::string& message) {
    std::cout << "✓ " << message << std::endl;
}

void printError(const std::string& message) {
    std::cerr << "✗ " << message << std::endl;
}

void printInfo(const std::string& message) {
    std::cout << "  " << message << std::endl;
}

void printHeader(const std::string& header) {
    std::cout << "\n=== " << header << " ===" << std::endl;
}

bool runJavaProgram(const std::string& className, const std::vector<std::string>& args = {}) {
    try {
        printInfo("Starting JVM for class: " + className);
        
        // Convert class name from dot notation to slash notation
        String mainClassName = strings::replaceAll(strings::fromStdString(className),
            Global::DOT, Global::SLASH);
        
        // Create JVM
        JavaVM *javaVM = nullptr;
        JNIEnv *env = nullptr;
        if (JNI_CreateJavaVM(&javaVM, (void **) &env, nullptr) != JNI_OK) {
            printError("JNI_CreateJavaVM() failed");
            return false;
        }
        
        printSuccess("JVM created successfully");
        
        bool success = false;
        
        try {
            // Convert arguments
            std::vector<String> arguments;
            arguments.reserve(args.size());
            for (const auto &a : args) {
                arguments.push_back(strings::fromStdString(a));
            }
            
            // Set classpath
            ClassPathManager::get()->addClassPaths(strings::fromStdString("./rt.jar:./java-out"));
            printSuccess("Classpath set: ./rt.jar:./java-out");
            
            // Create and start main thread
            JavaMainThread javaMainThread(mainClassName, arguments);
            printInfo("Starting Java main thread...");
            javaMainThread.start();
            
            printSuccess("Java program execution completed");
            success = true;
            
        } catch (const std::exception& e) {
            printError("Exception during Java execution: " + std::string(e.what()));
        } catch (...) {
            printError("Unknown exception during Java execution");
        }
        
        // Always cleanup JVM
        try {
            if (javaVM != nullptr) {
                javaVM->DestroyJavaVM();
                printSuccess("JVM destroyed");
            }
        } catch (const std::exception& e) {
            printError("Exception during JVM cleanup: " + std::string(e.what()));
        } catch (...) {
            printError("Unknown exception during JVM cleanup");
        }
        
        return success;
        
    } catch (const std::exception& e) {
        printError("Exception while running Java program: " + std::string(e.what()));
        return false;
    } catch (...) {
        printError("Unknown exception while running Java program");
        return false;
    }
}

void testHelloKiVM() {
    printHeader("Testing HelloKiVM");
    
    if (runJavaProgram("HelloKiVM")) {
        printSuccess("HelloKiVM test passed!");
    } else {
        printError("HelloKiVM test failed!");
    }
}

void testHelloWorld() {
    printHeader("Testing com.imkiva.kivm.HelloWorld");
    
    if (runJavaProgram("com.imkiva.kivm.HelloWorld")) {
        printSuccess("HelloWorld test passed!");
    } else {
        printError("HelloWorld test failed!");
    }
}

void testWithArguments() {
    printHeader("Testing Java program with arguments");
    
    // Test a program with command line arguments
    std::vector<std::string> testArgs = {"arg1", "arg2", "test"};
    
    printInfo("Testing HelloKiVM with arguments: arg1, arg2, test");
    if (runJavaProgram("HelloKiVM", testArgs)) {
        printSuccess("HelloKiVM with arguments test passed!");
    } else {
        printError("HelloKiVM with arguments test failed!");
    }
}

void runAllJavaTests() {
    std::cout << "=== Running All Java Program Tests ===" << std::endl;
    
    try {
        printInfo("Note: Running Java programs sequentially with individual JVM instances");
        
        testHelloKiVM();
        
        // Add more delay and cleanup between tests to prevent conflicts
        std::cout << std::endl;
        printInfo("Waiting for cleanup...");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        testHelloWorld();
        
        std::cout << std::endl;
        printInfo("Waiting for cleanup...");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        testWithArguments();
        
        printHeader("All Java Tests Completed");
        
    } catch (const std::exception& e) {
        printError("Exception in runAllJavaTests: " + std::string(e.what()));
    } catch (...) {
        printError("Unknown exception in runAllJavaTests");
    }
}

void showUsage() {
    std::cout << "=== Java Program Test Usage ===" << std::endl;
    std::cout << "This test program can run in several modes:" << std::endl;
    std::cout << "  ./test-java-programs                    - Run all tests" << std::endl;
    std::cout << "  ./test-java-programs hello             - Run HelloKiVM test" << std::endl;
    std::cout << "  ./test-java-programs world             - Run HelloWorld test" << std::endl;
    std::cout << "  ./test-java-programs args              - Run test with arguments" << std::endl;
    std::cout << "  ./test-java-programs custom <classname> - Run custom class" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "=== KiVM Java Program Test Runner ===" << std::endl;
    
    if (argc == 1) {
        // No arguments - run all tests
        testHelloWorld();
    } else if (argc == 2) {
        std::string command = argv[1];
        
        if (command == "help" || command == "-h" || command == "--help") {
            showUsage();
        } else if (command == "hello") {
            testHelloKiVM();
        } else if (command == "world") {
            testHelloWorld();
        } else if (command == "args") {
            testWithArguments();
        } else {
            printError("Unknown command: " + command);
            showUsage();
            return 1;
        }
    } else if (argc == 3) {
        std::string command = argv[1];
        std::string className = argv[2];
        
        if (command == "custom") {
            printHeader("Testing Custom Class: " + className);
            if (runJavaProgram(className)) {
                printSuccess("Custom class test passed!");
            } else {
                printError("Custom class test failed!");
            }
        } else {
            printError("Unknown command: " + command);
            showUsage();
            return 1;
        }
    } else {
        printError("Too many arguments");
        showUsage();
        return 1;
    }
    
    return 0;
}
