#include "hormann_bisecur.h"

#include <lib/flipper_format/flipper_format_i.h>
#include <lib/toolbox/manchester_decoder.h>
#include <lib/toolbox/manchester_encoder.h>
#include <lib/toolbox/stream/stream.h>

#include "../blocks/const.h"
#include "../blocks/decoder.h"
#include "../blocks/encoder.h"
#include "../blocks/generic.h"
#include "../blocks/math.h"

#define TAG "SubGhzProtocolHormannBiSecur"

static const SubGhzBlockConst subghz_protocol_hormann_bisecur_const = {
    .te_short = 208,
    .te_long = 416,
    .te_delta = 104,
    .min_count_bit_for_found = 176,
};

struct SubGhzProtocolDecoderHormannBiSecur {
    SubGhzProtocolDecoderBase base;

    SubGhzBlockDecoder decoder;
    SubGhzBlockGeneric generic;

    uint8_t sync_cnt;
    ManchesterState manchester_saved_state;

    uint8_t type;
    uint8_t data[22];
    uint8_t crc;
};

struct SubGhzProtocolEncoderHormannBiSecur {
    SubGhzProtocolEncoderBase base;

    SubGhzProtocolBlockEncoder encoder;
    SubGhzBlockGeneric generic;

    uint8_t data[22];
};

typedef enum {
    HormannBiSecurDecoderStepReset = 0,
    HormannBiSecurDecoderStepFoundPreambleAlternatingShort,
    HormannBiSecurDecoderStepFoundPreambleHighVeryLong,
    HormannBiSecurDecoderStepFoundPreambleAlternatingLong,
    HormannBiSecurDecoderStepFoundData,
} HormannBiSecurDecoderStep;

const SubGhzProtocolDecoder subghz_protocol_hormann_bisecur_decoder = {
    .alloc = subghz_protocol_decoder_hormann_bisecur_alloc,
    .free = subghz_protocol_decoder_hormann_bisecur_free,

    .feed = subghz_protocol_decoder_hormann_bisecur_feed,
    .reset = subghz_protocol_decoder_hormann_bisecur_reset,

    .get_hash_data = NULL,
    .get_hash_data_long = subghz_protocol_decoder_hormann_bisecur_get_hash_data,
    .serialize = subghz_protocol_decoder_hormann_bisecur_serialize,
    .deserialize = subghz_protocol_decoder_hormann_bisecur_deserialize,
    .get_string = subghz_protocol_decoder_hormann_bisecur_get_string,
    .get_string_brief = subghz_protocol_decoder_hormann_bisecur_get_string_brief,
};

const SubGhzProtocolEncoder subghz_protocol_hormann_bisecur_encoder = {
    .alloc = subghz_protocol_encoder_hormann_bisecur_alloc,
    .free = subghz_protocol_encoder_hormann_bisecur_free,

    .deserialize = subghz_protocol_encoder_hormann_bisecur_deserialize,
    .stop = subghz_protocol_encoder_hormann_bisecur_stop,
    .yield = subghz_protocol_encoder_hormann_bisecur_yield,
};

const SubGhzProtocol subghz_protocol_hormann_bisecur = {
    .name = SUBGHZ_PROTOCOL_HORMANN_BISECUR_NAME,
    .type = SubGhzProtocolTypeDynamic,
    .flag = SubGhzProtocolFlag_868 | SubGhzProtocolFlag_FM | SubGhzProtocolFlag_Decodable |
            SubGhzProtocolFlag_Load | SubGhzProtocolFlag_Save | SubGhzProtocolFlag_Send,

    .decoder = &subghz_protocol_hormann_bisecur_decoder,
    .encoder = &subghz_protocol_hormann_bisecur_encoder,
};

// there is a problem with the function in lib/toolbox/manchester_decoder, so it is reimplemented
// thanks to CodeAllNight (https://github.com/jamisonderek) for sharing his fixed version
static const ManchesterState manchester_reset_state = ManchesterStateMid1;

