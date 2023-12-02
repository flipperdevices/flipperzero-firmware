#include "schrader_gg4.h"
#include <lib/toolbox/manchester_decoder.h>

#define TAG "Schrader"

// https://github.com/merbanan/rtl_433/blob/master/src/devices/schraeder.c
// https://elib.dlr.de/81155/1/TPMS_for_Trafffic_Management_purposes.pdf
// https://github.com/furrtek/portapack-havoc/issues/349
// https://fccid.io/MRXGG4
// https://fccid.io/MRXGG4T

/**
 * Schrader 3013/3015 MRX-GG4

OEM:
KIA Sportage CGA 11-SPT1504-RA
Mercedes-Benz A0009054100

* Frequency: 433.92MHz+-38KHz
* Modulation: ASK
* Working Temperature: -50°C to 125°C
* Tire monitoring range value: 0kPa-350kPa+-7kPa

Examples in normal environmental conditions:
3000878456094cd0
3000878456084ecb
3000878456074d01

Data layout:
 * | Byte 0    | Byte 1    | Byte 2    | Byte 3    | Byte 4    | Byte 5    | Byte 6    | Byte 7    |
 * | --------- | --------- | --------- | --------- | --------- | --------- | --------- | --------- |
 * | SSSS SSSS | IIII IIII | IIII IIII | IIII IIII | IIII IIII | PPPP PPPP | TTTT TTTT | CCCC CCCC |
 *

- The preamble is 0b000
- S: always 0x30 in relearn state
- I: 32 bit ID
- P: 8 bit Pressure (multiplyed by 2.5 = PSI)
- T: 8 bit Temperature (deg. C offset by 50)
- C: 8 bit Checksum (CRC8, Poly 0x7, Init 0x0)
*/

#define PREAMBLE 0b000
#define PREAMBLE_BITS_LEN 3

static const SubGhzBlockConst tpms_protocol_schrader_gg4_const = {
    .te_short = 120,
    .te_long = 240,
    .te_delta = 55, // 50% of te_short due to poor sensitivity
    .min_count_bit_for_found = 64,
};

struct TPMSProtocolDecoderSchraderGG4 {
    SubGhzProtocolDecoderBase base;

    SubGhzBlockDecoder decoder;
    TPMSBlockGeneric generic;

    ManchesterState manchester_saved_state;
    uint16_t header_count;
};

struct TPMSProtocolEncoderSchraderGG4 {
    SubGhzProtocolEncoderBase base;

    SubGhzProtocolBlockEncoder encoder;
    TPMSBlockGeneric generic;
};

typedef enum {
    SchraderGG4DecoderStepReset = 0,
    SchraderGG4DecoderStepCheckPreamble,
    SchraderGG4DecoderStepDecoderData,
    SchraderGG4DecoderStepSaveDuration,
    SchraderGG4DecoderStepCheckDuration,
} SchraderGG4DecoderStep;

const SubGhzProtocolDecoder tpms_protocol_schrader_gg4_decoder = {
    .alloc = tpms_protocol_decoder_schrader_gg4_alloc,
    .free = tpms_protocol_decoder_schrader_gg4_free,

    .feed = tpms_protocol_decoder_schrader_gg4_feed,
    .reset = tpms_protocol_decoder_schrader_gg4_reset,

    .get_hash_data = tpms_protocol_decoder_schrader_gg4_get_hash_data,
    .serialize = tpms_protocol_decoder_schrader_gg4_serialize,
    .deserialize = tpms_protocol_decoder_schrader_gg4_deserialize,
    .get_string = tpms_protocol_decoder_schrader_gg4_get_string,
};

const SubGhzProtocolEncoder tpms_protocol_schrader_gg4_encoder = {
    .alloc = NULL,
    .free = NULL,

    .deserialize = NULL,
    .stop = NULL,
    .yield = NULL,
};

