#include "mag_text_input.h"
#include <gui/elements.h>
#include <assets_icons.h>
#include <furi.h>

struct Mag_TextInput {
    View* view;
    FuriTimer* timer;
};

typedef struct {
    const char text;
    const uint8_t x;
    const uint8_t y;
} Mag_TextInputKey;

typedef struct {
    const char* header;
    char* text_buffer;
    size_t text_buffer_size;
    bool clear_default_text;

    Mag_TextInputCallback callback;
    void* callback_context;

    uint8_t selected_row;
    uint8_t selected_column;

    // Mag_TextInputValidatorCallback validator_callback;
    // void* validator_callback_context;
    // FuriString* validator_text;
    // bool validator_message_visible;
} Mag_TextInputModel;

static const uint8_t keyboard_origin_x = 1;
static const uint8_t keyboard_origin_y = 29;
static const uint8_t keyboard_row_count = 3;

#define ENTER_KEY '\r'
#define BACKSPACE_KEY '\b'

static const Mag_TextInputKey keyboard_keys_row_1[] = {
    {'q', 1, 8},
    {'w', 9, 8},
    {'e', 17, 8},
    {'r', 25, 8},
    {'t', 33, 8},
    {'y', 41, 8},
    {'u', 49, 8},
    {'i', 57, 8},
    {'o', 65, 8},
    {'p', 73, 8},
    {'0', 81, 8},
    {'1', 89, 8},
    {'2', 97, 8},
    {'3', 105, 8},
    {'%', 113, 8},
    {'^', 120, 8},
};

static const Mag_TextInputKey keyboard_keys_row_2[] = {
    {'a', 1, 20},
    {'s', 9, 20},
    {'d', 18, 20},
    {'f', 25, 20},
    {'g', 33, 20},
    {'h', 41, 20},
    {'j', 49, 20},
    {'k', 57, 20},
    {'l', 65, 20},
    {BACKSPACE_KEY, 72, 12},
    {'4', 89, 20},
    {'5', 97, 20},
    {'6', 105, 20},
    {'/', 113, 20},
    {'?', 120, 20},

};

static const Mag_TextInputKey keyboard_keys_row_3[] = {
    {'z', 1, 32},
    {'x', 9, 32},
    {'c', 18, 32},
    {'v', 25, 32},
    {'b', 33, 32},
    {'n', 41, 32},
    {'m', 49, 32},
    {'_', 57, 32},
    {ENTER_KEY, 64, 23},
    {'7', 89, 32},
    {'8', 97, 32},
    {'9', 105, 32},
    {';', 113, 32},
    {'=', 120, 32},
};

static uint8_t get_row_size(uint8_t row_index) {
    uint8_t row_size = 0;

    switch(row_index + 1) {
    case 1:
        row_size = sizeof(keyboard_keys_row_1) / sizeof(Mag_TextInputKey);
        break;
    case 2:
        row_size = sizeof(keyboard_keys_row_2) / sizeof(Mag_TextInputKey);
        break;
    case 3:
        row_size = sizeof(keyboard_keys_row_3) / sizeof(Mag_TextInputKey);
        break;
    }

    return row_size;
}

static const Mag_TextInputKey* get_row(uint8_t row_index) {
    const Mag_TextInputKey* row = NULL;

    switch(row_index + 1) {
    case 1:
        row = keyboard_keys_row_1;
        break;
    case 2:
        row = keyboard_keys_row_2;
        break;
    case 3:
        row = keyboard_keys_row_3;
        break;
    }

    return row;
}

static char get_selected_char(Mag_TextInputModel* model) {
    return get_row(model->selected_row)[model->selected_column].text;
}

static bool char_is_lowercase(char letter) {
    return (letter >= 0x61 && letter <= 0x7A);
}

static char char_to_uppercase(const char letter) {
    if(letter == '_') {
        return 0x20;
    } else if(isalpha(letter)) {
        return (letter - 0x20);
    } else {
        return letter;
    }
}

static void mag_text_input_backspace_cb(Mag_TextInputModel* model) {
    uint8_t text_length = model->clear_default_text ? 1 : strlen(model->text_buffer);
    if(text_length > 0) {
        model->text_buffer[text_length - 1] = 0;
    }
}

