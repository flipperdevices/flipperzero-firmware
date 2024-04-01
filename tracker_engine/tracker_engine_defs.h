#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "../sound_engine/sound_engine_defs.h"

#define INST_PROG_LEN 16
#define MUS_SONG_NAME_LEN 16
#define MUS_INST_NAME_LEN (MUS_SONG_NAME_LEN - 3)

#define SONG_MAX_CHANNELS NUM_CHANNELS
#define MAX_INSTRUMENTS 31
#define MAX_PATTERN_LENGTH 256
#define MAX_PATTERNS 256
#define MAX_SEQUENCE_LENGTH 256

#define MUS_NOTE_NONE 127
#define MUS_NOTE_RELEASE 126
#define MUS_NOTE_CUT 125

#define MUS_NOTE_INSTRUMENT_NONE 31
#define MUS_NOTE_VOLUME_NONE 31

#define SONG_FILE_SIG "FZT!SONG"
#define SONG_FILE_EXT ".fzt"
#define INST_FILE_SIG "FZT!INST"
#define INST_FILE_EXT ".fzi"

#define TRACKER_ENGINE_VERSION 1

#define MIDDLE_C (12 * 4)
#define MAX_NOTE (12 * 7 + 11)

typedef enum {
    TE_ENABLE_VIBRATO = 1,
    TE_ENABLE_PWM = 2,
    TE_PROG_NO_RESTART = 4,
    TE_SET_CUTOFF = 8,
    TE_SET_PW = 16,
    TE_RETRIGGER_ON_SLIDE = 32, // call trigger instrument function even if slide command is there
} TrackerEngineFlags;

typedef enum {
    TEC_PLAYING = 1,
    TEC_PROGRAM_RUNNING = 2,
    TEC_DISABLED = 4,
} TrackerEngineChannelFlags;

typedef enum {
    TE_EFFECT_ARPEGGIO = 0x0000,
    TE_EFFECT_PORTAMENTO_UP = 0x0100,
    TE_EFFECT_PORTAMENTO_DOWN = 0x0200,
    TE_EFFECT_SLIDE = 0x0300,
    TE_EFFECT_VIBRATO = 0x0400,
    TE_EFFECT_PWM = 0x0500,
    TE_EFFECT_SET_PW = 0x0600,
    TE_EFFECT_PW_DOWN = 0x0700,
    TE_EFFECT_PW_UP = 0x0800,
    TE_EFFECT_SET_CUTOFF = 0x0900,
    TE_EFFECT_VOLUME_FADE = 0x0a00,
    TE_EFFECT_SET_WAVEFORM = 0x0b00,
    TE_EFFECT_SET_VOLUME = 0x0c00,
    TE_EFFECT_SKIP_PATTERN = 0x0d00,

    TE_EFFECT_EXT = 0x0e00,
    TE_EFFECT_EXT_TOGGLE_FILTER = 0x0e00,
    TE_EFFECT_EXT_PORTA_UP = 0x0e10,
    TE_EFFECT_EXT_PORTA_DN = 0x0e20,
    TE_EFFECT_EXT_FILTER_MODE = 0x0e30,
    TE_EFFECT_EXT_PATTERN_LOOP =
        0x0e60, // e60 = start, e61-e6f = end and indication how many loops you want
    TE_EFFECT_EXT_RETRIGGER = 0x0e90,
    TE_EFFECT_EXT_FINE_VOLUME_DOWN = 0x0ea0,
    TE_EFFECT_EXT_FINE_VOLUME_UP = 0x0eb0,
    TE_EFFECT_EXT_NOTE_CUT = 0x0ec0,
    TE_EFFECT_EXT_NOTE_DELAY = 0x0ed0,
    TE_EFFECT_EXT_PHASE_RESET = 0x0ef0,

    TE_EFFECT_SET_SPEED_PROG_PERIOD = 0x0f00,
    TE_EFFECT_CUTOFF_UP = 0x1000, // Gxx
    TE_EFFECT_CUTOFF_DOWN = 0x1100, // Hxx
    TE_EFFECT_SET_RESONANCE = 0x1200, // Ixx
    TE_EFFECT_RESONANCE_UP = 0x1300, // Jxx
    TE_EFFECT_RESONANCE_DOWN = 0x1400, // Kxx

    TE_EFFECT_SET_ATTACK = 0x1500, // Lxx
    TE_EFFECT_SET_DECAY = 0x1600, // Mxx
    TE_EFFECT_SET_SUSTAIN = 0x1700, // Nxx
    TE_EFFECT_SET_RELEASE = 0x1800, // Oxx
    TE_EFFECT_PROGRAM_RESTART = 0x1900, // Pxx
    
    TE_EFFECT_SET_RATE = 0x1a00, //Qxx

    TE_EFFECT_SET_RING_MOD_SRC = 0x1b00, // Rxx
    TE_EFFECT_SET_HARD_SYNC_SRC = 0x1c00, // Sxx

    TE_EFFECT_PORTA_UP_SEMITONE = 0x1d00, // Txx
    TE_EFFECT_PORTA_DOWN_SEMITONE = 0x1e00, // Uxx
    TE_EFFECT_PITCH = 0x1f00, //Vxx
    /*
    TE_EFFECT_ = 0x2000, //Wxx
    */

    TE_EFFECT_LEGATO = 0x2100, // Xxx
    TE_EFFECT_ARPEGGIO_ABS = 0x2200, // Yxx
    TE_EFFECT_TRIGGER_RELEASE = 0x2300, // Zxx

    /* These effects work only in instrument program */
    TE_PROGRAM_LOOP_BEGIN = 0x7d00,
    TE_PROGRAM_LOOP_END = 0x7e00,
    TE_PROGRAM_JUMP = 0x7f00,
    TE_PROGRAM_NOP = 0x7ffe,
    TE_PROGRAM_END = 0x7fff,
} EffectCommandsOpcodes;

