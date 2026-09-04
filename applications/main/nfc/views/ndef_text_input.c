#include "ndef_text_input.h"
#include <gui/elements.h>
#include <assets_icons.h>
#include <furi.h>

struct NdefTextInput {
    View* view;
    FuriTimer* timer;
};

typedef struct {
    const char text;
    const uint8_t x;
    const uint8_t y;
} NdefTextInputKey;

typedef struct {
    const char* header;
    char* text_buffer;
    size_t text_buffer_size;
    size_t minimum_length;
    bool clear_default_text;
    bool symbol_mode;

    NdefTextInputCallback callback;
    void* callback_context;

    uint8_t selected_row;
    uint8_t selected_column;

    NdefTextInputValidatorCallback validator_callback;
    void* validator_callback_context;
    FuriString* validator_text;
    bool validator_message_visible;
} NdefTextInputModel;

static const uint8_t keyboard_origin_x = 1;
static const uint8_t keyboard_origin_y = 29;
static const uint8_t keyboard_row_count = 3;

#define ENTER_KEY     '\r'
#define BACKSPACE_KEY '\b'
#define SHIFT_KEY     '\x01'

// Letter layout: identical to stock text_input except `_` is replaced with SHIFT.
// `_` is reachable via the symbol layout (symbols_row_2[0]).
static const NdefTextInputKey letters_row_1[] = {
    {'q', 1, 8},
    {'w', 10, 8},
    {'e', 19, 8},
    {'r', 28, 8},
    {'t', 37, 8},
    {'y', 46, 8},
    {'u', 55, 8},
    {'i', 64, 8},
    {'o', 73, 8},
    {'p', 82, 8},
    {'0', 91, 8},
    {'1', 100, 8},
    {'2', 110, 8},
    {'3', 120, 8},
};
static const NdefTextInputKey letters_row_2[] = {
    {'a', 1, 20},
    {'s', 10, 20},
    {'d', 19, 20},
    {'f', 28, 20},
    {'g', 37, 20},
    {'h', 46, 20},
    {'j', 55, 20},
    {'k', 64, 20},
    {'l', 73, 20},
    {BACKSPACE_KEY, 82, 12},
    {'4', 100, 20},
    {'5', 110, 20},
    {'6', 120, 20},
};
static const NdefTextInputKey letters_row_3[] = {
    {'z', 1, 32},
    {'x', 10, 32},
    {'c', 19, 32},
    {'v', 28, 32},
    {'b', 37, 32},
    {'n', 46, 32},
    {'m', 55, 32},
    {SHIFT_KEY, 64, 32},
    {ENTER_KEY, 74, 23},
    {'7', 100, 32},
    {'8', 110, 32},
    {'9', 120, 32},
};

// Symbol layout: same key counts and positions as letters; punctuation in
// place of the letters. The shift key keeps the same slot so muscle memory
// works across modes.
static const NdefTextInputKey symbols_row_1[] = {
    {'.', 1, 8},
    {'/', 10, 8},
    {':', 19, 8},
    {'-', 28, 8},
    {'?', 37, 8},
    {'=', 46, 8},
    {'&', 55, 8},
    {'#', 64, 8},
    {'@', 73, 8},
    {'+', 82, 8},
    {'0', 91, 8},
    {'1', 100, 8},
    {'2', 110, 8},
    {'3', 120, 8},
};
static const NdefTextInputKey symbols_row_2[] = {
    {'_', 1, 20},
    {'!', 10, 20},
    {'~', 19, 20},
    {',', 28, 20},
    {'*', 37, 20},
    {';', 46, 20},
    {'(', 55, 20},
    {')', 64, 20},
    {'\'', 73, 20},
    {BACKSPACE_KEY, 82, 12},
    {'4', 100, 20},
    {'5', 110, 20},
    {'6', 120, 20},
};
static const NdefTextInputKey symbols_row_3[] = {
    {'<', 1, 32},
    {'>', 10, 32},
    {'"', 19, 32},
    {'%', 28, 32},
    {'$', 37, 32},
    {'[', 46, 32},
    {']', 55, 32},
    {SHIFT_KEY, 64, 32},
    {ENTER_KEY, 74, 23},
    {'7', 100, 32},
    {'8', 110, 32},
    {'9', 120, 32},
};

