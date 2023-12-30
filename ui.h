#pragma once

#include "common.h"
#include <gui/gui.h>

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