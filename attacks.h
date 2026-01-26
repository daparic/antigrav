#ifndef ATTACKS_H
#define ATTACKS_H

#include "bitboard.h"

// Leaper pieces attack tables [square]
extern U64 pawn_attacks[2][64];
extern U64 knight_attacks[64];
extern U64 king_attacks[64];

// Function to initialize leaper attack tables
void init_leapers_attacks();

// Slider attacks (on-the-fly)
U64 get_bishop_attacks(int square, U64 occupancy);
U64 get_rook_attacks(int square, U64 occupancy);
U64 get_queen_attacks(int square, U64 occupancy);

// Check if square is attacked by a given side
int is_square_attacked(int square, int side, U64 bitboards[], U64 occupancies[]);

#endif
