#include "came.h"

#include "../blocks/const.h"
#include "../blocks/decoder.h"
#include "../blocks/encoder.h"
#include "../blocks/generic.h"
#include "../blocks/math.h"

/*
 * Help
 * https://phreakerclub.com/447
 *
 */

#define TAG "SubGhzProtocolCAME"

static const SubGhzBlockConst subghz_protocol_came_const = {
    .te_short = 320,
    .te_long = 640,
    .te_delta = 150,
    .min_count_bit_for_found = 12,
};

struct SubGhzProtocolDecoderCame {
    SubGhzProtocolDecoderBase base;

    SubGhzBlockDecoder decoder;
    SubGhzBlockGeneric generic;
};

struct SubGhzProtocolEncoderCame {
    SubGhzProtocolEncoderBase base;

    SubGhzProtocolBlockEncoder encoder;
    SubGhzBlockGeneric generic;
};

typedef enum {
    CameDecoderStepReset = 0,
    CameDecoderStepFoundStartBit,
    CameDecoderStepSaveDuration,
    CameDecoderStepCheckDuration,
} CameDecoderStep;

const SubGhzProtocolDecoder subghz_protocol_came_decoder = {
    .alloc = subghz_protocol_decoder_came_alloc,
    .free = subghz_protocol_decoder_came_free,

    .feed = subghz_protocol_decoder_came_feed,
    .reset = subghz_protocol_decoder_came_reset,

    .serialize = subghz_protocol_decoder_came_serialization,
    .save_file = subghz_protocol_came_save_file,
};

const SubGhzProtocolEncoder subghz_protocol_came_encoder = {
    .alloc = subghz_protocol_encoder_came_alloc,
    .free = subghz_protocol_encoder_came_free,

    .load = subghz_protocol_encoder_came_load,
    .stop = subghz_protocol_encoder_came_stop,
    .yield = subghz_protocol_encoder_came_yield,
    .load_file = subghz_protocol_came_load_file,
};

const SubGhzProtocol subghz_protocol_came = {
    .name = SUBGHZ_PROTOCOL_CAME_NAME,
    .type = SubGhzProtocolCommonTypeStatic_,

    .decoder = &subghz_protocol_came_decoder,
    .encoder = &subghz_protocol_came_encoder,
};

void* subghz_protocol_encoder_came_alloc(SubGhzEnvironment* environment) {
    SubGhzProtocolEncoderCame* instance = furi_alloc(sizeof(SubGhzProtocolEncoderCame));

    instance->base.protocol = &subghz_protocol_came;

    instance->encoder.repeat = 10;
    instance->encoder.size_upload = 52; //max 24bit*2 + 2 (start, stop)
    instance->encoder.upload = furi_alloc(instance->encoder.size_upload * sizeof(LevelDuration));
    instance->encoder.is_runing = false;
    return instance;
}

void subghz_protocol_encoder_came_free(void* context) {
    furi_assert(context);
    SubGhzProtocolEncoderCame* instance = context;
    free(instance->encoder.upload);
    free(instance);
}

static bool subghz_protocol_came_encoder_get_upload(SubGhzProtocolEncoderCame* instance) {
    furi_assert(instance);
    size_t index = 0;
    size_t size_upload = (instance->generic.data_count_bit * 2) + 2;
    if(size_upload > instance->encoder.size_upload) {
        FURI_LOG_E(TAG, "Size upload exceeds allocated encoder buffer.");
        return false;
    } else {
        instance->encoder.size_upload = size_upload;
    }
    //Send header
    instance->encoder.upload[index++] =
        level_duration_make(false, (uint32_t)subghz_protocol_came_const.te_short * 36);
    //Send start bit
    instance->encoder.upload[index++] =
        level_duration_make(true, (uint32_t)subghz_protocol_came_const.te_short);
    //Send key data
    for(uint8_t i = instance->generic.data_count_bit; i > 0; i--) {
        if(bit_read(instance->generic.data, i - 1)) {
            //send bit 1
            instance->encoder.upload[index++] =
                level_duration_make(false, (uint32_t)subghz_protocol_came_const.te_long);
            instance->encoder.upload[index++] =
                level_duration_make(true, (uint32_t)subghz_protocol_came_const.te_short);
        } else {
            //send bit 0
            instance->encoder.upload[index++] =
                level_duration_make(false, (uint32_t)subghz_protocol_came_const.te_short);
            instance->encoder.upload[index++] =
                level_duration_make(true, (uint32_t)subghz_protocol_came_const.te_long);
        }
    }
    return true;
}

bool subghz_protocol_encoder_came_load(
    void* context,
    uint64_t key,
    uint8_t count_bit,
    size_t repeat) {
    furi_assert(context);
    SubGhzProtocolEncoderCame* instance = context;
    instance->generic.data = key;
    instance->generic.data_count_bit = count_bit;
    instance->encoder.repeat = repeat;
    subghz_protocol_came_encoder_get_upload(instance);
    instance->encoder.is_runing = true;
    return true;
}

void subghz_protocol_encoder_came_stop(void* context) {
    SubGhzProtocolEncoderCame* instance = context;
    instance->encoder.is_runing = false;
}

LevelDuration subghz_protocol_encoder_came_yield(void* context) {
    SubGhzProtocolEncoderCame* instance = context;

    if(instance->encoder.repeat == 0 || !instance->encoder.is_runing) {
        instance->encoder.is_runing = false;
        return level_duration_reset();
    }

    LevelDuration ret = instance->encoder.upload[instance->encoder.front];

    if(++instance->encoder.front == instance->encoder.size_upload) {
        instance->encoder.repeat--;
        instance->encoder.front = 0;
    }

    return ret;
}

