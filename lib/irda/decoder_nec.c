#include <stdbool.h>
#include <stdint.h>
#include "decoder_common_i.h"


#define NEC_REPEAT_DELAY_MIN            (30000)
#define NEC_REPEAT_DELAY_MAX            (150000)
#define NEC_REPEAT_MARK                 (9000)
#define NEC_REPEAT_SPACE                (2250)


static bool interpret_nec(IrdaCommonDecoder* d);
static DecodeStatus decode_repeat_nec(IrdaCommonDecoder* d);


static IrdaCommonProtocolSpec protocol_nec = {
    "NEC",
    { 9000, 4500, 560, 1600, 560, 560 },
    32,
    decode_pwm,
    interpret_nec,
    decode_repeat_nec,
};


static bool interpret_nec(IrdaCommonDecoder* d) {
    bool rc = false;
    uint8_t adr = d->data[0];
    uint8_t adrInv = d->data[1];
    uint8_t cmd = d->data[2];
    uint8_t cmdInv = d->data[3];

    if ((cmd == (uint8_t) ~cmdInv) && (adr == (uint8_t) ~adrInv)) {
        d->im.cmd = cmd;
        d->im.adr = adr;
        d->im.protocol_name = d->spec->name;
        d->im.repeat = false;
        rc = true;
    }

    return rc;
}

// timings start from Space (delay between message and repeat)
static DecodeStatus decode_repeat_nec(IrdaCommonDecoder* d) {
    DecodeStatus status = DecodeStatusError;

    if (d->tc < 4)
        return DecodeStatusOk;

    if (((d->t[0] > NEC_REPEAT_DELAY_MIN) && (d->t[0] < NEC_REPEAT_DELAY_MAX))
        && MATCH_PREAMBLE_TIMING(d->t[1], NEC_REPEAT_MARK)
        && MATCH_PREAMBLE_TIMING(d->t[2], NEC_REPEAT_SPACE)
        && MATCH_PREAMBLE_TIMING(d->t[3], d->spec->timings.bit1_mark)) {
        status = DecodeStatusReady;
        d->tc = 0;
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

void fini_nec(void* d) {
    common_decoder_fini(d);
}
