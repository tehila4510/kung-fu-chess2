#include "model/Position.h"

#include <algorithm>
#include <cstdlib>

int Position::rowDistanceTo(const Position& o) const {
    return std::abs(row - o.row);
}

int Position::colDistanceTo(const Position& o) const {
    return std::abs(col - o.col);
}

int Position::chebyshevDistanceTo(const Position& o) const {
    return std::max(rowDistanceTo(o), colDistanceTo(o));
}

int Position::manhattanDistanceTo(const Position& o) const {
    return rowDistanceTo(o) + colDistanceTo(o);
}

std::string Position::toString() const {
    return "(" + std::to_string(row) + ", " + std::to_string(col) + ")";
}
