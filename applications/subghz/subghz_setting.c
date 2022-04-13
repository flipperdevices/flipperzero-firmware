#include "subghz_setting.h"
#include "subghz_i.h"

#include <furi.h>
#include <m-list.h>

#define TAG "SubGhzSetting"

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

void subghz_setting_load(SubGhzSetting* instance) {
    frequencies_list_clear(instance->frequencies);
    hopper_frequencies_list_clear(instance->hopper_frequencies);

    uint16_t i = 0;
    for(i = 0; i < subghz_frequencies_count; i++) {
        frequencies_list_push_back(instance->frequencies, subghz_frequencies[i]);
    }
    instance->frequencies_count = i;

    for(i = 0; i <  subghz_hopper_frequencies_count; i++) {
        hopper_frequencies_list_push_back(
            instance->hopper_frequencies, subghz_hopper_frequencies[i]);
    }
    instance->hopper_frequencies_count = i;
    instance->frequency_default_index = subghz_frequencies_433_92;
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