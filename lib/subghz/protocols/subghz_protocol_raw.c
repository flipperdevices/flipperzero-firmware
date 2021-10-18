#include "subghz_protocol_raw.h"
#include "file-worker.h"

#define SUBGHZ_DOWNLOAD_MAX_SIZE 512 //8192

struct SubGhzProtocolRAW {
    SubGhzProtocolCommon common;
    int16_t* download_raw;
    size_t ind_write;
    FileWorker* file_worker;
    bool file_is_open;
    string_t file_name;
    size_t sample_write;
};

typedef enum {
    RAWDecoderStepReset = 0,
    RAWDecoderStepCheckPreambula,
    RAWDecoderStepSaveDuration,
    RAWDecoderStepCheckDuration,
} RAWDecoderStep;

SubGhzProtocolRAW* subghz_protocol_raw_alloc(void) {
    SubGhzProtocolRAW* instance = furi_alloc(sizeof(SubGhzProtocolRAW));

    //instance->download_raw = furi_alloc(SUBGHZ_DOWNLOAD_MAX_SIZE * sizeof(uint16_t));
    instance->download_raw = NULL;
    instance->ind_write = 0;

    instance->file_worker = file_worker_alloc(false);
    instance->file_is_open = false;
    string_init(instance->file_name);

    instance->common.name = "RAW";
    instance->common.code_min_count_bit_for_found = 0;
    instance->common.te_short = 100;
    instance->common.te_long = 32700;
    instance->common.te_delta = 0;
    instance->common.type_protocol = SubGhzProtocolCommonTypeRAW;
    instance->common.to_load_protocol_from_file =
        (SubGhzProtocolCommonLoadFromFile)subghz_protocol_raw_to_load_protocol_from_file;
    instance->common.to_string = (SubGhzProtocolCommonToStr)subghz_protocol_raw_to_str;
    //instance->common.to_load_protocol =
    //    (SubGhzProtocolCommonLoadFromRAW)subghz_decoder_raw_to_load_protocol;
    instance->common.get_upload_protocol =
        (SubGhzProtocolCommonEncoderGetUpLoad)subghz_protocol_raw_send_key;

    return instance;
}

void subghz_protocol_raw_free(SubGhzProtocolRAW* instance) {
    furi_assert(instance);
    string_clear(instance->file_name);
    //free(instance->download_raw);
    file_worker_free(instance->file_worker);
    free(instance);
}

bool subghz_protocol_raw_send_key(
    SubGhzProtocolRAW* instance,
    SubGhzProtocolCommonEncoder* encoder) {
    furi_assert(instance);
    furi_assert(encoder);

    bool loaded = false;
    string_t temp_str;
    string_init(temp_str);
    int res = 0;
    // int data = 0;

    string_printf(temp_str, "%s/%s%s", SUBGHZ_RAW_PATH_FOLDER, "Raw", SUBGHZ_APP_EXTENSION);

    do {
        if(!file_worker_open(
               instance->file_worker, string_get_cstr(temp_str), FSAM_READ, FSOM_OPEN_EXISTING)) {
            return res;
        }

        if(!file_worker_read_until(instance->file_worker, temp_str, '\n')) {
            break;
        }
        if(!file_worker_read_until(instance->file_worker, temp_str, '\n')) {
            break;
        }
        if(!file_worker_read_until(instance->file_worker, temp_str, '\n')) {
            break;
        }

        if(!file_worker_read_until(instance->file_worker, temp_str, '\n')) {
            break;
        }
        // strlen("RAW_Data: ") = 10
        string_right(temp_str, 10);

        //uint8_t buf_key[512] = {0};
        // if(!subghz_protocol_common_read_hex(temp_str, buf_key, 512)) {
        //     break;
        // }

        // for(uint8_t i = 0; i < 8; i++) {
        //     instance->common.code_last_found = instance->common.code_last_found << 8 | buf_key[i];
        // }

        //loaded = true;
    } while(0);

    string_clear(temp_str);

    return loaded;
}

void subghz_protocol_raw_reset(SubGhzProtocolRAW* instance) {
    instance->ind_write = 0;
}

void subghz_protocol_raw_print(SubGhzProtocolRAW* instance) {
    for(size_t i = 0; i < instance->ind_write; i++) {
        printf("%d\r\n", instance->download_raw[i]);
    }
}

void subghz_protocol_raw_parse(SubGhzProtocolRAW* instance, bool level, uint32_t duration) {
    if(instance->download_raw != NULL) {
        if(duration > instance->common.te_short) {
            if(duration > instance->common.te_long) duration = instance->common.te_long;
            instance->download_raw[instance->ind_write++] = (level ? duration : -duration);
        }

        if(instance->ind_write == SUBGHZ_DOWNLOAD_MAX_SIZE) {
            //subghz_protocol_raw_print(instance);
            subghz_protocol_save_raw_to_file_write(instance);
        }
    }
}

void subghz_protocol_raw_to_str(SubGhzProtocolRAW* instance, string_t output) {
    string_cat_printf(output, "RAW Date");
}

