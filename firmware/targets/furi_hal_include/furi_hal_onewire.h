/**
 * @file furi_hal_onewire.h
 * OneWire HAL API
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "furi_hal_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Sets the pin to normal mode (open collector), and sets it to float
 * @param gpio
 */
void furi_hal_onewire_start_drive(const GpioPin* gpio);

/**
 * Sets the pin to normal mode (open collector), and clears pin EXTI interrupt.
 * Used in EXTI interrupt context.
 * @param gpio
 */
void furi_hal_onewire_start_drive_in_isr(const GpioPin* gpio);

/**
 * Sets the pin to interrupt mode (EXTI interrupt on rise or fall), and sets it to float
 * @param gpio
 */
void furi_hal_onewire_start_interrupt(const GpioPin* gpio);

/**
 * Sets the pin to interrupt mode (EXTI interrupt on rise or fall), and clears pin EXTI interrupt.
 * Used in EXTI interrupt context.
 * @param gpio
 */
void furi_hal_onewire_start_interrupt_in_isr(const GpioPin* gpio);

/**
 * Sets the pin to analog mode, and sets it to float
 * @param gpio
 */
void furi_hal_onewire_stop(const GpioPin* gpio);

/**
 * Attach interrupt callback to OneWire pin
 * @param cb callback
 * @param context context
 */
void furi_hal_onewire_add_interrupt(const GpioPin* gpio, GpioExtiCallback cb, void* context);

/**
 * Remove interrupt callback from OneWire pin
 */
void furi_hal_onewire_remove_interrupt(const GpioPin* gpio);

/**
 * Sets the pin to low
 */
void furi_hal_onewire_pin_low(const GpioPin* gpio);

/**
 * Sets the pin to high (float in OneWire pin modes)
 */
void furi_hal_onewire_pin_high(const GpioPin* gpio);

/**
 * Get pin level
 * @return true if level is high
 * @return false if level is low
 */
bool furi_hal_onewire_pin_get_level(const GpioPin* gpio);

#ifdef __cplusplus
}
#endif
