#include "Game.h"
#include <iostream>

bool Game::setup(const std::vector<std::string>& lines, size_t& index) {
    return board.loadFromLines(lines, index);
}

void Game::handleClick(int x, int y) {
    Position pos = board.pixelToCell(x, y);

    if (!board.isWithinBounds(pos)) {
        return;
    }

    if (!hasSelection) {
        if (board.isEmpty(pos)) {
            return;
        }
        handleSelectNew(pos);
    } else {
        char selectedColor = board.getCell(selectedPos)[0];

        if (board.isFriendly(pos, selectedColor)) {
            handleSelectNew(pos);
        } else {
            handleMoveRequest(selectedPos, pos);
        }
    }
}

void Game::handleSelectNew(const Position& pos) {
    hasSelection = true;
    selectedPos = pos;
}

void Game::handleMoveRequest(const Position& from, const Position& to) {
    std::string piece = board.getCell(from);
    board.setCell(to, piece);
    board.setCell(from, ".");

    hasSelection = false;
    selectedPos = {-1, -1};
}

void Game::handleWait(int ms) {
    gameClockMs += ms;
}

void Game::handlePrintBoard() const {
    board.print();
}