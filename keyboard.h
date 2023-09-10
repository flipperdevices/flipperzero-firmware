#pragma once

#include <furi.h>
#include <furi_hal.h>

void* flipboard_keyboard_alloc();
void flipboard_keyboard_send_char(void* keyboard, char ch);
void flipboard_keyboard_send_text(void* keyboard, char* message);
void flipboard_keyboard_send_keycode(void* keyboard, uint16_t code);
void flipboard_keyboard_release_all(void* keyboard);
void flipboard_keyboard_send_keycodes(void* keyboard, uint16_t* codes, size_t nb_codes);
void flipboard_keyboard_free(void* keyboard);