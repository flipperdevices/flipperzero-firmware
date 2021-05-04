#include <malloc.h>
#include "libirda.h"
#include <furi.h>


#define IRDA_MAX_DT_US                  (200*1000)
#define DECODER_BUF_MAX_SIZE            128
#define PREAMBULA_TOLERANCE             0.07
#define BIT_TOLERANCE                   120
#define MATCH_PREAMBULA_TIMING(x, v)    (((x) < (v) * (1 + PREAMBULA_TOLERANCE)) && ((x) > (v) * (1 - PREAMBULA_TOLERANCE)))
#define MATCH_BIT_TIMING(x, v)          (((x) < (v + BIT_TOLERANCE)) && ((x) > (v - BIT_TOLERANCE)))
#define COUNT_OF(x)                     (sizeof(x) / sizeof(x[0]))


typedef enum {
    DecodeStatusError,
    DecodeStatusOk,
    DecodeStatusReady,
} DecodeStatus;

typedef enum {
    DecoderStatePreambule,
    DecoderStateDecodeAll,
    DecoderStateDecode,
    DecoderStateInterpret,
    DecoderStateRepeat,
} DecoderState;

typedef DecodeStatus (*IrdaDecode)(IrdaDecoder*, const IrdaProtocolSpec*);
typedef DecodeStatus (*IrdaInterpret)(IrdaDecoder*);

struct IrdaDecoder {
    DecoderState state;
    uint8_t buf[DECODER_BUF_MAX_SIZE];
    IrdaMessage msg;
    uint32_t t[4];
    uint8_t tc;
    uint32_t level;     // only for check
    uint64_t data;      // largest protocol can't fit 32 bit
    uint8_t data_cnt;
    const IrdaProtocolSpec* protocol;
};

struct IrdaProtocolSpec{
    const char* name;
    uint16_t preamb_mark;
    uint16_t preamb_space;

    uint16_t bit1_mark;
    uint16_t bit1_space;
    uint16_t bit0_mark;
    uint16_t bit0_space;

    uint32_t data_len;       // pwm - bits, manchester - time in us
    IrdaDecode decode;
    IrdaInterpret interpret;
};


static const IrdaProtocolSpec* find_by_preambula(const uint32_t *t);
static const IrdaProtocolSpec* find_by_decode_all(IrdaDecoder *d);
//static DecodeStatus decode_manchester(IrdaDecoder* d, IrdaProtocolSpec* spec);
static DecodeStatus decode_pwm(IrdaDecoder* d, const IrdaProtocolSpec* spec);
static DecodeStatus InterpretNEC(IrdaDecoder* d);


static const IrdaProtocolSpec irda_spec[] = {
    {"NEC",     9000,   4500,   560,    1600,   560,    560,    32, decode_pwm,             InterpretNEC},
    {"JVC",     8400,   4200,   525,    1575,   525,    525,    16, decode_pwm,             NULL},
//    {"RC5",     0,      0,      890,    0,      0,      0,      0,  decode_manchester,     NULL},
//    {"SHARP",   0,      0,      320,    2000,   320,    1000,   0,  decode_pwm,            NULL},
//    {"RECS-80", 0,      0,      160,    4900,   160,    7440,   0,  decode_pwm             NULL},    // check whole cycle len
//    {"RC6",     2670,   890,    450,    0,      0,      0,      0,  decode_manchesterRc6,  NULL},
//    {"NOKIA",   500,    2500,   500,    0,      0,      0,      0,  decode_manchester,     NULL},
//    {"NOKIA-LB",500,    3500,   500,    0,      0,      0,      0,  decode_manchester,     NULL},
};

static const IrdaProtocolSpec* find_by_preambula(const uint32_t *t) {
    for (int i = 0; i < COUNT_OF(irda_spec); ++i) {
        if ((MATCH_PREAMBULA_TIMING(t[0], irda_spec[i].preamb_mark))
            && (MATCH_PREAMBULA_TIMING(t[1], irda_spec[i].preamb_space))) {
            return &irda_spec[i];
        }
    }
    return 0;
}