static uint8_t get_row_size(const NdefTextInputModel* model, uint8_t row_index) {
    uint8_t row_size = 0;
    if(model->symbol_mode) {
        switch(row_index + 1) {
        case 1:
            row_size = COUNT_OF(symbols_row_1);
            break;
        case 2:
            row_size = COUNT_OF(symbols_row_2);
            break;
        case 3:
            row_size = COUNT_OF(symbols_row_3);
            break;
        default:
            furi_crash();
        }
    } else {
        switch(row_index + 1) {
        case 1:
            row_size = COUNT_OF(letters_row_1);
            break;
        case 2:
            row_size = COUNT_OF(letters_row_2);
            break;
        case 3:
            row_size = COUNT_OF(letters_row_3);
            break;
        default:
            furi_crash();
        }
    }
    return row_size;
}

static const NdefTextInputKey* get_row(const NdefTextInputModel* model, uint8_t row_index) {
    const NdefTextInputKey* row = NULL;
    if(model->symbol_mode) {
        switch(row_index + 1) {
        case 1:
            row = symbols_row_1;
            break;
        case 2:
            row = symbols_row_2;
            break;
        case 3:
            row = symbols_row_3;
            break;
        default:
            furi_crash();
        }
    } else {
        switch(row_index + 1) {
        case 1:
            row = letters_row_1;
            break;
        case 2:
            row = letters_row_2;
            break;
        case 3:
            row = letters_row_3;
            break;
        default:
            furi_crash();
        }
    }
    return row;
}

static char get_selected_char(NdefTextInputModel* model) {
    return get_row(model, model->selected_row)[model->selected_column].text;
}

static bool char_is_lowercase(char letter) {
    return letter >= 0x61 && letter <= 0x7A;
}

static char char_to_uppercase(const char letter) {
    if(letter == '_') {
        return 0x20;
    } else if(islower((int)letter)) {
        return letter - 0x20;
    } else {
        return letter;
    }
}

static void ndef_text_input_backspace_cb(NdefTextInputModel* model) {
    uint8_t text_length = model->clear_default_text ? 1 : strlen(model->text_buffer);
    if(text_length > 0) {
        model->text_buffer[text_length - 1] = 0;
    }
}

// Render the shift key as a small framed box with a label that flips between
// modes. Stock text_input draws Enter/Backspace via dedicated icons; we don't
// have icons for shift, so we paint primitives instead.
static void draw_shift_key(Canvas* canvas, uint8_t x, uint8_t y, bool selected, bool symbol_mode) {
    // Key cell: 8 wide × 10 tall (matches the selection highlight box).
    if(selected) {
        canvas_set_color(canvas, ColorBlack);
        canvas_draw_box(canvas, x - 1, y - 8, 9, 10);
        canvas_set_color(canvas, ColorWhite);
    } else {
        canvas_set_color(canvas, ColorBlack);
        canvas_draw_rframe(canvas, x - 1, y - 8, 9, 10, 1);
    }
    // Label: "A" in symbol mode (press to return to letters), "#" in letters mode.
    canvas_draw_glyph(canvas, x, y, symbol_mode ? 'A' : '#');
    canvas_set_color(canvas, ColorBlack);
}

