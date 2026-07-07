#include "TestFramework.h"

void registerBoardTests();
void registerMoveRulesTests();
void registerGameTests();

int main() {
    registerBoardTests();
    registerMoveRulesTests();
    registerGameTests();

    if (testFailureCount() == 0) {
        std::cout << "All tests passed.\n";
        return 0;
    }

    std::cerr << testFailureCount() << " test(s) failed.\n";
    return 1;
}
