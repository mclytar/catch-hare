#include "board.h"

BOARD InitializeBoard() {
    return 0x240047FF;
}

PIECE BoardPieceAt(BOARD board, BOARD_COORDS coord) {
    if (coord.x > 4) return PIECE_ERR;
    if (coord.y > 4) return PIECE_ERR;

    uint8_t hare_x = (board >> 25) & 0x7;
    uint8_t hare_y = (board >> 28) & 0x7;
    if (hare_x == coord.x && hare_y == coord.y) return PIECE_HARE;

    uint8_t linear = coord.y * 5 + coord.x;
    if ((board >> linear) & 1) return PIECE_HOUND;
    return PIECE_NIL;
}

uint8_t BoardCurrentPlayer(BOARD board) {
    return board >> 31;
}

void BoardDisplay(BOARD board, VIRTUAL_SCREEN * vs, VS_COORDS coord) {
    for (uint8_t x = 0; x < 5; x++) {
        for (uint8_t y = 0; y < 5; y++) {
            uint8_t linear = y * 5 + x;
            if ((board >> linear) & 1) {
                VS_COORDS new_coord = {
                        .x = coord.x + x * 5,
                        .y = coord.y + 12 - y * 3
                };
                VirtualScreenPut(vs, new_coord, 'M', COLOR_RED | COLOR_GREEN | COLOR_BLUE, COLOR_BLACK);
            } else if (x == ((board >> 25) & 0x7) && y == ((board >> 28) & 0x7)) {
                VS_COORDS new_coord = {
                        .x = coord.x + x * 5,
                        .y = coord.y + 12 - y * 3
                };
                VirtualScreenPut(vs, new_coord, 'o', COLOR_RED | COLOR_GREEN | COLOR_BLUE, COLOR_BLACK);
            }
        }
    }
}

/*

uint8_t BoardCanMove(BOARD board, uint8_t from_x, uint8_t from_y, uint8_t to_x, uint8_t to_y) {
    uint8_t piece = PLAYER_PIECE(BoardCurrentPlayer(board));
    if (piece != BoardAt(board, from_x, from_y)) return 0;
    if (BoardAt(board, to_x, to_y) != CELL_NIL) return 0;

    uint8_t delta_x = from_x > to_x ? from_x - to_x : to_x - from_x;
    uint8_t delta_y = from_y > to_y ? from_y - to_y : to_y - from_y;

    if (!delta_x && !delta_y) return 0;
    if (delta_x > 1 || delta_y > 1) return 0;
    if (delta_x == delta_y && (from_x + from_y) % 2) return 0;
    return 1;
}

BOARD BoardMove(BOARD board, uint8_t from_x, uint8_t from_y, uint8_t to_x, uint8_t to_y) {
    if (!BoardCanMove(board, from_x, from_y, to_x, to_y)) return board;

    uint8_t piece = PLAYER_PIECE(BoardCurrentPlayer(board));

    if (piece == CELL_HARE) {
        board &= 0x01FFFFFF;
        board |= ((uint32_t)to_x) << 25;
        board |= ((uint32_t)to_y) << 28;
    } else {
        board &= ~(0x00000001 << ((from_y * 5) + from_x));
        board |= 0x00000001 << ((to_y * 5) + to_x);
        board |= 0x80000000;
    }

    return board;
}

uint8_t BoardCanCapture(BOARD board, uint8_t from_x, uint8_t from_y, uint8_t to_x, uint8_t to_y) {
    uint8_t piece = PLAYER_PIECE(BoardCurrentPlayer(board));
    if (piece == CELL_HOUND) return 0;
    if (piece != BoardAt(board, from_x, from_y)) return 0;
    if (BoardAt(board, to_x, to_y) != CELL_NIL) return 0;

    uint8_t delta_x = from_x > to_x ? from_x - to_x : to_x - from_x;
    uint8_t delta_y = from_y > to_y ? from_y - to_y : to_y - from_y;

    if (!delta_x && !delta_y) return 0;
    if (delta_x > 2 || delta_y > 2) return 0;
    if (delta_x == 1 || delta_y == 1) return 0;
    if (delta_x == delta_y && (from_x + from_y) % 2) return 0;
    if (BoardAt(board, (from_x + to_x) / 2, (from_y + to_y) / 2) == CELL_HOUND) return 1;
    return 0;
}

BOARD BoardCapture(BOARD board, uint8_t from_x, uint8_t from_y, uint8_t to_x, uint8_t to_y) {
    if (!BoardCanCapture(board, from_x, from_y, to_x, to_y)) return board;

    uint8_t captured_x = (from_x + to_x) / 2;
    uint8_t captured_y = (from_y + to_y) / 2;

    board &= ~(0x00000001 << ((captured_y * 5) + captured_x));
    board &= 0x01FFFFFF;
    board |= ((uint32_t)to_x) << 25;
    board |= ((uint32_t)to_y) << 28;

    return board;
}

uint8_t BoardGameOver(BOARD board) {
    BOARD hinds = board & 0x1FFFFFF;
    hinds = ((hinds & 0xAAAAAAAA) >> 1) + (hinds & 0x55555555);
    hinds = ((hinds & 0xCCCCCCCC) >> 2) + (hinds & 0x33333333);
    hinds = ((hinds & 0xF0F0F0F0) >> 4) + (hinds & 0x0F0F0F0F);
    hinds = ((hinds & 0xFF00FF00) >> 8) + (hinds & 0x00FF00FF);
    hinds = ((hinds & 0xFFFF0000) >> 16) + (hinds & 0x0000FFFF);

    if (hinds < 10) return 3;

    uint8_t hare_x = (DW_HI(board) >> 9) & 0x7;
    uint8_t hare_y = (DW_HI(board) >> 12) & 0x7;

    if ((hare_x + hare_y) % 2 == 0 && BoardAt(board, hare_x + 1, hare_y + 1) == CELL_NIL) return 0;
    if (BoardAt(board, hare_x + 1, hare_y) == CELL_NIL) return 0;
    if ((hare_x + hare_y) % 2 == 0 && BoardAt(board, hare_x + 1, hare_y - 1) == CELL_NIL) return 0;
    if (BoardAt(board, hare_x, hare_y + 1) == CELL_NIL) return 0;
    if (BoardAt(board, hare_x, hare_y - 1) == CELL_NIL) return 0;
    if ((hare_x + hare_y) % 2 == 0 && BoardAt(board, hare_x - 1, hare_y + 1) == CELL_NIL) return 0;
    if (BoardAt(board, hare_x - 1, hare_y) == CELL_NIL) return 0;
    if ((hare_x + hare_y) % 2 == 0 && BoardAt(board, hare_x - 1, hare_y - 1) == CELL_NIL) return 0;

    if ((hare_x + hare_y) % 2 == 0 && BoardAt(board, hare_x + 2, hare_y + 2) == CELL_NIL) return 0;
    if (BoardAt(board, hare_x + 2, hare_y) == CELL_NIL) return 0;
    if ((hare_x + hare_y) % 2 == 0 && BoardAt(board, hare_x + 2, hare_y - 2) == CELL_NIL) return 0;
    if (BoardAt(board, hare_x, hare_y + 2) == CELL_NIL) return 0;
    if (BoardAt(board, hare_x, hare_y - 2) == CELL_NIL) return 0;
    if ((hare_x + hare_y) % 2 == 0 && BoardAt(board, hare_x - 2, hare_y + 2) == CELL_NIL) return 0;
    if (BoardAt(board, hare_x - 2, hare_y) == CELL_NIL) return 0;
    if ((hare_x + hare_y) % 2 == 0 && BoardAt(board, hare_x - 2, hare_y - 2) == CELL_NIL) return 0;

    return 2;
}

*/