bool manchester_advance_alt(
    ManchesterState state,
    ManchesterEvent event,
    ManchesterState* next_state,
    bool* data) {
    bool result = false;
    ManchesterState new_state = manchester_reset_state;

    if(event == ManchesterEventReset) {
        new_state = manchester_reset_state;
    } else if(state == ManchesterStateStart1) {
        if(event == ManchesterEventShortLow) {
            new_state = ManchesterStateMid1;
            result = true;
            if(data) *data = false;
        } else if(event == ManchesterEventLongLow) {
            new_state = ManchesterStateStart0;
            result = true;
            if(data) *data = false;
        } else {
            new_state = manchester_reset_state;
        }
    } else if(state == ManchesterStateMid1) {
        if(event == ManchesterEventShortHigh) {
            new_state = ManchesterStateStart1;
        } else if(event == ManchesterEventShortLow) {
            new_state = ManchesterStateStart0;
        } else {
            new_state = manchester_reset_state;
        }
    } else if(state == ManchesterStateStart0) {
        if(event == ManchesterEventShortHigh) {
            new_state = ManchesterStateMid0;
            result = true;
            if(data) *data = true;
        } else if(event == ManchesterEventLongHigh) {
            new_state = ManchesterStateStart1;
            result = true;
            if(data) *data = true;
        } else {
            new_state = manchester_reset_state;
        }
    } else if(state == ManchesterStateMid0) {
        if(event == ManchesterEventShortLow) {
            new_state = ManchesterStateStart0;
        } else if(event == ManchesterEventShortHigh) {
            new_state = ManchesterStateStart1;
        } else {
            new_state = manchester_reset_state;
        }
    }

    *next_state = new_state;
    return result;
}

/**
 * Calculates the next LevelDuration in an upload
 * @param result ManchesterEncoderResult
 * @return LevelDuration
 */
static LevelDuration
    subghz_protocol_encoder_hormann_bisecur_add_duration_to_upload(ManchesterEncoderResult result);

/**
 * Calculates CRC from the raw demodulated bytes
 * @param instance Pointer to a SubGhzProtocolDecoderHormannBiSecur instance
 */
static uint8_t
    subghz_protocol_decoder_hormann_bisecur_crc(SubGhzProtocolDecoderHormannBiSecur* instance);

/**
 * Checks if the raw demodulated data has correct CRC
 * @param instance Pointer to a SubGhzProtocolDecoderHormannBiSecur instance
 * @return if CRC is valid or not
 */
static bool subghz_protocol_decoder_hormann_bisecur_check_crc(
    SubGhzProtocolDecoderHormannBiSecur* instance);

/**
 * Add the next bit to the decoding result
 * @param instance Pointer to a SubGhzProtocolDecoderHormannBiSecur instance
 * @param level Level of the next bit
 */
static void subghz_protocol_decoder_hormann_bisecur_add_bit(
    SubGhzProtocolDecoderHormannBiSecur* instance,
    bool level);

/**
 * Parses the raw data into separate fields
 * @param instance Pointer to a SubGhzProtocolDecoderHormannBiSecur instance
 */
static void
    subghz_protocol_hormann_bisecur_parse_data(SubGhzProtocolDecoderHormannBiSecur* instance);

void* subghz_protocol_encoder_hormann_bisecur_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    SubGhzProtocolEncoderHormannBiSecur* instance =
        malloc(sizeof(SubGhzProtocolEncoderHormannBiSecur));

    instance->base.protocol = &subghz_protocol_hormann_bisecur;
    instance->generic.protocol_name = instance->base.protocol->name;

    // TODO insert 504.3ms carrier off delay between repeats
    // subghz framework does not support this for FM encoders
    // sending either frequency constantly also seems to work

    // instance->encoder.repeat = 3; //original remote does 3 repeats
    instance->encoder.repeat = 1;
    instance->encoder.size_upload =
        21 * 2 + 2 * 2 + subghz_protocol_hormann_bisecur_const.min_count_bit_for_found * 2 + 1;
    instance->encoder.upload = malloc(instance->encoder.size_upload * sizeof(LevelDuration));
    instance->encoder.is_running = false;
    return instance;
}

void subghz_protocol_encoder_hormann_bisecur_free(void* context) {
    furi_assert(context);
    SubGhzProtocolEncoderHormannBiSecur* instance = context;
    free(instance->encoder.upload);
    free(instance);
}

/**
 * Generating an upload from data.
 * @param instance Pointer to a SubGhzProtocolEncoderHormannBiSecur instance
 * @return true On success
 */
