#include "keystroke_selector_i.h"

struct KeystrokeSelectorModel {
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
};

struct KeystrokeSelectorKeyResult {
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
};

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
    uint8_t rows) {
    KeystrokeSelector* keystroke_selector = (KeystrokeSelector*)malloc(sizeof(KeystrokeSelector));

    keystroke_selector->view_keystroke_selector = view_alloc();
    view_set_context(keystroke_selector->view_keystroke_selector, keystroke_selector);
    view_set_draw_callback(
        keystroke_selector->view_keystroke_selector, keystroke_selector_draw_callback);
    view_set_input_callback(
        keystroke_selector->view_keystroke_selector, keystroke_selector_input_callback);
    view_set_previous_callback(
        keystroke_selector->view_keystroke_selector,
        NULL); // This will be set later by key_config's item_clicked.
    view_allocate_model(
        keystroke_selector->view_keystroke_selector,
        ViewModelTypeLocking,
        sizeof(KeystrokeSelectorModel));

    for(size_t i = 0; i < rows * KEYSTROKE_SELECTOR_COLS; i++) {
        if(shift_keys[i].code != 0) {
            keys[i].shift = &shift_keys[i];
        } else {
            keys[i].shift = NULL;
        }
    }

    with_view_model(
        keystroke_selector->view_keystroke_selector,
        KeystrokeSelectorModel * model,
        {
            model->cols = KEYSTROKE_SELECTOR_COLS;
            model->rows = rows;
            model->top_row = 0;
            model->current_col = 0;
            model->current_row = 0;
            model->modifiers = 0;
            model->keys = keys;
        },
        true);

    return keystroke_selector;
}

/**
 * @brief Frees a keystroke selector.
 * @param keystroke_selector The keystroke selector to free.
*/
void keystroke_selector_free(KeystrokeSelector* keystroke_selector) {
    with_view_model(
        keystroke_selector->view_keystroke_selector,
        KeystrokeSelectorModel * model,
        {
            for(size_t i = 0; i < model->rows * KEYSTROKE_SELECTOR_COLS; i++) {
                if(model->keys[i].shift) {
                    free(model->keys[i].shift);
                    model->keys[i].shift = NULL;
                }
            }
        },
        false);
    view_free(keystroke_selector->view_keystroke_selector);
    free(keystroke_selector);
}

/**
 * @brief Gets the view for the keystroke selector.
 * @param keystroke_selector The keystroke selector.
 * @return The view.
*/
View* keystroke_selector_get_view(KeystrokeSelector* keystroke_selector) {
    return keystroke_selector->view_keystroke_selector;
}

/**
 * @brief Sets the key code to highlight.
 * @details Sets the key code to highlight. This will scroll the view to the key code. If the key
 * code is not found, it will default to the 'a' key. If the key code is a shift key, it will
 * highlight the key code that the shift key is associated with.
 * @param keystroke_selector The keystroke selector.
 * @param key_code The key code to highlight.
*/
void keystroke_selector_set_key(KeystrokeSelector* keystroke_selector, uint16_t key_code) {
    with_view_model(
        keystroke_selector->view_keystroke_selector,
        KeystrokeSelectorModel * model,
        {
            KeystrokeSelectorKeyResult kikr = keystroke_selector_model_find_key(model, key_code);
            model->current_row = kikr.row;
            model->top_row = KEYSTROKE_SELECTOR_DEFAULT_TOP_ROW;
            if(model->current_row > model->top_row + (KEYSTROKE_SELECTOR_DISPLAYED_ROWS - 1)) {
                model->top_row = model->current_row - (KEYSTROKE_SELECTOR_DISPLAYED_ROWS - 1);
            } else if(model->current_row < model->top_row) {
                model->top_row = model->current_row;
            }
            model->current_col = kikr.col;
            model->modifiers = key_code & 0xFF00;
        },
        true);
}

/**
 * @brief Sets the callback for keystroke selection.
 * @param keystroke_selector The keystroke selector.
 * @param callback The callback to call when a key is selected.
 * @param context The context to pass to the callback.
*/
void keystroke_selector_set_callback(
    KeystrokeSelector* keystroke_selector,
    KeystrokeSelectorCallback callback,
    void* context) {
    with_view_model(
        keystroke_selector->view_keystroke_selector,
        KeystrokeSelectorModel * model,
        {
            model->callback = callback;
            model->callback_context = context;
        },
        true);
}

/**
 * @brief Returns true if the shift key modifier is on.
 * @param model The keystroke selector model.
 * @return True if the shift key modifier is on.
*/
static bool keystroke_selector_model_shifted(KeystrokeSelectorModel* model) {
    return (model->modifiers & KEY_MOD_LEFT_SHIFT) != 0 ||
           (model->modifiers & KEY_MOD_RIGHT_SHIFT) != 0;
}

