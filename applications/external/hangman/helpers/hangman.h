#pragma once

#define HANGMAN_DICT_FILE APP_ASSETS_PATH("dict.bin")
#define HANGMAN_GALLOWS_MAX_STATE 7
#define HANGMAN_GALLOWS_INIT_STATE 0
#define HANGMAN_LETTERS_CNT 32
#define HANGMAN_KEYBOARD_COLS 8
#define HANGMAN_KEYBOARD_ROWS HANGMAN_LETTERS_CNT / HANGMAN_KEYBOARD_COLS
#define HANGMAN_KEYBOARD_GAP 3

#include "hangman_icons.h"
#include <gui/gui.h>
#include <gui/canvas_i.h>
#include <gui/elements.h>
#include <gui/icon_i.h>
#include <u8g2/u8g2.h>
#include <furi.h>
#include <storage/storage.h>
#include <toolbox/stream/stream.h>
#include <toolbox/stream/file_stream.h>
#include <furi_hal_random.h>

typedef enum {
    HangmanOpenedInit,
    HangmanOpenedFound,
    HangmanOpenedNotFound,
} HangmanOpened;

typedef enum {
    HangmanGameOn,
    HangmanGameWin,
    HangmanGameLoose,
} HangmanGameResult;

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    char* word;
    uint8_t pos;
    uint8_t gallows_state;
    HangmanOpened opened[HANGMAN_LETTERS_CNT];
    bool need_generate;
    HangmanGameResult eog;
} HangmanApp;

int8_t hangman_GetGlyphWidth(u8g2_t* u8g2, uint16_t requested_encoding);
void hangman_app_free(HangmanApp** app);
void hangman_render_callback(Canvas* canvas, void* ctx);
HangmanApp* hangman_app_alloc();
bool hangman_main_loop(HangmanApp* app);
bool hangman_wait_close_window(HangmanApp* app);
void hangman_draw_gallows(Canvas* canvas, HangmanApp* app);
void hangman_clear_state(HangmanApp* app);
void hangman_text_window(Canvas* canvas, char* txt);
