/**
 * The majority of this code is from the keeloq.c file.  The Genie garage door openers
 * use a modified version of the Keeloq algorithm.  The speed of the transmission is
 * different (twice as fast).  We don't have the manufacturer's code, so instead we
 * use the button+serial number to look through a file of previously captured codes.
 * 
 * You can use the Genie Recorder app to capture codes and save them to a .GNE file.
 * It uses the GPIO pin on the Flipper Zero to click the remote automatically and
 * extract the codes.  A full capture is 65536 codes, so it takes about 2 days.  Your
 * receiver may become out-of-sync with your remote & you may need to pair it again.
 * 
 * @CodeAllNight
*/

#include "genie.h"
#include "keeloq_common.h"

#include "../blocks/const.h"
#include "../blocks/decoder.h"
#include "../blocks/encoder.h"
#include "../blocks/generic.h"
#include "../blocks/math.h"

#define TAG "SubGhzProtocolGenie"

// Should be major version of supported Genie Recorder (.gne) files
#define GENIE_MAJOR_VERSION 2

#define GENIE_APPS_DATA_FOLDER EXT_PATH("apps_data")
#define GENIE_SAVE_FOLDER      \
    GENIE_APPS_DATA_FOLDER "/" \
                           "genie"
#define GENIE_FILE_EXT ".gne"

enum {
    GENIE_MAGIC = 0, // 2 bytes
    GENIE_VERSION = 2, // 2 bytes
    GENIE_SN = 4, // 4 bytes
    GENIE_LAST_SENT = 8, // 2 bytes
    GENIE_REC_COUNT = 10, // 2 bytes
    GENIE_RESERVED = 12, // 4 bytes
    GENIE_DATA = 16, // 64K bytes
} genie_file;

static const SubGhzBlockConst subghz_protocol_genie_const = {
    .te_short = 200,
    .te_long = 400,
    .te_delta = 70,
    .min_count_bit_for_found = 64,
};

struct SubGhzProtocolDecoderGenie {
    SubGhzProtocolDecoderBase base;

    SubGhzBlockDecoder decoder;
    SubGhzBlockGeneric generic;

    uint16_t header_count;
};

struct SubGhzProtocolEncoderGenie {
    SubGhzProtocolEncoderBase base;

    SubGhzProtocolBlockEncoder encoder;
    SubGhzBlockGeneric generic;
};

typedef enum {
    GenieDecoderStepReset = 0,
    GenieDecoderStepCheckPreambula,
    GenieDecoderStepSaveDuration,
    GenieDecoderStepCheckDuration,
} GenieDecoderStep;

const SubGhzProtocolDecoder subghz_protocol_genie_decoder = {
    .alloc = subghz_protocol_decoder_genie_alloc,
    .free = subghz_protocol_decoder_genie_free,

    .feed = subghz_protocol_decoder_genie_feed,
    .reset = subghz_protocol_decoder_genie_reset,

    .get_hash_data = subghz_protocol_decoder_genie_get_hash_data,
    .serialize = subghz_protocol_decoder_genie_serialize,
    .deserialize = subghz_protocol_decoder_genie_deserialize,
    .get_string = subghz_protocol_decoder_genie_get_string,
};

const SubGhzProtocolEncoder subghz_protocol_genie_encoder = {
    .alloc = subghz_protocol_encoder_genie_alloc,
    .free = subghz_protocol_encoder_genie_free,

    .deserialize = subghz_protocol_encoder_genie_deserialize,
    .stop = subghz_protocol_encoder_genie_stop,
    .yield = subghz_protocol_encoder_genie_yield,
};

