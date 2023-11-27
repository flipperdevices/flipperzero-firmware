#pragma once

/**
 * @file backlight.h
 * @brief This file contains the backlight module.
 * @details This file contains the backlight module.  The backlight module is
 * responsible for controlling the backlight.  You can turn the backlight on,
 * off, or force it off.
*/

/**
 * @brief    Turns on backlight, even if no user interaction.
*/
void backlight_on();

/**
 * @brief    Turns off backlight, unless there is user interaction.
*/
void backlight_off();

/**
 * @brief    Turns off backlight, even if user interaction.
*/
void backlight_force_off();