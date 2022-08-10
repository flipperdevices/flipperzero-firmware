#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint32_t start;
    uint32_t end;
    int8_t power_limit;
    uint8_t duty_cycle;
} FuriHalRegionBand;

typedef struct {
    char country_code[2];
    uint16_t bands_count;
    FuriHalRegionBand bands[];
} FuriHalRegion;

void furi_hal_region_init();

void furi_hal_region_set(FuriHalRegion* region);

bool furi_hal_region_is_provisioned();

const char* furi_hal_region_get_name();

/** Сheck if transmission is allowed on this frequency for your flipper region
 *
 * @param      value  frequency in Hz
 *
 * @return     true if allowed
 */
bool furi_hal_region_is_frequency_allowed(uint32_t frequency);

FuriHalRegionBand* furi_hal_region_get_band(uint32_t frequency);