const SubGhzProtocol subghz_protocol_genie = {
    .name = SUBGHZ_PROTOCOL_GENIE_NAME,
    .type = SubGhzProtocolTypeDynamic,
    .flag = SubGhzProtocolFlag_315 | SubGhzProtocolFlag_433 | SubGhzProtocolFlag_868 |
            SubGhzProtocolFlag_AM | SubGhzProtocolFlag_Decodable | SubGhzProtocolFlag_Load |
            SubGhzProtocolFlag_Save | SubGhzProtocolFlag_Send,

    .decoder = &subghz_protocol_genie_decoder,
    .encoder = &subghz_protocol_genie_encoder,
};

void* subghz_protocol_encoder_genie_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    SubGhzProtocolEncoderGenie* instance = malloc(sizeof(SubGhzProtocolEncoderGenie));

    instance->base.protocol = &subghz_protocol_genie;
    instance->generic.protocol_name = instance->base.protocol->name;

    instance->encoder.repeat = 20;
    instance->encoder.size_upload = 256;
    instance->encoder.upload = malloc(instance->encoder.size_upload * sizeof(LevelDuration));
    instance->encoder.is_running = false;

    return instance;
}

void subghz_protocol_encoder_genie_free(void* context) {
    furi_assert(context);
    SubGhzProtocolEncoderGenie* instance = context;
    free(instance->encoder.upload);
    free(instance);
}

/**
 * Set serial number and button number from data
 * @param instance Pointer to a SubGhzBlockGeneric* instance
*/
static void subghz_protocol_genie_set_sn_and_btn(SubGhzBlockGeneric* instance) {
    uint64_t key = subghz_protocol_blocks_reverse_key(instance->data, instance->data_count_bit);
    uint32_t key_fix = key >> 32;
    instance->serial = key_fix & 0x0FFFFFFF;
    instance->btn = key_fix >> 28;
}

/**
 * Read 16-bits from file
 * @param file Pointer to a File instance
 * @return 16-bit unsigned integer
*/
static uint16_t subghz_protocol_genie_storage_file_read16(File* file) {
    uint16_t read = 0;
    char buffer[2] = {0};
    storage_file_read(file, buffer, 2);
    read |= (buffer[0] << 8);
    read |= buffer[1];
    return read;
}

/**
 * Read 32-bits from file
 * @param file Pointer to a File instance
 * @return 32-bit unsigned integer
*/
static uint32_t subghz_protocol_genie_storage_file_read32(File* file) {
    uint32_t read = 0;
    char buffer[4] = {0};
    storage_file_read(file, buffer, 4);
    read = (buffer[0] << 24);
    read |= (buffer[1] << 16);
    read |= (buffer[2] << 8);
    read |= buffer[3];
    return read;
}

/**
 * Write 16-bits to file
 * @param file Pointer to a File instance
 * @param data 16-bit unsigned integer
 * @return true On success
*/
static bool subghz_protocol_genie_storage_file_write16(File* file, uint16_t data) {
    char buffer[2] = {0};
    buffer[0] = (data >> 8) & 0xFF;
    buffer[1] = data & 0xFF;
    return storage_file_write(file, buffer, 2) == 2;
}

