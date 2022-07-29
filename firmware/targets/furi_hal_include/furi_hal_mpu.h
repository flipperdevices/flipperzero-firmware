/**
 * @file furi_hal_light.h
 * Light control HAL API
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    FuriHalRegionNULL = 0x00, // region 0 used to protect null pointer dereference
    FuriHalRegionStack = 0x01, // region 1 used to protect stack
    FuriHalRegion2 = 0x02,
    FuriHalRegion3 = 0x03,
    FuriHalRegion4 = 0x04,
    FuriHalRegion5 = 0x05,
    FuriHalRegion6 = 0x06,
    FuriHalRegion7 = 0x07,
} FuriHalRegion;

typedef enum {
    FuriHalRegionSize32B = 0x04U,
    FuriHalRegionSize64B = 0x05U,
    FuriHalRegionSize128B = 0x06U,
    FuriHalRegionSize256B = 0x07U,
    FuriHalRegionSize512B = 0x08U,
    FuriHalRegionSize1KB = 0x09U,
    FuriHalRegionSize2KB = 0x0AU,
    FuriHalRegionSize4KB = 0x0BU,
    FuriHalRegionSize8KB = 0x0CU,
    FuriHalRegionSize16KB = 0x0DU,
    FuriHalRegionSize32KB = 0x0EU,
    FuriHalRegionSize64KB = 0x0FU,
    FuriHalRegionSize128KB = 0x10U,
    FuriHalRegionSize256KB = 0x11U,
    FuriHalRegionSize512KB = 0x12U,
    FuriHalRegionSize1MB = 0x13U,
    FuriHalRegionSize2MB = 0x14U,
    FuriHalRegionSize4MB = 0x15U,
    FuriHalRegionSize8MB = 0x16U,
    FuriHalRegionSize16MB = 0x17U,
    FuriHalRegionSize32MB = 0x18U,
    FuriHalRegionSize64MB = 0x19U,
    FuriHalRegionSize128MB = 0x1AU,
    FuriHalRegionSize256MB = 0x1BU,
    FuriHalRegionSize512MB = 0x1CU,
    FuriHalRegionSize1GB = 0x1DU,
    FuriHalRegionSize2GB = 0x1EU,
    FuriHalRegionSize4GB = 0x1FU,
} FuriHalRegionSize;

/**
 * @brief Initialize memory protection unit
 */
void furi_hal_mpu_init();

/**
* @brief Enable memory protection unit
*/
void furi_hal_mpu_enable();

/**
* @brief Disable memory protection unit
*/
void furi_hal_mpu_disable();

void furi_hal_mpu_protect_no_access(FuriHalRegion region, uint32_t address, FuriHalRegionSize size);

void furi_hal_mpu_protect_read_only(FuriHalRegion region, uint32_t address, FuriHalRegionSize size);

void furi_hal_mpu_protect_disable(FuriHalRegion region);