/**
 * @file furi_hal_boot.h
 * Bootloader HAL API
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Boot modes */
typedef enum { FuriHalBootModeNormal, FuriHalBootModeDFU } FuriHalBootMode;

/** Initialize boot subsystem
 */
void furi_hal_boot_init();

/** Set bootloader mode
 *
 * @param[in]  mode  FuriHalBootMode
 */
void furi_hal_boot_set_mode(FuriHalBootMode mode);

/** Get bootloader mode
 *
 * @return[out]  mode  FuriHalBootMode
 */
FuriHalBootMode furi_hal_boot_get_mode();

#ifdef __cplusplus
}
#endif
