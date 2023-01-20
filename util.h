#pragma once

#include <stdbool.h>
#include <stdio.h>

#include "flizzer_tracker.h"
#include "sound_engine/sound_engine_defs.h"
#include "tracker_engine/tracker_engine_defs.h"

#define clamp(val, add, _min, _max) val = fmin(_max, fmax(_min, (int32_t)val + add))
#define flipbit(val, bit) \
    {                     \
        val ^= bit;       \
    };

void set_note(TrackerSongPatternStep *step, uint8_t note);
void set_instrument(TrackerSongPatternStep *step, uint8_t inst);
void set_volume(TrackerSongPatternStep *step, uint8_t vol);
void set_command(TrackerSongPatternStep *step, uint16_t command);

void play_song(FlizzerTrackerApp *tracker, bool from_cursor);
void stop_song(FlizzerTrackerApp *tracker);

void set_empty_pattern(TrackerSongPattern* pattern, uint16_t pattern_length);
bool is_pattern_empty(TrackerSong *song, uint8_t pattern);
bool check_and_allocate_pattern(TrackerSong *song, uint8_t pattern);
void change_pattern_length(TrackerSong* song, uint16_t new_length);