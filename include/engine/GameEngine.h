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
    bool setup(const std::vector<std::string>& lines, size_t& index);
    MoveResult requestMove(const Position& from, const Position& to);
    void wait(int ms);
    GameSnapshot snapshot() const;
    bool isGameOver() const;
};
#endif