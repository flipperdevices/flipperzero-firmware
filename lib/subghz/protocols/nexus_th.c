#include "nexus_th.h"

#define TAG "WSProtocolNexus_TH"

/*
 * Help
 * https://github.com/merbanan/rtl_433/blob/master/src/devices/nexus.c
 *
 * Nexus sensor protocol with ID, temperature and optional humidity
 * also FreeTec (Pearl) NC-7345 sensors for FreeTec Weatherstation NC-7344,
 * also infactory/FreeTec (Pearl) NX-3980 sensors for infactory/FreeTec NX-3974 station,
 * also Solight TE82S sensors for Solight TE76/TE82/TE83/TE84 stations,
 * also TFA 30.3209.02 temperature/humidity sensor.
 * The sensor sends 36 bits 12 times,
 * the packets are ppm modulated (distance coding) with a pulse of ~500 us
 * followed by a short gap of ~1000 us for a 0 bit or a long ~2000 us gap for a
 * 1 bit, the sync gap is ~4000 us.
 * The data is grouped in 9 nibbles:
 *     [id0] [id1] [flags] [temp0] [temp1] [temp2] [const] [humi0] [humi1]
 * - The 8-bit id changes when the battery is changed in the sensor.
 * - flags are 4 bits B 0 C C, where B is the battery status: 1=OK, 0=LOW
 * - and CC is the channel: 0=CH1, 1=CH2, 2=CH3
 * - temp is 12 bit signed scaled by 10
 * - const is always 1111 (0x0F)
 * - humidity is 8 bits
 * The sensors can be bought at Clas Ohlsen (Nexus) and Pearl (infactory/FreeTec).
 * 
 *  Generate test files: https://htotoo.github.io/FlipperSUBGenerator/nexus-th-generator/index.html
 */

#define NEXUS_TH_CONST_DATA 0b1111

static const SubGhzBlockConst ws_protocol_nexus_th_const = {
    .te_short = 490,
    .te_long = 1980,
    .te_delta = 150,
    .min_count_bit_for_found = 36,
};

struct WSProtocolDecoderNexus_TH {
    SubGhzProtocolDecoderBase base;

    SubGhzBlockDecoder decoder;
    WSBlockGeneric generic;
};

struct WSProtocolEncoderNexus_TH {
    SubGhzProtocolEncoderBase base;

    SubGhzProtocolBlockEncoder encoder;
    WSBlockGeneric generic;
};

typedef enum {
    Nexus_THDecoderStepReset = 0,
    Nexus_THDecoderStepSaveDuration,
    Nexus_THDecoderStepCheckDuration,
} Nexus_THDecoderStep;

void* ws_protocol_encoder_nexus_th_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    WSProtocolEncoderNexus_TH* instance = malloc(sizeof(WSProtocolEncoderNexus_TH));

    instance->base.protocol = &ws_protocol_nexus_th;
    instance->generic.protocol_name = instance->base.protocol->name;

    instance->encoder.repeat = 12;
    instance->encoder.size_upload = ws_protocol_nexus_th_const.min_count_bit_for_found * 2 + 2;
    instance->encoder.upload = malloc(instance->encoder.size_upload * sizeof(LevelDuration));
    instance->encoder.is_running = false;
    return instance;
}

void ws_protocol_encoder_nexus_th_free(void* context) {
    furi_assert(context);
    WSProtocolEncoderNexus_TH* instance = context;
    free(instance->encoder.upload);
    free(instance);
}

void* ws_protocol_decoder_nexus_th_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    WSProtocolDecoderNexus_TH* instance = malloc(sizeof(WSProtocolDecoderNexus_TH));
    instance->base.protocol = &ws_protocol_nexus_th;
    instance->generic.protocol_name = instance->base.protocol->name;
    return instance;
}

void ws_protocol_decoder_nexus_th_free(void* context) {
    furi_assert(context);
    WSProtocolDecoderNexus_TH* instance = context;
    free(instance);
}

