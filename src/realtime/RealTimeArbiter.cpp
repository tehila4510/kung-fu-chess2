#include "realtime/RealTimeArbiter.h"

namespace {

// A pawn reaching the far rank is promoted to a queen of the same color.
std::string promotePawnIfNeeded(const std::string& piece, const Position& to, int lastRow) {
    if (piece.size() == 2 && piece[1] == 'P') {
        const char color = piece[0];
        if ((color == 'w' && to.row == 0) || (color == 'b' && to.row == lastRow)) {
            return std::string(1, color) + 'Q';
        }
    }
    return piece;
}

} // namespace

bool RealTimeArbiter::hasActiveMotion() const {
    return active.has_value();
}

void RealTimeArbiter::startMotion(const std::string& piece, const Position& from, const Position& to) {
    // Travel time scales with the king-step distance: one second per cell.
    const long long durationMs = static_cast<long long>(from.chebyshevDistanceTo(to)) * 1000;
    active = Motion{ from, to, piece, clockMs + durationMs };
}

std::optional<ArrivalEvent> RealTimeArbiter::advanceTime(int ms, Board& board) {
    clockMs += ms;

    if (!active || clockMs < active->arrivalTime) {
        return std::nullopt;
    }

    const Motion motion = *active;
    active.reset();

    const std::string capturedPiece = board.getCell(motion.to);
    const int lastRow = board.getRowCount() - 1;
    const std::string pieceToPlace = promotePawnIfNeeded(motion.piece, motion.to, lastRow);

    board.setCell(motion.to, pieceToPlace);
    board.setCell(motion.from, ".");

    return ArrivalEvent{ motion.to, capturedPiece };
}
