#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <vector>
#include <set>
#include <utility>
#include <cstdint>
#include <iostream>
#include <algorithm>

class AstraDoBoard{
private:
    // Position of pieces of current side (next to play)
    std::array<bool, 54> black_pieces{false};
    // Position of opponent pieces
    std::array<bool, 54> white_pieces{false};
    std::vector<uint8_t> moves;

    // Current turn of the game
    // Black - true
    // White - false
    bool turn;
    // Whether previous move cannot be made
    bool stale;

    static const std::vector<std::vector<uint8_t>> lines;
    static const std::vector<std::vector<std::vector<uint8_t>>> squares;

public:
    // Initialize board by default
    AstraDoBoard();

    // Initialize board from a given position
    explicit AstraDoBoard(
        const std::array<bool, 54>& black_array,
        const std::array<bool, 54>& white_array,
        bool next_turn
        );

    // Getter and setters
    const std::array<bool, 54>& getBlackPieces() const;

    const std::array<bool, 54>& getWhitePieces() const;

    const std::vector<uint8_t>& getMoves() const;

    bool getTurn() const;

    void switchTurn();

    bool getStale() const;

    void setStale(bool stale);

    std::pair<int, int> getPieceCount() const;

    // Find all current legal moves in the current state
    void findLegalMoves();

    // Note that this function does not check whether the move is legal
    // The check should be performed by caller before calling this method
    // If move >= 54 (illegal move), the function will simply flip the side of the board
    void makeMove(uint8_t move);

};

#endif // BOARD_H