void ws_protocol_decoder_nexus_th_reset(void* context) {
    furi_assert(context);
    WSProtocolDecoderNexus_TH* instance = context;
    instance->decoder.parser_step = Nexus_THDecoderStepReset;
}

static bool ws_protocol_nexus_th_check(WSProtocolDecoderNexus_TH* instance) {
    uint8_t type = (instance->decoder.decode_data >> 8) & 0x0F;

    if((type == NEXUS_TH_CONST_DATA) && ((instance->decoder.decode_data >> 4) != 0xffffffff)) {
        return true;
    } else {
        return false;
    }
    return true;
}

static bool ws_protocol_encoder_nexus_th_get_upload(WSProtocolEncoderNexus_TH* instance) {
    furi_assert(instance);
    size_t index = 0;
    size_t size_upload = (instance->generic.data_count_bit * 2) + 2;
    if(size_upload > instance->encoder.size_upload) {
        FURI_LOG_E(TAG, "Size upload exceeds allocated encoder buffer.");
        return false;
    } else {
        instance->encoder.size_upload = size_upload;
    }

    for(uint8_t i = instance->generic.data_count_bit; i > 0; i--) {
        if(!bit_read(instance->generic.data, i - 1)) {
            //send bit 1
            instance->encoder.upload[index++] =
                level_duration_make(true, (uint32_t)ws_protocol_nexus_th_const.te_short);
            instance->encoder.upload[index++] =
                level_duration_make(false, (uint32_t)ws_protocol_nexus_th_const.te_short * 2);
        } else {
            //send bit 0
            instance->encoder.upload[index++] =
                level_duration_make(true, (uint32_t)ws_protocol_nexus_th_const.te_short);
            instance->encoder.upload[index++] =
                level_duration_make(false, (uint32_t)ws_protocol_nexus_th_const.te_short * 4);
        }
    }

    instance->encoder.upload[index++] =
        level_duration_make(true, (uint32_t)ws_protocol_nexus_th_const.te_short);
    instance->encoder.upload[index++] =
        level_duration_make(false, (uint32_t)ws_protocol_nexus_th_const.te_short * 8);

    return true;
}

SubGhzProtocolStatus
    ws_protocol_encoder_nexus_th_deserialize(void* context, FlipperFormat* flipper_format) {
    furi_assert(context);
    WSProtocolEncoderNexus_TH* instance = context;
    SubGhzProtocolStatus ret = SubGhzProtocolStatusError;
    do {
        ret = ws_block_generic_deserialize(&instance->generic, flipper_format);
        if(ret != SubGhzProtocolStatusOk) {
            break;
        }
        if((instance->generic.data_count_bit >
            ws_protocol_nexus_th_const.min_count_bit_for_found + 1)) {
            FURI_LOG_E(TAG, "Wrong number of bits in key");
            ret = SubGhzProtocolStatusErrorValueBitCount;
            break;
        }
        //optional parameter parameter
        flipper_format_read_uint32(
            flipper_format, "Repeat", (uint32_t*)&instance->encoder.repeat, 12);

        if(!ws_protocol_encoder_nexus_th_get_upload(instance)) {
            ret = SubGhzProtocolStatusErrorEncoderGetUpload;
            break;
        }
        instance->encoder.is_running = true;
    } while(false);

    return ret;
}

