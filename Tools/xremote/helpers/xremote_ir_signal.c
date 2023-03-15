#include "xremote_ir_signal.h"

#include <stdlib.h>
#include <string.h>
#include <core/check.h>
#include <infrared_worker.h>
#include <infrared_transmit.h>

#define TAG "InfraredSignal"

struct InfraredSignal {
    bool is_raw;
    union {
        InfraredMessage message;
        InfraredRawSignal raw;
    } payload;
};

static void xremote_ir_signal_clear_timings(InfraredSignal* signal) {
    if(signal->is_raw) {
        free(signal->payload.raw.timings);
        signal->payload.raw.timings_size = 0;
        signal->payload.raw.timings = NULL;
    }
}

InfraredSignal* xremote_ir_signal_alloc() {
    InfraredSignal* signal = malloc(sizeof(InfraredSignal));

    signal->is_raw = false;
    signal->payload.message.protocol = InfraredProtocolUnknown;

    return signal;
}

void xremote_ir_signal_free(InfraredSignal* signal) {
    xremote_ir_signal_clear_timings(signal);
    free(signal);
}
