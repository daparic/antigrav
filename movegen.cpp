#include "movegen.h"
#include <iostream>

// Add move to list (helper)
void add_move(Moves& move_list, int move) {
    move_list.moves[move_list.count] = move;
    move_list.count++;
}

// Print move
void print_move(int move) {
    const char* square_to_coordinates[] = {
        "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
        "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
        "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
        "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
        "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
        "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
        "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
        "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"
    };
    
    char promoted_pieces[] = { 'p', 'n', 'b', 'r', 'q', 'k', 'p', 'n', 'b', 'r', 'q', 'k' };
    
    std::cout << square_to_coordinates[get_move_source(move)] <<
                 square_to_coordinates[get_move_target(move)];
                 
    if (get_move_promoted(move)) {
        int p = get_move_promoted(move);
        std::cout << promoted_pieces[p];
    }
}

void print_move_list(const Moves& moves) {
    std::cout << "Moves (" << moves.count << "): ";
    for (int i = 0; i < moves.count; i++) {
        print_move(moves.moves[i]);
        std::cout << " ";
    }
    std::cout << "\n";
}

void generate_moves(const Board& board, Moves& moves) {
    moves.count = 0;
    int source_square, target;
    U64 bitboard, attacks;
    
    // Loop over all pieces
    for (int piece = P; piece <= k; piece++) {
        bitboard = board.bitboards[piece];
        
        // Check if piece belongs to current side
        if (board.side == 0) { // White
            if (piece >= p) continue; // Skip black pieces
        } else { // Black
            if (piece < p) continue; // Skip white pieces
        }
        
        while (bitboard) {
            // Get source square (LSB)
            source_square = __builtin_ctzll(bitboard);
            
            // Generate moves for this piece
            // Pawns
            if (piece == P) { // White Pawn
                // 1. Single Push
                target = source_square - 8;
                if (target >= 0 && !get_bit(board.occupancies[2], target)) {
                     // Promotion (Rank 8 is a8-h8? No, Rank 8 is 0-7)
                     if (source_square >= a7 && source_square <= h7) {
                         add_move(moves, encode_move(source_square, target, piece, Q, 0, 0, 0, 0));
                         add_move(moves, encode_move(source_square, target, piece, R, 0, 0, 0, 0));
                         add_move(moves, encode_move(source_square, target, piece, B, 0, 0, 0, 0));
                         add_move(moves, encode_move(source_square, target, piece, N, 0, 0, 0, 0));
                     } else {
                         add_move(moves, encode_move(source_square, target, piece, 0, 0, 0, 0, 0));
                         // 2. Double Push (from Rank 2: 48-55)
                         if ((source_square >= a2 && source_square <= h2) && !get_bit(board.occupancies[2], target - 8)) {
                             add_move(moves, encode_move(source_square, target - 8, piece, 0, 0, 1, 0, 0));
                         }
                     }
                }
                
                // 3. Captures
                attacks = pawn_attacks[0][source_square] & board.occupancies[1]; // White pawns capture black
                while (attacks) {
                    target = __builtin_ctzll(attacks);
                    if (source_square >= a7 && source_square <= h7) {
                         add_move(moves, encode_move(source_square, target, piece, Q, 1, 0, 0, 0));
                         add_move(moves, encode_move(source_square, target, piece, R, 1, 0, 0, 0));
                         add_move(moves, encode_move(source_square, target, piece, B, 1, 0, 0, 0));
                         add_move(moves, encode_move(source_square, target, piece, N, 1, 0, 0, 0));
                    } else {
                        add_move(moves, encode_move(source_square, target, piece, 0, 1, 0, 0, 0));
                    }
                    pop_bit(attacks, target);
                }
                
            }
            
            else if (piece == p) { // Black Pawn
                 // 1. Single Push (+8)
                target = source_square + 8;
                if (target < 64 && !get_bit(board.occupancies[2], target)) {
                     // Promotion (Rank 1: 56-63)
                     if (source_square >= a2 && source_square <= h2) { // 2nd rank from top (7th rank)
                         add_move(moves, encode_move(source_square, target, piece, q, 0, 0, 0, 0));
                         add_move(moves, encode_move(source_square, target, piece, r, 0, 0, 0, 0));
                         add_move(moves, encode_move(source_square, target, piece, b, 0, 0, 0, 0));
                         add_move(moves, encode_move(source_square, target, piece, n, 0, 0, 0, 0));
                     } else {
                         add_move(moves, encode_move(source_square, target, piece, 0, 0, 0, 0, 0));
                         // 2. Double Push (from Rank 7: 8-15)
                         if ((source_square >= a7 && source_square <= h7) && !get_bit(board.occupancies[2], target + 8)) {
                             add_move(moves, encode_move(source_square, target + 8, piece, 0, 0, 1, 0, 0));
                         }
                     }
                }
                
                // 3. Captures
                attacks = pawn_attacks[1][source_square] & board.occupancies[0]; // Black pawns capture white
                while (attacks) {
                    target = __builtin_ctzll(attacks);
                    if (source_square >= a2 && source_square <= h2) {
                         add_move(moves, encode_move(source_square, target, piece, q, 1, 0, 0, 0));
                         add_move(moves, encode_move(source_square, target, piece, r, 1, 0, 0, 0));
                         add_move(moves, encode_move(source_square, target, piece, b, 1, 0, 0, 0));
                         add_move(moves, encode_move(source_square, target, piece, n, 1, 0, 0, 0));
                    } else {
                        add_move(moves, encode_move(source_square, target, piece, 0, 1, 0, 0, 0));
                    }
                    pop_bit(attacks, target);
                }
            }
            
            else { // Other pieces
                if (piece == N || piece == n) attacks = knight_attacks[source_square];
                else if (piece == B || piece == b) attacks = get_bishop_attacks(source_square, board.occupancies[2]);
                else if (piece == R || piece == r) attacks = get_rook_attacks(source_square, board.occupancies[2]);
                else if (piece == Q || piece == q) attacks = get_queen_attacks(source_square, board.occupancies[2]);
                else if (piece == K || piece == k) attacks = king_attacks[source_square];
                
                // Filter attacks: can only move to squares NOT occupied by own side
                attacks &= ~board.occupancies[board.side];
                
                while (attacks) {
                    target = __builtin_ctzll(attacks);
                    
                    // Capture logic
                    int capture = 0;
                    if (get_bit(board.occupancies[1 - board.side], target)) capture = 1;
                    
                    add_move(moves, encode_move(source_square, target, piece, 0, capture, 0, 0, 0));
                    
                    pop_bit(attacks, target);
                }
            }
            
            // Pop the source bit to move to next piece
            pop_bit(bitboard, source_square);
        }
    }
    
    // Castling
    if (board.side == 0) { // White Castling
        // King side (e1 -> g1)
        if ((board.castle & 1) && !get_bit(board.occupancies[2], f1) && !get_bit(board.occupancies[2], g1)) {
            // Check attacks (e1, f1, g1 must not be attacked)
            if (!is_square_attacked(e1, 1, (U64*)board.bitboards, (U64*)board.occupancies) &&
                !is_square_attacked(f1, 1, (U64*)board.bitboards, (U64*)board.occupancies) &&
                !is_square_attacked(g1, 1, (U64*)board.bitboards, (U64*)board.occupancies)) {
                    add_move(moves, encode_move(e1, g1, K, 0, 0, 0, 0, 1));
            }
        }
        // Queen side (e1 -> c1)
        if ((board.castle & 2) && !get_bit(board.occupancies[2], d1) && !get_bit(board.occupancies[2], c1) && !get_bit(board.occupancies[2], b1)) {
            if (!is_square_attacked(e1, 1, (U64*)board.bitboards, (U64*)board.occupancies) &&
                !is_square_attacked(d1, 1, (U64*)board.bitboards, (U64*)board.occupancies) &&
                // c1 check not required by FIDE rules? "squares that the king passes over or lands on".
                // King moves e1->d1->c1. So d1 and c1 must be safe. b1 is irrelevant to King path (rook moves over it)
                !is_square_attacked(c1, 1, (U64*)board.bitboards, (U64*)board.occupancies)) {
                    add_move(moves, encode_move(e1, c1, K, 0, 0, 0, 0, 1));
            }
        }
    }
    else { // Black Castling
        // King side (e8 -> g8)
        if ((board.castle & 4) && !get_bit(board.occupancies[2], f8) && !get_bit(board.occupancies[2], g8)) {
            if (!is_square_attacked(e8, 0, (U64*)board.bitboards, (U64*)board.occupancies) &&
                !is_square_attacked(f8, 0, (U64*)board.bitboards, (U64*)board.occupancies) &&
                !is_square_attacked(g8, 0, (U64*)board.bitboards, (U64*)board.occupancies)) {
                    add_move(moves, encode_move(e8, g8, k, 0, 0, 0, 0, 1));
            }
        }
        // Queen side (e8 -> c8)
        if ((board.castle & 8) && !get_bit(board.occupancies[2], d8) && !get_bit(board.occupancies[2], c8) && !get_bit(board.occupancies[2], b8)) {
            if (!is_square_attacked(e8, 0, (U64*)board.bitboards, (U64*)board.occupancies) &&
                !is_square_attacked(d8, 0, (U64*)board.bitboards, (U64*)board.occupancies) &&
                !is_square_attacked(c8, 0, (U64*)board.bitboards, (U64*)board.occupancies)) {
                    add_move(moves, encode_move(e8, c8, k, 0, 0, 0, 0, 1));
            }
        }
    }
}

