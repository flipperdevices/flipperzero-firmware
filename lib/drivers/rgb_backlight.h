/*
    RGB backlight FlipperZero driver
    Copyright (C) 2022-2023 Victor Nikitchuk (https://github.com/quen0n)
    Heavily modified by Willy-JL and Z3bro

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <furi.h>
#include "SK6805.h"
#include <toolbox/colors.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    RGBBacklightRainbowModeOff,
    RGBBacklightRainbowModeWave,
    RGBBacklightRainbowModeSolid,
    RGBBacklightRainbowModeCount,
} RGBBacklightRainbowMode;

/**
 * @brief Reconfigure rgb backlight with new settings
 *
 * @param enabled Whether the rgb backlight is enabled
 */
void rgb_backlight_reconfigure(bool enabled);

/**
 * @brief Load backlight settings from SD card
 */
void rgb_backlight_load_settings();

/**
 * @brief Save Current RGB Lighting Settings
 */
void rgb_backlight_save_settings();

/**
 * @brief Change the color of the backlight
 *
 * @param index What led to set the color to (0 - SK6805_LED_COUNT-1)
 * @param color RGB color to use
 */
void rgb_backlight_set_color(uint8_t index, RgbColor color);

RgbColor rgb_backlight_get_color(uint8_t index);

/**
 * @brief Change rainbow mode
 *
 * @param rainbow_mode What mode to use (0 - RGBBacklightRainbowModeCount)
 */
void rgb_backlight_set_rainbow_mode(RGBBacklightRainbowMode rainbow_mode);

RGBBacklightRainbowMode rgb_backlight_get_rainbow_mode();

/**
 * @brief Change rainbow speed
 *
 * @param rainbow_speed What speed to use (0 - 255)
 */
void rgb_backlight_set_rainbow_speed(uint8_t rainbow_speed);

uint8_t rgb_backlight_get_rainbow_speed();

/**
 * @brief Change rainbow interval
 *
 * @param rainbow_interval What interval to use
 */
void rgb_backlight_set_rainbow_interval(uint32_t rainbow_interval);

uint32_t rgb_backlight_get_rainbow_interval();

/**
 * @brief Change rainbow saturation
 *
 * @param rainbow_saturation What saturation to use (0 - 255)
 */
void rgb_backlight_set_rainbow_saturation(uint8_t rainbow_saturation);

uint8_t rgb_backlight_get_rainbow_saturation();

/**
 * @brief Apply current RGB lighting settings
 *
 * @param brightness Backlight intensity (0-255)
 * @param tick       Whether this update was a tick (for rainbow)
 */
void rgb_backlight_update(uint8_t brightness, bool tick);

#ifdef __cplusplus
}
#endif
