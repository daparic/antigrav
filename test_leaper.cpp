#include "attacks.h"
#include "bitboard.h"
#include <iostream>
#include <string>

// Helper to parse square (e.g., "e4" -> index)
int parse_square(const std::string &sq) {
  if (sq.length() != 2)
    return -1;
  int file = sq[0] - 'a';
  int rank = 8 - (sq[1] - '0');
  if (file < 0 || file > 7 || rank < 0 || rank > 7)
    return -1;
  return rank * 8 + file;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cout << "Usage: ./test_leaper <piece: P/N/K> <square: e4>\n";
    return 1;
  }

  std::string piece_str = argv[1];
  std::string sq_str = argv[2];

  int square = parse_square(sq_str);
  if (square == -1) {
    std::cout << "Invalid square: " << sq_str << "\n";
    return 1;
  }

  // Initialize tables
  init_leapers_attacks();

  U64 attack_bb = 0ULL;
  char piece = piece_str[0];

  std::cout << "Attacks for " << piece << " on " << sq_str << ":\n";

  if (piece == 'P') {
    // White Pawn
    attack_bb = pawn_attacks[0][square];
  } else if (piece == 'p') {
    // Black Pawn
    attack_bb = pawn_attacks[1][square];
  } else if (piece == 'N' || piece == 'n') {
    attack_bb = knight_attacks[square];
  } else if (piece == 'K' || piece == 'k') {
    attack_bb = king_attacks[square];
  } else {
    std::cout << "Invalid piece (use P, p, N, or K)\n";
    return 1;
  }

  print_bitboard(attack_bb);

  return 0;
}
