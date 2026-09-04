#include "honeywell_5834.h"

#include "../blocks/const.h"
#include "../blocks/decoder.h"
#include "../blocks/encoder.h"
#include "../blocks/generic.h"
#include "../blocks/math.h"

#define TAG "SubGhzProtocolHoneywell5834"

/*
 * Honeywell 5834-4 wireless security keyfob
 * Frequency: 345.000 MHz
 * Modulation: AM/OOK (AM650 preset)
 * Encoding: Pulse-width modulation (pulse duration encodes bit)
 *   short pulse (<183 µs) = 0, long pulse (>=183 µs) = 1
 *
 * Frame: sync_gap(710µs low) + sync_pulse(300µs high) + gap + 48 data bits
 * Data layout (same 5800 family as WDB):
 *   bits [47:28] = 20-bit serial/device ID
 *   bits [27:2]  = device type, channel, state, loop
 *   bit  [0]     = parity (LSB of popcount of bits [47:1])
 *
 * Buttons: Arm Away, Arm Stay, Disarm, Panic
 */

/* Timing constants (µs) derived from RAW captures */
#define H5834_TE_SHORT      160
#define H5834_TE_LONG       290
#define H5834_TE_DELTA      80
#define H5834_SYNC_GAP      710
#define H5834_SYNC_PULSE    300
#define H5834_GAP           120
#define H5834_MIN_COUNT_BIT 48
/* Encoder: sync(3) + 48 data bits * 2 (pulse+gap) = 99 LevelDuration entries */
#define H5834_UPLOAD_MAX    (3 + (H5834_MIN_COUNT_BIT * 2))

static const SubGhzBlockConst subghz_protocol_honeywell_5834_const = {
    .te_short = H5834_TE_SHORT,
    .te_long = H5834_TE_LONG,
    .te_delta = H5834_TE_DELTA,
    .min_count_bit_for_found = H5834_MIN_COUNT_BIT,
};

struct SubGhzProtocolDecoderHoneywell5834 {
    SubGhzProtocolDecoderBase base;

    SubGhzBlockDecoder decoder;
    SubGhzBlockGeneric generic;
};

struct SubGhzProtocolEncoderHoneywell5834 {
    SubGhzProtocolEncoderBase base;

    SubGhzProtocolBlockEncoder encoder;
    SubGhzBlockGeneric generic;
};

typedef enum {
    Honeywell5834DecoderStepReset = 0,
    Honeywell5834DecoderStepFoundSyncGap,
    Honeywell5834DecoderStepSaveDuration,
    Honeywell5834DecoderStepCheckDuration,
} Honeywell5834DecoderStep;

const SubGhzProtocolDecoder subghz_protocol_honeywell_5834_decoder = {
    .alloc = subghz_protocol_decoder_honeywell_5834_alloc,
    .free = subghz_protocol_decoder_honeywell_5834_free,

    .feed = subghz_protocol_decoder_honeywell_5834_feed,
    .reset = subghz_protocol_decoder_honeywell_5834_reset,

    .get_hash_data = subghz_protocol_decoder_honeywell_5834_get_hash_data,
    .serialize = subghz_protocol_decoder_honeywell_5834_serialize,
    .deserialize = subghz_protocol_decoder_honeywell_5834_deserialize,
    .get_string = subghz_protocol_decoder_honeywell_5834_get_string,
};

const SubGhzProtocolEncoder subghz_protocol_honeywell_5834_encoder = {
    .alloc = subghz_protocol_encoder_honeywell_5834_alloc,
    .free = subghz_protocol_encoder_honeywell_5834_free,

    .deserialize = subghz_protocol_encoder_honeywell_5834_deserialize,
    .stop = subghz_protocol_encoder_honeywell_5834_stop,
    .yield = subghz_protocol_encoder_honeywell_5834_yield,
};

