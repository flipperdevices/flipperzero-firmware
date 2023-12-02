#include "auriol_ahfl.h"

#define TAG "WSProtocolAuriol_AHFL"

/*
 *
Auriol AHFL 433B2 IPX4 sensor.

Data layout:
        IIIIIIII-B-X-CC-TTTTTTTTTTTT-HHHHHHH0-FFFF-SSSSSS
Exmpl.: 10111100-1-0-00-000011101000-01101100-0100-001011

- I: id, 8 bit
- B: where B is the battery status: 1=OK, 0=LOW, 1 bit
- X: tx-button, 1 bit (might not work)
- C: CC is the channel: 00=CH1, 01=CH2, 11=CH3, 2bit
- T: temperature, 12 bit: 2's complement, scaled by 10
- H: humidity, 7 bits data, 1 bit 0
- F: always 0x4 (0100)
- S: nibble sum, 6 bits

 * The sensor sends 42 bits 5 times,
 * the packets are ppm modulated (distance coding) with a pulse of ~500 us
 * followed by a short gap of ~1000 us for a 0 bit or a long ~2000 us gap for a
 * 1 bit, the sync gap is ~4000 us.
 * 
 */

#define AURIOL_AHFL_CONST_DATA 0b0100

static const SubGhzBlockConst ws_protocol_auriol_ahfl_const = {
    .te_short = 500,
    .te_long = 2000,
    .te_delta = 150,
    .min_count_bit_for_found = 42,
};

struct WSProtocolDecoderAuriol_AHFL {
    SubGhzProtocolDecoderBase base;

    SubGhzBlockDecoder decoder;
    WSBlockGeneric generic;
};

struct WSProtocolEncoderAuriol_AHFL {
    SubGhzProtocolEncoderBase base;

    SubGhzProtocolEncoder encoder;
    WSBlockGeneric generic;
};

typedef enum {
    auriol_AHFLDecoderStepReset = 0,
    auriol_AHFLDecoderStepSaveDuration,
    auriol_AHFLDecoderStepCheckDuration,
} auriol_AHFLDecoderStep;

const SubGhzProtocolDecoder ws_protocol_auriol_ahfl_decoder = {
    .alloc = ws_protocol_decoder_auriol_ahfl_alloc,
    .free = ws_protocol_decoder_auriol_ahfl_free,

    .feed = ws_protocol_decoder_auriol_ahfl_feed,
    .reset = ws_protocol_decoder_auriol_ahfl_reset,

    .get_hash_data = ws_protocol_decoder_auriol_ahfl_get_hash_data,
    .serialize = ws_protocol_decoder_auriol_ahfl_serialize,
    .deserialize = ws_protocol_decoder_auriol_ahfl_deserialize,
    .get_string = ws_protocol_decoder_auriol_ahfl_get_string,
};

const SubGhzProtocolEncoder ws_protocol_auriol_ahfl_encoder = {
    .alloc = NULL,
    .free = NULL,

    .deserialize = NULL,
    .stop = NULL,
    .yield = NULL,
};

const SubGhzProtocol ws_protocol_auriol_ahfl = {
    .name = WS_PROTOCOL_AURIOL_AHFL_NAME,
    .type = SubGhzProtocolWeatherStation,
    .flag = SubGhzProtocolFlag_433 | SubGhzProtocolFlag_315 | SubGhzProtocolFlag_868 |
            SubGhzProtocolFlag_AM | SubGhzProtocolFlag_Decodable,

    .decoder = &ws_protocol_auriol_ahfl_decoder,
    .encoder = &ws_protocol_auriol_ahfl_encoder,
};

void* ws_protocol_decoder_auriol_ahfl_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    WSProtocolDecoderAuriol_AHFL* instance = malloc(sizeof(WSProtocolDecoderAuriol_AHFL));
    instance->base.protocol = &ws_protocol_auriol_ahfl;
    instance->generic.protocol_name = instance->base.protocol->name;
    return instance;
}

void ws_protocol_decoder_auriol_ahfl_free(void* context) {
    furi_assert(context);
    WSProtocolDecoderAuriol_AHFL* instance = context;
    free(instance);
}

void ws_protocol_decoder_auriol_ahfl_reset(void* context) {
    furi_assert(context);
    WSProtocolDecoderAuriol_AHFL* instance = context;
    instance->decoder.parser_step = auriol_AHFLDecoderStepReset;
}

static bool ws_protocol_auriol_ahfl_check(WSProtocolDecoderAuriol_AHFL* instance) {
    uint8_t type = (instance->decoder.decode_data >> 6) & 0x0F;

    if(type != AURIOL_AHFL_CONST_DATA) {
        // Fail const data check
        return false;
    }

    uint64_t payload = instance->decoder.decode_data >> 6;
    // Checksum is the last 6 bits of data
    uint8_t checksum_received = instance->decoder.decode_data & 0x3F;
    uint8_t checksum_calculated = 0;
    for(uint8_t i = 0; i < 9; i++) {
        checksum_calculated += (payload >> (i * 4)) & 0xF;
    }
    return checksum_received == checksum_calculated;
}

/**
 * Analysis of received data
 * @param instance Pointer to a WSBlockGeneric* instance
 */
