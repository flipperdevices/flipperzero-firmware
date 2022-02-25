#include "raw.h"
#include <lib/flipper_format/flipper_format.h>
#include "../subghz_file_encoder_worker.h"

#include "../blocks/const.h"
#include "../blocks/decoder.h"
#include "../blocks/encoder.h"
#include "../blocks/generic.h"
#include "../blocks/math.h"

#define TAG "SubGhzProtocolRAW"
#define SUBGHZ_DOWNLOAD_MAX_SIZE 512

static const SubGhzBlockConst subghz_protocol_raw_const = {
    .te_short = 80,
    .te_long = 32700,
    .te_delta = 0,
    .min_count_bit_for_found = 0,
};

struct SubGhzProtocolDecoderRAW {
    SubGhzProtocolDecoderBase base;

    int32_t* upload_raw;
    uint16_t ind_write;
    Storage* storage;
    FlipperFormat* flipper_file;
    uint32_t file_is_open;
    string_t file_name;
    size_t sample_write;
    bool last_level;
};

struct SubGhzProtocolEncoderRAW {
    SubGhzProtocolEncoderBase base;

    bool is_runing;
    string_t file_name;
    SubGhzFileEncoderWorker* file_worker_encoder;
};

typedef enum {
    RAWFileIsOpenClose = 0,
    RAWFileIsOpenWrite,
    RAWFileIsOpenRead,
} RAWFilIsOpen;

const SubGhzProtocolDecoder subghz_protocol_raw_decoder = {
    .alloc = subghz_protocol_decoder_raw_alloc,
    .free = subghz_protocol_decoder_raw_free,

    .feed = subghz_protocol_decoder_raw_feed,
    .reset = subghz_protocol_decoder_raw_reset,

    .get_hash_data = NULL,
    .serialize = NULL,
    .get_string = subghz_protocol_decoder_raw_get_string,
    .save_file = NULL,
};

const SubGhzProtocolEncoder subghz_protocol_raw_encoder = {
    .alloc = subghz_protocol_encoder_raw_alloc,
    .free = subghz_protocol_encoder_raw_free,

    .deserialize = subghz_protocol_encoder_raw_deserialize,
    .stop = subghz_protocol_encoder_raw_stop,
    .yield = subghz_protocol_encoder_raw_yield,
    .load_file = NULL,
};

const SubGhzProtocol subghz_protocol_raw = {
    .name = SUBGHZ_PROTOCOL_RAW_NAME,
    .type = SubGhzProtocolTypeRAW,
    .flag = SubGhzProtocolFlag_433 | SubGhzProtocolFlag_868 | SubGhzProtocolFlag_315 |
            SubGhzProtocolFlag_AM | SubGhzProtocolFlag_FM | SubGhzProtocolFlag_RAW |
            SubGhzProtocolFlag_Load | SubGhzProtocolFlag_Save | SubGhzProtocolFlag_Send,

    .decoder = &subghz_protocol_raw_decoder,
    .encoder = &subghz_protocol_raw_encoder,
};

const char* subghz_protocol_raw_get_last_file_name(SubGhzProtocolDecoderRAW* instance) {
    return string_get_cstr(instance->file_name);
}

void subghz_protocol_raw_set_last_file_name(SubGhzProtocolDecoderRAW* instance, const char* name) {
    string_printf(instance->file_name, "%s", name);
}

