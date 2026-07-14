cmake_minimum_required(VERSION 3.10)
project(KungFuChess)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/tests)

# Legacy Board/Game/MoveRules sources have been removed. The engine layer
# (model/rules/realtime/engine/io/input) backs both the app and the tests.
set(ENGINE_SOURCES
    src/model/Board.cpp
    src/model/GameState.cpp
    src/model/Piece.cpp
    src/model/Position.cpp
    src/rules/PieceRules.cpp
    src/rules/RuleEngine.cpp
    src/realtime/RealTimeArbiter.cpp
    src/engine/GameEngine.cpp
    src/io/BoardParser.cpp
    src/io/BoardPrinter.cpp
    src/input/BoardMapper.cpp
    src/input/Controller.cpp
    src/texttests/ScriptParser.cpp
    src/texttests/ScriptRunner.cpp
)

# App executable: composition root in src/main.cpp wires the layers together.
add_executable(KungFuChess
    src/main.cpp
    ${ENGINE_SOURCES}
)

add_executable(KungFuChessEngineTests
    tests/test_main.cpp
    tests/RuleEngineTest.cpp
    tests/RealTimeArbiterTest.cpp
    tests/GameEngineTest.cpp
    tests/BoardMapperTest.cpp
    tests/ControllerTest.cpp
    tests/BoardIOTest.cpp
    ${ENGINE_SOURCES}
)

enable_testing()
add_test(NAME KungFuChessEngineTests COMMAND KungFuChessEngineTests)
