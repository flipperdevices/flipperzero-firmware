#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <infrared.h>
#include <flipper_format/flipper_format.h>

typedef struct InfraredSignal InfraredSignal;

typedef struct {
    size_t timings_size;
    uint32_t* timings;
    uint32_t frequency;
    float duty_cycle;
} InfraredRawSignal;

InfraredSignal* xremote_ir_signal_alloc();
void xremote_ir_signal_free(InfraredSignal* signal);

bool xremote_ir_signal_is_raw(InfraredSignal* signal);
bool xremote_ir_signal_is_valid(InfraredSignal* signal);

void xremote_ir_signal_set_signal(InfraredSignal* signal, const InfraredSignal* other);

void xremote_ir_signal_set_raw_signal(
    InfraredSignal* signal, 
    const uint32_t* timings, 
    size_t timings_size, 
    uint32_t frequency, 
    float duty_cycle);
InfraredRawSignal* xremote_ir_signal_get_raw_signal(InfraredSignal* signal);

void xremote_ir_signal_set_message(InfraredSignal* signal, const InfraredMessage* message);
InfraredMessage* xremote_ir_signal_get_message(InfraredSignal* signal);

bool xremote_ir_signal_read(InfraredSignal* signal, FlipperFormat* ff, FuriString* name);
