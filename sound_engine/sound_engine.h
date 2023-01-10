#pragma once

#include <stdio.h>
#include <stdbool.h>

#define NUM_CHANNELS 4

#define RANDOM_SEED 0xf31782ce

#define ACC_BITS 23
#define ACC_LENGTH (1 << (ACC_BITS - 1))

#define OUTPUT_BITS 16
#define WAVE_AMP (1 << OUTPUT_BITS)

typedef enum
{
	SE_WAVEFORM_NONE = 0,
	SE_WAVEFORM_NOISE = 1,
	SE_WAVEFORM_PULSE = 2,
	SE_WAVEFORM_TRIANGLE = 4,
	SE_WAVEFORM_SAW = 8,
} SoudEngineWaveformType;

typedef struct
{
	uint8_t a, d, s, r, volume;
	uint32_t envelope, envelope_speed;
} SoundEngineADSR;

typedef struct
{
	uint32_t accumulator;
	uint32_t frequency;
	uint8_t waveform;
	uint16_t pw;
	uint32_t lfsr;
	SoundEngineADSR adsr;
} SoundEngineChannel;

typedef struct
{
	SoundEngineChannel channel[NUM_CHANNELS];
	uint32_t sample_rate;
	uint16_t* audio_buffer;
	uint32_t audio_buffer_size;
	bool external_audio_output;
} SoundEngine;

void sound_engine_init(SoundEngine* sound_engine, uint32_t sample_rate, bool external_audio_output, uint32_t audio_buffer_size);
void sound_engine_deinit(SoundEngine* sound_engine);
void sound_engine_set_channel_frequency(SoundEngine* sound_engine, SoundEngineChannel* channel, uint32_t frequency);
void sound_engine_fill_buffer(SoundEngine* sound_engine, uint16_t* audio_buffer, uint32_t audio_buffer_size);