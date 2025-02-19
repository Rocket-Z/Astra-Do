#include "board.h"

// Board Representation of AstraDo, Version 1

// Initialize board with default setup
AstraDoBoard::AstraDoBoard(){
    // Pos 0, 18, 36 are initially occupied by the side to play first (black)
    black_pieces.fill(false);
    black_pieces[0] = true;
    black_pieces[18] = true;
    black_pieces[36] = true;

    // Pos 9, 27, 45 are initially occupied by the side to play second (white)
    white_pieces.fill(false);
    white_pieces[9] = true;
    white_pieces[27] = true;
    white_pieces[45] = true;

    turn = true;
    stale = false;
    findLegalMoves();
}

// Initialize with given board condition
AstraDoBoard::AstraDoBoard(
    const std::array<bool, 54>& black_array,
    const std::array<bool, 54>& white_array,
    bool next_turn
    ) : black_pieces(black_array),
    white_pieces(white_array),
    turn(next_turn)
{
    stale = false;
    findLegalMoves();
}

// Some getter and setters
const std::array<bool, 54>& AstraDoBoard::getBlackPieces() const {
    return black_pieces;
}

const std::array<bool, 54>& AstraDoBoard::getWhitePieces() const {
    return white_pieces;
}

const std::vector<uint8_t>& AstraDoBoard::getMoves() const {
    return moves;
}

bool AstraDoBoard::getTurn() const {
    return turn;
}

void AstraDoBoard::switchTurn() {
    turn = !turn;
}

bool AstraDoBoard::getStale() const {
    return stale;
}

void AstraDoBoard::setStale(bool stale) {
    this->stale = stale;
}

std::pair<int, int> AstraDoBoard::getPieceCount() const {
    int black_count = std::count(black_pieces.begin(), black_pieces.end(), true);
    int white_count = std::count(white_pieces.begin(), white_pieces.end(), true);
    return std::make_pair(black_count, white_count);
}

// Find all legal moves in the current position
void AstraDoBoard::findLegalMoves() {
    std::set<uint8_t> moves_set;
    size_t cur_pointer = 0, cur_pos, cur_move;
    bool own_piece, oppo_piece, capture_piece;
    for (size_t i = 0; i < lines.size(); ++i) {
        cur_pointer = 0;
        cur_move = 100;     // 100 indicates invalid move
        capture_piece = false;  // Indicate whether there exists opponent piece that can be captured

        // Search from front
        while(cur_pointer < lines[i].size()){
            cur_pos = lines[i][cur_pointer];
            own_piece = turn ? black_pieces[cur_pos] : white_pieces[cur_pos];
            oppo_piece = turn ? white_pieces[cur_pos] : black_pieces[cur_pos];

            // If found an unoccupied square in front
            if(cur_move < 54){
                // If already found pieces that can be captured
                if(capture_piece){
                    // Current square is own piece, output move
                    if(own_piece){
                        moves_set.insert(cur_move);
                        cur_move = 100;
                        capture_piece = false;
                    }
                    // Current square is unoccupied, move is invalid
                    else if(!oppo_piece){
                        cur_move = 100;
                        capture_piece = false;
                    }
                    // Current square is still opponent piece, continue
                }
                // Haven't found enemy piece to capture
                else{
                    // Current square is opponent piece, set flag
                    if(oppo_piece){
                        capture_piece = true;
                    }
                    // Current square is our own piece
                    // Cannot make move on this square, search next
                    else if(own_piece) {
                        cur_move = 100;
                    }
                    // Current square is unoccupied, update move
                    else{
                        cur_move = cur_pos;
                    }
                }
            }
            // If haven't found an unoccupied square in front
            else{
                // Current square is not occupied
                // Save the position of square for examination
                if(!own_piece && !oppo_piece){
                    cur_move = cur_pos;
                }
                // If current square is occupied, skip
            }
            ++cur_pointer;
        }

        // Search from back
        cur_pointer = lines[i].size() - 1;
        cur_move = 100;
        capture_piece = false;
        // Since cur_pointer is uint8_t, it will overflow to 255 after 0
        while(cur_pointer < lines[i].size()){
            cur_pos = lines[i][cur_pointer];
            if(turn){
                own_piece = black_pieces[cur_pos];
                oppo_piece = white_pieces[cur_pos];
            }
            else{
                own_piece = white_pieces[cur_pos];
                oppo_piece = black_pieces[cur_pos];
            }

            // If found an unoccupied square in front
            if(cur_move < 54){
                // If already found pieces that can be captured
                if(capture_piece){
                    // Current square is own piece, output move
                    if(own_piece){
                        moves_set.insert(cur_move);
                        cur_move = 100;
                        capture_piece = false;
                    }
                    // Current square is unoccupied, move is invalid
                    else if(!oppo_piece){
                        cur_move = 100;
                        capture_piece = false;
                    }
                    // Current square is still opponent piece, continue
                }
                // Haven't found enemy piece to capture
                else{
                    // Current square is opponent piece, set flag
                    if(oppo_piece){
                        capture_piece = true;
                    }
                    // Current square is our own piece
                    // Cannot make move on this square, search next
                    else if(own_piece) {
                        cur_move = 100;
                    }
                    // Current square is unoccupied, update move
                    else{
                        cur_move = cur_pos;
                    }
                }
            }
            // If haven't found an unoccupied square in front
            else{
                // Current square is not occupied
                // Save the position of square for examination
                if(!own_piece && !oppo_piece){
                    cur_move = cur_pos;
                }
                // If current square is occupied, skip
            }
            --cur_pointer;
        }
    }

    // Copy content to "moves"
    moves.assign(moves_set.begin(), moves_set.end());
}

