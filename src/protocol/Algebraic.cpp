#include "protocol/Algebraic.h"

#include <cctype>

namespace protocol {

std::optional<Position> squareToPosition(const std::string& square, int rowCount,
                                         int columnCount) {
    if (square.size() != 2 || rowCount <= 0 || columnCount <= 0) {
        return std::nullopt;
    }

    const char file = static_cast<char>(std::tolower(static_cast<unsigned char>(square[0])));
    const char rankChar = square[1];
    if (file < 'a' || file >= static_cast<char>('a' + columnCount)) {
        return std::nullopt;
    }
    if (rankChar < '1' || rankChar > '9') {
        return std::nullopt;
    }

    const int col = file - 'a';
    const int rank = rankChar - '0';
    if (rank < 1 || rank > rowCount) {
        return std::nullopt;
    }

    // Rank 1 is the bottom row (highest index); rank N is row 0.
    const int row = rowCount - rank;
    return Position{ row, col };
}

std::string positionToSquare(const Position& pos, int rowCount) {
    if (!pos.isValid() || rowCount <= 0 || pos.row >= rowCount) {
        return "";
    }
    const int rank = rowCount - pos.row;
    const char file = static_cast<char>('a' + pos.col);
    return std::string(1, file) + std::to_string(rank);
}

}  // namespace protocol
