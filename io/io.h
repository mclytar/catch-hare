#ifndef __IO_H
#define __IO_H

#include <stdint.h>
#ifdef _WIN32
#include <Windows.h>
#endif

/* VIRTUAL_SCREEN
 * Cross-platform abstraction of ANSI stdout.
 */

typedef struct virtual_screen_coords_t {
    uint16_t x;
    uint16_t y;
} VS_COORDS;
#define MAKE_COORDS(a, b) ((VS_COORDS) {.x = (a), .y = (b)})

#ifdef _WIN32
typedef struct win32_screen_info_t {
    CONSOLE_SCREEN_BUFFER_INFO info;
    DWORD consoleMode;
    HANDLE hStdOut;
} VIRTUAL_SCREEN;
#endif

VIRTUAL_SCREEN * CreateVirtualScreen();
void DestroyVirtualScreen(VIRTUAL_SCREEN ** vs);

unsigned int VirtualScreenWidth(VIRTUAL_SCREEN * vs);
unsigned int VirtualScreenHeight(VIRTUAL_SCREEN * vs);

void VirtualScreenDrawFrame(VIRTUAL_SCREEN * vs, const char * title);
void VirtualScreenPrint(VIRTUAL_SCREEN * vs, VS_COORDS coords, const char * text);
void VirtualScreenPrintMenu(VIRTUAL_SCREEN * vs, VS_COORDS coords, const char ** items, uint8_t item_count, uint8_t item_highlight);
void VirtualScreenPrintBoard(VIRTUAL_SCREEN * vs, VS_COORDS coords);

#define COLOR_BLACK     0
#define COLOR_RED       1
#define COLOR_GREEN     2
#define COLOR_BLUE      4
#define COLOR_LIGHT     8

void VirtualScreenPut(VIRTUAL_SCREEN * vs, VS_COORDS coords, char symbol, uint8_t fg_color, uint8_t bg_color);

/* VIRTUAL_INPUT
 * Cross-platform abstraction of input keys.
 */

#ifdef _WIN32
typedef struct win32_input_info_t {
    HANDLE hStdIn;
} VIRTUAL_INPUT;

#define CMD_ENTER   VK_RETURN
#define CMD_UP      VK_UP
#define CMD_DOWN    VK_DOWN
#define CMD_LEFT    VK_LEFT
#define CMD_RIGHT   VK_RIGHT
#endif

VIRTUAL_INPUT * CreateVirtualInput();
void DestroyVirtualInput(VIRTUAL_INPUT ** vi);

void VirtualInputFlush(VIRTUAL_INPUT * vi);
uint8_t VirtualInputAwait(VIRTUAL_INPUT * vi);

/* RAW_KNOWLEDGE
 * Cross-platform abstraction of the i/o operations on the knowledge file.
 */

typedef struct knowledge_header_t {
    uint32_t magic_number;
    uint16_t version_major;
    uint16_t version_minor;
} KNOWLEDGE_HEADER;

typedef struct raw_knowledge_t {
    const char * filename;
    KNOWLEDGE_HEADER header;
    unsigned int contents_length;
    uint32_t * contents;
} RAW_KNOWLEDGE;

RAW_KNOWLEDGE * CreateRawKnowledge(const char * filename);
void StoreRawKnowledge(RAW_KNOWLEDGE * k);
void DestroyRawKnowledge(RAW_KNOWLEDGE ** k);

#endif