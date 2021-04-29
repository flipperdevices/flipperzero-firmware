#include <malloc.h>
#include "libirda.h"


#define IRDA_MAX_DT_US          (200*1000)
#define DECODER_BUF_MAX_SIZE    128
#define TOLERANCE               0.07
#define MATCH_TIMING(x, v)      (((x) < (v)*(1+TOLERANCE)) && ((x) > (v)*(1-TOLERANCE)))
#define COUNT_OF(x)             (sizeof(x) / sizeof(x[0]))


bool (*IrdaDecodeStartBit)(bool level, uint32_t duration);
void (*IrdaDecodeData)(const uint64_t *data, IrdaMessage *msg);

typedef enum {
    DecodeStatusError,
    DecodeStatusOk,
    DecodeStatusReady,
} DecodeStatus;

typedef enum {
    DecoderStatePreambule,
    DecoderStateTryAll,
    DecoderStateDecodeData,
    DecoderStateInterpretData,
} DecoderState;

typedef struct {
    DecoderState state;
    uint8_t buf[DECODER_BUF_MAX_SIZE];
    IrdaMessage msg;
    uint32_t t[4];
    uint8_t tc;
    uint32_t level;     // only for check
    uint64_t data;      // largest protocol can't fit 32 bit
    uint8_t data_cnt;
    const IrdaProtocolSpec* protocol;
} IrdaDecoder;

typedef struct {
    const char* name;
    uint16_t preamb_mark;
    uint16_t preamb_space;

    enum { Pwm, Manchester } enconding;
    IrdaDecodeStartBit startbit_decode;

    uint16_t bit1_mark;
    uint16_t bit1_space;
    uint16_t bit0_mark;
    uint16_t bit0_space;

    uint32_t data_len;       // pwm - bits, manchester - time
    IrdaDecodeDecode decode;
    IrdaDecodeInterpret interpret;
} IrdaProtocolSpec;

static const IrdaProtocolSpec irda_spec[] = {
    {"NEC",     9000,   4500,   560,    1600,   560,    560,    32, decode_pwm,            InterpretNEC},
    {"RC5",     0,      0,      890,    0,      0,      0,      0,  decode_manchester,     NULL},
    {"SHARP",   0,      0,      320,    2000,   320,    1000,   0,  decode_pwm,            NULL},
    {"RECS-80", 0,      0,      160,    4900,   160,    7440,   0,  decode_pwm             NULL},    // check whole cycle len
    {"RC6",     2670,   890,    450,    0,      0,      0,      0,  decode_manchesterRc6,  NULL},
    {"NOKIA",   500,    2500,   500,    0,      0,      0,      0,  decode_manchester,     NULL},
    {"NOKIA-LB",500,    3500,   500,    0,      0,      0,      0,  decode_manchester,     NULL},
};

static IrdaProtocolSpec* find_by_preambula(uint32_t t[2]) {
    for (int i = 0; i < COUNT_OF(irda_spec); ++i) {
        if ((MATCH_TIMING(t[0], irda_spec[i].preamb_mark))
            && (MATCH_TIMING(t[1], irda_spec[i].preamb_space))) {
            return &irda_spec[i];
        }
    }
    return 0;
}

static IrdaProtocolSpec* find_by_decode_all(IrdaDecoder *d) {
    DecodeStatus status = DecodeStatusError;
    for (int i = 0; i < COUNT_OF(irda_spec); ++i) {
        if (irda_spec[i].preamb_mark)
            continue;
        status = irda_spec[i].decode(d, &irda_spec[i]);
        if (DecodeStatusOk != status)
            return &irda_spec[i];
    }

    return 0;
}

// decrease tc only in case of successfull decode
bool decode_manchester(IrdaDecoder* d, IrdaProtocolSpec* spec) {

}

// Todo: Check it tomorrow
// decrease tc only in case of successfull decode
DecodeStatus decode_pwm(IrdaDecoder* d, IrdaProtocolSpec* spec) {
    uint32_t* t = d->t;

    // Stop bit
    if ((d->data_cnt == d->protocol->data_len) && (d->tc == 1)) {
        if (!MATCH_TIMING(t[0], spec->bit1_mark)) {
            // ! error, we determined protocol uncorrectly
            return DecodeStatusError;
        }
        d->tc--;
        return DecodeStatusReady;
    }

    if (d->tc >= 2) {
        if (MATCH_TIMING(t[0], spec->bit1_mark)
            && MATCH_TIMING(t[1], spec->bit1_space)) {
            d->data = (d->data >> 1) & (0x1LU << 63);  // add 1
        } else if (MATCH_TIMING(t[0], spec->bit0_mark)
            && MATCH_TIMING(t[1], spec->bit0_space)) {
            d->data = (d->data >> 1);                   // add 0
        } else {
            // ! error, we determined protocol uncorrectly
            return DecodeStatusError;
        }
        ++d->data_cnt;
        d->tc -= 2;
    }

    return DecodeStatusOk;
}

bool find_by_start_bit(IrdaDecoder *d) {
    for (int i = 0; i < sizeof(proto_spec); ++i) {
        if (!proto_spec[i].decode_start_bit)
            continue;
        if (proto_spec[i].decode_start_bit(d)) {
            d->protocol = &proto_spec[i];
            found = 1;
            break;
        }
    }
}

// Processed in ISR - time critical
IrdaMessage* irda_decode(IrdaDecoder *d, bool level, uint32_t duration) {
    IrdaMessage* im = 0;

    if (duration > IRDA_MAX_DT_US) {
        d->state = DecoderStatePreambule;
        d->tc = 0;
        d->level = 0;
        d->protocol = 0;
        d->data = 0;
        return;
    }

    d->level = !d->level;
    furi_assert(d->level == level);

    d->t[d->tc] = duration;
    d->tc++;
    furi_assert(d->tc <= sizeof(d->t));

    switch(d->state) {
    case DecoderStatePreambule:
        if (d->tc < 2)
            break;
        d->protocol = find_by_preambula(d);
        if (d->protocol) {    // found
            d->tc -= 2;
            if (!d->protocol.decode_start_bit)
                d->state = DecoderStateGetData;
            break;
        }
        d->state = DecoderStateTryAll;
        /* FALLTHROUGH */       // if no preambula - try to guess protocol by start_bit
    case DecoderStateTryAll:
        d->protocol = find_by_decode_all();
        if (d->protocol)
            d->state = DecoderStateDecodeData;
        break;
    case DecoderStateDecodeData:      // Now we know protocol
        status = d->protocol->decode(d);
        if (status != DecodeReady)
            break;
        d->state = DecoderStateInterpretData;
        /* FALLTHROUGH */
    case DecoderStateInterpretData:
        status = d->protocol->interpret(d);
        if (status == DecodeReady)
            d->state = DecoderStateInterpretData;
        d->state = DecoderStateRepeat;
        break;
    case DecoderStateRepeat:
        d->state = DecoderStatePreambule;
        break;
    default:
        break;

    }

    if (status == DecoderError)
        d->state = DecoderStatePreambule;

    return im;
}


IrdaDecoder* irda_new_decoder(void) {
    IrdaDecoder* d = malloc(sizeof(IrdaDecoder));
    return d;
}

void irda_free_decoder(IrdaDecoder* d) {
    free(d);
}

const char* irda_get_protocol_name(const IrdaProtocolSpec* protocol) {
    return protocol->name;
}

