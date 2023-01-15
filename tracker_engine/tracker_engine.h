#pragma once

#include "tracker_engine_defs.h"

void tracker_engine_init(TrackerEngine* tracker_engine, uint8_t rate, SoundEngine* sound_engine);
void tracker_engine_deinit(TrackerEngine* tracker_engine, bool free_song);
void tracker_engine_advance_tick(TrackerEngine* tracker_engine);

uint8_t tracker_engine_get_note(TrackerSongPatternStep* step);
uint8_t tracker_engine_get_instrument(TrackerSongPatternStep* step);
uint8_t tracker_engine_get_volume(TrackerSongPatternStep* step);
uint16_t tracker_engine_get_command(TrackerSongPatternStep* step);