static const IrdaProtocolSpec* find_by_decode_all(IrdaDecoder *d) {
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

// bits
// 01234567 89ABCDEF 01234567 89ABCDEF 01234567 89ABCDEF 01234567 89ABCDEF
// CmdInv   Cmd      AddrInv  M Addr L

static DecodeStatus InterpretNEC(IrdaDecoder* d) {
    DecodeStatus status = DecodeStatusError;
    uint32_t data = ((uint32_t *)(&d->data))[1];        // get 32 MSB
    uint8_t adr = data;
    uint8_t adrInv = data >> 8;
    uint8_t cmdInv = data >> 16;
    uint8_t cmd = data >> 24;

    if ((cmd == ~cmdInv) && (adr == ~adrInv)) {
        d->msg.cmd = cmd;
        d->msg.adr = adr;
        d->msg.protocol = d->protocol;
        d->msg.repeat = false;
        status = DecodeStatusOk;
    }

    return status;
}

// decrease tc only in case of successfull decode
//static bool decode_manchester(IrdaDecoder* d, IrdaProtocolSpec* spec) {
//
//}

// Todo: Check it tomorrow
// decrease tc only in case of successfull decode
static DecodeStatus decode_pwm(IrdaDecoder* d, const IrdaProtocolSpec* spec) {
    uint32_t* t = d->t;

    // Stop bit
    if ((d->data_cnt == d->protocol->data_len) && (d->tc == 1)) {
        volatile bool p1 = t[0] < (spec->bit1_mark + BIT_TOLERANCE);
        volatile bool p2 = t[0] > (spec->bit1_mark - BIT_TOLERANCE);
        bool p = MATCH_BIT_TIMING(t[0], spec->bit1_mark);
        printf("MATCH_BIT_TIMING(t[0], spec->bit1_mark)) = %d, p1=%d, p2=%d\r\n", p, p1, p2);
        if (!MATCH_BIT_TIMING(t[0], spec->bit1_mark)) {
            // ! error, we determined protocol uncorrectly
            return DecodeStatusError;
        }
        d->tc--;
        return DecodeStatusReady;
    }

    if (d->tc >= 2) {
        if (MATCH_BIT_TIMING(t[0], spec->bit1_mark)
            && MATCH_BIT_TIMING(t[1], spec->bit1_space)) {
            d->data = (d->data >> 1) & (0x1LLU << 63);   // add 1
        } else if (MATCH_BIT_TIMING(t[0], spec->bit0_mark)
            && MATCH_BIT_TIMING(t[1], spec->bit0_space)) {
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

// Processed in ISR - time critical
IrdaMessage* irda_decode(IrdaDecoder *d, bool level, uint32_t duration) {
//static uint32_t m[500];
//static uint32_t mc;
//    m[mc++] = duration;
//    if (mc >= sizeof(m))
//        mc = 0;
//
//    if (mc == 200)
//        furi_assert(0);
//
//    (void) m;
//    return 0;

    IrdaMessage* im = 0;
    DecodeStatus status = DecodeStatusError;

    if (duration > IRDA_MAX_DT_US) {
//        furi_assert(d->tc == 0);
//        furi_assert(d->data_cnt == 0);
        d->state = DecoderStatePreambule;
        d->tc = 0;
        d->level = 0;
        d->protocol = 0;
        d->data = 0;
        d->data_cnt = 0;
        return 0;
    }

    d->level = !d->level;
//    furi_assert(d->level == level);

    d->t[d->tc] = duration;
    d->tc++;
    furi_assert(d->tc <= sizeof(d->t));

    switch(d->state) {
    case DecoderStatePreambule:
        if (d->tc < 2)
            break;
        d->protocol = find_by_preambula(d->t);
        if (d->protocol) {    // found
            d->tc -= 2;
            d->state = DecoderStateDecode;
            break;
        } else {
            d->state = DecoderStateDecodeAll;
        }
        /* FALLTHROUGH */       // if no preambula - try to guess protocol by start_bit
    case DecoderStateDecodeAll:
        d->protocol = find_by_decode_all(d);
        if (d->protocol)
            d->state = DecoderStateDecode;
        break;
    case DecoderStateDecode:      // Now we know protocol
        status = d->protocol->decode(d, d->protocol);
        if (status == DecodeStatusError)
            d->state = DecoderStatePreambule;
        if (status != DecodeStatusReady)
            break;
        d->state = DecoderStateInterpret;
        /* FALLTHROUGH */
    case DecoderStateInterpret:
        printf("decoded bits: %llX\r\n", d->data);      // DBG
        status = d->protocol->interpret(d);
        if (status == DecodeStatusReady) {
            d->state = DecoderStateRepeat;
            d->tc = 0;
            d->data_cnt = 0;
            im = &d->msg;
        } else if (status == DecodeStatusError) {
            d->state = DecoderStatePreambule;
        }
        d->state = DecoderStateRepeat;
        break;
    case DecoderStateRepeat:
        d->state = DecoderStatePreambule;
        break;
    default:
        break;
    }

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

