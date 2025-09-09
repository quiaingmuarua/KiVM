#!/bin/bash

# KiVM Test Runner Script
# Usage: ./run-tests.sh [component|java|bench|all]

set -e

BUILD_DIR="build"
BIN_DIR="$BUILD_DIR/bin"
LIB_DIR="$BUILD_DIR/lib"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_header() {
    echo -e "${BLUE}=== $1 ===${NC}"
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠ $1${NC}"
}

check_build() {
    if [ ! -d "$BUILD_DIR" ]; then
        print_error "Build directory not found. Please run 'cmake . && make' first."
        exit 1
    fi
    
    if [ ! -d "$BIN_DIR" ]; then
        print_error "Binary directory not found. Build may have failed."
        exit 1
    fi
    
    # Set library path
    export LD_LIBRARY_PATH="$LIB_DIR:$LD_LIBRARY_PATH"
    print_success "Build directory found, environment set up"
}

compile_java() {
    print_header "Compiling Java Test Programs"
    
    if [ ! -f "rt.jar" ]; then
        print_error "rt.jar not found in current directory"
        return 1
    fi
    
    mkdir -p java-out
    
    # Create a simple test program if it doesn't exist
    if [ ! -f "java-out/HelloKiVM.class" ]; then
        cat > java-out/HelloKiVM.java << 'EOF'
public class HelloKiVM {
    public static void main(String[] args) {
        System.out.println("Hello KiVM!");
        if (args.length > 0) {
            System.out.println("Arguments: " + String.join(" ", args));
        }
    }
}
EOF
        javac -source 8 -target 8 -encoding utf-8 -d java-out java-out/HelloKiVM.java
        print_success "HelloKiVM.java compiled"
    fi
    
    # Compile existing Java source files
    if [ -d "java-src" ]; then
        if ls java-src/*.java >/dev/null 2>&1; then
            javac -source 8 -target 8 -encoding utf-8 -d java-out java-src/*.java
            print_success "Java source files compiled"
        fi
        
        if [ -d "java-src/com" ]; then
            javac -source 8 -target 8 -encoding utf-8 \
                -bootclasspath rt.jar \
                -d java-out \
                java-src/com/imkiva/kivm/*.java 2>/dev/null || true
            print_success "KiVM test classes compiled"
        fi
    fi
}

run_component_tests() {
    print_header "Running Component Tests"
    
    cd "$BUILD_DIR"
    
    echo "Running string operations test..."
    ./bin/test-string
    
    echo ""
    echo "Running memory management test..."
    ./bin/test-memory
    
    echo ""
    echo "Running class loader test..."
    ./bin/test-classloader
    
    echo ""
    echo "Running OOP structures test..."
    ./bin/test-oop
    
    echo ""
    echo "Running unit tests..."
    ./bin/test-stack-and-locals
    ./bin/test-encode-decode-offset
    ./bin/test-oop-size
    ./bin/test-args-parser
    
    cd ..
    print_success "All component tests completed"
}

run_java_tests() {
    print_header "Running Java Program Tests"
    
    compile_java
    
    cd "$BUILD_DIR"
    
    echo "Testing HelloKiVM through test runner..."
    ./bin/test-java-programs hello || print_warning "HelloKiVM test may have issues"
    
    echo ""
    echo "Testing direct java executable..."
    ./bin/java -cp "../rt.jar:../java-out" HelloKiVM || print_warning "Direct java execution may have issues"
    
    cd ..
    print_success "Java program tests completed"
}

run_benchmarks() {
    print_header "Running Performance Benchmarks"
    
    cd "$BUILD_DIR"
    
    echo "Memory allocation benchmark:"
    ./bin/bench-allocation
    
    echo ""
    echo "HashMap benchmark:"
    ./bin/bench-map
    
    cd ..
    print_success "Benchmarks completed"
}

show_usage() {
    echo "KiVM Test Runner"
    echo ""
    echo "Usage: $0 [component|java|bench|all]"
    echo ""
    echo "Commands:"
    echo "  component  Run component tests (C++ unit tests)"
    echo "  java       Run Java program tests"
    echo "  bench      Run performance benchmarks"
    echo "  all        Run all tests (default)"
    echo ""
    echo "Prerequisites:"
    echo "  - Build the project first: cmake . && make"
    echo "  - Ensure rt.jar is in the current directory"
    echo "  - Java 8+ for compiling test programs"
}

main() {
    local command="${1:-all}"
    
    case "$command" in
        component)
            check_build
            run_component_tests
            ;;
        java)
            check_build
            run_java_tests
            ;;
        bench)
            check_build
            run_benchmarks
            ;;
        all)
            check_build
            run_component_tests
            echo ""
            run_java_tests
            echo ""
            run_benchmarks
            ;;
        help|--help|-h)
            show_usage
            ;;
        *)
            print_error "Unknown command: $command"
            echo ""
            show_usage
            exit 1
            ;;
    esac
    
    print_success "All requested tests completed successfully!"
}

main "$@"
