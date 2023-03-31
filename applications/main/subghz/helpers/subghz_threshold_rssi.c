#include "subghz_threshold_rssi.h"
#include <float_tools.h>
#include "../subghz_i.h"

#define TAG "SubGhzThresholdRssi"
#define THRESHOLD_RSSI_LOW_COUNT 10

struct SubGhzThresholdRssi {
    float threshold_rssi;
    uint8_t threshold_rssi_low_count;
};

SubGhzThresholdRssi* subghz_threshold_rssi_alloc(void) {
    SubGhzThresholdRssi* instance = malloc(sizeof(SubGhzThresholdRssi));
    instance->threshold_rssi = SUBGHZ_RAW_TRESHOLD_MIN;
    instance->threshold_rssi_low_count = THRESHOLD_RSSI_LOW_COUNT;
    return instance;
}

void subghz_threshold_rssi_free(SubGhzThresholdRssi* instance) {
    furi_assert(instance);
    free(instance);
}

void subghz_threshold_rssi_set(SubGhzThresholdRssi* instance, float rssi) {
    furi_assert(instance);
    instance->threshold_rssi = rssi;
}

float subghz_threshold_rssi_get(SubGhzThresholdRssi* instance) {
    furi_assert(instance);
    return instance->threshold_rssi;
}

SubGhzThresholdRssiRet subghz_threshold_rssi_check_threshold(SubGhzThresholdRssi* instance) {
    furi_assert(instance);
    float rssi = furi_hal_subghz_get_rssi();
    SubGhzThresholdRssiRet ret = {.current_rssi = rssi, .threshol_level = false};

    if(float_is_equal(instance->threshold_rssi, SUBGHZ_RAW_TRESHOLD_MIN)) {
        ret.threshol_level = true;
    } else {
        if(rssi < instance->threshold_rssi) {
            instance->threshold_rssi_low_count++;
            if(instance->threshold_rssi_low_count > THRESHOLD_RSSI_LOW_COUNT) {
                instance->threshold_rssi_low_count = THRESHOLD_RSSI_LOW_COUNT;
            }
            ret.threshol_level = false;
        } else {
            instance->threshold_rssi_low_count = 0;
        }

        if(instance->threshold_rssi_low_count == THRESHOLD_RSSI_LOW_COUNT) {
            ret.threshol_level = false;
        } else {
            ret.threshol_level = true;
        }
    }
    return ret;
}
