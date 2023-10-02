#pragma once

#include <furi_hal.h>

uint16_t keystroke_values[] = {
    0,
    HID_KEYBOARD_A | KEY_MOD_LEFT_SHIFT, // A
    HID_KEYBOARD_B | KEY_MOD_LEFT_SHIFT, // B
    HID_KEYBOARD_C | KEY_MOD_LEFT_SHIFT, // C
    HID_KEYBOARD_A, // a
    HID_KEYBOARD_B, // b
    HID_KEYBOARD_C, // c
};
char* keystroke_names[] = {"None", "A", "B", "C", "a", "b", "c"};

char* keystroke_count_names[] =
    {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15"};
