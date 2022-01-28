#include "subghz_protocol_came.h"
//#include "subghz_protocol_common.h"
#include "../subghz_protocol_blocks.h"

/*
 * Help
 * https://phreakerclub.com/447
 *
 */

static const SubGhzProtocolBlockConst subghz_protocol_came_const = {
    .te_short = 320,
    .te_long = 640,
    .te_delta = 150,
    .code_min_count_bit_for_found = 12,
};

struct SubGhzProtocolCameDecoder {
    SubGhzProtocolBlockDecoder decoder;
    SubGhzProtocolBlockRuntime runtime;
};
// struct SubGhzProtocolCame {
//     SubGhzProtocolCommon common;
// };

typedef enum {
    CameDecoderStepReset = 0,
    CameDecoderStepFoundStartBit,
    CameDecoderStepSaveDuration,
    CameDecoderStepCheckDuration,
} CameDecoderStep;

// SubGhzProtocolCame* subghz_protocol_came_alloc() {
//     SubGhzProtocolCame* instance = furi_alloc(sizeof(SubGhzProtocolCame));

//     instance->common.name = "CAME";
//     instance->common.code_min_count_bit_for_found = 12;
//     instance->common.te_short = 320;
//     instance->common.te_long = 640;
//     instance->common.te_delta = 150;
//     instance->common.type_protocol = SubGhzProtocolCommonTypeStatic;
//     instance->common.to_string = (SubGhzProtocolCommonToStr)subghz_protocol_came_to_str;
//     instance->common.to_save_file =
//         (SubGhzProtocolCommonSaveFile)subghz_protocol_came_to_save_file;
//     instance->common.to_load_protocol_from_file =
//         (SubGhzProtocolCommonLoadFromFile)subghz_protocol_came_to_load_protocol_from_file;
//     instance->common.to_load_protocol =
//         (SubGhzProtocolCommonLoadFromRAW)subghz_decoder_came_to_load_protocol;
//     instance->common.get_upload_protocol =
//         (SubGhzProtocolCommonEncoderGetUpLoad)subghz_protocol_came_send_key;

//     return instance;
// }

void* subghz_protocol_came_decoder_alloc() {
    SubGhzProtocolCameDecoder* instance = furi_alloc(sizeof(SubGhzProtocolCameDecoder));
    return instance;
}


void subghz_protocol_came_decoder_free(void* context) {
    furi_assert(context);
    SubGhzProtocolCameDecoder* instance = context;
    free(instance);
}

// bool subghz_protocol_came_send_key(
//     SubGhzProtocolCame* instance,
//     SubGhzProtocolCommonEncoder* encoder) {
//     furi_assert(instance);
//     furi_assert(encoder);
//     size_t index = 0;
//     encoder->size_upload = (instance->common.code_last_count_bit * 2) + 2;
//     if(encoder->size_upload > SUBGHZ_ENCODER_UPLOAD_MAX_SIZE) return false;
//     //Send header
//     encoder->upload[index++] =
//         level_duration_make(false, (uint32_t)instance->common.te_short * 36);
//     //Send start bit
//     encoder->upload[index++] = level_duration_make(true, (uint32_t)instance->common.te_short);
//     //Send key data
//     for(uint8_t i = instance->common.code_last_count_bit; i > 0; i--) {
//         if(bit_read(instance->common.code_last_found, i - 1)) {
//             //send bit 1
//             encoder->upload[index++] =
//                 level_duration_make(false, (uint32_t)instance->common.te_long);
//             encoder->upload[index++] =
//                 level_duration_make(true, (uint32_t)instance->common.te_short);
//         } else {
//             //send bit 0
//             encoder->upload[index++] =
//                 level_duration_make(false, (uint32_t)instance->common.te_short);
//             encoder->upload[index++] =
//                 level_duration_make(true, (uint32_t)instance->common.te_long);
//         }
//     }
//     return true;
// }

void subghz_protocol_came_decoder_reset(void* context) {
    furi_assert(context);
    SubGhzProtocolCameDecoder* instance = context;
   instance->decoder.parser_step = CameDecoderStepReset;
}

