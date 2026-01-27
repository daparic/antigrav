#include "attacks.h"
#include "bitboard.h"
#include "movegen.h"
#include <iostream>
#include <string>
#include <vector>

const char *square_to_coordinates[] = {
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8", "a7", "b7", "c7",
    "d7", "e7", "f7", "g7", "h7", "a6", "b6", "c6", "d6", "e6", "f6",
    "g6", "h6", "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5", "a4",
    "b4", "c4", "d4", "e4", "f4", "g4", "h4", "a3", "b3", "c3", "d3",
    "e3", "f3", "g3", "h3", "a2", "b2", "c2", "d2", "e2", "f2", "g2",
    "h2", "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"};

char promoted_pieces[] = {'p', 'n', 'b', 'r', 'q', 'k',
                          'p', 'n', 'b', 'r', 'q', 'k'};

// Helper to print a move in algebraic notation (e.g., e2e4)
void print_move_local(int move) {
  if (get_move_promoted(move)) {
    std::cout << square_to_coordinates[get_move_source(move)]
              << square_to_coordinates[get_move_target(move)]
              << promoted_pieces[get_move_promoted(move)] << " ";
  } else {
    std::cout << square_to_coordinates[get_move_source(move)]
              << square_to_coordinates[get_move_target(move)] << " ";
  }
}

void print_move_details(int move) {
  std::cout << "Move: ";
  print_move_local(move);
  std::cout << "(Source: " << get_move_source(move)
            << ", Target: " << get_move_target(move)
            << ", Double: " << (get_move_double(move) ? "Yes" : "No") << ")\n";
}

int main(int argc, char *argv[]) {
  // Initialize attack tables
  init_leapers_attacks();

  std::string fen;
  if (argc > 1) {
    fen = argv[1];
    // Concatenate remaining args in case FEN has spaces not quoted
    for (int i = 2; i < argc; i++) {
      fen += " " + std::string(argv[i]);
    }
  } else {
    // Default to start position if no FEN provided
    fen = "8/8/8/8/8/8/P3P2P/8 w KQkq - 0 1";
  }

  std::cout << "Testing FEN: " << fen << "\n\n";

  Board board;
  parse_fen(const_cast<char *>(fen.c_str()), board);

  // Print occupancy for both sides (index 2)
  print_bitboard(board.occupancies[2]);

  Moves moves;
  generate_moves(board, moves);

  std::cout << "Generated " << moves.count << " moves:\n";
  for (int i = 0; i < moves.count; i++) {
    print_move_details(moves.moves[i]);
  }

  return 0;
}
