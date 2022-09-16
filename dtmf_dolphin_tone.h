#pragma once
#include "tones/dtmf_dolphin_dialer_tones.h"
#include "tones/dtmf_dolphin_bluebox_tones.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DTMF_DOLPHIN_TONE_BLOCK_DIALER,
    DTMF_DOLPHIN_TONE_BLOCK_BLUEBOX,
    DTMF_DOLPHIN_TONE_BLOCK_REDBOX,
} DTMFDolphinToneSection;

const char* dtmf_dolphin_get_tone_name(uint8_t row, uint8_t col, DTMFDolphinToneSection block);

uint8_t dtmf_dolphin_get_tone_span(uint8_t row, uint8_t col, DTMFDolphinToneSection block);

void dtmf_dolphin_get_tone_frequencies(float *freq, uint8_t row, uint8_t col, DTMFDolphinToneSection block);

void dtmf_dolphin_tone_get_max_pos(uint8_t *max_rows, uint8_t *max_cols, uint8_t *max_span, DTMFDolphinToneSection block);

#ifdef __cplusplus
}
#endif
