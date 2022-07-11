#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct FSKOsc FSKOsc;

FSKOsc* fsk_osc_alloc(uint32_t freq_low, uint32_t freq_hi, uint32_t osc_phase_max);

void fsk_osc_free(FSKOsc* osc);

bool fsk_osc_next(FSKOsc* osc, bool bit, uint32_t* period);