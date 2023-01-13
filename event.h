#pragma once

#include <stdio.h>
#include <stdbool.h>

#include "sound_engine/sound_engine_defs.h"
#include "tracker_engine/tracker_engine_defs.h"

void set_note(TrackerSongPatternStep* step, uint8_t note);
void set_instrument(TrackerSongPatternStep* step, uint8_t inst);
void set_volume(TrackerSongPatternStep* step, uint8_t vol);
void set_command(TrackerSongPatternStep* step, uint16_t command);