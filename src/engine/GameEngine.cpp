#include "engine/GameEngine.h"

// A captured king ends the game. Piece tokens are "<color><kind>", e.g. "wK".
static bool isKing(const std::string& piece) {
    return piece.size() == 2 && piece[1] == 'K';
}

bool GameEngine::setup(const std::vector<std::string>& lines, size_t& index) {
    return board.loadFromLines(lines, index);
}

MoveResult GameEngine::requestMove(const Position& from, const Position& to) {
    if (gameOver) {
        return { false, "game_over" };
    }

    const std::string mover = board.getCell(from);
    // A player is disabled only while their own piece is in flight; the
    // opponent may move at the same time.
    if (!mover.empty() && arbiter.hasActiveMotion(mover[0])) {
        return { false, "move_in_flight" };
    }

    const MoveValidation validation = ruleEngine.validateMove(board, from, to);
    if (!validation.is_valid) {
        return { false, validation.reason };
    }

    arbiter.startMotion(board.getCell(from), from, to);
    return { true, "ok" };
}

void GameEngine::wait(int ms) {
    const std::vector<ArrivalEvent> arrivals = arbiter.advanceTime(ms, board);
    for (const ArrivalEvent& arrival : arrivals) {
        if (isKing(arrival.capturedPiece)) {
            gameOver = true;
        }
    }
}

bool GameEngine::isGameOver() const {
    return gameOver;
}

GameSnapshot GameEngine::snapshot() const {
    GameSnapshot snap;
    snap.gameOver = gameOver;

    const int rows = board.getRowCount();
    const int cols = board.getColCount();
    snap.cells.reserve(static_cast<size_t>(rows));
    for (int r = 0; r < rows; ++r) {
        std::vector<std::string> row;
        row.reserve(static_cast<size_t>(cols));
        for (int c = 0; c < cols; ++c) {
            row.push_back(board.getCell(Position{ r, c }));
        }
        snap.cells.push_back(std::move(row));
    }
    return snap;
}
