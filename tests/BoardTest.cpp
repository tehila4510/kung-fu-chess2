#include "TestFramework.h"
#include "Board.h"

#include <sstream>
#include <streambuf>
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

static std::string captureBoardPrint(const Board& board) {
    std::ostringstream buffer;
    std::streambuf* oldBuffer = std::cout.rdbuf(buffer.rdbuf());
    board.print();
    std::cout.rdbuf(oldBuffer);
    return buffer.str();
}

static std::string captureSetupOutput(const std::vector<std::string>& lines) {
    std::ostringstream buffer;
    std::streambuf* oldBuffer = std::cout.rdbuf(buffer.rdbuf());

    Board board;
    size_t index = 0;
    board.loadFromLines(lines, index);

    std::cout.rdbuf(oldBuffer);
    return buffer.str();
}

void testBoardLoadValid() {
    Board board = loadBoardFromRows({". wK . .", ". . . ."});
    ASSERT_EQ(".", board.getCell({0, 0}));
    ASSERT_EQ("wK", board.getCell({0, 1}));
    ASSERT_TRUE(board.isWithinBounds({0, 0}));
    ASSERT_TRUE(board.isWithinBounds({1, 3}));
    ASSERT_FALSE(board.isWithinBounds({2, 0}));
}

void testBoardUnknownToken() {
    std::vector<std::string> lines = {
        "Board:",
        ". xx . .",
        "Commands:"
    };
    size_t index = 0;
    Board board;
    ASSERT_FALSE(board.loadFromLines(lines, index));
    ASSERT_EQ("ERROR UNKNOWN_TOKEN\n", captureSetupOutput(lines));
}

void testBoardRowWidthMismatch() {
    std::vector<std::string> lines = {
        "Board:",
        ". wK . .",
        ". . .",
        "Commands:"
    };
    size_t index = 0;
    Board board;
    ASSERT_FALSE(board.loadFromLines(lines, index));
    ASSERT_EQ("ERROR ROW_WIDTH_MISMATCH\n", captureSetupOutput(lines));
}

void testBoardPixelToCell() {
    Board board = loadBoardFromRows({". . ."});
    Position pos = board.pixelToCell(250, 150);
    ASSERT_EQ(1, pos.row);
    ASSERT_EQ(2, pos.col);
}

void testBoardCellAccessAndEmptiness() {
    Board board = loadBoardFromRows({"wR . bQ"});
    ASSERT_FALSE(board.isEmpty({0, 0}));
    ASSERT_TRUE(board.isEmpty({0, 1}));
    board.setCell({0, 1}, "wN");
    ASSERT_EQ("wN", board.getCell({0, 1}));
}

void testBoardFriendly() {
    Board board = loadBoardFromRows({"wR bQ wB"});
    ASSERT_TRUE(board.isFriendly({0, 0}, 'w'));
    ASSERT_TRUE(board.isFriendly({0, 2}, 'w'));
    ASSERT_TRUE(board.isFriendly({0, 1}, 'b'));
    ASSERT_FALSE(board.isFriendly({0, 1}, 'w'));
}

void testBoardPrint() {
    Board board = loadBoardFromRows({"wK .", ". bQ"});
    ASSERT_EQ("wK .\n. bQ\n", captureBoardPrint(board));
}

void registerBoardTests() {
    runTest("Board.loadValid", testBoardLoadValid);
    runTest("Board.unknownToken", testBoardUnknownToken);
    runTest("Board.rowWidthMismatch", testBoardRowWidthMismatch);
    runTest("Board.pixelToCell", testBoardPixelToCell);
    runTest("Board.cellAccessAndEmptiness", testBoardCellAccessAndEmptiness);
    runTest("Board.friendly", testBoardFriendly);
    runTest("Board.print", testBoardPrint);
}
