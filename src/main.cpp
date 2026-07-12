#include "texttests/ScriptRunner.h"

#include <iostream>

// Composition root: reads a Board:/Commands: script from stdin and runs it
// through the engine + input + I/O layers via ScriptRunner, writing each
// "print board" result to stdout.
int main() {
    ScriptRunner runner;
    runner.run(std::cin, std::cout);
    return 0;
}
