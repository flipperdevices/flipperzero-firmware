#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* helper for easier signal generation */
#define DIGITAL_SIGNAL_MS(x) ((x)*100000000UL)
#define DIGITAL_SIGNAL_US(x) ((x)*100000UL)
#define DIGITAL_SIGNAL_NS(x) ((x)*100UL)
#define DIGITAL_SIGNAL_PS(x) ((x) / 10UL)

typedef struct DigitalSignal DigitalSignal;

DigitalSignal* digital_signal_alloc(uint32_t max_size);

void digital_signal_free(DigitalSignal* signal);

void digital_signal_add_period(DigitalSignal* signal, uint32_t ticks);

void digital_signal_add_period_with_level(DigitalSignal* signal, uint32_t ticks, bool level);

bool digital_signal_get_start_level(const DigitalSignal* signal);

void digital_signal_set_start_level(DigitalSignal* signal, bool level);

uint32_t digital_signal_get_size(const DigitalSignal* signal);

#ifdef __cplusplus
}
#endif
