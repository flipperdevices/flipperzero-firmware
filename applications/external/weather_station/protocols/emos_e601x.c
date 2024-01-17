#include "emos_e601x.h"

#define TAG "WSProtocolEmosE601x"

/*
 * Help
 * https://github.com/merbanan/rtl_433/blob/master/src/devices/emos_e6016.c
 *
 * Data Layout:
 *
 *     PP PP PP II ?K KK KK KK CT TT HH SS DF XX RR
 *
 * - P: (24 bit) preamble
 * - I: (8 bit) ID
 * - ?: (2 bit) unknown
 * - K: (32 bit) datetime, fields are 6d-4d-5d 5d:6d:6d
 * - C: (2 bit) channel
 * - T: (12 bit) temperature, signed, scale 10
 * - H: (8 bit) humidity
 * - S: (8 bit) wind speed
 * - D: (4 bit) wind direction
 * - F: (4 bit) flags of (?B??), B is battery good indication
 * - X: (8 bit) checksum
 * - R: (8 bit) repeat counter
 */

static const SubGhzBlockConst ws_protocol_emose601x_const = {
    .te_short = 260,
    .te_long = 800,
    .te_delta = 100,
    .min_count_bit_for_found = 24,
};

#define MAGIC_HEADER 0xaaa583

struct WSProtocolDecoderEmosE601x {
    SubGhzProtocolDecoderBase base;

    SubGhzBlockDecoder decoder;
    WSBlockGeneric generic;
    uint64_t upper_decode_data;
};

struct WSProtocolEncoderEmosE601x {
    SubGhzProtocolEncoderBase base;

    SubGhzProtocolBlockEncoder encoder;
    WSBlockGeneric generic;
};

typedef enum {
    EmosE601xDecoderStepReset = 0,
    EmosE601xDecoderStepCheckPreamble,
    EmosE601xDecoderStepSaveDuration,
    EmosE601xDecoderStepCheckDuration,
} EmosE601xDecoderStep;

const SubGhzProtocolDecoder ws_protocol_emose601x_decoder = {
    .alloc = ws_protocol_decoder_emose601x_alloc,
    .free = ws_protocol_decoder_emose601x_free,

    .feed = ws_protocol_decoder_emose601x_feed,
    .reset = ws_protocol_decoder_emose601x_reset,

    .get_hash_data = ws_protocol_decoder_emose601x_get_hash_data,
    .serialize = ws_protocol_decoder_emose601x_serialize,
    .deserialize = ws_protocol_decoder_emose601x_deserialize,
    .get_string = ws_protocol_decoder_emose601x_get_string,
};

const SubGhzProtocolEncoder ws_protocol_emose601x_encoder = {
    .alloc = NULL,
    .free = NULL,

    .deserialize = NULL,
    .stop = NULL,
    .yield = NULL,
};

const SubGhzProtocol ws_protocol_emose601x = {
    .name = WS_PROTOCOL_EMOSE601X_NAME,
    .type = SubGhzProtocolWeatherStation,
    .flag = SubGhzProtocolFlag_433 | SubGhzProtocolFlag_315 | SubGhzProtocolFlag_868 |
            SubGhzProtocolFlag_AM | SubGhzProtocolFlag_Decodable,

    .decoder = &ws_protocol_emose601x_decoder,
    .encoder = &ws_protocol_emose601x_encoder,
};

void* ws_protocol_decoder_emose601x_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    WSProtocolDecoderEmosE601x* instance = malloc(sizeof(WSProtocolDecoderEmosE601x));
    instance->base.protocol = &ws_protocol_emose601x;
    instance->generic.protocol_name = instance->base.protocol->name;
    return instance;
}

void ws_protocol_decoder_emose601x_free(void* context) {
    furi_assert(context);
    WSProtocolDecoderEmosE601x* instance = context;
    free(instance);
}

void ws_protocol_decoder_emose601x_reset(void* context) {
    furi_assert(context);
    WSProtocolDecoderEmosE601x* instance = context;
    instance->decoder.parser_step = EmosE601xDecoderStepReset;
}

static bool ws_protocol_emose601x_check(WSProtocolDecoderEmosE601x* instance) {
    uint8_t msg[] = {
        instance->upper_decode_data >> 48,
        instance->upper_decode_data >> 40,
        instance->upper_decode_data >> 32,
        instance->upper_decode_data >> 24,
        instance->upper_decode_data >> 16,
        instance->upper_decode_data >> 8,
        instance->upper_decode_data,
        instance->decoder.decode_data >> 56,
        instance->decoder.decode_data >> 48,
        instance->decoder.decode_data >> 40,
        instance->decoder.decode_data >> 32,
        instance->decoder.decode_data >> 24,
        instance->decoder.decode_data >> 16};

    uint8_t sum = subghz_protocol_blocks_add_bytes(msg, 13);
    return (sum == ((instance->decoder.decode_data >> 8) & 0xff));
}

/**
 * Analysis of received data
 * @param instance Pointer to a WSBlockGeneric* instance
 */
static void ws_protocol_emose601x_extract_data(WSProtocolDecoderEmosE601x* instance) {
    instance->generic.id = (instance->upper_decode_data >> 24) & 0xff;
    instance->generic.battery_low = (instance->decoder.decode_data >> 10) & 1;
    instance->generic.btn = WS_NO_BTN;
    int16_t temp = (instance->decoder.decode_data >> 40) & 0xfff;
    /* Handle signed data */
    if(temp & 0x800) {
        temp |= 0xf000;
    }
    instance->generic.temp = (float)temp / 10.0;
    instance->generic.humidity = (instance->decoder.decode_data >> 32) & 0xff;
    instance->generic.channel = (instance->decoder.decode_data >> 52) & 0x03;
    instance->generic.data = (instance->decoder.decode_data >> 16);
}