static bool subghz_protocol_encoder_hormann_bisecur_get_upload(
    SubGhzProtocolEncoderHormannBiSecur* instance) {
    furi_assert(instance);
    size_t index = 0;
    ManchesterEncoderState enc_state;
    manchester_encoder_reset(&enc_state);
    ManchesterEncoderResult result;

    uint32_t duration_short = (uint32_t)subghz_protocol_hormann_bisecur_const.te_short;
    uint32_t duration_long = (uint32_t)subghz_protocol_hormann_bisecur_const.te_long;
    uint32_t duration_half_short = duration_short / 2;

    // Send preamble
    for(uint8_t i = 0; i < 21; i++) {
        uint32_t duration_low = duration_short;
        uint32_t duration_high = duration_short;

        if(i == 0) {
            duration_low += duration_half_short;
        }

        if(i == 20) {
            duration_high = duration_long * 4;
        }

        instance->encoder.upload[index++] = level_duration_make(false, duration_low);
        instance->encoder.upload[index++] = level_duration_make(true, duration_high);
    }

    for(uint8_t i = 0; i < 2; i++) {
        instance->encoder.upload[index++] = level_duration_make(false, duration_long);
        instance->encoder.upload[index++] = level_duration_make(true, duration_long);
    }

    // Send key data
    uint8_t max_byte_index = instance->generic.data_count_bit / 8 - 1;

    for(uint8_t i = instance->generic.data_count_bit; i > 0; i--) {
        uint8_t bit_index = i - 1;
        uint8_t byte_index = max_byte_index - bit_index / 8;
        bool bit_is_set = !bit_read(instance->data[byte_index], bit_index & 0x07);

        if(!manchester_encoder_advance(&enc_state, bit_is_set, &result)) {
            instance->encoder.upload[index++] =
                subghz_protocol_encoder_hormann_bisecur_add_duration_to_upload(result);
            manchester_encoder_advance(&enc_state, bit_is_set, &result);
        }

        instance->encoder.upload[index++] =
            subghz_protocol_encoder_hormann_bisecur_add_duration_to_upload(result);
    }

    LevelDuration last_level_duration =
        subghz_protocol_encoder_hormann_bisecur_add_duration_to_upload(
            manchester_encoder_finish(&enc_state));

    last_level_duration.duration += duration_short + duration_half_short;
    instance->encoder.upload[index++] = last_level_duration;

    instance->encoder.size_upload = index;

    return true;
}

SubGhzProtocolStatus subghz_protocol_encoder_hormann_bisecur_deserialize(
    void* context,
    FlipperFormat* flipper_format) {
    furi_assert(context);
    SubGhzProtocolEncoderHormannBiSecur* instance = context;

    SubGhzProtocolStatus ret =
        subghz_block_generic_deserialize(&instance->generic, flipper_format);
    if(ret != SubGhzProtocolStatusOk) {
        return ret;
    }

    // Generic key is too small, so we reset it and rewind to get real, longer, data
    instance->generic.data = 0;

    if(instance->generic.data_count_bit !=
       subghz_protocol_hormann_bisecur_const.min_count_bit_for_found) {
        FURI_LOG_E(TAG, "Wrong number of bits in key");
        return SubGhzProtocolStatusErrorValueBitCount;
    }

    if(!flipper_format_rewind(flipper_format)) {
        FURI_LOG_E(TAG, "Rewind error");
        return SubGhzProtocolStatusErrorParserOthers;
    }

    size_t key_length = instance->generic.data_count_bit / 8;

    if(!flipper_format_read_hex(flipper_format, "Key", instance->data, key_length)) {
        FURI_LOG_E(TAG, "Unable to read Key in encoder");
        return SubGhzProtocolStatusErrorParserKey;
    }

    // optional parameter
    flipper_format_read_uint32(flipper_format, "Repeat", (uint32_t*)&instance->encoder.repeat, 1);

    if(!subghz_protocol_encoder_hormann_bisecur_get_upload(instance)) {
        return SubGhzProtocolStatusErrorEncoderGetUpload;
    }

    instance->encoder.is_running = true;

    return SubGhzProtocolStatusOk;
}

void subghz_protocol_encoder_hormann_bisecur_stop(void* context) {
    SubGhzProtocolEncoderHormannBiSecur* instance = context;
    instance->encoder.is_running = false;
}

