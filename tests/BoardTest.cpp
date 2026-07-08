#include "doctest.h"
#include "Board.h"

#include <iostream>
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

TEST_CASE("Board.loadValid") {
    Board board = loadBoardFromRows({". wK . .", ". . . ."});
    CHECK_EQ(".", board.getCell({0, 0}));
    CHECK_EQ("wK", board.getCell({0, 1}));
    CHECK(board.isWithinBounds({0, 0}));
    CHECK(board.isWithinBounds({1, 3}));
    CHECK_FALSE(board.isWithinBounds({2, 0}));
}

TEST_CASE("Board.unknownToken") {
    std::vector<std::string> lines = {
        "Board:",
        ". xx . .",
        "Commands:"
    };
    size_t index = 0;
    Board board;
    CHECK_FALSE(board.loadFromLines(lines, index));
    CHECK_EQ("ERROR UNKNOWN_TOKEN\n", captureSetupOutput(lines));
}

TEST_CASE("Board.rowWidthMismatch") {
    std::vector<std::string> lines = {
        "Board:",
        ". wK . .",
        ". . .",
        "Commands:"
    };
    size_t index = 0;
    Board board;
    CHECK_FALSE(board.loadFromLines(lines, index));
    CHECK_EQ("ERROR ROW_WIDTH_MISMATCH\n", captureSetupOutput(lines));
}

TEST_CASE("Board.pixelToCell") {
    Board board = loadBoardFromRows({". . ."});
    Position pos = board.pixelToCell(250, 150);
    CHECK_EQ(1, pos.row);
    CHECK_EQ(2, pos.col);
}

TEST_CASE("Board.cellAccessAndEmptiness") {
    Board board = loadBoardFromRows({"wR . bQ"});
    CHECK_FALSE(board.isEmpty({0, 0}));
    CHECK(board.isEmpty({0, 1}));
    board.setCell({0, 1}, "wN");
    CHECK_EQ("wN", board.getCell({0, 1}));
}

TEST_CASE("Board.friendly") {
    Board board = loadBoardFromRows({"wR bQ wB"});
    CHECK(board.isFriendly({0, 0}, 'w'));
    CHECK(board.isFriendly({0, 2}, 'w'));
    CHECK(board.isFriendly({0, 1}, 'b'));
    CHECK_FALSE(board.isFriendly({0, 1}, 'w'));
}

TEST_CASE("Board.print") {
    Board board = loadBoardFromRows({"wK .", ". bQ"});
    CHECK_EQ("wK .\n. bQ\n", captureBoardPrint(board));
}
