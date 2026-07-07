#include "TestFramework.h"
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

void testGameSetupSuccess() {
    Game game = makeGame({". wK . .", ". . . ."});
    ASSERT_EQ("wK", game.getBoard().getCell({0, 1}));
}

void testGameSetupFailure() {
    std::vector<std::string> lines = {
        "Board:",
        ". bad . .",
        "Commands:"
    };
    Game game;
    size_t index = 0;
    ASSERT_FALSE(game.setup(lines, index));
}

void testGameSelectPiece() {
    Game game = makeGame({". wK . .", ". . . ."});
    game.handleClick(cellX(1), cellY(0));
    ASSERT_TRUE(game.isPieceSelected());
    ASSERT_EQ(0, game.getSelectedPosition().row);
    ASSERT_EQ(1, game.getSelectedPosition().col);
}

void testGameClickEmptyWithoutSelection() {
    Game game = makeGame({". wK . .", ". . . ."});
    game.handleClick(cellX(0), cellY(0));
    ASSERT_FALSE(game.isPieceSelected());
}

void testGameClickOutOfBounds() {
    Game game = makeGame({". wK . ."});
    game.handleClick(cellX(1), cellY(0));
    ASSERT_TRUE(game.isPieceSelected());
    game.handleClick(9999, 9999);
    ASSERT_EQ("wK", game.getBoard().getCell({0, 1}));
}

void testGameLegalKingMove() {
    Game game = makeGame({". wK . .", ". . . ."});
    game.handleClick(cellX(1), cellY(0));
    game.handleClick(cellX(2), cellY(0));
    ASSERT_EQ(".", game.getBoard().getCell({0, 1}));
    ASSERT_EQ("wK", game.getBoard().getCell({0, 2}));
    ASSERT_FALSE(game.isPieceSelected());
}

void testGameIllegalKingMoveIgnored() {
    Game game = makeGame({". wK . .", ". . . ."});
    game.handleClick(cellX(1), cellY(0));
    game.handleClick(cellX(3), cellY(0));
    ASSERT_EQ("wK", game.getBoard().getCell({0, 1}));
    ASSERT_EQ(".", game.getBoard().getCell({0, 3}));
    ASSERT_TRUE(game.isPieceSelected());
}

void testGameRookDiagonalIgnored() {
    Game game = makeGame({"wR . . .", ". . . ."});
    game.handleClick(cellX(0), cellY(0));
    game.handleClick(cellX(1), cellY(1));
    ASSERT_EQ("wR", game.getBoard().getCell({0, 0}));
    ASSERT_EQ(".", game.getBoard().getCell({1, 1}));
}

void testGameReselectFriendlyPiece() {
    Game game = makeGame({"wR wN . .", ". . . ."});
    game.handleClick(cellX(0), cellY(0));
    game.handleClick(cellX(1), cellY(0));
    ASSERT_TRUE(game.isPieceSelected());
    ASSERT_EQ(0, game.getSelectedPosition().row);
    ASSERT_EQ(1, game.getSelectedPosition().col);
}

void testGameCaptureEnemyPiece() {
    Game game = makeGame({"wR . bQ .", ". . . ."});
    game.handleClick(cellX(0), cellY(0));
    game.handleClick(cellX(2), cellY(0));
    ASSERT_EQ(".", game.getBoard().getCell({0, 0}));
    ASSERT_EQ("wR", game.getBoard().getCell({0, 2}));
}

void testGameWait() {
    Game game = makeGame({". wK . ."});
    game.handleWait(500);
    game.handleWait(250);
    ASSERT_EQ(750, game.getGameClockMs());
}

void registerGameTests() {
    runTest("Game.setupSuccess", testGameSetupSuccess);
    runTest("Game.setupFailure", testGameSetupFailure);
    runTest("Game.selectPiece", testGameSelectPiece);
    runTest("Game.clickEmptyWithoutSelection", testGameClickEmptyWithoutSelection);
    runTest("Game.clickOutOfBounds", testGameClickOutOfBounds);
    runTest("Game.legalKingMove", testGameLegalKingMove);
    runTest("Game.illegalKingMoveIgnored", testGameIllegalKingMoveIgnored);
    runTest("Game.rookDiagonalIgnored", testGameRookDiagonalIgnored);
    runTest("Game.reselectFriendlyPiece", testGameReselectFriendlyPiece);
    runTest("Game.captureEnemyPiece", testGameCaptureEnemyPiece);
    runTest("Game.wait", testGameWait);
}
