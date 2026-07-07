#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

inline int& testFailureCount() {
    static int count = 0;
    return count;
}

inline void reportFailure(const char* file, int line, const std::string& message) {
    ++testFailureCount();
    std::cerr << "FAIL " << file << ":" << line << " " << message << '\n';
}

#define ASSERT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            reportFailure(__FILE__, __LINE__, "expected true: " #condition); \
            return; \
        } \
    } while (0)

#define ASSERT_FALSE(condition) \
    do { \
        if (condition) { \
            reportFailure(__FILE__, __LINE__, "expected false: " #condition); \
            return; \
        } \
    } while (0)

#define ASSERT_EQ(expected, actual) \
    do { \
        const auto& expectedValue = (expected); \
        const auto& actualValue = (actual); \
        if (expectedValue != actualValue) { \
            std::ostringstream message; \
            message << "expected '" << expectedValue << "' got '" << actualValue << "'"; \
            reportFailure(__FILE__, __LINE__, message.str()); \
            return; \
        } \
    } while (0)

using TestFunction = void (*)();

inline void runTest(const char* name, TestFunction test) {
    const int before = testFailureCount();
    test();
    if (testFailureCount() == before) {
        std::cout << "PASS " << name << '\n';
    }
}

#endif
