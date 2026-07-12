#include "engine/GameEngine.h"

// A captured king ends the game. Piece tokens are "<color><kind>", e.g. "wK".
static bool isKing(const std::string& piece) {
    return piece.size() == 2 && piece[1] == 'K';
}

void GameEngine::setup(Board initialBoard) {
    board = std::move(initialBoard);
    gameOver = false;
}

MoveResult GameEngine::requestMove(const Position& from, const Position& to) {
    if (gameOver) {
        return { false, "game_over" };
    }

    const std::string mover = board.getCell(from);
    if (mover.size() == 2) {
        const char color = mover[0];
        // A player is disabled while their own piece is in flight.
        if (arbiter.hasActiveMotion(color)) {
            return { false, "move_in_flight" };
        }
        const char opponent = color == 'w' ? 'b' : 'w';
        if (arbiter.hasActiveTravel(opponent)) {
            return { false, "common_route" };
        }
    }

    const MoveValidation validation = ruleEngine.validateMove(board, from, to);
    if (!validation.is_valid) {
        return { false, validation.reason };
    }

    arbiter.startMotion(board.getCell(from), from, to);
    return { true, "ok" };
}

MoveResult GameEngine::requestJump(const Position& at) {
    if (gameOver) {
        return { false, "game_over" };
    }
    if (!board.isWithinBounds(at)) {
        return { false, "outside_board" };
    }

    const std::string piece = board.getCell(at);
    if (piece == ".") {
        return { false, "empty_source" };
    }
    if (arbiter.hasActiveMotion(piece[0])) {
        return { false, "move_in_flight" };
    }

    arbiter.startJump(piece, at);
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

int GameEngine::rowCount() const {
    return board.getRowCount();
}

int GameEngine::columnCount() const {
    return board.getColCount();
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
