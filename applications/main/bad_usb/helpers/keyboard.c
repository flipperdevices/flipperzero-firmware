#include "helpers/bad_usb_hid.h"
#include <furi_hal.h>
#include <stdint.h>
#include <storage/storage.h>
#include "keyboard.h"
#include "keyboard_i.h"

const uint8_t header_magic[3] = {0xFF, 'K', 'L'};

static int determine_version(File* layout_file) {
    struct SharedHeader shared_header = {0};
    storage_file_read(layout_file, &shared_header, sizeof(shared_header));
    storage_file_seek(layout_file, 0, true);

    if(memcmp(
           shared_header.magic,
           header_magic,
           MIN(sizeof(shared_header.magic), sizeof(header_magic))) != 0) {
        // Backwards compat with v1
        return 1;
    }
    return shared_header.version;
}

BadUsbKeyboard* bad_usb_keyboard_alloc_read(File* layout_file) {
    switch(determine_version(layout_file)) {
    case 1:
        return bad_usb_keyboard_v1_alloc_read(layout_file);
    case 2:
        return bad_usb_keyboard_v2_alloc_read(layout_file);
    default:
        return NULL;
    }
}

BadUsbKeyboard* bad_usb_keyboard_alloc(void* data, const struct BadUsbKeyboardVTable* vtable) {
    // vtable->get_key_sequence is optional
    furi_assert(vtable);
    furi_assert(vtable->get_key);
    furi_assert(vtable->free);

    BadUsbKeyboard* kb = malloc(sizeof(BadUsbKeyboard));
    kb->data = data;
    kb->vtable = vtable;
    return kb;
}

void bad_usb_keyboard_free(BadUsbKeyboard* keyboard) {
    furi_assert(keyboard);

    keyboard->vtable->free(keyboard->data);
    free(keyboard);
}

void bad_usb_keyboard_type_codepoint(
    BadUsbKeyboard* keyboard,
    const BadUsbHidApi* hid,
    void* hid_inst,
    FuriStringUnicodeValue codepoint) {
    furi_assert(keyboard);

    if(keyboard->vtable->get_key_sequence) {
        uint16_t* key_sequence = keyboard->vtable->get_key_sequence(keyboard->data, codepoint);

        if(key_sequence == NULL) {
            return;
        }

        for(size_t i = 0; key_sequence[i] != HID_KEYBOARD_NONE; i++) {
            hid->kb_press(hid_inst, key_sequence[i]);
            hid->kb_release(hid_inst, key_sequence[i]);
        }
    } else {
        uint16_t key = keyboard->vtable->get_key(keyboard->data, codepoint);
        if(key != HID_KEYBOARD_NONE) {
            furi_hal_hid_kb_press(key);
            furi_hal_hid_kb_release(key);
        }
    }
}

void bad_usb_keyboard_type_string(
    BadUsbKeyboard* keyboard,
    const BadUsbHidApi* hid,
    void* hid_inst,
    const char* string) {
    furi_assert(keyboard);

    FuriStringUTF8State utf8_state = FuriStringUTF8StateStarting;
    FuriStringUnicodeValue codepoint;

    for(size_t i = 0; string[i] != '\0'; i++) {
        furi_string_utf8_decode(string[i], &utf8_state, &codepoint);
        if(utf8_state != FuriStringUTF8StateStarting) {
            continue;
        }

        bad_usb_keyboard_type_codepoint(keyboard, hid, hid_inst, codepoint);
    }
}

uint16_t bad_usb_keyboard_get_key(BadUsbKeyboard* keyboard, FuriStringUnicodeValue codepoint) {
    furi_assert(keyboard);

    return keyboard->vtable->get_key(keyboard->data, codepoint);
}