// Note that this function does not check whether the move is legal
// The check should be performed by caller before calling this method
// If move >= 54 (illegal move), the function will simply flip the side of the board
void AstraDoBoard::makeMove(uint8_t move) {
    // No legal moves can be made
    if(move >= 54){
        // Set stale condition
        stale = true;

        // Flip the side of the game
        switchTurn();

        // Update current potential moves
        findLegalMoves();
        return;
    }
    uint8_t line_id, line_pointer, cur_pointer, cur_pos;
    bool capture = false;
    std::array<bool, 54>& current_pieces = turn ? black_pieces : white_pieces;
    std::array<bool, 54>& opponent_pieces = turn ? white_pieces : black_pieces;
    // Set status on the square of the move
    current_pieces[move] = true;
    std::set<uint8_t> square_set;

    for (size_t i = 0; i < squares[move].size(); ++i){
        line_id = squares[move][i][0];
        line_pointer = squares[move][i][1];

        // Search in right-hand side
        capture = false;
        cur_pointer = line_pointer + 1;
        while(cur_pointer < lines[line_id].size()){
            cur_pos = lines[line_id][cur_pointer];
            // Has neighboring pieces that can be captured
            if(capture){
                // Has own pieces next to opponent pieces, make capture
                if(current_pieces[cur_pos]){
                    // Record all squares that should be flipped
                    --cur_pointer;
                    while(cur_pointer > line_pointer){
                        cur_pos = lines[line_id][cur_pointer];
                        square_set.insert(cur_pos);
                        --cur_pointer;
                    }
                    break;
                }
                // Neighboring square is unoccupied, end search
                else if(!opponent_pieces[cur_pos]){
                    break;
                }
                // Neighboring square is occupied by opponent, continue search
            }
            else{
                // Found neighboring opponent piece, mark as capture
                if(opponent_pieces[cur_pos]){
                    capture = true;
                }
                // Neighboring piece is our own piece or empty, cannot capture on this direction
                else{
                    break;
                }
            }
            ++cur_pointer;
        }

        // Search in left-hand side
        capture = false;
        cur_pointer = line_pointer - 1;
        // Note that cur_pointer has type uint8_t, it will overflow to 255 after 0
        while(cur_pointer < lines[line_id].size()){
            cur_pos = lines[line_id][cur_pointer];
            // Has neighboring pieces that can be captured
            if(capture){
                // Has own pieces next to opponent pieces, make capture
                if(current_pieces[cur_pos]){
                    // Record all squares that should be flipped
                    ++cur_pointer;
                    while(cur_pointer < line_pointer){
                        cur_pos = lines[line_id][cur_pointer];
                        square_set.insert(cur_pos);
                        ++cur_pointer;
                    }
                    break;
                }
                // Neighboring square is unoccupied, end search
                else if(!opponent_pieces[cur_pos]){
                    break;
                }
                // Neighboring square is occupied by opponent, continue search
            }
            else{
                // Found neighboring opponent piece, mark as capture
                if(opponent_pieces[cur_pos]){
                    capture = true;
                }
                // Neighboring piece is our own piece or empty, cannot capture on this direction
                else{
                    break;
                }
            }
            --cur_pointer;
        }
    }

    // Flip all the recorded squares
    for(uint8_t square : square_set){
        black_pieces[square] = !black_pieces[square];
        white_pieces[square] = !white_pieces[square];
    }

    // Set stale
    stale = false;

    // Flip the side of the game
    switchTurn();

    // Update current potential moves
    findLegalMoves();
}

