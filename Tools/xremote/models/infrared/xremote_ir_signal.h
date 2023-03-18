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
bool xremote_ir_signal_read(InfraredSignal* signal, FlipperFormat* ff, FuriString* name);
