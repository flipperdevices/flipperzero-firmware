#include "dickert_mahs.h"

#include "../blocks/const.h"
#include "../blocks/decoder.h"
#include "../blocks/encoder.h"
#include "../blocks/generic.h"
#include "../blocks/math.h"

#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_rtc.h>

/*
 * Help
 * 
 *
 */

#define TAG "SubGhzProtocolDicketMAHS"
#define DICKERT_MAHS_12_COUNT_BIT 12
#define DICKERT_MAHS_24_COUNT_BIT 24
#define PRASTEL_COUNT_BIT 25
#define PRASTEL_NAME "Prastel"
#define AIRFORCE_COUNT_BIT 18
#define AIRFORCE_NAME "Airforce"

#define MINUS 0x0
#define ZERO 0x1
#define PLUS 0x3

Storage* storage;
File* file;
const char* file_path = "/ext/subghz/dickert_log.txt";
// PCM, Short: 450, Long: 450, Gap: 1300, Sync: n/a (452+-52us, 852,0+-73us)
static const SubGhzBlockConst subghz_protocol_dickert_mahs_const = {
    .te_short = 400,
    .te_long = 800,
    .te_delta = 100,
    .min_count_bit_for_found = 36,
};

struct SubGhzProtocolDecoderDickertMAHS {
    SubGhzProtocolDecoderBase base;

    SubGhzBlockDecoder decoder;
    SubGhzBlockGeneric generic;

    uint32_t tmp[2];
    uint8_t tmp_cnt;
};

struct SubGhzProtocolEncoderDickertMAHS {
    SubGhzProtocolEncoderBase base;

    SubGhzProtocolBlockEncoder encoder;
    SubGhzBlockGeneric generic;
};

typedef enum {
    DickertMAHSDecoderStepReset = 0,
    DickertMAHSDecoderStepInitial,
    DickertMAHSDecoderStepRecording,
} DickertMAHSDecoderStep;

const SubGhzProtocolDecoder subghz_protocol_dickert_mahs_decoder = {
    .alloc = subghz_protocol_decoder_dickert_mahs_alloc,
    .free = subghz_protocol_decoder_dickert_mahs_free,

    .feed = subghz_protocol_decoder_dickert_mahs_feed,
    .reset = subghz_protocol_decoder_dickert_mahs_reset,

    .get_hash_data = subghz_protocol_decoder_dickert_mahs_get_hash_data,
    .serialize = subghz_protocol_decoder_dickert_mahs_serialize,
    .deserialize = subghz_protocol_decoder_dickert_mahs_deserialize,
    .get_string = subghz_protocol_decoder_dickert_mahs_get_string,
};

const SubGhzProtocolEncoder subghz_protocol_dickert_mahs_encoder = {
    .alloc = subghz_protocol_encoder_dickert_mahs_alloc,
    .free = subghz_protocol_encoder_dickert_mahs_free,

    .deserialize = subghz_protocol_encoder_dickert_mahs_deserialize,
    .stop = subghz_protocol_encoder_dickert_mahs_stop,
    .yield = subghz_protocol_encoder_dickert_mahs_yield,
};

const SubGhzProtocol subghz_protocol_dickert_mahs = {
    .name = SUBGHZ_PROTOCOL_DICKERT_MAHS_NAME,
    .type = SubGhzProtocolTypeStatic,
    .flag = SubGhzProtocolFlag_433 | SubGhzProtocolFlag_AM | SubGhzProtocolFlag_Decodable |
            SubGhzProtocolFlag_Load | SubGhzProtocolFlag_Save | SubGhzProtocolFlag_Send,

    .decoder = &subghz_protocol_dickert_mahs_decoder,
    .encoder = &subghz_protocol_dickert_mahs_encoder,
};

void* subghz_protocol_encoder_dickert_mahs_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    SubGhzProtocolEncoderDickertMAHS* instance = malloc(sizeof(SubGhzProtocolEncoderDickertMAHS));

    instance->base.protocol = &subghz_protocol_dickert_mahs;
    instance->generic.protocol_name = instance->base.protocol->name;

    instance->encoder.repeat = 10;
    instance->encoder.size_upload = 128;
    instance->encoder.upload = malloc(instance->encoder.size_upload * sizeof(LevelDuration));
    instance->encoder.is_running = false;
    return instance;
}

void subghz_protocol_encoder_dickert_mahs_free(void* context) {
    furi_assert(context);
    SubGhzProtocolEncoderDickertMAHS* instance = context;
    free(instance->encoder.upload);
    free(instance);
}