const std::vector<std::vector<uint8_t>> AstraDoBoard::lines = {
    // Horizontal lines
    {40, 35, 34, 33, 32, 31, 26},
    {42, 41, 37, 30, 29, 28, 21, 25, 24},
    {44, 43, 39, 38, 36, 27, 18, 20, 19, 23, 22},
    {49, 50, 46, 47, 45,  0,  9, 11, 12, 16, 17},
    {51, 52, 48,  1,  2,  3, 10, 14, 15},
    {53,  4,  5,  6,  7,  8,  13},

    // Top right to bottom left
    {35, 40, 41, 42, 43, 44, 49},
    {33, 34, 30, 37, 38, 39, 46, 50, 51},
    {31, 32, 28, 29, 27, 36, 45, 47, 48, 52, 53},
    {26, 25, 21, 20, 18,  9,  0,  2,  1,  5,  4},
    {24, 23, 19, 12, 11, 10,  3,  7,  6},
    {22, 17, 16, 15, 14, 13,  8},

    // Top Left to bottom right
    {31, 26, 25, 24, 23, 22, 17},
    {33, 32, 28, 21, 20, 19, 12, 16, 15},
    {35, 34, 30, 29, 27, 18,  9, 11, 10, 14, 13},
    {40, 41, 37, 38, 36, 45,  0,  2,  3,  7,  8},
    {42, 43, 39, 46, 47, 48,  1,  5,  6},
    {44, 49, 50, 51, 52, 53,  4},
    };

// Stores the id of line and the corresponding position within the line for each square
const std::vector<std::vector<std::vector<uint8_t>>> AstraDoBoard::squares = {
    {{3, 5}, {9, 6}, {15, 6}},
    {{4, 3}, {9, 8}, {16, 6}},
    {{4, 4}, {9, 7}, {15, 7}},
    {{4, 5}, {10, 6}, {15, 8}},
    {{5, 1}, {9, 10}, {17, 6}},
    {{5, 2}, {9, 9}, {16, 7}},
    {{5, 3}, {10, 8}, {16, 8}},
    {{5, 4}, {10, 7}, {15, 9}},
    {{5, 5}, {11, 6}, {15, 10}},
    {{3, 6}, {9, 5}, {14, 6}},
    {{4, 6}, {10, 5}, {14, 8}},
    {{3, 7}, {10, 4}, {14, 7}},
    {{3, 8}, {10, 3}, {13, 6}},
    {{5, 6}, {11, 5}, {14, 10}},
    {{4, 7}, {11, 4}, {14, 9}},
    {{4, 8}, {11, 3}, {13, 8}},
    {{3, 9}, {11, 2}, {13, 7}},
    {{3, 10}, {11, 1}, {12, 6}},
    {{2, 6}, {9, 4}, {14, 5}},
    {{2, 8}, {10, 2}, {13, 5}},
    {{2, 7}, {9, 3}, {13, 4}},
    {{1, 6}, {9, 2}, {13, 3}},
    {{2, 10}, {11, 0}, {12, 5}},
    {{2, 9}, {10, 1}, {12, 4}},
    {{1, 8}, {10, 0}, {12, 3}},
    {{1, 7}, {9, 1}, {12, 2}},
    {{0, 6}, {9, 0}, {12, 1}},
    {{2, 5}, {8, 4}, {14, 4}},
    {{1, 5}, {8, 2}, {13, 2}},
    {{1, 4}, {8, 3}, {14, 3}},
    {{1, 3}, {7, 2}, {14, 2}},
    {{0, 5}, {8, 0}, {12, 0}},
    {{0, 4}, {8, 1}, {13, 1}},
    {{0, 3}, {7, 0}, {13, 0}},
    {{0, 2}, {7, 1}, {14, 1}},
    {{0, 1}, {6, 0}, {14, 0}},
    {{2, 4}, {8, 5}, {15, 4}},
    {{1, 2}, {7, 3}, {15, 2}},
    {{2, 3}, {7, 4}, {15, 3}},
    {{2, 2}, {7, 5}, {16, 2}},
    {{0, 0}, {6, 1}, {15, 0}},
    {{1, 1}, {6, 2}, {15, 1}},
    {{1, 0}, {6, 3}, {16, 0}},
    {{2, 1}, {6, 4}, {16, 1}},
    {{2, 0}, {6, 5}, {17, 0}},
    {{3, 4}, {8, 6}, {15, 5}},
    {{3, 2}, {7, 6}, {16, 3}},
    {{3, 3}, {8, 7}, {16, 4}},
    {{4, 2}, {8, 8}, {16, 5}},
    {{3, 0}, {6, 6}, {17, 1}},
    {{3, 1}, {7, 7}, {17, 2}},
    {{4, 0}, {7, 8}, {17, 3}},
    {{4, 1}, {8, 9}, {17, 4}},
    {{5, 0}, {8, 10}, {17, 5}}
};