/**
 * @brief Returns true if the code represents a shift key.
 * @param model The keystroke selector model.
 * @param code The code to check.
 * @return True if the code represents a shift key.
*/
static bool
    keystroke_selector_model_enabled_modifier(KeystrokeSelectorModel* model, uint16_t code) {
    return (code == HID_KEYBOARD_L_SHIFT && (model->modifiers & KEY_MOD_LEFT_SHIFT)) ||
           (code == HID_KEYBOARD_R_SHIFT && (model->modifiers & KEY_MOD_RIGHT_SHIFT));
}

/**
 * @brief Gets the key at the specified column and row.
 * @param model The keystroke selector model.
 * @param col The column.
 * @param row The row.
 * @param shifted True if the shift key modifier is on.
 * @return The key result.
*/
static KeystrokeSelectorKeyResult keystroke_selector_model_get_key(
    KeystrokeSelectorModel* model,
    uint8_t col,
    uint8_t row,
    bool shifted) {
    if(row > model->rows || col > model->cols) {
        KeystrokeSelectorKeyResult result = {
            .code = 0, .ch = 0, .icon = NULL, .col = 0, .row = 0, .width = 0};
        FURI_LOG_E("keystroke_selector", "Invalid parameter. row=%u, col=%u", row, col);
        return result;
    }

    uint16_t index = row * model->cols + col;
    uint16_t idx = index - 1;

    KeystrokeSelectorKey* key = !shifted ? &model->keys[index] :
                                ((model->keys[index].shift != NULL) &&
                                 (model->keys[index].code != model->keys[index].shift->code)) ?
                                           model->keys[index].shift :
                                           &model->keys[index];

    if((model->keys[index].shift != NULL) &&
       keystroke_selector_model_enabled_modifier(model, key->code)) {
        key = model->keys[index].shift;
    }

    KeystrokeSelectorKeyResult result = {
        .code = key->code, .ch = key->ch, .icon = key->icon, .col = col, .row = row, .width = 1};

    while(idx != 0 && index != 0) {
        if(!shifted) {
            if(model->keys[idx].code != model->keys[index].code) {
                break;
            }
        } else {
            uint16_t c1 = model->keys[idx].shift != NULL ? model->keys[idx].shift->code :
                                                           model->keys[idx].code;
            uint16_t c2 = model->keys[index].shift != NULL ? model->keys[index].shift->code :
                                                             model->keys[index].code;
            if(c1 != c2) {
                break;
            }
        }

        result.col--;
        index = idx;
        idx--;
    }

    while(result.col < model->cols && result.code == model->keys[index + result.width].code) {
        result.width++;
    }

    return result;
}

/**
 * @brief Finds the key with the specified code.
 * @param model The keystroke selector model.
 * @param key_code The key code to find.
 * @return The key result.
*/
static KeystrokeSelectorKeyResult
    keystroke_selector_model_find_key(KeystrokeSelectorModel* model, uint16_t key_code) {
    for(int row = 0; row < model->rows; row++) {
        for(int col = 0; col < model->cols; col++) {
            uint16_t index = row * model->cols + col;
            if(model->keys[index].code == key_code) {
                KeystrokeSelectorKeyResult result =
                    keystroke_selector_model_get_key(model, col, row, false);
                return result;
            }
        }
    }

    for(int row = 0; row < model->rows; row++) {
        for(int col = 0; col < model->cols; col++) {
            uint16_t index = row * model->cols + col;
            if(model->keys[index].code == key_code) {
                KeystrokeSelectorKeyResult result =
                    keystroke_selector_model_get_key(model, col, row, true);
                return result;
            }
        }
    }

    uint16_t search_key_code = key_code & 0xFF;
    for(int row = 0; row < model->rows; row++) {
        for(int col = 0; col < model->cols; col++) {
            uint16_t index = row * model->cols + col;
            if(model->keys[index].code == search_key_code) {
                KeystrokeSelectorKeyResult result =
                    keystroke_selector_model_get_key(model, col, row, false);
                return result;
            }
        }
    }

    furi_assert(key_code != HID_KEYBOARD_A);
    return keystroke_selector_model_find_key(model, HID_KEYBOARD_A);
}

/**
 * @brief Returns true if the character is a hanging character (needs to be 
 * drawn higher in the UI).
 * @param ch The character to check.
 * @return True if the character is a hanging character.
*/
static bool is_hanging_char(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch == ',') || (ch == ';');
}

