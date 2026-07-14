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

rem Legacy Board/Game/MoveRules sources have been removed. The engine layer
rem (model/rules/realtime/engine/io/input) backs both the app and the tests.
set ENGINE_SOURCES=src\model\Board.cpp src\model\GameState.cpp src\model\Piece.cpp src\model\Position.cpp src\rules\PieceRules.cpp src\rules\RuleEngine.cpp src\realtime\RealTimeArbiter.cpp src\engine\GameEngine.cpp src\io\BoardParser.cpp src\io\BoardPrinter.cpp src\input\BoardMapper.cpp src\input\Controller.cpp src\texttests\ScriptParser.cpp src\texttests\ScriptRunner.cpp
set ENGINE_TEST_SOURCES=tests\test_main.cpp tests\RuleEngineTest.cpp tests\RealTimeArbiterTest.cpp tests\GameEngineTest.cpp tests\BoardMapperTest.cpp tests\ControllerTest.cpp tests\BoardIOTest.cpp %ENGINE_SOURCES%

if /i "%~1"=="test" (
    "%GPP%" %COMMON_FLAGS% %ENGINE_TEST_SOURCES% -o build\KungFuChessEngineTests.exe
    if errorlevel 1 exit /b 1
    echo Built: build\KungFuChessEngineTests.exe
    echo.
    build\KungFuChessEngineTests.exe
    exit /b %errorlevel%
)

rem Default target: build and run the app (composition root in src\main.cpp).
"%GPP%" %COMMON_FLAGS% src\main.cpp %ENGINE_SOURCES% -o build\KungFuChess.exe
if errorlevel 1 exit /b 1
echo Built: build\KungFuChess.exe
echo.
build\KungFuChess.exe
exit /b %errorlevel%
