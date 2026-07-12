#ifndef RULE_ENGINE_H
#define RULE_ENGINE_H
#include "model/Board.h"
#include "model/Piece.h"
#include <string>

struct MoveValidation {
    bool is_valid;
    std::string reason; // "ok" | "outside_board" | "empty_source" | "friendly_destination" | "illegal_piece_move"
};

class RuleEngine {
public:
    MoveValidation validateMove(const Board& board, const Position& from, const Position& to) const;
};
#endif