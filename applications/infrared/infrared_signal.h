#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <infrared.h>

typedef struct InfraredSignal InfraredSignal;

typedef struct {
    size_t timings_cnt;
    uint32_t* timings;
    uint32_t frequency;
    float duty_cycle;
} InfraredRawSignal;

InfraredSignal* infrared_signal_alloc();
void infrared_signal_free(InfraredSignal* signal);

bool infrared_signal_is_raw(InfraredSignal* signal);

void infrared_signal_set_raw_signal(InfraredSignal* signal, InfraredRawSignal* raw_signal);
InfraredRawSignal* infrared_signal_get_raw_signal(InfraredSignal* signal);

void infrared_signal_set_message(InfraredSignal* signal, InfraredMessage* message);
InfraredMessage* infrared_signal_get_message(InfraredSignal* signal);

void infrared_signal_transmit(InfraredSignal* signal);
