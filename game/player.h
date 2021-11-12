#ifndef __PLAYER_H
#define __PLAYER_H

#include "io.h"
#include "board.h"

typedef struct knowledge_move_t {
    BOARD_COORDS from;
    BOARD_COORDS to;
} KNOWLEDGE_MOVE;

//KNOWLEDGE * CreateKnowledge(KNOWLEDGE * raw);
//void DestroyKnowledge(KNOWLEDGE ** k);
uint32_t KnowledgeGetMove(KNOWLEDGE * k, BOARD board, KNOWLEDGE_MOVE * move);
void KnowledgeUpdate(KNOWLEDGE * k, BOARD board, KNOWLEDGE_MOVE * move);


struct player_t;

typedef uint8_t (* Strategy)(struct player_t *, BOARD);
//typedef void (* Destructor)(struct player_t *, BOARD);

typedef struct player_t {
    BOARD_COORDS cursor;
    BOARD_COORDS selection;
    VIRTUAL_INPUT * input;
    KNOWLEDGE * knowledge;
    Strategy strategy;
} PLAYER;

PLAYER * CreateHumanPlayer(KNOWLEDGE * k, VIRTUAL_INPUT * vi);
PLAYER * CreateAILearner(KNOWLEDGE * k, VIRTUAL_INPUT * vi);
void DestroyPlayer(PLAYER ** p);

/*
PLAYER CreateHumanPlayer(uint8_t);
PLAYER CreateAIPlayer(uint8_t);
PLAYER CreateAILearner(uint8_t);
 uint32_t BoardGetMoveInfo(BOARD, const BOARD *, uint32_t *);
 */


#endif