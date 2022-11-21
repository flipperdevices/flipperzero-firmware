#include "pocsag.h"

#include <inttypes.h>
#include "../blocks/const.h"
#include "../blocks/decoder.h"
#include "../blocks/generic.h"
#include "../blocks/math.h"
#include <lib/flipper_format/flipper_format_i.h>
#include <furi/core/string.h>

#define TAG "POCSAG"

static const SubGhzBlockConst pocsag_const = {
    .te_short = 833,
    .te_delta = 100,
};

// Minimal amount of sync bits (interleaving zeros and ones)
#define POCSAG_MIN_SYNC_BITS    32
#define POCSAG_CW_BITS          32
#define POCSAG_CW_MASK          0xFFFFFFFF
#define POCSAG_FRAME_SYNC_CODE  0x7CD215D8
#define POCSAG_IDLE_CODE_WORD   0x7A89C197
#define POCSAG_MAX_MSG          90


struct SubGhzProtocolDecoderPocsag {
    SubGhzProtocolDecoderBase base;

    SubGhzBlockDecoder decoder;
    SubGhzBlockGeneric generic;

    uint8_t codeword_idx;
    uint32_t ric;
    uint8_t func;
    FuriString* msg;

    // partially decoded character
    uint8_t char_bits;
    uint8_t char_data;
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
    instance->msg = furi_string_alloc();
    return instance;
}

void subghz_protocol_decoder_pocsag_free(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderPocsag* instance = context;
    furi_string_free(instance->msg);
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
    furi_string_reset(instance->msg);
    furi_string_reserve(instance->msg, POCSAG_MAX_MSG);
}


static void pocsag_decode_address_word(SubGhzProtocolDecoderPocsag* instance, uint32_t data) {
    instance->ric = (data >> 13);
    instance->ric = (instance->ric << 3) | (instance->codeword_idx >> 1);
    instance->func = (data >> 11) & 0b11;
}

// decode message word, maintaining instance state for partial decoding. Return true if more data
// might follow or false if end of message reached.
static bool pocsag_decode_message_word(SubGhzProtocolDecoderPocsag* instance, uint32_t data) {
    for (uint8_t i = 0; i < 20; i++) {
        instance->char_data >>= 1;
        if (data & (1 << 30)) {
            instance->char_data |= 1<<6;
        }
        instance->char_bits++;
        if (instance->char_bits == 7) {
            if (instance->char_data == 0)
                return false;
//            FURI_LOG_I(TAG, "%c", instance->char_data);
            furi_string_push_back(instance->msg, instance->char_data);
            instance->char_data = 0;
            instance->char_bits = 0;
        }
        data <<= 1;
    }
    return true;
}


