#ifndef SCRIPT_PARSER_H
#define SCRIPT_PARSER_H

#include "io/BoardParser.h"
#include "model/Board.h"

#include <iosfwd>
#include <optional>
#include <vector>

enum class ScriptCommandKind {
    Click,
    Jump,
    Wait,
    PrintBoard,
    Unknown
};

// A single parsed line from the "Commands:" section.
struct ScriptCommand {
    ScriptCommandKind kind = ScriptCommandKind::Unknown;
    int x = 0;   // click/jump: pixel x
    int y = 0;   // click/jump: pixel y
    int ms = 0;  // wait: duration in milliseconds
};

struct ScriptParseResult {
    std::optional<Board> board;
    BoardParseStatus boardStatus = BoardParseStatus::Ok;
    std::vector<ScriptCommand> commands;
};

// Parses the full stdin protocol: a board section (delegated to BoardParser)
// followed by a "Commands:" section of click/jump/wait/print-board lines.
// I/O-layer component: text in, structured data out; it never touches the
// engine, rules, or global streams itself.
class ScriptParser {
public:
    ScriptParseResult parse(std::istream& input) const;
};

#endif
