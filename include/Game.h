#ifndef GAME_H
#define GAME_H

#include "Board.h"

class Game {
private:
    Board board;
    long long gameClockMs = 0;

    bool hasSelection = false;
    Position selectedPos = {-1, -1};

    void handleSelectNew(const Position& pos);
    void handleMoveRequest(const Position& from, const Position& to);

public:
    bool setup(const std::vector<std::string>& lines, size_t& index);
    void handleClick(int x, int y);
    void handleWait(int ms);
    void handlePrintBoard() const;
};

#endif