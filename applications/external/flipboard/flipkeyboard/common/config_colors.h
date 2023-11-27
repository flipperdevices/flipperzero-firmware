#pragma once

/**
 * @file   config_colors.h
 * @brief  The configuration of the colors.
 * @details  The configuration of the LED colors. Feel free to add more colors, 
 * but be sure to add them to the color_names and color_values arrays.
*/

#include <furi.h>

/**
 * @brief    The names of the colors.
 * @details  The index of the color in this array is the same as
 *   the index of the color in the color_values array.
*/
static char* color_names[] = {
    "Off",
    "Red",
    "Orange",
    "Yellow",
    "Green",
    "Cyan",
    "Blue",
    "Violet",
    "Magenta",
    "White",
};

/**
 * @brief    The HEX value of red, green and blue LED brightness 
 *    (0xRRGGBB format). 00=off, FF=full brightness.
*/
enum LedColors {
    LedColorBlack = 0x000000,
    LedColorRed = 0xFF0000,
    LedColorOrange = 0xFF1F00,
    LedColorYellow = 0xFF7F00,
    LedColorGreen = 0x00FF00,
    LedColorCyan = 0x00FFFF,
    LedColorBlue = 0x0000FF,
    LedColorViolet = 0x1F00FF,
    LedColorMagenta = 0x7F00FF,
    LedColorWhite = 0xFFFFFF,
};

/**
 * @brief    The values of the colors.
 * @details  The index of the color in this array is the same as
 *   the index of the color in the color_names array.
*/
static uint32_t color_values[] = {
    LedColorBlack,
    LedColorRed,
    LedColorOrange,
    LedColorYellow,
    LedColorGreen,
    LedColorCyan,
    LedColorBlue,
    LedColorViolet,
    LedColorMagenta,
    LedColorWhite,
};