/**
 * Finds next code from the .gne file associated with the given code_low.
 * @param code_low 32-bit unsigned integer (static part of code)
 * @param code_high 32-bit unsigned integer (dynamic part of code)
 * @param update_index If true, the index of the last sent code will be updated.
 * @return 64-bit unsigned integer (next code) or 0xFFFFFFFFFFFFFFFF if not found
*/
static uint64_t subghz_protocol_genie_next_code_from_file(
    uint32_t code_low,
    uint32_t code_high,
    bool update_index) {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    char buffer[256] = {0};
    snprintf(buffer, 128, "%s/%08lX%s", GENIE_SAVE_FOLDER, code_low, GENIE_FILE_EXT);

    uint64_t result = 0xFFFFFFFFFFFFFFFF;
    File* file = NULL;
    do {
        if(!storage) {
            FURI_LOG_E(TAG, "Failed to access storage");
            break;
        }

        file = storage_file_alloc(storage);
        if(!file) {
            FURI_LOG_E(TAG, "Failed to alloc file");
            break;
        }

        if(!storage_file_exists(storage, buffer)) {
            FURI_LOG_E(TAG, "File %s does not exist, rerun Genie Recorder app!", buffer);
            break;
        }

        if(storage_file_open(file, buffer, FSAM_READ_WRITE, FSOM_OPEN_EXISTING)) {
            if(!storage_file_seek(file, GENIE_VERSION, true)) {
                FURI_LOG_E(TAG, "Failed to seek to %d", GENIE_VERSION);
                break;
            }

            uint16_t version = subghz_protocol_genie_storage_file_read16(file);
            if((version >> 8) > GENIE_MAJOR_VERSION) {
                FURI_LOG_E(TAG, "Unsupported version: %04X", version);
                break;
            }

            if(!storage_file_seek(file, GENIE_SN, true)) {
                FURI_LOG_E(TAG, "Failed to seek to %d", GENIE_SN);
                break;
            }

            uint32_t low = subghz_protocol_genie_storage_file_read32(file);
            if(low != code_low) {
                FURI_LOG_E(TAG, "Btn/SN mismatch. Expected: %08lX, got: %08lX", code_low, low);
                break;
            }

            if(!storage_file_seek(file, GENIE_LAST_SENT, true)) {
                FURI_LOG_E(TAG, "Failed to seek to %d", GENIE_SN);
                break;
            }

            uint16_t last_sent = subghz_protocol_genie_storage_file_read16(file);
            last_sent -= last_sent % (COUNT_OF(buffer) / 4);
            result = code_high;

            bool found = false;
            int j = 0;
            for(int i = 0; i <= 65536; i++) {
                if(last_sent % (COUNT_OF(buffer) / 4) == 0) {
                    if(!storage_file_seek(file, GENIE_DATA + (last_sent * 4), true)) {
                        FURI_LOG_E(TAG, "Failed to seek to %d", GENIE_DATA + (last_sent * 4));
                        break;
                    }

                    if(!storage_file_read(file, buffer, COUNT_OF(buffer))) {
                        FURI_LOG_E(TAG, "Failed to read %d", COUNT_OF(buffer));
                        break;
                    }

                    j = 0;
                }

                uint32_t high = (buffer[j++] << 24);
                high |= (buffer[j++] << 16);
                high |= (buffer[j++] << 8);
                high |= buffer[j++];

                if(found && high != 0) {
                    result = high;
                    break;
                }

                found |= (high == code_high);

                if(last_sent == 0xFFFF) {
                    last_sent = 0;
                } else {
                    last_sent++;
                }
            }

            if(found && update_index) {
                if(!storage_file_seek(file, GENIE_LAST_SENT, true)) {
                    FURI_LOG_E(TAG, "Failed to seek to %d", GENIE_SN);
                    break;
                }
                if(!subghz_protocol_genie_storage_file_write16(file, last_sent)) {
                    FURI_LOG_E(TAG, "Failed to write last sent.  %d", last_sent);
                    break;
                }
            } else if(!found) {
                FURI_LOG_E(TAG, "Code not found: %08lX", code_high);
                break;
            }
        } else {
            FURI_LOG_E(TAG, "Failed to open file");
            break;
        }
    } while(false);

    if(file) {
        storage_file_close(file);
        storage_file_free(file);
    }

    furi_record_close(RECORD_STORAGE);
    return result;
}

