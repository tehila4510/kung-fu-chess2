#include "texttests/ScriptParser.h"

#include <istream>
#include <sstream>
#include <string>

namespace {

std::string trim(const std::string& s) {
    const size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    const size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

// Parses one non-empty, trimmed command line into a ScriptCommand.
ScriptCommand parseCommandLine(const std::string& line) {
    std::istringstream iss(line);
    std::string verb;
    iss >> verb;

    ScriptCommand cmd;
    if (verb == "click") {
        cmd.kind = ScriptCommandKind::Click;
        iss >> cmd.x >> cmd.y;
    } else if (verb == "jump") {
        cmd.kind = ScriptCommandKind::Jump;
        iss >> cmd.x >> cmd.y;
    } else if (verb == "wait") {
        cmd.kind = ScriptCommandKind::Wait;
        iss >> cmd.ms;
    } else if (verb == "print") {
        std::string arg;
        iss >> arg;
        cmd.kind = (arg == "board") ? ScriptCommandKind::PrintBoard : ScriptCommandKind::Unknown;
    } else {
        cmd.kind = ScriptCommandKind::Unknown;
    }
    return cmd;
}

} // namespace

ScriptParseResult ScriptParser::parse(std::istream& input) const {
    ScriptParseResult result;

    // BoardParser::parse reads rows until EOF or a "Commands:" line, leaving
    // the stream positioned right after that marker for the commands below.
    const BoardParseResult boardResult = BoardParser().parse(input);
    result.board = boardResult.board;
    result.boardStatus = boardResult.status;
    if (!result.board.has_value()) {
        return result;
    }

    std::string line;
    while (std::getline(input, line)) {
        const std::string t = trim(line);
        if (t.empty()) continue;
        result.commands.push_back(parseCommandLine(t));
    }
    return result;
}
