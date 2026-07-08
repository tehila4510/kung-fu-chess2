#include "doctest.h"
#include "MoveRules.h"
#include "TestHelpers.h"

TEST_CASE("MoveRules.king") {
    SUBCASE("legal moves") {
        Board board = loadBoardFromRows({". wK . .", ". . . ."});
        CHECK(isValidMove(board, {0, 1}, {0, 2}));
        CHECK(isValidMove(board, {0, 1}, {1, 2}));
    }

    SUBCASE("illegal moves") {
        Board board = loadBoardFromRows({". wK . ."});
        CHECK_FALSE(isValidMove(board, {0, 1}, {0, 3}));
        CHECK_FALSE(isValidMove(board, {0, 1}, {0, 1}));
    }
}
