#include "io.h"
#include "player.h"

uint8_t strategy_HumanPlayer(PLAYER * p, BOARD b);
uint8_t strategy_AILearner(PLAYER * p, BOARD b);

KNOWLEDGE * CreateKnowledge(RAW_KNOWLEDGE * raw) {
    KNOWLEDGE * knowledge = malloc(sizeof(KNOWLEDGE));

    knowledge->raw = raw;

    uint32_t count = 0;
    for (uint32_t i = 0; i < raw->contents_length; i++) {
        count += 1 - (raw->contents[i] >> 31);
    }

    knowledge->status_ids = malloc(count * sizeof(uint32_t));
    uint32_t current = 0;
    for (uint32_t i = 0; i < raw->contents_length; i++) {
        if (raw->contents[i] >> 31 == 0) knowledge->status_ids[current++] = i;
    }

    return knowledge;
}

void DestroyKnowledge(KNOWLEDGE ** k) {
    KNOWLEDGE * knowledge = *k;

    free(knowledge->status_ids);
    *k = NULL;
}

PLAYER * CreateHumanPlayer(KNOWLEDGE * k, VIRTUAL_INPUT * vi) {
    PLAYER * player = malloc(sizeof(PLAYER));

    player->cursor = (BOARD_COORDS) { .x = 2, .y = 2 };
    player->selection = (BOARD_COORDS) { .x = 7, .y = 7 };
    player->input = vi;
    player->knowledge = k;
    player->strategy = strategy_HumanPlayer;

    return player;
}

PLAYER * CreateAILearner(KNOWLEDGE * k, VIRTUAL_INPUT * vi) {
    PLAYER * player = malloc(sizeof(PLAYER));

    player->cursor = (BOARD_COORDS) { .x = 2, .y = 2 };
    player->selection = (BOARD_COORDS) { .x = 7, .y = 7 };
    player->input = vi;
    player->knowledge = k;
    player->strategy = strategy_AILearner;

    return player;
}

void DestroyPlayer(PLAYER ** p) {
    PLAYER * player = *p;
    free(player);
    *p = NULL;
}

uint8_t strategy_HumanPlayer(PLAYER * p, BOARD b) {
    uint8_t input = VirtualInputAwait(p->input);

    switch (input) {
        case CMD_UP:
            if (p->cursor.y < 4) p->cursor.y++;
            return 0;
        case CMD_DOWN:
            if (p->cursor.y > 0) p->cursor.y--;
            return 0;
        case CMD_RIGHT:
            if (p->cursor.x < 4) p->cursor.x++;
            return 0;
        case CMD_LEFT:
            if (p->cursor.x > 0) p->cursor.x--;
            return 0;
        case CMD_ENTER:
            if (p->selection.x == p->cursor.x && p->selection.y == p->cursor.y) p->selection = (BOARD_COORDS) { .x = 7, .y = 7 };
            else if (PIECE_TO_PLAYER(BoardPieceAt(b, p->cursor)) == BoardCurrentPlayer(b)) p->selection = p->cursor;
            else if (p->selection.x != 7 && p->selection.y != 7 && BoardPieceAt(b, p->cursor) == PIECE_NIL) return 1;
            else return 0;
        default:
            return 0;
    }
}

uint8_t strategy_AILearner(PLAYER * p, BOARD b) {
    return strategy_HumanPlayer(p, b);
}

