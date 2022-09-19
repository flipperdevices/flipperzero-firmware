#pragma once
// #include "dtmf_dolphin_i.h"
#include "dtmf_dolphin_event.h"
#include "dtmf_dolphin_hal.h"

#define SAMPLE_BUFFER_LENGTH 8192
#define PERIOD_2_PI 6.2832
#define CPU_CLOCK_FREQ 64000000

typedef struct {
    float cached_freq;
    size_t period;
    float* lookup_table;
    uint16_t offset;
} DTMFDolphinOsc;

typedef struct {
    size_t buffer_length;
    size_t half_buffer_length;
    uint8_t *buffer_buffer;
    uint16_t *sample_buffer;
    float volume;
    FuriMessageQueue *queue;
    DTMFDolphinOsc *osc1;
    DTMFDolphinOsc *osc2;
} DTMFDolphinAudio;

DTMFDolphinOsc* dtmf_dolphin_osc_alloc();

DTMFDolphinAudio* dtmf_dolphin_audio_alloc();

void dtmf_dolphin_audio_free(DTMFDolphinAudio* player);

void dtmf_dolphin_osc_free(DTMFDolphinOsc* osc);

bool dtmf_dolphin_audio_play_tones(float freq1, float freq2);

bool dtmf_dolphin_audio_stop_tones();

bool dtmf_dolphin_audio_handle_tick();
