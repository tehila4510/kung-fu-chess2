#include "GraphicsApplication.h"
#include "bus/EventBus.h"
#include "bus/GameEvent.h"
#include "client/IMatchmaker.h"
#include "client/RemoteStateSource.h"
#include "client/SessionBootstrap.h"
#include "client/WebSocketClient.h"
#include "engine/GameEngine.h"
#include "graphics/AssetPaths.h"
#include "graphics/BoardLayoutLoader.h"
#include "graphics/IBoardInput.h"
#include "graphics/IFrameSideEffects.h"
#include "graphics/LocalEngineStateSource.h"
#include "input/Controller.h"
#include "model/GameState.h"
#include "model/Position.h"
#include "view/Img.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

namespace {

constexpr const char* kDefaultHost = "127.0.0.1";
constexpr std::uint16_t kDefaultPort = 9002;

struct CliOptions {
    bool offline = false;
    std::string host = kDefaultHost;
    std::uint16_t port = kDefaultPort;
};

CliOptions parseArgs(int argc, char** argv) {
    CliOptions opts;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--offline") {
            opts.offline = true;
        } else if (arg == "--host" && i + 1 < argc) {
            opts.host = argv[++i];
        } else if (arg == "--port" && i + 1 < argc) {
            opts.port = static_cast<std::uint16_t>(std::stoi(argv[++i]));
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: KungFuChessGraphics [--offline] [--host H] [--port P]\n"
                      << "  Default: online ws://127.0.0.1:9002 (shell AUTH then GUI)\n"
                      << "  --offline  local GameEngine, no server\n";
            std::exit(0);
        }
    }
    return opts;
}

int runOffline(const graphics::BoardLayout& layout, view::Img background) {
    GameEngine engine;
    engine.setup(graphics::boardFromAssetLayout(layout));

    graphics::LocalEngineStateSource frameSource(engine);
    client::NoOpMatchmaker matchmaker;
    EventBus bus;
    Controller controller(engine);
    graphics::LocalFrameSideEffects sideEffects(engine, bus);

    graphics::LocalBoardInput boardInput(
        controller, engine,
        [&](const GameEvent& event) {
            GameEvent stamped = event;
            stamped.timeMs = engine.elapsedMs();
            bus.publish(stamped);
        },
        [&](const Position& at) -> std::string {
            const GameSnapshot snap = engine.snapshot();
            if (at.row < 0 || at.col < 0 ||
                at.row >= static_cast<int>(snap.cells.size()) ||
                at.col >=
                    static_cast<int>(snap.cells[static_cast<size_t>(at.row)].size())) {
                return ".";
            }
            return snap.cells[static_cast<size_t>(at.row)]
                             [static_cast<size_t>(at.col)];
        });

    GraphicsApplication app(layout, std::move(background), frameSource, matchmaker,
                            boardInput, sideEffects, &bus);
    return app.run();
}

int runOnline(const graphics::BoardLayout& layout, view::Img background,
              const CliOptions& opts) {
    client::WebSocketClient client;
    const client::BootstrapResult auth =
        client::SessionBootstrap{}.run(client, opts.host, opts.port, std::cin,
                                       std::cout);
    if (!auth.ok) {
        std::cerr << "AUTH failed: " << auth.error << '\n';
        return 1;
    }

    client::RemoteStateSource frameSource(client.inboundQueue());
    frameSource.setIdentity(auth.username, auth.rating);
    client::WsMatchmaker matchmaker(client, frameSource);
    graphics::NoOpBoardInput boardInput;
    graphics::NoOpFrameSideEffects sideEffects;

    // OpenCV window is created lazily on first showFrame inside run().
    GraphicsApplication app(layout, std::move(background), frameSource, matchmaker,
                            boardInput, sideEffects, nullptr);
    return app.run();
}

}  // namespace

int main(int argc, char** argv) {
    try {
        const CliOptions opts = parseArgs(argc, argv);

        const graphics::BoardLayout layout =
            graphics::BoardLayoutLoader{}.load(graphics::AssetPaths::boardCsv());

        view::Img background;
        background.read(graphics::AssetPaths::boardImage());

        if (opts.offline) {
            return runOffline(layout, std::move(background));
        }
        return runOnline(layout, std::move(background), opts);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        view::Img::destroyWindows();
        return 1;
    }
}
