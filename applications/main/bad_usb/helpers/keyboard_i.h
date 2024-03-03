#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#include <storage/storage.h>
#include "keyboard.h"

struct BadUsbKeyboardVTable {
    /**
     * @brief Gets the key associated the codepoint.
     *
     * This gets the key that needs to be pressed to enter the character
     * associated with codepoint. The key is represented by a 2 byte value with
     * the USB HID Keyboard/Keypad Page Usage ID in the low byte and the
     * modifier keys in the high byte. It is suitable for passing to
     * furi_hal_hid_kb_press.
     *
     * This function is required to be implemented.
     *
     * @returns The keycode if the codepoint can meaningfully be entered by
     * pressing a single key. Otherwise returns HID_KEYBOARD_NONE.
     */
    uint16_t (*get_key)(void* data, FuriStringUnicodeValue codepoint);

    /**
     * @brief Gets the sequence of keys associated with the codepoint.
     *
     * This gets the key sequence that needs to be pressed to enter the
     * character associated with the codepoint. For example, to enter the
     * codepoint U+00E9 “é” LATIN SMALL LETTER E WITH ACUTE on the
     * US-International keyboard layout, one first needs to type
     * HID_KEYBOARD_APOSTROPHE (0x34), followed by HID_KEYBOARD_E (0x08).
     *
     * This function is optional. It may be set to NULL, in which case get_key
     * will be used instead.
     *
     * @returns A pointer to a sequence of keycodes, terminated by
     * HID_KEYBOARD_NONE. NULL may be returned if no sequence could be found.
     */
    uint16_t* (*get_key_sequence)(void* data, FuriStringUnicodeValue codepoint);

    /**
     * @brief Called when the data structure needs to be freed.
     */
    void (*free)(void* data);
};

struct BadUsbKeyboard {
    const struct BadUsbKeyboardVTable* vtable;
    void* data;
};

BadUsbKeyboard* bad_usb_keyboard_alloc(void* data, const struct BadUsbKeyboardVTable* vtable);

BadUsbKeyboard* bad_usb_keyboard_layout_default_alloc();
BadUsbKeyboard* bad_usb_keyboard_v1_alloc_read(File* layout);
BadUsbKeyboard* bad_usb_keyboard_v2_alloc_read(File* layout);

struct SharedHeader {
    uint8_t magic[3];
    uint8_t version;
} FURI_PACKED;

#ifdef __cplusplus
}
#endif
