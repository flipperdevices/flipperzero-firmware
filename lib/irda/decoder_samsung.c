#include <stdbool.h>
#include <stdint.h>
#include <furi.h>
#include "decoder_common_i.h"
#include "protocol_defs_i.h"


static bool interpret(IrdaCommonDecoder* decoder);
static DecodeStatus decode_repeat(IrdaCommonDecoder* decoder);


static const IrdaCommonProtocolSpec protocol_samsung32 = {
    {
        IRDA_SAMSUNG_PREAMBULE_MARK,
        IRDA_SAMSUNG_PREAMBULE_SPACE,
        IRDA_SAMSUNG_BIT1_MARK,
        IRDA_SAMSUNG_BIT1_SPACE,
        IRDA_SAMSUNG_BIT0_MARK,
        IRDA_SAMSUNG_BIT0_SPACE
    },
    32,
    decode_pwm,
    interpret,
    decode_repeat,
};


static bool interpret(IrdaCommonDecoder* decoder) {
    furi_assert(decoder);

    bool result = false;
    uint8_t address1 = decoder->data[0];
    uint8_t address2 = decoder->data[1];
    uint8_t command = decoder->data[2];
    uint8_t command_inverse = decoder->data[3];

    if ((address1 == address2) && (command == (uint8_t) ~command_inverse)) {
        decoder->message.command = command;
        decoder->message.address = address1;
        decoder->message.repeat = false;
        result = true;
    }

    return result;
}

// timings start from Space (delay between message and repeat)
static DecodeStatus decode_repeat(IrdaCommonDecoder* decoder) {
    furi_assert(decoder);

    DecodeStatus status = DecodeStatusError;

    if (decoder->timings_cnt < 6)
        return DecodeStatusOk;

    if ((decoder->timings[0] > IRDA_SAMSUNG_REPEAT_PAUSE_MIN)
        && (decoder->timings[0] < IRDA_SAMSUNG_REPEAT_PAUSE_MAX)
        && MATCH_PREAMBLE_TIMING(decoder->timings[1], IRDA_SAMSUNG_REPEAT_MARK)
        && MATCH_PREAMBLE_TIMING(decoder->timings[2], IRDA_SAMSUNG_REPEAT_SPACE)
        && MATCH_BIT_TIMING(decoder->timings[3], decoder->protocol->timings.bit1_mark)
        && MATCH_BIT_TIMING(decoder->timings[4], decoder->protocol->timings.bit1_space)
        && MATCH_BIT_TIMING(decoder->timings[5], decoder->protocol->timings.bit1_mark)
        ) {
        status = DecodeStatusReady;
        decoder->timings_cnt = 0;
    } else {
        status = DecodeStatusError;
    }

    return status;
}

void* init_samsung32(void) {
    return common_decoder_init(&protocol_samsung32);
}

IrdaMessage* decode_samsung32(void* decoder, bool level, uint32_t duration) {
    return decode_common(decoder, level, duration);
}

void fini_samsung32(void* decoder) {
    common_decoder_fini(decoder);
}

