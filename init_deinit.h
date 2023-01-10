#pragma once

#include "flizzer_tracker_hal.h"
#include "flizzer_tracker.h"

void init_tracker(FlizzerTrackerApp* tracker, uint32_t sample_rate, bool external_audio_output, uint32_t audio_buffer_size);
void deinit_tracker(FlizzerTrackerApp* tracker);