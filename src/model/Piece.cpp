#include "model/Piece.h"

Piece::Piece(char color, char kind, Position position)
    : kind(kind), color(color), position(position) {}

Piece Piece::fromToken(const std::string& token, Position at) {
    if (token.size() == 2) {
        return Piece(token[0], token[1], at);
    }
    return Piece();
}

std::string Piece::toToken() const {
    if (isEmpty()) {
        return std::string(1, NoKind);
    }
    return std::string{ color, kind };
}

bool Piece::isEmpty() const { return color == NoColor || kind == NoKind; }
bool Piece::isWhite() const { return color == 'w'; }
bool Piece::isBlack() const { return color == 'b'; }

bool Piece::sameColorAs(const Piece& other) const {
    return !isEmpty() && !other.isEmpty() && color == other.color;
}

bool Piece::isEnemyOf(const Piece& other) const {
    return !isEmpty() && !other.isEmpty() && color != other.color;
}

char Piece::opponentColor() const {
    if (isEmpty()) return NoColor;
    return isWhite() ? 'b' : 'w';
}

bool Piece::operator==(const Piece& o) const {
    return kind == o.kind && color == o.color
        && position.row == o.position.row
        && position.col == o.position.col;
}

bool Piece::operator!=(const Piece& o) const { return !(*this == o); }
