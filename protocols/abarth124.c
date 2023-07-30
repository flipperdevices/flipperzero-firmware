#include "abarth124.h"

#define TAG "TPMSProtocolAbarth124"

/*
 * Help
 * https://github.com/merbanan/rtl_433/blob/master/src/devices/tpms_abarth124.c
 * 
 * Acurite 592TXR Temperature Humidity sensor decoder
 * Message Type 0x04, 7 bytes
 * | Byte 0    | Byte 1    | Byte 2    | Byte 3    | Byte 4    | Byte 5    | Byte 6    |
 * | --------- | --------- | --------- | --------- | --------- | --------- | --------- |
 * | CCII IIII | IIII IIII | pB00 0100 | pHHH HHHH | p??T TTTT | pTTT TTTT | KKKK KKKK |
 * - C: Channel 00: C, 10: B, 11: A, (01 is invalid)
 * - I: Device ID (14 bits)
 * - B: Battery, 1 is battery OK, 0 is battery low
 * - M: Message type (6 bits), 0x04
 * - T: Temperature Celsius (11 - 14 bits?), + 1000 * 10
 * - H: Relative Humidity (%) (7 bits)
 * - K: Checksum (8 bits)
 * - p: Parity bit
 * Notes:
 * - Temperature
 *   - Encoded as Celsius + 1000 * 10
 *   - only 11 bits needed for specified range -40 C to 70 C (-40 F - 158 F)
 *   - However 14 bits available for temperature, giving possible range of -100 C to 1538.4 C
 *   - @todo - check if high 3 bits ever used for anything else
 * 
 */

// https://github.com/merbanan/rtl_433/blob/master/src/devices/tpms_abarth124.c
// test signals https://github.com/merbanan/rtl_433/issues/1271

/**
(VDO Type TG1C via) Abarth 124 Spider TPMS decoded by TTigges
Protocol similar (and based on) Jansite Solar TPMS by Andreas Spiess and Christian W. Zuckschwerdt

OEM Sensor is said to be a VDO Type TG1C, available in different cars,
e.g.: Abarth 124 Spider, some Fiat 124 Spider, some Mazda MX-5 ND (and NC?) and probably some other Mazdas.
Mazda reference/part no.: BHB637140A
VDO reference/part no.: A2C1132410180

Compatible with aftermarket sensors, e.g. Aligator sens.it RS3

// Working Temperature: -50°C to 125°C
// Working Frequency: 433.92MHz+-38KHz
// Tire monitoring range value: 0kPa-350kPa+-7kPa (to be checked, VDO says 450/900kPa)

Data layout (nibbles):
    II II II II ?? PP TT SS CC
- I: 32 bit ID
- ?: 4 bit unknown (seems to change with status)
- ?: 4 bit unknown (seems static)
- P: 8 bit Pressure (multiplyed by 1.38 = kPa)
- T: 8 bit Temperature (deg. C offset by 50)
- S: Status? (first nibble seems static, second nibble seems to change with status)
- C: 8 bit Checksum (Checksum8 XOR on bytes 0 to 8)
- The preamble is 0xaa..aa9 (or 0x55..556 depending on polarity)
*/

static const SubGhzBlockConst tpms_protocol_abarth124_const = {
    .te_short = 200,
    .te_long = 400,
    .te_delta = 90,
    .min_count_bit_for_found = 56,
};

struct TPMSProtocolDecoderAbarth124 {
    SubGhzProtocolDecoderBase base;

    SubGhzBlockDecoder decoder;
    TPMSBlockGeneric generic;

    uint16_t header_count;
};

struct TPMSProtocolEncoderAbarth124 {
    SubGhzProtocolEncoderBase base;

    SubGhzProtocolBlockEncoder encoder;
    TPMSBlockGeneric generic;
};

typedef enum {
    Abarth124DecoderStepReset = 0,
    Abarth124DecoderStepCheckPreamble,
    Abarth124DecoderStepSaveDuration,
    Abarth124DecoderStepCheckDuration,
} Abarth124DecoderStep;

const SubGhzProtocolDecoder tpms_protocol_abarth124_decoder = {
    .alloc = tpms_protocol_decoder_abarth124_alloc,
    .free = tpms_protocol_decoder_abarth124_free,

    .feed = tpms_protocol_decoder_abarth124_feed,
    .reset = tpms_protocol_decoder_abarth124_reset,

    .get_hash_data = tpms_protocol_decoder_abarth124_get_hash_data,
    .serialize = tpms_protocol_decoder_abarth124_serialize,
    .deserialize = tpms_protocol_decoder_abarth124_deserialize,
    .get_string = tpms_protocol_decoder_abarth124_get_string,
};

