// Methods for IR transmission

#include <infrared.h>
#include <infrared/encoder_decoder/infrared.h>
#include <applications/services/cli/cli.h>

#include "action_i.h"
#include "quac.h"

typedef struct {
    size_t timings_size; /**< Number of elements in the timings array. */
    uint32_t* timings; /**< Pointer to an array of timings describing the signal. */
    uint32_t frequency; /**< Carrier frequency of the signal. */
    float duty_cycle; /**< Duty cycle of the signal. */
} InfraredRawSignal;

typedef struct InfraredSignal {
    bool is_raw;
    union {
        InfraredMessage message;
        InfraredRawSignal raw;
    } payload;
} InfraredSignal;

InfraredSignal* infrared_signal_alloc() {
    InfraredSignal* signal = malloc(sizeof(InfraredSignal));
    signal->is_raw = false;
    signal->payload.message.protocol = InfraredProtocolUnknown;
    return signal;
}

void action_ir_tx(void* context, const FuriString* action_path, FuriString* error) {
    UNUSED(action_path);
    UNUSED(error);
    UNUSED(context);
    // App* app = context;

    // InfraredSignal* signal = infrared_signal_alloc();
    // const char* ir_file = furi_string_get_cstr(action_path);
    // bool success = infrared_parse_message(ir_file, signal) || infrared_parse_raw(ir_file, signal);
    // if(success) {
    //     infrared_signal_transmit(signal);
    // } else {
    //     ACTION_SET_ERROR("IR: Error sending signal");
    // }
    // infrared_signal_free(signal);
}