#include "acurite_5n1.h"

#define TAG "WSProtocolAcurite_5n1"

/*
 * Help
 * https://github.com/merbanan/rtl_433/blob/master/src/devices/acurite.c
 * 
 * Acurite 5n1 Wind Speed Temperature Humidity sensor decoder
 * Message Type 0x38, 8 bytes
 * | Byte 0    | Byte 1    | Byte 2    | Byte 3    | Byte 4    | Byte 5    | Byte 6    | Byte 7    |
 * | --------- | --------- | --------- | --------- | --------- | --------- | --------- | --------- |
 * | CCII IIII | IIII IIII | pB11 1000 | p??W WWWW | pWWW TTTT | pTTT TTTT | pHHH HHHH | KKKK KKKK |
 * - C: Channel 00: C, 10: B, 11: A, (01 is invalid)
 * - I: Device ID (14 bits)
 * - B: Battery, 1 is battery OK, 0 is battery low
 * - M: Message type (6 bits), 0x38
 * - W: Wind Speed (8 bits) 
 * - T: Temperature Fahrenheit (11 bits), + 400 * 10
 * - H: Relative Humidity (%) (7 bits)
 * - K: Checksum (8 bits)
 * - p: Parity bit
 * Notes:
 * - Temperature
 *   - Encoded as Fahrenheit + 400 * 10
 *   - only 11 bits needed for specified range -40 F - 158 F (-40 C to 70 C)
 * 
 */

static const SubGhzBlockConst ws_protocol_acurite_5n1_const = {
    .te_short = 200,
    .te_long = 400,
    .te_delta = 90,
    .min_count_bit_for_found = 64,
};

struct WSProtocolDecoderAcurite_5n1 {
    SubGhzProtocolDecoderBase base;

    SubGhzBlockDecoder decoder;
    WSBlockGeneric generic;

    uint16_t header_count;
};

struct WSProtocolEncoderAcurite_5n1 {
    SubGhzProtocolEncoderBase base;

    SubGhzProtocolBlockEncoder encoder;
    WSBlockGeneric generic;
};

typedef enum {
    Acurite_5n1DecoderStepReset = 0,
    Acurite_5n1DecoderStepCheckPreambule,
    Acurite_5n1DecoderStepSaveDuration,
    Acurite_5n1DecoderStepCheckDuration,
} Acurite_5n1DecoderStep;

const SubGhzProtocolDecoder ws_protocol_acurite_5n1_decoder = {
    .alloc = ws_protocol_decoder_acurite_5n1_alloc,
    .free = ws_protocol_decoder_acurite_5n1_free,

    .feed = ws_protocol_decoder_acurite_5n1_feed,
    .reset = ws_protocol_decoder_acurite_5n1_reset,

    .get_hash_data = ws_protocol_decoder_acurite_5n1_get_hash_data,
    .serialize = ws_protocol_decoder_acurite_5n1_serialize,
    .deserialize = ws_protocol_decoder_acurite_5n1_deserialize,
    .get_string = ws_protocol_decoder_acurite_5n1_get_string,
};

const SubGhzProtocolEncoder ws_protocol_acurite_5n1_encoder = {
    .alloc = NULL,
    .free = NULL,

    .deserialize = NULL,
    .stop = NULL,
    .yield = NULL,
};

const SubGhzProtocol ws_protocol_acurite_5n1 = {
    .name = WS_PROTOCOL_ACURITE_5N1_NAME,
    .type = SubGhzProtocolWeatherStation,
    .flag = SubGhzProtocolFlag_433 | SubGhzProtocolFlag_315 | SubGhzProtocolFlag_868 |
            SubGhzProtocolFlag_AM | SubGhzProtocolFlag_Decodable,

    .decoder = &ws_protocol_acurite_5n1_decoder,
    .encoder = &ws_protocol_acurite_5n1_encoder,
};

void* ws_protocol_decoder_acurite_5n1_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    WSProtocolDecoderAcurite_5n1* instance = malloc(sizeof(WSProtocolDecoderAcurite_5n1));
    instance->base.protocol = &ws_protocol_acurite_5n1;
    instance->generic.protocol_name = instance->base.protocol->name;
    return instance;
}

void ws_protocol_decoder_acurite_5n1_free(void* context) {
    furi_assert(context);
    WSProtocolDecoderAcurite_5n1* instance = context;
    free(instance);
}

