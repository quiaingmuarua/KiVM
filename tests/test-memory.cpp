//
// Test for KiVM Memory Management functionality
// Created for independent C++ testing
//

#include <kivm/memory/universe.h>
#include <kivm/memory/copyingHeap.h>
#include <iostream>
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

bool initializeMemory() {
    try {
        std::cout << "Initializing KiVM Memory System..." << std::endl;
        
        // Initialize Universe
        Universe::initialize();
        printSuccess("Universe initialized");
        
        return true;
    } catch (const std::exception& e) {
        printError("Failed to initialize memory system: " + std::string(e.what()));
        return false;
    } catch (...) {
        printError("Failed to initialize memory system: Unknown error");
        return false;
    }
}

void testBasicAllocation() {
    std::cout << "\n=== Testing Basic Memory Allocation ===" << std::endl;
    
    try {
        // Test small allocations
        for (int i = 1; i <= 1024; i *= 2) {
            void* ptr = Universe::allocHeap(i);
            if (ptr != nullptr) {
                printSuccess("Allocated " + std::to_string(i) + " bytes");
                // Write some data to verify memory is usable
                memset(ptr, 0xAA, i);
            } else {
                printError("Failed to allocate " + std::to_string(i) + " bytes");
            }
        }
        
    } catch (const std::exception& e) {
        printError("Exception in basic allocation test: " + std::string(e.what()));
    } catch (...) {
        printError("Unknown exception in basic allocation test");
    }
}

void testAllocationPerformance() {
    std::cout << "\n=== Testing Allocation Performance ===" << std::endl;
    
    const int ALLOC_COUNT = 10000;
    const size_t ALLOC_SIZE = 64;
    
    try {
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < ALLOC_COUNT; ++i) {
            void* ptr = Universe::allocHeap(ALLOC_SIZE);
            if (ptr == nullptr) {
                printError("Allocation failed at iteration " + std::to_string(i));
                return;
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        printSuccess("Allocated " + std::to_string(ALLOC_COUNT) + " objects of " + 
                    std::to_string(ALLOC_SIZE) + " bytes each");
        printInfo("  Time taken: " + std::to_string(duration.count()) + " microseconds");
        printInfo("  Average: " + std::to_string(duration.count() / ALLOC_COUNT) + " μs per allocation");
        
    } catch (const std::exception& e) {
        printError("Exception in performance test: " + std::string(e.what()));
    } catch (...) {
        printError("Unknown exception in performance test");
    }
}

void testLargeAllocation() {
    std::cout << "\n=== Testing Large Allocations ===" << std::endl;
    
    try {
        // Test progressively larger allocations
        std::vector<size_t> sizes = {
            1024,      // 1KB
            10240,     // 10KB
            102400,    // 100KB
            1048576    // 1MB
        };
        
        for (size_t size : sizes) {
            void* ptr = Universe::allocHeap(size);
            if (ptr != nullptr) {
                printSuccess("Allocated " + std::to_string(size) + " bytes");
                // Test write access
                memset(ptr, 0x55, size);
                printInfo("  Memory write test passed");
            } else {
                printError("Failed to allocate " + std::to_string(size) + " bytes");
            }
        }
        
    } catch (const std::exception& e) {
        printError("Exception in large allocation test: " + std::string(e.what()));
    } catch (...) {
        printError("Unknown exception in large allocation test");
    }
}

int main() {
    std::cout << "=== KiVM Memory Management Test ===" << std::endl;
    
    if (!initializeMemory()) {
        return 1;
    }
    
    testBasicAllocation();
    testAllocationPerformance();
    testLargeAllocation();
    
    printSuccess("All Memory Management tests completed!");
    return 0;
}
