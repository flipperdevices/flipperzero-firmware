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
#define POCSAG_MIN_SYNC_BITS    32
#define MASK_32_BITS            0xFFFFFFFF
#define POCSAG_FRAME_SYNC_CODE  0x7CD215D8
#define POCSAG_IDLE_CODE_WORD   0x7A89C197
#define POCSAG_MAX_MSG_LEN      80

struct SubGhzProtocolDecoderPocsag {
    SubGhzProtocolDecoderBase base;

    SubGhzBlockDecoder decoder;
    SubGhzBlockGeneric generic;

    uint8_t codeword_idx;
    uint32_t addr;
    uint8_t func;

    // partially decoded character
    uint8_t char_bits;
    uint8_t char_data;

    uint8_t msg_len;
    char msg[POCSAG_MAX_MSG_LEN+1];
};

typedef struct SubGhzProtocolDecoderPocsag SubGhzProtocolDecoderPocsag;

typedef enum {
    PocsagDecoderStepReset = 0,
    PocsagDecoderStepFoundSync,
    PocsagDecoderStepFoundPreamble,
    PocsagDecoderStepMessage,
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
    instance->codeword_idx = 0;
    instance->char_bits = 0;
    instance->char_data = 0;
    instance->msg_len = 0;
}


static void pocsag_decode_address_word(SubGhzProtocolDecoderPocsag* instance, uint32_t data) {
    instance->addr = (data >> 13);
    instance->addr = (instance->addr << 3) | (instance->codeword_idx >> 1);
    instance->func = (data >> 11) & 0b11;
}

// decode message word, maintaining instance state for partial decoding. Return true if more data
// might follow or false if end of message reached.
bool pocsag_decode_message_word(SubGhzProtocolDecoderPocsag* instance, uint32_t data) {
    for (uint8_t i = 0; i < 20; i++) {
        instance->char_data >>= 1;
        if (data & (1 << 30)) {
            instance->char_data |= 1<<6;
        }
        instance->char_bits++;
        if (instance->char_bits == 7) {
            if (instance->char_data == 0)
                return false;
            FURI_LOG_I(TAG, "%c", instance->char_data);
            if (instance->msg_len < POCSAG_MAX_MSG_LEN) {
                instance->msg[instance->msg_len++] = instance->char_data;
            }
            instance->char_data = 0;
            instance->char_bits = 0;
        }
        data <<= 1;
    }
    return true;
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

    // handle state machine for every incoming bit
    while (bits_count-- > 0) {
        subghz_protocol_blocks_add_bit(&instance->decoder, !level);

        switch(instance->decoder.parser_step) {
        case PocsagDecoderStepFoundSync:
            if ((instance->decoder.decode_data & MASK_32_BITS) == POCSAG_FRAME_SYNC_CODE) {
                FURI_LOG_I(TAG, "Found preamble!");
                instance->decoder.parser_step = PocsagDecoderStepFoundPreamble;
                instance->decoder.decode_count_bit = 0;
                instance->decoder.decode_data = 0UL;
            }
            break;
        case PocsagDecoderStepFoundPreamble:
            // handle codewords
            if (instance->decoder.decode_count_bit == 32) {
                switch (instance->decoder.decode_data & MASK_32_BITS) {
                case POCSAG_IDLE_CODE_WORD:
                    FURI_LOG_I(TAG, "Idle");
                    instance->codeword_idx++;
                    break;
                case POCSAG_FRAME_SYNC_CODE:
                    FURI_LOG_I(TAG, "Sync");
                    instance->codeword_idx = 0;
                    break;
                default:
                    if (instance->decoder.decode_data >> 31 == 0) {
                        pocsag_decode_address_word(instance, instance->decoder.decode_data & MASK_32_BITS);
                        FURI_LOG_I(TAG, "Address: %" PRIu32 , instance->addr);
                        FURI_LOG_I(TAG, "Function: %" PRIu8, instance->func);
                    }
                    else {
                        FURI_LOG_I(TAG, "Msg");
                        if (!pocsag_decode_message_word(instance, instance->decoder.decode_data & MASK_32_BITS)) {
                            // push decoded message
                            FURI_LOG_I(TAG, "Msg done %d", instance->msg_len);
                        }
                    }
                    instance->codeword_idx++;
                }

                instance->decoder.decode_count_bit = 0;
                instance->decoder.decode_data = 0UL;
            }
            break;
        }
    }
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