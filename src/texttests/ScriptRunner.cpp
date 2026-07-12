#include "texttests/ScriptRunner.h"

#include "engine/GameEngine.h"
#include "input/Controller.h"
#include "io/BoardParser.h"
#include "io/BoardPrinter.h"
#include "texttests/ScriptParser.h"

#include <ostream>

namespace {
constexpr int kCellSize = 100;
}

bool ScriptRunner::run(std::istream& input, std::ostream& output) const {
    ScriptParser parser;
    ScriptParseResult parsed = parser.parse(input);

    if (!parsed.board.has_value()) {
        output << toReasonCode(parsed.boardStatus) << '\n';
        return false;
    }

    GameEngine engine;
    engine.setup(std::move(*parsed.board));
    Controller controller(engine, kCellSize);
    BoardPrinter printer;

    for (const ScriptCommand& cmd : parsed.commands) {
        switch (cmd.kind) {
            case ScriptCommandKind::Click:
                controller.click(cmd.x, cmd.y);
                break;
            case ScriptCommandKind::Jump:
                controller.jump(cmd.x, cmd.y);
                break;
            case ScriptCommandKind::Wait:
                engine.wait(cmd.ms);
                break;
            case ScriptCommandKind::PrintBoard:
                printer.print(Board(engine.snapshot().cells), output);
                break;
            case ScriptCommandKind::Unknown:
                break;
        }
    }
    return true;
}
