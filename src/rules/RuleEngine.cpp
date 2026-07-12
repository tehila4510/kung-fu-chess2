#include "rules/RuleEngine.h"
#include "rules/PieceRules.h"
#include <set>

MoveValidation RuleEngine::validateMove(const Board& board, const Position& from, const Position& to) const {
    if (!board.isWithinBounds(from) || !board.isWithinBounds(to)) {
        return { false, "outside_board" };
    }

    const std::string token = board.getCell(from);
    if (token == ".") {
        return { false, "empty_source" };
    }

    if (board.isFriendly(to, token[0])) {
        return { false, "friendly_destination" };
    }

    const Piece piece = Piece::fromToken(token, from);
    const std::set<Position> legal = ruleFor(piece.kind).legalDestinations(board, piece);
    if (legal.find(to) == legal.end()) {
        return { false, "illegal_piece_move" };
    }

    return { true, "ok" };
}
