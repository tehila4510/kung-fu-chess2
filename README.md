# KUNG FU CHESS - C++ Game

פרויקט C++ למשחק KUNG FU CHESS.

## Requirements

- C++17 compiler (`g++` from MSYS2/MinGW, or MSVC)

## Build & Run

```powershell
.\build.bat
.\build\KungFuChess.exe
```

Build and run in one command:

```powershell
.\build.bat run
```

With input from a file:

```powershell
Get-Content input.txt | .\build\KungFuChess.exe
```

## Manual build (without script)

```powershell
C:\msys64\mingw64\bin\g++.exe -std=c++17 -Iinclude src\main.cpp src\Board.cpp src\Game.cpp -o build\KungFuChess.exe
```

## Project Structure

```
KungFuChess/
├── src/
├── include/
├── build.bat
└── README.md
```
