#include <furi_hal.h>
#include <stdint.h>
#include <storage/storage.h>
#include "keyboard_i.h"

static uint16_t get_key(void*, FuriStringUnicodeValue codepoint) {
    if(codepoint >= sizeof(hid_asciimap) / sizeof(*hid_asciimap)) {
        return HID_KEYBOARD_NONE;
    }

    return hid_asciimap[codepoint];
}

static void free_data(void*) {
    // No-op.
}

static const struct BadUsbKeyboardVTable vtable = {
    .get_key = get_key,
    .free = free_data,
};

BadUsbKeyboard* bad_usb_keyboard_alloc_default() {
    return bad_usb_keyboard_alloc(NULL, &vtable);
}
