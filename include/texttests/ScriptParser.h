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

struct ScriptCommand {
    ScriptCommandKind kind = ScriptCommandKind::Unknown;
    int x = 0;
    int y = 0;
    int ms = 0;
};

struct ScriptParseResult {
    std::optional<Board> board;
    BoardParseStatus boardStatus = BoardParseStatus::Ok;
    std::vector<ScriptCommand> commands;
};

class ScriptParser {
public:
    ScriptParseResult parse(std::istream& input) const;
};

#endif
