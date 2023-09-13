#include "keyboard_i.h"

#define PRESS_DELAY_MS 1
#define RELEASE_DELAY_MS 1

FlipboardKeyboard* flipboard_keyboard_alloc() {
    FlipboardKeyboard* keyboard = malloc(sizeof(FlipboardKeyboard));
    keyboard->usb_previous = NULL;
    keyboard->attached = false;
    return keyboard;
}

void flipboard_keyboard_attach(FlipboardKeyboard* keyboard) {
    keyboard->usb_previous = furi_hal_usb_get_config();
    keyboard->attached = furi_hal_usb_set_config(&usb_hid, NULL);
}

void flipboard_keyboard_detatch(FlipboardKeyboard* keyboard) {
    if(keyboard->attached) {
        furi_hal_usb_set_config(keyboard->usb_previous, NULL);
        keyboard->usb_previous = NULL;
        keyboard->attached = false;
    }
}

void flipboard_keyboard_free(FlipboardKeyboard* keyboard) {
    flipboard_keyboard_detatch(keyboard);
    free(keyboard);
}

void flipboard_keyboard_send_char(FlipboardKeyboard* keyboard, char ch) {
    if(keyboard->attached) {
        uint16_t keycode = HID_ASCII_TO_KEY(ch);
        furi_hal_hid_kb_press(keycode);
        furi_delay_ms(PRESS_DELAY_MS);
        furi_hal_hid_kb_release(keycode);
        furi_delay_ms(RELEASE_DELAY_MS);
    }
}

void flipboard_keyboard_send_text(FlipboardKeyboard* keyboard, char* message) {
    if(message != NULL) {
        for(size_t i = 0; i < strlen(message); i++) {
            flipboard_keyboard_send_char(keyboard, message[i]);
        }
    }
}

void flipboard_keyboard_send_keycode(FlipboardKeyboard* keyboard, uint16_t code) {
    if(keyboard->attached) {
        furi_delay_ms(PRESS_DELAY_MS);
        furi_hal_hid_kb_press(code);
    }
}

void flipboard_keyboard_release_all(FlipboardKeyboard* keyboard) {
    if(keyboard->attached) {
        furi_delay_ms(RELEASE_DELAY_MS);
        furi_hal_hid_kb_release_all();
    }
}

void flipboard_keyboard_send_keycodes(
    FlipboardKeyboard* keyboard,
    uint16_t* codes,
    size_t nb_codes) {
    for(size_t i = 0; i < nb_codes; i++) {
        flipboard_keyboard_send_keycode(keyboard, codes[i]);
    }

    flipboard_keyboard_release_all(keyboard);
}
