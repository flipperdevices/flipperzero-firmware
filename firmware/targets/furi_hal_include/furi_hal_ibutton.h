/**
 * @file furi_hal_ibutton.h
 * iButton HAL API
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "furi_hal_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*FuriHalIbuttonEmulateCallback)(void* context);

/** Initialize */
void furi_hal_ibutton_init();

void furi_hal_ibutton_emulate_start(
    uint32_t period,
    FuriHalIbuttonEmulateCallback callback,
    void* context);

void furi_hal_ibutton_emulate_set_next(uint32_t period);

void furi_hal_ibutton_emulate_stop();

/**
 * Sets the pin to normal mode (open collector), and sets it to float
 */
void furi_hal_ibutton_start_drive();

/**
 * Sets the pin to analog mode, and sets it to float
 */
void furi_hal_ibutton_stop();

/**
 * Sets the pin to low
 */
void furi_hal_ibutton_pin_low();

#ifdef __cplusplus
}
#endif
