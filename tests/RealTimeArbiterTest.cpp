#include "doctest.h"
#include "realtime/RealTimeArbiter.h"
#include "model/Board.h"

#include <string>
#include <vector>

namespace {

Board loadBoard(const std::vector<std::string>& rows) {
    std::vector<std::string> lines = { "Board:" };
    lines.insert(lines.end(), rows.begin(), rows.end());
    lines.push_back("Commands:");

    Board board;
    size_t index = 0;
    board.loadFromLines(lines, index);
    return board;
}

} // namespace

TEST_CASE("RealTimeArbiter times and resolves motion") {
    SUBCASE("no motion is inactive") {
        Board board = loadBoard({ "wR . . ." });
        RealTimeArbiter arb;
        CHECK_FALSE(arb.hasActiveMotion());
        CHECK_FALSE(arb.advanceTime(1000, board).has_value());
    }

    SUBCASE("motion stays in flight until arrival") {
        Board board = loadBoard({ "wR . . ." });
        RealTimeArbiter arb;
        arb.startMotion("wR", { 0, 0 }, { 0, 3 }); // 3 cells -> 3000 ms
        CHECK(arb.hasActiveMotion());

        CHECK_FALSE(arb.advanceTime(2999, board).has_value());
        CHECK(arb.hasActiveMotion());
        CHECK(board.getCell({ 0, 0 }) == "wR"); // not moved yet
    }

    SUBCASE("arrival moves the piece and clears the motion") {
        Board board = loadBoard({ "wR . . ." });
        RealTimeArbiter arb;
        arb.startMotion("wR", { 0, 0 }, { 0, 3 });

        auto arrival = arb.advanceTime(3000, board);
        REQUIRE(arrival.has_value());
        CHECK(arrival->at == Position{ 0, 3 });
        CHECK(arrival->capturedPiece == ".");
        CHECK(board.getCell({ 0, 3 }) == "wR");
        CHECK(board.getCell({ 0, 0 }) == ".");
        CHECK_FALSE(arb.hasActiveMotion());
    }

    SUBCASE("capture reports the taken piece") {
        Board board = loadBoard({ "wR . . bK" });
        RealTimeArbiter arb;
        arb.startMotion("wR", { 0, 0 }, { 0, 3 });

        auto arrival = arb.advanceTime(3000, board);
        REQUIRE(arrival.has_value());
        CHECK(arrival->capturedPiece == "bK");
        CHECK(board.getCell({ 0, 3 }) == "wR");
    }

    SUBCASE("pawn promotes on the far rank") {
        Board board = loadBoard({
            ". . . .",
            "wP . . ." });
        RealTimeArbiter arb;
        arb.startMotion("wP", { 1, 0 }, { 0, 0 });

        auto arrival = arb.advanceTime(1000, board);
        REQUIRE(arrival.has_value());
        CHECK(board.getCell({ 0, 0 }) == "wQ");
    }
}
