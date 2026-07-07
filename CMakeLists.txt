cmake_minimum_required(VERSION 3.10)
project(KungFuChess)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include_directories(${PROJECT_SOURCE_DIR}/include)

set(CORE_SOURCES
    src/Board.cpp
    src/Game.cpp
    src/MoveRules.cpp
)

add_executable(${PROJECT_NAME} src/main.cpp ${CORE_SOURCES})

add_executable(KungFuChessTests
    tests/test_main.cpp
    tests/BoardTest.cpp
    tests/MoveRulesTest.cpp
    tests/GameTest.cpp
    ${CORE_SOURCES}
)

enable_testing()
add_test(NAME KungFuChessTests COMMAND KungFuChessTests)