/**
 * Finds next code for this remote.
 * @param instance Pointer to a SubGhzProtocolEncoderGenie* instance
 * @param counter_up attempt to find next code if the value is true
*/
static void
    subghz_protocol_genie_find_next_code(SubGhzProtocolEncoderGenie* instance, bool counter_up) {
    instance->generic.data_count_bit = 64;
    instance->generic.cnt = 0x0000;

    if(counter_up) {
        uint32_t code_found_hi = instance->generic.data >> 32;
        uint32_t code_found_lo = instance->generic.data & 0x00000000ffffffff;
        uint64_t next_code =
            subghz_protocol_genie_next_code_from_file(code_found_lo, code_found_hi, true);

        FURI_LOG_D(
            TAG,
            "Low: %08lX High: %08lX New-High: %08lX",
            code_found_lo,
            code_found_hi,
            (uint32_t)next_code);
        instance->generic.cnt = next_code &
                                0xffff; // We don't know counter value, just use bottom of code.

        if((next_code & 0xFFFFFFFF) == 0xFFFFFFFF) {
            instance->generic.data = ((uint64_t)code_found_hi) << 32 | code_found_lo;
        } else {
            instance->generic.data = ((uint64_t)next_code) << 32 | code_found_lo;
        }
    }
}

/** 
 * Key generation from simple data
 * @param instance Pointer to a SubGhzProtocolEncoderGenie* instance
 * @param btn Button number, 4 bit
 * @param counter_up increasing the counter if the value is true
 */
static bool subghz_protocol_genie_gen_data(
    SubGhzProtocolEncoderGenie* instance,
    uint8_t btn,
    bool counter_up) {
    uint32_t fix = (uint32_t)btn << 28 | instance->generic.serial;
    uint32_t hop = 0;
    uint64_t code_found_reverse;

    subghz_protocol_genie_find_next_code(instance, counter_up);

    code_found_reverse = subghz_protocol_blocks_reverse_key(
        instance->generic.data, instance->generic.data_count_bit);
    hop = code_found_reverse & 0x00000000ffffffff;

    if(hop) {
        uint64_t yek = (uint64_t)fix << 32 | hop;
        instance->generic.data =
            subghz_protocol_blocks_reverse_key(yek, instance->generic.data_count_bit);
    }

    return true;
}

/**
 * Generating an upload from data.
 * @param instance Pointer to a SubGhzProtocolEncoderGenie instance
 * @return true On success
 */
static bool
    subghz_protocol_encoder_genie_get_upload(SubGhzProtocolEncoderGenie* instance, uint8_t btn) {
    furi_assert(instance);

    // Generate next key
    if(!subghz_protocol_genie_gen_data(instance, btn, true)) {
        return false;
    }

    size_t index = 0;
    size_t size_upload = 11 * 2 + 2 + (instance->generic.data_count_bit * 2) + 4;
    if(size_upload > instance->encoder.size_upload) {
        FURI_LOG_E(TAG, "Size upload exceeds allocated encoder buffer.");
        return false;
    } else {
        instance->encoder.size_upload = size_upload;
    }

    //Send header
    for(uint8_t i = 11; i > 0; i--) {
        instance->encoder.upload[index++] =
            level_duration_make(true, (uint32_t)subghz_protocol_genie_const.te_short);
        instance->encoder.upload[index++] =
            level_duration_make(false, (uint32_t)subghz_protocol_genie_const.te_short);
    }
    instance->encoder.upload[index++] =
        level_duration_make(true, (uint32_t)subghz_protocol_genie_const.te_short);
    instance->encoder.upload[index++] =
        level_duration_make(false, (uint32_t)subghz_protocol_genie_const.te_short * 10);

    //Send key data
    for(uint8_t i = instance->generic.data_count_bit; i > 0; i--) {
        if(bit_read(instance->generic.data, i - 1)) {
            //send bit 1
            instance->encoder.upload[index++] =
                level_duration_make(true, (uint32_t)subghz_protocol_genie_const.te_short);
            instance->encoder.upload[index++] =
                level_duration_make(false, (uint32_t)subghz_protocol_genie_const.te_long);
        } else {
            //send bit 0
            instance->encoder.upload[index++] =
                level_duration_make(true, (uint32_t)subghz_protocol_genie_const.te_long);
            instance->encoder.upload[index++] =
                level_duration_make(false, (uint32_t)subghz_protocol_genie_const.te_short);
        }
    }
    // +send 2 status bit
    instance->encoder.upload[index++] =
        level_duration_make(true, (uint32_t)subghz_protocol_genie_const.te_short);
    instance->encoder.upload[index++] =
        level_duration_make(false, (uint32_t)subghz_protocol_genie_const.te_long);
    // send end
    instance->encoder.upload[index++] =
        level_duration_make(true, (uint32_t)subghz_protocol_genie_const.te_short);
    instance->encoder.upload[index++] =
        level_duration_make(false, (uint32_t)subghz_protocol_genie_const.te_short * 40);

    return true;
}

