#include "TestFramework.h"
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

void testKingLegalMove() {
    Board board = loadBoardFromRows({". wK . ."});
    ASSERT_TRUE(isValidMove(board, {0, 1}, {0, 2}));
    ASSERT_TRUE(isValidMove(board, {0, 1}, {1, 2}));
}

void testKingIllegalMove() {
    Board board = loadBoardFromRows({". wK . ."});
    ASSERT_FALSE(isValidMove(board, {0, 1}, {0, 3}));
    ASSERT_FALSE(isValidMove(board, {0, 1}, {0, 1}));
}

void testRookLegalMove() {
    Board board = loadBoardFromRows({"wR . . .", ". . . ."});
    ASSERT_TRUE(isValidMove(board, {0, 0}, {0, 3}));
    ASSERT_TRUE(isValidMove(board, {0, 0}, {1, 0}));
}

void testRookIllegalMove() {
    Board board = loadBoardFromRows({"wR . . .", ". . . ."});
    ASSERT_FALSE(isValidMove(board, {0, 0}, {1, 1}));
    ASSERT_FALSE(isValidMove(board, {0, 0}, {2, 2}));
}

void testRookBlockedPath() {
    Board board = loadBoardFromRows({"wR . wP .", ". . . ."});
    ASSERT_FALSE(isValidMove(board, {0, 0}, {0, 3}));
}

void testBishopLegalMove() {
    Board board = loadBoardFromRows({"wB . . .", ". . . .", ". . . ."});
    ASSERT_TRUE(isValidMove(board, {0, 0}, {2, 2}));
}

void testBishopIllegalMove() {
    Board board = loadBoardFromRows({"wB . . .", ". . . ."});
    ASSERT_FALSE(isValidMove(board, {0, 0}, {0, 2}));
    ASSERT_FALSE(isValidMove(board, {0, 0}, {2, 0}));
}

void testBishopBlockedPath() {
    Board board = loadBoardFromRows({"wB . . .", ". wP . .", ". . . .", ". . . ."});
    ASSERT_FALSE(isValidMove(board, {0, 0}, {2, 2}));
}

void testQueenLegalMove() {
    Board board = loadBoardFromRows({". wQ . .", ". . . ."});
    ASSERT_TRUE(isValidMove(board, {0, 1}, {0, 3}));
    ASSERT_TRUE(isValidMove(board, {0, 1}, {2, 3}));
}

void testQueenIllegalMove() {
    Board board = loadBoardFromRows({". wQ . .", ". . . ."});
    ASSERT_FALSE(isValidMove(board, {0, 1}, {2, 2}));
}

void testKnightLegalMove() {
    Board board = loadBoardFromRows({". wN . .", ". . . .", ". . . ."});
    ASSERT_TRUE(isValidMove(board, {0, 1}, {2, 0}));
    ASSERT_TRUE(isValidMove(board, {0, 1}, {1, 3}));
}

void testKnightIllegalMove() {
    Board board = loadBoardFromRows({". wN . .", ". . . ."});
    ASSERT_FALSE(isValidMove(board, {0, 1}, {0, 3}));
    ASSERT_FALSE(isValidMove(board, {0, 1}, {0, 2}));
}

void testEmptySourceIsInvalid() {
    Board board = loadBoardFromRows({". . . ."});
    ASSERT_FALSE(isValidMove(board, {0, 0}, {0, 1}));
}

void registerMoveRulesTests() {
    runTest("MoveRules.kingLegal", testKingLegalMove);
    runTest("MoveRules.kingIllegal", testKingIllegalMove);
    runTest("MoveRules.rookLegal", testRookLegalMove);
    runTest("MoveRules.rookIllegal", testRookIllegalMove);
    runTest("MoveRules.rookBlockedPath", testRookBlockedPath);
    runTest("MoveRules.bishopLegal", testBishopLegalMove);
    runTest("MoveRules.bishopIllegal", testBishopIllegalMove);
    runTest("MoveRules.bishopBlockedPath", testBishopBlockedPath);
    runTest("MoveRules.queenLegal", testQueenLegalMove);
    runTest("MoveRules.queenIllegal", testQueenIllegalMove);
    runTest("MoveRules.knightLegal", testKnightLegalMove);
    runTest("MoveRules.knightIllegal", testKnightIllegalMove);
    runTest("MoveRules.emptySourceInvalid", testEmptySourceIsInvalid);
}