const SubGhzProtocol tpms_protocol_schrader_gg4 = {
    .name = TPMS_PROTOCOL_SCHRADER_GG4_NAME,
    .type = SubGhzProtocolTypeStatic,
    .flag = SubGhzProtocolFlag_433 | SubGhzProtocolFlag_315 | SubGhzProtocolFlag_AM |
            SubGhzProtocolFlag_Decodable,

    .decoder = &tpms_protocol_schrader_gg4_decoder,
    .encoder = &tpms_protocol_schrader_gg4_encoder,
};

void* tpms_protocol_decoder_schrader_gg4_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    TPMSProtocolDecoderSchraderGG4* instance = malloc(sizeof(TPMSProtocolDecoderSchraderGG4));
    instance->base.protocol = &tpms_protocol_schrader_gg4;
    instance->generic.protocol_name = instance->base.protocol->name;
    return instance;
}

void tpms_protocol_decoder_schrader_gg4_free(void* context) {
    furi_assert(context);
    TPMSProtocolDecoderSchraderGG4* instance = context;
    free(instance);
}

void tpms_protocol_decoder_schrader_gg4_reset(void* context) {
    furi_assert(context);
    TPMSProtocolDecoderSchraderGG4* instance = context;
    instance->decoder.parser_step = SchraderGG4DecoderStepReset;
}

static bool tpms_protocol_schrader_gg4_check_crc(TPMSProtocolDecoderSchraderGG4* instance) {
    uint8_t msg[] = {
        instance->decoder.decode_data >> 48,
        instance->decoder.decode_data >> 40,
        instance->decoder.decode_data >> 32,
        instance->decoder.decode_data >> 24,
        instance->decoder.decode_data >> 16,
        instance->decoder.decode_data >> 8};

    uint8_t crc = subghz_protocol_blocks_crc8(msg, 6, 0x7, 0);
    return (crc == (instance->decoder.decode_data & 0xFF));
}

/**
 * Analysis of received data
 * @param instance Pointer to a TPMSBlockGeneric* instance
 */
static void tpms_protocol_schrader_gg4_analyze(TPMSBlockGeneric* instance) {
    instance->id = instance->data >> 24;

    // TODO locate and fix
    instance->battery_low = TPMS_NO_BATT;

    instance->temperature = ((instance->data >> 8) & 0xFF) - 50;
    instance->pressure = ((instance->data >> 16) & 0xFF) * 2.5 * 0.069;
}

static ManchesterEvent level_and_duration_to_event(bool level, uint32_t duration) {
    bool is_long = false;

    if(DURATION_DIFF(duration, tpms_protocol_schrader_gg4_const.te_long) <
       tpms_protocol_schrader_gg4_const.te_delta) {
        is_long = true;
    } else if(
        DURATION_DIFF(duration, tpms_protocol_schrader_gg4_const.te_short) <
        tpms_protocol_schrader_gg4_const.te_delta) {
        is_long = false;
    } else {
        return ManchesterEventReset;
    }

    if(level)
        return is_long ? ManchesterEventLongHigh : ManchesterEventShortHigh;
    else
        return is_long ? ManchesterEventLongLow : ManchesterEventShortLow;
}

