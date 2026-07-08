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

set COMMON_FLAGS=-std=c++17 -Iinclude -Itests -Wall -Wextra -Wpedantic
set APP_SOURCES=src\main.cpp src\Board.cpp src\Game.cpp src\MoveRules.cpp
set TEST_SOURCES=tests\test_main.cpp tests\BoardTest.cpp tests\MoveRulesTest.cpp tests\GameTest.cpp src\Board.cpp src\Game.cpp src\MoveRules.cpp

if /i "%~1"=="test" (
    "%GPP%" %COMMON_FLAGS% %TEST_SOURCES% -o build\KungFuChessTests.exe
    if errorlevel 1 exit /b 1
    echo Built: build\KungFuChessTests.exe
    echo.
    build\KungFuChessTests.exe
    exit /b %errorlevel%
)

"%GPP%" %COMMON_FLAGS% %APP_SOURCES% -o build\KungFuChess.exe
if errorlevel 1 exit /b 1
echo Built: build\KungFuChess.exe

if /i "%~1"=="run" (
    echo.
    build\KungFuChess.exe
)
