#pragma once

#include "tracker_engine_defs.h"

void tracker_engine_init(TrackerEngine* tracker_engine, uint8_t rate, SoundEngine* sound_engine);
void tracker_engine_deinit(TrackerEngine* tracker_engine);
void tracker_engine_advance_tick(TrackerEngine* tracker_engine);