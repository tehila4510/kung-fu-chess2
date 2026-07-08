#include "doctest.h"
#include "MoveRules.h"
#include "TestHelpers.h"

TEST_CASE("MoveRules.rook") {
    SUBCASE("legal moves") {
        Board board = loadBoardFromRows({"wR . . .", ". . . ."});
        CHECK(isValidMove(board, {0, 0}, {0, 3}));
        CHECK(isValidMove(board, {0, 0}, {1, 0}));
    }

    SUBCASE("illegal moves") {
        Board board = loadBoardFromRows({"wR . . .", ". . . .", ". . . ."});
        CHECK_FALSE(isValidMove(board, {0, 0}, {1, 1}));
        CHECK_FALSE(isValidMove(board, {0, 0}, {2, 2}));
    }

    SUBCASE("blocked path") {
        Board board = loadBoardFromRows({"wR . wP .", ". . . ."});
        CHECK_FALSE(isValidMove(board, {0, 0}, {0, 3}));
    }

    SUBCASE("captures") {
        Board board = loadBoardFromRows({"wR . bR"});
        CHECK(isValidMove(board, {0, 0}, {0, 2}));
    }
}
