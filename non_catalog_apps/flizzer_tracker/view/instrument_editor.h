#pragma once

#include "../flizzer_tracker.h"
#include "../tracker_engine/tracker_engine_defs.h"
#include "pattern_editor.h"

#include <furi.h>
#include <gui/gui.h>

void draw_instrument_view(Canvas* canvas, FlizzerTrackerApp* tracker);
void draw_instrument_program_view(Canvas* canvas, FlizzerTrackerApp* tracker);