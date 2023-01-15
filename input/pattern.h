#pragma once

#include <stdio.h>
#include <furi.h>
#include <input/input.h>

#include "../sound_engine/sound_engine_defs.h"
#include "../tracker_engine/tracker_engine_defs.h"
#include "../flizzer_tracker.h"
#include "../util.h"

#define MAX_PATTERNX (2 + 1 + 1 + 3)

void pattern_edit_event(FlizzerTrackerApp* tracker, FlizzerTrackerEvent* event);