/**
 * Generating an upload from data.
 * @param instance Pointer to a SubGhzProtocolEncoderDickertMAHS instance
 * @return true On success
 */
static bool
    subghz_protocol_encoder_dickert_mahs_get_upload(SubGhzProtocolEncoderDickertMAHS* instance) {
    furi_assert(instance);
    size_t index = 0;
    size_t size_upload = (instance->generic.data_count_bit * 2) + 2;
    if(size_upload > instance->encoder.size_upload) {
        FURI_LOG_E(TAG, "Size upload exceeds allocated encoder buffer.");
        return false;
    } else {
        instance->encoder.size_upload = size_upload;
    }

    //Send start bit
    instance->encoder.upload[index++] =
        level_duration_make(true, (uint32_t)subghz_protocol_dickert_mahs_const.te_short);
    //Send key data
    for(uint8_t i = instance->generic.data_count_bit; i > 0; i--) {
        if(bit_read(instance->generic.data, i - 1)) {
            //send bit 1
            instance->encoder.upload[index++] =
                level_duration_make(false, (uint32_t)subghz_protocol_dickert_mahs_const.te_long);
            instance->encoder.upload[index++] =
                level_duration_make(true, (uint32_t)subghz_protocol_dickert_mahs_const.te_short);
        } else {
            //send bit 0
            instance->encoder.upload[index++] =
                level_duration_make(false, (uint32_t)subghz_protocol_dickert_mahs_const.te_short);
            instance->encoder.upload[index++] =
                level_duration_make(true, (uint32_t)subghz_protocol_dickert_mahs_const.te_long);
        }
    }
    return true;
}

SubGhzProtocolStatus
    subghz_protocol_encoder_dickert_mahs_deserialize(void* context, FlipperFormat* flipper_format) {
    furi_assert(context);
    SubGhzProtocolEncoderDickertMAHS* instance = context;
    SubGhzProtocolStatus ret = SubGhzProtocolStatusError;
    do {
        ret = subghz_block_generic_deserialize(&instance->generic, flipper_format);
        //printf("Dickert_deserialized()\n");
        if(ret != SubGhzProtocolStatusOk) {
            break;
        }

        // Allow for longer keys (<) instead of !=
        if((instance->generic.data_count_bit <
            subghz_protocol_dickert_mahs_const.min_count_bit_for_found)) {
            FURI_LOG_E(TAG, "Wrong number of bits in key");
            ret = SubGhzProtocolStatusErrorValueBitCount;
            break;
        }
        //optional parameter parameter
        flipper_format_read_uint32(
            flipper_format, "Repeat", (uint32_t*)&instance->encoder.repeat, 1);

        if(!subghz_protocol_encoder_dickert_mahs_get_upload(instance)) {
            ret = SubGhzProtocolStatusErrorEncoderGetUpload;
            break;
        }
        instance->encoder.is_running = true;
    } while(false);

    return ret;
}

void subghz_protocol_encoder_dickert_mahs_stop(void* context) {
    SubGhzProtocolEncoderDickertMAHS* instance = context;
    instance->encoder.is_running = false;
}

LevelDuration subghz_protocol_encoder_dickert_mahs_yield(void* context) {
    SubGhzProtocolEncoderDickertMAHS* instance = context;

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

void* subghz_protocol_decoder_dickert_mahs_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    SubGhzProtocolDecoderDickertMAHS* instance = malloc(sizeof(SubGhzProtocolDecoderDickertMAHS));
    instance->base.protocol = &subghz_protocol_dickert_mahs;
    instance->generic.protocol_name = instance->base.protocol->name;
    instance->tmp_cnt = 0;

    storage = furi_record_open("storage");
    file = storage_file_alloc(storage);
    if(!storage_file_open(file, file_path, FSAM_WRITE, FSOM_OPEN_APPEND)) {
        FURI_LOG_E("dickert", "Couldn´t open log file");
    } else {
        FURI_LOG_I("dickert", "Log file opened: 0x%p", file);
        FuriString* msg = furi_string_alloc_set("+++++++++++++++++++++++++++++++++\r\n");
        log_to_file(msg);
        furi_string_set(msg, "Dickert_MAHS Decoder starting up.\r\n");
        log_to_file(msg);
        furi_string_set(msg, "+++++++++++++++++++++++++++++++++\r\n");
        log_to_file(msg);
        furi_string_free(msg);
    }

    return instance;
}

