#pragma once

#include <furi_hal.h>

typedef struct RgbLeds RgbLeds;

/**
 * @brief Allocates a RgbLeds struct.
 * @details This method allocates a RgbLeds struct.  This is used to
 * control the addressable LEDs.
 * @param num_leds The number of LEDs to allocate.
 * @param leds_pin The GPIO pin to use for the LEDs. (&gpio_ext_pc3)
 * @return The allocated RgbLeds struct.
*/
RgbLeds* rgbleds_alloc(uint16_t num_leds, const GpioPin* const leds_pin);

/**
 * @brief Frees a RgbLeds struct.
 * @param leds The RgbLeds struct to free.
*/
void rgbleds_free(RgbLeds* leds);

/**
 * @brief Resets the LEDs to their default color pattern (off).
 * @details This method resets the LEDs data to their default color pattern (off).
 * You must still call rgbleds_update to update the LEDs.
 * @param leds The RgbLeds struct to reset.
*/
void rgbleds_reset(RgbLeds* leds);

/**
 * @brief Sets the color of the LEDs.
 * @details This method sets the color of the LEDs.
 * @param leds The RgbLeds struct to set the color of.
 * @param led The LED index to set the color of.
 * @param color The color to set the LED to (Hex value: RRGGBB).
 * @return True if the LED was set, false if the LED was out of range.
*/
bool rgbleds_set(RgbLeds* leds, uint16_t led, uint32_t color);

/**
 * @brief Gets the color of the LEDs.
 * @details This method gets the color of the LEDs.
 * @param leds The RgbLeds struct to get the color of.
 * @param led The LED index to get the color of.
 * @return The color of the LED (Hex value: RRGGBB).
*/
uint32_t rgbleds_get(RgbLeds* leds, uint16_t led);

/**
 * @brief Sets the brightness of the LEDs.
 * @details This method sets the brightness of the LEDs.
 * @param leds The RgbLeds struct to set the brightness of.
 * @param brightness The brightness to set the LEDs to (0-255).
*/
void rgbleds_set_brightness(RgbLeds* leds, uint8_t brightness);

/**
 * @brief Updates the LEDs.
 * @details This method changes the LEDs to the colors set by rgbleds_set.
 * @param leds The RgbLeds struct to update.
*/
void rgbleds_update(RgbLeds* leds);