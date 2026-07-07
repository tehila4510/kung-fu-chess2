# Kung Fu Chess — GitHub Copilot Instructions

## Project overview

Console C++17 game. Reads a board definition and command list from stdin, simulates clicks and timing.

## Structure

```
include/   → Board.h, Game.h
src/       → Board.cpp, Game.cpp, main.cpp
build.bat  → g++ build (Windows)
```

## Build

```powershell
.\build.bat
.\build.bat run
```

Add new `.cpp` files to both `build.bat` and `CMakeLists.txt`.

## Coding conventions

- C++17, STL only, `-Wall -Wextra -Wpedantic`
- Header guards (`#ifndef GAME_H`)
- PascalCase classes, camelCase methods
- `Board` owns grid + parsing; `Game` owns selection + move logic
- Piece format: `"."` or two-char `"wK"` / `"bR"` (white/black + KQRBNP)

## Input protocol

```
Board:
. wK . ...
Commands:
click 150 250
wait 1000
print board
```

- `click X Y` — pixel coordinates; converted with `CELL_SIZE = 100`
- Invalid tokens → `ERROR UNKNOWN_TOKEN`; uneven rows → `ERROR ROW_WIDTH_MISMATCH`

## Suggestions should

- Match existing patterns in `Board.cpp` and `Game.cpp`
- Stay minimal — no drive-by refactors
- Use `const` and references appropriately
- Print errors to `std::cout`, not `stderr`

## Suggestions should not

- Introduce external libraries or frameworks
- Add GUI, networking, or threading unless requested
- Rename public methods or change the stdin command format without being asked