SubGhzProtocolStatus
    subghz_protocol_encoder_genie_deserialize(void* context, FlipperFormat* flipper_format) {
    furi_assert(context);
    SubGhzProtocolEncoderGenie* instance = context;
    SubGhzProtocolStatus ret = SubGhzProtocolStatusError;
    do {
        ret = subghz_block_generic_deserialize_check_count_bit(
            &instance->generic,
            flipper_format,
            subghz_protocol_genie_const.min_count_bit_for_found);
        if(ret != SubGhzProtocolStatusOk) {
            break;
        }
        if(instance->generic.data_count_bit !=
           subghz_protocol_genie_const.min_count_bit_for_found) {
            FURI_LOG_E(TAG, "Wrong number of bits in key");
            break;
        }

        if(!flipper_format_rewind(flipper_format)) {
            FURI_LOG_E(TAG, "Rewind error");
            break;
        }

        subghz_protocol_genie_set_sn_and_btn(&instance->generic);

        //optional parameter parameter
        flipper_format_read_uint32(
            flipper_format, "Repeat", (uint32_t*)&instance->encoder.repeat, 1);

        // Get_upload will generate the next key.
        if(!subghz_protocol_encoder_genie_get_upload(instance, instance->generic.btn)) {
            ret = SubGhzProtocolStatusErrorEncoderGetUpload;
            break;
        }
        if(!flipper_format_rewind(flipper_format)) {
            FURI_LOG_E(TAG, "Rewind error");
            ret = SubGhzProtocolStatusErrorParserOthers;
            break;
        }
        uint8_t key_data[sizeof(uint64_t)] = {0};
        for(size_t i = 0; i < sizeof(uint64_t); i++) {
            key_data[sizeof(uint64_t) - i - 1] = (instance->generic.data >> (i * 8)) & 0xFF;
        }
        if(!flipper_format_update_hex(flipper_format, "Key", key_data, sizeof(uint64_t))) {
            FURI_LOG_E(TAG, "Unable to add Key");
            ret = SubGhzProtocolStatusErrorParserKey;
            break;
        }

        instance->encoder.is_running = true;
    } while(false);

    return ret;
}

void subghz_protocol_encoder_genie_stop(void* context) {
    SubGhzProtocolEncoderGenie* instance = context;
    instance->encoder.is_running = false;
}

LevelDuration subghz_protocol_encoder_genie_yield(void* context) {
    SubGhzProtocolEncoderGenie* instance = context;

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

void* subghz_protocol_decoder_genie_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    SubGhzProtocolDecoderGenie* instance = malloc(sizeof(SubGhzProtocolDecoderGenie));
    instance->base.protocol = &subghz_protocol_genie;
    instance->generic.protocol_name = instance->base.protocol->name;

    return instance;
}

void subghz_protocol_decoder_genie_free(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderGenie* instance = context;

    free(instance);
}

void subghz_protocol_decoder_genie_reset(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderGenie* instance = context;
    instance->decoder.parser_step = GenieDecoderStepReset;
}

