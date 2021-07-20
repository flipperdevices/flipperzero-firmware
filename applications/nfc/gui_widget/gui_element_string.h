#pragma once
#include <stdint.h>
#include <gui/canvas.h>

typedef struct GuiString GuiString;
typedef struct GuiElement GuiElement;

/** Allocate GuiString element
 * @param x - x coordinate
 * @param y - y coordinate
 * @param horizontal - Align instance
 * @param vertical - Align instance
 * @param font Font instance
 * @return GuiString instance
 */
GuiString* gui_string_alloc(
    uint8_t x,
    uint8_t y,
    Align horizontal,
    Align vertical,
    Font font,
    const char* text);

/** Free GuiString element
 * @param gui_string GuiString instance
 */
void gui_string_free(GuiString* gui_string);

/** Get generic Element pointer from GuiString
 * @param gui_string GuiString instance
 * @return GuiElement instance
 */
GuiElement* gui_string_get_element(GuiString* gui_string);
