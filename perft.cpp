#include <iostream>
#include <chrono>
#include "movegen.h"

// Leaf node count
long long nodes = 0;

// Perft recursive function
void perft(Board board, int depth) {
    if (depth == 0) {
        nodes++;
        return;
    }
    
    Moves moves;
    generate_moves(board, moves);
    
    // Simple loop over moves
    for (int i = 0; i < moves.count; i++) {
        // Copy board state
        Board next_board = board;
        
        // Execute move
        if (!make_move(next_board, moves.moves[i], get_move_capture(moves.moves[i]))) {
            continue; // Illegal move
        }
        
        // Recurse
        perft(next_board, depth - 1);
    }
}

// Perft driver
void perft_test(char* fen, int depth) {
    Board board;
    parse_fen(fen, board);
    print_bitboard(board.occupancies[2]);
    
    nodes = 0;
    
    std::cout << "\nStarting Perft Test for Depth " << depth << "\n";
    auto start = std::chrono::high_resolution_clock::now();
    
    // Root moves
    Moves moves;
    generate_moves(board, moves);
    
    for (int i = 0; i < moves.count; i++) {
        Board next_board = board;
        if (!make_move(next_board, moves.moves[i], get_move_capture(moves.moves[i]))) {
            continue;
        }
        
        long long old_nodes = nodes;
        perft(next_board, depth - 1);
        long long new_nodes = nodes - old_nodes;
        
        std::cout << "move: ";
        print_move(moves.moves[i]);
        std::cout << " nodes: " << new_nodes << "\n";
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> explained = end - start;
    
    std::cout << "\nTotal nodes: " << nodes;
    std::cout << "\nTime: " << explained.count() * 1000 << " ms\n";
    std::cout << "NPS: " << (nodes / explained.count()) << "\n";
}

int main() {
    init_leapers_attacks();
    
    // Start Position
    char* start_position = (char*)"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    // KiwiPete
    char* kiwipete = (char*)"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    
    std::cout << "-----------------------\n";
    std::cout << "Position 1 (Start Pos)\n";
    perft_test(start_position, 3); // Depth 3 (Should be 8902 nodes)
    
    std::cout << "\n-----------------------\n";
    std::cout << "Position 2 (KiwiPete)\n";
    perft_test(kiwipete, 1); // Depth 1 (Should be 48)
    
    return 0;
}
