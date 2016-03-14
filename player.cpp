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

    // Pick our ideal moves
    std::cerr << "Trying to pick a move" << std::endl;
    MovePair *moves = pickMove(board, 1);
    Move *m = moves->first;

    // Make it
    if (m) {
        std::cerr << "Doing move: (" << m->getX() << ", " << m->getY() << ")" << std::endl;
        board->doMove(m, us);
    }
    else {
        std::cerr << "Couldn't find valid move" << std::endl;
    }

    // Return a pointer to our move
    return m;
    
}

MovePair *Player::pickMove(Board *start_board, int depth) {

    // Sanity check for depth argument
    if (depth < 1) {
        std::cerr << "  FATAL ERROR: pickMove() called with depth < 1" << std::endl;
        return NULL;
    }
    
    // Get our valid moves
    std::vector<Move *> our_moves;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Move *here = new Move(i, j);
            if (start_board->checkMove(here, us)) {
                our_moves.push_back(here);
                std::cerr << "  Considering move: (" << i << ", " << j << ")" << std::endl;
            }
            else { delete here; }
        }
    }
    std::cerr << "  Found " << our_moves.size() << " valid moves" << std::endl;
    
    // Prepare struct for output
    MovePair* moves;

    // Handle trivial base case
    if (our_moves.size() == 0) { // No moves
        moves->first = NULL;
        moves->second = NULL;
        return moves;
    }
    std::cerr << "  Nontrivial case, predicting opponent moves" << std::endl;

    // Prepare to find our ideal move
    int score_max = TINY_SCORE; // After their ideal move
    Move *our_ideal_m = new Move(-1, -1);
    Board *our_work_board = start_board->copy();

    // Prepare ahead to find their ideal move
    Move *their_ideal_m = new Move(-1, -1);
    Board *their_work_board = our_work_board->copy();

    // For each of our moves...
    for (unsigned int i = 0; i < our_moves.size(); i++) {

        // Update our working board with our move
        Move *our_m = our_moves[i];
        our_work_board = start_board->copy();
        our_work_board->doMove(our_m, us);
        
        std::cerr << "    Predicting opponent moves after (" << our_m->getX()
        << ", " << our_m->getY() << ")" << std::endl;

        // Get their valid moves
        std::vector<Move *> their_moves;
        for (int j = 0; j < 8; j++) {
            for (int k = 0; k < 8; k++) {
                Move *here = new Move(j, k);
                if (our_work_board->checkMove(here, us)) {
                    their_moves.push_back(here);
                    std::cerr << "      Opponent could try: (" << j << ", " << k << ")" << std::endl;
                }
                else { delete here; }
            }
        }
        
        // Prepare to find their ideal move
        int score_min = HUGE_SCORE; // After their trial move

        // For each of their moves...
        for (unsigned int j = 0; j < their_moves.size(); j++) {
                
                // Update their working board with their move
                Move *their_m = their_moves[j];
                their_work_board = our_work_board->copy();
                their_work_board->doMove(their_m, us);

                // Update their ideal move
                if (their_work_board->count(us) < score_min) {
                    std::cerr << "      Their best move so far" << std::endl;
                    score_min = their_work_board->count(us); // New minimum score
                    their_ideal_m = their_m;
                }

        }
        
        // Found their ideal move
        std::cerr << "    Opponent will do: (" << their_ideal_m->getX()
        << ", " << their_ideal_m->getY() << ")" << std::endl;

        // Update our ideal move
        if (their_ideal_m->getX() != -1) { // They had countermoves
            our_work_board->doMove(their_ideal_m, them);
        }
        if (our_work_board->count(us) > score_max) {
            std::cerr << "    Our best move so far" << std::endl;
            score_max = our_work_board->count(us); // New maximum score
            our_ideal_m = our_m;
            std::cerr << "    Updated our ideal move" << std::endl;
        }

        // Clean up everything used to calculate their move
        delete their_work_board;

    }

    // Clean up everything used to calculate our move
    delete our_work_board;

    // Return move pair
    moves->first = our_ideal_m;
    moves->second = their_ideal_m;
    return moves;

}
