#pragma once

#define HANGMAN_GALLOWS_MAX_STATE 7
#define HANGMAN_GALLOWS_INIT_STATE 0
#define HANGMAN_MAX_ALP_SIZE 0xFF

#define HANGMAN_META_FILE APP_ASSETS_PATH("russian.bolk")

#include "hangman_icons.h"

#include <math.h>
#include <gui/gui.h>
#include <gui/elements.h>
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
    char* dict_file;
    uint16_t unicode_base;
    uint8_t first_letter_offset;
    uint8_t letters_cnt;
    uint8_t keyboard_cols;
    uint8_t keyboard_rows;
    uint8_t keyboard_gap;
    uint16_t letters[HANGMAN_MAX_ALP_SIZE];
    char *message_ok, *message_won, *message_loose;
} HangmanLangConfig;

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    char* word;
    uint8_t pos;
    uint8_t gallows_state;
    HangmanOpened opened[HANGMAN_MAX_ALP_SIZE];
    bool need_generate;
    HangmanGameResult eog;
    HangmanLangConfig* lang;
} HangmanApp;

void hangman_app_free(HangmanApp** app);
void hangman_render_callback(Canvas* canvas, void* ctx);
HangmanApp* hangman_app_alloc();
bool hangman_main_loop(HangmanApp* app);
bool hangman_wait_close_window(HangmanApp* app);
void hangman_draw_gallows(Canvas* canvas, HangmanApp* app);
void hangman_clear_state(HangmanApp* app);
void hangman_text_window(Canvas* canvas, char* ok, char* txt);
