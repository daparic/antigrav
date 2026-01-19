#include <iostream>
#include <vector>
#include <string>
#include <cstdint>

// Bitboard data type
typedef uint64_t U64;

// Chess board definitions
enum {
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1,
    no_sq
};

enum {
    P, N, B, R, Q, K, // White pieces
    p, n, b, r, q, k  // Black pieces
};

// Bit manipulation macros/functions
#define get_bit(bitboard, square) ((bitboard) & (1ULL << (square)))
#define set_bit(bitboard, square) ((bitboard) |= (1ULL << (square)))
#define pop_bit(bitboard, square) ((bitboard) &= ~(1ULL << (square)))

// Helper function to print bitboard
void print_bitboard(U64 bitboard) {
    std::cout << "\n";
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            if (!file) std::cout << "  " << 8 - rank << " ";
            std::cout << " " << (get_bit(bitboard, square) ? 1 : 0);
        }
        std::cout << "\n";
    }
    std::cout << "\n     a b c d e f g h\n\n";
    std::cout << "     Bitboard: " << bitboard << "\n\n";
}

int main() {
    // Example: Initialize bitboards for white pawns
    U64 white_pawns = 0ULL;
    
    // Set white pawns on rank 2
    // Squares a2 (48) to h2 (55)
    set_bit(white_pawns, a2);
    set_bit(white_pawns, b2);
    set_bit(white_pawns, c2);
    set_bit(white_pawns, d2);
    set_bit(white_pawns, e2);
    set_bit(white_pawns, f2);
    set_bit(white_pawns, g2);
    set_bit(white_pawns, h2);
    
    std::cout << "White Pawns:";
    print_bitboard(white_pawns);

    // Pop the e2 pawn (e4 opening representation start)
    pop_bit(white_pawns, e2);
    std::cout << "After moving e2 pawn:";
    print_bitboard(white_pawns);

    return 0;
}
