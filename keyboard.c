#include "keyboard.h"

// #define DISABLE_KEYBOARD true

FuriHalUsbInterface* usb_if_prev;

void* flipboard_keyboard_alloc() {
#ifndef DISABLE_KEYBOARD
    usb_if_prev = furi_hal_usb_get_config();
    furi_check(furi_hal_usb_set_config(&usb_hid, NULL));
#endif
    return NULL;
}

void flipboard_keyboard_free(void* p) {
    UNUSED(p);
#ifndef DISABLE_KEYBOARD
    furi_check(furi_hal_usb_set_config(usb_if_prev, NULL));
#endif
}

void flipboard_keyboard_send_char(void* keyboard, char ch) {
    UNUSED(keyboard);
    UNUSED(ch);
#ifndef DISABLE_KEYBOARD
    uint16_t keycode = HID_ASCII_TO_KEY(ch);
    furi_hal_hid_kb_press(keycode);
    furi_delay_ms(1);
    furi_hal_hid_kb_release(keycode);
    furi_delay_ms(1);
#endif
}

void flipboard_keyboard_send_text(void* keyboard, char* message) {
    UNUSED(keyboard);
    UNUSED(message);
    for(size_t i = 0; i < strlen(message); i++) {
        flipboard_keyboard_send_char(keyboard, message[i]);
    }
}

void flipboard_keyboard_send_keycode(void* keyboard, uint16_t code) {
    UNUSED(keyboard);
    furi_delay_ms(1);
#ifndef DISABLE_KEYBOARD
    furi_hal_hid_kb_press(code);
#else
    UNUSED(code);
#endif
}

void flipboard_keyboard_release_all(void* keyboard) {
    UNUSED(keyboard);
#ifndef DISABLE_KEYBOARD
    furi_hal_hid_kb_release_all();
#endif
    furi_delay_ms(1);
}

void flipboard_keyboard_send_keycodes(void* keyboard, uint16_t* codes, size_t nb_codes) {
    for(size_t i = 0; i < nb_codes; i++) {
        flipboard_keyboard_send_keycode(keyboard, codes[i]);
    }
    flipboard_keyboard_release_all(keyboard);
}