LevelDuration subghz_protocol_encoder_hormann_bisecur_yield(void* context) {
    SubGhzProtocolEncoderHormannBiSecur* instance = context;

    if(instance->encoder.repeat == 0 || !instance->encoder.is_running) {
        instance->encoder.is_running = false;
        return level_duration_reset();
    }

    LevelDuration ret = instance->encoder.upload[instance->encoder.front];

    if(++instance->encoder.front == instance->encoder.size_upload) {
        instance->encoder.repeat--;
        instance->encoder.front = 0;
    }

    return ret;
}

void* subghz_protocol_decoder_hormann_bisecur_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    SubGhzProtocolDecoderHormannBiSecur* instance =
        malloc(sizeof(SubGhzProtocolDecoderHormannBiSecur));
    instance->base.protocol = &subghz_protocol_hormann_bisecur;
    instance->generic.protocol_name = instance->base.protocol->name;
    return instance;
}

void subghz_protocol_decoder_hormann_bisecur_free(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderHormannBiSecur* instance = context;
    free(instance);
}

void subghz_protocol_decoder_hormann_bisecur_reset(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderHormannBiSecur* instance = context;
    instance->decoder.parser_step = HormannBiSecurDecoderStepReset;
    memset(instance->data, 0, 22);
    instance->generic.data_count_bit = 0;
    instance->manchester_saved_state = 0;
}

void subghz_protocol_decoder_hormann_bisecur_feed(void* context, bool level, uint32_t duration) {
    furi_assert(context);
    SubGhzProtocolDecoderHormannBiSecur* instance = context;

    uint32_t duration_short = (uint32_t)subghz_protocol_hormann_bisecur_const.te_short;
    uint32_t duration_long = (uint32_t)subghz_protocol_hormann_bisecur_const.te_long;
    uint32_t duration_delta = (uint32_t)subghz_protocol_hormann_bisecur_const.te_delta;
    uint32_t duration_half_short = duration_short / 2;

    ManchesterEvent event = ManchesterEventReset;

    switch(instance->decoder.parser_step) {
    case HormannBiSecurDecoderStepReset:
        if(!level &&
           DURATION_DIFF(duration, duration_short + duration_half_short) < duration_delta) {
            instance->decoder.parser_step = HormannBiSecurDecoderStepFoundPreambleAlternatingShort;
        }
        break;
    case HormannBiSecurDecoderStepFoundPreambleAlternatingShort:
        if(DURATION_DIFF(duration, duration_short) < duration_delta) {
            // stay on the same step, the pattern repeats around 21 times
            break;
        }

        if(level && DURATION_DIFF(duration, duration_long * 4) < duration_delta) {
            instance->decoder.parser_step = HormannBiSecurDecoderStepFoundPreambleHighVeryLong;
            break;
        }

        instance->decoder.parser_step = HormannBiSecurDecoderStepReset;
        break;
    case HormannBiSecurDecoderStepFoundPreambleHighVeryLong:
        if(!level && DURATION_DIFF(duration, duration_long) < duration_delta) {
            instance->sync_cnt = 3;
            instance->decoder.parser_step = HormannBiSecurDecoderStepFoundPreambleAlternatingLong;
            break;
        }

        instance->decoder.parser_step = HormannBiSecurDecoderStepReset;
        break;
    case HormannBiSecurDecoderStepFoundPreambleAlternatingLong:
        if(level == (instance->sync_cnt-- & 1) &&
           DURATION_DIFF(duration, duration_long) < duration_delta) {
            if(!instance->sync_cnt) {
                manchester_advance_alt(
                    instance->manchester_saved_state,
                    event,
                    &instance->manchester_saved_state,
                    NULL);
                instance->decoder.parser_step = HormannBiSecurDecoderStepFoundData;
            }

            // stay on the same step, or advance to the next if enough transitions are found
            break;
        }

        instance->decoder.parser_step = HormannBiSecurDecoderStepReset;
        break;
    case HormannBiSecurDecoderStepFoundData:
        if(DURATION_DIFF(duration, duration_short) < duration_delta ||
           (
               // the last bit can be arbitrary long, but it is parsed as a short
               instance->generic.data_count_bit ==
                   subghz_protocol_hormann_bisecur_const.min_count_bit_for_found - 1 &&
               duration > duration_short)) {
            event = !level ? ManchesterEventShortHigh : ManchesterEventShortLow;
        }

        if(DURATION_DIFF(duration, duration_long) < duration_delta) {
            event = !level ? ManchesterEventLongHigh : ManchesterEventLongLow;
        }

        if(event == ManchesterEventReset) {
            subghz_protocol_decoder_hormann_bisecur_reset(instance);
        } else {
            bool new_level;

            if(manchester_advance_alt(
                   instance->manchester_saved_state,
                   event,
                   &instance->manchester_saved_state,
                   &new_level)) {
                subghz_protocol_decoder_hormann_bisecur_add_bit(instance, new_level);
            }
        }
        break;
    }
}

