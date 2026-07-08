#include "doctest.h"
#include "MoveRules.h"
#include "TestHelpers.h"

TEST_CASE("MoveRules.pawn") {
    SUBCASE("legal moves") {
        Board singleForwardWhite = loadBoardFromRows({". . . .", "wP . . .", ". . . ."});
        CHECK(isValidMove(singleForwardWhite, {1, 0}, {0, 0}));

        Board singleForwardBlack = loadBoardFromRows({"bP . . .", ". . . ."});
        CHECK(isValidMove(singleForwardBlack, {0, 0}, {1, 0}));

        Board oneForwardNotInitialWhite = loadBoardFromRows({". . . .", ". . . .", "wP . . .", ". . . ."});
        CHECK(isValidMove(oneForwardNotInitialWhite, {2, 0}, {1, 0}));

        Board oneForwardNotInitialBlack = loadBoardFromRows({". . . .", "bP . . .", ". . . .", ". . . ."});
        CHECK(isValidMove(oneForwardNotInitialBlack, {1, 0}, {2, 0}));

        Board initialRowOneForwardWhite = loadBoardFromRows({". . . .", "wP . . .", ". . . .", ". . . ."});
        CHECK(isValidMove(initialRowOneForwardWhite, {1, 0}, {0, 0}));
        CHECK_FALSE(isValidMove(initialRowOneForwardWhite, {1, 0}, {2, 0}));

        Board initialRowOneForwardBlack = loadBoardFromRows({
            ". . . .", ". . . .", ". . . .", ". . . .",
            ". . . .", ". . . .", "bP . . .", ". . . ."
        });
        CHECK(isValidMove(initialRowOneForwardBlack, {6, 0}, {7, 0}));

        Board initialRowTwoForwardBlack = loadBoardFromRows({
            ". . . .", ". . . .", ". . . .", ". . . .",
            ". . . .", ". . . .", "bP . . .", ". . . .", ". . . ."
        });
        CHECK(isValidMove(initialRowTwoForwardBlack, {6, 0}, {8, 0}));

        Board initialRowCanChooseBlack = loadBoardFromRows({
            ". . . .", ". . . .", ". . . .", ". . . .",
            ". . . .", ". . . .", "bP . . .", ". . . .", ". . . ."
        });
        CHECK(isValidMove(initialRowCanChooseBlack, {6, 0}, {7, 0}));
        CHECK(isValidMove(initialRowCanChooseBlack, {6, 0}, {8, 0}));

        Board initialRowOneForwardWhiteOutOfBounds = loadBoardFromRows({". . . .", "wP . . .", ". . . ."});
        CHECK(isValidMove(initialRowOneForwardWhiteOutOfBounds, {1, 0}, {0, 0}));
        CHECK_FALSE(isValidMove(initialRowOneForwardWhiteOutOfBounds, {1, 0}, {-1, 0}));
    }

    SUBCASE("illegal moves") {
        Board longMoveInvalid = loadBoardFromRows({". . . .", ". . . .", "wP . . ."});
        CHECK_FALSE(isValidMove(longMoveInvalid, {2, 0}, {0, 0}));

        Board backwardWhite = loadBoardFromRows({". . . .", "wP . . .", ". . . ."});
        CHECK_FALSE(isValidMove(backwardWhite, {1, 0}, {2, 0}));

        Board backwardBlack = loadBoardFromRows({". . . .", "bP . . .", ". . . ."});
        CHECK_FALSE(isValidMove(backwardBlack, {1, 0}, {0, 0}));

        Board sideways = loadBoardFromRows({". . . .", "wP . . .", ". . . ."});
        CHECK_FALSE(isValidMove(sideways, {1, 0}, {1, 1}));
        CHECK_FALSE(isValidMove(sideways, {1, 0}, {1, 2}));

        Board threeForward = loadBoardFromRows({
            ". . . .", ". . . .", ". . . .", ". . . .",
            ". . . .", ". . . .", "bP . . .", ". . . .",
            ". . . .", ". . . ."
        });
        CHECK_FALSE(isValidMove(threeForward, {6, 0}, {9, 0}));

        Board twoForwardNotInitialWhite = loadBoardFromRows({". . . .", ". . . .", "wP . . .", ". . . ."});
        CHECK_FALSE(isValidMove(twoForwardNotInitialWhite, {2, 0}, {0, 0}));

        Board twoForwardNotInitialBlack = loadBoardFromRows({
            ". . . .", ". . . .", ". . . .", ". . . .",
            ". . . .", "bP . . .", ". . . .", ". . . ."
        });
        CHECK_FALSE(isValidMove(twoForwardNotInitialBlack, {5, 0}, {7, 0}));

        Board twoForwardBlackBlockedPath = loadBoardFromRows({
            ". . . .", ". . . .", ". . . .", ". . . .",
            ". . . .", ". . . .", "bP . . .", "wR . . .", ". . . ."
        });
        CHECK_FALSE(isValidMove(twoForwardBlackBlockedPath, {6, 0}, {8, 0}));

        Board twoForwardBlackDestinationBlocked = loadBoardFromRows({
            ". . . .", ". . . .", ". . . .", ". . . .",
            ". . . .", ". . . .", "bP . . .", ". . . .", "wR . . ."
        });
        CHECK_FALSE(isValidMove(twoForwardBlackDestinationBlocked, {6, 0}, {8, 0}));

        Board twoForwardWhiteOutOfBounds = loadBoardFromRows({". . . .", "wP . . .", ". . . ."});
        CHECK_FALSE(isValidMove(twoForwardWhiteOutOfBounds, {1, 0}, {-1, 0}));

        Board forwardCaptureInvalid = loadBoardFromRows({"bN . .", "wP . .", ". . ."});
        CHECK_FALSE(isValidMove(forwardCaptureInvalid, {1, 0}, {0, 0}));

        Board diagonalWithoutEnemy = loadBoardFromRows({". . .", ". wP .", ". . ."});
        CHECK_FALSE(isValidMove(diagonalWithoutEnemy, {1, 1}, {0, 2}));

        Board diagonalWithFriendly = loadBoardFromRows({". wN .", ". wP .", ". . ."});
        CHECK_FALSE(isValidMove(diagonalWithFriendly, {1, 1}, {0, 0}));

        Board blockedForward = loadBoardFromRows({"wR . .", "wP . .", ". . ."});
        CHECK_FALSE(isValidMove(blockedForward, {1, 0}, {0, 0}));

        Board diagonalWrongDirection = loadBoardFromRows({". . .", "bN . .", ". wP .", ". . ."});
        CHECK_FALSE(isValidMove(diagonalWrongDirection, {2, 1}, {3, 0}));
        CHECK_FALSE(isValidMove(diagonalWrongDirection, {2, 1}, {3, 2}));

        Board stayInPlace = loadBoardFromRows({". . . .", "wP . . .", ". . . ."});
        CHECK_FALSE(isValidMove(stayInPlace, {1, 0}, {1, 0}));
    }

    SUBCASE("captures") {
        Board diagonalCaptureWhite = loadBoardFromRows({"bN . .", ". wP .", ". . ."});
        CHECK(isValidMove(diagonalCaptureWhite, {1, 1}, {0, 0}));

        Board diagonalCaptureBlackLeft = loadBoardFromRows({". . .", ". bP .", "wN . ."});
        CHECK(isValidMove(diagonalCaptureBlackLeft, {1, 1}, {2, 0}));

        Board diagonalCaptureBlackRight = loadBoardFromRows({". . .", ". bP .", ". . wN"});
        CHECK(isValidMove(diagonalCaptureBlackRight, {1, 1}, {2, 2}));

        Board diagonalCaptureBothDirections = loadBoardFromRows({"bN . bR", ". wP .", ". . ."});
        CHECK(isValidMove(diagonalCaptureBothDirections, {1, 1}, {0, 0}));
        CHECK(isValidMove(diagonalCaptureBothDirections, {1, 1}, {0, 2}));
    }
}
