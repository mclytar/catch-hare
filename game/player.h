#ifndef __PLAYER_H
#define __PLAYER_H

#include "io.h"
#include "board.h"



typedef struct knowledge_t {
    RAW_KNOWLEDGE * raw;
    uint32_t status_count;
    uint32_t * status_ids;
} KNOWLEDGE;

KNOWLEDGE * CreateKnowledge(RAW_KNOWLEDGE * raw);
void DestroyKnowledge(KNOWLEDGE ** k);
uint32_t KnowledgeGetStatusId(KNOWLEDGE * k, BOARD board, uint32_t * offset);
uint32_t KnowledgeGetStatusCapacity(KNOWLEDGE * k, uint32_t status);
BOARD_COORDS KnowledgeGetStatusMoveSource(KNOWLEDGE * k, uint32_t status);
BOARD_COORDS KnowledgeGetStatusMoveDestination(KNOWLEDGE * k, uint32_t status);


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