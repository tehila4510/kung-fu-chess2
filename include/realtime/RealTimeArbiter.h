#ifndef REAL_TIME_ARBITER_H
#define REAL_TIME_ARBITER_H
#include "model/Board.h"
#include "realtime/Motion.h"
#include <optional>
#include <string>

struct ArrivalEvent {
    Position at;
    std::string capturedPiece;
};

class RealTimeArbiter {
    long long clockMs = 0;
    std::optional<Motion> active;
public:
    bool hasActiveMotion() const;
    void startMotion(const std::string& piece, const Position& from, const Position& to);
    // Advances the clock; on arrival, mutates the board and returns the capture.
    std::optional<ArrivalEvent> advanceTime(int ms, Board& board);
};
#endif