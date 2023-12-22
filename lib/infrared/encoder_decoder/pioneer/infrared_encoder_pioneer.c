#include "infrared_protocol_pioneer_i.h"
#include <core/check.h>

void infrared_encoder_pioneer_reset(void* encoder_ptr, const InfraredMessage* message) {
    furi_assert(encoder_ptr);
    furi_assert(message);

    InfraredCommonEncoder* encoder = encoder_ptr;
    infrared_common_encoder_reset(encoder);

    uint8_t* data = encoder->data;

    if(message->protocol == InfraredProtocolPIONEER) {
        data[0] = message->address & 0xFF;
        data[1] = ~(message->address & 0xFF);
        data[2] = message->command & 0xFF;
        data[3] = ~(message->command & 0xFF);
        data[4] = 0;
        encoder->bits_to_encode = 33;
    } else {
        furi_crash();
    }
}

void* infrared_encoder_pioneer_alloc(void) {
    return infrared_common_encoder_alloc(&infrared_protocol_pioneer);
}

void infrared_encoder_pioneer_free(void* encoder_ptr) {
    infrared_common_encoder_free(encoder_ptr);
}

InfraredStatus infrared_encoder_pioneer_encode(void* encoder_ptr, uint32_t* duration, bool* level) {
    InfraredCommonEncoder* encoder = encoder_ptr;

    InfraredStatus status = infrared_common_encode(encoder, duration, level);
    if((status == InfraredStatusOk) && (encoder->bits_encoded == encoder->bits_to_encode)) {
        furi_assert(!*level);
        status = InfraredStatusDone;
        encoder->state = InfraredCommonEncoderStateSilence;
    }
    return status;
}
