#pragma once

/**
 * @file keystroke_selector.h
 * @brief A view that allows the user to select a keystroke.
 * @details A view that allows the user to select a keystroke. The view will display a grid of keys. 
 * The user can scroll through the keys using the dpad button. The user can select a key by pressing
 * the OK button. The view will call a callback when a key is selected.
*/

#include <gui/view.h>
#include "button_model.h"

typedef struct KeystrokeSelector KeystrokeSelector;
typedef struct KeystrokeSelectorKey KeystrokeSelectorKey;

struct KeystrokeSelectorKey {
    // The code that should be sent to the host.
    uint16_t code;

    // The character that should be displayed on the key.
    char ch;

    // The icon that should be displayed on the key (set ch to 0).
    const Icon* icon;

    // The key used for shift.  (If this is a shift key, this should be set to NULL.)
    KeystrokeSelectorKey* shift;
};

/**
 * @brief Callback for keystroke selection.
 * @param key_code The key code that was selected.
 * @param context The context that was passed to the callback.
*/
typedef void (*KeystrokeSelectorCallback)(uint16_t key_code, void* context);

/**
 * @brief Allocates a new keystroke selector.
 * @param keys The keys to display.
 * @param shift_keys The shift keys to display.
 * @param rows The number of rows to display.
 * @return The new keystroke selector.
*/
KeystrokeSelector* keystroke_selector_alloc(
    KeystrokeSelectorKey* keys,
    KeystrokeSelectorKey* shift_keys,
    uint8_t rows);

/**
 * @brief Frees a keystroke selector.
 * @param keystroke_selector The keystroke selector to free.
*/
void keystroke_selector_free(KeystrokeSelector* keystroke_selector);

/**
 * @brief Gets the view for the keystroke selector.
 * @param keystroke_selector The keystroke selector.
 * @return The view.
*/
View* keystroke_selector_get_view(KeystrokeSelector* keystroke_selector);

/**
 * @brief Sets the key code to highlight.
 * @details Sets the key code to highlight. This will scroll the view to the key code. If the key
 * code is not found, it will default to the 'a' key. If the key code is a shift key, it will
 * highlight the key code that the shift key is associated with.
 * @param keystroke_selector The keystroke selector.
 * @param key_code The key code to highlight.
*/
void keystroke_selector_set_key(KeystrokeSelector* keystroke_selector, uint16_t key_code);

/**
 * @brief Sets the callback for keystroke selection.
 * @param keystroke_selector The keystroke selector.
 * @param callback The callback to call when a key is selected.
 * @param context The context to pass to the callback.
*/
void keystroke_selector_set_callback(
    KeystrokeSelector* keystroke_selector,
    KeystrokeSelectorCallback callback,
    void* context);