// Make move
int make_move(Board& board, int move, int capture_flag) {
    if (capture_flag) {
        // Remove captured piece
        int target = get_move_target(move);
        int start_piece, end_piece;
        
        if (board.side == 0) { // White captures Black
            start_piece = p; end_piece = k;
        } else { // Black captures White
            start_piece = P; end_piece = K;
        }
        
        for (int bb_piece = start_piece; bb_piece <= end_piece; bb_piece++) {
            if (get_bit(board.bitboards[bb_piece], target)) {
                pop_bit(board.bitboards[bb_piece], target);
                break;
            }
        }
    }

    // Move piece
    int piece = get_move_piece(move);
    int source = get_move_source(move);
    int target = get_move_target(move);
    
    // Check bounds
    if (piece < 0 || piece > 11) { std::cout << "Invalid piece " << piece << "\n"; return 0; }
    if (source < 0 || source > 63) { std::cout << "Invalid source " << source << "\n"; return 0; }
    if (target < 0 || target > 63) { std::cout << "Invalid target " << target << "\n"; return 0; }
    
    pop_bit(board.bitboards[piece], source);
    set_bit(board.bitboards[piece], target);
    
    // Promotion
    int promoted = get_move_promoted(move);
    if (promoted) {
        if (promoted < 0 || promoted > 11) { std::cout << "Invalid promoted " << promoted << "\n"; return 0; }
        pop_bit(board.bitboards[piece], target); // Remove Pawn
        set_bit(board.bitboards[promoted], target); // Add Promoted Piece
    }
    
    // En Passant
    if (get_move_enpassant(move)) {
        if (board.side == 0) { // White En Passant
             pop_bit(board.bitboards[p], target + 8);
        } else { // Black En Passant
             pop_bit(board.bitboards[P], target - 8);
        }
    }
    
    // Update En Passant Target
    board.enpassant = no_sq;
    if (get_move_double(move)) {
        if (board.side == 0) board.enpassant = target + 8;
        else board.enpassant = target - 8;
    }
    
    // Castling
    if (get_move_castling(move)) {
        if (target == g1) { // White King Slide
            pop_bit(board.bitboards[R], h1);
            set_bit(board.bitboards[R], f1);
        }
        else if (target == c1) { // White Queen Slide
            pop_bit(board.bitboards[R], a1);
            set_bit(board.bitboards[R], d1);
        }
        else if (target == g8) { // Black King Slide
            pop_bit(board.bitboards[r], h8);
            set_bit(board.bitboards[r], f8);
        }
        else if (target == c8) { // Black Queen Slide
            pop_bit(board.bitboards[r], a8);
            set_bit(board.bitboards[r], d8);
        }
    }
    
    // Update Castling Rights
    // If King moves, loose rights. If Rook moves/captured, loose rights.
    // Simple robust update:
    // Mask based on source/target.
    // 1=WK, 2=WQ, 4=BK, 8=BQ.
    // Maps: e1->~3, e8->~12. a1->~2, h1->~1. a8->~8, h8->~4.
    // Using simple if statements for clarity.
    
    // Only update if rights exist
    if (board.castle) {
        if (source == e1 || target == e1) board.castle &= ~3;
        if (source == e8 || target == e8) board.castle &= ~12;
        
        if (source == h1 || target == h1) board.castle &= ~1;
        if (source == a1 || target == a1) board.castle &= ~2;
        if (source == h8 || target == h8) board.castle &= ~4;
        if (source == a8 || target == a8) board.castle &= ~8;
    }
    
    // Update Occupancies
    board.occupancies[0] = 0ULL;
    board.occupancies[1] = 0ULL;
    for (int i = P; i <= K; i++) board.occupancies[0] |= board.bitboards[i];
    for (int i = p; i <= k; i++) board.occupancies[1] |= board.bitboards[i];
    board.occupancies[2] = board.occupancies[0] | board.occupancies[1];
    
    // Change Side
    board.side ^= 1;
    
    // Check for Legality (King safety)
    int king_sq = -1;
    if (board.side == 1) { // Was White's turn, now Black. Check if White king is attacked
         if (board.bitboards[K]) king_sq = __builtin_ctzll(board.bitboards[K]);
         if (king_sq != -1 && is_square_attacked(king_sq, 1, board.bitboards, board.occupancies)) return 0; // Illegal
    } else { // Was Black's turn, now White. Check if Black king is attacked
         if (board.bitboards[k]) king_sq = __builtin_ctzll(board.bitboards[k]);
         if (king_sq != -1 && is_square_attacked(king_sq, 0, board.bitboards, board.occupancies)) return 0; // Illegal
    }
    
    return 1;
}

