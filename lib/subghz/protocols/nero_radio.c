#include "nero_radio.h"

#include "../blocks/const.h"
#include "../blocks/decoder.h"
#include "../blocks/encoder.h"
#include "../blocks/generic.h"
#include "../blocks/math.h"

#define TAG "SubGhzProtocolNeroRadio"

static const SubGhzBlockConst subghz_protocol_nero_radio_const = {
    .te_short = 200,
    .te_long = 400,
    .te_delta = 80,
    .min_count_bit_for_found = 56,
};

struct SubGhzProtocolDecoderNeroRadio {
    SubGhzProtocolDecoderBase base;

    SubGhzBlockDecoder decoder;
    SubGhzBlockGeneric generic;

    uint16_t header_count;
};

struct SubGhzProtocolEncoderNeroRadio {
    SubGhzProtocolEncoderBase base;

    SubGhzProtocolBlockEncoder encoder;
    SubGhzBlockGeneric generic;
};

typedef enum {
    NeroRadioDecoderStepReset = 0,
    NeroRadioDecoderStepCheckPreambula,
    NeroRadioDecoderStepSaveDuration,
    NeroRadioDecoderStepCheckDuration,
} NeroRadioDecoderStep;

const SubGhzProtocolDecoder subghz_protocol_nero_radio_decoder = {
    .alloc = subghz_protocol_decoder_nero_radio_alloc,
    .free = subghz_protocol_decoder_nero_radio_free,

    .feed = subghz_protocol_decoder_nero_radio_feed,
    .reset = subghz_protocol_decoder_nero_radio_reset,

    .serialize = subghz_protocol_decoder_nero_radio_serialization,
    .save_file = subghz_protocol_nero_radio_save_file,
};

const SubGhzProtocolEncoder subghz_protocol_nero_radio_encoder = {
    .alloc = subghz_protocol_encoder_nero_radio_alloc,
    .free = subghz_protocol_encoder_nero_radio_free,

    .load = subghz_protocol_encoder_nero_radio_load,
    .stop = subghz_protocol_encoder_nero_radio_stop,
    .yield = subghz_protocol_encoder_nero_radio_yield,
    .load_file = subghz_protocol_nero_radio_load_file,
};

const SubGhzProtocol subghz_protocol_nero_radio = {
    .name = SUBGHZ_PROTOCOL_NERO_RADIO_NAME,
    .type = SubGhzProtocolTypeStatic,
    .flag = SubGhzProtocolFlag_433 | SubGhzProtocolFlag_AM | SubGhzProtocolFlag_Decodable,

    .decoder = &subghz_protocol_nero_radio_decoder,
    .encoder = &subghz_protocol_nero_radio_encoder,
};

void* subghz_protocol_encoder_nero_radio_alloc(SubGhzEnvironment* environment) {
    SubGhzProtocolEncoderNeroRadio* instance = furi_alloc(sizeof(SubGhzProtocolEncoderNeroRadio));

    instance->base.protocol = &subghz_protocol_nero_radio;
    instance->generic.protocol_name = instance->base.protocol->name;

    instance->encoder.repeat = 10;
    instance->encoder.size_upload = 256;
    instance->encoder.upload = furi_alloc(instance->encoder.size_upload * sizeof(LevelDuration));
    instance->encoder.is_runing = false;
    return instance;
}

void subghz_protocol_encoder_nero_radio_free(void* context) {
    furi_assert(context);
    SubGhzProtocolEncoderNeroRadio* instance = context;
    free(instance->encoder.upload);
    free(instance);
}

static bool
    subghz_protocol_nero_radio_encoder_get_upload(SubGhzProtocolEncoderNeroRadio* instance) {
    furi_assert(instance);
    size_t index = 0;
    size_t size_upload = 49 * 2 + 2 + (instance->generic.data_count_bit * 2);
    if(size_upload > instance->encoder.size_upload) {
        FURI_LOG_E(TAG, "Size upload exceeds allocated encoder buffer.");
        return false;
    } else {
        instance->encoder.size_upload = size_upload;
    }

    //Send header
    // instance->encoder.upload[index++] =
    //     level_duration_make(true, (uint32_t)subghz_protocol_nero_radio_const.te_short);
    // instance->encoder.upload[index++] =
    //     level_duration_make(false, (uint32_t)subghz_protocol_nero_radio_const.te_short * 37);
    for(uint8_t i = 0; i < 49; i++) {
        instance->encoder.upload[index++] =
            level_duration_make(true, (uint32_t)subghz_protocol_nero_radio_const.te_short);
        instance->encoder.upload[index++] =
            level_duration_make(false, (uint32_t)subghz_protocol_nero_radio_const.te_short);
    }

    //Send start bit
    instance->encoder.upload[index++] =
        level_duration_make(true, (uint32_t)subghz_protocol_nero_radio_const.te_short * 4);
    instance->encoder.upload[index++] =
        level_duration_make(false, (uint32_t)subghz_protocol_nero_radio_const.te_short);

    //Send key data
    for(uint8_t i = instance->generic.data_count_bit; i > 1; i--) {
        if(bit_read(instance->generic.data, i - 1)) {
            //send bit 1
            instance->encoder.upload[index++] =
                level_duration_make(true, (uint32_t)subghz_protocol_nero_radio_const.te_long);
            instance->encoder.upload[index++] =
                level_duration_make(false, (uint32_t)subghz_protocol_nero_radio_const.te_short);
        } else {
            //send bit 0
            instance->encoder.upload[index++] =
                level_duration_make(true, (uint32_t)subghz_protocol_nero_radio_const.te_short);
            instance->encoder.upload[index++] =
                level_duration_make(false, (uint32_t)subghz_protocol_nero_radio_const.te_long);
        }
    }
    if(bit_read(instance->generic.data, 0)) {
        //send bit 1
        instance->encoder.upload[index++] =
            level_duration_make(true, (uint32_t)subghz_protocol_nero_radio_const.te_long);
        instance->encoder.upload[index++] =
            level_duration_make(false, (uint32_t)subghz_protocol_nero_radio_const.te_short * 37);
    } else {
        //send bit 0
        instance->encoder.upload[index++] =
            level_duration_make(true, (uint32_t)subghz_protocol_nero_radio_const.te_short);
        instance->encoder.upload[index++] =
            level_duration_make(false, (uint32_t)subghz_protocol_nero_radio_const.te_short * 37);
    }
    return true;
}

