#pragma once

#include "../flizzer_tracker.h"
#include "../tracker_engine/tracker_engine_defs.h"

#include <furi.h>
#include <gui/gui.h>

extern const char to_char_array[];

void draw_pattern_view(Canvas* canvas, FlizzerTrackerApp* tracker);
void draw_sequence_view(Canvas* canvas, FlizzerTrackerApp* tracker);
void draw_songinfo_view(Canvas* canvas, FlizzerTrackerApp* tracker);

void draw_generic_n_digit_field(
    FlizzerTrackerApp* tracker,
    Canvas* canvas,
    uint8_t focus,
    uint8_t param,
    const char* text,
    uint8_t x,
    uint8_t y,
    uint8_t digits);
char to_char(uint8_t number);
char* notename(uint8_t note);