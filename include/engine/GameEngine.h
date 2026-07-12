#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H
#include "model/Board.h"
#include "rules/RuleEngine.h"
#include "realtime/RealTimeArbiter.h"

struct MoveResult { bool is_accepted; std::string reason; };

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
    void setup(Board board);
    MoveResult requestMove(const Position& from, const Position& to);
    MoveResult requestJump(const Position& at);
    void wait(int ms);
    GameSnapshot snapshot() const;
    bool isGameOver() const;
    int rowCount() const;
    int columnCount() const;
};
#endif
