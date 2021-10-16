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

uint8_t BoardGameOver(BOARD board) {
    // Check if game is over for hounds.
    uint32_t hounds_count = board & 0x01FFFFFF;
    hounds_count = (hounds_count & 0x55555555) + ((hounds_count & 0xAAAAAAAA) >> 1);
    hounds_count = (hounds_count & 0x33333333) + ((hounds_count & 0xCCCCCCCC) >> 2);
    hounds_count = (hounds_count & 0x0F0F0F0F) + ((hounds_count & 0xF0F0F0F0) >> 4);
    hounds_count = (hounds_count & 0x00FF00FF) + ((hounds_count & 0xFF00FF00) >> 8);
    hounds_count = (hounds_count & 0x0000FFFF) + ((hounds_count & 0xFFFF0000) >> 16);
    if (hounds_count < 10) return 1;

    // Check if game is over for hare.
    BOARD_COORDS hare_coord = { .x = (board >> 25) & 0x7, .y = (board >> 28) & 0x7 };
    uint8_t linear = hare_coord.y * 5 + hare_coord.x;
    BOARD hare_avail = 0;
    hare_avail |= (0x15 << ((hare_coord.y - 2) * 5 + hare_coord.x)) & (0x1F << ((hare_coord.y - 2) * 5));
    hare_avail |= (0x0E << ((hare_coord.y - 1) * 5 + hare_coord.x)) & (0x1F << ((hare_coord.y - 1) * 5));
    hare_avail |= (0x1B << ((hare_coord.y    ) * 5 + hare_coord.x)) & (0x1F << ((hare_coord.y    ) * 5));
    hare_avail |= (0x0E << ((hare_coord.y + 1) * 5 + hare_coord.x)) & (0x1F << ((hare_coord.y + 1) * 5));
    hare_avail |= (0x15 << ((hare_coord.y + 2) * 5 + hare_coord.x)) & (0x1F << ((hare_coord.y + 2) * 5));
    hare_avail &= (~board & 0x01FFFFFF);
    return !hare_avail;
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

void BoardDisplayEx(BOARD board, VIRTUAL_SCREEN * vs, VS_COORDS coord, BOARD_DISPLAY_FORMAT * format) {
    for (uint8_t x = 0; x < 5; x++) {
        for (uint8_t y = 0; y < 5; y++) {
            uint8_t linear = y * 5 + x;
            uint8_t bg = (format->format[linear] & 0xF0) >> 4;
            uint8_t fg = format->format[linear] & 0x0F;
            VS_COORDS new_coord = {
                    .x = coord.x + x * 5,
                    .y = coord.y + 12 - y * 3
            };
            if ((board >> linear) & 1) {
                VirtualScreenPut(vs, new_coord, 'M', fg, bg);
            } else if (x == ((board >> 25) & 0x7) && y == ((board >> 28) & 0x7)) {
                VirtualScreenPut(vs, new_coord, 'o', fg, bg);
            } else {
                VirtualScreenPut(vs, new_coord, ' ', fg, bg);
            }
        }
    }
}

void BoardDisplayFormatClear(BOARD_DISPLAY_FORMAT * format) {
    for (uint8_t i = 0; i < 25; i++) {
        format->format[i] = 0x07;
    }
}

void BoardDisplayFormatSet(BOARD_DISPLAY_FORMAT * format, BOARD_COORDS coord, uint8_t fg_color, uint8_t bg_color) {
    format->format[coord.y * 5 + coord.x] = ((bg_color << 4) & 0xF0) | (fg_color & 0x0F);
}

void BoardDisplayFormatInvert(BOARD_DISPLAY_FORMAT * format, BOARD_COORDS coord) {
    uint8_t * f = &format->format[coord.y * 5 + coord.x];
    *f = ((*f >> 4) & 0x0F) | ((*f << 4) & 0xF0);
}

BOARD BoardHareAvailableMoves(BOARD board) {
    BOARD_COORDS hare_coord = { .x = (board >> 25) & 0x7, .y = (board >> 28) & 0x7 };
    uint8_t linear = hare_coord.y * 5 + hare_coord.x;
    BOARD hare_avail = 0;
    uint8_t diagonal = (1 - (hare_coord.x + hare_coord.y % 2));
    diagonal = 0x04 | (diagonal << 1) | (diagonal << 3);
    hare_avail |= (diagonal << ((hare_coord.y - 1) * 5 + hare_coord.x)) & (0x1F << ((hare_coord.y - 1) * 5));
    hare_avail |= (0x0A     << ((hare_coord.y    ) * 5 + hare_coord.x)) & (0x1F << ((hare_coord.y    ) * 5));
    hare_avail |= (diagonal << ((hare_coord.y + 1) * 5 + hare_coord.x)) & (0x1F << ((hare_coord.y + 1) * 5));
    hare_avail &= (~board & 0x01FFFFFF);
    return hare_avail;
}

BOARD BoardHareAvailableCaptures(BOARD board) {
    BOARD_COORDS hare_coord = { .x = (board >> 25) & 0x7, .y = (board >> 28) & 0x7 };
    uint8_t linear = hare_coord.y * 5 + hare_coord.x;
    BOARD hare_avail = 0;
    uint8_t diagonal = (1 - (hare_coord.x + hare_coord.y % 2));
    diagonal = 0x04 | (diagonal << 0) | (diagonal << 4);
    hare_avail |= (diagonal << ((hare_coord.y - 2) * 5 + hare_coord.x)) & (0x1F << ((hare_coord.y - 2) * 5));
    hare_avail |= (0x11     << ((hare_coord.y    ) * 5 + hare_coord.x)) & (0x1F << ((hare_coord.y    ) * 5));
    hare_avail |= (diagonal << ((hare_coord.y + 2) * 5 + hare_coord.x)) & (0x1F << ((hare_coord.y + 2) * 5));
    hare_avail &= (~board & 0x01FFFFFF);
    return hare_avail;
}

BOARD BoardFromCoord(BOARD_COORDS coord) {
    uint8_t linear = coord.y * 5 + coord.x;
    linear = 1 << linear;
    return linear & 0x01FFFFFF;
}

BOARD BoardAvailableMovesFrom(BOARD board, BOARD_COORDS from) {
    BOARD hare = 1 << (((board >> 25) & 0x7) + ((board >> 28) & 0x7) * 5);
    board |= hare;
    if (from.x == 7 || from.y == 7) return 0;
    // Possible moves, de-centered in (4, 4).
    BOARD avail = 0x22880000;
    // Possible diagonal moves, de-centered in (4, 4).
    BOARD diag = 0x50140000;
    // Enable diagonal moves if suitable.
    diag *= (1 - ((from.x + from.y) % 2));
    // Add diagonal moves to the available ones.
    avail |= diag;
    // Center possible moves on (x, y).
    avail >>= (4 - from.y) * 5 + (4 - from.x);
    // Construct the column selector.
    BOARD col_selector = (0xE0 >> (6 - from.x)) & 0x1F;
    col_selector |= col_selector << 5;
    col_selector |= col_selector << 10;
    col_selector |= col_selector << 15;
    // Do not select far columns (needed to cancel offset).
    avail &= col_selector;
    // Select free cells only.
    avail &= (~board & 0x01FFFFFF);
    return avail;
}

BOARD BoardAvailableCapturesFrom(BOARD board, BOARD_COORDS from) {
    if (from.x == 7 || from.y == 7) return 0;
    if (!(board & 0x80000000)) return 0;
    // Possible moves, de-centered in (4, 4).
    BOARD avail = 0x22880000;
    // Possible diagonal moves, de-centered in (4, 4).
    BOARD diag = 0x50140000;
    // Enable diagonal moves if suitable.
    diag *= (1 - ((from.x + from.y) % 2));
    // Add diagonal moves to the available ones.
    avail |= diag;
    // Center possible moves on (x, y).
    avail >>= (4 - from.y) * 5 + (4 - from.x);
    // Construct the selectors.
    BOARD col_selector = (0xE0 >> (6 - from.x)) & 0x1F;
    col_selector |= col_selector << 5;
    col_selector |= col_selector << 10;
    col_selector |= col_selector << 15;
    // Do not select far columns (needed to cancel offset).
    avail &= col_selector;
    // Select occupied cells only.
    avail &= (board & 0x01FFFFFF);
    // Move columns 1 cell away from central position.
    if (from.x < 3) {
        BOARD col_mover = 0x108421 << (from.x + 1);
        avail |= (avail & col_mover) << 1;
        avail &= ~col_mover;
    }
    if (from.x > 1) {
        BOARD col_mover = 0x108421 << (from.x - 1);
        avail |= (avail & col_mover) >> 1;
        avail &= ~col_mover;
    }
    if (from.y < 3) {
        BOARD row_mover = 0x1F << (from.y + 1) * 5;
        avail |= (avail & row_mover) << 5;
        avail &= ~row_mover;
    }
    BOARD row_mover = 0x1F << (from.y - 1) * 5;
    avail |= (avail & row_mover) >> 5;
    avail &= ~row_mover;
    // Construct the selectors.
    col_selector = (0x150 >> (6 - from.x)) & 0x1F;
    col_selector |= col_selector << 5;
    col_selector |= col_selector << 10;
    col_selector |= col_selector << 15;
    avail &= col_selector;
    // Select free cells only.
    avail &= (~board & 0x01FFFFFF);
    return avail;
}

uint8_t BoardCanMove(BOARD board, BOARD_COORDS from, BOARD_COORDS to) {
    uint8_t piece_from = BoardPieceAt(board, from);
    uint8_t piece_to = BoardPieceAt(board, to);
    if (piece_to != PIECE_NIL) return 0;
    if (piece_from == PIECE_HOUND || piece_from == PIECE_HARE) {
        BOARD avail = BoardAvailableMovesFrom(board, from);
        BOARD to_linear = to.y * 5 + to.x;
        avail &= (1 << to_linear);
        return !!avail;
    } else return 0;
}

uint8_t BoardCanCapture(BOARD board, BOARD_COORDS from, BOARD_COORDS to) {
    uint8_t piece_from = BoardPieceAt(board, from);
    uint8_t piece_to = BoardPieceAt(board, to);
    if (piece_to != PIECE_NIL) return 0;
    if (piece_from == PIECE_HARE) {
        BOARD avail = BoardAvailableCapturesFrom(board, from);
        BOARD to_linear = to.y * 5 + to.x;
        avail &= (1 << to_linear);
        return !!avail;
    } else return 0;
}

void BoardMove(BOARD * board, BOARD_COORDS from, BOARD_COORDS to) {
    uint8_t piece = BoardPieceAt(*board, from);
    if (piece == PIECE_HARE) {
        *board &= 0x01FFFFFF;
        *board |= ((BOARD)to.x << 25) | ((BOARD)to.y << 28);
    } else if (piece == PIECE_HOUND) {
        BOARD linear_from = from.y * 5 + from.x;
        BOARD linear_to = to.y * 5 + to.x;
        *board &= ~(1 << linear_from);
        *board |= (1 << linear_to) | 0x80000000;
    }
}

void BoardCapture(BOARD * board, BOARD_COORDS from, BOARD_COORDS to) {
    uint8_t piece = BoardPieceAt(*board, from);
    if (piece == PIECE_HARE) {
        from.x += to.x;
        from.x /= 2;
        from.y += to.y;
        from.y /= 2;
        BOARD hound = 1 << (from.y * 5 + from.x);
        *board &= 0x01FFFFFF ^ hound;
        *board |= ((BOARD)to.x << 25) | ((BOARD)to.y << 28);

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