LevelDuration ws_protocol_encoder_nexus_th_yield(void* context) {
    WSProtocolEncoderNexus_TH* instance = context;

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

void ws_protocol_encoder_nexus_th_stop(void* context) {
    WSProtocolEncoderNexus_TH* instance = context;
    instance->encoder.is_running = false;
}

/**
 * Analysis of received data
 * @param instance Pointer to a WSBlockGeneric* instance
 */
static void ws_protocol_nexus_th_remote_controller(WSBlockGeneric* instance) {
    instance->id = (instance->data >> 28) & 0xFF;
    instance->battery_low = !((instance->data >> 27) & 1);
    instance->channel = ((instance->data >> 24) & 0x03) + 1;
    instance->btn = WS_NO_BTN;
    if(!((instance->data >> 23) & 1)) {
        instance->temp = (float)((instance->data >> 12) & 0x07FF) / 10.0f;
    } else {
        instance->temp = (float)((~(instance->data >> 12) & 0x07FF) + 1) / -10.0f;
    }

    instance->humidity = instance->data & 0xFF;
    if(instance->humidity > 95)
        instance->humidity = 95;
    else if(instance->humidity < 20)
        instance->humidity = 20;
}

void ws_protocol_decoder_nexus_th_feed(void* context, bool level, uint32_t duration) {
    furi_assert(context);
    WSProtocolDecoderNexus_TH* instance = context;

    switch(instance->decoder.parser_step) {
    case Nexus_THDecoderStepReset:
        if((!level) && (DURATION_DIFF(duration, ws_protocol_nexus_th_const.te_short * 8) <
                        ws_protocol_nexus_th_const.te_delta * 4)) {
            //Found sync
            instance->decoder.parser_step = Nexus_THDecoderStepSaveDuration;
            instance->decoder.decode_data = 0;
            instance->decoder.decode_count_bit = 0;
        }
        break;

    case Nexus_THDecoderStepSaveDuration:
        if(level) {
            instance->decoder.te_last = duration;
            instance->decoder.parser_step = Nexus_THDecoderStepCheckDuration;
        } else {
            instance->decoder.parser_step = Nexus_THDecoderStepReset;
        }
        break;

    case Nexus_THDecoderStepCheckDuration:
        if(!level) {
            if(DURATION_DIFF(duration, ws_protocol_nexus_th_const.te_short * 8) <
               ws_protocol_nexus_th_const.te_delta * 4) {
                //Found sync
                instance->decoder.parser_step = Nexus_THDecoderStepReset;
                if((instance->decoder.decode_count_bit ==
                    ws_protocol_nexus_th_const.min_count_bit_for_found) &&
                   ws_protocol_nexus_th_check(instance)) {
                    instance->generic.data = instance->decoder.decode_data;
                    instance->generic.data_count_bit = instance->decoder.decode_count_bit;
                    ws_protocol_nexus_th_remote_controller(&instance->generic);
                    if(instance->base.callback)
                        instance->base.callback(&instance->base, instance->base.context);
                    instance->decoder.parser_step = Nexus_THDecoderStepCheckDuration;
                }
                instance->decoder.decode_data = 0;
                instance->decoder.decode_count_bit = 0;

                break;
            } else if(
                (DURATION_DIFF(instance->decoder.te_last, ws_protocol_nexus_th_const.te_short) <
                 ws_protocol_nexus_th_const.te_delta) &&
                (DURATION_DIFF(duration, ws_protocol_nexus_th_const.te_short * 2) <
                 ws_protocol_nexus_th_const.te_delta * 2)) {
                subghz_protocol_blocks_add_bit(&instance->decoder, 0);
                instance->decoder.parser_step = Nexus_THDecoderStepSaveDuration;
            } else if(
                (DURATION_DIFF(instance->decoder.te_last, ws_protocol_nexus_th_const.te_short) <
                 ws_protocol_nexus_th_const.te_delta) &&
                (DURATION_DIFF(duration, ws_protocol_nexus_th_const.te_short * 4) <
                 ws_protocol_nexus_th_const.te_delta * 4)) {
                subghz_protocol_blocks_add_bit(&instance->decoder, 1);
                instance->decoder.parser_step = Nexus_THDecoderStepSaveDuration;
            } else {
                instance->decoder.parser_step = Nexus_THDecoderStepReset;
            }
        } else {
            instance->decoder.parser_step = Nexus_THDecoderStepReset;
        }
        break;
    }
}

uint32_t ws_protocol_decoder_nexus_th_get_hash_data(void* context) {
    furi_assert(context);
    WSProtocolDecoderNexus_TH* instance = context;
    return subghz_protocol_blocks_get_hash_data_long(
        &instance->decoder, (instance->decoder.decode_count_bit / 8) + 1);
}

SubGhzProtocolStatus ws_protocol_decoder_nexus_th_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset) {
    furi_assert(context);
    WSProtocolDecoderNexus_TH* instance = context;
    return ws_block_generic_serialize(&instance->generic, flipper_format, preset);
}

