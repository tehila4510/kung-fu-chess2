#include "doctest.h"
#include "MoveRules.h"
#include "TestHelpers.h"

TEST_CASE("MoveRules.knight") {
    SUBCASE("legal moves") {
        Board board = loadBoardFromRows({". wN . .", ". . . .", ". . . ."});
        CHECK(isValidMove(board, {0, 1}, {2, 0}));
        CHECK(isValidMove(board, {0, 1}, {1, 3}));
    }

    SUBCASE("illegal moves") {
        Board board = loadBoardFromRows({". wN . .", ". . . ."});
        CHECK_FALSE(isValidMove(board, {0, 1}, {0, 3}));
        CHECK_FALSE(isValidMove(board, {0, 1}, {0, 2}));
    }

    SUBCASE("jumps over blockers") {
        Board board = loadBoardFromRows({"wN wP .", "wP . .", ". . ."});
        CHECK(isValidMove(board, {0, 0}, {2, 1}));
    }
}
