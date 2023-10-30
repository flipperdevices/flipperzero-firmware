#include "keyboard_input_i.h"

#define KEYBOARD_INPUT_COLS 12
#define KEYBOARD_INPUT_ROWS 7

#define KEYBOARD_INPUT_DEFAULT_TOP_ROW 2
#define KEYBOARD_INPUT_DISPLAYED_ROWS 5
#define KEYBOARD_INPUT_DISPLAYED_WIDTH 10
#define KEYBOARD_INPUT_DISPLAYED_HEIGHT 13
#define KEYBOARD_INPUT_GLYPH_HEIGHT_OFFSET 10
#define KEYBOARD_INPUT_IMAGE_HEIGHT_OFFSET 1

static KeyboardInputKeyResult
    keyboard_input_model_get_key(KeyboardInputModel* model, uint8_t col, uint8_t row) {
    if(row > model->rows || col > model->cols) {
        KeyboardInputKeyResult result = {
            .code = 0, .ch = 0, .icon = NULL, .col = 0, .row = 0, .width = 0};
        FURI_LOG_E("keyboard_input", "Invalid parameter. row=%u, col=%u", row, col);
        return result;
    }

    uint16_t index = row * model->cols + col;
    uint16_t idx = index - 1;

    KeyboardInputKeyResult result = {
        .code = model->keys[index].code,
        .ch = model->keys[index].ch,
        .icon = model->keys[index].icon,
        .col = col,
        .row = row,
        .width = 1};

    while(idx != 0 && index != 0) {
        if(model->keys[idx].code != model->keys[index].code) {
            break;
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

static KeyboardInputKeyResult
    keyboard_input_model_find_key(KeyboardInputModel* model, uint16_t key_code) {
    for(int row = 0; row < model->rows; row++) {
        for(int col = 0; col < model->cols; col++) {
            uint16_t index = row * model->cols + col;
            if(model->keys[index].code == key_code) {
                KeyboardInputKeyResult result = keyboard_input_model_get_key(model, col, row);
                return result;
            }
        }
    }

    uint16_t search_key_code = key_code & 0xFF;
    for(int row = 0; row < model->rows; row++) {
        for(int col = 0; col < model->cols; col++) {
            uint16_t index = row * model->cols + col;
            if(model->keys[index].code == search_key_code) {
                KeyboardInputKeyResult result = keyboard_input_model_get_key(model, col, row);
                return result;
            }
        }
    }

    furi_assert(key_code != HID_KEYBOARD_A);
    return keyboard_input_model_find_key(model, HID_KEYBOARD_A);
}

static bool is_hanging_char(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch == ',') || (ch == ';');
}

static void keyboard_input_draw_callback(Canvas* canvas, void* context) {
    KeyboardInputModel* model = (KeyboardInputModel*)context;
    canvas_set_font(canvas, FontKeyboard);

    for(int y = 0; y < KEYBOARD_INPUT_DISPLAYED_ROWS; y++) {
        for(int x = 0; x < model->cols;) {
            KeyboardInputKeyResult result =
                keyboard_input_model_get_key(model, x, y + model->top_row);
            if(result.code == 0 || result.width == 0) {
                x++;
                continue;
            }

            uint8_t bx = x * KEYBOARD_INPUT_DISPLAYED_WIDTH;
            uint8_t by = y * KEYBOARD_INPUT_DISPLAYED_HEIGHT;
            uint8_t bw = (KEYBOARD_INPUT_DISPLAYED_WIDTH * result.width) - 1;

            if(result.col <= model->current_col &&
               result.col + result.width - 1 >= model->current_col &&
               result.row == model->current_row) {
                canvas_draw_rbox(canvas, bx, by, bw, KEYBOARD_INPUT_DISPLAYED_HEIGHT - 1, 1);
                canvas_set_color(canvas, ColorWhite);
            } else {
                canvas_draw_rframe(canvas, bx, by, bw, KEYBOARD_INPUT_DISPLAYED_HEIGHT - 1, 1);
                canvas_set_color(canvas, ColorBlack);
            }

            uint8_t gy = y * KEYBOARD_INPUT_DISPLAYED_HEIGHT;
            if(is_hanging_char(result.ch)) {
                gy--;
            }

            if(result.ch != 0) {
                canvas_draw_glyph(
                    canvas,
                    x * KEYBOARD_INPUT_DISPLAYED_WIDTH + 2,
                    gy + KEYBOARD_INPUT_GLYPH_HEIGHT_OFFSET,
                    result.ch);
            } else if(result.icon != NULL) {
                canvas_draw_icon(
                    canvas,
                    x * KEYBOARD_INPUT_DISPLAYED_WIDTH + 2,
                    gy + KEYBOARD_INPUT_IMAGE_HEIGHT_OFFSET,
                    result.icon);
            }
            canvas_set_color(canvas, ColorBlack);

            x += result.width;
        }
    }
}

static bool keyboard_input_input_callback(InputEvent* event, void* context) {
    KeyboardInput* keyboard_input = (KeyboardInput*)context;
    bool was_handled = false;

    if(event->type == InputTypeShort) {
        if(event->key == InputKeyLeft) {
            with_view_model(
                keyboard_input->view_keyboard_input,
                KeyboardInputModel * model,
                {
                    KeyboardInputKeyResult kikr = keyboard_input_model_get_key(
                        model, model->current_col, model->current_row);
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
                keyboard_input->view_keyboard_input,
                KeyboardInputModel * model,
                {
                    KeyboardInputKeyResult kikr = keyboard_input_model_get_key(
                        model, model->current_col, model->current_row);
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
                keyboard_input->view_keyboard_input,
                KeyboardInputModel * model,
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
                keyboard_input->view_keyboard_input,
                KeyboardInputModel * model,
                {
                    if(model->current_row + 1 < model->rows) {
                        model->current_row++;
                    }
                    if(model->current_row > model->top_row + (KEYBOARD_INPUT_DISPLAYED_ROWS - 1)) {
                        model->top_row = model->current_row - (KEYBOARD_INPUT_DISPLAYED_ROWS - 1);
                    }
                },
                true);
            was_handled = true;
        } else if(event->key == InputKeyOk) {
            with_view_model(
                keyboard_input->view_keyboard_input,
                KeyboardInputModel * model,
                {
                    if(model->callback != NULL) {
                        KeyboardInputKeyResult kikr = keyboard_input_model_get_key(
                            model, model->current_col, model->current_row);
                        model->callback(kikr.code, model->callback_context);
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

KeyboardInput* keyboard_input_alloc() {
    KeyboardInput* keyboard_input = (KeyboardInput*)malloc(sizeof(KeyboardInput));

    keyboard_input->view_keyboard_input = view_alloc();
    view_set_context(keyboard_input->view_keyboard_input, keyboard_input);
    view_set_draw_callback(keyboard_input->view_keyboard_input, keyboard_input_draw_callback);
    view_set_input_callback(keyboard_input->view_keyboard_input, keyboard_input_input_callback);
    view_set_previous_callback(
        keyboard_input->view_keyboard_input,
        NULL); // This will be set later by key_config's item_clicked.
    view_allocate_model(
        keyboard_input->view_keyboard_input, ViewModelTypeLocking, sizeof(KeyboardInputModel));
    with_view_model(
        keyboard_input->view_keyboard_input,
        KeyboardInputModel * model,
        {
            model->cols = KEYBOARD_INPUT_COLS;
            model->rows = KEYBOARD_INPUT_ROWS;
            model->top_row = 0;
            model->current_col = 0;
            model->current_row = 0;
            model->modifiers = 0;
            model->keys = keys;
        },
        true);

    return keyboard_input;
}

void keyboard_input_free(KeyboardInput* keyboard_input) {
    view_free(keyboard_input->view_keyboard_input);
    free(keyboard_input);
}

void keyboard_input_set_key(KeyboardInput* keyboard_input, uint16_t key_code) {
    with_view_model(
        keyboard_input->view_keyboard_input,
        KeyboardInputModel * model,
        {
            KeyboardInputKeyResult kikr = keyboard_input_model_find_key(model, key_code);
            model->current_row = kikr.row;
            model->top_row = KEYBOARD_INPUT_DEFAULT_TOP_ROW;
            if(model->current_row > model->top_row + (KEYBOARD_INPUT_DISPLAYED_ROWS - 1)) {
                model->top_row = model->current_row - (KEYBOARD_INPUT_DISPLAYED_ROWS - 1);
            } else if(model->current_row < model->top_row) {
                model->top_row = model->current_row;
            }
            model->current_col = kikr.col;
            model->modifiers = 0;
        },
        true);
}

void keyboard_input_set_callback(
    KeyboardInput* keyboard_input,
    KeyboardInputCallback callback,
    void* context) {
    with_view_model(
        keyboard_input->view_keyboard_input,
        KeyboardInputModel * model,
        {
            model->callback = callback;
            model->callback_context = context;
        },
        true);
}

View* keyboard_input_get_view(KeyboardInput* keyboard_input) {
    return keyboard_input->view_keyboard_input;
}