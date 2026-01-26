cmake_minimum_required(VERSION 3.10)
project(ChessBitboard)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(bitboard bitboard.cpp attacks.cpp movegen.cpp)
add_executable(perft perft.cpp movegen.cpp attacks.cpp bitboard.cpp)
target_compile_definitions(perft PRIVATE BITBOARD_LIB)