void ws_protocol_decoder_acurite_5n1_reset(void* context) {
    furi_assert(context);
    WSProtocolDecoderAcurite_5n1* instance = context;
    instance->decoder.parser_step = Acurite_5n1DecoderStepReset;
}

static bool ws_protocol_acurite_5n1_check_crc(WSProtocolDecoderAcurite_5n1* instance) {
    uint8_t msg[] = {
        instance->decoder.decode_data >> 56,
        instance->decoder.decode_data >> 48,
        instance->decoder.decode_data >> 40,
        instance->decoder.decode_data >> 32,
        instance->decoder.decode_data >> 24,
        instance->decoder.decode_data >> 16,
        instance->decoder.decode_data >> 8};

    if((subghz_protocol_blocks_add_bytes(msg, 7) ==
        (uint8_t)(instance->decoder.decode_data & 0xFF)) &&
       (!subghz_protocol_blocks_parity_bytes(&msg[2], 5))) {
        return true;
    } else {
        return false;
    }
}

static bool ws_protocol_acurite_5n1_check_message_type(WSProtocolDecoderAcurite_5n1* instance) {
    if(((instance->decoder.decode_data >> 40) & 0x3F) == 0x38) {
        return true;
    } else {
        return false;
    }
}

/**
 * Analysis of received data
 * @param instance Pointer to a WSBlockGeneric* instance
 */
static void ws_protocol_acurite_5n1_remote_controller(WSBlockGeneric* instance) {
    uint8_t channel[] = {3, 0, 2, 1};
    uint8_t channel_raw = ((instance->data >> 62) & 0x03);
    instance->channel = channel[channel_raw];
    instance->id = (instance->data >> 48) & 0x3FFF;
    instance->battery_low = !((instance->data >> 46) & 1);
    instance->humidity = (instance->data >> 8) & 0x7F;

    uint16_t temp_raw = ((instance->data >> (24 - 7)) & 0x780) | ((instance->data >> 16) & 0x7F);
    instance->temp = locale_fahrenheit_to_celsius(((float)(temp_raw)-400) / 10.0f);

    instance->btn = WS_NO_BTN;
}

