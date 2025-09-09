# KiVM 持续集成指南

## 概述

KiVM项目现已配置完整的CI/CD流程，包括GitHub Actions自动化测试和本地测试脚本。

## GitHub Actions 工作流

### 主要CI工作流 (`.github/workflows/ci.yml`)

**触发条件：**
- 推送到 `master`, `main`, `develop` 分支
- 对这些分支的Pull Request

**测试矩阵：**
- 操作系统: Ubuntu 20.04, Ubuntu 22.04
- 编译器: GCC, Clang
- Java版本: OpenJDK 8

**测试阶段：**

1. **环境设置**
   - 安装JDK 8
   - 安装系统依赖：cmake, libffi-dev, libzip-dev, zlib1g-dev
   - 配置编译器环境

2. **构建验证**
   - 验证rt.jar存在
   - CMake配置和构建
   - 验证编译产物

3. **Java程序编译**
   - 编译测试用的Java程序
   - 生成HelloKiVM.class等测试类

4. **组件测试**
   - 字符串操作测试
   - 内存管理测试
   - 类加载器测试
   - OOP结构测试
   - 原有单元测试

5. **Java程序测试**
   - 通过test-java-programs运行Java程序
   - 直接使用java可执行文件测试

6. **性能基准测试**
   - 内存分配基准测试
   - 哈希表性能测试

7. **构建产物上传**
   - 保存编译产物7天，便于调试

### 快速测试工作流 (`.github/workflows/quick-test.yml`)

**触发条件：**
- 修改源码文件时(`src/**`, `tests/**`, `include/**`, `CMakeLists.txt`)

**特点：**
- 只运行核心组件测试
- 不需要rt.jar
- 快速反馈代码改动

## 本地测试

### 使用测试脚本

```bash
# 运行所有测试
./run-tests.sh

# 只运行组件测试
./run-tests.sh component

# 只运行Java程序测试  
./run-tests.sh java

# 只运行性能基准测试
./run-tests.sh bench

# 查看帮助
./run-tests.sh help
```

### 手动测试流程

1. **编译项目**
   ```bash
   cmake . && make
   ```

2. **运行组件测试**
   ```bash
   cd build
   export LD_LIBRARY_PATH="./lib:$LD_LIBRARY_PATH"
   
   ./bin/test-string
   ./bin/test-memory
   ./bin/test-classloader
   ./bin/test-oop
   ```

3. **运行Java程序测试**
   ```bash
   # 编译Java测试程序
   mkdir -p java-out
   javac -d java-out java-src/*.java
   
   # 运行测试
   ./bin/test-java-programs hello
   ./bin/java -cp "../rt.jar:../java-out" HelloKiVM
   ```

## 项目结构

### 编译产物组织

```
kivm/
├── build/
│   ├── bin/           # 所有可执行文件
│   │   ├── java       # 主要的JVM可执行文件
│   │   ├── test-*     # 组件测试可执行文件
│   │   └── bench-*    # 性能基准测试
│   └── lib/           # 动态库
│       └── libkivm.so
├── java-out/          # 编译的Java类文件
├── rt.jar             # Java运行时库
└── run-tests.sh       # 本地测试脚本
```

### 测试分类

1. **组件测试** (`test-*`)
   - 直接测试C++组件
   - 不需要完整JVM环境
   - 快速启动，适合开发调试

2. **Java程序测试** (`test-java-programs`)
   - 测试完整的Java程序执行
   - 需要rt.jar和Java类文件
   - 验证端到端功能

3. **性能测试** (`bench-*`)
   - 内存分配性能
   - 数据结构性能
   - 用于性能回归检测

## CI状态徽章

将以下徽章添加到README.md中：

```markdown
[![CI](https://github.com/YOUR_USERNAME/KiVM/workflows/KiVM%20CI/badge.svg)](https://github.com/YOUR_USERNAME/KiVM/actions)
[![Quick Test](https://github.com/YOUR_USERNAME/KiVM/workflows/Quick%20Test/badge.svg)](https://github.com/YOUR_USERNAME/KiVM/actions)
```

## 依赖管理

### 系统依赖
- **构建工具**: cmake (>= 3.2), make, gcc/clang
- **C++库**: libffi-dev, libzip-dev, zlib1g-dev
- **Java**: JDK 8+ (用于编译测试程序)

### 运行时依赖
- **rt.jar**: Java 8 运行时库 (已包含在仓库中)
- **libkivm.so**: KiVM核心库 (构建时生成)

## 故障排除

### 常见CI失败原因

1. **缺少依赖**
   ```
   解决方案：确保所有系统依赖都在ci.yml中安装
   ```

2. **rt.jar缺失**
   ```
   解决方案：确保rt.jar已提交到仓库且大小正确
   ```

3. **段错误**
   ```
   解决方案：在CI中单独运行每个测试，避免连续JVM创建
   ```

4. **库路径问题**
   ```
   解决方案：设置LD_LIBRARY_PATH环境变量
   ```

### 本地调试

1. **查看详细输出**
   ```bash
   ./run-tests.sh component 2>&1 | tee test-output.log
   ```

2. **单独运行失败的测试**
   ```bash
   cd build
   export LD_LIBRARY_PATH="./lib:$LD_LIBRARY_PATH"
   ./bin/test-specific-component
   ```

3. **使用调试构建**
   ```bash
   cmake -DCMAKE_BUILD_TYPE=Debug . && make
   gdb ./build/bin/test-component
   ```

## 最佳实践

1. **提交前本地测试**
   ```bash
   ./run-tests.sh all
   ```

2. **小步提交**
   - 每次修改运行快速测试
   - 确保CI通过再合并

3. **监控CI状态**
   - 关注GitHub Actions通知
   - 及时修复失败的构建

4. **性能回归检测**
   - 定期检查基准测试结果
   - 对比历史性能数据

现在你拥有了完整的自动化测试体系，可以确保KiVM的代码质量和稳定性！
