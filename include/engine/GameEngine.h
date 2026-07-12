#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H
#include "model/Board.h"
#include "rules/RuleEngine.h"
#include "realtime/RealTimeArbiter.h"

struct MoveResult { bool is_accepted; std::string reason; };

// Immutable, self-contained copy of board state for the view layer.
struct GameSnapshot {
    std::vector<std::vector<std::string>> cells;
    bool gameOver = false;
};

class GameEngine {
    Board board;
    RuleEngine ruleEngine;
    RealTimeArbiter arbiter;
    bool gameOver = false;
public:
    // Initialize the engine with an already-parsed board (see io/BoardParser).
    void setup(Board board);
    MoveResult requestMove(const Position& from, const Position& to);
    // Lifts the piece at `at` airborne in place; it lands back on the same
    // square after a fixed duration, bypassing normal rule/travel-time checks
    // since it never changes squares. Rejected if empty or already in flight.
    MoveResult requestJump(const Position& at);
    void wait(int ms);
    GameSnapshot snapshot() const;
    bool isGameOver() const;
    int rowCount() const;
    int columnCount() const;
};
#endif