#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <gui/icon.h>
#include <gui/view.h>

#include "key_setting_model.h"
#include "keyboard_input.h"

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
