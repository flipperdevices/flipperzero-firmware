#pragma once

#include <stdio.h>
#include <stdbool.h>

#define NUM_CHANNELS 4

#define RANDOM_SEED 0xf31782ce

#define ACC_BITS 23
#define ACC_LENGTH (1 << (ACC_BITS - 1))

#define OUTPUT_BITS 16
#define WAVE_AMP (1 << OUTPUT_BITS)

#define SINE_LUT_SIZE 256
#define SINE_LUT_BITDEPTH 8

typedef enum
{
	SE_WAVEFORM_NONE = 0,
	SE_WAVEFORM_NOISE = 1,
	SE_WAVEFORM_PULSE = 2,
	SE_WAVEFORM_TRIANGLE = 4,
	SE_WAVEFORM_SAW = 8,
	SE_WAVEFORM_NOISE_METAL = 16,
	SE_WAVEFORM_SINE = 32,
} SoudEngineWaveformType;

typedef enum
{
	SE_ENABLE_FILTER = 1,
	SE_ENABLE_GATE = 2,
	SE_ENABLE_RING_MOD = 4,
	SE_ENABLE_HARD_SYNC = 8,
	SE_ENABLE_KEYDOWN_SYNC = 16, //sync oscillators on keydown
} SoundEngineFlags;

typedef enum
{
	FIL_OUTPUT_LOWPASS = 1,
	FIL_OUTPUT_HIGHPASS = 2,
	FIL_OUTPUT_BANDPASS = 3,
} SoundEngineFilterModes;

typedef struct
{
	uint8_t a, d, s, r, volume;
	uint32_t envelope, envelope_speed;
} SoundEngineADSR;

typedef struct
{
	int32_t f, q, p;
	int32_t b0, b1, b2, b3, b4; //filter coefficients
} SoundEngineFilter;

typedef struct
{
	uint32_t accumulator;
	uint32_t frequency;
	uint8_t waveform;
	uint16_t pw;
	uint32_t lfsr;
	SoundEngineADSR adsr;

	uint16_t flags;

	uint8_t filter_mode;
	uint16_t filter_cutoff;
	uint16_t filter_resonace;

	SoundEngineFilter filter;
} SoundEngineChannel;

typedef struct
{
	SoundEngineChannel channel[NUM_CHANNELS];
	uint32_t sample_rate;
	uint16_t* audio_buffer;
	uint32_t audio_buffer_size;
	bool external_audio_output;
	uint8_t sine_lut[SINE_LUT_SIZE];
} SoundEngine;

void sound_engine_init(SoundEngine* sound_engine, uint32_t sample_rate, bool external_audio_output, uint32_t audio_buffer_size);
void sound_engine_deinit(SoundEngine* sound_engine);
void sound_engine_set_channel_frequency(SoundEngine* sound_engine, SoundEngineChannel* channel, uint32_t frequency);
void sound_engine_fill_buffer(SoundEngine* sound_engine, uint16_t* audio_buffer, uint32_t audio_buffer_size);
void sound_engine_filter_set_coeff(SoundEngineFilter *flt, uint32_t frequency, uint16_t resonance);