/* ================================
 * UNIT TESTS
 * ================================
 */

/*
TestResult Test_InitializeBoard() {
    BOARD board = InitializeBoard();

    TEST_ASSERT_EQ(BoardAt(board, 0, 0), CELL_HOUND);
    TEST_ASSERT_EQ(BoardAt(board, 1, 0), CELL_HOUND);
    TEST_ASSERT_EQ(BoardAt(board, 2, 0), CELL_HOUND);
    TEST_ASSERT_EQ(BoardAt(board, 3, 0), CELL_HOUND);
    TEST_ASSERT_EQ(BoardAt(board, 4, 0), CELL_HOUND);
    TEST_ASSERT_EQ(BoardAt(board, 0, 1), CELL_HOUND);
    TEST_ASSERT_EQ(BoardAt(board, 1, 1), CELL_HOUND);
    TEST_ASSERT_EQ(BoardAt(board, 2, 1), CELL_HOUND);
    TEST_ASSERT_EQ(BoardAt(board, 3, 1), CELL_HOUND);
    TEST_ASSERT_EQ(BoardAt(board, 4, 1), CELL_HOUND);
    TEST_ASSERT_EQ(BoardAt(board, 0, 2), CELL_HOUND);
    TEST_ASSERT_EQ(BoardAt(board, 1, 2), CELL_NIL);
    TEST_ASSERT_EQ(BoardAt(board, 2, 2), CELL_HARE);
    TEST_ASSERT_EQ(BoardAt(board, 3, 2), CELL_NIL);
    TEST_ASSERT_EQ(BoardAt(board, 4, 2), CELL_HOUND);
    TEST_ASSERT_EQ(BoardAt(board, 0, 3), CELL_NIL);
    TEST_ASSERT_EQ(BoardAt(board, 1, 3), CELL_NIL);
    TEST_ASSERT_EQ(BoardAt(board, 2, 3), CELL_NIL);
    TEST_ASSERT_EQ(BoardAt(board, 3, 3), CELL_NIL);
    TEST_ASSERT_EQ(BoardAt(board, 4, 3), CELL_NIL);
    TEST_ASSERT_EQ(BoardAt(board, 0, 4), CELL_NIL);
    TEST_ASSERT_EQ(BoardAt(board, 1, 4), CELL_NIL);
    TEST_ASSERT_EQ(BoardAt(board, 2, 4), CELL_NIL);
    TEST_ASSERT_EQ(BoardAt(board, 3, 4), CELL_NIL);
    TEST_ASSERT_EQ(BoardAt(board, 4, 4), CELL_NIL);
    TEST_ASSERT_EQ(BoardAt(board, 0, 5), CELL_ERR);
    TEST_ASSERT_EQ(BoardAt(board, 5, 0), CELL_ERR);

    return TEST_SUCCESS;
}

void BoardUpdateTestEnvironment(TestEnvironment * environment) {
    AddTest(environment, Test_InitializeBoard, "InitializeBoard");
}

*/