#include "realtime/RealTimeArbiter.h"

#include <algorithm>
#include <array>

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
    active[colorIndex(piece[0])] = Motion{ from, to, piece, clockMs + durationMs, nextStartSeq++ };
}

void RealTimeArbiter::startJump(const std::string& piece, const Position& at) {
    // from == to marks this motion as an airborne jump rather than travel.
    active[colorIndex(piece[0])] = Motion{ at, at, piece, clockMs + kJumpDurationMs, nextStartSeq++ };
}

std::vector<ArrivalEvent> RealTimeArbiter::advanceTime(int ms, Board& board) {
    clockMs += ms;

    std::vector<ArrivalEvent> arrivals;
    const int lastRow = board.getRowCount() - 1;

    // A jump lands on the same square it left, so a piece that travels into
    // that square within the same tick must be resolved first; the jumper
    // then "lands" on top of it and captures it, rather than the reverse.
    auto isJumpLandingHere = [&](const Position& p) {
        for (const auto& slot : active) {
            if (slot && slot->from == slot->to && slot->to == p && clockMs >= slot->arrivalTime) {
                return true;
            }
        }
        return false;
    };

    // Pass 1: resolve travelling motions (from != to). When two motions land
    // on the same square in the same tick (a head-on capture), the motion
    // that started *earlier* must be resolved *last* so it overwrites the
    // other -- i.e. the piece that set off first wins the race and survives.
    std::array<int, kColorCount> travelOrder{};
    int travelCount = 0;
    for (int i = 0; i < kColorCount; ++i) {
        const auto& slot = active[i];
        if (slot && slot->from != slot->to && clockMs >= slot->arrivalTime) {
            travelOrder[static_cast<size_t>(travelCount++)] = i;
        }
    }
    std::sort(travelOrder.begin(), travelOrder.begin() + travelCount, [&](int a, int b) {
        return active[a]->startSeq > active[b]->startSeq; // later start resolves first
    });

    for (int i = 0; i < travelCount; ++i) {
        auto& slot = active[static_cast<size_t>(travelOrder[static_cast<size_t>(i)])];
        const Motion motion = *slot;
        slot.reset();

        std::string capturedPiece = board.getCell(motion.to);
        if (isJumpLandingHere(motion.to)) {
            // The defender is airborne, not actually present to be captured;
            // it will reclaim the square in pass 2 below.
            capturedPiece = ".";
        }
        const std::string pieceToPlace = promotePawnIfNeeded(motion.piece, motion.to, lastRow);

        board.setCell(motion.to, pieceToPlace);
        board.setCell(motion.from, ".");

        arrivals.push_back(ArrivalEvent{ motion.to, capturedPiece });
    }

    // Pass 2: resolve jump landings, capturing anyone now on the square.
    for (auto& slot : active) {
        if (!slot || slot->from != slot->to || clockMs < slot->arrivalTime) {
            continue;
        }

        const Motion motion = *slot;
        slot.reset();

        const std::string occupant = board.getCell(motion.to);
        const std::string capturedPiece = (occupant == "." || occupant == motion.piece) ? "." : occupant;

        board.setCell(motion.to, motion.piece);

        arrivals.push_back(ArrivalEvent{ motion.to, capturedPiece });
    }

    return arrivals;
}
