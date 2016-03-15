#include "board.h"

/*
 * Make a standard 8x8 othello board and initialize it to the standard setup.
 */
Board::Board() {
    taken.set(3 + 8 * 3);
    taken.set(3 + 8 * 4);
    taken.set(4 + 8 * 3);
    taken.set(4 + 8 * 4);
    black.set(4 + 8 * 3);
    black.set(3 + 8 * 4);
}

/*
 * Destructor for the board.
 */
Board::~Board() {
}

/*
 * Returns a copy of this board.
 */
Board *Board::copy() {
    Board *newBoard = new Board();
    newBoard->black = black;
    newBoard->taken = taken;
    return newBoard;
}

bool Board::occupied(int x, int y) {
    return taken[x + 8*y];
}

bool Board::get(Side side, int x, int y) {
    return occupied(x, y) && (black[x + 8*y] == (side == BLACK));
}

void Board::set(Side side, int x, int y) {
    taken.set(x + 8*y);
    black.set(x + 8*y, side == BLACK);
}

bool Board::onBoard(int x, int y) {
    return(0 <= x && x < 8 && 0 <= y && y < 8);
}

 
/*
 * Returns true if the game is finished; false otherwise. The game is finished 
 * if neither side has a legal move.
 */
bool Board::isDone() {
    return !(hasMoves(BLACK) || hasMoves(WHITE));
}

/*
 * Returns true if there are legal moves for the given side.
 */
bool Board::hasMoves(Side side) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Move move(i, j);
            if (checkMove(&move, side)) return true;
        }
    }
    return false;
}

/*
 * Returns true if a move is legal for the given side; false otherwise.
 */
bool Board::checkMove(Move *m, Side side) {
    
    // Passing is only legal if you have no moves.
    if (m == NULL) return !hasMoves(side);

    int X = m->getX();
    int Y = m->getY();

    // Make sure the square hasn't already been taken.
    if (occupied(X, Y)) return false;

    Side other = (side == BLACK) ? WHITE : BLACK;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dy == 0 && dx == 0) continue;

            // Is there a capture in that direction?
            int x = X + dx;
            int y = Y + dy;
            if (onBoard(x, y) && get(other, x, y)) {
                do {
                    x += dx;
                    y += dy;
                } while (onBoard(x, y) && get(other, x, y));

                if (onBoard(x, y) && get(side, x, y)) return true;
            }
        }
    }
    return false;
}

/*
 * Modifies the board to reflect the specified move.
 */
void Board::doMove(Move *m, Side side) {
    // A NULL move means pass.
    if (m == NULL) return;

    // Ignore if move is invalid.
    if (!checkMove(m, side)) return;

    int X = m->getX();
    int Y = m->getY();
    Side other = (side == BLACK) ? WHITE : BLACK;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dy == 0 && dx == 0) continue;

            int x = X;
            int y = Y;
            do {
                x += dx;
                y += dy;
            } while (onBoard(x, y) && get(other, x, y));

            if (onBoard(x, y) && get(side, x, y)) {
                x = X;
                y = Y;
                x += dx;
                y += dy;
                while (onBoard(x, y) && get(other, x, y)) {
                    set(side, x, y);
                    x += dx;
                    y += dy;
                }
            }
        }
    }
    set(side, X, Y);
}

/*
 * Current count of given side's stones.
 */
int Board::count(Side side) {
    return (side == BLACK) ? countBlack() : countWhite();
}

/*
 * Current count of black stones.
 */
int Board::countBlack() {
    return black.count();
}

/*
 * Current count of white stones.
 */
int Board::countWhite() {
    return taken.count() - black.count();
}

/*
 * Current score of the given side's stones.
 */
int Board::score(Side side) {
    return (side == BLACK) ? scoreBlack() : scoreWhite();
}

/*
 * Current score of black stones -- corners and sides are more valuable.
 */
int Board::scoreBlack() {
    // Start with the original score
    int score = countBlack();
    // Bonus for corners -- Corners are worth 10
    score += 9 * (black[0] + black[7] + black[56] + black[63]);

    // Bonus for edges -- Edges are worth 3
    for (int i = 1; i < 7; i++) {
        score += 2 * black[i]; // Top edge
        score += 2 * black[56 + i]; // Bottom edge
        score += 2 * black[i * 8]; // Left edge
        score += 2 * black[i * 8 + 7]; // Right edge
    }
    // Penalty for edge piece adjacent to corner -- worth -3 total
    for (int i = 0; i < 2; i++) {
        // The adjacent pieces on the top and bottom edges
        score -= 6 * black[56 * i + 1];
        score -= 6 * black[56 * i + 6];
        // The adjacent pieces on the left and right edges
        score -= 6 * black[40 * i + 8];
        score -= 6 * black[40 * i + 15];
    }
    // Penalty for piece diagonally adjacent to corner -- worth -10 total
    score -= 11 * (black[9] + black[14] + black[49] + black[54]);

    return score;
}

/*
 * Current score of white stones -- corners and sides are more valuable.
 */
int Board::scoreWhite() {
    // Get bitset of white stones -- spots that are taken but not black
    bitset<64> white = (taken & ~black);
    // Start with the original score
    int score = countWhite();
    // Bonus for corners -- Corners are worth 3
    score += 2 * (white[0] + white[7] + white[56] + white[63]);
    // Bonus for edges -- Edges are worth 2
    for (int i = 1; i < 7; i++) {
        score += white[i]; // Top edge
        score += white[56 + i]; // Bottom edge
        score += white[i * 8 + 0]; // Left edge
        score += white[i * 8 + 7]; // Right edge
    }
    // Penalty for edge piece adjacent to corner -- worth -3 total
    for (int i = 0; i < 2; i++) {
        // The adjacent pieces on the top and bottom edges
        score -= 6 * white[56 * i + 1];
        score -= 6 * white[56 * i + 6];
        // The adjacent pieces on the left and right edges
        score -= 6 * white[40 * i + 8];
        score -= 6 * white[40 * i + 15];
    }
    // Penalty for piece diagonally adjacent to corner -- worth -10 total
    score -= 11 * (white[9] + white[14] + white[49] + white[54]);
    return score;
}

/*
 * Sets the board state given an 8x8 char array where 'w' indicates a white
 * piece and 'b' indicates a black piece. Mainly for testing purposes.
 */
void Board::setBoard(char data[]) {
    taken.reset();
    black.reset();
    for (int i = 0; i < 64; i++) {
        if (data[i] == 'b') {
            taken.set(i);
            black.set(i);
        } if (data[i] == 'w') {
            taken.set(i);
        }
    }
}