const SubGhzProtocol subghz_protocol_honeywell_5834 = {
    .name = SUBGHZ_PROTOCOL_HONEYWELL_5834_NAME,
    .type = SubGhzProtocolTypeStatic,
    .flag = SubGhzProtocolFlag_315 | SubGhzProtocolFlag_433 | SubGhzProtocolFlag_AM |
            SubGhzProtocolFlag_Decodable | SubGhzProtocolFlag_Load | SubGhzProtocolFlag_Save |
            SubGhzProtocolFlag_Send,

    .decoder = &subghz_protocol_honeywell_5834_decoder,
    .encoder = &subghz_protocol_honeywell_5834_encoder,
};

/* ─── Encoder ─────────────────────────────────────────────────────────── */

void* subghz_protocol_encoder_honeywell_5834_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    SubGhzProtocolEncoderHoneywell5834* instance =
        malloc(sizeof(SubGhzProtocolEncoderHoneywell5834));

    instance->base.protocol = &subghz_protocol_honeywell_5834;
    instance->generic.protocol_name = instance->base.protocol->name;

    instance->encoder.repeat = 10;
    instance->encoder.size_upload = H5834_UPLOAD_MAX;
    instance->encoder.upload = malloc(instance->encoder.size_upload * sizeof(LevelDuration));
    instance->encoder.is_running = false;
    return instance;
}

void subghz_protocol_encoder_honeywell_5834_free(void* context) {
    furi_assert(context);
    SubGhzProtocolEncoderHoneywell5834* instance = context;
    free(instance->encoder.upload);
    free(instance);
}

/**
 * Generate an upload from data.
 * Frame on air: sync_gap (low) + sync_pulse (high) + gap (low) + data bits (pulse-width encoded)
 */
static bool subghz_protocol_encoder_honeywell_5834_get_upload(
    SubGhzProtocolEncoderHoneywell5834* instance) {
    furi_assert(instance);

    /* sync(3) + data_bits * 2; 48-bit frame => 99 entries (H5834_UPLOAD_MAX) */
    size_t index = 0;
    size_t size_upload = 3 + (instance->generic.data_count_bit * 2);
    if(size_upload > H5834_UPLOAD_MAX) {
        FURI_LOG_E(TAG, "Size upload exceeds allocated encoder buffer.");
        return false;
    }
    instance->encoder.size_upload = size_upload;

    /* Sync: gap (low) + pulse (high) + short gap (low) */
    instance->encoder.upload[index++] = level_duration_make(false, (uint32_t)H5834_SYNC_GAP);
    instance->encoder.upload[index++] = level_duration_make(true, (uint32_t)H5834_SYNC_PULSE);
    instance->encoder.upload[index++] = level_duration_make(false, (uint32_t)H5834_GAP);

    /* Data bits – MSB first (bit data_count_bit-1 .. 0) */
    for(uint8_t i = instance->generic.data_count_bit; i > 0; i--) {
        if(bit_read(instance->generic.data, i - 1)) {
            /* bit 1: long pulse */
            instance->encoder.upload[index++] = level_duration_make(true, (uint32_t)H5834_TE_LONG);
        } else {
            /* bit 0: short pulse */
            instance->encoder.upload[index++] =
                level_duration_make(true, (uint32_t)H5834_TE_SHORT);
        }
        /* inter-bit gap */
        instance->encoder.upload[index++] = level_duration_make(false, (uint32_t)H5834_GAP);
    }
    /* Replace last inter-bit gap with inter-frame tail for decoder frame-end detection */
    instance->encoder.upload[index - 1] = level_duration_make(false, 15000);
    return true;
}