static void mag_text_input_view_draw_callback(Canvas* canvas, void* _model) {
    Mag_TextInputModel* model = _model;
    // uint8_t text_length = model->text_buffer ? strlen(model->text_buffer) : 0;
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

    for(uint8_t row = 0; row <= keyboard_row_count; row++) {
        const uint8_t column_count = get_row_size(row);
        const Mag_TextInputKey* keys = get_row(row);

        for(size_t column = 0; column < column_count; column++) {
            if(keys[column].text == ENTER_KEY) {
                canvas_set_color(canvas, ColorBlack);
                if(model->selected_row == row && model->selected_column == column) {
                    canvas_draw_icon(
                        canvas,
                        keyboard_origin_x + keys[column].x,
                        keyboard_origin_y + keys[column].y,
                        &I_KeySaveSelected_24x11);
                } else {
                    canvas_draw_icon(
                        canvas,
                        keyboard_origin_x + keys[column].x,
                        keyboard_origin_y + keys[column].y,
                        &I_KeySave_24x11);
                }
            } else if(keys[column].text == BACKSPACE_KEY) {
                canvas_set_color(canvas, ColorBlack);
                if(model->selected_row == row && model->selected_column == column) {
                    canvas_draw_icon(
                        canvas,
                        keyboard_origin_x + keys[column].x,
                        keyboard_origin_y + keys[column].y,
                        &I_KeyBackspaceSelected_16x9);
                } else {
                    canvas_draw_icon(
                        canvas,
                        keyboard_origin_x + keys[column].x,
                        keyboard_origin_y + keys[column].y,
                        &I_KeyBackspace_16x9);
                }
            } else {
                if(model->selected_row == row && model->selected_column == column) {
                    canvas_set_color(canvas, ColorBlack);
                    canvas_draw_box(
                        canvas,
                        keyboard_origin_x + keys[column].x - 1,
                        keyboard_origin_y + keys[column].y - 8,
                        7,
                        10);
                    canvas_set_color(canvas, ColorWhite);
                } else {
                    canvas_set_color(canvas, ColorBlack);
                }

                if(model->clear_default_text || (char_is_lowercase(keys[column].text))) {
                    canvas_draw_glyph(
                        canvas,
                        keyboard_origin_x + keys[column].x,
                        keyboard_origin_y + keys[column].y,
                        char_to_uppercase(keys[column].text));
                    //keys[column].text);
                } else {
                    canvas_draw_glyph(
                        canvas,
                        keyboard_origin_x + keys[column].x,
                        keyboard_origin_y + keys[column].y,
                        keys[column].text);
                }
            }
        }
    }
    /*if(model->validator_message_visible) {
        canvas_set_font(canvas, FontSecondary);
        canvas_set_color(canvas, ColorWhite);
        canvas_draw_box(canvas, 8, 10, 110, 48);
        canvas_set_color(canvas, ColorBlack);
        canvas_draw_icon(canvas, 10, 14, &I_WarningDolphin_45x42);
        canvas_draw_rframe(canvas, 8, 8, 112, 50, 3);
        canvas_draw_rframe(canvas, 9, 9, 110, 48, 2);
        elements_multiline_text(canvas, 62, 20, furi_string_get_cstr(model->validator_text));
        canvas_set_font(canvas, FontKeyboard);
    }*/
}

static void mag_text_input_handle_up(Mag_TextInput* mag_text_input, Mag_TextInputModel* model) {
    UNUSED(mag_text_input);
    if(model->selected_row > 0) {
        model->selected_row--;
        if(model->selected_column > get_row_size(model->selected_row) - 6) {
            model->selected_column = model->selected_column + 1;
        }
    }
}

static void mag_text_input_handle_down(Mag_TextInput* mag_text_input, Mag_TextInputModel* model) {
    UNUSED(mag_text_input);
    if(model->selected_row < keyboard_row_count - 1) {
        model->selected_row++;
        if(model->selected_column > get_row_size(model->selected_row) - 4) {
            model->selected_column = model->selected_column - 1;
        }
    }
}

static void mag_text_input_handle_left(Mag_TextInput* mag_text_input, Mag_TextInputModel* model) {
    UNUSED(mag_text_input);
    if(model->selected_column > 0) {
        model->selected_column--;
    } else {
        model->selected_column = get_row_size(model->selected_row) - 1;
    }
}

