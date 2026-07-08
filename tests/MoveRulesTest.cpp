#include "doctest.h"
#include "MoveRules.h"

#include <string>
#include <vector>

static Board loadBoardFromRows(const std::vector<std::string>& rows) {
    std::vector<std::string> lines = {"Board:"};
    lines.insert(lines.end(), rows.begin(), rows.end());
    lines.push_back("Commands:");

    Board board;
    size_t index = 0;
    board.loadFromLines(lines, index);
    return board;
}

TEST_CASE("MoveRules.kingLegal") {
    Board board = loadBoardFromRows({". wK . .", ". . . ."});
    CHECK(isValidMove(board, {0, 1}, {0, 2}));
    CHECK(isValidMove(board, {0, 1}, {1, 2}));
}

TEST_CASE("MoveRules.kingIllegal") {
    Board board = loadBoardFromRows({". wK . ."});
    CHECK_FALSE(isValidMove(board, {0, 1}, {0, 3}));
    CHECK_FALSE(isValidMove(board, {0, 1}, {0, 1}));
}

TEST_CASE("MoveRules.rookLegal") {
    Board board = loadBoardFromRows({"wR . . .", ". . . ."});
    CHECK(isValidMove(board, {0, 0}, {0, 3}));
    CHECK(isValidMove(board, {0, 0}, {1, 0}));
}

TEST_CASE("MoveRules.rookIllegal") {
    Board board = loadBoardFromRows({"wR . . .", ". . . .", ". . . ."});
    CHECK_FALSE(isValidMove(board, {0, 0}, {1, 1}));
    CHECK_FALSE(isValidMove(board, {0, 0}, {2, 2}));
}

TEST_CASE("MoveRules.rookBlockedPath") {
    Board board = loadBoardFromRows({"wR . wP .", ". . . ."});
    CHECK_FALSE(isValidMove(board, {0, 0}, {0, 3}));
}

TEST_CASE("MoveRules.bishopLegal") {
    Board board = loadBoardFromRows({"wB . . .", ". . . .", ". . . ."});
    CHECK(isValidMove(board, {0, 0}, {2, 2}));
}

TEST_CASE("MoveRules.bishopIllegal") {
    Board board = loadBoardFromRows({"wB . . .", ". . . .", ". . . ."});
    CHECK_FALSE(isValidMove(board, {0, 0}, {0, 2}));
    CHECK_FALSE(isValidMove(board, {0, 0}, {2, 0}));
}

TEST_CASE("MoveRules.bishopBlockedPath") {
    Board board = loadBoardFromRows({"wB . . .", ". wP . .", ". . . .", ". . . ."});
    CHECK_FALSE(isValidMove(board, {0, 0}, {2, 2}));
}

TEST_CASE("MoveRules.queenLegal") {
    Board board = loadBoardFromRows({". wQ . .", ". . . .", ". . . ."});
    CHECK(isValidMove(board, {0, 1}, {0, 3}));
    CHECK(isValidMove(board, {0, 1}, {2, 3}));
}

TEST_CASE("MoveRules.queenIllegal") {
    Board board = loadBoardFromRows({". wQ . .", ". . . .", ". . . ."});
    CHECK_FALSE(isValidMove(board, {0, 1}, {2, 2}));
}

TEST_CASE("MoveRules.knightLegal") {
    Board board = loadBoardFromRows({". wN . .", ". . . .", ". . . ."});
    CHECK(isValidMove(board, {0, 1}, {2, 0}));
    CHECK(isValidMove(board, {0, 1}, {1, 3}));
}

TEST_CASE("MoveRules.knightIllegal") {
    Board board = loadBoardFromRows({". wN . .", ". . . ."});
    CHECK_FALSE(isValidMove(board, {0, 1}, {0, 3}));
    CHECK_FALSE(isValidMove(board, {0, 1}, {0, 2}));
}

TEST_CASE("MoveRules.emptySourceInvalid") {
    Board board = loadBoardFromRows({". . . ."});
    CHECK_FALSE(isValidMove(board, {0, 0}, {0, 1}));
}

TEST_CASE("MoveRules.friendlyTargetInvalid") {
    Board board = loadBoardFromRows({"wP wR . .", ". . . ."}); 
    CHECK_FALSE(isValidMove(board, {0, 0}, {0, 1}));
}

TEST_CASE("MoveRules.outOfBoundsInvalid") {
    Board board = loadBoardFromRows({". wK . .", ". . . ."});
    CHECK_FALSE(isValidMove(board, {0, 1}, {5, 5}));
    CHECK_FALSE(isValidMove(board, {5, 5}, {0, 1}));
}

