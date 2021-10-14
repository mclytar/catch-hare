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
    VirtualScreenPrintBoard(vs, board_coords);

    RAW_KNOWLEDGE * rk = CreateRawKnowledge("knowledge.dat");
    KNOWLEDGE * k = CreateKnowledge(rk);

    PLAYER ** players = malloc(2 * sizeof(PLAYER *));
    players[0] = CreateAILearner(k);
    players[1] = CreateHumanPlayer(k);

    BOARD board = InitializeBoard();
    BoardDisplay(board, vs, board_coords);



    // TODO: remove once game is implemented!
    VirtualInputAwait(vi);

    // TODO: implement game!

    DestroyKnowledge(&k);
    DestroyRawKnowledge(&rk);

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