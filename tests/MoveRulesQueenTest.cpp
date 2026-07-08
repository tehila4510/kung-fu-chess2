#include "doctest.h"
#include "MoveRules.h"
#include "TestHelpers.h"

TEST_CASE("MoveRules.queen") {
    SUBCASE("legal moves") {
        Board board = loadBoardFromRows({". wQ . .", ". . . .", ". . . ."});
        CHECK(isValidMove(board, {0, 1}, {0, 3}));
        CHECK(isValidMove(board, {0, 1}, {2, 3}));
    }

    SUBCASE("illegal moves") {
        Board board = loadBoardFromRows({". wQ . .", ". . . .", ". . . ."});
        CHECK_FALSE(isValidMove(board, {0, 1}, {2, 2}));
    }
}
