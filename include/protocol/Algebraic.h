#ifndef PROTOCOL_ALGEBRAIC_H
#define PROTOCOL_ALGEBRAIC_H

#include "model/Position.h"

#include <optional>
#include <string>

namespace protocol {

// Algebraic square (e.g. "e2") <-> engine Position. Protocol layer only.
std::optional<Position> squareToPosition(const std::string& square, int rowCount,
                                         int columnCount);
std::string positionToSquare(const Position& pos, int rowCount);

}  // namespace protocol

#endif
