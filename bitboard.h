#ifndef BITBOARD_H
#define BITBOARD_H

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

// Bitboard data type
typedef uint64_t U64;

// Chess board definitions
enum {
  a8,
  b8,
  c8,
  d8,
  e8,
  f8,
  g8,
  h8,
  a7,
  b7,
  c7,
  d7,
  e7,
  f7,
  g7,
  h7,
  a6,
  b6,
  c6,
  d6,
  e6,
  f6,
  g6,
  h6,
  a5,
  b5,
  c5,
  d5,
  e5,
  f5,
  g5,
  h5,
  a4,
  b4,
  c4,
  d4,
  e4,
  f4,
  g4,
  h4,
  a3,
  b3,
  c3,
  d3,
  e3,
  f3,
  g3,
  h3,
  a2,
  b2,
  c2,
  d2,
  e2,
  f2,
  g2,
  h2,
  a1,
  b1,
  c1,
  d1,
  e1,
  f1,
  g1,
  h1,
  no_sq
};

enum {
  P,
  N,
  B,
  R,
  Q,
  K, // White pieces
  p,
  n,
  b,
  r,
  q,
  k // Black pieces
};

// MSVC support
#ifdef _MSC_VER
#include <intrin.h>
#pragma intrinsic(_BitScanForward64)
#endif

// Portable count trailing zeros
inline int count_trailing_zeros(U64 bitboard) {
#ifdef _MSC_VER
  unsigned long index;
  if (_BitScanForward64(&index, bitboard)) {
    return index;
  }
  return 64; // Or appropriate undefined behavior handling if 0
#else
  // GCC/Clang built-in
  if (bitboard) {
    return __builtin_ctzll(bitboard);
  }
  return 64;
#endif
}

// Bit manipulation macros/functions
#define get_bit(bitboard, square) ((bitboard) & (1ULL << (square)))
#define set_bit(bitboard, square) ((bitboard) |= (1ULL << (square)))
#define pop_bit(bitboard, square) ((bitboard) &= ~(1ULL << (square)))

// Helper function to print bitboard
void print_bitboard(U64 bitboard);

#endif
