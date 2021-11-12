#define DEBUG

#include <stdlib.h>

#include "main.h"
#include "board.h"
#include "io.h"
#include "player.h"

#ifdef DEBUG
#include "test.h"
#endif

#define MENU_MAIN_PLAY              0
#define MENU_MAIN_MULTIPLAYER       1
#define MENU_MAIN_TEACH             2
#define MENU_MAIN_QUIT              3

void play_teach(VIRTUAL_SCREEN * vs, VIRTUAL_INPUT * vi) {
    VirtualScreenDrawFrame(vs, "Catch the Hare -- Teach the AI");
    VS_COORDS board_coords = MAKE_COORDS(8, 5);
    VirtualScreenDrawBoard(vs, board_coords);

    KNOWLEDGE * k = CreateKnowledge("knowledge.dat");

    PLAYER ** players = malloc(2 * sizeof(PLAYER *));
    players[0] = CreateAILearner(k, vi);
    players[1] = CreateHumanPlayer(k, vi);

    BOARD board = InitializeBoard();
    BoardDisplay(board, vs, board_coords);
    BOARD_DISPLAY_FORMAT display_format;

    // main game loop.
    while (!BoardGameOver(board)) {
        PLAYER * current_player = players[BoardCurrentPlayer(board)];

        // Clear board.
        BoardDisplayFormatClear(&display_format);
        // Colorize selection.
        BoardDisplayFormatSet(&display_format, current_player->selection, COLOR_GREEN | COLOR_BLUE, COLOR_BLACK);
        // Colorize possible moves.
        BOARD avail_moves = BoardAvailableMovesFrom(board, current_player->selection);
        BOARD avail_captures = BoardAvailableCapturesFrom(board, current_player->selection);
        for (uint8_t i = 0; i < 25; i++) {
            if (avail_moves & (1 << i)) {
                display_format.format[i] = COLOR_GREEN << 4;
            }
            if (avail_captures & (1 << i)) {
                display_format.format[i] = COLOR_RED << 4;
            }
            KNOWLEDGE_MOVE move;
            BOARD board_move = board & 0x01FFFFFF;
            if (current_player == players[0]) {
                board_move |= 0x7E000000;
                BoardMove(&board_move, current_player->selection, (BOARD_COORDS) { .x = (i % 5), .y = (i / 5) });
                for (uint32_t j = 0; j < k->contents_length; j++) {
                    if ((avail_moves & (1 << i)) && k->contents[j] == (board_move & 0x01FFFFFF)) {
                        display_format.format[i] |= COLOR_BLUE << 4;
                        break;
                    }
                }
                board_move = board & 0x01FFFFFF;
            } else {
                board_move |= (i % 5) << 25;
                board_move |= (i / 5) << 28;
                if (((avail_moves | avail_captures) & (1 << i)) && (KnowledgeGetMove(current_player->knowledge, board_move, &move))) {
                    display_format.format[i] |= display_format.format[i] << 1;
                }
            }
        }
        if (display_format.format[current_player->cursor.y * 5 + current_player->cursor.x] & 0xF0) {
            display_format.format[current_player->cursor.y * 5 + current_player->cursor.x] |= 0x80;
        } else {
            BoardDisplayFormatInvert(&display_format, current_player->cursor);
        }
        BoardDisplayEx(board, vs, board_coords, &display_format);

        VirtualScreenPrint(vs, (VS_COORDS) {.x = 4, .y = 20 }, "                   ");
        if (BoardCurrentPlayer(board)) {
            VirtualScreenPrint(vs, (VS_COORDS) {.x = 4, .y = 20 }, "Now playing: hare");
        } else if (!KnowledgeGetMove(k, board, NULL)) {
            VirtualScreenPrint(vs, (VS_COORDS) {.x = 4, .y = 20 }, "Hounds need help!");
        } else {
            VirtualScreenPrint(vs, (VS_COORDS) {.x = 4, .y = 20 }, "Now playing: hounds");
        }

        if (current_player->strategy(current_player, board)) {
            if (BoardCanMove(board, current_player->selection, current_player->cursor)) {
                BoardMove(&board, current_player->selection, current_player->cursor);
                current_player->selection = (BOARD_COORDS) { .x = 7, .y = 7 };
            } else if (BoardCanCapture(board, current_player->selection, current_player->cursor)) {
                BoardCapture(&board, current_player->selection, current_player->cursor);
                current_player->selection = (BOARD_COORDS) { .x = 7, .y = 7 };
            }
        }
    }

    VirtualScreenDrawFrame(vs, "Catch the Hare -- Teach the AI");
    VirtualScreenPrint(vs, (VS_COORDS) {.x = 4, .y = 6}, "GAME OVER!");
    if (BoardCurrentPlayer(board)) {
        KnowledgeStore(k);
        VirtualScreenPrint(vs, (VS_COORDS) {.x = 4, .y = 7}, "Hounds' victory!");
    } else {
        VirtualScreenPrint(vs, (VS_COORDS) {.x = 4, .y = 7}, "Hare's victory!");
    }
    VirtualInputFlush(vi);
    VirtualInputAwait(vi);

    //DestroyPlayer(&players[1]);
    //DestroyPlayer(&players[0]);
    //free(players);
    //DestroyKnowledge(&k);

    VirtualScreenDrawFrame(vs, "Catch the Hare");
    VirtualScreenPrint(vs, MAKE_COORDS(2, 3), "Main Menu");
}

