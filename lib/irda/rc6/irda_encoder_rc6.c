#include "furi/memmgr.h"
#include "irda.h"
#include "irda_common_i.h"
#include "irda_protocol_defs_i.h"
#include <stdint.h>
#include "../irda_i.h"

typedef struct IrdaEncoderRC6 {
    IrdaCommonEncoder* common_encoder;
    bool toggle_bit;
} IrdaEncoderRC6;

void irda_encoder_rc6_reset(void* encoder_ptr, const IrdaMessage* message) {
    furi_assert(encoder_ptr);

    IrdaEncoderRC6* encoder = encoder_ptr;
    IrdaCommonEncoder* common_encoder = encoder->common_encoder;
    irda_common_encoder_reset(common_encoder);

    uint32_t* data = (void*) common_encoder->data;
    *data |= 0x01;    // start bit
    (void) *data;     // 3 bits for mode == 0
    *data |= encoder->toggle_bit ? 0x10 : 0;
    *data |= reverse(message->address) << 5;
    *data |= reverse(message->command) << 13;
}

IrdaStatus irda_encoder_rc6_encode(void* encoder_ptr, uint32_t* out, bool* polarity) {
    IrdaCommonEncoder* common_encoder = encoder_ptr;
    IrdaEncoderRC6* encoder_rc6 = common_encoder->context;
    IrdaStatus status = IrdaStatusError;

    bool toggle_bit = (common_encoder->bits_encoded == 4);
    status = irda_common_encode_manchester(common_encoder, out, polarity);
    if (toggle_bit)
        *out *= 2;
    
    if (status == IrdaStatusDone)
        encoder_rc6->toggle_bit ^= 1;
    return status;
}

void* irda_encoder_rc6_alloc(void) {
    IrdaEncoderRC6* encoder = furi_alloc(sizeof(IrdaEncoderRC6));
    encoder->common_encoder = irda_common_encoder_alloc(&protocol_rc6);
    irda_common_encoder_set_context(encoder->common_encoder, encoder);
    encoder->toggle_bit = false;
    return encoder;
}

void irda_encoder_rc6_free(void* encoder_ptr) {
    furi_assert(encoder_ptr);

    IrdaEncoderRC6* encoder = encoder_ptr;
    free(encoder->common_encoder);
    free(encoder);
}

