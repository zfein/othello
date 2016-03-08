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
 * Constructor for the player; initialize everything here. The side your AI is
 * on (BLACK or WHITE) is passed in as "side", and uses the board provided.
 * The constructor must finish within 30 seconds.
 */
Player::Player(Side side, Board *b) {
    board = b;
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

    // Update board if the opponent just made a move
    if (opponentsMove) {
        board->doMove(opponentsMove, them);
        std::cerr << "Opponent made a move, I updated" << std::endl;
    }

    /* RANDOM
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
    */
    
    std::cerr << "Trying to pick a move" << std::endl;
    Move *m = pickMove(0);
    if (m) {
        std::cerr << "Doing move: (" << m->getX() << ", " << m->getY() << ")" << std::endl;
        board->doMove(m, us);
    }
    else {
        std::cerr << "Couldn't find valid move" << std::endl;
    }
    return m;
    
    /* RANDOM
    // Update board with our move if we could make one
    if (n > 0) {
        board->doMove(m, us);
        return m;
    }
    else {
        // No valid moves
        return NULL;
    }
    */

}

Move *Player::pickMove(int depth) {

    // Find valid moves
    std::vector<Move *> valid_moves;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Move *here = new Move(i, j);
            if (board->checkMove(here, us)) {
                valid_moves.push_back(here);
                std::cerr << "  Considering move: (" << i << ", " << j << ")" << std::endl;
            }
            else { delete here; }
        }
    }

    std::cerr << "  Found " << valid_moves.size() << " valid moves" << std::endl;

    // Trivial base cases
    if (valid_moves.size() == 1) { // One move
        return valid_moves[0];
    }
    else if (valid_moves.size() == 0) { // No moves
        return NULL;
    }
    
    std::cerr << "  Nontrivial case, predicting opponent moves" << std::endl;

    // Base case: consider opponent's next move, one level

    int score_max = TINY_SCORE; // After their ideal move
    Move *our_ideal_m = new Move(-1, -1);
    Board *our_work_board;
    Board *their_work_board;

    for (unsigned int i = 0; i < valid_moves.size(); i++) {

        Move *our_m = valid_moves[i];
        Move *their_m = new Move(-1, -1);
                
        std::cerr << "    Predicting opponent moves after (" << our_m->getX()
        << ", " << our_m->getY() << ")" << std::endl;

        // Update our working board
        our_work_board = board->copy();
        our_work_board->doMove(our_m, us);
        
        // They pick the move that minimizes our score
        int score_min = HUGE_SCORE; // After their trial move
        for (int j = 0; j < 8; j++) {
            for (int k = 0; k < 8; k++) {
               
                // Prepare their working board
                their_work_board = our_work_board->copy();

                // Set their move if it decreases the score beyond the min so far
                Move *here = new Move(j, k);
                if (their_work_board->checkMove(here, them)) {
                    their_work_board->doMove(here, them);
                    std::cerr << "      Opponent could try: (" << j << ", " << k << ")" << std::endl;
                    if (their_work_board->count(us) < score_min) {
                        std::cerr << "      Their best move so far" << std::endl;
                        score_min = their_work_board->count(us); // New minimum score
                        their_m = here;
                    }
                }
                else { delete here; }

            }
        }
        std::cerr << "    Opponent will do: (" << their_m->getX() << ", " << their_m->getY() << ")" << std::endl;

        // Assume their ideal move and update our move if the score is better
        if (their_m->getX() != -1) { // They had countermoves
            our_work_board->doMove(their_m, them);
        }
        if (our_work_board->count(us) > score_max) {
            std::cerr << "    Our best move so far" << std::endl;
            score_max = our_work_board->count(us);
            our_ideal_m = our_m;
        }

        // Clean up everything used to calculate their move
        delete their_work_board;
        delete their_m;

    }

    delete our_work_board;
    return our_ideal_m;

}
