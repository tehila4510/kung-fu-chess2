#ifndef PIECE_RULES_H
#define PIECE_RULES_H
#include "model/Board.h"
#include "model/Piece.h"
#include <set>

class IPieceRule {
public:
    virtual ~IPieceRule() = default;
    virtual std::set<Position> legalDestinations(const Board& board,
        const Piece& piece) const = 0;
};

class RookRule   : public IPieceRule { public: std::set<Position> legalDestinations(const Board&, const Piece&) const override; };
class BishopRule : public IPieceRule { public: std::set<Position> legalDestinations(const Board&, const Piece&) const override; };
class QueenRule  : public IPieceRule { public: std::set<Position> legalDestinations(const Board&, const Piece&) const override; };
class KnightRule : public IPieceRule { public: std::set<Position> legalDestinations(const Board&, const Piece&) const override; };
class KingRule   : public IPieceRule { public: std::set<Position> legalDestinations(const Board&, const Piece&) const override; };
class PawnRule   : public IPieceRule { public: std::set<Position> legalDestinations(const Board&, const Piece&) const override; };

const IPieceRule& ruleFor(char kind);
#endif