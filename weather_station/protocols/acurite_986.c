#include "acurite_986.h"

#define TAG "WSProtocolAcurite_986"

/*
 * Help
 * https://github.com/merbanan/rtl_433/blob/5bef4e43133ac4c0e2d18d36f87c52b4f9458453/src/devices/acurite.c#L1644
 *
 *     0110 0100 | 1010 1011 | 0110 0010 | 0000 0000 | 0111 0110
 *     tttt tttt | IIII IIII | iiii iiii | nbuu uuuu | cccc cccc
 * - t: temperature in °F
 * - I: identification (high byte)
 * - i: identification (low byte)
 * - n: sensor number
 * - b: battery low flag to indicate low battery voltage
 * - u: unknown
 * - c: CRC (CRC-8 poly 0x07, little-endian)
 *
 *  bits are sent and shown above LSB first
 *  identification changes on battery switch
 */

static const SubGhzBlockConst ws_protocol_acurite_986_const = {
    .te_short = 800,
    .te_long = 1750,
    .te_delta = 50,
    .min_count_bit_for_found = 40,
};

struct WSProtocolDecoderAcurite_986 {
    SubGhzProtocolDecoderBase base;

    SubGhzBlockDecoder decoder;
    WSBlockGeneric generic;
};

struct WSProtocolEncoderAcurite_986 {
    SubGhzProtocolEncoderBase base;

    SubGhzProtocolBlockEncoder encoder;
    WSBlockGeneric generic;
};

typedef enum {
    Acurite_986DecoderStepReset = 0,
    Acurite_986DecoderStepSync1,
    Acurite_986DecoderStepSync2,
    Acurite_986DecoderStepSync3,
    Acurite_986DecoderStepSaveDuration,
    Acurite_986DecoderStepCheckDuration,
} Acurite_986DecoderStep;

const SubGhzProtocolDecoder ws_protocol_acurite_986_decoder = {
    .alloc = ws_protocol_decoder_acurite_986_alloc,
    .free = ws_protocol_decoder_acurite_986_free,

    .feed = ws_protocol_decoder_acurite_986_feed,
    .reset = ws_protocol_decoder_acurite_986_reset,

    .get_hash_data = ws_protocol_decoder_acurite_986_get_hash_data,
    .serialize = ws_protocol_decoder_acurite_986_serialize,
    .deserialize = ws_protocol_decoder_acurite_986_deserialize,
    .get_string = ws_protocol_decoder_acurite_986_get_string,
};

const SubGhzProtocolEncoder ws_protocol_acurite_986_encoder = {
    .alloc = NULL,
    .free = NULL,

    .deserialize = NULL,
    .stop = NULL,
    .yield = NULL,
};

const SubGhzProtocol ws_protocol_acurite_986 = {
    .name = WS_PROTOCOL_ACURITE_986_NAME,
    .type = SubGhzProtocolWeatherStation,
    .flag = SubGhzProtocolFlag_433 | SubGhzProtocolFlag_315 | SubGhzProtocolFlag_868 |
            SubGhzProtocolFlag_AM | SubGhzProtocolFlag_Decodable,

    .decoder = &ws_protocol_acurite_986_decoder,
    .encoder = &ws_protocol_acurite_986_encoder,
};

void* ws_protocol_decoder_acurite_986_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    WSProtocolDecoderAcurite_986* instance = malloc(sizeof(WSProtocolDecoderAcurite_986));
    instance->base.protocol = &ws_protocol_acurite_986;
    instance->generic.protocol_name = instance->base.protocol->name;
    return instance;
}

void ws_protocol_decoder_acurite_986_free(void* context) {
    furi_assert(context);
    WSProtocolDecoderAcurite_986* instance = context;
    free(instance);
}

void ws_protocol_decoder_acurite_986_reset(void* context) {
    furi_assert(context);
    WSProtocolDecoderAcurite_986* instance = context;
    instance->decoder.parser_step = Acurite_986DecoderStepReset;
}

static bool ws_protocol_acurite_986_check(WSProtocolDecoderAcurite_986* instance) {
    if(!instance->decoder.decode_data) return false;
    uint8_t msg[] = {
    instance->decoder.decode_data >> 32,
    instance->decoder.decode_data >> 24,
    instance->decoder.decode_data >> 16,
    instance->decoder.decode_data >> 8 };

    uint8_t crc = subghz_protocol_blocks_crc8(msg, 4, 0x07, 0x00);
    return (crc == (instance->decoder.decode_data & 0xFF));
}

/**
 * Analysis of received data
 * @param instance Pointer to a WSBlockGeneric* instance
 */
static void ws_protocol_acurite_986_remote_controller(WSBlockGeneric* instance) {
    int temp;

    instance->id = subghz_protocol_blocks_reverse_key(instance->data >> 24, 8);
    instance->id = (instance->id << 8) | subghz_protocol_blocks_reverse_key(instance->data >> 16, 8);
    instance->battery_low = (instance->data >> 14) & 1;
    instance->channel = ((instance->data >> 15) & 1) + 1;

    temp = subghz_protocol_blocks_reverse_key(instance->data >> 32, 8);
    if(temp & 0x80) {
        temp = -(temp & 0x7F);
    }
    instance->temp = locale_fahrenheit_to_celsius((float)temp);
    instance->btn = WS_NO_BTN;
    instance->humidity = WS_NO_HUMIDITY;
}

