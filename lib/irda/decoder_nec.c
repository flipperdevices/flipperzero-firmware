#include <stdbool.h>
#include <stdint.h>
#include <furi.h>
#include "decoder_common_i.h"


#define NEC_REPEAT_DELAY_MIN            (30000)
#define NEC_REPEAT_DELAY_MAX            (150000)
#define NEC_REPEAT_MARK                 (9000)
#define NEC_REPEAT_SPACE                (2250)


static bool interpret_nec(IrdaCommonDecoder* decoder);
static DecodeStatus decode_repeat_nec(IrdaCommonDecoder* decoder);


static const IrdaCommonProtocolSpec protocol_nec = {
    "NEC",
    { 9000, 4500, 560, 1600, 560, 560 },
    32,
    decode_pwm,
    interpret_nec,
    decode_repeat_nec,
};

/***************************************************************************************************
*   NEC protocol description
*   https://radioparty.ru/manuals/encyclopedia/213-ircontrol?start=1
****************************************************************************************************
*     Preamble   Preamble      Pulse Distance/Width          Pause       Preamble   Preamble  Stop
*       mark      space          Modulation                               repeat     repeat    bit
*                                                                          mark       space
*
*        9000      4500       32 bit + stop bit           40000-100000     9000       2250
*     __________          _ _ _ _  _  _  _ _ _  _  _ _ _                ___________            _
* ____          __________ _ _ _ __ __ __ _ _ __ __ _ _ ________________           ____________ ___
*
***************************************************************************************************/

static bool interpret_nec(IrdaCommonDecoder* decoder) {
    furi_assert(decoder);

    bool result = false;
    uint8_t address = decoder->data[0];
    uint8_t addressInverted = decoder->data[1];
    uint8_t command = decoder->data[2];
    uint8_t commandInverted = decoder->data[3];

    if ((command == (uint8_t) ~commandInverted) && (address == (uint8_t) ~addressInverted)) {
        decoder->message.command = command;
        decoder->message.address = address;
        decoder->message.protocol_name = decoder->protocol->name;
        decoder->message.repeat = false;
        result = true;
    }

    return result;
}

// timings start from Space (delay between message and repeat)
static DecodeStatus decode_repeat_nec(IrdaCommonDecoder* decoder) {
    furi_assert(decoder);

    DecodeStatus status = DecodeStatusError;

    if (decoder->timings_cnt < 4)
        return DecodeStatusOk;

    if ((decoder->timings[0] > NEC_REPEAT_DELAY_MIN)
        && (decoder->timings[0] < NEC_REPEAT_DELAY_MAX)
        && MATCH_PREAMBLE_TIMING(decoder->timings[1], NEC_REPEAT_MARK)
        && MATCH_PREAMBLE_TIMING(decoder->timings[2], NEC_REPEAT_SPACE)
        && MATCH_PREAMBLE_TIMING(decoder->timings[3], decoder->protocol->timings.bit1_mark)) {
        status = DecodeStatusReady;
        decoder->timings_cnt = 0;
    } else {
        status = DecodeStatusError;
    }

    return status;
}

const IrdaMessage* decode_nec(void *decoder, bool level, uint32_t duration) {
    return decode_common(decoder, level, duration);
}

void* init_nec(void) {
    return common_decoder_init(&protocol_nec);
}

void fini_nec(void* decoder) {
    common_decoder_fini(decoder);
}
