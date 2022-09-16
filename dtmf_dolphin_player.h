#pragma once
#include "dtmf_dolphin_event.h"
#include "dtmf_dolphin_hal.h"
#include "dtmf_dolphin_tone.h"

#define PERIOD_2_PI 6.2832

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    size_t half_samples;
    size_t sample_count;
    float wf1_freq;
    float wf2_freq;
    uint16_t wf1_period;
    uint16_t wf2_period;
    uint16_t wf1_pos;
    uint16_t wf2_pos;
    uint8_t *buffer_buffer;
    uint16_t *sample_buffer;
    float volume;
    bool playing;

    FuriMessageQueue* queue;
} DTMFDolphinPlayer;

void dtmf_dolphin_dma_isr(void* ctx);

bool dtmf_dolphin_player_init(void* context);

void dtmf_dolphin_player_clear_samples();

bool dtmf_dolphin_player_generate_waveform(size_t index);

bool dtmf_dolphin_player_play_tones(float *freq);

bool dtmf_dolphin_player_stop_tones();

bool dtmf_dolphin_player_handle_tick();

#ifdef __cplusplus
}
#endif


