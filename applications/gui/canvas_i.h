#pragma once

#include "canvas.h"

Canvas* canvas_init();

void canvas_free(Canvas* canvas);

void canvas_reset(Canvas* canvas);

void canvas_commit(Canvas* canvas);

void canvas_frame_set(
    Canvas* canvas,
    uint8_t offset_x,
    uint8_t offset_y,
    uint8_t width,
    uint8_t height);