void tpms_protocol_decoder_schrader_gg4_feed(void* context, bool level, uint32_t duration) {
    furi_assert(context);
    bool bit = false;
    bool have_bit = false;
    TPMSProtocolDecoderSchraderGG4* instance = context;

    // low-level bit sequence decoding
    if(instance->decoder.parser_step != SchraderGG4DecoderStepReset) {
        ManchesterEvent event = level_and_duration_to_event(level, duration);

        if(event == ManchesterEventReset) {
            if((instance->decoder.parser_step == SchraderGG4DecoderStepDecoderData) &&
               instance->decoder.decode_count_bit) {
                // FURI_LOG_D(TAG, "%d-%ld", level, duration);
                FURI_LOG_D(
                    TAG,
                    "reset accumulated %d bits: %llx",
                    instance->decoder.decode_count_bit,
                    instance->decoder.decode_data);
            }

            instance->decoder.parser_step = SchraderGG4DecoderStepReset;
        } else {
            have_bit = manchester_advance(
                instance->manchester_saved_state, event, &instance->manchester_saved_state, &bit);
            if(!have_bit) return;

            // Invert value, due to signal is Manchester II and decoder is Manchester I
            bit = !bit;
        }
    }

    switch(instance->decoder.parser_step) {
    case SchraderGG4DecoderStepReset:
        // wait for start ~480us pulse
        if((level) && (DURATION_DIFF(duration, tpms_protocol_schrader_gg4_const.te_long * 2) <
                       tpms_protocol_schrader_gg4_const.te_delta)) {
            instance->decoder.parser_step = SchraderGG4DecoderStepCheckPreamble;
            instance->header_count = 0;
            instance->decoder.decode_data = 0;
            instance->decoder.decode_count_bit = 0;

            // First will be short space, so set correct initial state for machine
            // https://clearwater.com.au/images/rc5/rc5-state-machine.gif
            instance->manchester_saved_state = ManchesterStateStart1;
        }
        break;
    case SchraderGG4DecoderStepCheckPreamble:
        if(bit != 0) {
            instance->decoder.parser_step = SchraderGG4DecoderStepReset;
            break;
        }

        instance->header_count++;
        if(instance->header_count == PREAMBLE_BITS_LEN)
            instance->decoder.parser_step = SchraderGG4DecoderStepDecoderData;
        break;

    case SchraderGG4DecoderStepDecoderData:
        subghz_protocol_blocks_add_bit(&instance->decoder, bit);
        if(instance->decoder.decode_count_bit ==
           tpms_protocol_schrader_gg4_const.min_count_bit_for_found) {
            FURI_LOG_D(TAG, "%016llx", instance->decoder.decode_data);
            if(!tpms_protocol_schrader_gg4_check_crc(instance)) {
                FURI_LOG_D(TAG, "CRC mismatch drop");
            } else {
                instance->generic.data = instance->decoder.decode_data;
                instance->generic.data_count_bit = instance->decoder.decode_count_bit;
                tpms_protocol_schrader_gg4_analyze(&instance->generic);
                if(instance->base.callback)
                    instance->base.callback(&instance->base, instance->base.context);
            }
            instance->decoder.parser_step = SchraderGG4DecoderStepReset;
        }
        break;
    }
}

uint8_t tpms_protocol_decoder_schrader_gg4_get_hash_data(void* context) {
    furi_assert(context);
    TPMSProtocolDecoderSchraderGG4* instance = context;
    return subghz_protocol_blocks_get_hash_data(
        &instance->decoder, (instance->decoder.decode_count_bit / 8) + 1);
}

SubGhzProtocolStatus tpms_protocol_decoder_schrader_gg4_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset) {
    furi_assert(context);
    TPMSProtocolDecoderSchraderGG4* instance = context;
    return tpms_block_generic_serialize(&instance->generic, flipper_format, preset);
}

SubGhzProtocolStatus
    tpms_protocol_decoder_schrader_gg4_deserialize(void* context, FlipperFormat* flipper_format) {
    furi_assert(context);
    TPMSProtocolDecoderSchraderGG4* instance = context;
    return tpms_block_generic_deserialize_check_count_bit(
        &instance->generic,
        flipper_format,
        tpms_protocol_schrader_gg4_const.min_count_bit_for_found);
}

void tpms_protocol_decoder_schrader_gg4_get_string(void* context, FuriString* output) {
    furi_assert(context);
    TPMSProtocolDecoderSchraderGG4* instance = context;
    furi_string_printf(
        output,
        "%s\r\n"
        "Id:0x%08lX\r\n"
        "Bat:%d\r\n"
        "Temp:%2.0f C Bar:%2.1f",
        instance->generic.protocol_name,
        instance->generic.id,
        instance->generic.battery_low,
        (double)instance->generic.temperature,
        (double)instance->generic.pressure);
}
