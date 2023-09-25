#pragma once

#include <furi.h>
#include <furi_hal.h>

typedef struct FlipboardKeyboard FlipboardKeyboard;

FlipboardKeyboard* flipboard_keyboard_alloc();
void flipboard_keyboard_attach(FlipboardKeyboard* keyboard);
void flipboard_keyboard_detatch(FlipboardKeyboard* keyboard);
void flipboard_keyboard_send_char(FlipboardKeyboard* keyboard, char ch);
void flipboard_keyboard_send_text(FlipboardKeyboard* keyboard, const char* message);
void flipboard_keyboard_send_keycode(FlipboardKeyboard* keyboard, uint16_t code);
void flipboard_keyboard_release_all(FlipboardKeyboard* keyboard);
void flipboard_keyboard_send_keycodes(
    FlipboardKeyboard* keyboard,
    uint16_t* codes,
    size_t nb_codes);
void flipboard_keyboard_free(FlipboardKeyboard* keyboard);