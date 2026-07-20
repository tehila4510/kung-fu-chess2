# Kung Fu Chess

C++17 real-time chess game with a console (scripted stdin) mode and an interactive graphics window.

## Requirements

- **Console / engine tests:** C++17 compiler (`g++` from MSYS2/MinGW, or MSVC)
- **Graphics mode / graphics tests:** MSVC + bundled `OpenCV_451` in the project root (course starter pack)

## Build & Run

```powershell
.\build.bat              # build and run the console app
.\build.bat test         # engine + STL graphics helper tests
.\build.bat graphics     # interactive board window (requires MSVC + OpenCV_451)
.\build.bat graphics-test  # OpenCV graphics unit tests
.\build.bat server       # WebSocket game server on port 9002 (see deps below)
```

Console app with input from a file:

```powershell
Get-Content input.txt | .\build\KungFuChess.exe
```

## Input protocol (console)

1. Board rows until a `Board:` marker (tokens: `.`, `wK`, `bQ`, …)
2. `Commands:` section — `click X Y`, `jump X Y`, `wait MS`, `print board`

Errors are printed to stdout (e.g. `ERROR UNKNOWN_TOKEN`, `ERROR ROW_WIDTH_MISMATCH`).

## Server dependencies

Header-only libs under `third_party/` (not committed). Clone once from the project root:

```powershell
mkdir third_party -Force
git clone --depth 1 https://github.com/zaphoyd/websocketpp.git third_party/websocketpp
git clone --depth 1 --branch asio-1-28-0 https://github.com/chriskohlhoff/asio.git third_party/asio
git clone --depth 1 https://github.com/nlohmann/json.git third_party/json
```

> Pin **Asio 1.28.0** (`asio-1-28-0`). Newer Asio removed APIs that websocketpp still calls (`expires_from_now`). No Boost required.

Expected include roots used by the build:

| Dependency | Include path |
|------------|----------------|
| websocketpp | `third_party/websocketpp` |
| Asio standalone 1.28 (no Boost) | `third_party/asio/asio/include` |
| nlohmann/json | `third_party/json/single_include` |

Then:

```powershell
.\build.bat server
```

Listens on **port 9002**. First client = White, second = Black; further connections are rejected (spectator/Room support TODO later).

**Wire protocol (hybrid):**

- Client → server (plain text): `WMe2e4`, `BJe7`, `WAIT 100`, `STATE`
- Server → client (JSON): board snapshot, motions, rests, Bus event payloads

## Project structure

```
include/
├── model/       Board, Piece, Position, GameState
├── rules/       IPieceRules strategies, RuleEngine
├── realtime/    Motion, Rest, RealTimeArbiter
├── engine/      GameEngine
├── io/          BoardParser, BoardPrinter
├── input/       Controller, BoardMapper
├── graphics/    Animation, PieceVisual, BoardLayout, …
├── view/        Img (OpenCV wrapper), Renderer
├── texttests/   ScriptParser, ScriptRunner
├── bus/         EventBus (Observer), GameEvent, subscribers
├── protocol/    Algebraic, CommandParser, StateSerializer (JSON)
├── server/      GameSession, WebSocketServer
├── App.h
└── GraphicsApplication.h

src/             mirrors include/ + main.cpp + graphics_main.cpp + server_main.cpp
assets/          board.png (background), piece sprites, configs, board.csv
third_party/     websocketpp, asio, nlohmann/json (cloned locally)
tests/           unit tests
build.bat        primary Windows build script
CMakeLists.txt   optional CMake build
```

## Assets

Game art lives under `assets/`:

- `assets/board.png` — board background
- `assets/pieces/` — per-piece state sprites (`idle`, `move`, `jump`, `long_rest`, `short_rest`), `config.json`, and `board.csv`

These files are tracked in git. OpenCV itself (`OpenCV_451/`) is **not** committed — download it separately from the course starter.

## Architecture

Dependencies point inward only:

```
main/App → input/view/io → engine → rules/realtime → model
```

Graphics stack:

```
graphics_main → GraphicsApplication → engine/input + graphics + view(Renderer/Img)
```

Server stack (reuses the same `GameEngine`; does not fork game logic):

```
server_main → WebSocketServer → protocol + GameSession → GameEngine
                              → EventBus (Observer) → MoveLog / Sound / …
```

OpenCV is used **only** inside `view/Img`. All other graphics code goes through the `Img` API.

## Graphics controls

- Left click: select / move (via `Controller`)
- Jump: as wired in the graphics app
- ESC / Q or close the window: quit

After a move, pieces take a long rest; after a jump, a short rest (visual overlays + rest animations).
