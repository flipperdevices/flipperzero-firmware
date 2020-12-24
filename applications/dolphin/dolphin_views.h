#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <gui/canvas.h>
#include <flipper_v2.h>

typedef enum {
    DolphinViewFirstStart,
    DolphinViewProfile,
} DolphinView;

void dolphin_view_first_start_draw(Canvas* canvas, void* context);

void dolphin_view_profile_draw(Canvas* canvas, void* context);
