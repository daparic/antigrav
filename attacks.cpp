#include "attacks.h"

// Attack tables
U64 pawn_attacks[2][64];
U64 knight_attacks[64];
U64 king_attacks[64];

// Masks for file wrapping (0=a8 ... 63=h1)
const U64 not_a_file = 0xfefefefefefefefe;
const U64 not_h_file = 0x7f7f7f7f7f7f7f7f;
const U64 not_hg_file = 0x3f3f3f3f3f3f3f3f;
const U64 not_ab_file = 0xfcfcfcfcfcfcfcfc;

// Mask pawn attacks
U64 mask_pawn_attacks(int side, int square) {
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;
    
    set_bit(bitboard, square);
    
    // White pawns (Move UP, indices decrease)
    if (!side) {
        // Capture Right (-7: Up 1, Right 1) -> Avoid A file wrap
        if ((bitboard >> 7) & not_a_file) attacks |= (bitboard >> 7);
        // Capture Left (-9: Up 1, Left 1) -> Avoid H file wrap
        if ((bitboard >> 9) & not_h_file) attacks |= (bitboard >> 9);
    }
    // Black pawns (Move DOWN, indices increase)
    else {
        // Capture Left (+7: Down 1, Left 1) -> Avoid H file wrap
        if ((bitboard << 7) & not_h_file) attacks |= (bitboard << 7);
        // Capture Right (+9: Down 1, Right 1) -> Avoid A file wrap
        if ((bitboard << 9) & not_a_file) attacks |= (bitboard << 9);
    }
    
    return attacks;
}

// Mask knight attacks
U64 mask_knight_attacks(int square) {
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;
    
    set_bit(bitboard, square);
    
    // >> 17: Up 2 Left 1 (-17). Avoid H wrap.
    if ((bitboard >> 17) & not_h_file) attacks |= (bitboard >> 17);
    // >> 15: Up 2 Right 1 (-15). Avoid A wrap.
    if ((bitboard >> 15) & not_a_file) attacks |= (bitboard >> 15);
    // >> 10: Up 1 Left 2 (-10). Avoid GH wrap.
    if ((bitboard >> 10) & not_hg_file) attacks |= (bitboard >> 10);
    // >> 6: Up 1 Right 2 (-6). Avoid AB wrap.
    if ((bitboard >> 6) & not_ab_file) attacks |= (bitboard >> 6);
    
    // << 17: Down 2 Right 1 (+17). Avoid A wrap.
    if ((bitboard << 17) & not_a_file) attacks |= (bitboard << 17);
    // << 15: Down 2 Left 1 (+15). Avoid H wrap.
    if ((bitboard << 15) & not_h_file) attacks |= (bitboard << 15);
    // << 10: Down 1 Right 2 (+10). Avoid AB wrap.
    if ((bitboard << 10) & not_ab_file) attacks |= (bitboard << 10);
    // << 6: Down 1 Left 2 (+6). Avoid GH wrap.
    if ((bitboard << 6) & not_hg_file) attacks |= (bitboard << 6);
    
    return attacks;
}

// Mask king attacks
U64 mask_king_attacks(int square) {
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;
    
    set_bit(bitboard, square);
    
    // North/South
    if (bitboard >> 8) attacks |= (bitboard >> 8);
    if (bitboard << 8) attacks |= (bitboard << 8);
    
    // East/West (Right/Left)
    if ((bitboard >> 1) & not_h_file) attacks |= (bitboard >> 1); // Left
    if ((bitboard << 1) & not_a_file) attacks |= (bitboard << 1); // Right
    
    // Diagonals
    if ((bitboard >> 9) & not_h_file) attacks |= (bitboard >> 9); // UL
    if ((bitboard >> 7) & not_a_file) attacks |= (bitboard >> 7); // UR
    if ((bitboard << 9) & not_a_file) attacks |= (bitboard << 9); // DR
    if ((bitboard << 7) & not_h_file) attacks |= (bitboard << 7); // DL
    
    return attacks;
}

