#include "doctest.h"
#include "MoveRules.h"
#include "TestHelpers.h"

TEST_CASE("MoveRules.bishop") {
    SUBCASE("legal moves") {
        Board board = loadBoardFromRows({"wB . . .", ". . . .", ". . . ."});
        CHECK(isValidMove(board, {0, 0}, {2, 2}));
    }

    SUBCASE("illegal moves") {
        Board board = loadBoardFromRows({"wB . . .", ". . . .", ". . . ."});
        CHECK_FALSE(isValidMove(board, {0, 0}, {0, 2}));
        CHECK_FALSE(isValidMove(board, {0, 0}, {2, 0}));
    }

    SUBCASE("blocked path") {
        Board board = loadBoardFromRows({"wB . . .", ". wP . .", ". . . .", ". . . ."});
        CHECK_FALSE(isValidMove(board, {0, 0}, {2, 2}));
    }
}
