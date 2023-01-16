#pragma once

#include "../flizzer_tracker.h"
#include "../tracker_engine/tracker_engine_defs.h"

#include <furi.h>
#include <gui/gui.h>

void draw_pattern_view(Canvas *canvas, FlizzerTrackerApp *tracker);
void draw_sequence_view(Canvas *canvas, FlizzerTrackerApp *tracker);
void draw_songinfo_view(Canvas *canvas, FlizzerTrackerApp *tracker);