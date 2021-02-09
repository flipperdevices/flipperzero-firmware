#pragma once
#include <stdint.h>

typedef struct GuiString GuiString;
typedef struct GuiElement GuiElement;

// allocate string element
GuiString* gui_string_alloc(uint8_t x, uint8_t y, const char* text);

// free string element
void gui_string_free(GuiString* gui_string);

// get generic element pointer from string
GuiElement* gui_string_get_element(GuiString* gui_string);

// gtring element setters
void gui_string_set_x(GuiString* gui_string, uint8_t x);
void gui_string_set_y(GuiString* gui_string, uint8_t y);
void gui_string_set_text(GuiString* gui_string, const char* text);