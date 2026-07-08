#include "doctest.h"
#include "Game.h"

#include <string>
#include <vector>

static Game makeGame(const std::vector<std::string>& rows) {
    std::vector<std::string> lines = {"Board:"};
    lines.insert(lines.end(), rows.begin(), rows.end());
    lines.push_back("Commands:");

    Game game;
    size_t index = 0;
    game.setup(lines, index);
    return game;
}

static int cellX(int col) { return col * 100 + 50; }
static int cellY(int row) { return row * 100 + 50; }

TEST_CASE("Game.setupSuccess") {
    Game game = makeGame({". wK . .", ". . . ."});
    CHECK_EQ("wK", game.getBoard().getCell({0, 1}));
}

TEST_CASE("Game.setupFailure") {
    std::vector<std::string> lines = {
        "Board:",
        ". bad . .",
        "Commands:"
    };
    Game game;
    size_t index = 0;
    CHECK_FALSE(game.setup(lines, index));
}

TEST_CASE("Game.selectPiece") {
    Game game = makeGame({". wK . .", ". . . ."});
    game.handleClick(cellX(1), cellY(0));
    CHECK(game.isPieceSelected());
    CHECK_EQ(0, game.getSelectedPosition().row);
    CHECK_EQ(1, game.getSelectedPosition().col);
}

TEST_CASE("Game.clickEmptyWithoutSelection") {
    Game game = makeGame({". wK . .", ". . . ."});
    game.handleClick(cellX(0), cellY(0));
    CHECK_FALSE(game.isPieceSelected());
}

TEST_CASE("Game.clickOutOfBounds") {
    Game game = makeGame({". wK . ."});
    game.handleClick(cellX(1), cellY(0));
    CHECK(game.isPieceSelected());
    game.handleClick(9999, 9999);
    CHECK_EQ("wK", game.getBoard().getCell({0, 1}));
}

TEST_CASE("Game.legalKingMove") {
    Game game = makeGame({". wK . .", ". . . ."});
    game.handleClick(cellX(1), cellY(0));
    game.handleClick(cellX(2), cellY(0));
    CHECK_EQ(".", game.getBoard().getCell({0, 1}));
    CHECK_EQ("wK", game.getBoard().getCell({0, 2}));
    CHECK_FALSE(game.isPieceSelected());
}

TEST_CASE("Game.illegalKingMoveIgnored") {
    Game game = makeGame({". wK . .", ". . . ."});
    game.handleClick(cellX(1), cellY(0));
    game.handleClick(cellX(3), cellY(0));
    CHECK_EQ("wK", game.getBoard().getCell({0, 1}));
    CHECK_EQ(".", game.getBoard().getCell({0, 3}));
    CHECK(game.isPieceSelected());
}

TEST_CASE("Game.rookDiagonalIgnored") {
    Game game = makeGame({"wR . . .", ". . . ."});
    game.handleClick(cellX(0), cellY(0));
    game.handleClick(cellX(1), cellY(1));
    CHECK_EQ("wR", game.getBoard().getCell({0, 0}));
    CHECK_EQ(".", game.getBoard().getCell({1, 1}));
}

TEST_CASE("Game.reselectFriendlyPiece") {
    Game game = makeGame({"wR wN . .", ". . . ."});
    game.handleClick(cellX(0), cellY(0));
    game.handleClick(cellX(1), cellY(0));
    CHECK(game.isPieceSelected());
    CHECK_EQ(0, game.getSelectedPosition().row);
    CHECK_EQ(1, game.getSelectedPosition().col);
}

TEST_CASE("Game.captureEnemyPiece") {
    Game game = makeGame({"wR . bQ .", ". . . ."});
    game.handleClick(cellX(0), cellY(0));
    game.handleClick(cellX(2), cellY(0));
    CHECK_EQ(".", game.getBoard().getCell({0, 0}));
    CHECK_EQ("wR", game.getBoard().getCell({0, 2}));
}

TEST_CASE("Game.wait") {
    Game game = makeGame({". wK . ."});
    game.handleWait(500);
    game.handleWait(250);
    CHECK_EQ(750, game.getGameClockMs());
}