// void subghz_decoder_raw_to_load_protocol(SubGhzProtocolRAW* instance, void* context) {
//     furi_assert(context);
//     furi_assert(instance);
//     SubGhzProtocolCommonLoad* data = context;
//     instance->common.code_last_found = data->code_found;
//     instance->common.code_last_count_bit = data->code_count_bit;
//     subghz_protocol_raw_check_remote_controller(instance);
// }

const char* subghz_protocol_get_last_file_name(SubGhzProtocolRAW* instance) {
    return string_get_cstr(instance->file_name);
}

bool subghz_protocol_save_raw_to_file_init(
    SubGhzProtocolRAW* instance,
    const char* dev_name,
    uint32_t frequency,
    FuriHalSubGhzPreset preset) {
    furi_assert(instance);

    string_t dev_file_name;
    string_init(dev_file_name);
    string_t temp_str;
    string_init(temp_str);
    bool init = false;

    do {
        // Create subghz folder directory if necessary
        if(!file_worker_mkdir(instance->file_worker, SUBGHZ_RAW_FOLDER)) {
            break;
        }
        // Create saved directory if necessary
        if(!file_worker_mkdir(instance->file_worker, SUBGHZ_RAW_PATH_FOLDER)) {
            break;
        }

        file_worker_get_next_filename(
            instance->file_worker,
            SUBGHZ_RAW_PATH_FOLDER,
            dev_name,
            SUBGHZ_APP_EXTENSION,
            temp_str);

        string_set(instance->file_name, temp_str);

        string_printf(
            dev_file_name,
            "%s/%s%s",
            SUBGHZ_RAW_PATH_FOLDER,
            string_get_cstr(temp_str),
            SUBGHZ_APP_EXTENSION);
        // Open file
        if(!file_worker_open(
               instance->file_worker,
               string_get_cstr(dev_file_name),
               FSAM_WRITE,
               FSOM_CREATE_ALWAYS)) {
            break;
        }
        //Get string frequency preset protocol
        string_printf(
            temp_str,
            "Frequency: %d\n"
            "Preset: %d\n"
            "Protocol: RAW\n",
            (int)frequency,
            (int)preset);

        if(!file_worker_write(
               instance->file_worker, string_get_cstr(temp_str), string_size(temp_str))) {
            break;
        }

        instance->download_raw = furi_alloc(SUBGHZ_DOWNLOAD_MAX_SIZE * sizeof(uint16_t));
        instance->file_is_open = true;
        instance->sample_write = 0;
        init = true;
    } while(0);

    string_clear(temp_str);
    string_clear(dev_file_name);

    return init;
}

void subghz_protocol_save_raw_to_file_stop(SubGhzProtocolRAW* instance) {
    furi_assert(instance);
    if(instance->file_is_open && instance->ind_write)
        subghz_protocol_save_raw_to_file_write(instance);
    free(instance->download_raw);
    instance->download_raw = NULL;
    file_worker_close(instance->file_worker);
}

bool subghz_protocol_save_raw_to_file_write(SubGhzProtocolRAW* instance) {
    furi_assert(instance);

    string_t temp_str;
    string_init(temp_str);
    bool is_write = false;
    if(instance->file_is_open) {
        do {
            string_printf(temp_str, "RAW_Data: ");
            // Prepare and write data to file
            if(!file_worker_write(
                   instance->file_worker, string_get_cstr(temp_str), string_size(temp_str))) {
                break;
            }

            for(size_t i = 0; i < instance->ind_write - 1; i++) {
                //string_printf(temp_str, "%d, ", instance->download_raw[i]);
                string_printf(temp_str, "%08X, ", instance->download_raw[i]);
                // Prepare and write data to file
                if(!file_worker_write(
                       instance->file_worker, string_get_cstr(temp_str), string_size(temp_str))) {
                    break;
                }
            }

            // if(!file_worker_write(
            //        instance->file_worker,
            //        instance->download_raw,
            //        instance->ind_write * sizeof(uint16_t))) {
            //     break;
            // }

            //string_printf(temp_str, "%d\n", instance->download_raw[instance->ind_write - 1]);
            string_printf(temp_str, "%08X\n", instance->download_raw[instance->ind_write - 1]);
            // Prepare and write data to file
            if(!file_worker_write(
                   instance->file_worker, string_get_cstr(temp_str), string_size(temp_str))) {
                break;
            }
            //subghz_protocol_raw_print(instance);
            instance->sample_write += instance->ind_write;
            instance->ind_write = 0;
            is_write = true;
        } while(0);
        string_clear(temp_str);
    }
    return is_write;
}

size_t subghz_save_protocol_raw_get_sample_write(SubGhzProtocolRAW* instance) {
    return instance->sample_write + instance->ind_write;
}

bool subghz_protocol_raw_to_load_protocol_from_file(
    FileWorker* file_worker,
    SubGhzProtocolRAW* instance) {
    return true;
}