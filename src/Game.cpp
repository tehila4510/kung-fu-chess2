#include "Game.h"
#include "MoveRules.h"
#include <iostream>

bool Game::isValidPlayerColor(char playerColor) {
    return playerColor == 'w' || playerColor == 'b';
}

int Game::colorToIndex(char playerColor) {
    return playerColor == 'w' ? 0 : 1;
}

Game::PlayerSelection& Game::selectionFor(char playerColor) {
    return selections[colorToIndex(playerColor)];
}

const Game::PlayerSelection& Game::selectionFor(char playerColor) const {
    return selections[colorToIndex(playerColor)];
}

bool Game::setup(const std::vector<std::string>& lines, size_t& index) {
    return board.loadFromLines(lines, index);
}

char Game::resolveClickColor(const Position& pos) const {
    if (isPieceSelected('w')) {
        return 'w';
    }
    if (isPieceSelected('b')) {
        return 'b';
    }
    if (!board.isEmpty(pos)) {
        return board.getCell(pos)[0];
    }
    return '\0';
}

void Game::handleClick(int x, int y) {
    Position pos = board.pixelToCell(x, y);
    const char playerColor = resolveClickColor(pos);
    if (playerColor == '\0') {
        return;
    }
    handlePlayerClick(pos, playerColor);
}

void Game::handleClick(int x, int y, char playerColor) {
    if (!isValidPlayerColor(playerColor)) {
        return;
    }
    handlePlayerClick(board.pixelToCell(x, y), playerColor);
}

void Game::handlePlayerClick(const Position& pos, char playerColor) {
    if (!isValidPlayerColor(playerColor)) {
        return;
    }

    if (!board.isWithinBounds(pos)) {
        return;
    }

    PlayerSelection& sel = selectionFor(playerColor);

    if (!sel.active) {
        if (board.isEmpty(pos) || !board.isFriendly(pos, playerColor)) {
            return;
        }
        handleSelectNew(pos, playerColor);
    } else {
        if (board.isFriendly(pos, playerColor)) {
            handleSelectNew(pos, playerColor);
        } else {
            handleMoveRequest(sel.pos, pos, playerColor);
        }
    }
}

void Game::handleSelectNew(const Position& pos, char playerColor) {
    PlayerSelection& sel = selectionFor(playerColor);
    sel.active = true;
    sel.pos = pos;
}

void Game::handleMoveRequest(const Position& from, const Position& to, char playerColor) {
    if (!isValidMove(board, from, to)) {
        return;
    }

    std::string piece = board.getCell(from);
    board.setCell(to, piece);
    board.setCell(from, ".");

    PlayerSelection& sel = selectionFor(playerColor);
    sel.active = false;
    sel.pos = {-1, -1};
}

void Game::handleWait(int ms) {
    gameClockMs += ms;
}

void Game::handlePrintBoard() const {
    board.print();
}

bool Game::isPieceSelected(char playerColor) const {
    if (!isValidPlayerColor(playerColor)) {
        return false;
    }
    return selectionFor(playerColor).active;
}

Position Game::getSelectedPosition(char playerColor) const {
    if (!isValidPlayerColor(playerColor)) {
        return {-1, -1};
    }
    return selectionFor(playerColor).pos;
}
