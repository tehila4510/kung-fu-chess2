#include "doctest.h"
#include "engine/GameEngine.h"
#include "io/BoardParser.h"
#include "realtime/RealTimeArbiter.h"

#include <string>
#include <vector>

namespace {

GameEngine loadEngine(const std::vector<std::string>& rows) {
    std::string text;
    for (const std::string& row : rows) {
        text += row;
        text += '\n';
    }

    BoardParseResult parsed = BoardParser().parseText(text);
    REQUIRE(parsed.board.has_value());

    GameEngine engine;
    engine.setup(std::move(*parsed.board));
    return engine;
}

}

TEST_CASE("GameEngine orchestrates moves, timing and game over") {
    SUBCASE("snapshot reflects the loaded board") {
        GameEngine engine = loadEngine({ "wR . . bK" });
        const GameSnapshot snap = engine.snapshot();
        CHECK(snap.cells[0][0] == "wR");
        CHECK(snap.cells[0][3] == "bK");
        CHECK_FALSE(snap.gameOver);
    }

    SUBCASE("a legal move is accepted") {
        GameEngine engine = loadEngine({ "wR . . ." });
        const MoveOutcome result = engine.requestMove({ 0, 0 }, { 0, 3 });
        CHECK(result.is_accepted);
    }

    SUBCASE("the moving player is disabled while their piece is in flight") {
        GameEngine engine = loadEngine({
            "wR . . .",
            ". . . .",
            "bR . . ." });

        CHECK(engine.requestMove({ 0, 0 }, { 0, 3 }).is_accepted);
        const MoveOutcome second = engine.requestMove({ 0, 0 }, { 0, 1 });
        CHECK_FALSE(second.is_accepted);
        CHECK(second.reason == "move_in_flight");
    }

    SUBCASE("opposite colors may travel while the other player is in flight") {
        GameEngine engine = loadEngine({
            "wR . . .",
            ". . . .",
            "bR . . ." });

        CHECK(engine.requestMove({ 0, 0 }, { 0, 3 }).is_accepted);
        const MoveOutcome second = engine.requestMove({ 2, 0 }, { 2, 3 });
        CHECK(second.is_accepted);
    }

    SUBCASE("capturing the king ends the game") {
        GameEngine engine = loadEngine({ "wR . . bK" });
        CHECK(engine.requestMove({ 0, 0 }, { 0, 3 }).is_accepted);

        engine.wait(3000);
        CHECK(engine.isGameOver());
        CHECK(engine.snapshot().gameOver);

        const MoveOutcome afterOver = engine.requestMove({ 0, 3 }, { 1, 3 });
        CHECK_FALSE(afterOver.is_accepted);
        CHECK(afterOver.reason == "game_over");
    }

    SUBCASE("opponent may move into a square while a jump is airborne") {
        GameEngine engine = loadEngine({
            "bP wR .",
            ". . ." });
        CHECK(engine.requestJump({ 0, 0 }).is_accepted);
        CHECK(engine.snapshot().cells[0][0] == ".");

        const MoveOutcome move = engine.requestMove({ 0, 1 }, { 0, 0 });
        CHECK(move.is_accepted);
    }

    SUBCASE("pawn may move diagonally onto an airborne enemy jump square") {
        GameEngine engine = loadEngine({
            "bP . .",
            ". wP ." });
        CHECK(engine.requestJump({ 0, 0 }).is_accepted);
        CHECK(engine.snapshot().cells[0][0] == ".");

        const MoveOutcome move = engine.requestMove({ 1, 1 }, { 0, 0 });
        CHECK(move.is_accepted);
    }

    SUBCASE("jumper captures a piece that entered the square while airborne") {
        GameEngine engine = loadEngine({
            "bP wR .",
            ". . ." });
        CHECK(engine.requestJump({ 0, 0 }).is_accepted);
        CHECK(engine.requestMove({ 0, 1 }, { 0, 0 }).is_accepted);

        engine.wait(RealTimeArbiter::kJumpDurationMs);
        CHECK(engine.snapshot().cells[0][0] == "bP");
        CHECK(engine.snapshot().cells[0][1] == ".");
    }
}
