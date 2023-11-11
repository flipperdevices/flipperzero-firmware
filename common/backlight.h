#pragma once

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