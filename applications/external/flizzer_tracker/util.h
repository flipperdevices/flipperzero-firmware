#pragma once

#include <stdbool.h>
#include <stdio.h>

#include "flizzer_tracker.h"
#include "sound_engine/sound_engine_defs.h"
#include "tracker_engine/tracker_engine.h"
#include "tracker_engine/tracker_engine_defs.h"

#include "macros.h"

#define clamp(val, add, _min, _max) val = my_min(_max, my_max(_min, (int32_t)val + add))
#define flipbit(val, bit) \
    { val ^= bit; };

void reset_buffer(SoundEngine* sound_engine);
void play_song(FlizzerTrackerApp* tracker, bool from_cursor);
void stop_song(FlizzerTrackerApp* tracker);

bool is_pattern_empty(TrackerSong* song, uint8_t pattern);
bool check_and_allocate_pattern(TrackerSong* song, uint8_t pattern);
void change_pattern_length(TrackerSong* song, uint16_t new_length);

bool check_and_allocate_instrument(TrackerSong* song, uint8_t inst);
void set_default_song(FlizzerTrackerApp* tracker);