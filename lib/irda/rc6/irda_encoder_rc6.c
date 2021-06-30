#include "furi/memmgr.h"
#include "irda.h"
#include <stdint.h>
#include "../irda_i.h"

IrdaStatus irda_encoder_rc6_encode(void* encoder_ptr, uint32_t* out, bool* polarity) {
    IrdaCommonEncoder* encoder = encoder_ptr;
    IrdaStatus status = IrdaStatusError;

    bool toggle_bit = (encoder->bits_encoded == 4);
    status = irda_common_encode_manchester(encoder, out, polarity);
    if (toggle_bit)
        *out *= 2;

    return status;
}

void* irda_encoder_rc6_alloc(void) {
    furi_assert(protocol);

    uint32_t alloc_size = sizeof(IrdaCommonEncoder)
                          + protocol->databit_len / 8
                          + !!(protocol->databit_len % 8);
    IrdaCommonEncoder* encoder = furi_alloc(alloc_size);
    memset(encoder, 0, alloc_size);
    encoder->protocol = protocol;

    return encoder;
}

void irda_common_encoder_free(IrdaCommonEncoder* encoder) {
    furi_assert(encoder);
    free(encoder);
}

void irda_common_encoder_reset(IrdaCommonEncoder* encoder) {
    furi_assert(encoder);
    encoder->timings_encoded = 0;
    encoder->bits_encoded = 0;
    encoder->state = IrdaCommonEncoderStatePreamble;
}

void irda_common_encoder_set_context(void* decoder, void* context) {