void* subghz_protocol_decoder_came_alloc(SubGhzEnvironment* environment) {
    SubGhzProtocolDecoderCame* instance = furi_alloc(sizeof(SubGhzProtocolDecoderCame));
    instance->base.protocol = &subghz_protocol_came;
    return instance;
}

void subghz_protocol_decoder_came_free(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderCame* instance = context;
    free(instance);
}

void subghz_protocol_decoder_came_reset(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderCame* instance = context;
    instance->decoder.parser_step = CameDecoderStepReset;
}

void subghz_protocol_decoder_came_feed(void* context, bool level, uint32_t duration) {
    furi_assert(context);
    SubGhzProtocolDecoderCame* instance = context;
    switch(instance->decoder.parser_step) {
    case CameDecoderStepReset:
        if((!level) && (DURATION_DIFF(duration, subghz_protocol_came_const.te_short * 51) <
                        subghz_protocol_came_const.te_delta * 51)) { //Need protocol 36 te_short
            //Found header CAME
            instance->decoder.parser_step = CameDecoderStepFoundStartBit;
        }
        break;
    case CameDecoderStepFoundStartBit:
        if(!level) {
            break;
        } else if(
            DURATION_DIFF(duration, subghz_protocol_came_const.te_short) <
            subghz_protocol_came_const.te_delta) {
            //Found start bit CAME
            instance->decoder.parser_step = CameDecoderStepSaveDuration;
            instance->decoder.decode_data = 0;
            instance->decoder.decode_count_bit = 0;
        } else {
            instance->decoder.parser_step = CameDecoderStepReset;
        }
        break;
    case CameDecoderStepSaveDuration:
        if(!level) { //save interval
            if(duration >= (subghz_protocol_came_const.te_short * 4)) {
                instance->decoder.parser_step = CameDecoderStepFoundStartBit;
                if(instance->decoder.decode_count_bit >=
                   subghz_protocol_came_const.min_count_bit_for_found) {
                    instance->generic.serial = 0x0;
                    instance->generic.btn = 0x0;

                    instance->generic.data = instance->decoder.decode_data;
                    instance->generic.data_count_bit = instance->decoder.decode_count_bit;

                    if(instance->base.callback)
                        instance->base.callback(&instance->base, instance->base.context);
                }
                break;
            }
            instance->decoder.te_last = duration;
            instance->decoder.parser_step = CameDecoderStepCheckDuration;
        } else {
            instance->decoder.parser_step = CameDecoderStepReset;
        }
        break;
    case CameDecoderStepCheckDuration:
        if(level) {
            if((DURATION_DIFF(instance->decoder.te_last, subghz_protocol_came_const.te_short) <
                subghz_protocol_came_const.te_delta) &&
               (DURATION_DIFF(duration, subghz_protocol_came_const.te_long) <
                subghz_protocol_came_const.te_delta)) {
                subghz_protocol_blocks_add_bit(&instance->decoder, 0);
                instance->decoder.parser_step = CameDecoderStepSaveDuration;
            } else if(
                (DURATION_DIFF(instance->decoder.te_last, subghz_protocol_came_const.te_long) <
                 subghz_protocol_came_const.te_delta) &&
                (DURATION_DIFF(duration, subghz_protocol_came_const.te_short) <
                 subghz_protocol_came_const.te_delta)) {
                subghz_protocol_blocks_add_bit(&instance->decoder, 1);
                instance->decoder.parser_step = CameDecoderStepSaveDuration;
            } else
                instance->decoder.parser_step = CameDecoderStepReset;
        } else {
            instance->decoder.parser_step = CameDecoderStepReset;
        }
        break;
    }
}

void subghz_protocol_decoder_came_serialization(void* context, string_t output) {
    furi_assert(context);
    SubGhzProtocolDecoderCame* instance = context;

    uint32_t code_found_lo = instance->generic.data & 0x00000000ffffffff;

    uint64_t code_found_reverse = subghz_protocol_blocks_reverse_key(
        instance->generic.data, instance->generic.data_count_bit);

    uint32_t code_found_reverse_lo = code_found_reverse & 0x00000000ffffffff;

    string_cat_printf(
        output,
        "%s %dbit\r\n"
        "Key:0x%08lX\r\n"
        "Yek:0x%08lX\r\n",
        instance->base.protocol->name,
        instance->generic.data_count_bit,
        code_found_lo,
        code_found_reverse_lo);
}

bool subghz_protocol_came_save_file(void* context, FlipperFile* flipper_file) {
    furi_assert(context);
    SubGhzProtocolDecoderCame* instance = context;
    return subghz_block_generic_save_file(&instance->generic, flipper_file);
}

bool subghz_protocol_came_load_file(
    void* context,
    FlipperFile* flipper_file,
    const char* file_path) {
    furi_assert(context);
    SubGhzProtocolEncoderCame* instance = context;
    return subghz_block_generic_load_file(&instance->generic, flipper_file);
}

// void subghz_decoder_came_to_load_protocol(SubGhzProtocolCame* instance, void* context) {
//     furi_assert(context);
//     furi_assert(instance);
//     SubGhzProtocolCommonLoad* data = context;
//     instance->common.data = data->code_found;
//     instance->common.data_count_bit = data->code_count_bit;
// }
