#pragma once

#include <stdint.h>
#include "irda_i.h"


#define BIT_TOLERANCE                   (120)
#define MATCH_BIT_TIMING(x, v)          (  ((x) < (v + BIT_TOLERANCE)) \
                                        && ((x) > (v - BIT_TOLERANCE)))

#define MATCH_PREAMBLE_TIMING(x, v)     MATCH_TIMING(x,v,0.07)
#define MATCH_TIMING(x, v, d)           (  ((x) < ((v) * (1 + (d)))) \
                                        && ((x) > ((v) * (1 - (d)))))

typedef enum {
    DecodeStatusError,
    DecodeStatusOk,
    DecodeStatusReady,
} DecodeStatus;

typedef struct IrdaCommonDecoder IrdaCommonDecoder;

typedef DecodeStatus (*IrdaDecode)(IrdaCommonDecoder*);
typedef bool (*IrdaInterpret)(IrdaCommonDecoder*);
typedef DecodeStatus (*IrdaDecodeRepeat)(IrdaCommonDecoder*);

typedef enum IrdaCommonState {
    IrdaCommonStateWaitPreamble,
    IrdaCommonStateDecode,
    IrdaCommonStateProcessRepeat,
} IrdaCommonState;

typedef struct {
    uint16_t preamb_mark;
    uint16_t preamb_space;
    uint16_t bit1_mark;
    uint16_t bit1_space;
    uint16_t bit0_mark;
    uint16_t bit0_space;
} IrdaCommonDecoderTimings;

typedef struct {
    IrdaCommonDecoderTimings timings;
    uint32_t databit_len;
    IrdaDecode decode;
    IrdaInterpret interpret;
    IrdaDecodeRepeat decode_repeat;
} IrdaCommonProtocolSpec;

struct IrdaCommonDecoder {
    const IrdaCommonProtocolSpec* protocol;
    IrdaCommonState state;
    IrdaMessage message;
    uint32_t timings[6];
    uint8_t timings_cnt;
    uint32_t level;
    uint16_t databit_cnt;
    uint8_t data[];
};


static inline void shift_left_array(uint32_t *array, uint32_t len, uint32_t shift) {
    for (int i = 0; i < len; ++i)
        array[i] = array[i + shift];
}


IrdaMessage* decode_common(IrdaCommonDecoder *decoder, bool level, uint32_t duration);
void* common_decoder_init(const IrdaCommonProtocolSpec *protocol);
void common_decoder_fini(void* decoder);
DecodeStatus decode_pwm(IrdaCommonDecoder* decoder);

