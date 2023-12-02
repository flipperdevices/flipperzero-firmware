#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define NUM_CHANNELS 4

#define RANDOM_SEED 0xf31782ce

#define ACC_BITS 23
#define ACC_LENGTH (1 << (ACC_BITS - 1))

#define OUTPUT_BITS 16
#define WAVE_AMP (1 << OUTPUT_BITS)

#define SINE_LUT_SIZE 256
#define SINE_LUT_BITDEPTH 8

#define MAX_ADSR (0xff << 17)
#define MAX_ADSR_VOLUME 0x80
#define BASE_FREQ 22050
#define envspd(eng, slope)                                                                     \
    ((slope) != 0 ?                                                                            \
         (((uint64_t)MAX_ADSR / ((slope) * (slope)*256 / 8)) * BASE_FREQ / eng->sample_rate) : \
         ((uint64_t)MAX_ADSR * BASE_FREQ / eng->sample_rate))

typedef enum {
    SE_WAVEFORM_NONE = 0,
    SE_WAVEFORM_NOISE = 1,
    SE_WAVEFORM_PULSE = 2,
    SE_WAVEFORM_TRIANGLE = 4,
    SE_WAVEFORM_SAW = 8,
    SE_WAVEFORM_NOISE_METAL = 16,
    SE_WAVEFORM_SINE = 32,
} SoundEngineWaveformType;

typedef enum {
    SE_ENABLE_FILTER = 1,
    SE_ENABLE_GATE = 2,
    SE_ENABLE_RING_MOD = 4,
    SE_ENABLE_HARD_SYNC = 8,
    SE_ENABLE_KEYDOWN_SYNC = 16, // sync oscillators on keydown
} SoundEngineFlags;

typedef enum {
    FIL_OUTPUT_LOWPASS = 1,
    FIL_OUTPUT_HIGHPASS = 2,
    FIL_OUTPUT_BANDPASS = 3,
    FIL_OUTPUT_LOW_HIGH = 4,
    FIL_OUTPUT_HIGH_BAND = 5,
    FIL_OUTPUT_LOW_BAND = 6,
    FIL_OUTPUT_LOW_HIGH_BAND = 7,
    /* ============ */
    FIL_MODES = 8,
} SoundEngineFilterModes;

typedef enum {
    ATTACK = 1,
    DECAY = 2,
    SUSTAIN = 3,
    RELEASE = 4,
    DONE = 5,
} SoundEngineEnvelopeStates;

typedef struct {
    uint8_t a, d, s, r, volume, envelope_state;
    uint32_t envelope, envelope_speed;
} SoundEngineADSR;

typedef struct {
    int32_t cutoff, resonance, low, high, band;
} SoundEngineFilter;

typedef struct {
    uint32_t accumulator;
    uint32_t frequency;
    uint8_t waveform;
    uint16_t pw;
    uint32_t lfsr;
    SoundEngineADSR adsr;

    uint16_t flags;

    uint8_t ring_mod, hard_sync; // 0xff = self
    uint8_t sync_bit;

    uint8_t filter_mode;

    SoundEngineFilter filter;
} SoundEngineChannel;

typedef struct {
    SoundEngineChannel channel[NUM_CHANNELS];
    uint32_t sample_rate;
    uint16_t* audio_buffer;
    uint32_t audio_buffer_size;
    bool external_audio_output;
    uint8_t sine_lut[SINE_LUT_SIZE];

    // uint32_t counter; //for debug
} SoundEngine;