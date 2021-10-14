#include "io.h"

#include <stdio.h>

#define ASSERT(assertion, exit_text) if (!(assertion)) { fprintf(stderr, "%s. Aborting.", exit_text); exit(2); }

VIRTUAL_SCREEN * CreateVirtualScreen() {
    VIRTUAL_SCREEN * screen = malloc(sizeof(VIRTUAL_SCREEN));
    ASSERT(screen, "Could not allocate memory");

    screen->hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(screen->hStdOut, &screen->info);
    GetConsoleMode(screen->hStdOut, &screen->consoleMode);

    SetConsoleMode(screen->hStdOut, screen->consoleMode | ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    printf("\033[37;40m\033[2J\033[?25l");

    return screen;
}

void DestroyVirtualScreen(VIRTUAL_SCREEN ** vs) {
    VIRTUAL_SCREEN * screen = *vs;
    SetConsoleMode(screen->hStdOut, screen->consoleMode);
    SetConsoleTextAttribute(screen->hStdOut, screen->info.wAttributes);
    free(screen);
    *vs = NULL;
}

unsigned int VirtualScreenWidth(VIRTUAL_SCREEN * vs) {
    ASSERT(vs, "VIRTUAL_SCREEN not allocated");

    return vs->info.srWindow.Right - vs->info.srWindow.Left + 1;
}

unsigned int VirtualScreenHeight(VIRTUAL_SCREEN * vs) {
    ASSERT(vs, "VIRTUAL_SCREEN not allocated");

    return vs->info.srWindow.Bottom - vs->info.srWindow.Top + 1;
}

void VirtualScreenDrawFrame(VIRTUAL_SCREEN * vs, const char * title) {
    ASSERT(vs, "VIRTUAL_SCREEN not allocated");
    unsigned int w = VirtualScreenWidth(vs);
    unsigned int h = VirtualScreenHeight(vs);

    printf("\033[1;1H");

    // Drawing top part of the box.
    printf("%c", 201);
    for (unsigned int i = 0; i < w - 2; i++) {
        printf("%c", 205);
    }
    printf("%c", 187);

    // Drawing the title part.
    uint16_t title_len;
    for (title_len = 0; title_len < w - 4 && title[title_len]; title_len++);
    printf("%c %.*s", 186, title_len, title);
    for (unsigned int i = title_len; i < w - 3; i++) {
        printf(" ");
    }
    printf("%c", 186);

    // Drawing the line under the title.
    printf("%c", 204);
    for (unsigned int i = 0; i < w - 2; i++) {
        printf("%c", 196);
    }
    printf("%c", 185);

    // Drawing all the other lines.
    for (unsigned int y = 3; y < h - 1; y++) {
        printf("%c", 186);
        for (unsigned int x = 0; x < w - 2; x++) {
            printf(" ");
        }
        printf("%c", 186);
    }

    // Drawing the last row.
    printf("%c", 200);
    for (unsigned int i = 0; i < w - 2; i++) {
        printf("%c", 205);
    }
    printf("%c", 188);

    // Set cursor to upper left corner.
    printf("\033[%dA\033[%dD", h, w);
}

void VirtualScreenPrint(VIRTUAL_SCREEN * vs, VS_COORDS coords, const char * text) {
    ASSERT(vs, "VIRTUAL_SCREEN not allocated");
    ASSERT(text, "string not allocated");

    printf("\033[%d;%dH%s\033[1;1H", coords.y + 1, coords.x + 1, text);
}

void VirtualScreenPrintMenu(VIRTUAL_SCREEN * vs, VS_COORDS coords, const char ** items, uint8_t item_count, uint8_t item_highlight) {
    ASSERT(vs, "VIRTUAL_SCREEN not allocated");
    ASSERT(items, "menu not allocated");

    for (uint8_t i = 0; i < item_count; i++) {
        printf("\033[%d;%dH", coords.y + i + 1, coords.x + 1);
        if (item_highlight == i) printf("\033[30;47m");
        printf("%s\033[37;40m", items[i]);
    }
    printf("\033[1;1H");
}

void VirtualScreenPrintBoard(VIRTUAL_SCREEN * vs, VS_COORDS coords) {
    ASSERT(vs, "VIRTUAL_SCREEN not allocated");

    printf("\033[%d;%dH  --   --   --   --  ", coords.y + 1, coords.x + 1);
    printf("\033[%d;%dH| \\  |  / | \\  |  / |", coords.y + 2, coords.x + 1);
    printf("\033[%d;%dH|  \\ | /  |  \\ | /  |", coords.y + 3, coords.x + 1);
    printf("\033[%d;%dH  --   --   --   --  ", coords.y + 4, coords.x + 1);
    printf("\033[%d;%dH|  / | \\  |  / | \\  |", coords.y + 5, coords.x + 1);
    printf("\033[%d;%dH| /  |  \\ | /  |  \\ |", coords.y + 6, coords.x + 1);
    printf("\033[%d;%dH  --   --   --   --  ", coords.y + 7, coords.x + 1);
    printf("\033[%d;%dH| \\  |  / | \\  |  / |", coords.y + 8, coords.x + 1);
    printf("\033[%d;%dH|  \\ | /  |  \\ | /  |", coords.y + 9, coords.x + 1);
    printf("\033[%d;%dH  --   --   --   --  ", coords.y + 10, coords.x + 1);
    printf("\033[%d;%dH|  / | \\  |  / | \\  |", coords.y + 11, coords.x + 1);
    printf("\033[%d;%dH| /  |  \\ | /  |  \\ |", coords.y + 12, coords.x + 1);
    printf("\033[%d;%dH  --   --   --   --  ", coords.y + 13, coords.x + 1);
}

void VirtualScreenPut(VIRTUAL_SCREEN * vs, VS_COORDS coords, char symbol, uint8_t fg_color, uint8_t bg_color) {
    ASSERT(vs, "VIRTUAL_SCREEN not allocated");

    fg_color = (fg_color & 0x7) + 30 + 60 * ((fg_color & 0x8) >> 3);
    bg_color = (bg_color & 0x7) + 40 + 60 * ((bg_color & 0x8) >> 3);

    printf("\033[%d;%dH\033[%d;%dm%c\033[37;40m\033[1;1H", coords.y + 1, coords.x + 1, fg_color, bg_color, symbol);
}

VIRTUAL_INPUT * CreateVirtualInput() {
    VIRTUAL_INPUT * input = malloc(sizeof(VIRTUAL_INPUT));
    ASSERT(input, "Could not allocate memory");

    input->hStdIn = GetStdHandle(STD_INPUT_HANDLE);

    return input;
}

void DestroyVirtualInput(VIRTUAL_INPUT ** vi) {
    VIRTUAL_INPUT * input = *vi;
    free(input);
    *vi = NULL;
}

void VirtualInputFlush(VIRTUAL_INPUT * vi) {
    ASSERT(vi, "VIRTUAL_INPUT not allocated");

    FlushConsoleInputBuffer(vi->hStdIn);
}

uint8_t VirtualInputAwait(VIRTUAL_INPUT * vi) {
    ASSERT(vi, "VIRTUAL_INPUT not allocated");
    INPUT_RECORD input;
    DWORD input_count;

    while (1) {
        WaitForSingleObject(vi->hStdIn, INFINITE);
        ReadConsoleInputA(vi->hStdIn, &input, 1, &input_count);
        if (input_count && input.EventType == KEY_EVENT && input.Event.KeyEvent.bKeyDown) return (uint8_t)input.Event.KeyEvent.wVirtualKeyCode;
    }
}

RAW_KNOWLEDGE * CreateRawKnowledge(const char * filename) {
    ASSERT(filename, "string not allocated");
    RAW_KNOWLEDGE * knowledge;
    FILE * file;

    errno_t err = fopen_s(&file, filename, "rb");
    if (!err) {
        knowledge = malloc(sizeof(RAW_KNOWLEDGE));
        ASSERT(knowledge, "Could not allocate memory");
        // Read file length.
        fseek(file, 0, SEEK_END);
        fpos_t length;
        fgetpos(file, &length);
        fseek(file, 0, SEEK_SET);
        // Read header.
        fread_s(&knowledge->header, sizeof(KNOWLEDGE_HEADER), sizeof(KNOWLEDGE_HEADER), 1, file);
        ASSERT(knowledge->header.magic_number == 0x4B4E4F57, "Unknown file type");
        ASSERT(knowledge->header.version_major != 0, "Incompatible version");
        ASSERT(knowledge->header.version_minor != 1, "Incompatible version");
        // Get contents length.
        unsigned int contents_length = (unsigned int)((length - sizeof(KNOWLEDGE_HEADER)) / sizeof(uint32_t));
        knowledge->contents_length = contents_length;
        // Get contents.
        knowledge->contents = malloc(contents_length * sizeof(uint32_t));
        ASSERT(knowledge->contents, "Could not allocate memory");
        fread_s(knowledge->contents, sizeof(uint32_t), sizeof(uint32_t), contents_length, file);
        // Close file.
        fclose(file);
        return knowledge;
    } else if (err == ENOENT) {
        // Construct file.
        knowledge = malloc(sizeof(RAW_KNOWLEDGE));
        ASSERT(knowledge, "Could not allocate memory");
        knowledge->header.magic_number = 0x4B4E4F57;
        knowledge->header.version_major = 0;
        knowledge->header.version_minor = 1;
        knowledge->filename = filename;
        knowledge->contents_length = 0;
        knowledge->contents = NULL;
        return knowledge;
    } else {
        ASSERT(0, "I/O error");
    }
}

void StoreRawKnowledge(RAW_KNOWLEDGE * k) {
    // TODO
}

void DestroyRawKnowledge(RAW_KNOWLEDGE ** k) {
    // TODO
}