TEST_CASE("MoveRules.knightJumpsOverBlockers") {
    Board board = loadBoardFromRows({"wN wP .", "wP . .", ". . ."});
    CHECK(isValidMove(board, {0, 0}, {2, 1}));
}

TEST_CASE("MoveRules.rookCapturesEnemyAtDestination") {
    Board board = loadBoardFromRows({"wR . bR"});
    CHECK(isValidMove(board, {0, 0}, {0, 2}));
}

TEST_CASE("MoveRules.pawnLegal") {
    Board board = loadBoardFromRows({". . . .", "wP . . .", ". . . ."});
    CHECK(isValidMove(board, {1, 0}, {0, 0}));
}

TEST_CASE("MoveRules.blackPawnLegal") {
    Board board = loadBoardFromRows({"bP . . .", ". . . ."});
    CHECK(isValidMove(board, {0, 0}, {1, 0}));
}

TEST_CASE("MoveRules.pawnIllegal") {
    Board board = loadBoardFromRows({". . . .", ". . . .", "wP . . ."});
    CHECK_FALSE(isValidMove(board, {2, 0}, {0, 0}));
}

TEST_CASE("MoveRules.pawnDiagonalCapture") {
    Board board = loadBoardFromRows({"bN . .", ". wP .", ". . ."});
    CHECK(isValidMove(board, {1, 1}, {0, 0}));
}

TEST_CASE("MoveRules.pawnForwardCaptureInvalid") {
    Board board = loadBoardFromRows({"bN . .", "wP . .", ". . ."});
    CHECK_FALSE(isValidMove(board, {1, 0}, {0, 0}));
}

TEST_CASE("MoveRules.pawnDiagonalWithoutEnemyInvalid") {
    Board board = loadBoardFromRows({". . .", ". wP .", ". . ."});
    CHECK_FALSE(isValidMove(board, {1, 1}, {0, 2}));
}

TEST_CASE("MoveRules.pawnDiagonalWithFriendlyInvalid") {
    Board board = loadBoardFromRows({". wN .", ". wP .", ". . ."});
    CHECK_FALSE(isValidMove(board, {1, 1}, {0, 0}));
}

TEST_CASE("MoveRules.pawnBlockedForwardInvalid") {
    Board board = loadBoardFromRows({"wR . .", "wP . .", ". . ."});
    CHECK_FALSE(isValidMove(board, {1, 0}, {0, 0}));
}

TEST_CASE("MoveRules.pawnInitialRowOneForwardWhite") {
    Board board = loadBoardFromRows({". . . .", "wP . . .", ". . . .", ". . . ."});
    CHECK(isValidMove(board, {1, 0}, {0, 0}));
    CHECK_FALSE(isValidMove(board, {1, 0}, {2, 0}));
}

TEST_CASE("MoveRules.pawnInitialRowOneForwardBlack") {
    Board board = loadBoardFromRows({
        ". . . .", ". . . .", ". . . .", ". . . .",
        ". . . .", ". . . .", "bP . . .", ". . . ."
    });
    CHECK(isValidMove(board, {6, 0}, {7, 0}));
}

TEST_CASE("MoveRules.pawnInitialRowTwoForwardBlackLegal") {
    Board board = loadBoardFromRows({
        ". . . .", ". . . .", ". . . .", ". . . .",
        ". . . .", ". . . .", "bP . . .", ". . . .", ". . . ."
    });
    CHECK(isValidMove(board, {6, 0}, {8, 0}));
}

TEST_CASE("MoveRules.pawnInitialRowTwoForwardBlackBlockedPath") {
    Board board = loadBoardFromRows({
        ". . . .", ". . . .", ". . . .", ". . . .",
        ". . . .", ". . . .", "bP . . .", "wR . . .", ". . . ."
    });
    CHECK_FALSE(isValidMove(board, {6, 0}, {8, 0}));
}

TEST_CASE("MoveRules.pawnInitialRowTwoForwardBlackDestinationBlocked") {
    Board board = loadBoardFromRows({
        ". . . .", ". . . .", ". . . .", ". . . .",
        ". . . .", ". . . .", "bP . . .", ". . . .", "wR . . ."
    });
    CHECK_FALSE(isValidMove(board, {6, 0}, {8, 0}));
}

TEST_CASE("MoveRules.pawnInitialRowTwoForwardWhiteOutOfBounds") {
    Board board = loadBoardFromRows({". . . .", "wP . . .", ". . . ."});
    CHECK_FALSE(isValidMove(board, {1, 0}, {-1, 0}));
}

