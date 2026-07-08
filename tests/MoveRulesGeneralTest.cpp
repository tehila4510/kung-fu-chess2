#include "doctest.h"
#include "MoveRules.h"
#include "TestHelpers.h"

TEST_CASE("MoveRules.general") {
    SUBCASE("empty source invalid") {
        Board board = loadBoardFromRows({". . . ."});
        CHECK_FALSE(isValidMove(board, {0, 0}, {0, 1}));
    }

    SUBCASE("friendly target invalid") {
        Board board = loadBoardFromRows({"wP wR . .", ". . . ."});
        CHECK_FALSE(isValidMove(board, {0, 0}, {0, 1}));
    }

    SUBCASE("out of bounds invalid") {
        Board board = loadBoardFromRows({". wK . .", ". . . ."});
        CHECK_FALSE(isValidMove(board, {0, 1}, {5, 5}));
        CHECK_FALSE(isValidMove(board, {5, 5}, {0, 1}));
    }
}
