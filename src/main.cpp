#include "texttests/ScriptRunner.h"

#include <iostream>

int main() {
    ScriptRunner runner;
    runner.run(std::cin, std::cout);
    return 0;
}
