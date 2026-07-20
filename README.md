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

Libs under `third_party/` (not committed). Clone / download once from the project root:

```powershell
mkdir third_party -Force
git clone --depth 1 https://github.com/zaphoyd/websocketpp.git third_party/websocketpp
git clone --depth 1 --branch asio-1-28-0 https://github.com/chriskohlhoff/asio.git third_party/asio
git clone --depth 1 https://github.com/nlohmann/json.git third_party/json

# SQLite amalgamation (official C source — single .c/.h, no external deps)
Invoke-WebRequest -Uri "https://www.sqlite.org/2024/sqlite-amalgamation-3460100.zip" -OutFile sqlite-amalgamation.zip
Expand-Archive sqlite-amalgamation.zip -DestinationPath third_party/_sqlite_extract -Force
New-Item -ItemType Directory -Force -Path third_party/sqlite | Out-Null
Copy-Item third_party/_sqlite_extract/sqlite-amalgamation-3460100/sqlite3.c third_party/sqlite/
Copy-Item third_party/_sqlite_extract/sqlite-amalgamation-3460100/sqlite3.h third_party/sqlite/
Remove-Item -Recurse -Force third_party/_sqlite_extract, sqlite-amalgamation.zip

# Portable bcrypt (Openwall crypt_blowfish + wrapper; https://github.com/trusch/libbcrypt)
git clone --depth 1 https://github.com/trusch/libbcrypt.git third_party/_libbcrypt
New-Item -ItemType Directory -Force -Path third_party/bcrypt/include/bcrypt, third_party/bcrypt/src | Out-Null
Copy-Item third_party/_libbcrypt/LICENSE third_party/bcrypt/
Copy-Item third_party/_libbcrypt/include/bcrypt/bcrypt.h,
         third_party/_libbcrypt/include/bcrypt/crypt.h,
         third_party/_libbcrypt/include/bcrypt/crypt_blowfish.h,
         third_party/_libbcrypt/include/bcrypt/crypt_gensalt.h,
         third_party/_libbcrypt/include/bcrypt/ow-crypt.h third_party/bcrypt/include/bcrypt/
Copy-Item third_party/_libbcrypt/src/bcrypt.c,
         third_party/_libbcrypt/src/crypt_blowfish.c,
         third_party/_libbcrypt/src/crypt_gensalt.c,
         third_party/_libbcrypt/src/wrapper.c third_party/bcrypt/src/
# MinGW/Windows: declare crypt_rn / crypt_gensalt_rn (upstream only includes ow-crypt.h on non-Windows)
(Get-Content third_party/bcrypt/src/bcrypt.c -Raw) -replace `
  '(#include "../include/bcrypt/bcrypt.h"\r?\n)\r?\n#include <windows.h>', `
  "`$1#include `"../include/bcrypt/ow-crypt.h`"`n`n#include <windows.h>" |
  Set-Content third_party/bcrypt/src/bcrypt.c -NoNewline
Remove-Item -Recurse -Force third_party/_libbcrypt
```

> Pin **Asio 1.28.0** (`asio-1-28-0`). Newer Asio removed APIs that websocketpp still calls (`expires_from_now`). No Boost required.

Expected include / source roots used by the build:

| Dependency | Path |
|------------|------|
| websocketpp | `third_party/websocketpp` |
| Asio standalone 1.28 (no Boost) | `third_party/asio/asio/include` |
| nlohmann/json | `third_party/json/single_include` |
| SQLite amalgamation | `third_party/sqlite/sqlite3.c` + `sqlite3.h` |
| bcrypt (vendored C) | `third_party/bcrypt/src/*.c` + `include/bcrypt/` |

Then:

```powershell
.\build.bat server
```

Listens on **port 9002**. Flow: connect → `AUTH <username> <password>` (register-or-login, bcrypt + SQLite) → seat White then Black → play. Further connections are rejected (spectator/Room support TODO later). Users and ratings live in `data/users.db` (rating starts at 1200; ELO K=32 on `GameEnded`).

**Wire protocol (hybrid):**

- Client → server (plain text): `AUTH user pass`, then `WMe2e4`, `BJe7`, `WAIT 100`, `STATE`
- Server → client (JSON): `auth_required` / `auth_ok`, board snapshot, motions, rests, Bus event payloads

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
third_party/     websocketpp, asio, nlohmann/json, sqlite, bcrypt (cloned/downloaded locally)
tests/           unit tests
build.bat        primary Windows build script
CMakeLists.txt   optional CMake build
```

## Assets

Game art lives under `assets/`:

- `assets/board.png` — board background
- `assets/pieces/` — per-piece state sprites (`idle`, `move`, `jump`, `long_rest`, `short_rest`), `config.json`, and `board.csv`
- `assets/sounds/` — WAV cues for select / deselect / move / jump / capture / promote / game_end / game_start (see `assets/sounds/README.txt`)

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
When the king is captured, graphics shows a centered **GAME OVER** overlay and plays `game_end`.