uint32_t subghz_protocol_decoder_hormann_bisecur_get_hash_data(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderHormannBiSecur* instance = context;

    union {
        uint32_t full;
        uint8_t split[4];
    } hash = {0};
    size_t key_length = instance->generic.data_count_bit / 8;

    for(size_t i = 0; i < key_length; i++) {
        hash.split[i % sizeof(hash)] ^= instance->data[i];
    }

    return hash.full;
}

SubGhzProtocolStatus subghz_protocol_decoder_hormann_bisecur_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset) {
    furi_assert(context);

    SubGhzProtocolDecoderHormannBiSecur* instance = context;
    SubGhzProtocolStatus res = SubGhzProtocolStatusError;

    do {
        res = subghz_block_generic_serialize(&instance->generic, flipper_format, preset);
        if(res != SubGhzProtocolStatusOk) {
            break;
        }

        // Generic key is too small, so it writes empty and we update here with real, longer, data
        if(!flipper_format_rewind(flipper_format)) {
            FURI_LOG_E(TAG, "Rewind error");
            res = SubGhzProtocolStatusErrorParserOthers;
            break;
        }

        uint16_t key_length = instance->generic.data_count_bit / 8;

        if(!flipper_format_update_hex(flipper_format, "Key", instance->data, key_length)) {
            FURI_LOG_E(TAG, "Unable to update Key");
            res = SubGhzProtocolStatusErrorParserKey;
            break;
        }
    } while(false);

    return res;
}

SubGhzProtocolStatus subghz_protocol_decoder_hormann_bisecur_deserialize(
    void* context,
    FlipperFormat* flipper_format) {
    furi_assert(context);
    SubGhzProtocolDecoderHormannBiSecur* instance = context;

    SubGhzProtocolStatus ret =
        subghz_block_generic_deserialize(&instance->generic, flipper_format);
    if(ret != SubGhzProtocolStatusOk) {
        return ret;
    }

    // Generic key is too small, so we reset it and rewind to get real, longer, data
    instance->generic.data = 0;

    if(instance->generic.data_count_bit !=
       subghz_protocol_hormann_bisecur_const.min_count_bit_for_found) {
        FURI_LOG_E(TAG, "Wrong number of bits in key");
        return SubGhzProtocolStatusErrorValueBitCount;
    }

    if(!flipper_format_rewind(flipper_format)) {
        FURI_LOG_E(TAG, "Rewind error");
        return SubGhzProtocolStatusErrorParserOthers;
    }

    size_t key_length = instance->generic.data_count_bit / 8;

    if(!flipper_format_read_hex(flipper_format, "Key", instance->data, key_length)) {
        FURI_LOG_E(TAG, "Unable to read Key in decoder");
        return SubGhzProtocolStatusErrorParserKey;
    }

    subghz_protocol_hormann_bisecur_parse_data(instance);

    return SubGhzProtocolStatusOk;
}

void subghz_protocol_decoder_hormann_bisecur_get_string(void* context, FuriString* output) {
    furi_assert(context);
    SubGhzProtocolDecoderHormannBiSecur* instance = context;
    bool valid_crc = subghz_protocol_decoder_hormann_bisecur_check_crc(instance);

    furi_string_cat_printf(
        output,
        "%s\r\n"
        "%dbit CRC:0x%02X %s\r\n"
        "Type:0x%02X Sn:0x%08lX\r\n"
        "Key:%016llX\r\n"
        "Key:%016llX\r\n",
        instance->generic.protocol_name,
        instance->generic.data_count_bit,
        instance->crc,
        valid_crc ? "OK" : "WRONG",
        instance->type,
        instance->generic.serial,
        instance->generic.data,
        instance->generic.data_2);
}