SubGhzProtocolStatus subghz_protocol_encoder_honeywell_5834_deserialize(
    void* context,
    FlipperFormat* flipper_format) {
    furi_assert(context);
    SubGhzProtocolEncoderHoneywell5834* instance = context;
    SubGhzProtocolStatus ret = SubGhzProtocolStatusError;
    do {
        ret = subghz_block_generic_deserialize_check_count_bit(
            &instance->generic,
            flipper_format,
            subghz_protocol_honeywell_5834_const.min_count_bit_for_found);
        if(ret != SubGhzProtocolStatusOk) {
            break;
        }
        flipper_format_read_uint32(
            flipper_format, "Repeat", (uint32_t*)&instance->encoder.repeat, 1);

        if(!subghz_protocol_encoder_honeywell_5834_get_upload(instance)) {
            ret = SubGhzProtocolStatusErrorEncoderGetUpload;
            break;
        }
        instance->encoder.is_running = true;
    } while(false);

    return ret;
}

void subghz_protocol_encoder_honeywell_5834_stop(void* context) {
    SubGhzProtocolEncoderHoneywell5834* instance = context;
    instance->encoder.is_running = false;
}

LevelDuration subghz_protocol_encoder_honeywell_5834_yield(void* context) {
    SubGhzProtocolEncoderHoneywell5834* instance = context;

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

/* ─── Decoder ─────────────────────────────────────────────────────────── */

void* subghz_protocol_decoder_honeywell_5834_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    SubGhzProtocolDecoderHoneywell5834* instance =
        malloc(sizeof(SubGhzProtocolDecoderHoneywell5834));
    instance->base.protocol = &subghz_protocol_honeywell_5834;
    instance->generic.protocol_name = instance->base.protocol->name;
    return instance;
}

void subghz_protocol_decoder_honeywell_5834_free(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderHoneywell5834* instance = context;
    free(instance);
}

void subghz_protocol_decoder_honeywell_5834_reset(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderHoneywell5834* instance = context;
    instance->decoder.parser_step = Honeywell5834DecoderStepReset;
}

void subghz_protocol_decoder_honeywell_5834_feed(void* context, bool level, uint32_t duration) {
    furi_assert(context);
    SubGhzProtocolDecoderHoneywell5834* instance = context;

    switch(instance->decoder.parser_step) {
    case Honeywell5834DecoderStepReset:
        if((!level) && (DURATION_DIFF(duration, H5834_SYNC_GAP) < 200)) {
            /* Found sync gap (~710 µs low) */
            instance->decoder.decode_count_bit = 0;
            instance->decoder.decode_data = 0;
            instance->decoder.parser_step = Honeywell5834DecoderStepFoundSyncGap;
        }
        break;

    case Honeywell5834DecoderStepFoundSyncGap:
        if(level && (DURATION_DIFF(duration, H5834_SYNC_PULSE) < 150)) {
            /* Found sync pulse (~300 µs high) – next is short gap, then data */
            instance->decoder.parser_step = Honeywell5834DecoderStepSaveDuration;
        } else {
            instance->decoder.parser_step = Honeywell5834DecoderStepReset;
        }
        break;

    case Honeywell5834DecoderStepSaveDuration:
        if(level) {
            /* HIGH pulse – save for classification in next LOW */
            instance->decoder.te_last = duration;
            instance->decoder.parser_step = Honeywell5834DecoderStepCheckDuration;
        } else {
            /* LOW gap before first data pulse (post-sync gap) – skip */
            if(duration > 10000) {
                /* Very long gap: frame ended before any data */
                instance->decoder.parser_step = Honeywell5834DecoderStepReset;
            }
            /* Otherwise stay in StepSaveDuration, waiting for next HIGH */
        }
        break;

    case Honeywell5834DecoderStepCheckDuration:
        if(!level) {
            /* LOW gap – classify the preceding pulse */
            if(DURATION_DIFF(instance->decoder.te_last, H5834_TE_SHORT) <
               subghz_protocol_honeywell_5834_const.te_delta) {
                subghz_protocol_blocks_add_bit(&instance->decoder, 0);
            } else if(
                DURATION_DIFF(instance->decoder.te_last, H5834_TE_LONG) <
                subghz_protocol_honeywell_5834_const.te_delta) {
                subghz_protocol_blocks_add_bit(&instance->decoder, 1);
            } else {
                /* Pulse doesn't match either – reset */
                instance->decoder.parser_step = Honeywell5834DecoderStepReset;
                break;
            }

            /* Check for frame end (inter-frame gap > 10 ms) */
            if(duration > 10000) {
                if(instance->decoder.decode_count_bit >=
                   subghz_protocol_honeywell_5834_const.min_count_bit_for_found) {
                    /* Parity check: bit 0 == parity of bits [1..47] */
                    if((instance->decoder.decode_data & 0x01) ==
                       subghz_protocol_blocks_get_parity(
                           instance->decoder.decode_data >> 1,
                           subghz_protocol_honeywell_5834_const.min_count_bit_for_found - 1)) {
                        instance->generic.data = instance->decoder.decode_data;
                        instance->generic.data_count_bit = instance->decoder.decode_count_bit;

                        if(instance->base.callback)
                            instance->base.callback(&instance->base, instance->base.context);
                    }
                }
                instance->decoder.parser_step = Honeywell5834DecoderStepReset;
            } else {
                instance->decoder.parser_step = Honeywell5834DecoderStepSaveDuration;
            }
        } else {
            instance->decoder.parser_step = Honeywell5834DecoderStepReset;
        }
        break;
    }
}

