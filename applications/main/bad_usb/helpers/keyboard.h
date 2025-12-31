#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <furi.h>
#include <storage/storage.h>
#include "bad_usb_hid.h"

typedef struct BadUsbKeyboard BadUsbKeyboard;

BadUsbKeyboard* bad_usb_keyboard_alloc_default();
BadUsbKeyboard* bad_usb_keyboard_alloc_read(File* layout_file);
void bad_usb_keyboard_free(BadUsbKeyboard* keyboard);

void bad_usb_keyboard_type_string(
    BadUsbKeyboard* keyboard,
    const BadUsbHidApi* hid,
    void* hid_inst,
    const char* string);
void bad_usb_keyboard_type_codepoint(
    BadUsbKeyboard* keyboard,
    const BadUsbHidApi* hid,
    void* hid_inst,
    FuriStringUnicodeValue codepoint);

uint16_t bad_usb_keyboard_get_key(BadUsbKeyboard* keyboard, FuriStringUnicodeValue codepoint);

#ifdef __cplusplus
}
#endif
