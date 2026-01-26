#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include "movegen.h"

// Define helper to assert and print
void test_fen(std::string fen_label, char* input_fen) {
    std::cout << "Testing: " << fen_label << "\n";
    std::cout << "Input:  " << input_fen << "\n";
    
    Board board;
    parse_fen(input_fen, board);
    
    std::string output_fen = board_to_fen(board);
    std::cout << "Output: " << output_fen << "\n";
    
    if (std::string(input_fen) == output_fen) {
        std::cout << "RESULT: PASS\n";
    } else {
        std::cout << "RESULT: FAIL\n";
    }
    std::cout << "--------------------------------------------------\n";
}

int main() {
    // 1. Start Position
    char* start_pos = (char*)"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    test_fen("Start Position", start_pos);
    
    // 2. KiwiPete (with EP and Castling)
    // Note: KiwiPete standard FEN: r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1
    char* kiwipete = (char*)"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    test_fen("KiwiPete", kiwipete);
    
    // 3. Tricky EP
    char* ep_pos = (char*)"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1";
    test_fen("En Passant Target", ep_pos);
    
    // 4. Counts
    char* counts = (char*)"8/8/8/8/8/8/8/K7 w - - 50 100";
    test_fen("Large Counters", counts);
    
    return 0;
}