static void mag_text_input_handle_right(Mag_TextInput* mag_text_input, Mag_TextInputModel* model) {
    UNUSED(mag_text_input);
    if(model->selected_column < get_row_size(model->selected_row) - 1) {
        model->selected_column++;
    } else {
        model->selected_column = 0;
    }
}

static void
    mag_text_input_handle_ok(Mag_TextInput* mag_text_input, Mag_TextInputModel* model, bool shift) {
    UNUSED(mag_text_input);

    char selected = get_selected_char(model);
    uint8_t text_length = strlen(model->text_buffer);

    if(shift) {
        selected = char_to_uppercase(selected);
    }

    if(selected == ENTER_KEY) {
        /*if(model->validator_callback &&
           (!model->validator_callback(
               model->text_buffer, model->validator_text, model->validator_callback_context))) {
            model->validator_message_visible = true;
            furi_timer_start(mag_text_input->timer, furi_kernel_get_tick_frequency() * 4);
        } else*/
        if(model->callback != 0 && text_length > 0) {
            model->callback(model->callback_context);
        }
    } else if(selected == BACKSPACE_KEY) {
        mag_text_input_backspace_cb(model);
    } else {
        if(model->clear_default_text) {
            text_length = 0;
        }
        if(text_length < (model->text_buffer_size - 1)) {
            if(char_is_lowercase(selected)) {
                selected = char_to_uppercase(selected);
            }
            model->text_buffer[text_length] = selected;
            model->text_buffer[text_length + 1] = 0;
        }
    }
    model->clear_default_text = false;
}

static bool mag_text_input_view_input_callback(InputEvent* event, void* context) {
    Mag_TextInput* mag_text_input = context;
    furi_assert(mag_text_input);

    bool consumed = false;

    // Acquire model
    Mag_TextInputModel* model = view_get_model(mag_text_input->view);

    /* if((!(event->type == InputTypePress) && !(event->type == InputTypeRelease)) &&
       model->validator_message_visible) {
        model->validator_message_visible = false;
        consumed = true;
    } else*/
    if(event->type == InputTypeShort) {
        consumed = true;
        switch(event->key) {
        case InputKeyUp:
            mag_text_input_handle_up(mag_text_input, model);
            break;
        case InputKeyDown:
            mag_text_input_handle_down(mag_text_input, model);
            break;
        case InputKeyLeft:
            mag_text_input_handle_left(mag_text_input, model);
            break;
        case InputKeyRight:
            mag_text_input_handle_right(mag_text_input, model);
            break;
        case InputKeyOk:
            mag_text_input_handle_ok(mag_text_input, model, false);
            break;
        default:
            consumed = false;
            break;
        }
    } else if(event->type == InputTypeLong) {
        consumed = true;
        switch(event->key) {
        case InputKeyUp:
            mag_text_input_handle_up(mag_text_input, model);
            break;
        case InputKeyDown:
            mag_text_input_handle_down(mag_text_input, model);
            break;
        case InputKeyLeft:
            mag_text_input_handle_left(mag_text_input, model);
            break;
        case InputKeyRight:
            mag_text_input_handle_right(mag_text_input, model);
            break;
        case InputKeyOk:
            mag_text_input_handle_ok(mag_text_input, model, true);
            break;
        case InputKeyBack:
            mag_text_input_backspace_cb(model);
            break;
        default:
            consumed = false;
            break;
        }
    } else if(event->type == InputTypeRepeat) {
        consumed = true;
        switch(event->key) {
        case InputKeyUp:
            mag_text_input_handle_up(mag_text_input, model);
            break;
        case InputKeyDown:
            mag_text_input_handle_down(mag_text_input, model);
            break;
        case InputKeyLeft:
            mag_text_input_handle_left(mag_text_input, model);
            break;
        case InputKeyRight:
            mag_text_input_handle_right(mag_text_input, model);
            break;
        case InputKeyBack:
            mag_text_input_backspace_cb(model);
            break;
        default:
            consumed = false;
            break;
        }
    }

    // Commit model
    view_commit_model(mag_text_input->view, consumed);

    return consumed;
}

void mag_text_input_timer_callback(void* context) {
    furi_assert(context);
    Mag_TextInput* mag_text_input = context;
    UNUSED(mag_text_input);

    /*with_view_model(
        mag_text_input->view,
        Mag_TextInputModel * model,
        { model->validator_message_visible = false; },
        true);*/
}

