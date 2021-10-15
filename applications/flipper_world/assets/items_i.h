#pragma once
#include "items.h"

typedef struct {
    const Icon* icon;
    uint8_t len;
    Vec2 rel_pos;
    Vec2 path[16];
} AnimationSequence;

void food_redraw(Canvas* canvas, void* state);
void food_callback(void* state);

void console_redraw(Canvas* canvas, void* state);
void console_callback(void* state);