// Parse FEN
void parse_fen(char* fen, Board& board) {
    // Clear board
    for (int i = 0; i < 12; i++) board.bitboards[i] = 0ULL;
    for (int i = 0; i < 3; i++) board.occupancies[i] = 0ULL;
    board.side = 0;
    board.enpassant = no_sq;
    board.castle = 0;
    
    int rank = 0;
    int file = 0;
    
    // Parse Pieces
    while (rank < 8 && *fen) {
        int square = rank * 8 + file;
        
        if ((*fen >= 'a' && *fen <= 'z') || (*fen >= 'A' && *fen <= 'Z')) {
            int piece = -1;
            if (*fen == 'P') piece = P;
            else if (*fen == 'N') piece = N;
            else if (*fen == 'B') piece = B;
            else if (*fen == 'R') piece = R;
            else if (*fen == 'Q') piece = Q;
            else if (*fen == 'K') piece = K;
            else if (*fen == 'p') piece = p;
            else if (*fen == 'n') piece = n;
            else if (*fen == 'b') piece = b;
            else if (*fen == 'r') piece = r;
            else if (*fen == 'q') piece = q;
            else if (*fen == 'k') piece = k;
            
            if (piece != -1) set_bit(board.bitboards[piece], square);
            file++;
        }
        else if (*fen >= '0' && *fen <= '9') {
            int offset = *fen - '0';
            file += offset;
        }
        else if (*fen == '/') {
            rank++;
            file = 0;
        }
        
        if (file >= 8) {
            // End of rank
            if (rank == 7) {
                fen++; // Consume last char
                break;
            }
            // Else expect '/' next iteration
        }
        
        fen++;
    }
    
    // Update occupancies
    board.occupancies[0] = 0ULL;
    board.occupancies[1] = 0ULL;
    for (int i = P; i <= K; i++) board.occupancies[0] |= board.bitboards[i];
    for (int i = p; i <= k; i++) board.occupancies[1] |= board.bitboards[i];
    board.occupancies[2] = board.occupancies[0] | board.occupancies[1];
    
    // Skip spaces
    while (*fen == ' ') fen++;
    
    // Parse Side to Move
    if (*fen == 'w') board.side = 0;
    else board.side = 1;
    fen++;
    
    while (*fen == ' ') fen++;
    
    // Parse Castling
    while (*fen != ' ') {
        if (*fen == 'K') board.castle |= 1;
        else if (*fen == 'Q') board.castle |= 2;
        else if (*fen == 'k') board.castle |= 4;
        else if (*fen == 'q') board.castle |= 8;
        else if (*fen == '-') { /* do nothing */ }
        fen++;
    }
    fen++;
    
    // EP parsing skipped
}