static void pocsag_message_done(SubGhzProtocolDecoderPocsag* instance) {
//    FURI_LOG_I(TAG, "%s", furi_string_get_cstr(instance->msg));
    FURI_LOG_I(TAG, "done: %d", furi_string_size(instance->msg));
    FURI_LOG_I(TAG, "bits: %d", instance->generic.data_count_bit);
    if(instance->base.callback)
        instance->base.callback(&instance->base, instance->base.context);
    instance->char_bits = 0;
    instance->char_data = 0;
    furi_string_reset(instance->msg);
    furi_string_reserve(instance->msg, POCSAG_MAX_MSG);
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
//                FURI_LOG_I(TAG, "Sync found");
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

    uint32_t codeword;

    // handle state machine for every incoming bit
    while (bits_count-- > 0) {
        subghz_protocol_blocks_add_bit(&instance->decoder, !level);

        switch(instance->decoder.parser_step) {
        case PocsagDecoderStepFoundSync:
            if ((instance->decoder.decode_data & POCSAG_CW_MASK) == POCSAG_FRAME_SYNC_CODE) {
//                FURI_LOG_I(TAG, "Found preamble!");
                instance->decoder.parser_step = PocsagDecoderStepFoundPreamble;
                instance->decoder.decode_count_bit = 0;
                instance->decoder.decode_data = 0UL;
            }
            break;
        case PocsagDecoderStepFoundPreamble:
            // handle codewords
            if (instance->decoder.decode_count_bit == POCSAG_CW_BITS) {
                codeword = (uint32_t)(instance->decoder.decode_data & POCSAG_CW_MASK);
                switch (codeword) {
                case POCSAG_IDLE_CODE_WORD:
//                    FURI_LOG_I(TAG, "Idle");
                    instance->codeword_idx++;
                    break;
                case POCSAG_FRAME_SYNC_CODE:
//                    FURI_LOG_I(TAG, "Sync");
                    instance->codeword_idx = 0;
                    break;
                default:
                    // Here we expect only address messages
                    if (codeword >> 31 == 0) {
                        pocsag_decode_address_word(instance, codeword);
//                        FURI_LOG_I(TAG, "Address: %" PRIu32 , instance->addr);
//                        FURI_LOG_I(TAG, "Function: %" PRIu8, instance->func);
                        instance->decoder.parser_step = PocsagDecoderStepMessage;
                    }
                    instance->codeword_idx++;
                }
                instance->decoder.decode_count_bit = 0;
                instance->decoder.decode_data = 0UL;
            }
            break;

        case PocsagDecoderStepMessage:
            if (instance->decoder.decode_count_bit == POCSAG_CW_BITS) {
                codeword = (uint32_t)(instance->decoder.decode_data & POCSAG_CW_MASK);
                switch (codeword) {
                case POCSAG_IDLE_CODE_WORD:
                    // Idle during the message stops the message
//                    FURI_LOG_I(TAG, "MIdle");
                    instance->codeword_idx++;
                    instance->decoder.parser_step = PocsagDecoderStepFoundPreamble;
                    pocsag_message_done(instance);
//                    FURI_LOG_I(TAG, "Msg done %d", instance->msg_len);
                    break;
                case POCSAG_FRAME_SYNC_CODE:
//                    FURI_LOG_I(TAG, "MSync");
                    instance->codeword_idx = 0;
                    break;
                default:
                    // In this state, both address and message words can arrive
                    if (codeword >> 31 == 0) {
//                        FURI_LOG_I(TAG, "MAddr");
//                        FURI_LOG_I(TAG, "Msg done %d", instance->msg_len);
                        pocsag_decode_address_word(instance, codeword);
//                        FURI_LOG_I(TAG, "Address: %" PRIu32 , instance->addr);
//                        FURI_LOG_I(TAG, "Function: %" PRIu8, instance->func);
                    } else {
//                        FURI_LOG_I(TAG, "Msg");
                        if (!pocsag_decode_message_word(instance, codeword)) {
                            instance->decoder.parser_step = PocsagDecoderStepFoundPreamble;
//                            FURI_LOG_I(TAG, "Msg done %d", instance->msg_len);
                            pocsag_message_done(instance);
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

uint8_t subghz_protocol_decoder_pocsag_get_hash_data(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderPocsag* instance = context;
    uint8_t hash = 0;

    for(size_t i = 0; i < furi_string_size(instance->msg); i++)
        hash ^= furi_string_get_char(instance->msg, i);
    // address is 21 bit
    hash ^= (instance->ric & 0xFF) ^
            ((instance->ric >> 8) & 0xFF) ^
            ((instance->ric >> 16) & 0xFF);
    return hash;
}

bool subghz_protocol_decoder_pocsag_serialize(void* context, FlipperFormat* flipper_format, SubGhzRadioPreset* preset) {
    furi_assert(context);
    SubGhzProtocolDecoderPocsag* instance = context;
    FURI_LOG_I(TAG, "ser: %d", furi_string_size(instance->msg));

    if(!subghz_block_generic_serialize(&instance->generic, flipper_format, preset))
        return false;

    if(!flipper_format_write_uint32(flipper_format, "RIC", &instance->ric, 1)) {
        FURI_LOG_E(TAG, "Error adding RIC");
        return false;
    }

    if(!flipper_format_write_string(flipper_format, "Msg", instance->msg)) {
        FURI_LOG_E(TAG, "Error adding Msg");
        return false;
    }
    return true;
}

bool subghz_protocol_decoder_pocsag_deserialize(void* context, FlipperFormat* flipper_format) {
    furi_assert(context);
    SubGhzProtocolDecoderPocsag* instance = context;
    bool ret = false;

    do {
        if(!subghz_block_generic_deserialize(&instance->generic, flipper_format)) {
            break;
        }
        if(!flipper_format_read_uint32(flipper_format, "RIC", &instance->ric, 1)) {
            FURI_LOG_E(TAG, "Missing RIC");
            break;
        }
        if(!flipper_format_read_string(flipper_format, "Msg", instance->msg)) {
            FURI_LOG_E(TAG, "Missing Msg");
            break;
        }
        FURI_LOG_I(TAG, "deser: %d", furi_string_size(instance->msg));
        FURI_LOG_I(TAG, "deser: %s", furi_string_get_cstr(instance->msg));
        ret = true;
    } while(false);
    return ret;
}

void subhz_protocol_decoder_pocsag_get_string(void* context, FuriString* output) {
    furi_assert(context);
    SubGhzProtocolDecoderPocsag* instance = context;

    furi_string_cat_printf(
        output,
        "%s\r\n"
        "RIC: %" PRIu32 "\r\n",
        instance->generic.protocol_name,
        instance->ric
    );
    furi_string_cat(output, instance->msg);
    FURI_LOG_I(TAG, "str: %s", furi_string_get_cstr(output));
}

const SubGhzProtocolDecoder subghz_protocol_pocsag_decoder = {
    .alloc = subghz_protocol_decoder_pocsag_alloc,
    .free = subghz_protocol_decoder_pocsag_free,
    .reset = subghz_protocol_decoder_pocsag_reset,
    .feed = subghz_protocol_decoder_pocsag_feed,
    .get_hash_data = subghz_protocol_decoder_pocsag_get_hash_data,
    .serialize = subghz_protocol_decoder_pocsag_serialize,
    .deserialize = subghz_protocol_decoder_pocsag_deserialize,
    .get_string = subhz_protocol_decoder_pocsag_get_string,
};

const SubGhzProtocol subghz_protocol_pocsag = {
    .name = SUBGHZ_PROTOCOL_POCSAG,
    .type = SubGhzProtocolTypeStatic,
    .flag = SubGhzProtocolFlag_FM | SubGhzProtocolFlag_Decodable | SubGhzProtocolFlag_Save | SubGhzProtocolFlag_Load,

    .decoder = &subghz_protocol_pocsag_decoder,
};