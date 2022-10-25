#include "kaku.h"
#include "keeloq_common.h"

#include <m-string.h>
#include <m-array.h>

#include "../blocks/const.h"
#include "../blocks/decoder.h"
#include "../blocks/encoder.h"
#include "../blocks/generic.h"
#include "../blocks/math.h"

#define TAG "SubGhzProtocolKaku"

static const SubGhzBlockConst subghz_protocol_kaku_const = {
    .te_short = 300,
    .te_long = 4 * 300,
    .te_delta = 110,
    .min_count_bit_for_found = 32,
};

//        _   _
// '0':  | |_| |____  (T,T,T,3T)
//        _      _
// '1':  | |____| |_  (T,3T,T,T)
//        _   _
// dimm: | |_| |_     (T,T,T,T)
//
// T = short period = 275 µs
// long period = 4*T
//
// A frame is either 32 or 36 bits:
// start pulse (T high, 9T low)
// 26   address
// 1    group-bit
// 1    on/off/[dimm]
// 4    unit (multiple channels on single transmitter)
// [4]  [dimm level]
// stop pulse (T high, then low)

struct SubGhzProtocolDecoderKaku {
    SubGhzProtocolDecoderBase base;

    SubGhzBlockDecoder decoder;
    SubGhzBlockGeneric generic;
    // Fields from generic. For reference and to indicate their usage
    // const char* protocol_name;
    // uint64_t data;             - Raw data bits
    // uint32_t serial;           - KaKu address
    // uint8_t data_count_bit;    - Raw data bit count
    // uint8_t btn;               - KaKu unit (0-15)
    // uint32_t cnt;              - KaKu mode (0: off, 1: on)

    // Receiving variables
    bool is_dimm_cmd;

    // Decoding variables
    bool is_group_cmd;
    uint32_t dimm_value;
};

struct SubGhzProtocolEncoderKaku {
    SubGhzProtocolEncoderBase base;

    SubGhzProtocolBlockEncoder encoder;
    SubGhzBlockGeneric generic;
};

typedef enum {
    KakuDecoderStepReset = 0,
    KakuDecoderStepReceiving,
    KakuDecoderStepRecvdHalf,
    KakuDecoderStepRecvdFull,
} KakuDecoderStep;

const SubGhzProtocolDecoder subghz_protocol_kaku_decoder = {
    .alloc = subghz_protocol_decoder_kaku_alloc,
    .free = subghz_protocol_decoder_kaku_free,

    .feed = subghz_protocol_decoder_kaku_feed,
    .reset = subghz_protocol_decoder_kaku_reset,

    .get_hash_data = subghz_protocol_decoder_kaku_get_hash_data,
    .serialize = subghz_protocol_decoder_kaku_serialize,
    .deserialize = subghz_protocol_decoder_kaku_deserialize,
    .get_string = subghz_protocol_decoder_kaku_get_string,
};

const SubGhzProtocolEncoder subghz_protocol_kaku_encoder = {
    .alloc = subghz_protocol_encoder_kaku_alloc,
    .free = subghz_protocol_encoder_kaku_free,

    .deserialize = subghz_protocol_encoder_kaku_deserialize,
    .stop = subghz_protocol_encoder_kaku_stop,
    .yield = subghz_protocol_encoder_kaku_yield,
};

const SubGhzProtocol subghz_protocol_kaku = {
    .name = SUBGHZ_PROTOCOL_KAKU_NAME,
    .type = SubGhzProtocolTypeStatic,
    .flag = SubGhzProtocolFlag_433 | SubGhzProtocolFlag_AM | SubGhzProtocolFlag_Decodable |
            SubGhzProtocolFlag_Save | SubGhzProtocolFlag_Load | SubGhzProtocolFlag_Send,

    .decoder = &subghz_protocol_kaku_decoder,
    .encoder = &subghz_protocol_kaku_encoder,
};

/**
 * Analysis of received data
 * @param instance Pointer to a SubGhzBlockGeneric* instance
 */
static void subghz_protocol_kaku_check_remote_controller(SubGhzProtocolDecoderKaku* instance) {
    // AAAAAAAAAAAAAAAAAAAAAAAAAAGMUUUUDDDD
    // Address, Group mode, Mode (off/on/dimm), Unit, Dimm level

    instance->generic.serial = (instance->generic.data >> 10) & 0x3FFFFFF;
    instance->is_group_cmd = (instance->generic.data >> 9) & 0x1;
    instance->generic.cnt = (instance->generic.data >> 8) & 0x1;
    instance->generic.btn = (instance->generic.data >> 4) & 0xF;
    instance->dimm_value = (instance->generic.data >> 0) & 0xF;
}

