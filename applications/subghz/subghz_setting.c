#include "subghz_setting.h"
#include "subghz_i.h"

#include <furi.h>
#include <m-list.h>
#include <lib/flipper_format/flipper_format.h>

#define TAG "SubGhzSetting"

#define SUBGHZ_SETTING_FILE_VERSION 1
#define SUBGHZ_SETTING_FILE_TYPE "Flipper SubGhz Setting File"

typedef enum {
    SubGhzSettingStateNoLoad = 0,
    SubGhzSettingStateLoadFrequencyDefault,
    SubGhzSettingStateOkLoad,
} SubGhzSettingState;

LIST_DEF(frequencies_list, uint32_t)
LIST_DEF(hopper_frequencies_list, uint32_t)

struct SubGhzSetting {
    frequencies_list_t frequencies;
    hopper_frequencies_list_t hopper_frequencies;
    size_t frequencies_count;
    size_t hopper_frequencies_count;
    uint32_t frequency_default_index;
};

SubGhzSetting* subghz_setting_alloc(void) {
    SubGhzSetting* instance = malloc(sizeof(SubGhzSetting));
    frequencies_list_init(instance->frequencies);
    hopper_frequencies_list_init(instance->hopper_frequencies);
    return instance;
}

void subghz_setting_free(SubGhzSetting* instance) {
    furi_assert(instance);
    frequencies_list_clear(instance->frequencies);
    hopper_frequencies_list_clear(instance->hopper_frequencies);
    free(instance);
}

void subghz_setting_load(SubGhzSetting* instance, const char* file_path) {
    furi_assert(instance);

    frequencies_list_clear(instance->frequencies);
    hopper_frequencies_list_clear(instance->hopper_frequencies);

    Storage* storage = furi_record_open("storage");
    FlipperFormat* fff_data_file = flipper_format_file_alloc(storage);

    string_t temp_str;
    string_init(temp_str);
    uint32_t temp_data32;
    SubGhzSettingState loading = SubGhzSettingStateNoLoad;
    uint16_t i = 0;

    if(file_path) {
        do {
            if(!flipper_format_file_open_existing(fff_data_file, file_path)) {
                FURI_LOG_E(TAG, "Error open file %s", file_path);
                break;
            }

            if(!flipper_format_read_header(fff_data_file, temp_str, &temp_data32)) {
                FURI_LOG_E(TAG, "Missing or incorrect header");
                break;
            }

            if((!strcmp(string_get_cstr(temp_str), SUBGHZ_SETTING_FILE_TYPE)) &&
               temp_data32 == SUBGHZ_SETTING_FILE_VERSION) {
            } else {
                FURI_LOG_E(TAG, "Type or version mismatch");
                break;
            }

            if(!flipper_format_rewind(fff_data_file)) {
                FURI_LOG_E(TAG, "Rewind error");
                break;
            }
            i = 0;
            while(flipper_format_read_uint32(
                fff_data_file, "Frequency", (uint32_t*)&temp_data32, 1)) {
                if(furi_hal_subghz_is_frequency_valid(temp_data32)) {
                    FURI_LOG_I(TAG, "Frequency loaded %lu", temp_data32);
                    frequencies_list_push_back(instance->frequencies, temp_data32);
                    i++;
                } else {
                    FURI_LOG_E(TAG, "Frequency not supported %lu", temp_data32);
                }
            }
            instance->frequencies_count = i;

            if(!flipper_format_rewind(fff_data_file)) {
                FURI_LOG_E(TAG, "Rewind error");
                break;
            }
            i = 0;
            while(flipper_format_read_uint32(
                fff_data_file, "Hopper_frequency", (uint32_t*)&temp_data32, 1)) {
                if(furi_hal_subghz_is_frequency_valid(temp_data32)) {
                    FURI_LOG_I(TAG, "Hopper frequency loaded %lu", temp_data32);
                    hopper_frequencies_list_push_back(instance->hopper_frequencies, temp_data32);
                    i++;
                } else {
                    FURI_LOG_E(TAG, "Hopper frequency not supported %lu", temp_data32);
                }
            }
            instance->hopper_frequencies_count = i;

            if(!flipper_format_rewind(fff_data_file)) {
                FURI_LOG_E(TAG, "Rewind error");
                break;
            }
            if(!flipper_format_read_uint32(
                   fff_data_file, "Frequency_default", (uint32_t*)&temp_data32, 1)) {
                FURI_LOG_E(TAG, "Frequency default missing");
                break;
            }

            for(i = 0; i < instance->frequencies_count; i++) {
                if(subghz_setting_get_frequency(instance, i) == temp_data32) {
                    instance->frequency_default_index = i;
                    FURI_LOG_I(TAG, "Frequency default index %lu", i);
                    loading = SubGhzSettingStateLoadFrequencyDefault;
                    break;
                }
            }

            if(loading == SubGhzSettingStateLoadFrequencyDefault) {
                loading = SubGhzSettingStateOkLoad;
            } else {
                FURI_LOG_E(TAG, "Frequency default index missing");
            }
        } while(false);
    }

    if(loading != SubGhzSettingStateOkLoad) {
        frequencies_list_clear(instance->frequencies);
        hopper_frequencies_list_clear(instance->hopper_frequencies);
        for(i = 0; i < subghz_frequencies_count; i++) {
            frequencies_list_push_back(instance->frequencies, subghz_frequencies[i]);
        }
        instance->frequencies_count = i;

        for(i = 0; i < subghz_hopper_frequencies_count; i++) {
            hopper_frequencies_list_push_back(
                instance->hopper_frequencies, subghz_hopper_frequencies[i]);
        }
        instance->hopper_frequencies_count = i;
        instance->frequency_default_index = subghz_frequencies_433_92;
    }
}

size_t subghz_setting_get_frequency_count(SubGhzSetting* instance) {
    return instance->frequencies_count;
}

size_t subghz_setting_get_hopper_frequency_count(SubGhzSetting* instance) {
    return instance->hopper_frequencies_count;
}

uint32_t subghz_setting_get_frequency(SubGhzSetting* instance, size_t idx) {
    return *frequencies_list_get(instance->frequencies, idx);
}

uint32_t subghz_setting_get_hopper_frequency(SubGhzSetting* instance, size_t idx) {
    return *hopper_frequencies_list_get(instance->hopper_frequencies, idx);
}

uint32_t subghz_setting_get_frequency_default_index(SubGhzSetting* instance) {
    return instance->frequency_default_index;
}