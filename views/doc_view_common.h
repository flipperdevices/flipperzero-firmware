#pragma once

#include <gui/view.h>

void doc_draw_text(Canvas* canvas, FuriString* text,
    uint8_t x_one, uint8_t x_two, uint8_t y,
    uint8_t font_height, uint8_t start_index, uint8_t max_index);