void menu_main(VIRTUAL_SCREEN * vs, VIRTUAL_INPUT * vi) {
    uint8_t item_highlight = 0;

    VirtualScreenDrawFrame(vs, "Catch the Hare");
    VirtualScreenPrint(vs, MAKE_COORDS(2, 3), "Main Menu");

    while (1) {
        VirtualScreenPrintMenu(vs, MAKE_COORDS(4, 5), (const char *[]) {
            "PLAY",
            "MULTIPLAYER",
            "TEACH THE AI",
            "QUIT"
            }, 4, item_highlight);
        uint8_t input = VirtualInputAwait(vi);

        switch (input) {
            case CMD_UP:
                if (item_highlight > 0) item_highlight--;
                break;
            case CMD_DOWN:
                if (item_highlight < 3) item_highlight++;
                break;
            case CMD_ENTER:
                switch (item_highlight) {
                    case MENU_MAIN_PLAY:
                    case MENU_MAIN_MULTIPLAYER:
                        break;
                    case MENU_MAIN_TEACH:
                        play_teach(vs, vi);
                        break;
                    case MENU_MAIN_QUIT:
                        return;
                }
                break;
        }
    }
}

int main() {
    VIRTUAL_SCREEN * screen = CreateVirtualScreen();
    VIRTUAL_INPUT * input = CreateVirtualInput();

    menu_main(screen, input);

    DestroyVirtualInput(&input);
    DestroyVirtualScreen(&screen);

    return 0;
}

/*

PLAYER * PlaySinglePlayer (GAMESCREEN screen) {
    PLAYER * players = malloc(2 * sizeof(PLAYER));

    uint8_t human_player = DisplayMenu(screen, "Select your team", (const char *[]) {
        "HOUNDS",
        "HARE"
        }, 2);

    uint8_t difficulty = DisplayMenu(screen, "Select difficulty", (const char *[]) {
        "AI DOESN'T KNOW HOW TO PLAY",
        "EASY",
        "MEDIUM",
        "HARD",
        "IMPOSSIBLE (at least for HARE)"
        }, 5);

    players[human_player] = CreateHumanPlayer(human_player);
    players[1 - human_player] = CreateAIPlayer(1 - human_player);

    return players;
}
PLAYER * PlayMultiplayer (GAMESCREEN screen) {
    PLAYER * players = malloc(2 * sizeof(PLAYER));

    players[0] = CreateHumanPlayer(0);
    players[1] = CreateHumanPlayer(1);

    return players;
}
PLAYER * PlayTeach (GAMESCREEN screen) {
    PLAYER * players = malloc(2 * sizeof(PLAYER));

    players[0] = CreateAILearner(0);
    players[1] = CreateHumanPlayer(1);
    screen->aux = &players[0].aux;
    screen->training = 1;

    return players;
}

BOARD RunGame(GAMESCREEN screen, BOARD board, PLAYER * players) {
    uint8_t status;

    while (!(status = BoardGameOver(board))) {
        PrintBoard(screen, board);
        uint8_t current_player = BoardCurrentPlayer(board);
        board = players[current_player].strategy(&players[current_player], screen, board);
    }

    DisplayGameOver(screen, status);

    return board;
}

void FinishGame(BOARD board, PLAYER * players) {
    if (players[0].drop) players[0].drop(&players[0], board);
    if (players[1].drop) players[1].drop(&players[1], board);
    free(players);
}

 */