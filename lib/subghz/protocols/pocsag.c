#include "pocsag.h"

#include <inttypes.h>
#include "../blocks/const.h"
#include "../blocks/decoder.h"
#include "../blocks/generic.h"
#include "../blocks/math.h"


#define TAG "POCSAG"

static const SubGhzBlockConst pocsag_const = {
    .te_short = 833,
    .te_delta = 100,
};

// Minimal amount of sync bits (interleaving zeros and ones)
#define POCSAG_MIN_SYNC_BITS 32

struct SubGhzProtocolDecoderPocsag {
    SubGhzProtocolDecoderBase base;

    SubGhzBlockDecoder decoder;
    SubGhzBlockGeneric generic;

    bool debug;
};

typedef struct SubGhzProtocolDecoderPocsag SubGhzProtocolDecoderPocsag;

typedef enum {
    PocsagDecoderStepReset = 0,
    PocsagDecoderStepFoundSync,
    PocsagDecoderStepFoundPreamble,
} PocsagDecoderStep;


void* subghz_protocol_decoder_pocsag_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);

    SubGhzProtocolDecoderPocsag* instance = malloc(sizeof(SubGhzProtocolDecoderPocsag));
    instance->base.protocol = &subghz_protocol_pocsag;
    instance->generic.protocol_name = instance->base.protocol->name;
    return instance;
}

void subghz_protocol_decoder_pocsag_free(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderPocsag* instance = context;
    free(instance);
}

void subghz_protocol_decoder_pocsag_reset(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderPocsag* instance = context;

    instance->decoder.parser_step = PocsagDecoderStepReset;
    instance->decoder.decode_data = 0UL;
    instance->decoder.decode_count_bit = 0;
    instance->debug = false;
}

void subghz_protocol_decoder_pocsag_feed(void* context, bool level, uint32_t duration) {
    furi_assert(context);
    SubGhzProtocolDecoderPocsag* instance = context;

    // reset state - waiting for 32 bits of interleaving 1s and 0s
    if (instance->decoder.parser_step == PocsagDecoderStepReset) {
        if (DURATION_DIFF(duration, pocsag_const.te_short) < pocsag_const.te_delta) {
            // POCSAG signals are inverted
            subghz_protocol_blocks_add_bit(&instance->decoder, !level);

            if (instance->decoder.decode_count_bit == POCSAG_MIN_SYNC_BITS) {
                instance->decoder.parser_step = PocsagDecoderStepFoundSync;
                FURI_LOG_I(TAG, "Sync found");
            }
        }
        else {
            subghz_protocol_decoder_pocsag_reset(context);
        }
        return;
    }

    int bits_count = duration / pocsag_const.te_short;
    uint32_t extra = duration - pocsag_const.te_short * bits_count;

    if (DURATION_DIFF(extra, pocsag_const.te_short) < pocsag_const.te_delta)
        bits_count++;
    else if (extra > pocsag_const.te_delta) {
        subghz_protocol_decoder_pocsag_reset(context);
        return;
    }

    if (level)
        FURI_LOG_I(TAG, "1 * %d", bits_count);
    else
        FURI_LOG_I(TAG, "0 * %d", bits_count);
//
//    // as we found sync bits of 32 bits (in fact it is longer), we have to do proper decoding
//    // of 1200-bod signal. As it could be any sequence of 1s and 0s, duration might be the
//    // multiple of 833 us
//    while (true) {
//        // signal is too short, but not shorter than delta - something is wrong, reset
////        if (duration < pocsag_const.te_short &&
////           DURATION_DIFF(duration, pocsag_const.te_short) > pocsag_const.te_delta)
////        {
////            subghz_protocol_decoder_pocsag_reset(context);
////            return;
////        }
//
//        subghz_protocol_blocks_add_bit(&instance->decoder, !level);
//
//        // new bit arrived, update the decoding state machine
//        switch(instance->decoder.parser_step) {
//        case PocsagDecoderStepFoundSync:
//            if (instance->decoder.decode_count_bit == 64) {
//                FURI_LOG_I(TAG, "%" PRIx64, instance->decoder.decode_data);
//                instance->decoder.decode_count_bit = 0;
//            }
//            break;
//        }
//
//        if (duration < pocsag_const.te_short)
//            break;
//        duration -= pocsag_const.te_short;
//    }
}

const SubGhzProtocolDecoder subghz_protocol_pocsag_decoder = {
    .alloc = subghz_protocol_decoder_pocsag_alloc,
    .free = subghz_protocol_decoder_pocsag_free,
    .reset = subghz_protocol_decoder_pocsag_reset,
    .feed = subghz_protocol_decoder_pocsag_feed,
};

const SubGhzProtocol subghz_protocol_pocsag = {
    .name = SUBGHZ_PROTOCOL_POCSAG,
    .type = SubGhzProtocolTypeStatic,
    .flag = SubGhzProtocolFlag_FM | SubGhzProtocolFlag_Decodable,

    .decoder = &subghz_protocol_pocsag_decoder,
};