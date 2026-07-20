#include "doctest.h"
#include "realtime/RealTimeArbiter.h"
#include "model/Board.h"
#include "io/BoardParser.h"

#include <string>
#include <vector>

namespace {

Board loadBoard(const std::vector<std::string>& rows) {
    std::string text;
    for (const std::string& row : rows) {
        text += row;
        text += '\n';
    }

    BoardParseResult parsed = BoardParser().parseText(text);
    REQUIRE(parsed.board.has_value());
    return std::move(*parsed.board);
}

}

TEST_CASE("RealTimeArbiter times and resolves motion") {
    SUBCASE("no motion is inactive") {
        Board board = loadBoard({ "wR . . ." });
        RealTimeArbiter arb;
        CHECK_FALSE(arb.hasActiveMotion());
        CHECK(arb.advanceTime(1000, board).empty());
    }

    SUBCASE("motion stays in flight until arrival") {
        Board board = loadBoard({ "wR . . ." });
        RealTimeArbiter arb;
        arb.startMotion("wR", { 0, 0 }, { 0, 3 });
        CHECK(arb.hasActiveMotion());

        CHECK(arb.advanceTime(2999, board).empty());
        CHECK(arb.hasActiveMotion());
        CHECK(board.getCell({ 0, 0 }).getContent() == "wR");
    }

    SUBCASE("arrival moves the piece and clears the motion") {
        Board board = loadBoard({ "wR . . ." });
        RealTimeArbiter arb;
        arb.startMotion("wR", { 0, 0 }, { 0, 3 });

        auto arrivals = arb.advanceTime(3000, board);
        REQUIRE(arrivals.size() == 1);
        CHECK(arrivals[0].at == Position{ 0, 3 });
        CHECK(arrivals[0].capturedPiece == ".");
        CHECK(board.getCell({ 0, 3 }).getContent() == "wR");
        CHECK(board.getCell({ 0, 0 }).getContent() == ".");
        CHECK_FALSE(arb.hasActiveMotion());
    }

    SUBCASE("capture reports the taken piece") {
        Board board = loadBoard({ "wR . . bK" });
        RealTimeArbiter arb;
        arb.startMotion("wR", { 0, 0 }, { 0, 3 });

        auto arrivals = arb.advanceTime(3000, board);
        REQUIRE(arrivals.size() == 1);
        CHECK(arrivals[0].capturedPiece == "bK");
        CHECK(board.getCell({ 0, 3 }).getContent() == "wR");
    }

    SUBCASE("pawn promotes on the far rank") {
        Board board = loadBoard({
            ". . . .",
            "wP . . ." });
        RealTimeArbiter arb;
        arb.startMotion("wP", { 1, 0 }, { 0, 0 });

        auto arrivals = arb.advanceTime(1000, board);
        REQUIRE(arrivals.size() == 1);
        CHECK(board.getCell({ 0, 0 }).getContent() == "wQ");
        CHECK(arrivals[0].promoted);
        CHECK(arrivals[0].piece == "wQ");
    }

    SUBCASE("both colors move simultaneously and resolve together") {
        Board board = loadBoard({
            "wR . . .",
            ". . . bR" });
        RealTimeArbiter arb;
        arb.startMotion("wR", { 0, 0 }, { 0, 2 });
        arb.startMotion("bR", { 1, 3 }, { 1, 1 });

        CHECK(arb.hasActiveMotion('w'));
        CHECK(arb.hasActiveMotion('b'));

        auto arrivals = arb.advanceTime(2000, board);
        CHECK(arrivals.size() == 2);
        CHECK(board.getCell({ 0, 2 }).getContent() == "wR");
        CHECK(board.getCell({ 1, 1 }).getContent() == "bR");
        CHECK_FALSE(arb.hasActiveMotion());
    }

    SUBCASE("one color in flight leaves the other free to move") {
        Board board = loadBoard({ "wR . . ." });
        RealTimeArbiter arb;
        arb.startMotion("wR", { 0, 0 }, { 0, 3 });

        CHECK(arb.hasActiveMotion('w'));
        CHECK_FALSE(arb.hasActiveMotion('b'));
    }

    SUBCASE("pursuer lands on fled square without capturing or vanishing") {
        Board board = loadBoard({ "wR bK . ." });
        RealTimeArbiter arb;
        arb.startMotion("wR", { 0, 0 }, { 0, 1 });
        arb.startMotion("bK", { 0, 1 }, { 0, 3 });

        auto mid = arb.advanceTime(1000, board);
        REQUIRE(mid.size() == 1);
        CHECK(mid[0].capturedPiece == ".");
        CHECK(board.getCell({ 0, 1 }).getContent() == "wR");
        CHECK(arb.hasActiveMotion('b'));

        auto end = arb.advanceTime(2000, board);
        REQUIRE(end.size() == 1);
        CHECK(end[0].capturedPiece == ".");
        CHECK(board.getCell({ 0, 1 }).getContent() == "wR");
        CHECK(board.getCell({ 0, 3 }).getContent() == "bK");
        CHECK_FALSE(arb.hasActiveMotion());
    }

    SUBCASE("traveler enters jump square while jumper is airborne then gets captured on landing") {
        Board board = loadBoard({
            "bP wR .",
            ". . ." });
        RealTimeArbiter arb;
        arb.startJump("bP", { 0, 0 });
        board.setCell({ 0, 0 }, ".");
        arb.startMotion("wR", { 0, 1 }, { 0, 0 });

        auto arrivals = arb.advanceTime(RealTimeArbiter::kJumpDurationMs, board);
        REQUIRE(arrivals.size() == 2);
        CHECK(board.getCell({ 0, 0 }).getContent() == "bP");
        CHECK(arrivals[0].capturedPiece == ".");
        CHECK(arrivals[1].capturedPiece == "wR");
    }

    SUBCASE("traveler captures jumper when move started before the jump at same tick") {
        // Distance 5 matches kJumpDurationMs so both land on the same clock tick;
        // later startSeq (the jump) resolves first, then the traveler captures.
        Board board = loadBoard({
            "bP . . . . wR",
            ". . . . . ." });
        RealTimeArbiter arb;
        arb.startMotion("wR", { 0, 5 }, { 0, 0 });
        arb.startJump("bP", { 0, 0 });
        board.setCell({ 0, 0 }, ".");

        auto arrivals = arb.advanceTime(RealTimeArbiter::kJumpDurationMs, board);
        REQUIRE(arrivals.size() == 2);
        CHECK(board.getCell({ 0, 0 }).getContent() == "wR");
        CHECK(arrivals[0].capturedPiece == ".");
        CHECK(arrivals[1].capturedPiece == "bP");
    }
}
