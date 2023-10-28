#pragma once

#define DICT_FILE APP_ASSETS_PATH("dict.bin")

#include <gui/gui.h>
#include <gui/canvas_i.h>
#include <u8g2/u8g2.h>
#include <furi.h>
#include <storage/storage.h>
#include <toolbox/stream/stream.h>
#include <toolbox/stream/file_stream.h>
#include <furi_hal_random.h>

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    char* word;
} HangmanApp;

int8_t hangman_GetGlyphWidth(u8g2_t* u8g2, uint16_t requested_encoding);
void hangman_app_free(HangmanApp** app);
void hangman_render_callback(Canvas* canvas, void* ctx);
HangmanApp* hangman_app_alloc();
void hangman_wait_a_key(HangmanApp* app);
char* hangman_get_random_word();
void hangman_draw_utf8_str(Canvas* canvas, uint8_t x, uint8_t y, const char* str);
