#ifndef SCRIPT_RUNNER_H
#define SCRIPT_RUNNER_H

#include <iosfwd>

// Drives a fresh GameEngine + Controller from a parsed script: dispatches
// click/jump/wait commands in order and writes each "print board" result to
// the output stream. This is the layer that lets the app run the full
// Board:/Commands: text protocol from stdin instead of a hardcoded demo.
class ScriptRunner {
public:
    // Reads a script from `input` and writes its output to `output`.
    // Returns false (after writing a single ERROR line) if the board section
    // failed to parse; returns true otherwise.
    bool run(std::istream& input, std::ostream& output) const;
};

#endif
