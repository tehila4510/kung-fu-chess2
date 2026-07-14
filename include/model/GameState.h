#ifndef GAME_STATE_H
#define GAME_STATE_H
#include "model/Board.h"
#include <vector>
#include <string>

struct GameSnapshot {
    std::vector<std::vector<std::string>> cells;
    bool gameOver = false;
};

class GameState {
    Board board;
    bool gameOver = false;
public:
    void initialize(Board initialBoard);
    Board& getBoard();
    const Board& getBoard() const;
    bool isGameOver() const;
    void setGameOver(bool state);
    int rowCount() const;
    int columnCount() const;
    GameSnapshot createSnapshot() const;
};
#endif
