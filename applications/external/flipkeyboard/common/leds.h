#pragma once

/**
 * @file leds.h
 * @brief This file contains methods to control the LEDs on the flipboard.
 * @details The leds module is used to control the addressable LEDs on the flipboard,
 * and also the status LED.
*/

#include <furi.h>
#include "resources.h"

typedef struct FlipboardLeds FlipboardLeds;

typedef enum {
    LedId1 = 1,
    LedId2 = 1 << 1,
    LedId3 = 1 << 2,
    LedId4 = 1 << 3,
} LedIds;

/**
 * @brief Allocates a FlipboardLeds struct.
 * @details This method allocates a FlipboardLeds struct.  This is used to
 * control the status LED and the addressable LEDs on the flipboard.
 * @param resources The resources struct to use for hardware access.
 * @return The allocated FlipboardLeds struct.
*/
FlipboardLeds* flipboard_leds_alloc(Resources* resources);

/**
 * @brief Frees a FlipboardLeds struct.
 * @param leds The FlipboardLeds struct to free.
*/
void flipboard_leds_free(FlipboardLeds* leds);

/**
 * @brief Resets the LEDs to their default color pattern (off).
 * @details This method resets the LEDs data to their default color pattern (off).
 * You must still call flipboard_leds_update to update the LEDs.
 * @param leds The FlipboardLeds struct to reset.
*/
void flipboard_leds_reset(FlipboardLeds* leds);

/**
 * @brief Sets the color of the LEDs.
 * @details This method sets the color of the LEDs.
 * @param leds The FlipboardLeds struct to set the color of.
 * @param led The LED to set the color of.
 * @param color The color to set the LED to (Hex value: RRGGBB).
*/
void flipboard_leds_set(FlipboardLeds* leds, LedIds led, uint32_t color);

/**
 * @brief Updates the LEDs.
 * @details This method changes the LEDs to the colors set by flipboard_leds_set.
 * @param leds The FlipboardLeds struct to update.
*/
void flipboard_leds_update(FlipboardLeds* leds);

/**
 * @brief Sets the status LED.
 * @details This method sets the status LED to the specified state.
 * @param leds The FlipboardLeds struct to set the status LED of.
 * @param glow True to turn the status LED on, false to turn it off.
*/
void flipboard_status_led(FlipboardLeds* leds, bool glow);