void subghz_protocol_decoder_dickert_mahs_free(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderDickertMAHS* instance = context;
    free(instance);

    FuriString* msg = furi_string_alloc_set("+++++++++++++++++++++++++++++++++++\r\n");
    log_to_file(msg);
    furi_string_set(msg, "Dickert_MAHS Decoder shutting down.\r\n");
    log_to_file(msg);
    furi_string_set(msg, "+++++++++++++++++++++++++++++++++++\r\n");
    log_to_file(msg);
    furi_string_free(msg);
    storage_file_close(file);
    storage_file_free(file);
    furi_record_close("storage");
}

void subghz_protocol_decoder_dickert_mahs_reset(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderDickertMAHS* instance = context;
    instance->decoder.parser_step = DickertMAHSDecoderStepReset;
    // printf("Decoder_Dickert_MAHS_reset()\n");
}

void subghz_protocol_decoder_dickert_mahs_feed(void* context, bool level, uint32_t duration) {
    furi_assert(context);
    SubGhzProtocolDecoderDickertMAHS* instance = context;

    switch(instance->decoder.parser_step) {
    case DickertMAHSDecoderStepReset:
        // Check if done
        if(instance->decoder.decode_count_bit >=
           subghz_protocol_dickert_mahs_const.min_count_bit_for_found) {
            instance->generic.serial = 0x0;
            instance->generic.btn = 0x0;

            instance->generic.data = instance->decoder.decode_data;
            instance->generic.data_count_bit = instance->decoder.decode_count_bit;

            // Log out automatically
            FuriString* full_buf = furi_string_alloc();
            parseBuffer(context, full_buf);
            furi_string_free(full_buf);

            if(instance->base.callback)
                instance->base.callback(&instance->base, instance->base.context);

            instance->decoder.decode_data = 0;
            instance->decoder.decode_count_bit = 0;
        }

        if((!level) && (duration > 10 * subghz_protocol_dickert_mahs_const.te_short)) {
            //Found header DICKERT_MAHS
            instance->decoder.parser_step = DickertMAHSDecoderStepInitial;
        }
        break;
    case DickertMAHSDecoderStepInitial:
        if(!level) {
            break;
        } else if(
            DURATION_DIFF(duration, subghz_protocol_dickert_mahs_const.te_short) <
            subghz_protocol_dickert_mahs_const.te_delta) {
            //Found start bit DICKERT_MAHS
            instance->decoder.parser_step = DickertMAHSDecoderStepRecording;
            instance->decoder.decode_data = 0;
            instance->decoder.decode_count_bit = 0;
        } else {
            instance->decoder.parser_step = DickertMAHSDecoderStepReset;
        }
        break;
    case DickertMAHSDecoderStepRecording:
        if((!level && instance->tmp_cnt == 0) || (level && instance->tmp_cnt == 1)) {
            instance->tmp[instance->tmp_cnt] = duration;

            instance->tmp_cnt++;

            if(instance->tmp_cnt == 2) {
                if(DURATION_DIFF(instance->tmp[0] + instance->tmp[1], 1200) <
                   subghz_protocol_dickert_mahs_const.te_delta) {
                    if(DURATION_DIFF(instance->tmp[0], subghz_protocol_dickert_mahs_const.te_long) <
                       subghz_protocol_dickert_mahs_const.te_delta) {
                        subghz_protocol_blocks_add_bit(&instance->decoder, 1);
                    } else if(
                        DURATION_DIFF(
                            instance->tmp[0], subghz_protocol_dickert_mahs_const.te_short) <
                        subghz_protocol_dickert_mahs_const.te_delta) {
                        subghz_protocol_blocks_add_bit(&instance->decoder, 0);
                    }

                    instance->tmp_cnt = 0;
                } else {
                    instance->tmp_cnt = 0;
                    instance->decoder.parser_step = DickertMAHSDecoderStepReset;
                }
            }
        } else {
            instance->tmp_cnt = 0;
            instance->decoder.parser_step = DickertMAHSDecoderStepReset;
        }

        break;
    }
}

uint8_t subghz_protocol_decoder_dickert_mahs_get_hash_data(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderDickertMAHS* instance = context;
    return subghz_protocol_blocks_get_hash_data(
        &instance->decoder, (instance->decoder.decode_count_bit / 8) + 1);
}

SubGhzProtocolStatus subghz_protocol_decoder_dickert_mahs_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset) {
    furi_assert(context);
    SubGhzProtocolDecoderDickertMAHS* instance = context;
    return subghz_block_generic_serialize(&instance->generic, flipper_format, preset);
}