/**
 * Extract fields from received data.
 * Uses the Honeywell 5800-series frame layout (same family as WDB).
 */
static const char* subghz_protocol_honeywell_5834_get_button_name(uint8_t state_data) {
    /* State/button byte: lower bits indicate function.
     * Exact mapping derived from captures; common 5800-series values: */
    switch(state_data & 0xF0) {
    case 0x40:
        return "Arm Away";
    case 0x20:
        return "Arm Stay";
    case 0x10:
        return "Disarm";
    case 0x80:
        return "Panic";
    default:
        return "Unknown";
    }
}

uint8_t subghz_protocol_decoder_honeywell_5834_get_hash_data(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderHoneywell5834* instance = context;
    return subghz_protocol_blocks_get_hash_data(
        &instance->decoder, (instance->decoder.decode_count_bit / 8) + 1);
}

SubGhzProtocolStatus subghz_protocol_decoder_honeywell_5834_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset) {
    furi_assert(context);
    SubGhzProtocolDecoderHoneywell5834* instance = context;
    return subghz_block_generic_serialize(&instance->generic, flipper_format, preset);
}

SubGhzProtocolStatus subghz_protocol_decoder_honeywell_5834_deserialize(
    void* context,
    FlipperFormat* flipper_format) {
    furi_assert(context);
    SubGhzProtocolDecoderHoneywell5834* instance = context;
    return subghz_block_generic_deserialize_check_count_bit(
        &instance->generic,
        flipper_format,
        subghz_protocol_honeywell_5834_const.min_count_bit_for_found);
}

void subghz_protocol_decoder_honeywell_5834_get_string(void* context, FuriString* output) {
    furi_assert(context);
    SubGhzProtocolDecoderHoneywell5834* instance = context;

    uint32_t serial = (instance->generic.data >> 28) & 0xFFFFF;
    uint8_t state_data = (instance->generic.data >> 4) & 0xFF;
    const char* btn_name = subghz_protocol_honeywell_5834_get_button_name(state_data);
    uint8_t lowbat = (uint8_t)((instance->generic.data >> 1) & 0x1);

    furi_string_cat_printf(
        output,
        "%s\r\n"
        "Key:0x%lX%08lX %dbit\r\n"
        "Sn:0x%05lX  Btn:%s\r\n"
        "LBat:%01X\r\n",
        instance->generic.protocol_name,
        (uint32_t)((instance->generic.data >> 32) & 0xFFFFFFFF),
        (uint32_t)(instance->generic.data & 0xFFFFFFFF),
        instance->generic.data_count_bit,
        serial,
        btn_name,
        lowbat);
}
