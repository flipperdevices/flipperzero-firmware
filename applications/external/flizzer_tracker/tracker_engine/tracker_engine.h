#pragma once

#include "do_effects.h"
#include "tracker_engine_defs.h"

void tracker_engine_init(TrackerEngine* tracker_engine, uint8_t rate, SoundEngine* sound_engine);
void tracker_engine_deinit(TrackerEngine* tracker_engine, bool free_song);
void tracker_engine_advance_tick(TrackerEngine* tracker_engine);
void tracker_engine_set_song(TrackerEngine* tracker_engine, TrackerSong* song);
void tracker_engine_deinit_song(TrackerSong* song, bool free_song);
void tracker_engine_trigger_instrument_internal(
    TrackerEngine* tracker_engine,
    uint8_t chan,
    Instrument* pinst,
    uint16_t note);

uint8_t tracker_engine_get_note(TrackerSongPatternStep* step);
uint8_t tracker_engine_get_instrument(TrackerSongPatternStep* step);
uint8_t tracker_engine_get_volume(TrackerSongPatternStep* step);
uint16_t tracker_engine_get_command(TrackerSongPatternStep* step);

void set_note(TrackerSongPatternStep* step, uint8_t note);
void set_instrument(TrackerSongPatternStep* step, uint8_t inst);
void set_volume(TrackerSongPatternStep* step, uint8_t vol);
void set_command(TrackerSongPatternStep* step, uint16_t command);

void set_default_instrument(Instrument* inst);
void set_empty_pattern(TrackerSongPattern* pattern, uint16_t pattern_length);