SubGhzProtocolStatus
    ws_protocol_decoder_nexus_th_deserialize(void* context, FlipperFormat* flipper_format) {
    furi_assert(context);
    WSProtocolDecoderNexus_TH* instance = context;
    return ws_block_generic_deserialize_check_count_bit(
        &instance->generic, flipper_format, ws_protocol_nexus_th_const.min_count_bit_for_found);
}

void ws_protocol_decoder_nexus_th_get_string(void* context, FuriString* output) {
    furi_assert(context);
    WSProtocolDecoderNexus_TH* instance = context;
    bool locale_is_metric = furi_hal_rtc_get_locale_units() == FuriHalRtcLocaleUnitsMetric;
    furi_string_cat_printf(
        output,
        "%s\r\n%dbit\r\n"
        "Key:0x%lX%08lX\r\n"
        "Sn:0x%lX Ch:%d  Bat:%d\r\n"
        "Temp:%3.1f %c Hum:%d%%",
        instance->generic.protocol_name,
        instance->generic.data_count_bit,
        (uint32_t)(instance->generic.data >> 32),
        (uint32_t)(instance->generic.data),
        instance->generic.id,
        instance->generic.channel,
        instance->generic.battery_low,
        (double)(locale_is_metric ? instance->generic.temp :
                                    locale_celsius_to_fahrenheit(instance->generic.temp)),
        locale_is_metric ? 'C' : 'F',
        instance->generic.humidity);
}

const SubGhzProtocolDecoder ws_protocol_nexus_th_decoder = {
    .alloc = ws_protocol_decoder_nexus_th_alloc,
    .free = ws_protocol_decoder_nexus_th_free,

    .feed = ws_protocol_decoder_nexus_th_feed,
    .reset = ws_protocol_decoder_nexus_th_reset,

    .get_hash_data = ws_protocol_decoder_nexus_th_get_hash_data,
    .serialize = ws_protocol_decoder_nexus_th_serialize,
    .deserialize = ws_protocol_decoder_nexus_th_deserialize,
    .get_string = ws_protocol_decoder_nexus_th_get_string,
};

const SubGhzProtocolEncoder ws_protocol_nexus_th_encoder = {
    .alloc = ws_protocol_encoder_nexus_th_alloc,
    .free = ws_protocol_encoder_nexus_th_free,

    .deserialize = ws_protocol_encoder_nexus_th_deserialize,
    .stop = ws_protocol_encoder_nexus_th_stop,
    .yield = ws_protocol_encoder_nexus_th_yield,
};

const SubGhzProtocol ws_protocol_nexus_th = {
    .name = WS_PROTOCOL_NEXUS_TH_NAME,
    .type = SubGhzProtocolTypeStatic,
    .flag = SubGhzProtocolFlag_433 | SubGhzProtocolFlag_315 | SubGhzProtocolFlag_868 |
            SubGhzProtocolFlag_AM | SubGhzProtocolFlag_Decodable | SubGhzProtocolFlag_Load |
            SubGhzProtocolFlag_Save | SubGhzProtocolFlag_Send,

    .decoder = &ws_protocol_nexus_th_decoder,
    .encoder = &ws_protocol_nexus_th_encoder,

    .filter = SubGhzProtocolFilter_Weather,
};
