#include "movegen.h"
#include <iostream>
#include <string>
#include <vector>

// Helper to check if a position is Checkmate or Stalemate
// Returns 1 for Checkmate, 0 for Stalemate, -1 for Not Mate (Legal moves exist)
int check_game_state(Board& board) {
    Moves moves;
    generate_moves(board, moves);
    
    int legal_moves = 0;
    for (int i = 0; i < moves.count; i++) {
        Board copy = board;
        if (make_move(copy, moves.moves[i], get_move_capture(moves.moves[i]))) {
            legal_moves++;
            break; // Found at least one legal move
        }
    }
    
    if (legal_moves > 0) return -1; // Not game over
    
    // No legal moves. Check for check.
    // get king square
    int king_sq = -1;
    if (board.side == 0) // White
        king_sq = __builtin_ctzll(board.bitboards[K]);
    else
        king_sq = __builtin_ctzll(board.bitboards[k]);
        
    // check if king is attacked by opponent (side ^ 1)
    if (is_square_attacked(king_sq, board.side ^ 1, board.bitboards, board.occupancies)) {
        return 1; // Checkmate
    } else {
        return 0; // Stalemate
    }
}

bool can_mate_in_1(Board& board, int& winning_move) {
    Moves moves;
    generate_moves(board, moves);
    
    for (int i = 0; i < moves.count; i++) {
        Board copy = board;
        // Try move
        if (make_move(copy, moves.moves[i], get_move_capture(moves.moves[i]))) {
            // Check if result is checkmate
            if (check_game_state(copy) == 1) {
                winning_move = moves.moves[i];
                return true;
            }
        }
    }
    return false;
}

void solve_mate_in_2(char* fen) {
    Board root_board;
    parse_fen(fen, root_board);
    
    init_leapers_attacks(); // Ensure attacks are initialized
    
    std::cout << "Solving for FEN: " << fen << "\n";
    
    Moves root_moves;
    generate_moves(root_board, root_moves);
    
    for (int i = 0; i < root_moves.count; i++) {
        Board b1 = root_board;
        int m1 = root_moves.moves[i];
        
        if (!make_move(b1, m1, get_move_capture(m1))) continue; // Skip illegal moves
        
        // 1. Check for immediate Mate (Mate in 1)
        if (check_game_state(b1) == 1) {
            std::cout << "Mate in 1 found: ";
            print_move(m1);
            std::cout << "\n";
            return;
        }
        
        // 2. Check for Stalemate or other draw
        if (check_game_state(b1) == 0) continue; // Stalemate, not mate
        
        // 3. This is now Black's turn.
        // We need to verify that FOR ALL Black legal moves, White has a responding Mate in 1.
        
        Moves black_moves;
        generate_moves(b1, black_moves);
        
        bool all_black_responses_lead_to_mate = true;
        
        // Verify at least one black move exists (already checked by check_game_state != -1)
        // But let's be safe.
        int legal_black_moves = 0;
        
        for (int j = 0; j < black_moves.count; j++) {
            Board b2 = b1;
            int m2 = black_moves.moves[j];
            
            if (!make_move(b2, m2, get_move_capture(m2))) continue;
            
            legal_black_moves++;
            
            // Now it's White's turn (b2)
            // White must have a Mate in 1
            int m3_dummy;
            if (!can_mate_in_1(b2, m3_dummy)) {
                all_black_responses_lead_to_mate = false;
                break; // Failed this branch
            }
        }
        
        if (legal_black_moves > 0 && all_black_responses_lead_to_mate) {
            std::cout << "Mate in 2 found: ";
            print_move(m1);
            std::cout << "\n";
            
            // Optional: Print lines?
            // "If ... then ..."
            return;
        }
    }
    
    std::cout << "No Mate in 2 found.\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: ./mate2 <FEN>\n";
        return 1;
    }
    
    // Combine args into single FEN string if user didn't quote
    std::string fen_str = argv[1];
    for (int i = 2; i < argc; i++) {
        fen_str += " " + std::string(argv[i]);
    }
    
    solve_mate_in_2((char*)fen_str.c_str());
    
    return 0;
}
