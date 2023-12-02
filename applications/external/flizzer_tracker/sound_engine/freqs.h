#pragma once

#include <furi.h>
#include <stdio.h>

#define FREQ_TAB_SIZE 12 /* one octave */
#define NUM_OCTAVES 8 /* 0-7th octaves */

extern const uint32_t frequency_table[FREQ_TAB_SIZE];

uint32_t get_freq(uint16_t note);