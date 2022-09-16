#include "dtmf_dolphin_tone.h"

const char* dtmf_dolphin_get_tone_name(uint8_t row, uint8_t col, DTMFDolphinToneSection block) {
    if (block == DTMF_DOLPHIN_TONE_BLOCK_DIALER) {
        for (int i = 0; i < DTMF_DOLPHIN_DIALER_TONE_COUNT; i++) {
            if (dtmf_dolphin_dialer_tone[i].pos.row == row && dtmf_dolphin_dialer_tone[i].pos.col == col) {
                return dtmf_dolphin_dialer_tone[i].name;
            }
        }
    } else if (block == DTMF_DOLPHIN_TONE_BLOCK_BLUEBOX) {
        // if (index < DTMF_DOLPHIN_BLUEBOX_TONE_COUNT) {
        //     return dtmf_dolphin_bluebox_tone[index].name;
        // }
    }
    return "N";
}

uint8_t dtmf_dolphin_get_tone_span(uint8_t row, uint8_t col, DTMFDolphinToneSection block) {
    if (block == DTMF_DOLPHIN_TONE_BLOCK_DIALER) {
        for (int i = 0; i < DTMF_DOLPHIN_DIALER_TONE_COUNT; i++) {
            if (dtmf_dolphin_dialer_tone[i].pos.row == row && dtmf_dolphin_dialer_tone[i].pos.col == col) {
                return dtmf_dolphin_dialer_tone[i].pos.span;
            }
        }
    } else if (block == DTMF_DOLPHIN_TONE_BLOCK_BLUEBOX) {
        // if (index < DTMF_DOLPHIN_BLUEBOX_TONE_COUNT) {
        //     return dtmf_dolphin_bluebox_tone[index].name;
        // }
    }
    return 1; // Default to 1
}

void dtmf_dolphin_get_tone_frequencies(float *freq, uint8_t row, uint8_t col, DTMFDolphinToneSection block) {
    freq[0] = 0;
    freq[1] = 0;

    if (block == DTMF_DOLPHIN_TONE_BLOCK_DIALER) {
        for (int i = 0; i < DTMF_DOLPHIN_DIALER_TONE_COUNT; i++) {
            if (dtmf_dolphin_dialer_tone[i].pos.row == row && dtmf_dolphin_dialer_tone[i].pos.col == col) {
                freq[0] = dtmf_dolphin_dialer_tone[i].frequency_1;
                freq[1] = dtmf_dolphin_dialer_tone[i].frequency_2;
            }
        }
    } else if (block == DTMF_DOLPHIN_TONE_BLOCK_BLUEBOX) {
        // if (index < DTMF_DOLPHIN_BLUEBOX_TONE_COUNT) {
        //     return dtmf_dolphin_bluebox_tone[index].name;
        // }
    }
}

void dtmf_dolphin_tone_get_max_pos(uint8_t *max_rows, uint8_t *max_cols, uint8_t *max_span, DTMFDolphinToneSection block) {
    max_rows[0] = 0;
    max_cols[0] = 0;
    max_span[0] = 0;
    uint8_t span[8] = { 0 };
    if (block == DTMF_DOLPHIN_TONE_BLOCK_DIALER) {
        for (int i = 0; i < DTMF_DOLPHIN_DIALER_TONE_COUNT; i++) {
            if (dtmf_dolphin_dialer_tone[i].pos.row > max_rows[0])
                max_rows[0] = dtmf_dolphin_dialer_tone[i].pos.row;
            if (dtmf_dolphin_dialer_tone[i].pos.col > max_cols[0])
                max_cols[0] = dtmf_dolphin_dialer_tone[i].pos.col;
            span[dtmf_dolphin_dialer_tone[i].pos.row] += dtmf_dolphin_dialer_tone[i].pos.span;
        }
        max_rows[0]++;
        max_cols[0]++;
        for (int i = 0; i < max_rows[0]; i++) {
            if (span[i] > max_span[0])
                max_span[0] = span[i];
        }
    } else if (block == DTMF_DOLPHIN_TONE_BLOCK_BLUEBOX) {
        // if (index < DTMF_DOLPHIN_BLUEBOX_TONE_COUNT) {
        // for (int i; i < DTMF_DOLPHIN_BLUEBOX_TONE_COUNT; i++) {
        //     if (dtmf_dolphin_bluebox_tone[i].pos.row > max_rows)
        //         max_rows = dtmf_dolphin_bluebox_tone[i].pos.row;
        //     if (dtmf_dolphin_bluebox_tone[i].pos.col > max_cols)
        //         max_cols = dtmf_dolphin_bluebox_tone[i].pos.col;
        // }
    }
}

// void dtmf_dolphin_bluebox_generate(uint8_t index, uint8_t *buffer) {

//     // TODO: Generate the waveform
// }
