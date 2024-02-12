#include "kedsum_th.h"

#define TAG "WSProtocolKedsumTH"

/*
 * Help
 * https://github.com/merbanan/rtl_433/blob/master/src/devices/kedsum.c
 *
 *  Frame structure:
 * 
 *     Byte:      0        1        2        3        4
 *     Nibble:    1   2    3   4    5   6    7   8    9   10
 *     Type:   00 IIIIIIII BBCC++++ ttttTTTT hhhhHHHH FFFFXXXX
 * 
 * - I: unique id. changes on powercycle
 * - B: Battery state 10 = Ok, 01 = weak, 00 = bad
 * - C: channel, 00 = ch1, 10=ch3
 * - + low temp nibble
 * - t: med temp nibble
 * - T: high temp nibble
 * - h: humidity low nibble
 * - H: humidity high nibble
 * - F: flags
 * - X: CRC-4 poly 0x3 init 0x0 xor last 4 bits
 */

static const SubGhzBlockConst ws_protocol_kedsum_th_const = {
    .te_short = 500,
    .te_long = 2000,
    .te_delta = 150,
    .min_count_bit_for_found = 42,
};

struct WSProtocolDecoderKedsumTH {
    SubGhzProtocolDecoderBase base;

    SubGhzBlockDecoder decoder;
    WSBlockGeneric generic;

    uint16_t header_count;
};

struct WSProtocolEncoderKedsumTH {
    SubGhzProtocolEncoderBase base;

    SubGhzProtocolBlockEncoder encoder;
    WSBlockGeneric generic;
};

typedef enum {
    KedsumTHDecoderStepReset = 0,
    KedsumTHDecoderStepCheckPreambule,
    KedsumTHDecoderStepSaveDuration,
    KedsumTHDecoderStepCheckDuration,
} KedsumTHDecoderStep;

const SubGhzProtocolDecoder ws_protocol_kedsum_th_decoder = {
    .alloc = ws_protocol_decoder_kedsum_th_alloc,
    .free = ws_protocol_decoder_kedsum_th_free,

    .feed = ws_protocol_decoder_kedsum_th_feed,
    .reset = ws_protocol_decoder_kedsum_th_reset,

    .get_hash_data = ws_protocol_decoder_kedsum_th_get_hash_data,
    .serialize = ws_protocol_decoder_kedsum_th_serialize,
    .deserialize = ws_protocol_decoder_kedsum_th_deserialize,
    .get_string = ws_protocol_decoder_kedsum_th_get_string,
};

const SubGhzProtocolEncoder ws_protocol_kedsum_th_encoder = {
    .alloc = NULL,
    .free = NULL,

    .deserialize = NULL,
    .stop = NULL,
    .yield = NULL,
};

const SubGhzProtocol ws_protocol_kedsum_th = {
    .name = WS_PROTOCOL_KEDSUM_TH_NAME,
    .type = SubGhzProtocolTypeStatic,
    .flag = SubGhzProtocolFlag_433 | SubGhzProtocolFlag_315 | SubGhzProtocolFlag_868 |
            SubGhzProtocolFlag_AM | SubGhzProtocolFlag_Decodable | SubGhzProtocolFlag_Load |
            SubGhzProtocolFlag_Save,

    .decoder = &ws_protocol_kedsum_th_decoder,
    .encoder = &ws_protocol_kedsum_th_encoder,

    .filter = SubGhzProtocolFilter_Weather,
};

void* ws_protocol_decoder_kedsum_th_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    WSProtocolDecoderKedsumTH* instance = malloc(sizeof(WSProtocolDecoderKedsumTH));
    instance->base.protocol = &ws_protocol_kedsum_th;
    instance->generic.protocol_name = instance->base.protocol->name;
    return instance;
}

void ws_protocol_decoder_kedsum_th_free(void* context) {
    furi_assert(context);
    WSProtocolDecoderKedsumTH* instance = context;
    free(instance);
}

void ws_protocol_decoder_kedsum_th_reset(void* context) {
    furi_assert(context);
    WSProtocolDecoderKedsumTH* instance = context;
    instance->decoder.parser_step = KedsumTHDecoderStepReset;
}

