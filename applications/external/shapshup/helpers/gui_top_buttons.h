#pragma once

#include <input/input.h>
#include <gui/elements.h>
#include <gui/icon.h>
#include <gui/icon_animation.h>
#include <shapshup_icons.h>

/**
 * @brief draw button in top left corner of screen
 *
 * @author panki27 (https://github.com/panki27/Metronome)
 * @param canvas Canvas* 
 * @param str caption of button
 */
void elements_button_top_left(Canvas* canvas, const char* str);

/**
 * @brief draw button in top right corner of screen
 * 
 * @author panki27 (https://github.com/panki27/Metronome)
 * @param canvas Canvas*
 * @param str caption of button
 */
void elements_button_top_right(Canvas* canvas, const char* str);
