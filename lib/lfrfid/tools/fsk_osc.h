#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FSKOsc FSKOsc;

FSKOsc* fsk_osc_alloc(uint32_t freq_low, uint32_t freq_hi, uint32_t osc_phase_max);

void fsk_osc_free(FSKOsc* osc);

void fsk_osc_reset(FSKOsc* osc);

bool fsk_osc_next(FSKOsc* osc, bool bit, uint32_t* period);

bool fsk_osc_next_half(FSKOsc* osc, bool bit, bool* level, uint32_t* duration);

#ifdef __cplusplus
}
#endif