void subghz_protocol_encoder_kaku_upload_pulse(
    LevelDuration* data,
    size_t* index,
    size_t on_time,
    size_t off_time) {
    LevelDuration ld1 = level_duration_make(true, subghz_protocol_kaku_const.te_short * on_time);
    LevelDuration ld2 = level_duration_make(false, subghz_protocol_kaku_const.te_short * off_time);

    FURI_LOG_D(
        TAG,
        "Upload pulse %d: %dT (lvl:%d, dur:%d), %d: %dT (lvl:%d, dur:%d)",
        *index,
        on_time,
        ld1.level,
        ld1.duration,
        (*index) + 1,
        off_time,
        ld2.level,
        ld2.duration);

    //FURI_LOG_D(TAG, "Upload pulse %d: %dT (lvl:%d, dur:%d)", *index, off_time, ld2.level, ld2.duration);

    data[(*index)++] = ld1;
    data[(*index)++] = ld2;
}

/**
 * Generating an upload from data.
 * @param instance Pointer to a SubGhzProtocolEncoderKaku instance
 * @return true On success
 */
static bool subghz_protocol_encoder_kaku_get_upload(SubGhzProtocolEncoderKaku* instance) {
    furi_assert(instance);

    size_t index = 0;
    size_t size_upload = 1 + 2 + (instance->generic.data_count_bit * 4) + 2;
    if(size_upload > instance->encoder.size_upload) {
        FURI_LOG_E(TAG, "Size upload exceeds allocated encoder buffer.");
        return false;
    } else {
        instance->encoder.size_upload = size_upload;
    }

    //FURI_LOG_D(TAG, "Upload size: %d, data: %lx", instance->generic.data_count_bit, instance->generic.data);

    // Start with low signal
    instance->encoder.upload[index++] =
        level_duration_make(false, subghz_protocol_kaku_const.te_short * 9);

    // Send Start bit
    subghz_protocol_encoder_kaku_upload_pulse(instance->encoder.upload, &index, 1, 9);

    //Send key data
    for(uint8_t i = 0; i < instance->generic.data_count_bit; ++i) {
        if(instance->generic.data_count_bit == 36 && i == 27) {
            // Send mode 'dimm'
            subghz_protocol_encoder_kaku_upload_pulse(instance->encoder.upload, &index, 1, 1);
            subghz_protocol_encoder_kaku_upload_pulse(instance->encoder.upload, &index, 1, 1);
        } else if(bit_read(instance->generic.data, 35 - i)) {
            //send bit 1
            subghz_protocol_encoder_kaku_upload_pulse(instance->encoder.upload, &index, 1, 4);
            subghz_protocol_encoder_kaku_upload_pulse(instance->encoder.upload, &index, 1, 1);
        } else {
            //send bit 0
            subghz_protocol_encoder_kaku_upload_pulse(instance->encoder.upload, &index, 1, 1);
            subghz_protocol_encoder_kaku_upload_pulse(instance->encoder.upload, &index, 1, 4);
        }
    }

    //Send Stop bit
    subghz_protocol_encoder_kaku_upload_pulse(instance->encoder.upload, &index, 1, 9);

    return true;
}
// {{{ Decoder

// .alloc implementation
void* subghz_protocol_decoder_kaku_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    SubGhzProtocolDecoderKaku* instance = malloc(sizeof(SubGhzProtocolDecoderKaku));
    instance->base.protocol = &subghz_protocol_kaku;
    instance->generic.protocol_name = instance->base.protocol->name;

    return instance;
}

// .free implementation
void subghz_protocol_decoder_kaku_free(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderKaku* instance = context;

    free(instance);
}

// .reset implementation
void subghz_protocol_decoder_kaku_reset(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderKaku* instance = context;
    instance->decoder.parser_step = KakuDecoderStepReset;
}