void ws_protocol_decoder_acurite_986_feed(void* context, bool level, uint32_t duration) {
    furi_assert(context);
    WSProtocolDecoderAcurite_986* instance = context;

    switch(instance->decoder.parser_step) {
    case Acurite_986DecoderStepReset:
        if((!level) && (DURATION_DIFF(duration, ws_protocol_acurite_986_const.te_long) <
                        ws_protocol_acurite_986_const.te_delta * 15)) {
            //Found 1st sync bit
            instance->decoder.parser_step = Acurite_986DecoderStepSync1;
            instance->decoder.decode_data = 0;
            instance->decoder.decode_count_bit = 0;
        }
        break;

    case Acurite_986DecoderStepSync1:
        if(DURATION_DIFF(duration, ws_protocol_acurite_986_const.te_long) <
                         ws_protocol_acurite_986_const.te_delta * 15) {
            if(!level) {
                instance->decoder.parser_step = Acurite_986DecoderStepSync2;
            }
        } else {
            instance->decoder.parser_step = Acurite_986DecoderStepReset;
        }
        break;

    case Acurite_986DecoderStepSync2:
        if(DURATION_DIFF(duration, ws_protocol_acurite_986_const.te_long) <
                         ws_protocol_acurite_986_const.te_delta * 15) {
            if(!level) {
                instance->decoder.parser_step = Acurite_986DecoderStepSync3;
            }
        } else {
            instance->decoder.parser_step = Acurite_986DecoderStepReset;
        }
        break;

    case Acurite_986DecoderStepSync3:
        if(DURATION_DIFF(duration, ws_protocol_acurite_986_const.te_long) <
                         ws_protocol_acurite_986_const.te_delta * 15) {
            if(!level) {
                instance->decoder.parser_step = Acurite_986DecoderStepSaveDuration;
            }
        } else {
            instance->decoder.parser_step = Acurite_986DecoderStepReset;
        }
        break;

    case Acurite_986DecoderStepSaveDuration:
        if(level) {
            instance->decoder.te_last = duration;
            instance->decoder.parser_step = Acurite_986DecoderStepCheckDuration;
        } else {
            instance->decoder.parser_step = Acurite_986DecoderStepReset;
        }
        break;

    case Acurite_986DecoderStepCheckDuration:
        if(!level) {
            if(DURATION_DIFF(duration, ws_protocol_acurite_986_const.te_short) <
                             ws_protocol_acurite_986_const.te_delta * 10) {
                if(duration < ws_protocol_acurite_986_const.te_short) {
                    subghz_protocol_blocks_add_bit(&instance->decoder, 0);
                    instance->decoder.parser_step = Acurite_986DecoderStepSaveDuration;
                } else {
                    subghz_protocol_blocks_add_bit(&instance->decoder, 1);
                    instance->decoder.parser_step = Acurite_986DecoderStepSaveDuration;
                }
            } else {
                //Found syncPostfix
                instance->decoder.parser_step = Acurite_986DecoderStepReset;
                if((instance->decoder.decode_count_bit == ws_protocol_acurite_986_const.min_count_bit_for_found) &&
                    ws_protocol_acurite_986_check(instance)) {
                    instance->generic.data = instance->decoder.decode_data;
                    instance->generic.data_count_bit = instance->decoder.decode_count_bit;
                    ws_protocol_acurite_986_remote_controller(&instance->generic);
                    if(instance->base.callback)
                        instance->base.callback(&instance->base, instance->base.context);
                }
                instance->decoder.decode_data = 0;
                instance->decoder.decode_count_bit = 0;
            }
        } else {
            instance->decoder.parser_step = Acurite_986DecoderStepReset;
        }
        break;
    }
}

uint8_t ws_protocol_decoder_acurite_986_get_hash_data(void* context) {
    furi_assert(context);
    WSProtocolDecoderAcurite_986* instance = context;
    return subghz_protocol_blocks_get_hash_data(
        &instance->decoder, (instance->decoder.decode_count_bit / 8) + 1);
}

SubGhzProtocolStatus ws_protocol_decoder_acurite_986_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset) {
    furi_assert(context);
    WSProtocolDecoderAcurite_986* instance = context;
    return ws_block_generic_serialize(&instance->generic, flipper_format, preset);
}

SubGhzProtocolStatus
    ws_protocol_decoder_acurite_986_deserialize(void* context, FlipperFormat* flipper_format) {
    furi_assert(context);
    WSProtocolDecoderAcurite_986* instance = context;
    return ws_block_generic_deserialize_check_count_bit(
        &instance->generic,
        flipper_format,
        ws_protocol_acurite_986_const.min_count_bit_for_found);
}

void ws_protocol_decoder_acurite_986_get_string(void* context, FuriString* output) {
    furi_assert(context);
    WSProtocolDecoderAcurite_986* instance = context;
    furi_string_printf(
        output,
        "%s %dbit\r\n"
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
