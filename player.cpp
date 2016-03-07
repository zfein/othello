#include "player.h"

/*
 * Constructor for the player; initialize everything here. The side your AI is
 * on (BLACK or WHITE) is passed in as "side". The constructor must finish 
 * within 30 seconds.
 */
Player::Player(Side side) {
    // Will be set to true in test_minimax.cpp.
    testingMinimax = false;

    /* 
     * TODO: Do any initialization you need to do here (setting up the board,
     * precalculating things, etc.) However, remember that you will only have
     * 30 seconds.
     */
    board = new Board();
    us = side;
    them = (us == BLACK) ? WHITE : BLACK;

}

/*
 * Destructor for the player.
 */
Player::~Player() {
    // comment from Zach to change this file
    // comment from Aritra
    delete board;
}

/*
 * Compute the next move given the opponent's last move. Your AI is
 * expected to keep track of the board on its own. If this is the first move,
 * or if the opponent passed on the last move, then opponentsMove will be NULL.
 *
 * msLeft represents the time your AI has left for the total game, in
 * milliseconds. doMove() must take no longer than msLeft, or your AI will
 * be disqualified! An msLeft value of -1 indicates no time limit.
 *
 * The move returned must be legal; if there are no valid moves for your side,
 * return NULL.
 */
Move *Player::doMove(Move *opponentsMove, int msLeft) {
    /* 
     * TODO: Implement how moves your AI should play here. You should first
     * process the opponent's opponents move before calculating your own move
     */ 

    // Update board if the opponent just made a move
    if (opponentsMove)
        board->doMove(opponentsMove, them);

    // Make a random move
    int n = 0; // Number of valid moves
    Move *m = new Move(-1, -1);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Move here = Move(i, j);
            if (board->checkMove(&here, us)) {
                // With probability 1/m, choose the mth possible spot to 
                // be the next move, and with probability 1 - 1/m, keep 
                // the old move and discard the mth spot. This guarantees 
                // that of the n total possible moves each is chosen with 
                // probability 1/n.
                n++;
                double r = ((double) rand() / (RAND_MAX));
                if (r < 1. / n) {
                    m->setX(i);
                    m->setY(j);
                }
            }
        }
    }

    // Update board with our move if we could make one
    if (n > 0) {
        board->doMove(m, us);
        return m;
    }
    else {
        // No valid moves
        return NULL;
    }
}