// .feed implementation
void subghz_protocol_decoder_kaku_feed(void* context, bool level, uint32_t duration) {
    furi_assert(context);
    SubGhzProtocolDecoderKaku* instance = context;

    switch(instance->decoder.parser_step) {
    case KakuDecoderStepReset:
        if(!level) {
            if(DURATION_DIFF(duration, subghz_protocol_kaku_const.te_short * 9) <
               subghz_protocol_kaku_const.te_delta * 9) {
                instance->decoder.parser_step = KakuDecoderStepReceiving;
                instance->decoder.decode_data = 0;
                instance->decoder.decode_count_bit = 0;
                instance->is_dimm_cmd = 0;

                //FURI_LOG_D(TAG, "Start %d (state: %d, bits: %d)", duration, instance->decoder.parser_step, instance->decoder.decode_count_bit);
            }
        }
        break;
    case KakuDecoderStepReceiving:
        if(!level) {
            //FURI_LOG_D(TAG, "First half %d (state: %d, bits: %d, data: %08x)", duration, instance->decoder.parser_step, instance->decoder.decode_count_bit, instance->decoder.decode_count_bit);
            instance->decoder.te_last = duration;
            instance->decoder.parser_step = KakuDecoderStepRecvdHalf;
        }
        break;
    case KakuDecoderStepRecvdHalf:
        if(!level) {
            //FURI_LOG_D(TAG, "Second half %d (state: %d, bits: %d, data: %08x)", duration, instance->decoder.parser_step, instance->decoder.decode_count_bit, instance->decoder.decode_count_bit);
            if(DURATION_DIFF(duration, subghz_protocol_kaku_const.te_long) <
               subghz_protocol_kaku_const.te_delta * 4) {
                // xxx, long
                if(DURATION_DIFF(instance->decoder.te_last, subghz_protocol_kaku_const.te_long) <
                   subghz_protocol_kaku_const.te_delta * 4) {
                    // long, long
                    instance->decoder.parser_step = KakuDecoderStepReset;
                    //FURI_LOG_D(TAG, "Reset - long (%d), long (%d) (state: %d, bits: %d, data: %x)", instance->decoder.te_last, duration, instance->decoder.parser_step, instance->decoder.decode_count_bit, instance->decoder.decode_data);
                    break;
                } else if(
                    DURATION_DIFF(instance->decoder.te_last, subghz_protocol_kaku_const.te_short) <
                    subghz_protocol_kaku_const.te_delta) {
                    // short, long
                    subghz_protocol_blocks_add_bit(&instance->decoder, 0);
                    //FURI_LOG_D(TAG, "bit: %d, data: %x, count: %d", 0, instance->decoder.decode_data, instance->decoder.decode_count_bit);
                    instance->decoder.parser_step = KakuDecoderStepReceiving;
                } else {
                    instance->decoder.parser_step = KakuDecoderStepReset;
                    //FURI_LOG_D(TAG, "Reset - 1st half invalid (%d), long (%d) (state: %d, bits: %d, data: %x)", instance->decoder.te_last, duration, instance->decoder.parser_step, instance->decoder.decode_count_bit, instance->decoder.decode_data);
                    break;
                }
            } else if(
                DURATION_DIFF(duration, subghz_protocol_kaku_const.te_short) <
                subghz_protocol_kaku_const.te_delta) {
                if(DURATION_DIFF(instance->decoder.te_last, subghz_protocol_kaku_const.te_long) <
                   subghz_protocol_kaku_const.te_delta * 4) {
                    // long, short
                    subghz_protocol_blocks_add_bit(&instance->decoder, 1);
                    //FURI_LOG_D(TAG, "bit: %d, data: %x, count: %d", 1, instance->decoder.decode_data, instance->decoder.decode_count_bit);
                    instance->decoder.parser_step = KakuDecoderStepReceiving;
                } else if(
                    DURATION_DIFF(instance->decoder.te_last, subghz_protocol_kaku_const.te_short) <
                    subghz_protocol_kaku_const.te_delta) {
                    // short, short
                    instance->is_dimm_cmd = 1;
                    subghz_protocol_blocks_add_bit(&instance->decoder, 0);
                    //FURI_LOG_D(TAG, "bit: %d, data: %x, count: %d", 0, instance->decoder.decode_data, instance->decoder.decode_count_bit);
                    instance->decoder.parser_step = KakuDecoderStepReceiving;
                } else {
                    instance->decoder.parser_step = KakuDecoderStepReset;
                    //FURI_LOG_D(TAG, "Reset - 1st half invalid (%d), short (%d) (state: %d, bits: %d, data: %x)", instance->decoder.te_last, duration, instance->decoder.parser_step, instance->decoder.decode_count_bit, instance->decoder.decode_data);
                    break;
                }
            } else {
                instance->decoder.parser_step = KakuDecoderStepReset;
                //FURI_LOG_D(TAG, "Reset - 2nd half invalid (%d) (state: %d, bits: %d, data: %x)", instance->decoder.te_last, duration, instance->decoder.parser_step, instance->decoder.decode_count_bit, instance->decoder.decode_data);
                break;
            }

            if(((!instance->is_dimm_cmd) &&
                instance->decoder.decode_count_bit >=
                    subghz_protocol_kaku_const.min_count_bit_for_found) ||
               (instance->is_dimm_cmd &&
                instance->decoder.decode_count_bit >=
                    subghz_protocol_kaku_const.min_count_bit_for_found + 4)) {
                FURI_LOG_D(
                    TAG,
                    "Packet received dimm: %d (state: %d, bits: %d, data: %lx)",
                    instance->is_dimm_cmd,
                    instance->decoder.parser_step,
                    instance->decoder.decode_count_bit,
                    instance->decoder.decode_data);

                if(instance->decoder.decode_count_bit ==
                   subghz_protocol_kaku_const.min_count_bit_for_found) {
                    // Old protocol, shift to match new protocol
                    instance->decoder.decode_data <<= 4;
                    // We keep decode_count_bit unchanged, to use this to differentiate between 32 and 36 bits
                }

                FURI_LOG_D(TAG, "Shifted count: %d", instance->decoder.decode_count_bit);
                FURI_LOG_D(TAG, "data: %lx", instance->decoder.decode_data);

                if(instance->generic.data != instance->decoder.decode_data) {
                    instance->generic.data = instance->decoder.decode_data;
                    instance->generic.data_count_bit = instance->decoder.decode_count_bit;
                    if(instance->base.callback) {
                        instance->base.callback(&instance->base, instance->base.context);
                    }
                }

                instance->decoder.parser_step = KakuDecoderStepReset;
                break;
            }
        }
        break;
    }
}

