#pragma once

#include <gui/gui.h>

const Icon* tile_to_icon(uint8_t tile, bool gameOver);

void gray_canvas(Canvas* const canvas);
void mask_canvas(Canvas* const canvas, uint8_t sx, uint8_t sy, uint8_t w, uint8_t h);

void elements_button_right_back(Canvas* canvas, const char* str);
void elements_multiline_text_aligned_limited(
    Canvas* canvas,
    uint8_t x,
    uint8_t y,
    uint8_t h,
    Align horizontal,
    Align vertical,
    const char* text);

void hint_pill_single(Canvas* canvas, const char* str);
void hint_pill_double(Canvas* canvas, const char* str1, const char* str2, const Icon* icon);

void menu_pill(
    Canvas* canvas,
    int no,
    int count,
    bool selected,
    bool masked,
    const char* label,
    const Icon* icon);

void panel_histogram(Canvas* canvas, const char* bricks, const uint8_t* values);