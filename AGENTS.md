# Kung Fu Chess — Agent Instructions

C++17 console game that reads board setup and commands from stdin.

## Layout

- `include/` — headers (`Board.h`, `Game.h`)
- `src/` — implementations (`Board.cpp`, `Game.cpp`, `main.cpp`)
- `build.bat` — primary Windows build script
- `CMakeLists.txt` — optional CMake build

## Build & verify

```powershell
.\build.bat
.\build.bat run
```

Compiler flags: `-std=c++17 -Iinclude -Wall -Wextra -Wpedantic`

## Architecture

| Class | Role |
|-------|------|
| `Board` | Grid storage, parsing, bounds, piece tokens, printing |
| `Game` | Selection state, click/move logic, game clock |
| `main` | Read stdin, dispatch commands |

## Input format

1. Lines until `Board:` — board rows (tokens like `.`, `wK`, `bQ`)
2. `Commands:` section — `click X Y`, `wait MS`, `print board`

Errors go to stdout (e.g. `ERROR UNKNOWN_TOKEN`, `ERROR ROW_WIDTH_MISMATCH`).

## When changing code

- Keep changes minimal and focused on the requested task
- Match existing naming: PascalCase classes, camelCase methods
- Do not add external libraries — use STL only
- Do not commit `build/` artifacts unless explicitly asked
- Update `build.bat` and `CMakeLists.txt` together if source files change
