#ifndef REAL_TIME_ARBITER_H
#define REAL_TIME_ARBITER_H
#include "model/Board.h"
#include "realtime/Motion.h"
#include <optional>
#include <string>
#include <vector>

struct ArrivalEvent {
    Position at;
    std::string capturedPiece;
};

// Tracks in-flight motions on a shared clock. Each color may have at most one
// motion in flight at a time: a player is disabled only while their own piece
// travels, so the opponent can move simultaneously.
class RealTimeArbiter {
    static constexpr int kColorCount = 2;
    // A jump always lands back on its own square, so it has no travel
    // distance to derive a duration from; it uses this fixed airborne time.
    static constexpr int kJumpDurationMs = 1000;
    long long clockMs = 0;
    long long nextStartSeq = 0;
    std::optional<Motion> active[kColorCount];
public:
    bool hasActiveMotion() const;
    bool hasActiveMotion(char color) const;
    void startMotion(const std::string& piece, const Position& from, const Position& to);
    // Lifts a piece airborne in place: it leaves the board visually blocking
    // nothing new (the token stays put until landing), and it lands on the
    // same square after a fixed duration, capturing whatever occupies it then.
    void startJump(const std::string& piece, const Position& at);
    // Advances the clock; resolves any motions that have arrived, mutating the
    // board on landing. Returns one event per resolved arrival.
    std::vector<ArrivalEvent> advanceTime(int ms, Board& board);
};
#endif