static void ndef_text_input_view_draw_callback(Canvas* canvas, void* _model) {
    NdefTextInputModel* model = _model;
    uint8_t text_length = model->text_buffer ? strlen(model->text_buffer) : 0;
    uint8_t needed_string_width = canvas_width(canvas) - 8;
    uint8_t start_pos = 4;

    const char* text = model->text_buffer;

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    canvas_draw_str(canvas, 2, 8, model->header);
    elements_slightly_rounded_frame(canvas, 1, 12, 126, 15);

    if(canvas_string_width(canvas, text) > needed_string_width) {
        canvas_draw_str(canvas, start_pos, 22, "...");
        start_pos += 6;
        needed_string_width -= 8;
    }

    while(text != 0 && canvas_string_width(canvas, text) > needed_string_width) {
        text++;
    }

    if(model->clear_default_text) {
        elements_slightly_rounded_box(
            canvas, start_pos - 1, 14, canvas_string_width(canvas, text) + 2, 10);
        canvas_set_color(canvas, ColorWhite);
    } else {
        canvas_draw_str(canvas, start_pos + canvas_string_width(canvas, text) + 1, 22, "|");
        canvas_draw_str(canvas, start_pos + canvas_string_width(canvas, text) + 2, 22, "|");
    }
    canvas_draw_str(canvas, start_pos, 22, text);

    canvas_set_font(canvas, FontKeyboard);

    for(uint8_t row = 0; row < keyboard_row_count; row++) {
        const uint8_t column_count = get_row_size(model, row);
        const NdefTextInputKey* keys = get_row(model, row);

        for(size_t column = 0; column < column_count; column++) {
            const uint8_t key_x = keyboard_origin_x + keys[column].x;
            const uint8_t key_y = keyboard_origin_y + keys[column].y;
            const bool selected = model->selected_row == row && model->selected_column == column;

            if(keys[column].text == ENTER_KEY) {
                canvas_set_color(canvas, ColorBlack);
                canvas_draw_icon(
                    canvas, key_x, key_y, selected ? &I_KeySaveSelected_24x11 : &I_KeySave_24x11);
            } else if(keys[column].text == BACKSPACE_KEY) {
                canvas_set_color(canvas, ColorBlack);
                canvas_draw_icon(
                    canvas,
                    key_x,
                    key_y,
                    selected ? &I_KeyBackspaceSelected_16x9 : &I_KeyBackspace_16x9);
            } else if(keys[column].text == SHIFT_KEY) {
                draw_shift_key(canvas, key_x, key_y, selected, model->symbol_mode);
            } else {
                if(selected) {
                    canvas_set_color(canvas, ColorBlack);
                    canvas_draw_box(canvas, key_x - 1, key_y - 8, 7, 10);
                    canvas_set_color(canvas, ColorWhite);
                } else {
                    canvas_set_color(canvas, ColorBlack);
                }

                // Auto-uppercase only applies to lowercase letters at the start
                // of an empty buffer. Symbols are drawn verbatim.
                if((model->clear_default_text ||
                    (text_length == 0 && char_is_lowercase(keys[column].text))) &&
                   !model->symbol_mode) {
                    canvas_draw_glyph(canvas, key_x, key_y, char_to_uppercase(keys[column].text));
                } else {
                    canvas_draw_glyph(canvas, key_x, key_y, keys[column].text);
                }
            }
        }
    }

    if(model->validator_message_visible) {
        canvas_set_font(canvas, FontSecondary);
        canvas_set_color(canvas, ColorWhite);
        canvas_draw_box(canvas, 8, 10, 110, 48);
        canvas_set_color(canvas, ColorBlack);
        canvas_draw_icon(canvas, 10, 14, &I_WarningDolphin_45x42);
        canvas_draw_rframe(canvas, 8, 8, 112, 50, 3);
        canvas_draw_rframe(canvas, 9, 9, 110, 48, 2);
        elements_multiline_text(canvas, 62, 20, furi_string_get_cstr(model->validator_text));
        canvas_set_font(canvas, FontKeyboard);
    }
}

static void ndef_text_input_handle_up(NdefTextInput* text_input, NdefTextInputModel* model) {
    UNUSED(text_input);
    if(model->selected_row > 0) {
        model->selected_row--;
        if(model->selected_column > get_row_size(model, model->selected_row) - 6) {
            model->selected_column = model->selected_column + 1;
        }
    }
}

static void ndef_text_input_handle_down(NdefTextInput* text_input, NdefTextInputModel* model) {
    UNUSED(text_input);
    if(model->selected_row < keyboard_row_count - 1) {
        model->selected_row++;
        if(model->selected_column > get_row_size(model, model->selected_row) - 4) {
            model->selected_column = model->selected_column - 1;
        }
    }
}

static void ndef_text_input_handle_left(NdefTextInput* text_input, NdefTextInputModel* model) {
    UNUSED(text_input);
    if(model->selected_column > 0) {
        model->selected_column--;
    } else {
        model->selected_column = get_row_size(model, model->selected_row) - 1;
    }
}

static void ndef_text_input_handle_right(NdefTextInput* text_input, NdefTextInputModel* model) {
    UNUSED(text_input);
    if(model->selected_column < get_row_size(model, model->selected_row) - 1) {
        model->selected_column++;
    } else {
        model->selected_column = 0;
    }
}

