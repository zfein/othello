#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <iostream>
#include <cstdlib>
#include <vector>
#include "common.h"
#include "board.h"
using namespace std;

#define HUGE_SCORE 1000
#define TINY_SCORE -1000

struct MovePair {
    Move *first;
    Move *second;
};

class Player {

public:
    Player(Side side);
    Player(Side side, Board *b);
    ~Player();
    
    Side us; // The player's side
    Side them; // The opponent's side
    Board *board; // The board state for this player

    Move *doMove(Move *opponentsMove, int msLeft);
    MovePair *pickMove(Board *start_board, int depth, bool verbose);

    // Flag to tell if the player is running within the test_minimax context
    bool testingMinimax;

};


#endif