bool subghz_protocol_raw_save_to_file_init(
    SubGhzProtocolDecoderRAW* instance,
    const char* dev_name,
    uint32_t frequency,
    FuriHalSubGhzPreset preset) {
    furi_assert(instance);

    instance->storage = furi_record_open("storage");
    instance->flipper_file = flipper_format_file_alloc(instance->storage);

    string_t temp_str;
    string_init(temp_str);
    bool init = false;

    do {
        // Create subghz folder directory if necessary
        if(!storage_simply_mkdir(instance->storage, SUBGHZ_RAW_FOLDER)) {
            break;
        }
        // Create saved directory if necessary
        if(!storage_simply_mkdir(instance->storage, SUBGHZ_RAW_FOLDER)) {
            break;
        }

        string_set(instance->file_name, dev_name);
        // First remove subghz device file if it was saved
        string_printf(temp_str, "%s/%s%s", SUBGHZ_RAW_FOLDER, dev_name, SUBGHZ_APP_EXTENSION);

        if(!storage_simply_remove(instance->storage, string_get_cstr(temp_str))) {
            break;
        }

        // Open file
        if(!flipper_format_file_open_always(instance->flipper_file, string_get_cstr(temp_str))) {
            FURI_LOG_E(TAG, "Unable to open file for write: %s", temp_str);
            break;
        }

        if(!flipper_format_write_header_cstr(
               instance->flipper_file, SUBGHZ_RAW_FILE_TYPE, SUBGHZ_RAW_FILE_VERSION)) {
            FURI_LOG_E(TAG, "Unable to add header");
            break;
        }

        if(!flipper_format_write_uint32(instance->flipper_file, "Frequency", &frequency, 1)) {
            FURI_LOG_E(TAG, "Unable to add Frequency");
            break;
        }
        if(!subghz_block_generic_get_preset_name(preset, temp_str)) {
            break;
        }
        if(!flipper_format_write_string_cstr(
               instance->flipper_file, "Preset", string_get_cstr(temp_str))) {
            FURI_LOG_E(TAG, "Unable to add Preset");
            break;
        }

        if(!flipper_format_write_string_cstr(
               instance->flipper_file, "Protocol", instance->base.protocol->name)) {
            FURI_LOG_E(TAG, "Unable to add Protocol");
            break;
        }

        instance->upload_raw = malloc(SUBGHZ_DOWNLOAD_MAX_SIZE * sizeof(int32_t));
        instance->file_is_open = RAWFileIsOpenWrite;
        instance->sample_write = 0;
        init = true;
    } while(0);

    string_clear(temp_str);

    return init;
}

static bool subghz_protocol_raw_save_to_file_write(SubGhzProtocolDecoderRAW* instance) {
    furi_assert(instance);

    bool is_write = false;
    if(instance->file_is_open == RAWFileIsOpenWrite) {
        if(!flipper_format_write_int32(
               instance->flipper_file, "RAW_Data", instance->upload_raw, instance->ind_write)) {
            FURI_LOG_E(TAG, "Unable to add RAW_Data");
        } else {
            instance->sample_write += instance->ind_write;
            instance->ind_write = 0;
            is_write = true;
        }
    }
    return is_write;
}

void subghz_protocol_raw_save_to_file_stop(SubGhzProtocolDecoderRAW* instance) {
    furi_assert(instance);

    if(instance->file_is_open == RAWFileIsOpenWrite && instance->ind_write)
        subghz_protocol_raw_save_to_file_write(instance);
    if(instance->file_is_open != RAWFileIsOpenClose) {
        free(instance->upload_raw);
        instance->upload_raw = NULL;
        flipper_format_file_close(instance->flipper_file);
        flipper_format_free(instance->flipper_file);
        furi_record_close("storage");
    }

    instance->file_is_open = RAWFileIsOpenClose;
}

size_t subghz_protocol_raw_get_sample_write(SubGhzProtocolDecoderRAW* instance) {
    return instance->sample_write + instance->ind_write;
}

void* subghz_protocol_decoder_raw_alloc(SubGhzEnvironment* environment) {
    SubGhzProtocolDecoderRAW* instance = malloc(sizeof(SubGhzProtocolDecoderRAW));
    instance->base.protocol = &subghz_protocol_raw;
    instance->upload_raw = NULL;
    instance->ind_write = 0;
    instance->last_level = false;
    instance->file_is_open = RAWFileIsOpenClose;
    string_init(instance->file_name);

    return instance;
}

void subghz_protocol_decoder_raw_free(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderRAW* instance = context;
    string_clear(instance->file_name);
    free(instance);
}

void subghz_protocol_decoder_raw_reset(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderRAW* instance = context;
    instance->ind_write = 0;
    instance->last_level = false;
}

void subghz_protocol_decoder_raw_feed(void* context, bool level, uint32_t duration) {
    furi_assert(context);
    SubGhzProtocolDecoderRAW* instance = context;

    if(instance->upload_raw != NULL) {
        if(duration > subghz_protocol_raw_const.te_short) {
            if(duration > subghz_protocol_raw_const.te_long)
                duration = subghz_protocol_raw_const.te_long;
            if(instance->last_level != level) {
                instance->last_level = (level ? true : false);
                instance->upload_raw[instance->ind_write++] = (level ? duration : -duration);
            }
        }

        if(instance->ind_write == SUBGHZ_DOWNLOAD_MAX_SIZE) {
            subghz_protocol_raw_save_to_file_write(instance);
        }
    }
}