void init_leapers_attacks() {
    for (int square = 0; square < 64; square++) {
        pawn_attacks[0][square] = mask_pawn_attacks(0, square);
        pawn_attacks[1][square] = mask_pawn_attacks(1, square);
        
        knight_attacks[square] = mask_knight_attacks(square);
        king_attacks[square] = mask_king_attacks(square);
    }
}

// Get bishop attacks
U64 get_bishop_attacks(int square, U64 occupancy) {
    U64 attacks = 0ULL;
    int r, f;
    int tr = square / 8;
    int tf = square % 8;
    
    // South East
    for (r = tr + 1, f = tf + 1; r < 8 && f < 8; r++, f++) {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & occupancy) break;
    }
    // North East
    for (r = tr - 1, f = tf + 1; r >= 0 && f < 8; r--, f++) {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & occupancy) break;
    }
    // South West
    for (r = tr + 1, f = tf - 1; r < 8 && f >= 0; r++, f--) {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & occupancy) break;
    }
    // North West
    for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--) {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & occupancy) break;
    }
    
    return attacks;
}

// Get rook attacks
U64 get_rook_attacks(int square, U64 occupancy) {
    U64 attacks = 0ULL;
    int r, f;
    int tr = square / 8;
    int tf = square % 8;
    
    // South
    for (r = tr + 1; r < 8; r++) {
        attacks |= (1ULL << (r * 8 + tf));
        if ((1ULL << (r * 8 + tf)) & occupancy) break;
    }
    // North
    for (r = tr - 1; r >= 0; r--) {
        attacks |= (1ULL << (r * 8 + tf));
        if ((1ULL << (r * 8 + tf)) & occupancy) break;
    }
    // East
    for (f = tf + 1; f < 8; f++) {
        attacks |= (1ULL << (tr * 8 + f));
        if ((1ULL << (tr * 8 + f)) & occupancy) break;
    }
    // West
    for (f = tf - 1; f >= 0; f--) {
        attacks |= (1ULL << (tr * 8 + f));
        if ((1ULL << (tr * 8 + f)) & occupancy) break;
    }
    
    return attacks;
}

// Get queen attacks
U64 get_queen_attacks(int square, U64 occupancy) {
    return get_bishop_attacks(square, occupancy) | get_rook_attacks(square, occupancy);
}

// Check if square is attacked
int is_square_attacked(int square, int side, U64 bitboards[], U64 occupancies[]) {
    // Attacked by white (0) or black (1) pieces?
    
    // 1. Attacked by pawns
    if (!side) { // Attacked by White Pawns
        if (pawn_attacks[1][square] & bitboards[P]) return 1;
        // Logic: if I am a black piece at 'square', attacked by white pawn?
        // White pawn attacks FROM (square - 7) or (square - 9).
        // My table: pawn_attacks[1][square] simulates "Black pawn at square".
        // Black pawn captures downwards.
        // If a black pawn at 'square' attacks X, then a white pawn at X attacks 'square'.
        // Hence: check if "reverse capture" hits a white pawn.
    } else { // Attacked by Black Pawns
        if (pawn_attacks[0][square] & bitboards[p]) return 1;
    }
    
    // 2. Attacked by Knights
    // Knight attacks are symmetric. If Knight at X attacks 'square', Knight at 'square' attacks X.
    if (knight_attacks[square] & ((!side) ? bitboards[N] : bitboards[n])) return 1;
    
    // 3. Attacked by Kings
    if (king_attacks[square] & ((!side) ? bitboards[K] : bitboards[k])) return 1;
    
    // 4. Attacked by Bishops/Queens (Diagonal)
    U64 bishop_queen = (!side) ? (bitboards[B] | bitboards[Q]) : (bitboards[b] | bitboards[q]);
    if (get_bishop_attacks(square, occupancies[2]) & bishop_queen) return 1;
    
    // 5. Attacked by Rooks/Queens (Orthogonal)
    U64 rook_queen = (!side) ? (bitboards[R] | bitboards[Q]) : (bitboards[r] | bitboards[q]);
    if (get_rook_attacks(square, occupancies[2]) & rook_queen) return 1;
    
    return 0;
}