void subghz_protocol_decoder_genie_feed(void* context, bool level, uint32_t duration) {
    furi_assert(context);
    SubGhzProtocolDecoderGenie* instance = context;

    switch(instance->decoder.parser_step) {
    case GenieDecoderStepReset:
        if((level) && DURATION_DIFF(duration, subghz_protocol_genie_const.te_short) <
                          subghz_protocol_genie_const.te_delta) {
            instance->decoder.parser_step = GenieDecoderStepCheckPreambula;
            instance->header_count++;
        }
        break;
    case GenieDecoderStepCheckPreambula:
        if((!level) && (DURATION_DIFF(duration, subghz_protocol_genie_const.te_short) <
                        subghz_protocol_genie_const.te_delta)) {
            instance->decoder.parser_step = GenieDecoderStepReset;
            break;
        }
        if((instance->header_count > 2) &&
           (DURATION_DIFF(duration, subghz_protocol_genie_const.te_short * 10) <
            subghz_protocol_genie_const.te_delta * 10)) {
            // Found header
            instance->decoder.parser_step = GenieDecoderStepSaveDuration;
            instance->decoder.decode_data = 0;
            instance->decoder.decode_count_bit = 0;
        } else {
            instance->decoder.parser_step = GenieDecoderStepReset;
            instance->header_count = 0;
        }
        break;
    case GenieDecoderStepSaveDuration:
        if(level) {
            instance->decoder.te_last = duration;
            instance->decoder.parser_step = GenieDecoderStepCheckDuration;
        }
        break;
    case GenieDecoderStepCheckDuration:
        if(!level) {
            if(duration >= ((uint32_t)subghz_protocol_genie_const.te_short * 2 +
                            subghz_protocol_genie_const.te_delta)) {
                // Found end TX
                instance->decoder.parser_step = GenieDecoderStepReset;
                if((instance->decoder.decode_count_bit >=
                    subghz_protocol_genie_const.min_count_bit_for_found) &&
                   (instance->decoder.decode_count_bit <=
                    subghz_protocol_genie_const.min_count_bit_for_found + 2)) {
                    if(instance->generic.data != instance->decoder.decode_data) {
                        instance->generic.data = instance->decoder.decode_data;
                        instance->generic.data_count_bit =
                            subghz_protocol_genie_const.min_count_bit_for_found;
                        if(instance->base.callback)
                            instance->base.callback(&instance->base, instance->base.context);
                    }
                    instance->decoder.decode_data = 0;
                    instance->decoder.decode_count_bit = 0;
                    instance->header_count = 0;
                }
                break;
            } else if(
                (DURATION_DIFF(instance->decoder.te_last, subghz_protocol_genie_const.te_short) <
                 subghz_protocol_genie_const.te_delta) &&
                (DURATION_DIFF(duration, subghz_protocol_genie_const.te_long) <
                 subghz_protocol_genie_const.te_delta * 2)) {
                if(instance->decoder.decode_count_bit <
                   subghz_protocol_genie_const.min_count_bit_for_found) {
                    subghz_protocol_blocks_add_bit(&instance->decoder, 1);
                } else {
                    instance->decoder.decode_count_bit++;
                }
                instance->decoder.parser_step = GenieDecoderStepSaveDuration;
            } else if(
                (DURATION_DIFF(instance->decoder.te_last, subghz_protocol_genie_const.te_long) <
                 subghz_protocol_genie_const.te_delta * 2) &&
                (DURATION_DIFF(duration, subghz_protocol_genie_const.te_short) <
                 subghz_protocol_genie_const.te_delta)) {
                if(instance->decoder.decode_count_bit <
                   subghz_protocol_genie_const.min_count_bit_for_found) {
                    subghz_protocol_blocks_add_bit(&instance->decoder, 0);
                } else {
                    instance->decoder.decode_count_bit++;
                }
                instance->decoder.parser_step = GenieDecoderStepSaveDuration;
            } else {
                instance->decoder.parser_step = GenieDecoderStepReset;
                instance->header_count = 0;
            }
        } else {
            instance->decoder.parser_step = GenieDecoderStepReset;
            instance->header_count = 0;
        }
        break;
    }
}

