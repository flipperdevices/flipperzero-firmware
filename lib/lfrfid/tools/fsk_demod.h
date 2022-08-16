#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FSKDemod FSKDemod;

FSKDemod*
    fsk_demod_alloc(uint32_t low_time, uint32_t low_pulses, uint32_t hi_time, uint32_t hi_pulses);

void fsk_demod_free(FSKDemod* fsk_demod);

void fsk_demod_feed(FSKDemod* demod, bool polarity, uint32_t time, bool* value, uint32_t* count);

#ifdef __cplusplus
}
#endif
