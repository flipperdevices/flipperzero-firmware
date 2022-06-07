#include "infrared_signal.h"

#include <stdlib.h>
#include <string.h>
#include <furi/check.h>
#include <infrared_transmit.h>

struct InfraredSignal {
    bool is_raw;
    union {
        InfraredMessage message;
        InfraredRawSignal raw;
    } payload;
};

static void infrared_signal_clear_timings(InfraredSignal* signal) {
    if(signal->is_raw) {
        free(signal->payload.raw.timings);
        signal->payload.raw.timings_cnt = 0;
        signal->payload.raw.timings = NULL;
    }
}

InfraredSignal* infrared_signal_alloc() {
    InfraredSignal* signal = malloc(sizeof(InfraredSignal));

    signal->is_raw = false;
    signal->payload.message.protocol = InfraredProtocolUnknown;

    return signal;
}

void infrared_signal_free(InfraredSignal* signal) {
    infrared_signal_clear_timings(signal);
    free(signal);
}

bool infrared_signal_is_raw(InfraredSignal* signal) {
    return signal->is_raw;
}

void infrared_signal_set_raw_signal(InfraredSignal* signal, InfraredRawSignal* raw_signal) {
    infrared_signal_clear_timings(signal);

    signal->is_raw = true;

    signal->payload.raw.timings_cnt = raw_signal->timings_cnt;
    signal->payload.raw.frequency = raw_signal->frequency;
    signal->payload.raw.duty_cycle = raw_signal->duty_cycle;

    signal->payload.raw.timings = malloc(raw_signal->timings_cnt * sizeof(uint32_t));
    memcpy(
        signal->payload.raw.timings,
        raw_signal->timings,
        raw_signal->timings_cnt * sizeof(uint32_t));
}

InfraredRawSignal* infrared_signal_get_raw_signal(InfraredSignal* signal) {
    furi_assert(signal->is_raw);
    return &signal->payload.raw;
}

void infrared_signal_set_message(InfraredSignal* signal, InfraredMessage* message) {
    infrared_signal_clear_timings(signal);

    signal->is_raw = false;
    signal->payload.message = *message;
}

InfraredMessage* infrared_signal_get_message(InfraredSignal* signal) {
    furi_assert(!signal->is_raw);
    return &signal->payload.message;
}

void infrared_signal_transmit(InfraredSignal* signal) {
    if(signal->is_raw) {
        InfraredMessage* message = &signal->payload.message;
        infrared_send(message, 1);
    } else {
        InfraredRawSignal* raw_signal = &signal->payload.raw;
        infrared_send_raw_ext(
            raw_signal->timings,
            raw_signal->timings_cnt,
            true,
            raw_signal->frequency,
            raw_signal->duty_cycle);
    }
}