uint8_t subghz_protocol_decoder_genie_get_hash_data(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderGenie* instance = context;
    return subghz_protocol_blocks_get_hash_data(
        &instance->decoder, (instance->decoder.decode_count_bit / 8) + 1);
}

SubGhzProtocolStatus subghz_protocol_decoder_genie_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset) {
    furi_assert(context);
    SubGhzProtocolDecoderGenie* instance = context;

    SubGhzProtocolStatus res =
        subghz_block_generic_serialize(&instance->generic, flipper_format, preset);

    subghz_protocol_genie_set_sn_and_btn(&instance->generic);

    return res;
}

SubGhzProtocolStatus
    subghz_protocol_decoder_genie_deserialize(void* context, FlipperFormat* flipper_format) {
    furi_assert(context);
    SubGhzProtocolDecoderGenie* instance = context;
    SubGhzProtocolStatus res = SubGhzProtocolStatusError;
    do {
        if(SubGhzProtocolStatusOk !=
           subghz_block_generic_deserialize(&instance->generic, flipper_format)) {
            FURI_LOG_E(TAG, "Deserialize error");
            break;
        }
        if(instance->generic.data_count_bit !=
           subghz_protocol_genie_const.min_count_bit_for_found) {
            FURI_LOG_E(TAG, "Wrong number of bits in key");
            break;
        }

        if(!flipper_format_rewind(flipper_format)) {
            FURI_LOG_E(TAG, "Rewind error");
            break;
        }

        res = SubGhzProtocolStatusOk;
    } while(false);

    return res;
}

void subghz_protocol_decoder_genie_get_string(void* context, FuriString* output) {
    furi_assert(context);
    SubGhzProtocolDecoderGenie* instance = context;

    uint32_t code_found_hi = instance->generic.data >> 32;
    uint32_t code_found_lo = instance->generic.data & 0x00000000ffffffff;

    // If we are in the genie thread (running the Genie Recorder app),
    // output the protocol & key, then exit without looking for the next code.
    const char* name = furi_thread_get_name(furi_thread_get_current_id());
    if(name != NULL && strcmp(name, "genie-rx") == 0) {
        furi_string_cat_printf(
            output,
            "%s %dbit\r\n"
            "Key:%08lX%08lX\r\n",
            instance->generic.protocol_name,
            instance->generic.data_count_bit,
            code_found_hi,
            code_found_lo);
        return;
    }

    uint64_t next_code =
        subghz_protocol_genie_next_code_from_file(code_found_lo, code_found_hi, false);
    subghz_protocol_genie_set_sn_and_btn(&instance->generic);

    furi_string_cat_printf(
        output,
        "%s %dbit\r\n"
        "Key:%08lX%08lX\r\n",
        instance->generic.protocol_name,
        instance->generic.data_count_bit,
        code_found_hi,
        code_found_lo);

    if((next_code & 0xFFFFFFFF) == 0xFFFFFFFF) {
        furi_string_cat_printf(
            output,
            "No keys found. Please run the\r\n"
            "Genie Recorder app to\r\n"
            "extract keys from the\r\n"
            "remote.");
        instance->generic.cnt = 0;
        instance->generic.data = ((uint64_t)code_found_hi) << 32 | code_found_lo;
    } else if((next_code & 0xFFFFFFFF) == code_found_hi) {
        furi_string_cat_printf(
            output,
            "Key missing. Please run the\r\n"
            "Genie Recorder app to\r\n"
            "extract additional keys from\r\n"
            "the remote.");
        instance->generic.cnt = 0;
        instance->generic.data = ((uint64_t)code_found_hi) << 32 | code_found_lo;
    } else {
        furi_string_cat_printf(output, "Next code: %08lX", (uint32_t)next_code);
    }
}
