#pragma once

#define HANGMAN_GALLOWS_MAX_STATE 7
#define HANGMAN_GALLOWS_INIT_STATE 0
#define HANGMAN_MAX_ALP_SIZE 0xFF
#define HANGMAN_MAX_MENU_SIZE 3U

#define HANGMAN_MENU_FILE APP_ASSETS_PATH("menu.txt")

#define CONST const __auto_type

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
    uint8_t word_letters_gap;
    uint16_t letters[HANGMAN_MAX_ALP_SIZE];
    char *message_ok, *message_won, *message_lose;
} HangmanLangConfig;

typedef struct {
    uint16_t* arr;
    size_t len;
} HangmanWord;

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    HangmanWord word;
    uint8_t pos;
    uint8_t gallows_state;
    HangmanOpened opened[HANGMAN_MAX_ALP_SIZE];
    bool need_generate;
    HangmanGameResult eog;
    HangmanLangConfig* lang;
    bool menu_show;
    int8_t menu_item;
    uint8_t menu_frame_position;
    size_t menu_cnt;
    char** menu;
} HangmanApp;

void hangman_app_free(HangmanApp** app);
void hangman_render_callback(Canvas* canvas, void* ctx);
HangmanApp* hangman_app_alloc();
void hangman_draw_gallows(Canvas* canvas, HangmanApp* app);
void hangman_clear_state(HangmanApp* app);
void hangman_text_window(Canvas* canvas, char* ok, char* txt);
void hangman_window(Canvas* canvas, uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void hangman_draw_utf8_str(Canvas* canvas, uint8_t x, uint8_t y, const char* str);

size_t hangman_string_length(const char* str);

char** hangman_menu_read(size_t* menu_size);
void hangman_free_menu_data(char** lines, size_t menu_size);
void hangman_draw_menu(Canvas* canvas, HangmanApp* app);

void hangman_load_lang(HangmanApp* app);
void hangman_choice_letter(HangmanApp* app);

bool hangman_wait_close_window(HangmanApp* app);
bool hangman_menu_selection(HangmanApp* app);
bool hangman_main_loop(HangmanApp* app);

void hangman_set_font(Canvas* canvas, const uint8_t h);
