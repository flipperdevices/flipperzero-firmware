#pragma once

#include <furi.h>
#include <input/input.h>
#include <stdio.h>

#include "../flizzer_tracker.h"
#include "../sound_engine/sound_engine_defs.h"
#include "../tracker_engine/tracker_engine_defs.h"
#include "../util.h"

#define MAX_PATTERNX (2 + 1 + 1 + 3)

void pattern_edit_event(FlizzerTrackerApp* tracker, FlizzerTrackerEvent* event);