void ws_protocol_decoder_emose601x_feed(void* context, bool level, uint32_t duration) {
    furi_assert(context);
    WSProtocolDecoderEmosE601x* instance = context;

    switch(instance->decoder.parser_step) {
    case EmosE601xDecoderStepReset:
        if((level) && (DURATION_DIFF(duration, ws_protocol_emose601x_const.te_short * 7) <
                       ws_protocol_emose601x_const.te_delta * 2)) {
            instance->decoder.parser_step = EmosE601xDecoderStepCheckPreamble;
            instance->decoder.te_last = duration;
        }
        break;

    case EmosE601xDecoderStepCheckPreamble:
        if(level) {
            instance->decoder.te_last = duration;
        } else {
            if((DURATION_DIFF(instance->decoder.te_last, ws_protocol_emose601x_const.te_short * 7) <
                ws_protocol_emose601x_const.te_delta * 2) &&
               (DURATION_DIFF(duration, ws_protocol_emose601x_const.te_short) <
                ws_protocol_emose601x_const.te_delta)) {
                //Found preamble
                instance->decoder.parser_step = EmosE601xDecoderStepSaveDuration;
                instance->decoder.decode_data = 0;
                instance->decoder.decode_count_bit = 0;
            } else {
                instance->decoder.parser_step = EmosE601xDecoderStepReset;
            }
        }
        break;

    case EmosE601xDecoderStepSaveDuration:
        if(level) {
            instance->decoder.te_last = duration;
            instance->decoder.parser_step = EmosE601xDecoderStepCheckDuration;
        } else {
            instance->decoder.parser_step = EmosE601xDecoderStepReset;
        }
        break;

    case EmosE601xDecoderStepCheckDuration:
        if(!level) {
            if((DURATION_DIFF(instance->decoder.te_last, ws_protocol_emose601x_const.te_short) <
                ws_protocol_emose601x_const.te_delta) &&
               (DURATION_DIFF(duration, ws_protocol_emose601x_const.te_long) <
                ws_protocol_emose601x_const.te_delta)) {
                subghz_protocol_blocks_add_to_128_bit(
                    &instance->decoder, 0, &instance->upper_decode_data);
                instance->decoder.parser_step = EmosE601xDecoderStepSaveDuration;
            } else if(
                (DURATION_DIFF(instance->decoder.te_last, ws_protocol_emose601x_const.te_long) <
                 ws_protocol_emose601x_const.te_delta) &&
                (DURATION_DIFF(duration, ws_protocol_emose601x_const.te_short) <
                 ws_protocol_emose601x_const.te_delta)) {
                subghz_protocol_blocks_add_to_128_bit(
                    &instance->decoder, 1, &instance->upper_decode_data);
                instance->decoder.parser_step = EmosE601xDecoderStepSaveDuration;
            } else {
                instance->decoder.parser_step = EmosE601xDecoderStepReset;
                break;
            }

            /* Bail out if the header doesn't match */
            if(instance->decoder.decode_count_bit ==
               ws_protocol_emose601x_const.min_count_bit_for_found) {
                if(instance->decoder.decode_data != MAGIC_HEADER) {
                    instance->decoder.parser_step = EmosE601xDecoderStepReset;
                    break;
                }
            }

            if(instance->decoder.decode_count_bit == 120) {
                instance->generic.data_count_bit = instance->decoder.decode_count_bit;
                if(ws_protocol_emose601x_check(instance)) {
                    ws_protocol_emose601x_extract_data(instance);
                    if(instance->base.callback)
                        instance->base.callback(&instance->base, instance->base.context);
                }
                break;
            }
        } else {
            instance->decoder.parser_step = EmosE601xDecoderStepReset;
        }
        break;
    }
}

uint8_t ws_protocol_decoder_emose601x_get_hash_data(void* context) {
    furi_assert(context);
    WSProtocolDecoderEmosE601x* instance = context;
    return subghz_protocol_blocks_get_hash_data(
        &instance->decoder, (instance->decoder.decode_count_bit / 8) + 1);
}

SubGhzProtocolStatus ws_protocol_decoder_emose601x_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset) {
    furi_assert(context);
    WSProtocolDecoderEmosE601x* instance = context;
    return ws_block_generic_serialize(&instance->generic, flipper_format, preset);
}

SubGhzProtocolStatus
    ws_protocol_decoder_emose601x_deserialize(void* context, FlipperFormat* flipper_format) {
    furi_assert(context);
    WSProtocolDecoderEmosE601x* instance = context;
    return ws_block_generic_deserialize_check_count_bit(
        &instance->generic, flipper_format, ws_protocol_emose601x_const.min_count_bit_for_found);
}

void ws_protocol_decoder_emose601x_get_string(void* context, FuriString* output) {
    furi_assert(context);
    WSProtocolDecoderEmosE601x* instance = context;
    furi_string_printf(
        output,
        "%s %dbit\r\n"
        "Code:0x%ld\r\n"
        "Ch:%d  Bat:%d\r\n"
        "Temp:%3.1f C Hum:%d%%",
        instance->generic.protocol_name,
        instance->generic.data_count_bit,
        instance->generic.id,
        instance->generic.channel,
        instance->generic.battery_low,
        (double)instance->generic.temp,
        instance->generic.humidity);
}