static void ws_protocol_auriol_ahfl_remote_controller(WSBlockGeneric* instance) {
    instance->id = instance->data >> 34;
    instance->battery_low = (instance->data >> 33) & 1;
    instance->btn = (instance->data >> 32) & 1;
    instance->channel = ((instance->data >> 30) & 0x3) + 1;
    if(!((instance->data >> 29) & 1)) {
        instance->temp = (float)((instance->data >> 18) & 0x07FF) / 10.0f;
    } else {
        instance->temp = (float)((~(instance->data >> 18) & 0x07FF) + 1) / -10.0f;
    }
    instance->humidity = (instance->data >> 11) & 0x7F;
}

void ws_protocol_decoder_auriol_ahfl_feed(void* context, bool level, uint32_t duration) {
    furi_assert(context);
    WSProtocolDecoderAuriol_AHFL* instance = context;

    switch(instance->decoder.parser_step) {
    case auriol_AHFLDecoderStepReset:
        if((!level) && (DURATION_DIFF(duration, ws_protocol_auriol_ahfl_const.te_short * 18) <
                        ws_protocol_auriol_ahfl_const.te_delta)) {
            //Found syncPrefix
            instance->decoder.parser_step = auriol_AHFLDecoderStepSaveDuration;
            instance->decoder.decode_data = 0;
            instance->decoder.decode_count_bit = 0;
        }
        break;
    case auriol_AHFLDecoderStepSaveDuration:
        if(level) {
            instance->decoder.te_last = duration;
            instance->decoder.parser_step = auriol_AHFLDecoderStepCheckDuration;
        } else {
            instance->decoder.parser_step = auriol_AHFLDecoderStepReset;
        }
        break;
    case auriol_AHFLDecoderStepCheckDuration:
        if(!level) {
            if(DURATION_DIFF(instance->decoder.te_last, ws_protocol_auriol_ahfl_const.te_short) <
               ws_protocol_auriol_ahfl_const.te_delta) {
                if(DURATION_DIFF(duration, ws_protocol_auriol_ahfl_const.te_short * 18) <
                   ws_protocol_auriol_ahfl_const.te_delta * 8) {
                    //Found syncPostfix
                    instance->decoder.parser_step = auriol_AHFLDecoderStepReset;
                    if((instance->decoder.decode_count_bit ==
                        ws_protocol_auriol_ahfl_const.min_count_bit_for_found) &&
                       ws_protocol_auriol_ahfl_check(instance)) {
                        instance->generic.data = instance->decoder.decode_data;
                        instance->generic.data_count_bit = instance->decoder.decode_count_bit;
                        ws_protocol_auriol_ahfl_remote_controller(&instance->generic);
                        if(instance->base.callback) {
                            instance->base.callback(&instance->base, instance->base.context);
                        }
                    } else if(instance->decoder.decode_count_bit == 1) {
                        instance->decoder.parser_step = auriol_AHFLDecoderStepSaveDuration;
                    }
                    instance->decoder.decode_data = 0;
                    instance->decoder.decode_count_bit = 0;
                } else if(
                    DURATION_DIFF(duration, ws_protocol_auriol_ahfl_const.te_long) <
                    ws_protocol_auriol_ahfl_const.te_delta * 2) {
                    subghz_protocol_blocks_add_bit(&instance->decoder, 0);
                    instance->decoder.parser_step = auriol_AHFLDecoderStepSaveDuration;
                } else if(
                    DURATION_DIFF(duration, ws_protocol_auriol_ahfl_const.te_long * 2) <
                    ws_protocol_auriol_ahfl_const.te_delta * 4) {
                    subghz_protocol_blocks_add_bit(&instance->decoder, 1);
                    instance->decoder.parser_step = auriol_AHFLDecoderStepSaveDuration;
                } else {
                    instance->decoder.parser_step = auriol_AHFLDecoderStepReset;
                }
            } else {
                instance->decoder.parser_step = auriol_AHFLDecoderStepReset;
            }
        } else {
            instance->decoder.parser_step = auriol_AHFLDecoderStepReset;
        }
        break;
    }
}

uint8_t ws_protocol_decoder_auriol_ahfl_get_hash_data(void* context) {
    furi_assert(context);
    WSProtocolDecoderAuriol_AHFL* instance = context;
    return subghz_protocol_blocks_get_hash_data(
        &instance->decoder, (instance->decoder.decode_count_bit / 8) + 1);
}

SubGhzProtocolStatus ws_protocol_decoder_auriol_ahfl_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset) {
    furi_assert(context);
    WSProtocolDecoderAuriol_AHFL* instance = context;
    return ws_block_generic_serialize(&instance->generic, flipper_format, preset);
}

SubGhzProtocolStatus
    ws_protocol_decoder_auriol_ahfl_deserialize(void* context, FlipperFormat* flipper_format) {
    furi_assert(context);
    WSProtocolDecoderAuriol_AHFL* instance = context;
    return ws_block_generic_deserialize_check_count_bit(
        &instance->generic, flipper_format, ws_protocol_auriol_ahfl_const.min_count_bit_for_found);
}

void ws_protocol_decoder_auriol_ahfl_get_string(void* context, FuriString* output) {
    furi_assert(context);
    WSProtocolDecoderAuriol_AHFL* instance = context;
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