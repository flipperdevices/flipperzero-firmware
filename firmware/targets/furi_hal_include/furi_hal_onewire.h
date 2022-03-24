/**
 * @file furi_hal_onewire.h
 * Onewire HAL API
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*FuriHalOnewireEmulateCallback)(void* context);

/** Initialize */
void furi_hal_onewire_init();

void furi_hal_onewire_emulate_start(
    uint32_t period,
    FuriHalOnewireEmulateCallback callback,
    void* context);

void furi_hal_onewire_emulate_set_next(uint32_t period);

void furi_hal_onewire_emulate_stop();

#ifdef __cplusplus
}
#endif
