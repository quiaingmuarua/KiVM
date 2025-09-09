#include <kivm/classpath/classLoader.h>
#include <kivm/oop/instanceKlass.h>
#include <kivm/runtime/javaThread.h>
#include <kivm/classpath/classPathManager.h>
#include <kivm/runtime/runtimeConfig.h>
#include <kivm/test/testFramework.h>
#include <bin/clipp.h>
#include <iostream>

int main(int argc, char **argv) {
    using namespace kivm;
    using namespace clipp;

    std::string optClassPath;
    std::string optClassName;
    std::vector<std::string> optArgs;
    bool optShowHelp = false;
    bool optTestMode = false;
    std::string optTestName;

    auto cli = (
            option("-h", "-help").call([&]() { optShowHelp = true; }) % "show help",
            option("-v", "-version").call([argv]() {
                fprintf(stderr, "%s: %s\n", argv[0], KIVM_VERSION_STRING);
            }) % "show version",
            (option("-cp") & value("path").set(optClassPath)) % "class search path",
            (option("--test") & value("test-name").set(optTestName).call([&]() { optTestMode = true; })) % "run C++ test mode",
            opt_value("class-name", optClassName),
            opt_values("args", optArgs)
    );

    if (!parse(argc, argv, cli) || optShowHelp) {
        auto fmt = doc_formatting{}.doc_column(28);
        auto filter = param_filter{}.prefix("-");
        std::cerr << "Usage:\n" << usage_lines(cli, argv[0]) << "\n\n"
                  << "Options:\n" << documentation(cli, fmt, filter) << "\n\n"
                  << "Available tests:\n"
                  << "  classloader    - Test class loading functionality\n"
                  << "  memory         - Test memory management\n"
                  << "  oop            - Test object-oriented programming structures\n"
                  << "  bytecode       - Test bytecode execution\n"
                  << "  string         - Test string operations\n"
                  << "  all            - Run all tests\n\n";
        return optShowHelp ? 0 : 1;
    }

    // Handle test mode
    if (optTestMode) {
        std::cout << "=== KiVM C++ Test Mode ===" << std::endl;
        std::cout << "Running test: " << optTestName << std::endl << std::endl;

        // Initialize test framework
        if (!test::TestFramework::initialize(optClassPath)) {
            std::cerr << "Failed to initialize test framework" << std::endl;
            return 1;
        }

        // Run the specified test
        bool success = test::TestFramework::runTest(optTestName);
        
        // Cleanup test framework
        test::TestFramework::cleanup();
        
        std::cout << std::endl << "=== Test completed ===" << std::endl;
        return success ? 0 : 1;
    }

    // Original Java execution logic
    if (optClassName.empty()) {
        std::cerr << "Error: class-name is required when not in test mode" << std::endl;
        return 1;
    }

    String mainClassName = strings::replaceAll(strings::fromStdString(optClassName),
        Global::DOT, Global::SLASH);
    D("java: main class name: %S\n", (mainClassName).c_str());

    JavaVM *javaVM = nullptr;
    JNIEnv *env = nullptr;
    if (JNI_CreateJavaVM(&javaVM, (void **) &env, nullptr) != JNI_OK) {
        PANIC("JNI_CreateJavaVM() failed");
    }

    std::vector<String> arguments;
    arguments.reserve(optArgs.size());
    for (const auto &a : optArgs) {
        arguments.push_back(strings::fromStdString(a));
    }

    if (!optClassPath.empty()) {
        ClassPathManager::get()->addClassPaths(strings::fromStdString(optClassPath));
    }

    JavaMainThread javaMainThread(mainClassName, arguments);
    javaMainThread.start();

    javaVM->DestroyJavaVM();
    return 0;
}