void subghz_protocol_came_decoder_feed(void* context, bool level, uint32_t duration) {
    furi_assert(context);
    SubGhzProtocolCameDecoder* instance = context;
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
        } else if(DURATION_DIFF(duration, subghz_protocol_came_const.te_short) < subghz_protocol_came_const.te_delta) {
            //Found start bit CAME
            instance->decoder.parser_step = CameDecoderStepSaveDuration;
            instance->runtime.code_found = 0;
            instance->runtime.code_count_bit = 0;
        } else {
            instance->decoder.parser_step = CameDecoderStepReset;
        }
        break;
    case CameDecoderStepSaveDuration:
        if(!level) { //save interval
            if(duration >= (subghz_protocol_came_const.te_short * 4)) {
                instance->decoder.parser_step = CameDecoderStepFoundStartBit;
                if(instance->runtime.code_count_bit >=
                   subghz_protocol_came_const.code_min_count_bit_for_found) {
                    instance->runtime.serial = 0x0;
                    instance->runtime.btn = 0x0;

                    instance->runtime.code_last_found = instance->runtime.code_found;
                    instance->runtime.code_last_count_bit = instance->runtime.code_count_bit;

                    if(instance->decoder.callback)
                        instance->decoder.callback(
                            instance, instance->decoder.context);
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
               (DURATION_DIFF(duration, subghz_protocol_came_const.te_long) < subghz_protocol_came_const.te_delta)) {
                subghz_protocol_blocks_add_bit(&instance->runtime, 0);
                instance->decoder.parser_step = CameDecoderStepSaveDuration;
            } else if(
                (DURATION_DIFF(instance->decoder.te_last, subghz_protocol_came_const.te_long) <
                 subghz_protocol_came_const.te_delta) &&
                (DURATION_DIFF(duration, subghz_protocol_came_const.te_short) < subghz_protocol_came_const.te_delta)) {
                subghz_protocol_blocks_add_bit(&instance->runtime, 1);
               instance->decoder.parser_step = CameDecoderStepSaveDuration;
            } else
                instance->decoder.parser_step = CameDecoderStepReset;
        } else {
            instance->decoder.parser_step = CameDecoderStepReset;
        }
        break;
    }
}

void subghz_protocol_came_to_str(void* context, string_t output) {
    furi_assert(context);
    SubGhzProtocolCameDecoder* instance = context;

    uint32_t code_found_lo = instance->runtime.code_last_found & 0x00000000ffffffff;

    uint64_t code_found_reverse = subghz_protocol_common_reverse_key(
        instance->runtime.code_last_found, instance->runtime.code_last_count_bit);

    uint32_t code_found_reverse_lo = code_found_reverse & 0x00000000ffffffff;

    string_cat_printf(
        output,
        "%s %dbit\r\n"
        "Key:0x%08lX\r\n"
        "Yek:0x%08lX\r\n",
        SUBGHZ_PROTOCOL_CAME_NAME,
        instance->runtime.code_last_count_bit,
        code_found_lo,
        code_found_reverse_lo);
}

// bool subghz_protocol_came_to_save_file(SubGhzProtocolCame* instance, FlipperFile* flipper_file) {
//     return subghz_protocol_common_to_save_file((SubGhzProtocolCommon*)instance, flipper_file);
// }

// bool subghz_protocol_came_to_load_protocol_from_file(
//     FlipperFile* flipper_file,
//     SubGhzProtocolCame* instance,
//     const char* file_path) {
//     return subghz_protocol_common_to_load_protocol_from_file(
//         (SubGhzProtocolCommon*)instance, flipper_file);
// }

// void subghz_decoder_came_to_load_protocol(SubGhzProtocolCame* instance, void* context) {
//     furi_assert(context);
//     furi_assert(instance);
//     SubGhzProtocolCommonLoad* data = context;
//     instance->common.code_last_found = data->code_found;
//     instance->common.code_last_count_bit = data->code_count_bit;
// }