// .get_hash_data implementation
uint8_t subghz_protocol_decoder_kaku_get_hash_data(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderKaku* instance = context;
    return subghz_protocol_blocks_get_hash_data(
        &instance->decoder, (instance->decoder.decode_count_bit / 8) + 1);
}

// .serialize implementation
bool subghz_protocol_decoder_kaku_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzPresetDefinition* preset) {
    furi_assert(context);
    SubGhzProtocolDecoderKaku* instance = context;
    subghz_protocol_kaku_check_remote_controller(instance);
    bool res = subghz_block_generic_serialize(&instance->generic, flipper_format, preset);

    if(res && !flipper_format_write_bool(flipper_format, "Group", &instance->is_group_cmd, 1)) {
        FURI_LOG_E(TAG, "Unable to add Group value");
        res = false;
    }

    if(res && !flipper_format_write_uint32(flipper_format, "Dimm", &instance->dimm_value, 1)) {
        FURI_LOG_E(TAG, "Unable to add Dimm value");
        res = false;
    }

    if(res &&
       instance->generic.data_count_bit != subghz_protocol_kaku_const.min_count_bit_for_found &&
       instance->generic.data_count_bit !=
           subghz_protocol_kaku_const.min_count_bit_for_found + 4) {
        FURI_LOG_E(TAG, "Wrong number of bits in key");
        res = false;
    }

    return res;
}

// .deserialize implementation
bool subghz_protocol_decoder_kaku_deserialize(void* context, FlipperFormat* flipper_format) {
    furi_assert(context);
    SubGhzProtocolDecoderKaku* instance = context;
    bool res = false;
    do {
        if(!subghz_block_generic_deserialize(&instance->generic, flipper_format)) {
            FURI_LOG_E(TAG, "Deserialize error");
            break;
        }
        if(!flipper_format_read_bool(flipper_format, "Group", &instance->is_group_cmd, 1)) {
            FURI_LOG_E(TAG, "Could not deserialize Group value");
            break;
        }
        if(!flipper_format_read_uint32(flipper_format, "Dimm", &instance->dimm_value, 1)) {
            FURI_LOG_E(TAG, "Could not deserialize Dimm value");
            break;
        }
        res = true;
    } while(false);

    FURI_LOG_D(
        TAG,
        "D Loaded size: %d, data: %lx",
        instance->generic.data_count_bit,
        instance->generic.data);

    return res;
}