const SubGhzProtocolEncoder tpms_protocol_abarth124_encoder = {
    .alloc = NULL,
    .free = NULL,

    .deserialize = NULL,
    .stop = NULL,
    .yield = NULL,
};

const SubGhzProtocol tpms_protocol_abarth124 = {
    .name = TPMS_PROTOCOL_ABARTH124_NAME,
    .type = SubGhzProtocolTypeStatic,
    .flag = SubGhzProtocolFlag_433 | SubGhzProtocolFlag_315 | SubGhzProtocolFlag_868 |
            SubGhzProtocolFlag_AM | SubGhzProtocolFlag_Decodable,

    .decoder = &tpms_protocol_abarth124_decoder,
    .encoder = &tpms_protocol_abarth124_encoder,
};

void* tpms_protocol_decoder_abarth124_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    TPMSProtocolDecoderAbarth124* instance = malloc(sizeof(TPMSProtocolDecoderAbarth124));
    instance->base.protocol = &tpms_protocol_abarth124;
    instance->generic.protocol_name = instance->base.protocol->name;
    return instance;
}

void tpms_protocol_decoder_abarth124_free(void* context) {
    furi_assert(context);
    TPMSProtocolDecoderAbarth124* instance = context;
    free(instance);
}

void tpms_protocol_decoder_abarth124_reset(void* context) {
    furi_assert(context);
    TPMSProtocolDecoderAbarth124* instance = context;
    instance->decoder.parser_step = Abarth124DecoderStepReset;
}

static bool tpms_protocol_abarth124_check_crc(TPMSProtocolDecoderAbarth124* instance) {
    uint8_t msg[] = {
        instance->decoder.decode_data >> 48,
        instance->decoder.decode_data >> 40,
        instance->decoder.decode_data >> 32,
        instance->decoder.decode_data >> 24,
        instance->decoder.decode_data >> 16,
        instance->decoder.decode_data >> 8};

    if((subghz_protocol_blocks_add_bytes(msg, 6) ==
        (uint8_t)(instance->decoder.decode_data & 0xFF)) &&
       (!subghz_protocol_blocks_parity_bytes(&msg[2], 4))) {
        return true;
    } else {
        return false;
    }
}

/**
 * Analysis of received data
 * @param instance Pointer to a TPMSBlockGeneric* instance
 */
static void tpms_protocol_abarth124_remote_controller(TPMSBlockGeneric* instance) {
    uint8_t channel[] = {3, 0, 2, 1};
    uint8_t channel_raw = ((instance->data >> 54) & 0x03);
    instance->channel = channel[channel_raw];
    instance->id = (instance->data >> 40) & 0x3FFF;
    instance->battery_low = !((instance->data >> 38) & 1);
    instance->humidity = (instance->data >> 24) & 0x7F;

    uint16_t temp_raw = ((instance->data >> 9) & 0xF80) | ((instance->data >> 8) & 0x7F);
    instance->temp = ((float)(temp_raw)-1000) / 10.0f;

    instance->btn = TPMS_NO_BTN;
}

