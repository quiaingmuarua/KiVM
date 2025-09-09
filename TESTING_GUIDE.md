# KiVM 测试指南

## 概述

现在你拥有了完整的独立测试系统，就像 `tests/` 目录下的其他测试一样。每个测试都有自己的 `main` 方法，可以随时独立运行和修改。

## 可用的测试

### 1. 组件测试 (C++ 功能测试)

这些测试直接测试KiVM的C++组件，无需完整的Java执行环境：

```bash
# 字符串操作测试
./test-string

# 内存管理测试
./test-memory

# 类加载器测试
./test-classloader

# 面向对象结构测试
./test-oop
```

**特点：**
- ✅ 快速启动，直接测试组件
- ✅ 便于调试特定功能
- ✅ 可以多次运行而无冲突

### 2. Java程序运行测试

这个测试工具可以直接运行你的Java程序，就像你之前在命令行中做的那样：

```bash
# 运行 HelloKiVM
./test-java-programs hello

# 运行 com.imkiva.kivm.HelloWorld  
./test-java-programs world

# 运行自定义Java类
./test-java-programs custom YourClassName

# 查看帮助
./test-java-programs help
```

**等效命令对照：**
- `./java -cp "./rt.jar:./java-out" HelloKiVM` → `./test-java-programs hello`
- `./java -cp "./rt.jar:./java-out" com.imkiva.kivm.HelloWorld` → `./test-java-programs world`

**注意：** 由于KiVM的JVM重复创建限制，建议单独运行每个Java程序测试，避免连续运行。

## 测试文件结构

```
tests/
├── test-classloader.cpp     # 类加载器测试 
├── test-memory.cpp          # 内存管理测试
├── test-string.cpp          # 字符串操作测试
├── test-oop.cpp             # OOP结构测试
├── test-java-programs.cpp   # Java程序运行测试
├── bench-allocation.cpp     # 内存分配基准测试
├── bench-map.cpp            # 哈希表基准测试
└── ...                      # 其他原有测试
```

## 使用示例

### 场景1: 测试字符串功能修改
```bash
# 修改 tests/test-string.cpp 中的测试代码
# 重新编译
make test-string

# 运行测试
./test-string
```

### 场景2: 测试新的Java程序
```bash
# 编译你的Java程序到 java-out/ 目录
javac -d java-out YourProgram.java

# 运行测试
./test-java-programs custom YourProgram
```

### 场景3: 调试类加载问题
```bash
# 运行类加载器测试
./test-classloader

# 如果有问题，修改 tests/test-classloader.cpp
# 重新编译并测试
make test-classloader && ./test-classloader
```

## 添加新测试

### 添加组件测试
1. 在 `tests/` 目录创建 `test-yourfeature.cpp`
2. 在 `CMakeLists.txt` 中添加：
   ```cmake
   add_test_target(yourfeature)
   ```
3. 编译并运行：
   ```bash
   make test-yourfeature
   ./test-yourfeature
   ```

### 测试模板
```cpp
//
// Test for KiVM YourFeature functionality
//

#include <kivm/your/headers.h>
#include <iostream>

void printSuccess(const std::string& message) {
    std::cout << "✓ " << message << std::endl;
}

void printError(const std::string& message) {
    std::cerr << "✗ " << message << std::endl;
}

int main() {
    std::cout << "=== KiVM YourFeature Test ===" << std::endl;
    
    try {
        // 你的测试代码
        printSuccess("Test passed!");
        return 0;
    } catch (const std::exception& e) {
        printError("Test failed: " + std::string(e.what()));
        return 1;
    }
}
```

## 测试输出示例

### 成功的组件测试
```
=== KiVM String Operations Test ===
Initializing minimal KiVM state...
✓ Universe initialized

=== Testing String Conversion ===
✓ Converted std::string to KiVM String: Hello KiVM
✓ Round-trip conversion: 'Simple ASCII'
✓ Round-trip conversion: 'Numbers: 123456789'

=== Testing String Replacement ===
✓ String replacement test: com/example/Test
✓ Replace test: 'hello world hello' -> 'hi world hi'

✓ All String Operations tests completed!
```

### 成功的Java程序测试
```
=== Testing HelloKiVM ===
  Starting JVM for class: HelloKiVM
✓ JVM created successfully
✓ Classpath set: ./rt.jar:./java-out
  Starting Java main thread...
Hello KiVM!
✓ Java program execution completed
✓ JVM destroyed
✓ HelloKiVM test passed!
```

## 优势

✅ **独立性**: 每个测试都有自己的main方法，完全独立  
✅ **便于调试**: 可以直接运行特定测试，快速定位问题  
✅ **易于修改**: 测试代码分离，修改不影响主程序  
✅ **标准化**: 遵循tests/目录的标准模式  
✅ **高效**: 组件测试启动快，Java测试直接调用  

## 最佳实践

1. **开发时**: 优先使用组件测试 (`test-*`) 进行快速验证
2. **集成测试**: 使用Java程序测试验证端到端功能
3. **调试**: 修改测试代码来复现和定位问题
4. **持续测试**: 每次修改后运行相关测试确保功能正常

现在你可以像使用其他专业测试工具一样，方便地测试和调试KiVM的各个组件！
