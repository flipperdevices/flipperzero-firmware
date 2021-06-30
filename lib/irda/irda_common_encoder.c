#include "furi/check.h"
#include "irda_common_i.h"
#include <stdbool.h>
#include <furi.h>
#include "irda_i.h"

IrdaStatus irda_common_encode_manchester(IrdaCommonEncoder* encoder, uint32_t* duration, bool* level) {
    furi_assert(encoder);
    furi_assert(duration);
    furi_assert(level);

    const IrdaTimings* timings = &encoder->protocol->timings;
    uint8_t index = encoder->bits_encoded / 8;
    uint8_t shift = encoder->bits_encoded % 8;   // LSB first
    bool logic_value = !!(encoder->data[index] & (0x01 << shift));
    bool inverse = encoder->protocol->manchester_inverse_level;
    bool odd_timing = encoder->timings_encoded % 2;
 
    *level = odd_timing ^ logic_value ^ inverse;
    *duration = timings->bit1_mark;
    if (odd_timing)
        ++encoder->bits_encoded;
    ++encoder->timings_encoded;
    return IrdaStatusOk;
}

IrdaStatus irda_common_encode_pdwm(IrdaCommonEncoder* encoder, uint32_t* duration, bool* level) {
    furi_assert(encoder);
    furi_assert(duration);
    furi_assert(level);

    const IrdaTimings* timings = &encoder->protocol->timings;
    uint8_t index = encoder->bits_encoded / 8;
    uint8_t shift = encoder->bits_encoded % 8;   // LSB first
    bool logic_value = !!(encoder->data[index] & (0x01 << shift));

    // stop bit
    if (encoder->bits_encoded == encoder->protocol->databit_len) {
        *duration = timings->bit1_mark;
        *level = true;
        ++encoder->timings_encoded;
        return IrdaStatusDone;
    }

    if (encoder->timings_encoded % 2) {
        *duration = logic_value ? timings->bit1_space : timings->bit0_space;
    } else {
        *duration = logic_value ? timings->bit1_mark : timings->bit0_mark;
        ++encoder->bits_encoded;
    }

    *level = logic_value;
    ++encoder->timings_encoded;
    return IrdaStatusOk;
}

IrdaStatus irda_common_encode(IrdaCommonEncoder* encoder, uint32_t* duration, bool* level) {
    furi_assert(encoder);
    furi_assert(duration);
    furi_assert(level);
    IrdaStatus status = IrdaStatusOk;
    const IrdaTimings* timings = &encoder->protocol->timings;

    switch (encoder->state) {
    case IrdaCommonEncoderStatePreamble:
        furi_assert(encoder->timings_encoded < 2);
        if (timings->preamble_mark) {
            *duration = encoder->timings_encoded ? timings->preamble_space : timings->preamble_mark;
            ++encoder->timings_encoded;
            *level = encoder->timings_encoded % 2;
            break;
        }
        encoder->state = IrdaCommonEncoderStateEncode;
        /* FALLTHROUGH */
    case IrdaCommonEncoderStateEncode:
        furi_assert(encoder->timings_encoded >= 2); // tmp
        status = encoder->protocol->encode(encoder, duration, level);
        if (status == IrdaStatusDone) {
            if (encoder->protocol->encode_repeat) {
                encoder->state = IrdaCommonEncoderStateEncodeRepeat;
            } else {
                encoder->state = IrdaCommonEncoderStateEncodeSpace;
            }
        }
        break;
    case IrdaCommonEncoderStateEncodeRepeat:
        status = encoder->protocol->encode_repeat(encoder, duration, level);
        break;
    case IrdaCommonEncoderStateEncodeSpace:
        *duration = encoder->protocol->timings.silence_time ? encoder->protocol->timings.silence_time : 0;
        *level = false;
        status = IrdaStatusDone;
        encoder->state = IrdaCommonEncoderStatePreamble;
        break;
    }   
    return status;
}

void* irda_common_encoder_alloc(const IrdaCommonProtocolSpec* protocol) {
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
    IrdaCommonDecoder* common_decoder = decoder;
    common_decoder->context = context;
}

