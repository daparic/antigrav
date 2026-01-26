#include "bitboard.h"
#include "attacks.h"
#include "movegen.h"

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

#ifndef BITBOARD_LIB
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
    
    init_leapers_attacks();
    
    std::cout << "White Pawns:";
    print_bitboard(white_pawns);

    // Pop the e2 pawn (e4 opening representation start)
    pop_bit(white_pawns, e2);
    std::cout << "After moving e2 pawn:";
    set_bit(white_pawns, e4);
    print_bitboard(white_pawns);

    std::cout << "Pawn attacks for e4 (white side):";
    print_bitboard(pawn_attacks[0][e4]);
    
    std::cout << "Knight attacks for g1:";
    print_bitboard(knight_attacks[g1]);

    std::cout << "King attacks for e1:";
    print_bitboard(king_attacks[e1]);

    // Slider attack verification
    U64 occupancy = 0ULL;
    set_bit(occupancy, d7);
    set_bit(occupancy, b4);
    set_bit(occupancy, g4);
    set_bit(occupancy, d2);
    
    std::cout << "Occupancy for Rook at d4:";
    print_bitboard(occupancy);

    std::cout << "Rook attacks for d4 (with blockers):";
    print_bitboard(get_rook_attacks(d4, occupancy));

    occupancy = 0ULL;
    set_bit(occupancy, b6); // UL path blocker
    set_bit(occupancy, g7); // UR path blocker
    set_bit(occupancy, f2); // DR path blocker
    // no blocker DL
    
    std::cout << "Occupancy for Bishop at d4:";
    print_bitboard(occupancy);
    
    std::cout << "Bishop attacks for d4 (with blockers):";
    print_bitboard(get_bishop_attacks(d4, occupancy));

    // Move Generation Verification
    std::cout << "\n--- Move Generation Test ---\n";
    
    Board board;
    // Clear board
    for (int i = 0; i < 12; i++) board.bitboards[i] = 0ULL;
    for (int i = 0; i < 3; i++) board.occupancies[i] = 0ULL;
    
    // Setup position
    // White King at e1, White Pawn at e2
    set_bit(board.bitboards[K], e1);
    set_bit(board.bitboards[P], e2);
    
    // Black King at e8, Black Pawn at d3 (capture target)
    set_bit(board.bitboards[k], e8);
    set_bit(board.bitboards[p], d3);
    
    board.side = 0; // White to move
    board.enpassant = no_sq;
    board.castle = 0;
    
    // Update occupancies
    board.occupancies[0] = board.bitboards[P] | board.bitboards[K];
    board.occupancies[1] = board.bitboards[p] | board.bitboards[k];
    board.occupancies[2] = board.occupancies[0] | board.occupancies[1];
    
    std::cout << "Board Position:";
    print_bitboard(board.occupancies[2]);
    
    Moves moves;
    generate_moves(board, moves);
    
    print_move_list(moves);
    
    return 0;
}
#endif
