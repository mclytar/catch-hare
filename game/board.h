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

#define PIECE_TO_PLAYER(a) ((a) - 1)

typedef uint32_t BOARD;
typedef uint8_t PIECE;
typedef struct board_coord_t {
    uint8_t x : 3;
    uint8_t y : 3;
} BOARD_COORDS;

typedef struct board_display_format_t {
    uint8_t format[25];
} BOARD_DISPLAY_FORMAT;

#define BOARD_DISPLAY_MOVES             1
#define BOARD_DISPLAY_TRAINED_FOR       2
#define BOARD_DISPLAY_TRAINED_SIMILAR   4

BOARD InitializeBoard();
PIECE BoardPieceAt(BOARD board, BOARD_COORDS coord);
uint8_t BoardCurrentPlayer(BOARD board);
uint8_t BoardGameOver(BOARD BOARD);
void BoardDisplay(BOARD board, VIRTUAL_SCREEN * vs, VS_COORDS coord);
void BoardDisplayEx(BOARD board, VIRTUAL_SCREEN * vs, VS_COORDS coord, BOARD_DISPLAY_FORMAT * format);
void BoardDisplayFormatClear(BOARD_DISPLAY_FORMAT * format);
void BoardDisplayFormatSet(BOARD_DISPLAY_FORMAT * format, BOARD_COORDS coord, uint8_t fg_color, uint8_t bg_color);
void BoardDisplayFormatInvert(BOARD_DISPLAY_FORMAT * format, BOARD_COORDS coord);
BOARD BoardHareAvailableMoves(BOARD board);
BOARD BoardHareAvailableCaptures(BOARD board);
BOARD BoardFromCoord(BOARD_COORDS coord);
BOARD BoardAvailableMovesFrom(BOARD board, BOARD_COORDS from);
BOARD BoardAvailableCapturesFrom(BOARD board, BOARD_COORDS from);
uint8_t BoardCanMove(BOARD board, BOARD_COORDS from, BOARD_COORDS to);
uint8_t BoardCanCapture(BOARD board, BOARD_COORDS from, BOARD_COORDS to);
void BoardMove(BOARD * board, BOARD_COORDS from, BOARD_COORDS to);
void BoardCapture(BOARD * board, BOARD_COORDS from, BOARD_COORDS to);

#define PLAYER_PIECE(a) ((a) + 1)


void BoardUpdateTestEnvironment(TestEnvironment * environment);

#endif