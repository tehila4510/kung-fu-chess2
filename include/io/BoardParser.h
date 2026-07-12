#ifndef BOARD_PARSER_H
#define BOARD_PARSER_H

#include "model/Board.h"

#include <iosfwd>
#include <optional>
#include <string>
#include <vector>

enum class BoardParseStatus {
    Ok,
    EmptyBoard,
    UnknownToken,
    RowWidthMismatch
};

struct BoardParseResult {
    std::optional<Board> board;
    BoardParseStatus status = BoardParseStatus::Ok;
};

const char* toReasonCode(BoardParseStatus status);

class BoardParser {
public:
    BoardParseResult parse(std::istream& input) const;

    BoardParseResult parseText(const std::string& text) const;
    
    BoardParseResult parseLines(const std::vector<std::string>& lines, size_t& index) const;
};

#endif