SubGhzProtocolStatus
    subghz_protocol_decoder_dickert_mahs_deserialize(void* context, FlipperFormat* flipper_format) {
    furi_assert(context);
    SubGhzProtocolDecoderDickertMAHS* instance = context;
    SubGhzProtocolStatus ret = SubGhzProtocolStatusError;
    // printf("Decoder_Dickert_MAHS_deserialize()\n");
    do {
        ret = subghz_block_generic_deserialize(&instance->generic, flipper_format);
        if(ret != SubGhzProtocolStatusOk) {
            break;
        }

        // Allow for longer keys (<) instead of !=
        if((instance->generic.data_count_bit <
            subghz_protocol_dickert_mahs_const.min_count_bit_for_found)) {
            FURI_LOG_E(TAG, "Wrong number of bits in key");
            ret = SubGhzProtocolStatusErrorValueBitCount;
            break;
        }
    } while(false);
    return ret;
}

void parseBuffer(void* context, FuriString* output) {
    SubGhzProtocolDecoderDickertMAHS* instance = context;

    // We assume we have only decodes < 64 bit!
    uint64_t data = instance->generic.data;
    uint8_t bits[36] = {0};
    char full_64bit[67];
    memset(full_64bit, '0', sizeof(full_64bit));
    full_64bit[64] = '\r';
    full_64bit[65] = '\n';
    full_64bit[66] = '\0';
    // Read the bit array 2 bit at a time and build our symbols
    //char code[19] = {0};
    //char user_dips[11] = {0};
    //char fact_dips[9] = {0};

    // Convert uint64_t into bit array
    for(int i = 35; i >= 0; i--) {
        if(data & 1) {
            bits[i] = 1;
        }
        data >>= 1;
    }

    // Convert full uint64_t into bit array
    data = instance->generic.data;
    for(int i = 63; i >= 0; i--) {
        if(data & 1) {
            full_64bit[i] = '1';
        }
        data >>= 1;
    }
    // Decode symbols
    FuriString* code = furi_string_alloc();
    for(int i = 0; i < 35; i += 2) {
        uint8_t dip = (bits[i] << 1) + bits[i + 1];
        //  PLUS  = 3, // 0b11
        //  ZERO  = 1, // 0b01
        //  MINUS = 0, // 0x00
        if(dip == ZERO) {
            furi_string_cat(code, "0");
        } else if(dip == MINUS) {
            furi_string_cat(code, "-");
        } else if(dip == PLUS) {
            furi_string_cat(code, "+");
        } else {
            furi_string_cat(code, "?");
        }
    }

    FuriString* user_dips = furi_string_alloc();
    FuriString* fact_dips = furi_string_alloc();
    furi_string_set_n(user_dips, code, 0, 10);
    furi_string_set_n(fact_dips, code, 10, 8);

    FuriString* full_buf = furi_string_alloc();
    furi_string_set_str(full_buf, full_64bit);
    log_to_file(full_buf);
    furi_string_free(full_buf);

    furi_string_cat_printf(
        output,
        "%s\r\n"
        "User-Dips:\t%s\r\n"
        "Fac-Code:\t%s\r\n",
        instance->generic.protocol_name,
        furi_string_get_cstr(user_dips),
        furi_string_get_cstr(fact_dips));
    log_to_file(output);
    furi_string_free(user_dips);
    furi_string_free(fact_dips);
    furi_string_free(code);
}

void subghz_protocol_decoder_dickert_mahs_get_string(void* context, FuriString* output) {
    furi_assert(context);
    parseBuffer(context, output);
}

void log_to_file(FuriString* message) {
    char* msgbuf;
    DateTime datetime = {0};
    furi_hal_rtc_get_datetime(&datetime);

    msgbuf = (char*)malloc(256 * sizeof(char));

    snprintf(
        msgbuf,
        256,
        "%04i%02i%02i%02i%02i%02i: ",
        datetime.year,
        datetime.month,
        datetime.day,
        datetime.hour,
        datetime.minute,
        datetime.second);

    const char* msg = furi_string_get_cstr(message);
    strncat(msgbuf, msg, 256 - strlen(msgbuf));
    if(file == NULL) {
        FURI_LOG_E("dickert", "Logfile handle is NULL. Won´t log.");
    } else {
        storage_file_write(file, msgbuf, strlen(msgbuf));
        FURI_LOG_I("dickert", "Written to logfile.");
    }
    free(msgbuf);
}
