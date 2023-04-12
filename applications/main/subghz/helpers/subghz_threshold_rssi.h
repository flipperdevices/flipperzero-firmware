#pragma once
#include <furi.h>

typedef struct {
    float current_rssi; /** Current RSSI */
    bool threshold_level; /** Exceeded threshold level */
} SubGhzThresholdRssiRet;

typedef struct SubGhzThresholdRssi SubGhzThresholdRssi;

/**
 * Allocate SubGhzThresholdRssi
 * 
 * @return SubGhzThresholdRssi* 
 */
SubGhzThresholdRssi* subghz_threshold_rssi_alloc(void);

/**
 * Free SubGhzThresholdRssi
 * 
 * @param instance Pointer to a SubGhzThresholdRssi
 */
void subghz_threshold_rssi_free(SubGhzThresholdRssi* instance);

/**
 * Set threshold
 * 
 * @param instance Pointer to a SubGhzThresholdRssi
 * @param rssi RSSI threshold
 */
void subghz_threshold_rssi_set(SubGhzThresholdRssi* instance, float rssi);

/**
 * Get threshold
 * 
 * @param instance Pointer to a SubGhzThresholdRssi
 * @return float RSSI threshold
 */
float subghz_threshold_rssi_get(SubGhzThresholdRssi* instance);

/**
 * Check threshold
 * 
 * @param instance Pointer to a SubGhzThresholdRssi
 * @return SubGhzThresholdRssiRet 
 */
SubGhzThresholdRssiRet subghz_threshold_rssi_check_threshold(SubGhzThresholdRssi* instance);
