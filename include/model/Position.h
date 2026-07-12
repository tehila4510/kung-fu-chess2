#ifndef POSITION_H
#define POSITION_H

#include <string>

struct Position {
    int row = -1;
    int col = -1;

    Position() = default;
    Position(int row, int col) : row(row), col(col) {}

    bool isValid() const { return row >= 0 && col >= 0; }

    Position offset(int dRow, int dCol) const {
        return Position{ row + dRow, col + dCol };
    }

    int rowDistanceTo(const Position& o) const;
    int colDistanceTo(const Position& o) const;

    // Number of king steps between cells (Chebyshev distance).
    int chebyshevDistanceTo(const Position& o) const;

    // Number of orthogonal steps between cells (Manhattan distance).
    int manhattanDistanceTo(const Position& o) const;

    bool isSameRow(const Position& o) const { return row == o.row; }
    bool isSameCol(const Position& o) const { return col == o.col; }
    bool isSameDiagonal(const Position& o) const {
        return rowDistanceTo(o) == colDistanceTo(o);
    }

    std::string toString() const;

    bool operator==(const Position& o) const { return row == o.row && col == o.col; }
    bool operator!=(const Position& o) const { return !(*this == o); }

    bool operator<(const Position& o) const {
        return row < o.row || (row == o.row && col < o.col);
    }
};

#endif