void subghz_protocol_decoder_hormann_bisecur_get_string_brief(void* context, FuriString* output) {
    furi_assert(context);
    SubGhzProtocolDecoderHormannBiSecur* instance = context;
    subghz_protocol_hormann_bisecur_parse_data(instance);
    bool valid_crc = subghz_protocol_decoder_hormann_bisecur_check_crc(instance);

    if(!valid_crc) {
        furi_string_cat_printf(output, "HBS Bad checksum");
        return;
    }

    uint8_t data_hash = subghz_protocol_blocks_xor_bytes(
        (const uint8_t*)&instance->generic.data, sizeof(uint64_t));
    uint8_t data_2_hash = subghz_protocol_blocks_xor_bytes(
        (const uint8_t*)&instance->generic.data_2, sizeof(uint64_t));

    furi_string_cat_printf(
        output, "HBS %08lX:%02X%02X", instance->generic.serial, data_hash, data_2_hash);
}

static LevelDuration subghz_protocol_encoder_hormann_bisecur_add_duration_to_upload(
    ManchesterEncoderResult result) {
    LevelDuration data = {.duration = 0, .level = 0};

    switch(result) {
    case ManchesterEncoderResultShortLow:
        data.duration = subghz_protocol_hormann_bisecur_const.te_short;
        data.level = false;
        break;
    case ManchesterEncoderResultLongLow:
        data.duration = subghz_protocol_hormann_bisecur_const.te_long;
        data.level = false;
        break;
    case ManchesterEncoderResultLongHigh:
        data.duration = subghz_protocol_hormann_bisecur_const.te_long;
        data.level = true;
        break;
    case ManchesterEncoderResultShortHigh:
        data.duration = subghz_protocol_hormann_bisecur_const.te_short;
        data.level = true;
        break;

    default:
        furi_crash("SubGhz: ManchesterEncoderResult is incorrect.");
        break;
    }

    return level_duration_make(data.level, data.duration);
}

static uint8_t
    subghz_protocol_decoder_hormann_bisecur_crc(SubGhzProtocolDecoderHormannBiSecur* instance) {
    furi_assert(instance);

    switch(instance->type) {
    case 0x50:
        return ~(subghz_protocol_blocks_crc8(instance->data + 1, 20, 0x07, 0x00) ^ 0x55);
    case 0x70:
        return subghz_protocol_blocks_crc8le(instance->data, 21, 0x07, 0xFF);
    }

    FURI_LOG_E(TAG, "Unknown type 0x%02X", instance->type);

    return 0;
}

static bool subghz_protocol_decoder_hormann_bisecur_check_crc(
    SubGhzProtocolDecoderHormannBiSecur* instance) {
    furi_assert(instance);

    if(instance->type != 0x50 && instance->type != 0x70) {
        FURI_LOG_W(TAG, "Unknown type 0x%02X", instance->type);
        return false;
    }

    return subghz_protocol_decoder_hormann_bisecur_crc(instance) == instance->crc;
}

static void
    subghz_protocol_hormann_bisecur_parse_data(SubGhzProtocolDecoderHormannBiSecur* instance) {
    furi_assert(instance);

    instance->type = instance->data[0];

    instance->generic.serial = 0;

    for(uint8_t i = 1; i < 5; i++) {
        instance->generic.serial = instance->generic.serial << 8 | instance->data[i];
    }

    instance->generic.data = 0;

    for(uint8_t i = 5; i < 13; i++) {
        instance->generic.data = instance->generic.data << 8 | instance->data[i];
    }

    instance->generic.data_2 = 0;

    for(uint8_t i = 13; i < 21; i++) {
        instance->generic.data_2 = instance->generic.data_2 << 8 | instance->data[i];
    }

    instance->crc = instance->data[21];
}

static void subghz_protocol_decoder_hormann_bisecur_add_bit(
    SubGhzProtocolDecoderHormannBiSecur* instance,
    bool level) {
    furi_assert(instance);

    if(instance->generic.data_count_bit >=
       subghz_protocol_hormann_bisecur_const.min_count_bit_for_found) {
        return;
    }

    if(level) {
        uint8_t byte_index = instance->generic.data_count_bit / 8;
        uint8_t bit_index = instance->generic.data_count_bit % 8;

        instance->data[byte_index] |= 1 << (7 - bit_index);
    }

    instance->generic.data_count_bit++;

    if(instance->generic.data_count_bit >=
       subghz_protocol_hormann_bisecur_const.min_count_bit_for_found) {
        if(instance->base.callback) {
            instance->base.callback(&instance->base, instance->base.context);
        } else {
            subghz_protocol_decoder_hormann_bisecur_reset(instance);
        }
    }
}
