#ifndef _TOTP_UI_CONTROLS_H_
#define _TOTP_UI_CONTROLS_H_

#include <inttypes.h>
#include <gui/gui.h>

void ui_control_text_box_render(Canvas* const canvas, uint8_t y, char* text, bool is_selected);
void ui_control_button_render(Canvas* const canvas, uint8_t x, uint8_t y, uint8_t width, uint8_t height, char* text, bool is_selected);

#endif
