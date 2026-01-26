#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "bitboard.h"
#include "attacks.h"

// Move encoding
// 0000 0000 0000 0000 0011 1111    Source Square (0-63)
// 0000 0000 0000 1111 1100 0000    Target Square (0-63)
// 0000 0000 1111 0000 0000 0000    Piece (0-11)
// 0000 1111 0000 0000 0000 0000    Promoted Piece (0-11)
// 0001 0000 0000 0000 0000 0000    Capture Flag
// 0010 0000 0000 0000 0000 0000    Double Push Flag
// 0100 0000 0000 0000 0000 0000    En Passant Flag
// 1000 0000 0000 0000 0000 0000    Castling Flag

#define encode_move(source, target, piece, promoted, capture, double_push, enpassant, castling) \
    ((source) | \
    ((target) << 6) | \
    ((piece) << 12) | \
    ((promoted) << 16) | \
    ((capture) << 20) | \
    ((double_push) << 21) | \
    ((enpassant) << 22) | \
    ((castling) << 23))

#define get_move_source(move) ((move) & 0x3f)
#define get_move_target(move) (((move) & 0xfc0) >> 6)
#define get_move_piece(move) (((move) & 0xf000) >> 12)
#define get_move_promoted(move) (((move) & 0xf0000) >> 16)
#define get_move_capture(move) ((move) & 0x100000)
#define get_move_double(move) ((move) & 0x200000)
#define get_move_enpassant(move) ((move) & 0x400000)
#define get_move_castling(move) ((move) & 0x800000)

// Move List
typedef struct {
    int moves[256];
    int count;
} Moves;

// Board state for move generation (needs to be defined or we use a context)
// For now, we'll pass required state or use global state variables if we had them.
// But cleanly, we should have a Board struct. 
// Given the current codebase uses global bitboards in main, we need to adapt.
// We will assume global bitboards for now, or define a functional interface.

// Let's define the bitboards as externs here if we want to use them, 
// OR pass them in. passing them effectively would be 12 U64s + occupancy + side + ...
// A Board struct is highly recommended.

struct Board {
    U64 bitboards[12];
    U64 occupancies[3]; // [white, black, both]
    int side; // 0=white, 1=black
    int enpassant; // square, or no_sq
    int castle; // bitmask: 1=WK, 2=WQ, 4=BK, 8=BQ (example)
};

// Functions
void generate_moves(const Board& board, Moves& moves);
// Returns 0 if move is illegal (leaves king in check), 1 otherwise
int make_move(Board& board, int move, int capture_flag);
void print_move(int move);
void print_move_list(const Moves& moves);

// FEN parsing (minimal helper for perft tests)
void parse_fen(char* fen, Board& board);

#endif
