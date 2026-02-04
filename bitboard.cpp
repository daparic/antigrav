#include "bitboard.h"
#include <cstring>
#include <fstream>
#include <vector>
#include "attacks.h"
#include "movegen.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <map>
#include "assets.h"
#include "stb_image_write.h"

// Colors
const uint8_t LIGHT_SQUARE_RGB[] = {240, 217, 181};
const uint8_t DARK_SQUARE_RGB[] = {181, 136, 99};

struct Image {
    int width, height, channels;
    unsigned char* data;
};

std::map<char, Image> piece_images;

void
load_assets() {
    auto embedded_assets = get_embedded_assets();
    const char* pieces = "PNBRQKpnbrqk";
    for (int i = 0; i < 12; i++) {
        char p = pieces[i];

        if (embedded_assets.count(p)) {
            Asset asset = embedded_assets[p];
            int w, h, c;
            unsigned char* data = stbi_load_from_memory(asset.data, asset.size, &w, &h, &c, 4);

            if (data) {
                piece_images[p] = {w, h, c, data};
            } else {
                std::cerr << "Failed to load embedded image for piece: " << p << "\n";
            }
        } else {
            std::cerr << "Piece " << p << " not found in embedded assets\n";
        }
    }
}

void
free_assets() {
    for (auto& pair : piece_images) {
        if (pair.second.data) {
            stbi_image_free(pair.second.data);
        }
    }
}

void
blend_image(std::vector<uint8_t>& board_img, int board_w, int x_off, int y_off, const Image& img) {
    if (!img.data) {
        return;
    }

    // Center the image if possible, or assume 60x60 image in 64x64 square
    // Center offset
    int center_x = (64 - img.width) / 2;
    int center_y = (64 - img.height) / 2;

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            int bx = x_off + x + center_x;
            int by = y_off + y + center_y;

            if (bx >= 0 && bx < board_w && by >= 0 && by < board_w) {
                int src_idx = (y * img.width + x) * 4;
                int dst_idx = (by * board_w + bx) * 3;

                uint8_t src_r = img.data[src_idx];
                uint8_t src_g = img.data[src_idx + 1];
                uint8_t src_b = img.data[src_idx + 2];
                uint8_t src_a = img.data[src_idx + 3];

                // Alpha blend
                // dst = (src * alpha) + (dst * (1 - alpha))
                float alpha = src_a / 255.0f;
                float inv_alpha = 1.0f - alpha;

                board_img[dst_idx] = (uint8_t)(src_r * alpha + board_img[dst_idx] * inv_alpha);
                board_img[dst_idx + 1] = (uint8_t)(src_g * alpha + board_img[dst_idx + 1] * inv_alpha);
                board_img[dst_idx + 2] = (uint8_t)(src_b * alpha + board_img[dst_idx + 2] * inv_alpha);
            }
        }
    }
}

void
render_board(const Board& board, const std::string& filename) {
    load_assets();

    int square_size = 64;
    int board_size = 8 * square_size;

    std::vector<uint8_t> image(board_size * board_size * 3);

    // 1. Draw Board
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            bool is_light = (rank + file) % 2 == 0;
            const uint8_t* color = is_light ? LIGHT_SQUARE_RGB : DARK_SQUARE_RGB;

            for (int y = 0; y < square_size; y++) {
                for (int x = 0; x < square_size; x++) {
                    int global_x = file * square_size + x;
                    int global_y = rank * square_size + y;
                    int idx = (global_y * board_size + global_x) * 3;

                    image[idx] = color[0];
                    image[idx + 1] = color[1];
                    image[idx + 2] = color[2];
                }
            }

            // 2. Draw Piece
            int square = rank * 8 + file;
            char piece_char = ' ';

            // Check piece on this square
            for (int p = P; p <= k; p++) {
                if (get_bit(board.bitboards[p], square)) {
                    const char piece_chars[] = "PNBRQKpnbrqk";
                    piece_char = piece_chars[p];
                    break;
                }
            }

            if (piece_char != ' ') {
                blend_image(image, board_size, file * square_size, rank * square_size, piece_images[piece_char]);
            }
        }
    }

    if (stbi_write_png(filename.c_str(), board_size, board_size, 3, image.data(), board_size * 3)) {
        std::cout << "Board image saved to " << filename << "\n";
    } else {
        std::cerr << "Failed to write image to " << filename << "\n";
    }

    free_assets();
}

// Helper function to print bitboard
void
print_bitboard(U64 bitboard) {
    std::cout << "\n";
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            if (!file) {
                std::cout << "  " << 8 - rank << " ";
            }
            std::cout << " " << (get_bit(bitboard, square) ? 1 : 0);
        }
        std::cout << "\n";
    }
    std::cout << "\n     a b c d e f g h\n\n";
    std::cout << "     Bitboard: " << bitboard << "\n\n";
}

#ifndef BITBOARD_LIB
int
main(int argc, char* argv[]) {
    // Check for CLI args
    std::string fen = "";
    std::string output_file = "board.png";
    bool run_demo = true;

    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--fen" && i + 1 < argc) {
            fen = argv[++i];
            run_demo = false;
        } else if (std::string(argv[i]) == "--output" && i + 1 < argc) {
            output_file = argv[++i];
        }
    }

    if (!run_demo) {
        Board board;
        parse_fen((char*)fen.c_str(), board);
        // print_board is not readily available, using naive print for now or just skip
        std::cout << "Rendering FEN: " << fen << "\n";
        render_board(board, output_file);
        return 0;
    }

    // Default Demo Code
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
    for (int i = 0; i < 12; i++) {
        board.bitboards[i] = 0ULL;
    }
    for (int i = 0; i < 3; i++) {
        board.occupancies[i] = 0ULL;
    }

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
