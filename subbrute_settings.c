#include "subbrute_settings.h"
#include "subbrute_i.h"

#define TAG "SubBruteSettings"

#define SUBBRUTE_SETTINGS_FILE_TYPE "Sub-GHz BruteForcer Settings File"
#define SUBBRUTE_SETTINGS_FILE_VERSION 1
#define SUBBRUTE_SETTINGS_PATH APP_DATA_PATH("bruteforcer.settings")

#define SUBBRUTE_FIELD_LAST_INDEX "LastIndex"
#define SUBBRUTE_FIELD_REPEAT_VALUES "RepeatValue"

SubBruteSettings* subbrute_settings_alloc(void) {
    SubBruteSettings* instance = malloc(sizeof(SubBruteSettings));
    return instance;
}

void subbrute_settings_free(SubBruteSettings* instance) {
    furi_assert(instance);
    free(instance);
}

void subbrute_settings_load(SubBruteSettings* instance) {
    furi_assert(instance);

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* fff_data_file = flipper_format_file_alloc(storage);

    uint32_t temp_last_index = 0;
    uint8_t temp_repeat_values[SubBruteAttackTotalCount] = {0};
    bool was_read_last_index = false;
    bool was_read_repeat_values = false;

    storage_common_migrate(
        storage, EXT_PATH("subghz-bruteforcer.settings"), SUBBRUTE_SETTINGS_PATH);

    if(FSE_OK == storage_sd_status(storage) && SUBBRUTE_SETTINGS_PATH &&
       flipper_format_file_open_existing(fff_data_file, SUBBRUTE_SETTINGS_PATH)) {
        was_read_last_index = flipper_format_read_uint32(
            fff_data_file, SUBBRUTE_FIELD_LAST_INDEX, (uint32_t*)&temp_last_index, 1);
        was_read_repeat_values = flipper_format_read_hex(
            fff_data_file,
            SUBBRUTE_FIELD_REPEAT_VALUES,
            temp_repeat_values,
            SubBruteAttackTotalCount);
    } else {
        FURI_LOG_E(TAG, "Error open file %s", SUBBRUTE_SETTINGS_PATH);
    }

    if(was_read_last_index && temp_last_index < SubBruteAttackTotalCount) {
        instance->last_index = temp_last_index;
    } else {
        FURI_LOG_W(TAG, "Last used index not found or can't be used!");
        instance->last_index = (uint32_t)SubBruteAttackCAME12bit433;
    }
    if(was_read_repeat_values) {
        for(size_t i = 0; i < SubBruteAttackTotalCount; i++) {
            uint8_t protocol_count = subbrute_protocol_repeats_count(i);
            uint8_t max_protocol_count = protocol_count * 3;
            if(temp_repeat_values[i] < protocol_count) {
                instance->repeat_values[i] = protocol_count;
            } else if(temp_repeat_values[i] > max_protocol_count) {
                instance->repeat_values[i] = max_protocol_count;
            } else {
                instance->repeat_values[i] = temp_repeat_values[i];
            }
        }
    } else {
        FURI_LOG_W(TAG, "Last used repeat values can't be used!");
        for(size_t i = 0; i < SubBruteAttackTotalCount; i++) {
            instance->repeat_values[i] = subbrute_protocol_repeats_count(i);
        }
    }

    flipper_format_file_close(fff_data_file);
    flipper_format_free(fff_data_file);
    furi_record_close(RECORD_STORAGE);
}

bool subbrute_settings_save(SubBruteSettings* instance) {
    furi_assert(instance);

    bool saved = false;
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* file = flipper_format_file_alloc(storage);

    do {
        if(FSE_OK != storage_sd_status(storage)) {
            break;
        }
        // Open file
        if(!flipper_format_file_open_always(file, SUBBRUTE_SETTINGS_PATH)) {
            break;
        }
        // Write header
        if(!flipper_format_write_header_cstr(
               file, SUBBRUTE_SETTINGS_FILE_TYPE, SUBBRUTE_SETTINGS_FILE_VERSION)) {
            break;
        }
        if(!flipper_format_insert_or_update_uint32(
               file, SUBBRUTE_FIELD_LAST_INDEX, &instance->last_index, 1)) {
            break;
        }

        if(!flipper_format_insert_or_update_hex(
               file,
               SUBBRUTE_FIELD_REPEAT_VALUES,
               instance->repeat_values,
               SubBruteAttackTotalCount)) {
            break;
        }
        saved = true;
        break;
    } while(true);

    if(!saved) {
        FURI_LOG_E(TAG, "Error save file %s", SUBBRUTE_SETTINGS_PATH);
    }

    flipper_format_file_close(file);
    flipper_format_free(file);
    furi_record_close(RECORD_STORAGE);

    return saved;
}

void subbrute_settings_set_value(SubBruteSettings* instance, SubBruteAttacks index, uint8_t value) {
    furi_assert(instance);

    instance->repeat_values[index] = value;
}
uint8_t subbrute_settings_get_value(SubBruteSettings* instance, SubBruteAttacks index) {
    furi_assert(instance);

    return instance->repeat_values[index];
}

void subbrute_settings_set_repeats(SubBruteSettings* instance, const uint8_t* repeated_values) {
    furi_assert(instance);

    for(size_t i = 0; i < SubBruteAttackTotalCount; i++) {
        instance->repeat_values[i] = repeated_values[i];
    }
}

uint8_t subbrute_settings_get_current_repeats(SubBruteSettings* instance) {
    furi_assert(instance);

    return instance->repeat_values[instance->last_index];
}
