#include <stdlib.h>
#include <string.h>
#include <core/check.h>
#include <infrared_worker.h>
#include <infrared_transmit.h>

#include "xremote_ir_signal.h"

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

static bool xremote_ir_signal_read_body(InfraredSignal* signal, FlipperFormat* ff) {
    UNUSED(signal);
    FuriString* tmp = furi_string_alloc();

    bool success = false;

    do {
        if(!flipper_format_read_string(ff, "type", tmp)) break;
        if(furi_string_equal(tmp, "raw")) {
            //success = 
            success = true;
        } else if(furi_string_equal(tmp, "parsed")) {
            //success =
            success = true;
        } else {
            FURI_LOG_E(TAG, "Unknown signal type");
        }
    } while(false);

    furi_string_free(tmp);
    return success;
}

bool xremote_ir_signal_read(InfraredSignal* signal, FlipperFormat* ff, FuriString* name) {
    FuriString* tmp = furi_string_alloc();

    bool success = false;

    do {
        if(!flipper_format_read_string(ff, "name", tmp)) break;
        furi_string_set(name, tmp);
        if(!xremote_ir_signal_read_body(signal, ff)) break;
        success = true;
    } while(0);

    furi_string_free(tmp);
    return success;
}