void tpms_protocol_decoder_abarth124_feed(void* context, bool level, uint32_t duration) {
    furi_assert(context);
    TPMSProtocolDecoderAbarth124* instance = context;

    switch(instance->decoder.parser_step) {
    case Abarth124DecoderStepReset:
        if((level) && (DURATION_DIFF(duration, tpms_protocol_abarth124_const.te_short * 3) <
                       tpms_protocol_abarth124_const.te_delta * 2)) {
            instance->decoder.parser_step = Abarth124DecoderStepCheckPreamble;
            instance->decoder.te_last = duration;
            instance->header_count = 0;
        }
        break;

    case Abarth124DecoderStepCheckPreamble:
        if(level) {
            instance->decoder.te_last = duration;
        } else {
            if((DURATION_DIFF(
                    instance->decoder.te_last, tpms_protocol_abarth124_const.te_short * 3) <
                tpms_protocol_abarth124_const.te_delta * 2) &&
               (DURATION_DIFF(duration, tpms_protocol_abarth124_const.te_short * 3) <
                tpms_protocol_abarth124_const.te_delta * 2)) {
                //Found preambule
                instance->header_count++;
            } else if((instance->header_count > 2) && (instance->header_count < 5)) {
                if((DURATION_DIFF(
                        instance->decoder.te_last, tpms_protocol_abarth124_const.te_short) <
                    tpms_protocol_abarth124_const.te_delta) &&
                   (DURATION_DIFF(duration, tpms_protocol_abarth124_const.te_long) <
                    tpms_protocol_abarth124_const.te_delta)) {
                    instance->decoder.decode_data = 0;
                    instance->decoder.decode_count_bit = 0;
                    subghz_protocol_blocks_add_bit(&instance->decoder, 0);
                    instance->decoder.parser_step = Abarth124DecoderStepSaveDuration;
                } else if(
                    (DURATION_DIFF(
                         instance->decoder.te_last, tpms_protocol_abarth124_const.te_long) <
                     tpms_protocol_abarth124_const.te_delta) &&
                    (DURATION_DIFF(duration, tpms_protocol_abarth124_const.te_short) <
                     tpms_protocol_abarth124_const.te_delta)) {
                    instance->decoder.decode_data = 0;
                    instance->decoder.decode_count_bit = 0;
                    subghz_protocol_blocks_add_bit(&instance->decoder, 1);
                    instance->decoder.parser_step = Abarth124DecoderStepSaveDuration;
                } else {
                    instance->decoder.parser_step = Abarth124DecoderStepReset;
                }
            } else {
                instance->decoder.parser_step = Abarth124DecoderStepReset;
            }
        }
        break;

    case Abarth124DecoderStepSaveDuration:
        if(level) {
            instance->decoder.te_last = duration;
            instance->decoder.parser_step = Abarth124DecoderStepCheckDuration;
        } else {
            instance->decoder.parser_step = Abarth124DecoderStepReset;
        }
        break;

    case Abarth124DecoderStepCheckDuration:
        if(!level) {
            if(duration >= ((uint32_t)tpms_protocol_abarth124_const.te_short * 5)) {
                if((instance->decoder.decode_count_bit ==
                    tpms_protocol_abarth124_const.min_count_bit_for_found) &&
                   tpms_protocol_abarth124_check_crc(instance)) {
                    instance->generic.data = instance->decoder.decode_data;
                    instance->generic.data_count_bit = instance->decoder.decode_count_bit;
                    tpms_protocol_abarth124_remote_controller(&instance->generic);
                    if(instance->base.callback)
                        instance->base.callback(&instance->base, instance->base.context);
                }
                instance->decoder.decode_data = 0;
                instance->decoder.decode_count_bit = 0;
                instance->decoder.parser_step = Abarth124DecoderStepReset;
                break;
            } else if(
                (DURATION_DIFF(instance->decoder.te_last, tpms_protocol_abarth124_const.te_short) <
                 tpms_protocol_abarth124_const.te_delta) &&
                (DURATION_DIFF(duration, tpms_protocol_abarth124_const.te_long) <
                 tpms_protocol_abarth124_const.te_delta)) {
                subghz_protocol_blocks_add_bit(&instance->decoder, 0);
                instance->decoder.parser_step = Abarth124DecoderStepSaveDuration;
            } else if(
                (DURATION_DIFF(instance->decoder.te_last, tpms_protocol_abarth124_const.te_long) <
                 tpms_protocol_abarth124_const.te_delta) &&
                (DURATION_DIFF(duration, tpms_protocol_abarth124_const.te_short) <
                 tpms_protocol_abarth124_const.te_delta)) {
                subghz_protocol_blocks_add_bit(&instance->decoder, 1);
                instance->decoder.parser_step = Abarth124DecoderStepSaveDuration;
            } else {
                instance->decoder.parser_step = Abarth124DecoderStepReset;
            }
        } else {
            instance->decoder.parser_step = Abarth124DecoderStepReset;
        }
        break;
    }
}

uint8_t tpms_protocol_decoder_abarth124_get_hash_data(void* context) {
    furi_assert(context);
    TPMSProtocolDecoderAbarth124* instance = context;
    return subghz_protocol_blocks_get_hash_data(
        &instance->decoder, (instance->decoder.decode_count_bit / 8) + 1);
}

SubGhzProtocolStatus tpms_protocol_decoder_abarth124_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset) {
    furi_assert(context);
    TPMSProtocolDecoderAbarth124* instance = context;
    return tpms_block_generic_serialize(&instance->generic, flipper_format, preset);
}

SubGhzProtocolStatus
    tpms_protocol_decoder_abarth124_deserialize(void* context, FlipperFormat* flipper_format) {
    furi_assert(context);
    TPMSProtocolDecoderAbarth124* instance = context;
    return tpms_block_generic_deserialize_check_count_bit(
        &instance->generic, flipper_format, tpms_protocol_abarth124_const.min_count_bit_for_found);
}

void tpms_protocol_decoder_abarth124_get_string(void* context, FuriString* output) {
    furi_assert(context);
    TPMSProtocolDecoderAbarth124* instance = context;
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