static void
    ndef_text_input_handle_ok(NdefTextInput* text_input, NdefTextInputModel* model, bool shift) {
    char selected = get_selected_char(model);
    size_t text_length = strlen(model->text_buffer);

    // Auto-uppercase logic applies only in letter mode.
    bool toggle_case = !model->symbol_mode && (text_length == 0 || model->clear_default_text);
    if(shift) toggle_case = !toggle_case;
    if(toggle_case) {
        selected = char_to_uppercase(selected);
    }

    if(selected == ENTER_KEY) {
        if(model->validator_callback &&
           (!model->validator_callback(
               model->text_buffer, model->validator_text, model->validator_callback_context))) {
            model->validator_message_visible = true;
            furi_timer_start(text_input->timer, furi_kernel_get_tick_frequency() * 4);
        } else if(model->callback != 0 && text_length >= model->minimum_length) {
            model->callback(model->callback_context);
        }
    } else if(selected == BACKSPACE_KEY) {
        ndef_text_input_backspace_cb(model);
    } else if(selected == SHIFT_KEY) {
        model->symbol_mode = !model->symbol_mode;
        // Clamp selection in case the row sizes change in the future. For now
        // both layouts use identical positions/sizes, so no clamping needed.
    } else {
        if(model->clear_default_text) {
            text_length = 0;
        }
        if(text_length < (model->text_buffer_size - 1)) {
            model->text_buffer[text_length] = selected;
            model->text_buffer[text_length + 1] = 0;
        }
    }
    model->clear_default_text = false;
}

static bool ndef_text_input_view_input_callback(InputEvent* event, void* context) {
    NdefTextInput* text_input = context;
    furi_assert(text_input);

    bool consumed = false;
    NdefTextInputModel* model = view_get_model(text_input->view);

    if((!(event->type == InputTypePress) && !(event->type == InputTypeRelease)) &&
       model->validator_message_visible) {
        model->validator_message_visible = false;
        consumed = true;
    } else if(event->type == InputTypeShort) {
        consumed = true;
        switch(event->key) {
        case InputKeyUp:
            ndef_text_input_handle_up(text_input, model);
            break;
        case InputKeyDown:
            ndef_text_input_handle_down(text_input, model);
            break;
        case InputKeyLeft:
            ndef_text_input_handle_left(text_input, model);
            break;
        case InputKeyRight:
            ndef_text_input_handle_right(text_input, model);
            break;
        case InputKeyOk:
            ndef_text_input_handle_ok(text_input, model, false);
            break;
        default:
            consumed = false;
            break;
        }
    } else if(event->type == InputTypeLong) {
        consumed = true;
        switch(event->key) {
        case InputKeyUp:
            ndef_text_input_handle_up(text_input, model);
            break;
        case InputKeyDown:
            ndef_text_input_handle_down(text_input, model);
            break;
        case InputKeyLeft:
            ndef_text_input_handle_left(text_input, model);
            break;
        case InputKeyRight:
            ndef_text_input_handle_right(text_input, model);
            break;
        case InputKeyOk:
            ndef_text_input_handle_ok(text_input, model, true);
            break;
        case InputKeyBack:
            ndef_text_input_backspace_cb(model);
            break;
        default:
            consumed = false;
            break;
        }
    } else if(event->type == InputTypeRepeat) {
        consumed = true;
        switch(event->key) {
        case InputKeyUp:
            ndef_text_input_handle_up(text_input, model);
            break;
        case InputKeyDown:
            ndef_text_input_handle_down(text_input, model);
            break;
        case InputKeyLeft:
            ndef_text_input_handle_left(text_input, model);
            break;
        case InputKeyRight:
            ndef_text_input_handle_right(text_input, model);
            break;
        case InputKeyBack:
            ndef_text_input_backspace_cb(model);
            break;
        default:
            consumed = false;
            break;
        }
    }

    view_commit_model(text_input->view, consumed);
    return consumed;
}

static void ndef_text_input_timer_callback(void* context) {
    furi_assert(context);
    NdefTextInput* text_input = context;
    with_view_model(
        text_input->view,
        NdefTextInputModel * model,
        { model->validator_message_visible = false; },
        true);
}

