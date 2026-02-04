cmake_minimum_required(VERSION 3.10)
project(ChessBitboard)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Asset Bundling
find_package(Python3 REQUIRED)
set(ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
set(GEN_ASSETS_H "${CMAKE_CURRENT_BINARY_DIR}/assets.h")

add_custom_command(
    OUTPUT ${GEN_ASSETS_H}
    COMMAND Python3::Interpreter "${CMAKE_CURRENT_SOURCE_DIR}/scripts/bundle_assets.py" "${ASSETS_DIR}" "${GEN_ASSETS_H}"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/scripts/bundle_assets.py"
    COMMENT "Bundling assets into ${GEN_ASSETS_H}"
)

# Shared sources (all except the one with main)
set(COMMON_SOURCES attacks.cpp movegen.cpp ${GEN_ASSETS_H})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(bitboard bitboard.cpp ${COMMON_SOURCES})
add_executable(perft perft.cpp bitboard.cpp ${COMMON_SOURCES})
target_compile_definitions(perft PRIVATE BITBOARD_LIB)

add_executable(test_fen test_fen.cpp bitboard.cpp ${COMMON_SOURCES})
target_compile_definitions(test_fen PRIVATE BITBOARD_LIB)

add_executable(mate2 mate2.cpp bitboard.cpp ${COMMON_SOURCES})
target_compile_definitions(mate2 PRIVATE BITBOARD_LIB)

add_executable(test_leaper test_leaper.cpp bitboard.cpp ${COMMON_SOURCES})
target_compile_definitions(test_leaper PRIVATE BITBOARD_LIB)

add_executable(test_movegen test_movegen.cpp bitboard.cpp ${COMMON_SOURCES})
target_compile_definitions(test_movegen PRIVATE BITBOARD_LIB)