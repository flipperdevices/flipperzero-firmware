#include <furi_hal.h>
#include <storage/storage.h>
#include "keyboard.h"
#include "keyboard_i.h"

struct Layout {
    uint16_t keycodes[128];
};

static uint16_t get_key(void* data, FuriStringUnicodeValue codepoint) {
    struct Layout* layout = data;

    if(codepoint >= sizeof(layout->keycodes) / sizeof(*layout->keycodes)) {
        return HID_KEYBOARD_NONE;
    }

    return layout->keycodes[codepoint];
}

static const struct BadUsbKeyboardVTable imp_v1 = {
    .get_key = get_key,
    .free = free,
};

BadUsbKeyboard* bad_usb_keyboard_v1_alloc_read(File* layout) {
    struct Layout* data = malloc(sizeof(*data));
    if(storage_file_read(layout, data, sizeof(*data)) != sizeof(*data)) {
        free(data);
        return NULL;
    }

    return bad_usb_keyboard_alloc(data, &imp_v1);
}
