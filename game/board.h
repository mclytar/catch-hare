#ifndef __BOARD_H
#define __BOARD_H

#include <stdint.h>

#include "io.h"
#include "test.h"

#define DW_HI(a) ((a & 0xFFFF0000) >> 16)
#define MAKE_BOARD_COORD(a, b) ((BOARD_COORD) { .x = (a), .y = (b) })

#define PLAYER_HOUND    0
#define PLAYER_HARE     1

#define PIECE_NIL   0
#define PIECE_HOUND 1
#define PIECE_HARE  2
#define PIECE_ERR   3

typedef uint32_t BOARD;
typedef uint8_t PIECE;
typedef struct board_coord_t {
    uint8_t x : 3;
    uint8_t y : 3;
} BOARD_COORDS;

#define BOARD_DISPLAY_MOVES             1
#define BOARD_DISPLAY_TRAINED_FOR       2
#define BOARD_DISPLAY_TRAINED_SIMILAR   4

BOARD InitializeBoard();
PIECE BoardPieceAt(BOARD board, BOARD_COORDS coord);
uint8_t BoardCurrentPlayer(BOARD board);
void BoardDisplay(BOARD board, VIRTUAL_SCREEN * vs, VS_COORDS coord);

#define PLAYER_PIECE(a) ((a) + 1)

uint8_t BoardCanMove(BOARD, uint8_t, uint8_t, uint8_t, uint8_t);
uint8_t BoardCanCapture(BOARD, uint8_t, uint8_t, uint8_t, uint8_t);
BOARD BoardMove(BOARD, uint8_t, uint8_t, uint8_t, uint8_t);
BOARD BoardCapture(BOARD, uint8_t, uint8_t, uint8_t, uint8_t);
uint8_t BoardGameOver(BOARD);


void BoardUpdateTestEnvironment(TestEnvironment * environment);

#endif