static bool ws_protocol_kedsum_th_check_crc(WSProtocolDecoderKedsumTH* instance) {
    uint8_t msg[] = {
        instance->decoder.decode_data >> 32,
        instance->decoder.decode_data >> 24,
        instance->decoder.decode_data >> 16,
        instance->decoder.decode_data >> 8,
        instance->decoder.decode_data};

    uint8_t crc =
        subghz_protocol_blocks_crc4(msg, 4, 0x03, 0); // CRC-4 poly 0x3 init 0x0 xor last 4 bits
    crc ^= msg[4] >> 4; // last nibble is only XORed
    return (crc == (msg[4] & 0x0F));
}

/**
 * Analysis of received data
 * @param instance Pointer to a WSBlockGeneric* instance
 */
static void ws_protocol_kedsum_th_remote_controller(WSBlockGeneric* instance) {
    instance->id = instance->data >> 32;
    if((instance->data >> 30) & 0x3) {
        instance->battery_low = 0;
    } else {
        instance->battery_low = 1;
    }
    instance->channel = ((instance->data >> 28) & 0x3) + 1;
    instance->btn = WS_NO_BTN;
    uint16_t temp_raw = ((instance->data >> 16) & 0x0f) << 8 |
                        ((instance->data >> 20) & 0x0f) << 4 | ((instance->data >> 24) & 0x0f);
    instance->temp = locale_fahrenheit_to_celsius(((float)temp_raw - 900.0f) / 10.0f);
    instance->humidity = ((instance->data >> 8) & 0x0f) << 4 | ((instance->data >> 12) & 0x0f);
}

void ws_protocol_decoder_kedsum_th_feed(void* context, bool level, uint32_t duration) {
    furi_assert(context);
    WSProtocolDecoderKedsumTH* instance = context;

    switch(instance->decoder.parser_step) {
    case KedsumTHDecoderStepReset:
        if((level) && (DURATION_DIFF(duration, ws_protocol_kedsum_th_const.te_short) <
                       ws_protocol_kedsum_th_const.te_delta)) {
            instance->decoder.parser_step = KedsumTHDecoderStepCheckPreambule;
            instance->decoder.te_last = duration;
            instance->header_count = 0;
        }
        break;

    case KedsumTHDecoderStepCheckPreambule:
        if(level) {
            instance->decoder.te_last = duration;
        } else {
            if((DURATION_DIFF(instance->decoder.te_last, ws_protocol_kedsum_th_const.te_short) <
                ws_protocol_kedsum_th_const.te_delta) &&
               (DURATION_DIFF(duration, ws_protocol_kedsum_th_const.te_long * 4) <
                ws_protocol_kedsum_th_const.te_delta * 4)) {
                //Found preambule
                instance->header_count++;
            } else if(
                (DURATION_DIFF(instance->decoder.te_last, ws_protocol_kedsum_th_const.te_short) <
                 ws_protocol_kedsum_th_const.te_delta) &&
                (duration < (ws_protocol_kedsum_th_const.te_long * 2 +
                             ws_protocol_kedsum_th_const.te_delta * 2))) {
                //Found syncPrefix
                if(instance->header_count > 0) {
                    instance->decoder.parser_step = KedsumTHDecoderStepSaveDuration;
                    instance->decoder.decode_data = 0;
                    instance->decoder.decode_count_bit = 0;
                    if((DURATION_DIFF(
                            instance->decoder.te_last, ws_protocol_kedsum_th_const.te_short) <
                        ws_protocol_kedsum_th_const.te_delta) &&
                       (DURATION_DIFF(duration, ws_protocol_kedsum_th_const.te_long) <
                        ws_protocol_kedsum_th_const.te_delta * 2)) {
                        subghz_protocol_blocks_add_bit(&instance->decoder, 0);
                        instance->decoder.parser_step = KedsumTHDecoderStepSaveDuration;
                    } else if(
                        (DURATION_DIFF(
                             instance->decoder.te_last, ws_protocol_kedsum_th_const.te_short) <
                         ws_protocol_kedsum_th_const.te_delta) &&
                        (DURATION_DIFF(duration, ws_protocol_kedsum_th_const.te_long * 2) <
                         ws_protocol_kedsum_th_const.te_delta * 4)) {
                        subghz_protocol_blocks_add_bit(&instance->decoder, 1);
                        instance->decoder.parser_step = KedsumTHDecoderStepSaveDuration;
                    } else {
                        instance->decoder.parser_step = KedsumTHDecoderStepReset;
                    }
                }
            } else {
                instance->decoder.parser_step = KedsumTHDecoderStepReset;
            }
        }
        break;

    case KedsumTHDecoderStepSaveDuration:
        if(level) {
            instance->decoder.te_last = duration;
            instance->decoder.parser_step = KedsumTHDecoderStepCheckDuration;
        } else {
            instance->decoder.parser_step = KedsumTHDecoderStepReset;
        }
        break;

    case KedsumTHDecoderStepCheckDuration:
        if(!level) {
            if(DURATION_DIFF(duration, ws_protocol_kedsum_th_const.te_long * 4) <
               ws_protocol_kedsum_th_const.te_delta * 4) {
                //Found syncPostfix
                if((instance->decoder.decode_count_bit ==
                    ws_protocol_kedsum_th_const.min_count_bit_for_found) &&
                   ws_protocol_kedsum_th_check_crc(instance)) {
                    instance->generic.data = instance->decoder.decode_data;
                    instance->generic.data_count_bit = instance->decoder.decode_count_bit;
                    ws_protocol_kedsum_th_remote_controller(&instance->generic);
                    if(instance->base.callback)
                        instance->base.callback(&instance->base, instance->base.context);
                }
                instance->decoder.decode_data = 0;
                instance->decoder.decode_count_bit = 0;
                instance->decoder.parser_step = KedsumTHDecoderStepReset;
                break;
            } else if(
                (DURATION_DIFF(instance->decoder.te_last, ws_protocol_kedsum_th_const.te_short) <
                 ws_protocol_kedsum_th_const.te_delta) &&
                (DURATION_DIFF(duration, ws_protocol_kedsum_th_const.te_long) <
                 ws_protocol_kedsum_th_const.te_delta * 2)) {
                subghz_protocol_blocks_add_bit(&instance->decoder, 0);
                instance->decoder.parser_step = KedsumTHDecoderStepSaveDuration;
            } else if(
                (DURATION_DIFF(instance->decoder.te_last, ws_protocol_kedsum_th_const.te_short) <
                 ws_protocol_kedsum_th_const.te_delta) &&
                (DURATION_DIFF(duration, ws_protocol_kedsum_th_const.te_long * 2) <
                 ws_protocol_kedsum_th_const.te_delta * 4)) {
                subghz_protocol_blocks_add_bit(&instance->decoder, 1);
                instance->decoder.parser_step = KedsumTHDecoderStepSaveDuration;
            } else {
                instance->decoder.parser_step = KedsumTHDecoderStepReset;
            }
        } else {
            instance->decoder.parser_step = KedsumTHDecoderStepReset;
        }
        break;
    }
}