bool subghz_protocol_encoder_nero_radio_load(
    void* context,
    uint64_t key,
    uint8_t count_bit,
    size_t repeat) {
    furi_assert(context);
    SubGhzProtocolEncoderNeroRadio* instance = context;
    instance->generic.data = key;
    instance->generic.data_count_bit = 56;
    instance->encoder.repeat = repeat;
    subghz_protocol_nero_radio_encoder_get_upload(instance);
    instance->encoder.is_runing = true;
    return true;
}

void subghz_protocol_encoder_nero_radio_stop(void* context) {
    SubGhzProtocolEncoderNeroRadio* instance = context;
    instance->encoder.is_runing = false;
}

LevelDuration subghz_protocol_encoder_nero_radio_yield(void* context) {
    SubGhzProtocolEncoderNeroRadio* instance = context;

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

void* subghz_protocol_decoder_nero_radio_alloc(SubGhzEnvironment* environment) {
    SubGhzProtocolDecoderNeroRadio* instance = furi_alloc(sizeof(SubGhzProtocolDecoderNeroRadio));
    instance->base.protocol = &subghz_protocol_nero_radio;
    instance->generic.protocol_name = instance->base.protocol->name;
    return instance;
}

void subghz_protocol_decoder_nero_radio_free(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderNeroRadio* instance = context;
    free(instance);
}

void subghz_protocol_decoder_nero_radio_reset(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderNeroRadio* instance = context;
    instance->decoder.parser_step = NeroRadioDecoderStepReset;
}

void subghz_protocol_decoder_nero_radio_feed(void* context, bool level, uint32_t duration) {
    furi_assert(context);
    SubGhzProtocolDecoderNeroRadio* instance = context;

    switch(instance->decoder.parser_step) {
    case NeroRadioDecoderStepReset:
        if((level) && (DURATION_DIFF(duration, subghz_protocol_nero_radio_const.te_short) <
                       subghz_protocol_nero_radio_const.te_delta)) {
            instance->decoder.parser_step = NeroRadioDecoderStepCheckPreambula;
            instance->decoder.te_last = duration;
            instance->header_count = 0;
        }
        break;
    case NeroRadioDecoderStepCheckPreambula:
        if(level) {
            if((DURATION_DIFF(duration, subghz_protocol_nero_radio_const.te_short) <
                subghz_protocol_nero_radio_const.te_delta) ||
               (DURATION_DIFF(duration, subghz_protocol_nero_radio_const.te_short * 4) <
                subghz_protocol_nero_radio_const.te_delta)) {
                instance->decoder.te_last = duration;
            } else {
                instance->decoder.parser_step = NeroRadioDecoderStepReset;
            }
        } else if(
            DURATION_DIFF(duration, subghz_protocol_nero_radio_const.te_short) <
            subghz_protocol_nero_radio_const.te_delta) {
            if(DURATION_DIFF(instance->decoder.te_last, subghz_protocol_nero_radio_const.te_short) <
               subghz_protocol_nero_radio_const.te_delta) {
                // Found header
                instance->header_count++;
                break;
            } else if(
                DURATION_DIFF(
                    instance->decoder.te_last, subghz_protocol_nero_radio_const.te_short * 4) <
                subghz_protocol_nero_radio_const.te_delta) {
                // Found start bit
                if(instance->header_count > 40) {
                    instance->decoder.parser_step = NeroRadioDecoderStepSaveDuration;
                    instance->decoder.decode_data = 0;
                    instance->decoder.decode_count_bit = 0;
                } else {
                    instance->decoder.parser_step = NeroRadioDecoderStepReset;
                }
            } else {
                instance->decoder.parser_step = NeroRadioDecoderStepReset;
            }
        } else {
            instance->decoder.parser_step = NeroRadioDecoderStepReset;
        }
        break;
    case NeroRadioDecoderStepSaveDuration:
        if(level) {
            instance->decoder.te_last = duration;
            instance->decoder.parser_step = NeroRadioDecoderStepCheckDuration;
        } else {
            instance->decoder.parser_step = NeroRadioDecoderStepReset;
        }
        break;
    case NeroRadioDecoderStepCheckDuration:
        if(!level) {
            if(duration >= (subghz_protocol_nero_radio_const.te_short * 10 +
                            subghz_protocol_nero_radio_const.te_delta * 2)) {
                //Found stop bit
                if(DURATION_DIFF(
                       instance->decoder.te_last, subghz_protocol_nero_radio_const.te_short) <
                   subghz_protocol_nero_radio_const.te_delta) {
                    subghz_protocol_blocks_add_bit(&instance->decoder, 0);
                } else if(
                    DURATION_DIFF(
                        instance->decoder.te_last, subghz_protocol_nero_radio_const.te_long) <
                    subghz_protocol_nero_radio_const.te_delta) {
                    subghz_protocol_blocks_add_bit(&instance->decoder, 1);
                }
                instance->decoder.parser_step = NeroRadioDecoderStepReset;
                if(instance->decoder.decode_count_bit >=
                   subghz_protocol_nero_radio_const.min_count_bit_for_found) {
                    instance->generic.data = instance->decoder.decode_data;
                    instance->generic.data_count_bit = instance->decoder.decode_count_bit;

                    if(instance->base.callback)
                        instance->base.callback(&instance->base, instance->base.context);
                }
                instance->decoder.decode_data = 0;
                instance->decoder.decode_count_bit = 0;
                instance->decoder.parser_step = NeroRadioDecoderStepReset;
                break;
            } else if(
                (DURATION_DIFF(
                     instance->decoder.te_last, subghz_protocol_nero_radio_const.te_short) <
                 subghz_protocol_nero_radio_const.te_delta) &&
                (DURATION_DIFF(duration, subghz_protocol_nero_radio_const.te_long) <
                 subghz_protocol_nero_radio_const.te_delta)) {
                subghz_protocol_blocks_add_bit(&instance->decoder, 0);
                instance->decoder.parser_step = NeroRadioDecoderStepSaveDuration;
            } else if(
                (DURATION_DIFF(
                     instance->decoder.te_last, subghz_protocol_nero_radio_const.te_long) <
                 subghz_protocol_nero_radio_const.te_delta) &&
                (DURATION_DIFF(duration, subghz_protocol_nero_radio_const.te_short) <
                 subghz_protocol_nero_radio_const.te_delta)) {
                subghz_protocol_blocks_add_bit(&instance->decoder, 1);
                instance->decoder.parser_step = NeroRadioDecoderStepSaveDuration;
            } else {
                instance->decoder.parser_step = NeroRadioDecoderStepReset;
            }
        } else {
            instance->decoder.parser_step = NeroRadioDecoderStepReset;
        }
        break;
    }
}

void subghz_protocol_decoder_nero_radio_serialization(void* context, string_t output) {
    furi_assert(context);
    SubGhzProtocolDecoderNeroRadio* instance = context;

    uint32_t code_found_hi = instance->generic.data >> 32;
    uint32_t code_found_lo = instance->generic.data & 0x00000000ffffffff;

    uint64_t code_found_reverse = subghz_protocol_blocks_reverse_key(
        instance->generic.data, instance->generic.data_count_bit);

    uint32_t code_found_reverse_hi = code_found_reverse >> 32;
    uint32_t code_found_reverse_lo = code_found_reverse & 0x00000000ffffffff;

    string_cat_printf(
        output,
        "%s %dbit\r\n"
        "Key:0x%lX%08lX\r\n"
        "Yek:0x%lX%08lX\r\n",
        instance->generic.protocol_name,
        instance->generic.data_count_bit,
        code_found_hi,
        code_found_lo,
        code_found_reverse_hi,
        code_found_reverse_lo);
}

bool subghz_protocol_nero_radio_save_file(void* context, FlipperFile* flipper_file) {
    furi_assert(context);
    SubGhzProtocolDecoderNeroRadio* instance = context;
    return subghz_block_generic_save_file(&instance->generic, flipper_file);
}

bool subghz_protocol_nero_radio_load_file(
    void* context,
    FlipperFile* flipper_file,
    const char* file_path) {
    furi_assert(context);
    SubGhzProtocolEncoderNeroRadio* instance = context;
    return subghz_block_generic_load_file(&instance->generic, flipper_file);
}

// void subghz_decoder_nero_radio_to_load_protocol(SubGhzProtocolNeroRadio* instance, void* context) {
//     furi_assert(context);
//     furi_assert(instance);
//     SubGhzProtocolCommonLoad* data = context;
//     instance->generic.data = data->code_found;
//     instance->generic.data_count_bit = data->code_count_bit;
// }