/**
 * @file furi_hal_power.h
 * Power HAL API
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/** Enable 3.3v on external gpio and sd card
 */
void furi_hal_power_enable_external_3_3v();

/** Disable 3.3v on external gpio and sd card
 */
void furi_hal_power_disable_external_3_3v();

#ifdef __cplusplus
}
#endif
