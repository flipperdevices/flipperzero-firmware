#include "subghz_protocol_raw.h"
#include "file-worker.h"

#define SUBGHZ_DOWNLOAD_MAX_SIZE 512 //8192

struct SubGhzProtocolRAW {
    SubGhzProtocolCommon common;
    int16_t* download_raw;
    size_t ind_write;
    FileWorker* file_worker;
    bool file_is_open;
    //size_t ind_raw_data;
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

    instance->download_raw = furi_alloc(SUBGHZ_DOWNLOAD_MAX_SIZE * sizeof(uint16_t));
    instance->ind_write = 0;

    instance->file_worker = file_worker_alloc(false);
    instance->file_is_open = false;

    instance->common.name = "RAW";
    instance->common.code_min_count_bit_for_found = 0;
    instance->common.te_short = 100;
    instance->common.te_long = 32700;
    instance->common.te_delta = 0;
    instance->common.type_protocol = SubGhzProtocolCommonTypeRAW;
    //instance->common.to_string = (SubGhzProtocolCommonToStr)subghz_protocol_raw_to_str;
    //instance->common.to_load_protocol =
    //    (SubGhzProtocolCommonLoadFromRAW)subghz_decoder_raw_to_load_protocol;

    return instance;
}

void subghz_protocol_raw_free(SubGhzProtocolRAW* instance) {
    furi_assert(instance);
    free(instance->download_raw);
    file_worker_free(instance->file_worker);
    free(instance);
}

void subghz_protocol_raw_reset(SubGhzProtocolRAW* instance) {
    //instance->common.parser_step = RAWDecoderStepReset;
    instance->ind_write = 0;
}

void subghz_protocol_raw_print(SubGhzProtocolRAW* instance) {
    for(size_t i = 0; i < instance->ind_write; i++) {
        printf("%d\r\n", instance->download_raw[i]);
    }
}

void subghz_protocol_raw_parse(SubGhzProtocolRAW* instance, bool level, uint32_t duration) {
    if(duration > instance->common.te_short) {
        if(duration > instance->common.te_long) duration = instance->common.te_long;
        instance->download_raw[instance->ind_write++] = (level ? duration : -duration);
    }

    if(instance->ind_write == SUBGHZ_DOWNLOAD_MAX_SIZE) {
        //subghz_protocol_raw_print(instance);
        subghz_protocol_save_raw_to_file_write(instance);
    }
}

// void subghz_protocol_raw_to_str(SubGhzProtocolRAW* instance, string_t output) {
//     uint32_t code_found_hi = instance->common.code_last_found >> 32;
//     uint32_t code_found_lo = instance->common.code_last_found & 0x00000000ffffffff;

//     string_cat_printf(
//         output,
//         "%s %dbit\r\n"
//         "Key:%08lX%08lX\r\n"
//         "Sn:%07lX Btn:%lX Cnt:%04X\r\n",
//         instance->common.name,
//         instance->common.code_last_count_bit,
//         code_found_hi,
//         code_found_lo,
//         instance->common.serial,
//         instance->common.btn,
//         instance->common.cnt);
// }

// void subghz_decoder_raw_to_load_protocol(SubGhzProtocolRAW* instance, void* context) {
//     furi_assert(context);
//     furi_assert(instance);
//     SubGhzProtocolCommonLoad* data = context;
//     instance->common.code_last_found = data->code_found;
//     instance->common.code_last_count_bit = data->code_count_bit;
//     subghz_protocol_raw_check_remote_controller(instance);
// }

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
            "Protocol: RAW\n"
            "Frequency: %d\n"
            "Preset: %d\n",
            (int)frequency,
            (int)preset);

        if(!file_worker_write(
               instance->file_worker, string_get_cstr(temp_str), string_size(temp_str))) {
            break;
        }

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
                string_printf(temp_str, "%d, ", instance->download_raw[i]);
                // Prepare and write data to file
                if(!file_worker_write(
                       instance->file_worker, string_get_cstr(temp_str), string_size(temp_str))) {
                    break;
                }
            }
            string_printf(temp_str, "%d\n", instance->download_raw[instance->ind_write - 1]);
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

bool subghz_save_protocol_raw_to_file(SubGhzProtocolRAW* instance, const char* dev_name) {
    furi_assert(instance);

    FileWorker* file_worker = file_worker_alloc(false);
    string_t dev_file_name;
    string_init(dev_file_name);
    string_t temp_str;
    string_init(temp_str);
    bool saved = false;

    do {
        // Create subghz folder directory if necessary
        if(!file_worker_mkdir(file_worker, SUBGHZ_RAW_FOLDER)) {
            break;
        }
        // Create saved directory if necessary
        if(!file_worker_mkdir(file_worker, SUBGHZ_RAW_PATH_FOLDER)) {
            break;
        }
        // First remove subghz device file if it was saved
        string_printf(
            dev_file_name, "%s/%s%s", SUBGHZ_RAW_PATH_FOLDER, dev_name, SUBGHZ_APP_EXTENSION);
        if(!file_worker_remove(file_worker, string_get_cstr(dev_file_name))) {
            break;
        }
        // Open file
        if(!file_worker_open(
               file_worker, string_get_cstr(dev_file_name), FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
            break;
        }
        //Get string frequency preset protocol
        string_printf(
            temp_str,
            "Protocol: RAW\n"
            "Frequency: %d\n"
            "Preset: %d\n",
            (int)433920000,
            (int)1);

        if(!file_worker_write(file_worker, string_get_cstr(temp_str), string_size(temp_str))) {
            break;
        }

        string_printf(temp_str, "RAW_Data: ");
        // Prepare and write data to file
        if(!file_worker_write(file_worker, string_get_cstr(temp_str), string_size(temp_str))) {
            break;
        }

        for(size_t i = 0; i < instance->ind_write; i++) {
            string_printf(temp_str, "%d, ", instance->download_raw[i]);
            // Prepare and write data to file
            if(!file_worker_write(file_worker, string_get_cstr(temp_str), string_size(temp_str))) {
                break;
            }
        }

        string_printf(temp_str, "\n");
        // Prepare and write data to file
        if(!file_worker_write(file_worker, string_get_cstr(temp_str), string_size(temp_str))) {
            break;
        }

        saved = true;
    } while(0);

    string_clear(temp_str);
    string_clear(dev_file_name);
    file_worker_close(file_worker);
    file_worker_free(file_worker);

    return saved;
}