NdefTextInput* ndef_text_input_alloc(void) {
    NdefTextInput* text_input = malloc(sizeof(NdefTextInput));
    text_input->view = view_alloc();
    view_set_context(text_input->view, text_input);
    view_allocate_model(text_input->view, ViewModelTypeLocking, sizeof(NdefTextInputModel));
    view_set_draw_callback(text_input->view, ndef_text_input_view_draw_callback);
    view_set_input_callback(text_input->view, ndef_text_input_view_input_callback);

    text_input->timer =
        furi_timer_alloc(ndef_text_input_timer_callback, FuriTimerTypeOnce, text_input);

    with_view_model(
        text_input->view,
        NdefTextInputModel * model,
        { model->validator_text = furi_string_alloc(); },
        false);

    ndef_text_input_reset(text_input);
    return text_input;
}

void ndef_text_input_free(NdefTextInput* text_input) {
    furi_check(text_input);
    with_view_model(
        text_input->view,
        NdefTextInputModel * model,
        { furi_string_free(model->validator_text); },
        false);
    furi_timer_stop(text_input->timer);
    furi_timer_free(text_input->timer);
    view_free(text_input->view);
    free(text_input);
}

void ndef_text_input_reset(NdefTextInput* text_input) {
    furi_check(text_input);
    with_view_model(
        text_input->view,
        NdefTextInputModel * model,
        {
            model->header = "";
            model->selected_row = 0;
            model->selected_column = 0;
            model->minimum_length = 1;
            model->clear_default_text = false;
            model->symbol_mode = false;
            model->text_buffer = NULL;
            model->text_buffer_size = 0;
            model->callback = NULL;
            model->callback_context = NULL;
            model->validator_callback = NULL;
            model->validator_callback_context = NULL;
            furi_string_reset(model->validator_text);
            model->validator_message_visible = false;
        },
        true);
}

View* ndef_text_input_get_view(NdefTextInput* text_input) {
    furi_check(text_input);
    return text_input->view;
}

void ndef_text_input_set_result_callback(
    NdefTextInput* text_input,
    NdefTextInputCallback callback,
    void* callback_context,
    char* text_buffer,
    size_t text_buffer_size,
    bool clear_default_text) {
    furi_check(text_input);
    with_view_model(
        text_input->view,
        NdefTextInputModel * model,
        {
            model->callback = callback;
            model->callback_context = callback_context;
            model->text_buffer = text_buffer;
            model->text_buffer_size = text_buffer_size;
            model->clear_default_text = clear_default_text;
            if(text_buffer && text_buffer[0] != '\0') {
                // Focus on Save (Enter) like the stock widget does.
                model->selected_row = 2;
                model->selected_column = 8;
            }
        },
        true);
}

void ndef_text_input_set_minimum_length(NdefTextInput* text_input, size_t minimum_length) {
    with_view_model(
        text_input->view,
        NdefTextInputModel * model,
        { model->minimum_length = minimum_length; },
        true);
}

void ndef_text_input_set_validator(
    NdefTextInput* text_input,
    NdefTextInputValidatorCallback callback,
    void* callback_context) {
    furi_check(text_input);
    with_view_model(
        text_input->view,
        NdefTextInputModel * model,
        {
            model->validator_callback = callback;
            model->validator_callback_context = callback_context;
        },
        true);
}

NdefTextInputValidatorCallback ndef_text_input_get_validator_callback(NdefTextInput* text_input) {
    furi_check(text_input);
    NdefTextInputValidatorCallback validator_callback = NULL;
    with_view_model(
        text_input->view,
        NdefTextInputModel * model,
        { validator_callback = model->validator_callback; },
        false);
    return validator_callback;
}

void* ndef_text_input_get_validator_callback_context(NdefTextInput* text_input) {
    furi_check(text_input);
    void* validator_callback_context = NULL;
    with_view_model(
        text_input->view,
        NdefTextInputModel * model,
        { validator_callback_context = model->validator_callback_context; },
        false);
    return validator_callback_context;
}

void ndef_text_input_set_header_text(NdefTextInput* text_input, const char* text) {
    furi_check(text_input);
    with_view_model(text_input->view, NdefTextInputModel * model, { model->header = text; }, true);
}
