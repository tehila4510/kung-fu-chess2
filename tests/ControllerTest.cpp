#include "doctest.h"
#include "engine/GameEngine.h"
#include "input/Controller.h"
#include "io/BoardParser.h"

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

TEST_CASE("Controller drives selection from pixel clicks") {
    GameEngine engine = loadEngine({
        "wR . . .",
        ". . . .",
        "bR . . ." });
    Controller controller(engine, 100);

    SUBCASE("first click on a piece selects it") {
        const ClickResult r = controller.click(50, 50);
        CHECK(r.outcome == ClickOutcome::Selected);
        CHECK(controller.hasActiveSelection());
        CHECK(controller.selectedCell() == Position{ 0, 0 });
    }

    SUBCASE("second click requests a move to the destination") {
        controller.click(50, 50);
        const ClickResult r = controller.click(350, 50);
        CHECK(r.outcome == ClickOutcome::MoveRequested);
        CHECK(r.moveResult.is_accepted);
        CHECK_FALSE(controller.hasActiveSelection());
    }

    SUBCASE("clicking an empty cell with no selection is ignored") {
        const ClickResult r = controller.click(150, 50);
        CHECK(r.outcome == ClickOutcome::Ignored);
        CHECK_FALSE(controller.hasActiveSelection());
    }

    SUBCASE("clicking the selected cell again cancels the selection") {
        controller.click(50, 50);
        const ClickResult r = controller.click(50, 50);
        CHECK(r.outcome == ClickOutcome::Cleared);
        CHECK_FALSE(controller.hasActiveSelection());
    }

    SUBCASE("a click with negative pixels cancels the selection") {
        controller.click(50, 50);
        const ClickResult r = controller.click(-5, 50);
        CHECK(r.outcome == ClickOutcome::Cleared);
        CHECK_FALSE(controller.hasActiveSelection());
    }

    SUBCASE("a click beyond the board width cancels the selection") {
        controller.click(50, 50);
        const ClickResult r = controller.click(400, 50);
        CHECK(r.outcome == ClickOutcome::Cleared);
        CHECK_FALSE(controller.hasActiveSelection());
    }

    SUBCASE("a click beyond the board height cancels the selection") {
        controller.click(50, 50);
        const ClickResult r = controller.click(50, 300);
        CHECK(r.outcome == ClickOutcome::Cleared);
        CHECK_FALSE(controller.hasActiveSelection());
    }

    SUBCASE("the bottom-left piece is selectable within dynamic bounds") {
        const ClickResult r = controller.click(50, 250);
        CHECK(r.outcome == ClickOutcome::Selected);
        CHECK(controller.selectedCell() == Position{ 2, 0 });
    }
}

TEST_CASE("Controller respects a wide rectangular board's column bound") {
    GameEngine engine = loadEngine({
        "wR . . . . .",
        "bR . . . . ." });
    Controller controller(engine, 100);

    SUBCASE("the last column is still on the board") {
        const ClickResult r = controller.click(550, 50);
        CHECK(r.outcome == ClickOutcome::Ignored);
    }

    SUBCASE("one column past the width is off the board") {
        controller.click(50, 50);
        const ClickResult r = controller.click(600, 50);
        CHECK(r.outcome == ClickOutcome::Cleared);
    }

    SUBCASE("a row past the shorter height is off the board") {
        controller.click(50, 50);
        const ClickResult r = controller.click(50, 200);
        CHECK(r.outcome == ClickOutcome::Cleared);
    }
}

TEST_CASE("Controller accepts an explicit mapper override") {
    GameEngine engine = loadEngine({
        "wR . . .",
        ". . . .",
        "bR . . ." });
    Controller controller(engine, BoardMapper(50, 4, 3));

    SUBCASE("clicks use the overridden cell size") {
        const ClickResult r = controller.click(25, 25);
        CHECK(r.outcome == ClickOutcome::Selected);
        CHECK(controller.selectedCell() == Position{ 0, 0 });
    }

    SUBCASE("bounds follow the overridden geometry") {
        controller.click(25, 25);
        const ClickResult r = controller.click(200, 25);
        CHECK(r.outcome == ClickOutcome::Cleared);
    }
}