void subghz_protocol_decoder_raw_get_string(void* context, string_t output) {
    furi_assert(context);
    //SubGhzProtocolDecoderRAW* instance = context;
    //ToDo no use
    string_cat_printf(output, "RAW Date");
}

//bool subghz_protocol_raw_to_load_protocol_from_file(
//     FlipperFormat* flipper_file,
//     SubGhzProtocolDecoderRAW* instance,
//     const char* file_path) {
//     furi_assert(file_path);
//     subghz_protocol_raw_set_last_file_name(instance, file_path);
//     return true;
// }

void* subghz_protocol_encoder_raw_alloc(SubGhzEnvironment* environment) {
    SubGhzProtocolEncoderRAW* instance = malloc(sizeof(SubGhzProtocolEncoderRAW));

    instance->base.protocol = &subghz_protocol_raw;
    string_init(instance->file_name);
    instance->is_runing = false;
    return instance;
}

void subghz_protocol_encoder_raw_stop(void* context) {
    SubGhzProtocolEncoderRAW* instance = context;
    instance->is_runing = false;
    if(subghz_file_encoder_worker_is_running(instance->file_worker_encoder)) {
        subghz_file_encoder_worker_stop(instance->file_worker_encoder);
        subghz_file_encoder_worker_free(instance->file_worker_encoder);
    }
}

void subghz_protocol_encoder_raw_free(void* context) {
    furi_assert(context);
    SubGhzProtocolEncoderRAW* instance = context;
    subghz_protocol_encoder_raw_stop(instance);
    string_clear(instance->file_name);
    free(instance);
}

void subghz_protocol_raw_file_encoder_worker_set_callback_end(
    SubGhzProtocolEncoderRAW* instance,
    SubGhzProtocolEncoderRAWCallbackEnd callback_end,
    void* context_end) {
    furi_assert(instance);
    furi_assert(callback_end);
    subghz_file_encoder_worker_callback_end(
        instance->file_worker_encoder, callback_end, context_end);
}

static bool subghz_protocol_raw_encoder_worker_init(SubGhzProtocolEncoderRAW* instance) {
    furi_assert(instance);

    instance->file_worker_encoder = subghz_file_encoder_worker_alloc();
    if(subghz_file_encoder_worker_start(
           instance->file_worker_encoder, string_get_cstr(instance->file_name))) {
        //the worker needs a file in order to open and read part of the file
        osDelay(100);
        instance->is_runing = true;
    } else {
        subghz_protocol_encoder_raw_stop(instance);
    }
    return instance->is_runing;
}

void subghz_protocol_encoder_raw_set_file_name(
    SubGhzProtocolEncoderRAW* instance,
    const char* name) {
    string_printf(instance->file_name, "%s", name);
}

bool subghz_protocol_encoder_raw_deserialize(void* context, FlipperFormat* flipper_format) {
    furi_assert(context);
    SubGhzProtocolEncoderRAW* instance = context;
    bool res = false;
    do {
        // if(!subghz_block_generic_deserialize(&instance->generic, flipper_format)) {
        //     FURI_LOG_E(TAG, "Deserialize error");
        //     break;
        // }

        // //optional parameter parameter
        // flipper_format_read_uint32(
        //     flipper_format, "Repeat", (uint32_t*)&instance->encoder.repeat, 1);

        subghz_protocol_raw_encoder_worker_init(instance);
        //instance->encoder.is_runing = true;

        res = true;
    } while(false);

    return res;
}

// bool subghz_protocol_encoder_raw_load(
//     void* context,
//     uint64_t key,
//     uint8_t count_bit,
//     size_t repeat) {
//     furi_assert(context);
//     SubGhzProtocolEncoderRAW* instance = context;
//     string_set(instance->file_name, "/ext/subghz/saved/Raw_signal_9.sub");
//     subghz_protocol_raw_encoder_worker_init(instance);
//     return true;
// }

LevelDuration subghz_protocol_encoder_raw_yield(void* context) {
    SubGhzProtocolEncoderRAW* instance = context;

    if(!instance->is_runing) return level_duration_reset();
    return subghz_file_encoder_worker_get_level_duration(instance->file_worker_encoder);
}
