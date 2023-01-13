#pragma once

#include <stdio.h>
#include <stdbool.h>

#include "../sound_engine/sound_engine_defs.h"

#define INST_PROG_LEN 16
#define MUS_SONG_NAME_LEN 17
#define MUS_INST_NAME_LEN (MUS_SONG_NAME_LEN - 3)

#define SONG_MAX_CHANNELS NUM_CHANNELS
#define MAX_INSTRUMENTS 31
#define MAX_PATTERN_LENGTH 256
#define MAX_PATTERNS 64
#define MAX_SEQUENCE_LENGTH 256

#define MUS_NOTE_NONE 127
#define MUS_NOTE_RELEASE 126
#define MUS_NOTE_CUT 125

#define MUS_NOTE_INSTRUMENT_NONE 31
#define MUS_NOTE_VOLUME_NONE 31

#define SONG_FILE_SIG "FZT!SONG"

#define TRACKER_ENGINE_VERSION 1

typedef enum
{
	TE_ENABLE_VIBRATO = 1,
	TE_ENABLE_PWM = 2,
	TE_PROG_NO_RESTART = 4,
	TE_SET_CUTOFF = 8,
	TE_SET_PW = 16,
	TE_KEY_SYNC = 32, //sync oscillators on keydown
	TE_RETRIGGER_ON_SLIDE = 64, //call trigger instrument function even if slide command is there
} TrackerEngineFlags;

typedef enum
{
	TEC_PLAYING = 1,
	TEC_PROGRAM_RUNNING = 2,
	TEC_DISABLED = 4,
} TrackerEngineChannelFlags;

typedef struct
{
	uint8_t a, d, s, r, volume;
} InstrumentAdsr;

typedef struct
{
	char name[MUS_INST_NAME_LEN];

	uint8_t waveform;
	uint16_t flags;
	uint16_t sound_engine_flags;

	uint8_t slide_speed;

	InstrumentAdsr adsr;

	uint8_t ring_mod, hard_sync; //0xff = self

	uint8_t pw; //store only one byte since we don't have the luxury of virtually unlimited memory!

	uint16_t program[INST_PROG_LEN]; //MSB is unite bit (indicates this and next command must be executed at once)
	uint8_t program_period;

	uint8_t vibrato_speed, vibrato_depth, vibrato_delay;
	uint8_t pwm_speed, pwm_depth, pwm_delay;

	uint8_t filter_cutoff, filter_resonance, filter_type;

	int8_t finetune;
} Instrument;

typedef struct
{
	Instrument* instrument;

	uint16_t flags;

	uint8_t channel_flags;

	uint16_t note, target_note, last_note, fixed_note;
	int16_t arpeggio_note;

	uint8_t volume;

	uint8_t program_counter, program_tick, program_loop, program_period;

	uint16_t filter_cutoff;
	uint8_t filter_resonance, filter_type;

	uint8_t vibrato_speed, vibrato_depth, vibrato_delay;
	uint8_t pwm_speed, pwm_depth, pwm_delay;

	uint16_t vibrato_position, pwm_position;

	uint8_t extarp1, extarp2;

	uint16_t pw;

	uint8_t slide_speed;
} TrackerEngineChannel;

typedef struct
{
	uint8_t note; //MSB is used for instrument number MSB
	uint8_t inst_vol; //high nibble + MSB from note = instrument, low nibble = 4 volume LSBs
	uint16_t command; //MSB used as volume MSB
} TrackerSongPatternStep;

typedef struct
{
	TrackerSongPatternStep* step;
} TrackerSongPattern;

typedef struct
{
	uint8_t pattern_indices[SONG_MAX_CHANNELS];
} TrackerSongSequenceStep;

typedef struct
{
	TrackerSongSequenceStep sequence_step[MAX_SEQUENCE_LENGTH];
} TrackerSongSequence;

typedef struct
{
	Instrument* instrument[MAX_INSTRUMENTS];
	TrackerSongPattern pattern[MAX_PATTERNS];
	TrackerSongSequence sequence;

	uint8_t num_patterns, num_sequence_steps, num_instruments;
	uint8_t pattern_length;

	char song_name[MUS_SONG_NAME_LEN];
	uint8_t speed, rate;

	uint8_t loop_start, loop_end;
} TrackerSong;

typedef struct
{
	TrackerEngineChannel channel[SONG_MAX_CHANNELS];

	TrackerSong* song;
	SoundEngine* sound_engine;

	uint8_t pattern_position, sequence_position, current_tick;
	uint16_t absolute_position; //sequence_position * pattern_length + pattern_position

	uint8_t pattern_length;

	uint8_t speed, rate;

	bool playing; //if we reach the end of the song and song does not loop we just stop there
} TrackerEngine;