/**
 * @brief Draws the keystroke selector.
 * @param canvas The canvas to draw on.
 * @param context The context.
*/
static void keystroke_selector_draw_callback(Canvas* canvas, void* context) {
    KeystrokeSelectorModel* model = (KeystrokeSelectorModel*)context;
    canvas_set_font(canvas, FontKeyboard);

    bool shifted = keystroke_selector_model_shifted(model);
    for(int y = 0; y < KEYSTROKE_SELECTOR_DISPLAYED_ROWS; y++) {
        for(int x = 0; x < model->cols;) {
            KeystrokeSelectorKeyResult result =
                keystroke_selector_model_get_key(model, x, y + model->top_row, shifted);

            if(result.code == 0 || result.width == 0) {
                x++;
                continue;
            }

            uint8_t bx = x * KEYSTROKE_SELECTOR_DISPLAYED_WIDTH;
            uint8_t by = y * KEYSTROKE_SELECTOR_DISPLAYED_HEIGHT;
            uint8_t bw = (KEYSTROKE_SELECTOR_DISPLAYED_WIDTH * result.width) - 1;

            if(result.col <= model->current_col &&
               result.col + result.width - 1 >= model->current_col &&
               result.row == model->current_row) {
                canvas_draw_rbox(canvas, bx, by, bw, KEYSTROKE_SELECTOR_DISPLAYED_HEIGHT - 1, 1);
                canvas_set_color(canvas, ColorWhite);
            } else {
                canvas_draw_rframe(canvas, bx, by, bw, KEYSTROKE_SELECTOR_DISPLAYED_HEIGHT - 1, 1);
                canvas_set_color(canvas, ColorBlack);
            }

            uint8_t gy = y * KEYSTROKE_SELECTOR_DISPLAYED_HEIGHT;
            if(is_hanging_char(result.ch)) {
                gy--;
            }

            if(result.ch != 0) {
                canvas_draw_glyph(
                    canvas,
                    x * KEYSTROKE_SELECTOR_DISPLAYED_WIDTH + 2,
                    gy + KEYSTROKE_SELECTOR_GLYPH_HEIGHT_OFFSET,
                    result.ch);
            } else if(result.icon != NULL) {
                canvas_draw_icon(
                    canvas,
                    x * KEYSTROKE_SELECTOR_DISPLAYED_WIDTH + 2,
                    gy + KEYSTROKE_SELECTOR_IMAGE_HEIGHT_OFFSET,
                    result.icon);
            }
            canvas_set_color(canvas, ColorBlack);

            x += result.width;
        }
    }
}

/**
 * @brief Handles input events for the keystroke selector.
 * @param event The input event.
 * @param context The context.
 * @return True if the event was handled.
*/
static bool keystroke_selector_input_callback(InputEvent* event, void* context) {
    KeystrokeSelector* keystroke_selector = (KeystrokeSelector*)context;
    bool was_handled = false;

    if(event->type == InputTypeShort) {
        if(event->key == InputKeyLeft) {
            with_view_model(
                keystroke_selector->view_keystroke_selector,
                KeystrokeSelectorModel * model,
                {
                    bool shifted = keystroke_selector_model_shifted(model);
                    KeystrokeSelectorKeyResult kikr = keystroke_selector_model_get_key(
                        model, model->current_col, model->current_row, shifted);
                    model->current_col = kikr.col;
                    if(model->current_col > 0) {
                        model->current_col--;
                    } else {
                        model->current_col = model->cols - 1;
                    }
                },
                true);
            was_handled = true;
        } else if(event->key == InputKeyRight) {
            with_view_model(
                keystroke_selector->view_keystroke_selector,
                KeystrokeSelectorModel * model,
                {
                    bool shifted = keystroke_selector_model_shifted(model);
                    KeystrokeSelectorKeyResult kikr = keystroke_selector_model_get_key(
                        model, model->current_col, model->current_row, shifted);
                    model->current_col = kikr.col + kikr.width - 1;
                    if(model->current_col < model->cols - 1) {
                        model->current_col++;
                    } else {
                        model->current_col = 0;
                    }
                },
                true);
            was_handled = true;
        } else if(event->key == InputKeyUp) {
            with_view_model(
                keystroke_selector->view_keystroke_selector,
                KeystrokeSelectorModel * model,
                {
                    if(model->current_row > 0) {
                        model->current_row--;
                    }
                    if(model->top_row > model->current_row) {
                        model->top_row = model->current_row;
                    }
                },
                true);
            was_handled = true;
        } else if(event->key == InputKeyDown) {
            with_view_model(
                keystroke_selector->view_keystroke_selector,
                KeystrokeSelectorModel * model,
                {
                    if(model->current_row + 1 < model->rows) {
                        model->current_row++;
                    }
                    if(model->current_row >
                       model->top_row + (KEYSTROKE_SELECTOR_DISPLAYED_ROWS - 1)) {
                        model->top_row =
                            model->current_row - (KEYSTROKE_SELECTOR_DISPLAYED_ROWS - 1);
                    }
                },
                true);
            was_handled = true;
        } else if(event->key == InputKeyOk) {
            with_view_model(
                keystroke_selector->view_keystroke_selector,
                KeystrokeSelectorModel * model,
                {
                    bool shifted = keystroke_selector_model_shifted(model);
                    if(model->callback != NULL) {
                        KeystrokeSelectorKeyResult kikr = keystroke_selector_model_get_key(
                            model, model->current_col, model->current_row, shifted);
                        if(kikr.code == HID_KEYBOARD_L_SHIFT) {
                            model->modifiers ^= KEY_MOD_LEFT_SHIFT;
                        } else {
                            model->callback(kikr.code, model->callback_context);
                        }
                        was_handled = true;
                    } else {
                        furi_assert(model->callback != NULL);
                    }
                },
                true);
        }
    }

    return was_handled;
}
