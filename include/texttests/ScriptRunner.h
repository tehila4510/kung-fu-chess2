#ifndef SCRIPT_RUNNER_H
#define SCRIPT_RUNNER_H

#include <iosfwd>

class ScriptRunner {
public:
    bool run(std::istream& input, std::ostream& output) const;
};

#endif