/*

BOARD HumanPlayerStrategy(PLAYER * player, GAMESCREEN screen, BOARD board) {
    while (1) {
        PrintBoard(screen, board);
        WORD input = AwaitForInput(screen);

        switch (input) {
            case VK_UP:
                if (screen->current_cell_y < 4) screen->current_cell_y++;
                break;
            case VK_DOWN:
                if (screen->current_cell_y > 0) screen->current_cell_y--;
                break;
            case VK_LEFT:
                if (screen->current_cell_x > 0) screen->current_cell_x--;
                break;
            case VK_RIGHT:
                if (screen->current_cell_x < 4) screen->current_cell_x++;
                break;
            case VK_RETURN:
                if (screen->selected_cell_x == screen->current_cell_x && screen->selected_cell_y == screen->current_cell_y) {
                    screen->selected_cell_x = 5;
                    screen->selected_cell_y = 5;
                } else if (PLAYER_PIECE(BoardCurrentPlayer(board)) == BoardAt(board, screen->current_cell_x, screen->current_cell_y)) {
                    screen->selected_cell_x = screen->current_cell_x;
                    screen->selected_cell_y = screen->current_cell_y;
                } else if (BoardCanMove(board, screen->selected_cell_x, screen->selected_cell_y, screen->current_cell_x, screen->current_cell_y)) {
                    BOARD update = BoardMove(board, screen->selected_cell_x, screen->selected_cell_y, screen->current_cell_x, screen->current_cell_y);
                    screen->selected_cell_x = 5;
                    screen->selected_cell_y = 5;
                    return update;
                } else if (BoardCanCapture(board, screen->selected_cell_x, screen->selected_cell_y, screen->current_cell_x, screen->current_cell_y)) {
                    BOARD update = BoardCapture(board, screen->selected_cell_x, screen->selected_cell_y, screen->current_cell_x, screen->current_cell_y);
                    screen->selected_cell_x = 5;
                    screen->selected_cell_y = 5;
                    return update;
                }
                break;
        }
    }
}

typedef struct move_t {
    uint8_t from_x;
    uint8_t from_y;
    uint8_t to_x;
    uint8_t to_y;
} MOVE;

uint32_t BoardGetMoveInfo(BOARD board, const BOARD * aux, uint32_t * variants) {
    uint32_t length = aux[0];
    aux++;

    for (uint32_t i = 0; i < length; i++) {
        // Check if the DWORD is the start of a new record.
        if (aux[i] & 0xFE000000) continue;
        // Check if the selected board is the one currently in game.
        if ((aux[i] & 0x01FFFFFF) != (board & 0x01FFFFFF)) continue;
        // If the above checks pass, the board has been found at index i.
        uint32_t idx = i;
        // Search the board length.
        for (i++; i < length && (aux[i] & 0xFE000000); i++);
        *variants = i - idx;
        return idx;
    }
    // Otherwise, return the default value.
    return UINT32_MAX;
}

MOVE FindMove(BOARD board, const BOARD * aux) {
    uint32_t length = aux[0];
    aux++;

    for (uint32_t i = 0; i < length; i++) {
        // Check if the DWORD is the start of a new record.
        if (aux[i] & 0xFE000000) continue;
        // Check if the selected board is the one currently in game.
        if ((aux[i] & 0x01FFFFFF) != (board & 0x01FFFFFF)) continue;
        // If the above checks pass, search among the possible positions of the hare.
        for (uint32_t j = 1; i + j < length; j++) {
            // If a new record is found, return "no move".
            if (!(aux[i + j] & 0xFE000000)) {
                return (MOVE) {
                    .from_x = 7,
                    .from_y = 7,
                    .to_x = 7,
                    .to_y = 7
                };
            }
            // Check if the selected move is for the current position of the hare.
            if ((aux[i + j] & 0x7E000000) != (board & 0x7E000000)) continue;
            // If the above check passes, return the move.
            return (MOVE) {
                .from_x =  aux[i + j] & 0x00000007,
                .from_y = (aux[i + j] & 0x00000038) >> 3,
                .to_x   = (aux[i + j] & 0x000001C0) >> 6,
                .to_y   = (aux[i + j] & 0x00000E00) >> 9
            };
        }
        // If no record is found, return "no move".
        return (MOVE) {
            .from_x = 7,
            .from_y = 7,
            .to_x = 7,
            .to_y = 7
        };
    }
    // If no record is found, return "no move".
    return (MOVE) {
        .from_x = 7,
        .from_y = 7,
        .to_x = 7,
        .to_y = 7
    };
}

void UpdateKnowledge(BOARD ** knowledge, BOARD board, uint8_t from_x, uint8_t from_y, uint8_t to_x, uint8_t to_y) {
    BOARD * aux = *knowledge;
    uint32_t length = aux[0];
    aux++;

    for (uint32_t i = 0; i < length; i++) {
        // Check if the DWORD is the start of a new record.
        if (aux[i] & 0xFE000000) continue;
        // Check if the selected board is the one currently in game.
        if ((aux[i] & 0x01FFFFFF) != (board & 0x01FFFFFF)) continue;
        // If the above checks pass, add the new move.
        // Construct the move.
        BOARD move = board & 0x7E000000;
        move |= 0x80000000;
        move |= from_x;
        move |= (from_y << 3);
        move |= (to_x << 6);
        move |= (to_y << 9);
        // Reinitialize the array.
        BOARD * new_k = malloc(sizeof(BOARD) * (length + 2));
        new_k[0] = length + 1;
        memcpy(&new_k[1], aux, i * sizeof(BOARD));
        new_k[i + 1] = move;
        memcpy(&new_k[i + 2], &aux[i], (length - i) * sizeof(BOARD));
        // Replace the array.
        free(*knowledge);
        *knowledge = new_k;
        return;
    }
    // If no record is found, add the new record in full.
    // Construct the move.
    BOARD move = board & 0x7E000000;
    move |= 0x80000000;
    move |= from_x;
    move |= (from_y << 3);
    move |= (to_x << 6);
    move |= (to_y << 9);
    // Reinitialize the array.
    BOARD * new_k = malloc(sizeof(BOARD) * (length + 3));
    new_k[0] = length + 2;
    memcpy(&new_k[1], aux, length * sizeof(BOARD));
    new_k[length + 1] = board & 0x01FFFFFF;
    new_k[length + 2] = move;
    // Replace the array.
    free(*knowledge);
    *knowledge = new_k;
}

BOARD AILearnerStrategy(PLAYER * player, GAMESCREEN screen, BOARD board) {
    PrintBoard(screen, board);

    MOVE move = FindMove(board, player->aux);

    if (move.from_x != 7 && move.from_y != 7 && move.to_x != 7 && move.to_y != 7) {
        printf("\33[19BAI is playing...\33[16D");
        Sleep(1000);
        board = BoardMove(board, move.from_x, move.from_y, move.to_x, move.to_y);
        printf("                \33[16D\33[19A");
        return board;
    }

    while (1) {
        PrintBoard(screen, board);

        printf("\33[19BAI does not know what to do!\33[28D\33[19A");

        WORD input = AwaitForInput(screen);

        switch (input) {
            case VK_UP:
                if (screen->current_cell_y < 4) screen->current_cell_y++;
                break;
            case VK_DOWN:
                if (screen->current_cell_y > 0) screen->current_cell_y--;
                break;
            case VK_LEFT:
                if (screen->current_cell_x > 0) screen->current_cell_x--;
                break;
            case VK_RIGHT:
                if (screen->current_cell_x < 4) screen->current_cell_x++;
                break;
            case VK_RETURN:
                if (screen->selected_cell_x == screen->current_cell_x && screen->selected_cell_y == screen->current_cell_y) {
                    screen->selected_cell_x = 5;
                    screen->selected_cell_y = 5;
                } else if (PLAYER_PIECE(BoardCurrentPlayer(board)) == BoardAt(board, screen->current_cell_x, screen->current_cell_y)) {
                    screen->selected_cell_x = screen->current_cell_x;
                    screen->selected_cell_y = screen->current_cell_y;
                } else if (BoardCanMove(board, screen->selected_cell_x, screen->selected_cell_y, screen->current_cell_x, screen->current_cell_y)) {
                    // Teach!
                    UpdateKnowledge((BOARD **)&player->aux, board, screen->selected_cell_x, screen->selected_cell_y, screen->current_cell_x, screen->current_cell_y);
                    BOARD update = BoardMove(board, screen->selected_cell_x, screen->selected_cell_y, screen->current_cell_x, screen->current_cell_y);
                    screen->selected_cell_x = 5;
                    screen->selected_cell_y = 5;
                    printf("\33[19B                            \33[28D\33[19A");
                    return update;
                }
                break;
        }
    }
}

PLAYER CreateHumanPlayer(uint8_t side) {
    PLAYER player = {
            .aux = NULL,
            .side = side,
            .strategy = HumanPlayerStrategy,
            .drop = NULL
    };
    return player;
}

PLAYER CreateAIPlayer(uint8_t side) {
    // TODO: read the `strategy.dat` file (if any) and perform some move from the list (or a random safe move).
    PLAYER player = {
            .aux = NULL,
            .side = side,
            .strategy = HumanPlayerStrategy,
            .drop = NULL
    };
    return player;
}

void AILearnerDestructor(PLAYER * player, BOARD board) {
    uint8_t result = BoardGameOver(board);

    if (result == 2) {
        FILE * pFile;

        errno_t err = fopen_s(&pFile, "knowledge.dat", "wb");

        if (err) {
            printf("E%d", err);
            Sleep(2000);
        }

        fwrite(&player->aux[1], sizeof(BOARD), player->aux[0], pFile);

        fclose(pFile);
    }
}

PLAYER CreateAILearner(uint8_t side) {
    // TODO: read the `strategy.dat` file (if any) and perform some move from the list (or ask the player).
    FILE * pFile;
    BOARD * aux;

    errno_t err = fopen_s(&pFile, "knowledge.dat", "rb");

    if (err == ENOENT) {
        aux = malloc(3 * sizeof(BOARD));
        aux[0] = 2;
        aux[1] = 0x000047FF;
        aux[2] = 0xA4000449;
    } else {
        fseek(pFile, 0, SEEK_END);
        fpos_t size;
        fgetpos(pFile, &size);
        fseek(pFile, 0, SEEK_SET);
        aux = malloc(size + sizeof(BOARD));
        aux[0] = (BOARD)size / sizeof(BOARD);
        size_t sz = fread(&aux[1], sizeof(BOARD), aux[0], pFile);
        fclose(pFile);
    }

    PLAYER player = {
            .aux = aux,
            .side = side,
            .strategy = AILearnerStrategy,
            .drop = AILearnerDestructor
    };
    return player;
}

 */