// .get_string implementation
void subghz_protocol_decoder_kaku_get_string(void* context, string_t output) {
    furi_assert(context);
    SubGhzProtocolDecoderKaku* instance = context;

    subghz_protocol_kaku_check_remote_controller(instance);

    string_cat_printf(
        output,
        "%s %dbit\r\n"
        "Addr:%9d\r\n"
        "Unit:%2d    Group:%1d\r\n"
        "Cmd: %1d     Dimm:%2d\r\n",
        instance->generic.protocol_name,
        instance->generic.data_count_bit,
        instance->generic.serial,
        instance->generic.btn,
        instance->is_group_cmd,
        instance->dimm_value ? 2 : instance->generic.cnt,
        instance->dimm_value);
}

// }}}

// {{{ Encoder

// .alloc implementation
void* subghz_protocol_encoder_kaku_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    SubGhzProtocolEncoderKaku* instance = malloc(sizeof(SubGhzProtocolEncoderKaku));

    instance->base.protocol = &subghz_protocol_kaku;
    instance->generic.protocol_name = instance->base.protocol->name;

    //         _
    // Start: | |________ - 2 pulse
    //         _   _
    // '0'  : | |_| |____ - 4 pulses
    //         _      _
    // '1'  : | |____| |_ - 4 pulses
    //         _   _
    // dimm : | |_| |_    - 4 pulses
    //         _
    // Stop : | |________ - 2 pulse

    instance->encoder.repeat = 4;
    //                             low + Start + 36*bit + Stop
    instance->encoder.size_upload = 1 + 2 + 36 * 4 + 2;
    instance->encoder.upload = malloc(instance->encoder.size_upload * sizeof(LevelDuration));
    instance->encoder.is_running = false;
    return instance;
}

// .free implementation
void subghz_protocol_encoder_kaku_free(void* context) {
    furi_assert(context);
    SubGhzProtocolEncoderKaku* instance = context;
    free(instance->encoder.upload);
    free(instance);
}

// .deserialize implementation
bool subghz_protocol_encoder_kaku_deserialize(void* context, FlipperFormat* flipper_format) {
    furi_assert(context);
    SubGhzProtocolEncoderKaku* instance = context;
    bool res = false;

    do {
        if(!subghz_block_generic_deserialize(&instance->generic, flipper_format)) {
            FURI_LOG_E(TAG, "Deserialize error");
            break;
        }
        if(instance->generic.data_count_bit !=
               subghz_protocol_kaku_const.min_count_bit_for_found &&
           instance->generic.data_count_bit !=
               subghz_protocol_kaku_const.min_count_bit_for_found + 4) {
            FURI_LOG_E(TAG, "Wrong number of bits in key (must be 32 or 36)");
            break;
        }

        //optional parameter parameter
        //TODO: fix repeat first
        /*flipper_format_read_uint32(
            flipper_format, "Repeat", (uint32_t*)&instance->encoder.repeat, 1);
        */

        subghz_protocol_encoder_kaku_get_upload(instance);
        instance->encoder.is_running = true;

        res = true;
    } while(false);

    FURI_LOG_D(
        TAG,
        "E Loaded size: %d, data: %lx",
        instance->generic.data_count_bit,
        instance->generic.data);

    return res;
}

// .stop implementation
void subghz_protocol_encoder_kaku_stop(void* context) {
    FURI_LOG_D(TAG, "Encoder Stop");

    SubGhzProtocolEncoderKaku* instance = context;
    instance->encoder.is_running = false;
}

// .yield implementation
LevelDuration subghz_protocol_encoder_kaku_yield(void* context) {
    SubGhzProtocolEncoderKaku* instance = context;

    if(instance->encoder.repeat == 0 || !instance->encoder.is_running) {
        instance->encoder.is_running = false;
        FURI_LOG_D(TAG, "Encoder done");
        return level_duration_reset();
    }

    LevelDuration ret = instance->encoder.upload[instance->encoder.front];

    //FURI_LOG_D(TAG, "Encoder: front: %d/%d, rpt: %d, lvl: %d, dur: %d", instance->encoder.front, instance->encoder.size_upload, instance->encoder.repeat, ret.level, ret.duration);

    //TODO: make repeat work

    if(++instance->encoder.front == instance->encoder.size_upload) {
        instance->encoder.repeat--;
        instance->encoder.front = 0;
    }

    return ret;
}

// }}}
