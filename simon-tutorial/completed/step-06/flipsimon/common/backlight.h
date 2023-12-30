/**
 * @file backlight.h
 * @brief This file contains the backlight module.
 * @details This file contains the backlight module.  The backlight module is
 * responsible for controlling the backlight.  You can turn the backlight on,
 * or turn it back off (unless user interaction).
*/

#pragma once

typedef struct Backlight Backlight;

/**
 * @brief      Allocates a new Backlight object.
 * @details    Creates a new Backlight object.  The Backlight object is responsible for
 *            controlling the backlight.  You can turn the backlight on, or turn it back
 *            off (unless user interaction).
 * @return     Pointer to Backlight object.
*/
Backlight* backlight_alloc();

/**
 * @brief      Frees a Backlight object.
 * @details    Frees a Backlight object.
 * @param      backlight  Pointer to Backlight object.
*/
void backlight_free(Backlight* backlight);

/**
 * @brief    Turns on backlight, even if no user interaction.
 * @param    backlight  Pointer to Backlight object.
*/
void backlight_on(Backlight* backlight);

/**
 * @brief    Turns off backlight, unless there is user interaction.
 * @param    backlight  Pointer to Backlight object.
*/
void backlight_off(Backlight* backlight);