TEST_CASE("MoveRules.pawnOneForwardNotInitialRowWhite") {
    Board board = loadBoardFromRows({". . . .", ". . . .", "wP . . .", ". . . ."});
    CHECK(isValidMove(board, {2, 0}, {1, 0}));
}

TEST_CASE("MoveRules.pawnOneForwardNotInitialRowBlack") {
    Board board = loadBoardFromRows({". . . .", "bP . . .", ". . . .", ". . . ."});
    CHECK(isValidMove(board, {1, 0}, {2, 0}));
}

TEST_CASE("MoveRules.pawnTwoForwardNotFromInitialRowInvalid") {
    Board board = loadBoardFromRows({". . . .", ". . . .", "wP . . .", ". . . ."});
    CHECK_FALSE(isValidMove(board, {2, 0}, {0, 0}));
}

TEST_CASE("MoveRules.pawnTwoForwardBlackNotFromInitialRowInvalid") {
    Board board = loadBoardFromRows({
        ". . . .", ". . . .", ". . . .", ". . . .",
        ". . . .", "bP . . .", ". . . .", ". . . ."
    });
    CHECK_FALSE(isValidMove(board, {5, 0}, {7, 0}));
}

TEST_CASE("MoveRules.pawnBackwardMoveWhiteInvalid") {
    Board board = loadBoardFromRows({". . . .", "wP . . .", ". . . ."});
    CHECK_FALSE(isValidMove(board, {1, 0}, {2, 0}));
}

TEST_CASE("MoveRules.pawnBackwardMoveBlackInvalid") {
    Board board = loadBoardFromRows({". . . .", "bP . . .", ". . . ."});
    CHECK_FALSE(isValidMove(board, {1, 0}, {0, 0}));
}

TEST_CASE("MoveRules.pawnSidewaysMoveInvalid") {
    Board board = loadBoardFromRows({". . . .", "wP . . .", ". . . ."});
    CHECK_FALSE(isValidMove(board, {1, 0}, {1, 1}));
    CHECK_FALSE(isValidMove(board, {1, 0}, {1, 2}));
}

TEST_CASE("MoveRules.pawnThreeForwardInvalid") {
    Board board = loadBoardFromRows({
        ". . . .", ". . . .", ". . . .", ". . . .",
        ". . . .", ". . . .", "bP . . .", ". . . .",
        ". . . .", ". . . ."
    });
    CHECK_FALSE(isValidMove(board, {6, 0}, {9, 0}));
}

TEST_CASE("MoveRules.pawnBlackDiagonalCapture") {
    Board board = loadBoardFromRows({". . .", ". bP .", "wN . ."});
    CHECK(isValidMove(board, {1, 1}, {2, 0}));

    Board boardRight = loadBoardFromRows({". . .", ". bP .", ". . wN"});
    CHECK(isValidMove(boardRight, {1, 1}, {2, 2}));
}

TEST_CASE("MoveRules.pawnDiagonalWrongDirectionInvalid") {
    Board board = loadBoardFromRows({". . .", "bN . .", ". wP .", ". . ."});
    CHECK_FALSE(isValidMove(board, {2, 1}, {3, 0}));
    CHECK_FALSE(isValidMove(board, {2, 1}, {3, 2}));
}

TEST_CASE("MoveRules.pawnDiagonalCaptureBothDirections") {
    Board board = loadBoardFromRows({"bN . bR", ". wP .", ". . ."});
    CHECK(isValidMove(board, {1, 1}, {0, 0}));
    CHECK(isValidMove(board, {1, 1}, {0, 2}));
}

TEST_CASE("MoveRules.pawnStayInPlaceInvalid") {
    Board board = loadBoardFromRows({". . . .", "wP . . .", ". . . ."});
    CHECK_FALSE(isValidMove(board, {1, 0}, {1, 0}));
}

TEST_CASE("MoveRules.pawnInitialRowCanChooseOneOrTwoBlack") {
    Board board = loadBoardFromRows({
        ". . . .", ". . . .", ". . . .", ". . . .",
        ". . . .", ". . . .", "bP . . .", ". . . .", ". . . ."
    });
    CHECK(isValidMove(board, {6, 0}, {7, 0}));
    CHECK(isValidMove(board, {6, 0}, {8, 0}));
}

TEST_CASE("MoveRules.pawnInitialRowOneForwardWhiteWhenTwoWouldBeOutOfBounds") {
    Board board = loadBoardFromRows({". . . .", "wP . . .", ". . . ."});
    CHECK(isValidMove(board, {1, 0}, {0, 0}));
    CHECK_FALSE(isValidMove(board, {1, 0}, {-1, 0}));
}
