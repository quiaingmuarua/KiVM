//
// KiVM Test Framework Header
// Created for testing KiVM components independently
//
#pragma once

#include <string>

namespace kivm {
    namespace test {
        // Test framework interface
        class TestFramework {
        public:
            // Initialize test environment
            static bool initialize(const std::string& classpath = "");
            
            // Cleanup test environment
            static void cleanup();
            
            // Individual test functions
            static void testClassLoader();
            static void testMemoryManagement();
            static void testOOPStructures();
            static void testBytecodeExecution();
            static void testStringOperations();
            
            // Run all tests
            static void runAllTests();
            
            // Run specific test by name
            static bool runTest(const std::string& testName);
            
        private:
            static bool isInitialized;
            static bool initializeMinimalVM();
        };
        
        // Test utility functions
        namespace utils {
            void printTestHeader(const std::string& testName);
            void printSuccess(const std::string& message);
            void printError(const std::string& message);
            void printInfo(const std::string& message);
        }
    }
}
