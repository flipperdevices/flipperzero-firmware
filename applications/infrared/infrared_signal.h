#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <infrared.h>

typedef struct InfraredSignal InfraredSignal;

InfraredSignal* infrared_signal_alloc();
void infrared_signal_free(InfraredSignal* signal);

bool infrared_signal_is_raw(InfraredSignal* signal);
void infrared_signal_set_raw_signal(InfraredSignal* signal, uint32_t* timings, size_t timings_count, uint32_t frequency, float duty_cycle);
void infrared_signal_set_message(InfraredSignal* signal, InfraredMessage* message);