void ws_protocol_decoder_acurite_5n1_feed(void* context, bool level, uint32_t duration) {
    furi_assert(context);
    WSProtocolDecoderAcurite_5n1* instance = context;

    switch(instance->decoder.parser_step) {
    case Acurite_5n1DecoderStepReset:
        if((level) && (DURATION_DIFF(duration, ws_protocol_acurite_5n1_const.te_short * 3) <
                       ws_protocol_acurite_5n1_const.te_delta * 2)) {
            instance->decoder.parser_step = Acurite_5n1DecoderStepCheckPreambule;
            instance->decoder.te_last = duration;
            instance->header_count = 0;
        }
        break;

    case Acurite_5n1DecoderStepCheckPreambule:
        if(level) {
            instance->decoder.te_last = duration;
        } else {
            if((DURATION_DIFF(
                    instance->decoder.te_last, ws_protocol_acurite_5n1_const.te_short * 3) <
                ws_protocol_acurite_5n1_const.te_delta * 2) &&
               (DURATION_DIFF(duration, ws_protocol_acurite_5n1_const.te_short * 3) <
                ws_protocol_acurite_5n1_const.te_delta * 2)) {
                //Found preambule
                instance->header_count++;
            } else if((instance->header_count > 2) && (instance->header_count < 5)) {
                if((DURATION_DIFF(
                        instance->decoder.te_last, ws_protocol_acurite_5n1_const.te_short) <
                    ws_protocol_acurite_5n1_const.te_delta) &&
                   (DURATION_DIFF(duration, ws_protocol_acurite_5n1_const.te_long) <
                    ws_protocol_acurite_5n1_const.te_delta)) {
                    instance->decoder.decode_data = 0;
                    instance->decoder.decode_count_bit = 0;
                    subghz_protocol_blocks_add_bit(&instance->decoder, 0);
                    instance->decoder.parser_step = Acurite_5n1DecoderStepSaveDuration;
                } else if(
                    (DURATION_DIFF(
                         instance->decoder.te_last, ws_protocol_acurite_5n1_const.te_long) <
                     ws_protocol_acurite_5n1_const.te_delta) &&
                    (DURATION_DIFF(duration, ws_protocol_acurite_5n1_const.te_short) <
                     ws_protocol_acurite_5n1_const.te_delta)) {
                    instance->decoder.decode_data = 0;
                    instance->decoder.decode_count_bit = 0;
                    subghz_protocol_blocks_add_bit(&instance->decoder, 1);
                    instance->decoder.parser_step = Acurite_5n1DecoderStepSaveDuration;
                } else {
                    instance->decoder.parser_step = Acurite_5n1DecoderStepReset;
                }
            } else {
                instance->decoder.parser_step = Acurite_5n1DecoderStepReset;
            }
        }
        break;

    case Acurite_5n1DecoderStepSaveDuration:
        if(level) {
            instance->decoder.te_last = duration;
            instance->decoder.parser_step = Acurite_5n1DecoderStepCheckDuration;
        } else {
            instance->decoder.parser_step = Acurite_5n1DecoderStepReset;
        }
        break;

    case Acurite_5n1DecoderStepCheckDuration:
        if(!level) {
            if(duration >= ((uint32_t)ws_protocol_acurite_5n1_const.te_short * 5)) {
                if((instance->decoder.decode_count_bit ==
                    ws_protocol_acurite_5n1_const.min_count_bit_for_found) &&
                   ws_protocol_acurite_5n1_check_crc(instance) &&
                   ws_protocol_acurite_5n1_check_message_type(instance)) {
                    instance->generic.data = instance->decoder.decode_data;
                    instance->generic.data_count_bit = instance->decoder.decode_count_bit;
                    ws_protocol_acurite_5n1_remote_controller(&instance->generic);
                    if(instance->base.callback)
                        instance->base.callback(&instance->base, instance->base.context);
                }
                instance->decoder.decode_data = 0;
                instance->decoder.decode_count_bit = 0;
                instance->decoder.parser_step = Acurite_5n1DecoderStepReset;
                break;
            } else if(
                (DURATION_DIFF(instance->decoder.te_last, ws_protocol_acurite_5n1_const.te_short) <
                 ws_protocol_acurite_5n1_const.te_delta) &&
                (DURATION_DIFF(duration, ws_protocol_acurite_5n1_const.te_long) <
                 ws_protocol_acurite_5n1_const.te_delta)) {
                subghz_protocol_blocks_add_bit(&instance->decoder, 0);
                instance->decoder.parser_step = Acurite_5n1DecoderStepSaveDuration;
            } else if(
                (DURATION_DIFF(instance->decoder.te_last, ws_protocol_acurite_5n1_const.te_long) <
                 ws_protocol_acurite_5n1_const.te_delta) &&
                (DURATION_DIFF(duration, ws_protocol_acurite_5n1_const.te_short) <
                 ws_protocol_acurite_5n1_const.te_delta)) {
                subghz_protocol_blocks_add_bit(&instance->decoder, 1);
                instance->decoder.parser_step = Acurite_5n1DecoderStepSaveDuration;
            } else {
                instance->decoder.parser_step = Acurite_5n1DecoderStepReset;
            }
        } else {
            instance->decoder.parser_step = Acurite_5n1DecoderStepReset;
        }
        break;
    }
}

uint8_t ws_protocol_decoder_acurite_5n1_get_hash_data(void* context) {
    furi_assert(context);
    WSProtocolDecoderAcurite_5n1* instance = context;
    return subghz_protocol_blocks_get_hash_data(
        &instance->decoder, (instance->decoder.decode_count_bit / 8) + 1);
}

SubGhzProtocolStatus ws_protocol_decoder_acurite_5n1_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset) {
    furi_assert(context);
    WSProtocolDecoderAcurite_5n1* instance = context;
    return ws_block_generic_serialize(&instance->generic, flipper_format, preset);
}

SubGhzProtocolStatus
    ws_protocol_decoder_acurite_5n1_deserialize(void* context, FlipperFormat* flipper_format) {
    furi_assert(context);
    WSProtocolDecoderAcurite_5n1* instance = context;
    return ws_block_generic_deserialize_check_count_bit(
        &instance->generic, flipper_format, ws_protocol_acurite_5n1_const.min_count_bit_for_found);
}

void ws_protocol_decoder_acurite_5n1_get_string(void* context, FuriString* output) {
    furi_assert(context);
    WSProtocolDecoderAcurite_5n1* instance = context;
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
