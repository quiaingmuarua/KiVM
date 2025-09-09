# KiVM C++ 测试框架

## 概述

现在你可以直接通过命令行运行 C++ 测试，而无需完整的 Java 程序执行流程。这个测试框架被设计为独立模块，便于调试和开发。

## 架构

测试代码已从主要的 `java.cpp` 文件中分离出来：

- **头文件**: `include/kivm/test/testFramework.h`
- **实现文件**: `src/kivm/test/testFramework.cpp`
- **主程序**: `src/bin/java.cpp` (只包含调用逻辑)

## 使用方法

### 基本语法
```bash
./java --test <test-name>
```

### 可用的测试

1. **字符串操作测试**
   ```bash
   ./java --test string
   ```
   - 测试 KiVM 字符串转换函数
   - 测试字符串替换功能

2. **内存管理测试**
   ```bash
   ./java --test memory
   ```
   - 验证内存管理系统初始化
   - 基本内存分配测试

3. **类加载器测试**
   ```bash
   ./java --test classloader
   ```
   - 测试 Bootstrap ClassLoader
   - 加载核心 Java 类 (Object, String)
   - 显示类状态信息

4. **面向对象结构测试**
   ```bash
   ./java --test oop
   ```
   - 检查类层次结构
   - 验证类属性和元数据

5. **字节码执行测试**
   ```bash
   ./java --test bytecode
   ```
   - 验证字节码执行框架可用性

6. **运行所有测试**
   ```bash
   ./java --test all
   ```
   - 依次运行所有测试模块

### 查看帮助
```bash
./java -h
```

## 示例输出

```bash
$ ./java --test string
=== KiVM C++ Test Mode ===
Running test: string

  Initializing KiVM Test Framework...
  Using default classpath: ./rt.jar
✓ Classpath initialized
  Initializing minimal KiVM state...
✓ Universe initialized
✓ Test framework initialized successfully
=== String Operations ===
✓ Converted std::string to KiVM String: Hello KiVM
✓ String replacement test: com/example/Test
✓ String operations test completed
  Cleaning up test framework...
✓ Test framework cleaned up

=== Test completed ===
```

## 添加新测试

如果你想添加新的测试功能：

1. 在 `include/kivm/test/testFramework.h` 中声明新的测试函数
2. 在 `src/kivm/test/testFramework.cpp` 中实现测试逻辑
3. 在 `TestFramework::runTest()` 方法中添加新的测试案例
4. 更新帮助文档

## 优势

- **快速调试**: 直接测试特定组件，无需完整的 Java 执行流程
- **独立模块**: 测试代码与主要逻辑分离，易于维护
- **灵活性**: 可以单独测试任何组件或运行所有测试
- **可扩展**: 易于添加新的测试功能

## 注意事项

- 测试框架会自动设置最小的 VM 环境
- 默认使用 `./rt.jar` 作为类路径
- 可以通过 `-cp` 选项指定自定义类路径
- 某些测试可能需要完整的 JVM 初始化（如字节码执行）
