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

// Maps a piece color to a motion slot: white -> 0, black -> 1.
int colorIndex(char color) {
    return color == 'w' ? 0 : 1;
}

} // namespace

bool RealTimeArbiter::hasActiveMotion() const {
    return active[0].has_value() || active[1].has_value();
}

bool RealTimeArbiter::hasActiveMotion(char color) const {
    return active[colorIndex(color)].has_value();
}

void RealTimeArbiter::startMotion(const std::string& piece, const Position& from, const Position& to) {
    // Travel time scales with the king-step distance: one second per cell.
    const long long durationMs = static_cast<long long>(from.chebyshevDistanceTo(to)) * 1000;
    active[colorIndex(piece[0])] = Motion{ from, to, piece, clockMs + durationMs };
}

std::vector<ArrivalEvent> RealTimeArbiter::advanceTime(int ms, Board& board) {
    clockMs += ms;

    std::vector<ArrivalEvent> arrivals;
    const int lastRow = board.getRowCount() - 1;

    for (auto& slot : active) {
        if (!slot || clockMs < slot->arrivalTime) {
            continue;
        }

        const Motion motion = *slot;
        slot.reset();

        const std::string capturedPiece = board.getCell(motion.to);
        const std::string pieceToPlace = promotePawnIfNeeded(motion.piece, motion.to, lastRow);

        board.setCell(motion.to, pieceToPlace);
        board.setCell(motion.from, ".");

        arrivals.push_back(ArrivalEvent{ motion.to, capturedPiece });
    }

    return arrivals;
}
