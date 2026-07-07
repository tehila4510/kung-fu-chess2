@echo off
setlocal

set GPP=C:\msys64\mingw64\bin\g++.exe
if not exist "%GPP%" (
    where g++ >nul 2>&1
    if errorlevel 1 (
        echo g++ not found. Install MSYS2/MinGW or add g++ to PATH.
        exit /b 1
    )
    set GPP=g++
)

if not exist build mkdir build

"%GPP%" -std=c++17 -Iinclude -Wall -Wextra -Wpedantic ^
    src\main.cpp src\Board.cpp src\Game.cpp ^
    -o build\KungFuChess.exe

if errorlevel 1 exit /b 1
echo Built: build\KungFuChess.exe

if /i "%~1"=="run" (
    echo.
    build\KungFuChess.exe
)
