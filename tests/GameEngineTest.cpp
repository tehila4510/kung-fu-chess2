#include "doctest.h"
#include "engine/GameEngine.h"

#include <string>
#include <vector>

namespace {

GameEngine loadEngine(const std::vector<std::string>& rows) {
    std::vector<std::string> lines = { "Board:" };
    lines.insert(lines.end(), rows.begin(), rows.end());
    lines.push_back("Commands:");

    GameEngine engine;
    size_t index = 0;
    engine.setup(lines, index);
    return engine;
}

} // namespace

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
        const MoveResult result = engine.requestMove({ 0, 0 }, { 0, 3 });
        CHECK(result.is_accepted);
    }

    SUBCASE("the moving player is disabled while their piece is in flight") {
        GameEngine engine = loadEngine({
            "wR . . .",
            ". . . .",
            "bR . . ." });

        CHECK(engine.requestMove({ 0, 0 }, { 0, 3 }).is_accepted); // white in flight
        const MoveResult second = engine.requestMove({ 0, 0 }, { 0, 1 });
        CHECK_FALSE(second.is_accepted);
        CHECK(second.reason == "move_in_flight");
    }

    SUBCASE("the opponent may move while the other player is in flight") {
        GameEngine engine = loadEngine({
            "wR . . .",
            ". . . .",
            "bR . . ." });

        CHECK(engine.requestMove({ 0, 0 }, { 0, 3 }).is_accepted); // white in flight
        CHECK(engine.requestMove({ 2, 0 }, { 2, 3 }).is_accepted); // black still free
    }

    SUBCASE("capturing the king ends the game") {
        GameEngine engine = loadEngine({ "wR . . bK" });
        CHECK(engine.requestMove({ 0, 0 }, { 0, 3 }).is_accepted);

        engine.wait(3000); // rook travels 3 cells -> 3000 ms
        CHECK(engine.isGameOver());
        CHECK(engine.snapshot().gameOver);

        const MoveResult afterOver = engine.requestMove({ 0, 3 }, { 1, 3 });
        CHECK_FALSE(afterOver.is_accepted);
        CHECK(afterOver.reason == "game_over");
    }
}