Mag_TextInput* mag_text_input_alloc() {
    Mag_TextInput* mag_text_input = malloc(sizeof(Mag_TextInput));
    mag_text_input->view = view_alloc();
    view_set_context(mag_text_input->view, mag_text_input);
    view_allocate_model(mag_text_input->view, ViewModelTypeLocking, sizeof(Mag_TextInputModel));
    view_set_draw_callback(mag_text_input->view, mag_text_input_view_draw_callback);
    view_set_input_callback(mag_text_input->view, mag_text_input_view_input_callback);

    mag_text_input->timer =
        furi_timer_alloc(mag_text_input_timer_callback, FuriTimerTypeOnce, mag_text_input);

    /*with_view_model(
        mag_text_input->view,
        Mag_TextInputModel * model,
        { model->validator_text = furi_string_alloc(); },
        false);*/

    mag_text_input_reset(mag_text_input);

    return mag_text_input;
}

void mag_text_input_free(Mag_TextInput* mag_text_input) {
    furi_assert(mag_text_input);
    /*with_view_model(
        mag_text_input->view,
        Mag_TextInputModel * model,
        { furi_string_free(model->validator_text); },
        false);*/

    // Send stop command
    furi_timer_stop(mag_text_input->timer);
    // Release allocated memory
    furi_timer_free(mag_text_input->timer);

    view_free(mag_text_input->view);

    free(mag_text_input);
}

void mag_text_input_reset(Mag_TextInput* mag_text_input) {
    furi_assert(mag_text_input);
    with_view_model(
        mag_text_input->view,
        Mag_TextInputModel * model,
        {
            model->text_buffer_size = 0;
            model->header = "";
            model->selected_row = 0;
            model->selected_column = 0;
            model->clear_default_text = false;
            model->text_buffer = NULL;
            model->text_buffer_size = 0;
            model->callback = NULL;
            model->callback_context = NULL;
            /*model->validator_callback = NULL;
            model->validator_callback_context = NULL;
            furi_string_reset(model->validator_text);
            model->validator_message_visible = false;*/
        },
        true);
}

View* mag_text_input_get_view(Mag_TextInput* mag_text_input) {
    furi_assert(mag_text_input);
    return mag_text_input->view;
}

void mag_text_input_set_result_callback(
    Mag_TextInput* mag_text_input,
    Mag_TextInputCallback callback,
    void* callback_context,
    char* text_buffer,
    size_t text_buffer_size,
    bool clear_default_text) {
    with_view_model(
        mag_text_input->view,
        Mag_TextInputModel * model,
        {
            model->callback = callback;
            model->callback_context = callback_context;
            model->text_buffer = text_buffer;
            model->text_buffer_size = text_buffer_size;
            model->clear_default_text = clear_default_text;
            if(text_buffer && text_buffer[0] != '\0') {
                // Set focus on Save
                model->selected_row = 2;
                model->selected_column = 8;
            }
        },
        true);
}

/* void mag_text_input_set_validator(
    Mag_TextInput* mag_text_input,
    Mag_TextInputValidatorCallback callback,
    void* callback_context) {
    with_view_model(
        mag_text_input->view,
        Mag_TextInputModel * model,
        {
            model->validator_callback = callback;
            model->validator_callback_context = callback_context;
        },
        true);
}

Mag_TextInputValidatorCallback
    mag_text_input_get_validator_callback(Mag_TextInput* mag_text_input) {
    Mag_TextInputValidatorCallback validator_callback = NULL;
    with_view_model(
        mag_text_input->view,
        Mag_TextInputModel * model,
        { validator_callback = model->validator_callback; },
        false);
    return validator_callback;
}

void* mag_text_input_get_validator_callback_context(Mag_TextInput* mag_text_input) {
    void* validator_callback_context = NULL;
    with_view_model(
        mag_text_input->view,
        Mag_TextInputModel * model,
        { validator_callback_context = model->validator_callback_context; },
        false);
    return validator_callback_context;
}*/

void mag_text_input_set_header_text(Mag_TextInput* mag_text_input, const char* text) {
    with_view_model(
        mag_text_input->view, Mag_TextInputModel * model, { model->header = text; }, true);
}
