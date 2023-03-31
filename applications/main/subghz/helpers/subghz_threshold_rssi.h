#pragma once
#include <furi.h>

typedef struct {
    float current_rssi;
    bool threshol_level;
} SubGhzThresholdRssiRet;

typedef struct SubGhzThresholdRssi SubGhzThresholdRssi;

SubGhzThresholdRssi* subghz_threshold_rssi_alloc(void);
void subghz_threshold_rssi_free(SubGhzThresholdRssi* instance);
void subghz_threshold_rssi_set(SubGhzThresholdRssi* instance, float rssi);
float subghz_threshold_rssi_get(SubGhzThresholdRssi* instance);
SubGhzThresholdRssiRet subghz_threshold_rssi_check_threshold(SubGhzThresholdRssi* instance);
