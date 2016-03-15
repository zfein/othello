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

    bool verbose = false;

    // Update board if the opponent just made a move
    if (opponentsMove) {
        board->doMove(opponentsMove, them);
        if (verbose)
            std::cerr << "Opponent made a move, I updated" << std::endl;
    }

    // Pick our ideal moves
    if (verbose)
        std::cerr << "Trying to pick a move" << std::endl;
    MovePair *moves = pickMove(board, 3, verbose);
    if (verbose)
        std::cerr << "Got moves" << std::endl;
    Move *m = moves->first;

    // Make it
    if (m) {
        if (verbose)
            std::cerr << "Doing move: (" << m->getX() << ", " << m->getY() << ")" << std::endl;
        board->doMove(m, us);
    }
    else {
        if (verbose)
            std::cerr << "Couldn't find valid move" << std::endl;
    }

    // Clean up and return a pointer to our move
    if (moves->second)
        delete moves->second;
    delete moves;
    if (verbose)
        std::cerr << "Returning move" << std::endl;
    return m;
    
}

MovePair *Player::pickMove(Board *start_board, int depth, bool verbose) {

    int movesmade = 0; 
    int movesdelete = 0;
    // Sanity check for depth argument
    if (depth < 1) {
        std::cerr << "  FATAL ERROR: pickMove() called with depth < 1" << std::endl;
        return NULL;
    }

    if (verbose)
        std::cerr << "  MEMORY: Received start_board at " << start_board << std::endl;
    
    // Get our valid moves
    std::vector<Move *> our_moves;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Move *here = new Move(i, j);
            movesmade++;
            if (start_board->checkMove(here, us)) {
                our_moves.push_back(here);
            }
            else { delete here; movesdelete++;}
        }
    }
    if (verbose)
        std::cerr << "  Found " << our_moves.size() << " valid moves" << std::endl;
    
    // Prepare struct for output
    MovePair* moves = new MovePair;

    // Handle trivial base case
    if (our_moves.size() == 0) { // No moves
        moves->first = NULL;
        moves->second = NULL;
        return moves;
    }
    if (verbose)
        std::cerr << "  Nontrivial case, predicting opponent moves" << std::endl;

    // Prepare to find our ideal move
    int score_max = TINY_SCORE; // After their ideal move
    Move *our_ideal_m = new Move(-1, -1);
    movesmade++;

    // Prepare ahead to find their ideal move
    Move *their_ideal_m = new Move(-1, -1);
    movesmade++;

    // For each of our moves...
    for (unsigned int i = 0; i < our_moves.size(); i++) {

        // Update our working board with our move
        Move *our_m = our_moves[i];
        if (verbose)
            std::cerr << "    Considering our move (" << our_m->getX()
        << ", " << our_m->getY() << ")" << std::endl;
        Board *our_work_board = start_board->copy();
        if (verbose) {
            std::cerr << "      MEMORY: Created our_work_board (copied start_board) at "
            << our_work_board << std::endl;
        }
        our_work_board->doMove(our_m, us);

        // Get their valid moves
        std::vector<Move *> their_moves;
        for (int j = 0; j < 8; j++) {
            for (int k = 0; k < 8; k++) {
                Move *here = new Move(j, k);
                movesmade++;
                if (our_work_board->checkMove(here, us)) {
                    their_moves.push_back(here);
                }
                else { delete here; movesdelete++;}
            }
        }
        if (verbose)
            std::cerr << "      Opponent has " << their_moves.size() << " valid countermoves" << std::endl;
        
        // Prepare to find their ideal move
        int score_min = HUGE_SCORE; // After their trial move
        Move *their_ideal_m_for_ours = new Move(-1, -1);
        movesmade++;

        // For each of their moves...
        for (unsigned int j = 0; j < their_moves.size(); j++) {
                
            // Update their working board with their move
            Move *their_m = their_moves[j];
            if (verbose)
                std::cerr << "      Considering their move (" << their_m->getX()
            << ", " << their_m->getY() << ")" << std::endl;
            Board* their_work_board = our_work_board->copy();
            if (verbose)
                std::cerr << "        MEMORY: Created their_work_board (copied our_work_board) at "
            << their_work_board << std::endl;
            their_work_board->doMove(their_m, them);

            // If requested, recursively get next two moves and update their working board
            if (depth > 1) {

                if (verbose)
                    std::cerr << "        Attempting recursion" << std::endl;

                MovePair* next_moves = pickMove(their_work_board, depth - 1, verbose);
                
                if (verbose)
                    std::cerr << "        Recursion returned:" << std::endl;

                if (next_moves->first) { // Our next ideal move exists
                    
                    if (verbose) {
                        std::cerr << "          Our ideal move: ("
                        << next_moves->first->getX() << ", "
                        << next_moves->first->getY() << ")"
                        << std::endl;
                    }
                    
                    their_work_board->doMove(next_moves->first, us);

                }
                else {
                    if (verbose)
                        std::cerr << "          No ideal move for us" << std::endl;
                }
                if (next_moves->second) { // Their next ideal countermove exists
                    
                    if (verbose) {
                        std::cerr << "          Their ideal countermove: ("
                        << next_moves->second->getX() << ", "
                        << next_moves->second->getY() << ")"
                        << std::endl;
                    }
                    
                    their_work_board->doMove(next_moves->second, them);

                }
                else {
                    if (verbose)
                        std::cerr << "          No ideal countermove for them" << std::endl;
                }


                delete next_moves;

            }

            // Update their ideal move using their working board
            if (their_work_board->score(us) < score_min) {
                if (verbose)
                   std::cerr << "        Their best move so far" << std::endl;
                score_min = their_work_board->score(us); // New minimum score
                their_ideal_m_for_ours = their_m;
            }

        
            // Clean up everything used for this one of their moves
            if (verbose)
                std::cerr << "        MEMORY: Deleting their_work_board at " << their_work_board << std::endl;
            delete their_work_board;
            if (verbose)
                std::cerr << "        MEMORY: Deleted their_work_board successfully " << std::endl;

        }
        
        Move *their_ideal_m_for_ours2 = new Move(-1, -1);
        their_ideal_m_for_ours2->setX(their_ideal_m_for_ours->getX());
        their_ideal_m_for_ours2->setY(their_ideal_m_for_ours->getY());
        // Found their ideal move
        if (verbose) {
            std::cerr << "      Opponent will do: (" << their_ideal_m_for_ours2->getX()
            << ", " << their_ideal_m_for_ours2->getY() << ")" << std::endl;
        }

        // Set up score
        if (their_moves.size() > 0) { // They had countermoves
            
            // Update work board with their ideal countermove for this one of our moves
            our_work_board->doMove(their_ideal_m_for_ours2, them);

            // their_ideal_m_for_ours is already filled
            // Delete memory allocated to all their moves
            for (unsigned int w = 0; w < their_moves.size(); w++) {
                delete their_moves[w];
                movesdelete++;
            }

        }
        else { // They had no countermoves

            // Work board is already updated

            // They have no ideal countermove for this one of our moves
            their_ideal_m_for_ours2 = NULL;

        }


        // Update our ideal move
        if (our_work_board->score(us) > score_max) {
            if (verbose)
                std::cerr << "      Our best move so far" << std::endl;
            score_max = our_work_board->score(us); // New maximum score
            our_ideal_m = our_m;
            their_ideal_m = their_ideal_m_for_ours2;
        }

        // Clean up everything used to calculate for this one of our moves
        if (verbose)
            std::cerr << "      MEMORY: Deleting our_work_board at " << our_work_board << std::endl;
        delete our_work_board;
        if (verbose)
            std::cerr << "      MEMORY: Deleted our_work_board succesfully" << std::endl;

    }

    // Return move pair
    if (verbose) {
        std::cerr << "  Returning moves!" << std::endl;
        std::cerr << "  We will do: (" << our_ideal_m->getX() << ", " << our_ideal_m->getY() << ")" << std::endl;
        if (their_ideal_m) {
            std::cerr << "  They should do: (" << their_ideal_m->getX() << ", "
            << their_ideal_m->getY() << ")" << std::endl;
        }
        else {
            std::cerr << "  They will have no countermoves" << std::endl;
        }
        std::cerr << "  Putting our move into output package" << std::endl; 
    }
    Move *our_ideal_m2 = new Move(-1, -1);
    our_ideal_m2->setX(our_ideal_m->getX());
    our_ideal_m2->setY(our_ideal_m->getY());


    for (unsigned int ww = 0; ww < our_moves.size(); ww++) {
        delete our_moves[ww];
        movesdelete++;
    }

    moves->first = our_ideal_m2;
    if (verbose) {
        std::cerr << "  Putting their expected move into output package" << std::endl; 
        std::cerr << "moves leaked = " << movesmade - movesdelete << std::endl;

    }

    moves->second = their_ideal_m;
    return moves;

}