uint32_t ws_protocol_decoder_kedsum_th_get_hash_data(void* context) {
    furi_assert(context);
    WSProtocolDecoderKedsumTH* instance = context;
    return subghz_protocol_blocks_get_hash_data_long(
        &instance->decoder, (instance->decoder.decode_count_bit / 8) + 1);
}

SubGhzProtocolStatus ws_protocol_decoder_kedsum_th_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset) {
    furi_assert(context);
    WSProtocolDecoderKedsumTH* instance = context;
    return ws_block_generic_serialize(&instance->generic, flipper_format, preset);
}

SubGhzProtocolStatus
    ws_protocol_decoder_kedsum_th_deserialize(void* context, FlipperFormat* flipper_format) {
    furi_assert(context);
    WSProtocolDecoderKedsumTH* instance = context;
    return ws_block_generic_deserialize_check_count_bit(
        &instance->generic, flipper_format, ws_protocol_kedsum_th_const.min_count_bit_for_found);
}

void ws_protocol_decoder_kedsum_th_get_string(void* context, FuriString* output) {
    furi_assert(context);
    WSProtocolDecoderKedsumTH* instance = context;
    furi_string_cat_printf(
        output,
        "%s\r\n%dbit\r\n"
        "Key:0x%lX%08lX\r\n"
        "Sn:0x%lX Ch:%d  Bat:%d\r\n"
        "Temp:%3.1f C Hum:%d%%",
        instance->generic.protocol_name,
        instance->generic.data_count_bit,
        (uint32_t)(instance->generic.data >> 32),
        (uint32_t)(instance->generic.data),
        instance->generic.id,
        instance->generic.channel,
        instance->generic.battery_low,
        (double)instance->generic.temp,
        instance->generic.humidity);
}
