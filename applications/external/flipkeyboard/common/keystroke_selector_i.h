#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <gui/icon.h>
#include <gui/view.h>

#include "button_model.h"
#include "keystroke_selector.h"

struct KeystrokeSelector {
    View* view_keystroke_selector;
    ButtonModel* bm;
};

typedef struct KeystrokeSelectorModel {
    // The total number of rows.
    uint8_t rows;

    // The total number of columns.
    uint8_t cols;

    // An array of all the keys.
    KeystrokeSelectorKey* keys;

    // The current row to show at the top of the view.
    uint8_t top_row;

    // The current column where the cursor is.
    uint8_t current_col;

    // The current row where the cursor is.
    uint8_t current_row;

    // Any modifiers that should be sent with the keystroke (CTRL, SHIFT, etc.)
    uint16_t modifiers;

    // The callback to call when a key is selected.
    KeystrokeSelectorCallback callback;

    // The context to pass to the callback.
    void* callback_context;
} KeystrokeSelectorModel;

typedef struct KeystrokeSelectorKeyResult {
    // The code that should be sent to the host.
    uint16_t code;

    // The character that should be displayed on the key.
    char ch;

    // The icon that should be displayed on the key (set ch to 0).
    const Icon* icon;

    // The starting column of the key.
    uint8_t col;

    // The row of the key.
    uint8_t row;

    // The width of the key.
    uint8_t width;
} KeystrokeSelectorKeyResult;