typedef struct {
    uint8_t a, d, s, r, volume;
} InstrumentAdsr;

typedef struct {
    char name[MUS_INST_NAME_LEN + 1];

    uint8_t waveform;
    uint16_t flags;
    uint16_t sound_engine_flags;

    uint8_t slide_speed;

    InstrumentAdsr adsr;

    uint8_t ring_mod, hard_sync; // 0xff = self

    uint8_t pw; // store only one byte since we don't have the luxury of virtually unlimited memory!

    uint16_t program
        [INST_PROG_LEN]; // MSB is unite bit (indicates this and next command must be executed at once)
    uint8_t program_period;

    uint8_t vibrato_speed, vibrato_depth, vibrato_delay;
    uint8_t pwm_speed, pwm_depth, pwm_delay;

    uint8_t filter_cutoff, filter_resonance, filter_type;

    uint8_t base_note;
    int8_t finetune;
} Instrument;

typedef struct {
    Instrument* instrument;

    uint16_t flags;

    uint8_t channel_flags;

    uint16_t note, target_note, last_note, fixed_note;
    int16_t finetune_note;
    int16_t arpeggio_note;

    uint8_t volume;

    uint8_t program_counter, program_tick, program_loop, program_period;

    uint16_t filter_cutoff, filter_resonance;
    uint8_t filter_type;

    uint8_t vibrato_speed, vibrato_depth, vibrato_delay;
    uint8_t pwm_speed, pwm_depth, pwm_delay;

    uint32_t vibrato_position, pwm_position; // basically accumulators

    uint8_t extarp1, extarp2;

    uint16_t pw;

    uint8_t slide_speed;
} TrackerEngineChannel;

typedef struct {
    uint8_t note; // MSB is used for instrument number MSB
    uint8_t inst_vol; // high nibble + MSB from note = instrument, low nibble = 4 volume LSBs
    uint16_t command; // MSB used as volume MSB
} TrackerSongPatternStep;

typedef struct {
    TrackerSongPatternStep* step;
} TrackerSongPattern;

typedef struct {
    uint8_t pattern_indices[SONG_MAX_CHANNELS];
} TrackerSongSequenceStep;

typedef struct {
    TrackerSongSequenceStep sequence_step[MAX_SEQUENCE_LENGTH];
} TrackerSongSequence;

typedef struct {
    Instrument* instrument[MAX_INSTRUMENTS];
    TrackerSongPattern pattern[MAX_PATTERNS];
    TrackerSongSequence sequence;

    uint8_t num_patterns, num_instruments;
    uint16_t num_sequence_steps;
    uint16_t pattern_length;

    char song_name[MUS_SONG_NAME_LEN + 1];
    uint8_t speed, rate;

    uint8_t loop_start, loop_end;
} TrackerSong;

typedef struct {
    TrackerEngineChannel channel[SONG_MAX_CHANNELS];

    TrackerSong* song;
    SoundEngine* sound_engine;

    uint16_t pattern_position, sequence_position;
    int16_t current_tick;
    uint16_t absolute_position; // sequence_position * pattern_length + pattern_position

    uint8_t speed, rate;
    uint8_t master_volume;

    bool playing; // if we reach the end of the song and song does not loop we just stop there

    bool in_loop; // for E6X (pattern loop) command
    uint8_t loops_left;

    // uint32_t counter; //for debug
} TrackerEngine;