#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <gui/icon.h>
#include <gui/view.h>

#include "key_setting_model.h"
#include "flipboard_keyboard_icons.h" // ARGH!!!! MAYBE MAKE THE KEYBOARD CONFIG NOT IN COMMON?
#include "keyboard_input.h"

typedef struct KeyboardInputKey KeyboardInputKey;

struct KeyboardInputKey {
    uint16_t code;
    char ch;
    const Icon* icon;
    KeyboardInputKey* shift;
};

struct KeyboardInput {
    View* view_keyboard_input;
    KeySettingModel* ksm;
};

typedef struct KeyboardInputModel {
    uint8_t rows;
    uint8_t cols;
    KeyboardInputKey* keys;
    uint8_t top_row;
    uint8_t current_col;
    uint8_t current_row;
    uint16_t modifiers;

    KeyboardInputCallback callback;
    void* callback_context;
} KeyboardInputModel;

typedef struct KeyboardInputKeyResult {
    uint16_t code;
    char ch;
    const Icon* icon;
    uint8_t col;
    uint8_t row;
    uint8_t width;
} KeyboardInputKeyResult;

KeyboardInputKey keys[] = {
    {.code = HID_KEYBOARD_F1, .ch = '1', .icon = NULL},
    {.code = HID_KEYBOARD_F1, .ch = '1', .icon = NULL},
    {.code = HID_KEYBOARD_F2, .ch = '2', .icon = NULL},
    {.code = HID_KEYBOARD_F2, .ch = '2', .icon = NULL},
    {.code = HID_KEYBOARD_F3, .ch = '3', .icon = NULL},
    {.code = HID_KEYBOARD_F3, .ch = '3', .icon = NULL},
    {.code = HID_KEYBOARD_F4, .ch = '4', .icon = NULL},
    {.code = HID_KEYBOARD_F4, .ch = '4', .icon = NULL},
    {.code = HID_KEYBOARD_F5, .ch = '5', .icon = NULL},
    {.code = HID_KEYBOARD_F5, .ch = '5', .icon = NULL},
    {.code = HID_KEYBOARD_F6, .ch = '6', .icon = NULL},
    {.code = HID_KEYBOARD_F6, .ch = '6', .icon = NULL},

    {.code = HID_KEYBOARD_F7, .ch = '7', .icon = NULL},
    {.code = HID_KEYBOARD_F7, .ch = '7', .icon = NULL},
    {.code = HID_KEYBOARD_F8, .ch = '8', .icon = NULL},
    {.code = HID_KEYBOARD_F8, .ch = '8', .icon = NULL},
    {.code = HID_KEYBOARD_F9, .ch = '9', .icon = NULL},
    {.code = HID_KEYBOARD_F9, .ch = '9', .icon = NULL},
    {.code = HID_KEYBOARD_F10, .ch = 'A', .icon = NULL},
    {.code = HID_KEYBOARD_F10, .ch = 'A', .icon = NULL},
    {.code = HID_KEYBOARD_F11, .ch = 'B', .icon = NULL},
    {.code = HID_KEYBOARD_F11, .ch = 'B', .icon = NULL},
    {.code = HID_KEYBOARD_F12, .ch = 'C', .icon = NULL},
    {.code = HID_KEYBOARD_F12, .ch = 'C', .icon = NULL},

    {.code = HID_KEYBOARD_1, .ch = '1', .icon = NULL},
    {.code = HID_KEYBOARD_2, .ch = '2', .icon = NULL},
    {.code = HID_KEYBOARD_3, .ch = '3', .icon = NULL},
    {.code = HID_KEYBOARD_4, .ch = '4', .icon = NULL},
    {.code = HID_KEYBOARD_5, .ch = '5', .icon = NULL},
    {.code = HID_KEYBOARD_6, .ch = '6', .icon = NULL},
    {.code = HID_KEYBOARD_7, .ch = '7', .icon = NULL},
    {.code = HID_KEYBOARD_8, .ch = '8', .icon = NULL},
    {.code = HID_KEYBOARD_9, .ch = '9', .icon = NULL},
    {.code = HID_KEYBOARD_0, .ch = '0', .icon = NULL},
    {.code = HID_KEYBOARD_DELETE, .ch = 0, .icon = &I_backspace_15x10},
    {.code = HID_KEYBOARD_DELETE, .ch = 0, .icon = &I_backspace_15x10},

    {.code = HID_KEYBOARD_TAB, .ch = 'T', .icon = NULL},
    {.code = HID_KEYBOARD_TAB, .ch = 'T', .icon = NULL},
    {.code = HID_KEYBOARD_Q, .ch = 'q', .icon = NULL},
    {.code = HID_KEYBOARD_W, .ch = 'w', .icon = NULL},
    {.code = HID_KEYBOARD_E, .ch = 'e', .icon = NULL},
    {.code = HID_KEYBOARD_R, .ch = 'r', .icon = NULL},
    {.code = HID_KEYBOARD_T, .ch = 't', .icon = NULL},
    {.code = HID_KEYBOARD_Y, .ch = 'y', .icon = NULL},
    {.code = HID_KEYBOARD_U, .ch = 'u', .icon = NULL},
    {.code = HID_KEYBOARD_I, .ch = 'i', .icon = NULL},
    {.code = HID_KEYBOARD_O, .ch = 'o', .icon = NULL},
    {.code = HID_KEYBOARD_P, .ch = 'p', .icon = NULL},

    {.code = HID_KEYBOARD_A, .ch = 'a', .icon = NULL},
    {.code = HID_KEYBOARD_S, .ch = 's', .icon = NULL},
    {.code = HID_KEYBOARD_D, .ch = 'd', .icon = NULL},
    {.code = HID_KEYBOARD_F, .ch = 'f', .icon = NULL},
    {.code = HID_KEYBOARD_G, .ch = 'g', .icon = NULL},
    {.code = HID_KEYBOARD_H, .ch = 'h', .icon = NULL},
    {.code = HID_KEYBOARD_J, .ch = 'j', .icon = NULL},
    {.code = HID_KEYBOARD_K, .ch = 'k', .icon = NULL},
    {.code = HID_KEYBOARD_L, .ch = 'l', .icon = NULL},
    {.code = HID_KEYBOARD_RETURN, .ch = 0, .icon = &I_enter_25x10},
    {.code = HID_KEYBOARD_RETURN, .ch = 0, .icon = &I_enter_25x10},
    {.code = HID_KEYBOARD_RETURN, .ch = 0, .icon = &I_enter_25x10},

    {.code = HID_KEYBOARD_L_SHIFT, .ch = 'S', .icon = NULL},
    {.code = HID_KEYBOARD_L_SHIFT, .ch = 'S', .icon = NULL},
    {.code = HID_KEYBOARD_Z, .ch = 'z', .icon = NULL},
    {.code = HID_KEYBOARD_X, .ch = 'x', .icon = NULL},
    {.code = HID_KEYBOARD_C, .ch = 'c', .icon = NULL},
    {.code = HID_KEYBOARD_V, .ch = 'v', .icon = NULL},
    {.code = HID_KEYBOARD_B, .ch = 'b', .icon = NULL},
    {.code = HID_KEYBOARD_N, .ch = 'n', .icon = NULL},
    {.code = HID_KEYBOARD_M, .ch = 'm', .icon = NULL},
    {.code = HID_KEYBOARD_COMMA, .ch = ',', .icon = NULL},
    {.code = HID_KEYBOARD_DOT, .ch = '.', .icon = NULL},
    {.code = HID_KEYBOARD_SLASH, .ch = '/', .icon = NULL},

    {.code = HID_KEYBOARD_L_CTRL, .ch = 'C', .icon = NULL},
    {.code = HID_KEYBOARD_L_GUI, .ch = 0, .icon = &I_gui_5x10},
    {.code = HID_KEYBOARD_L_ALT, .ch = 'A', .icon = NULL},
    {.code = HID_KEYBOARD_SPACEBAR, .ch = ' ', .icon = NULL},
    {.code = HID_KEYBOARD_SPACEBAR, .ch = ' ', .icon = NULL},
    {.code = HID_KEYBOARD_SPACEBAR, .ch = ' ', .icon = NULL},
    {.code = HID_KEYBOARD_SPACEBAR, .ch = ' ', .icon = NULL},
    {.code = HID_KEYBOARD_BACKSLASH, .ch = '\\', .icon = NULL},
    {.code = HID_KEYBOARD_SEMICOLON, .ch = ';', .icon = NULL},
    {.code = HID_KEYBOARD_GRAVE_ACCENT, .ch = '`', .icon = NULL},
    {.code = HID_KEYBOARD_MINUS, .ch = '-', .icon = NULL},
    {.code = HID_KEYBOARD_EQUAL_SIGN, .ch = '=', .icon = NULL},

};