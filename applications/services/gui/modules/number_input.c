#include "number_input.h"

#include <gui/elements.h>
#include <furi.h>
#include <assets_icons.h>

/** NumberInput type */
struct NumberInput {
    View* view;
};

typedef struct {
    const char text;
    const size_t x;
    const size_t y;
} NumberInputKey;

typedef struct {
    const char* header;
    FuriString* text_buffer;
    bool clear_default_text;
    int32_t max_value;
    int32_t min_value;
    bool useSign;
    char sign;
    NumberInputCallback callback;
    void* callback_context;
    size_t selected_row;
    size_t selected_column;
} NumberInputModel;

static const size_t keyboard_origin_x = 7;
static const size_t keyboard_origin_y = 31;
static const size_t keyboard_row_count = 2;
static const size_t enter_symbol = '\r';
static const size_t backspace_symbol = '\b';
static const size_t sign_symbol = '-';

static const NumberInputKey keyboard_keys_row_1[] = {
    {'0', 0, 12},
    {'1', 11, 12},
    {'2', 22, 12},
    {'3', 33, 12},
    {'4', 44, 12},
    {backspace_symbol, 103, 4},
};

static const NumberInputKey keyboard_keys_row_2[] = {
    {'5', 0, 26},
    {'6', 11, 26},
    {'7', 22, 26},
    {'8', 33, 26},
    {'9', 44, 26},
    {sign_symbol, 55, 17},
    {enter_symbol, 95, 17},
};

/** Get row size
 *
 * @param      row_index  Index of row
 *
 * @return     size_t Row size
 */
static size_t number_input_get_row_size(size_t row_index) {
    size_t row_size = 0;

    switch(row_index + 1) {
    case 1:
        row_size = COUNT_OF(keyboard_keys_row_1);
        break;
    case 2:
        row_size = COUNT_OF(keyboard_keys_row_2);
        break;
    default:
        furi_crash();
    }

    return row_size;
}

/** Get row pointer
 *
 * @param      row_index  Index of row
 *
 * @return     const NumberInputKey* Row pointer
 */
static const NumberInputKey* number_input_get_row(size_t row_index) {
    const NumberInputKey* row = NULL;

    switch(row_index + 1) {
    case 1:
        row = keyboard_keys_row_1;
        break;
    case 2:
        row = keyboard_keys_row_2;
        break;
    default:
        furi_crash();
    }

    return row;
}

/** Draw input box (common view)
 *
 * @param      canvas  The canvas
 * @param      model   The model
 */
static void number_input_draw_input(Canvas* canvas, NumberInputModel* model) {
    const size_t text_x = 8;
    const size_t text_y = 25;

    elements_slightly_rounded_frame(canvas, 6, 14, 116, 15);

    FuriString* text = model->text_buffer;
    canvas_draw_str(canvas, text_x, text_y, furi_string_get_cstr(text));
}

static void number_input_backspace_cb(NumberInputModel* model) {
    size_t text_length = model->clear_default_text ? 1 :
                                                      furi_string_utf8_length(model->text_buffer);
    if(text_length > 0) {
        furi_string_set_strn(
            model->text_buffer, furi_string_get_cstr(model->text_buffer), text_length - 1);
    }
}

static char* int32_to_string(int32_t value) {
    char* buffer = (char*)malloc(12);
    snprintf(buffer, 12, "%ld", value);
    return buffer;
}

/** Handle up button
 *
 * @param      model  The model
 */
static void number_input_handle_up(NumberInputModel* model) {
    if(model->selected_row > 0) {
        model->selected_row--;
        if(model->selected_column > number_input_get_row_size(model->selected_row) - 1) {
            model->selected_column = number_input_get_row_size(model->selected_row) - 1;
        }
    }
}

/** Handle down button
 *
 * @param      model  The model
 */
static void number_input_handle_down(NumberInputModel* model) {
    if(model->selected_row < keyboard_row_count - 1) {
        if(model->selected_column >= number_input_get_row_size(model->selected_row) - 1) {
            model->selected_column = number_input_get_row_size(model->selected_row + 1) - 1;
        }
        model->selected_row += 1;
    }
    const NumberInputKey* keys = number_input_get_row(model->selected_row);
    if(keys[model->selected_column].text == sign_symbol && !model->useSign) {
        model->selected_column--;
    }
}

/** Handle left button
 *
 * @param      model  The model
 */
static void number_input_handle_left(NumberInputModel* model) {
    if(model->selected_column > 0) {
        model->selected_column--;
    } else {
        model->selected_column = number_input_get_row_size(model->selected_row) - 1;
    }
    const NumberInputKey* keys = number_input_get_row(model->selected_row);
    if(keys[model->selected_column].text == sign_symbol && !model->useSign) {
        model->selected_column--;
    }
}

/** Handle right button
 *
 * @param      model  The model
 */
static void number_input_handle_right(NumberInputModel* model) {
    if(model->selected_column < number_input_get_row_size(model->selected_row) - 1) {
        model->selected_column++;
    } else {
        model->selected_column = 0;
    }
    const NumberInputKey* keys = number_input_get_row(model->selected_row);
    if(keys[model->selected_column].text == sign_symbol && !model->useSign) {
        model->selected_column++;
    }
}

static void prevent_to_large_number(NumberInputModel* model) {
    if(strtol(furi_string_get_cstr(model->text_buffer), NULL, 10) > model->max_value) {
        char* str = int32_to_string(model->max_value);
        furi_string_set_str(model->text_buffer, str);
        free(str);
    }
    // Added in prevent large, as it would block the input of small positive numbers
    if(model->sign == '-' &&
       strtol(furi_string_get_cstr(model->text_buffer), NULL, 10) < model->min_value) {
        char* str = int32_to_string(model->min_value);
        furi_string_set_str(model->text_buffer, str);
        free(str);
    }
}

static void prevent_to_small_number(NumberInputModel* model) {
    if(strtol(furi_string_get_cstr(model->text_buffer), NULL, 10) < model->min_value) {
        char* str = int32_to_string(model->min_value);
        furi_string_set_str(model->text_buffer, str);
        free(str);
    }
}

static void number_input_sign(NumberInputModel* model) {
    if(model->sign == '-') {
        model->sign = '+';
    } else {
        model->sign = '-';
    }
    int32_t number = strtol(furi_string_get_cstr(model->text_buffer), NULL, 10);
    number = number * -1;
    char* str = int32_to_string(number);
    furi_string_set_str(model->text_buffer, str);
    free(str);
    prevent_to_small_number(model);
    prevent_to_large_number(model);
}

/** Handle OK button
 *
 * @param      model  The model
 */
static void number_input_handle_ok(NumberInputModel* model) {
    char selected = number_input_get_row(model->selected_row)[model->selected_column].text;
    char temp_str[2];
    temp_str[0] = selected;
    temp_str[1] = '\0';
    if(selected == enter_symbol) {
        prevent_to_small_number(model);
        prevent_to_large_number(model);
        model->callback(model->callback_context);
    } else if(selected == backspace_symbol) {
        number_input_backspace_cb(model);
    } else if(selected == sign_symbol) {
        number_input_sign(model);
    } else {
        furi_string_cat_str(model->text_buffer, temp_str);
        prevent_to_large_number(model);
    }
    model->clear_default_text = false;
}

/** Draw callback
 *
 * @param      canvas  The canvas
 * @param      _model  The model
 */
static void number_input_view_draw_callback(Canvas* canvas, void* _model) {
    NumberInputModel* model = _model;
    size_t text_length = model->text_buffer ? furi_string_utf8_length(model->text_buffer) : 0;
    UNUSED(text_length);

    canvas_set_color(canvas, ColorBlack);

    number_input_draw_input(canvas, model);

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 9, model->header);
    canvas_set_font(canvas, FontKeyboard);
    // Draw keyboard
    for(size_t row = 0; row < keyboard_row_count; row++) {
        const size_t column_count = number_input_get_row_size(row);
        const NumberInputKey* keys = number_input_get_row(row);

        for(size_t column = 0; column < column_count; column++) {
            if(keys[column].text == sign_symbol && !model->useSign) {
                continue;
            }

            if(keys[column].text == enter_symbol) {
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
            } else if(keys[column].text == backspace_symbol) {
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
            } else if(keys[column].text == sign_symbol) {
                canvas_set_color(canvas, ColorBlack);
                if(model->selected_row == row && model->selected_column == column) {
                    canvas_draw_icon(
                        canvas,
                        keyboard_origin_x + keys[column].x,
                        keyboard_origin_y + keys[column].y,
                        &I_KeySignSelected_21x11);
                } else {
                    canvas_draw_icon(
                        canvas,
                        keyboard_origin_x + keys[column].x,
                        keyboard_origin_y + keys[column].y,
                        &I_KeySign_21x11);
                }
            } else {
                canvas_set_color(canvas, ColorBlack);
                if(model->selected_row == row && model->selected_column == column) {
                    canvas_draw_box(
                        canvas,
                        keyboard_origin_x + keys[column].x - 3,
                        keyboard_origin_y + keys[column].y - 10,
                        11,
                        13);
                    canvas_set_color(canvas, ColorWhite);
                }

                canvas_draw_glyph(
                    canvas,
                    keyboard_origin_x + keys[column].x,
                    keyboard_origin_y + keys[column].y,
                    keys[column].text);
            }
        }
    }
}

/** Input callback
 *
 * @param      event    The event
 * @param      context  The context
 *
 * @return     true
 * @return     false
 */
static bool number_input_view_input_callback(InputEvent* event, void* context) {
    NumberInput* number_input = context;
    furi_assert(number_input);

    bool consumed = false;

    // Fetch the model
    NumberInputModel* model = view_get_model(number_input->view);

    if(event->type == InputTypeShort || event->type == InputTypeLong ||
       event->type == InputTypeRepeat) {
        consumed = true;
        switch(event->key) {
        case InputKeyLeft:
            number_input_handle_left(model);
            break;
        case InputKeyRight:
            number_input_handle_right(model);
            break;
        case InputKeyUp:
            number_input_handle_up(model);
            break;
        case InputKeyDown:
            number_input_handle_down(model);
            break;
        case InputKeyOk:
            number_input_handle_ok(model);
            break;
        case InputKeyBack:
            prevent_to_small_number(model);
            model->callback(model->callback_context);
            break;
        default:
            consumed = false;
            break;
        }
    }

    // commit view
    view_commit_model(number_input->view, consumed);

    return consumed;
}

void number_input_reset(NumberInput* number_input) {
    furi_assert(number_input);
    with_view_model(
        number_input->view,
        NumberInputModel * model,
        {
            model->header = "";
            model->selected_row = 0;
            model->selected_column = 0;
            model->clear_default_text = false;
            model->text_buffer = furi_string_alloc();
            model->callback = NULL;
            model->callback_context = NULL;
            model->max_value = 0;
            model->min_value = 0;
            model->useSign = 0;
            model->sign = '+';
        },
        true);
}

NumberInput* number_input_alloc(void) {
    NumberInput* number_input = malloc(sizeof(NumberInput));
    number_input->view = view_alloc();
    view_set_context(number_input->view, number_input);
    view_allocate_model(number_input->view, ViewModelTypeLocking, sizeof(NumberInputModel));
    view_set_draw_callback(number_input->view, number_input_view_draw_callback);
    view_set_input_callback(number_input->view, number_input_view_input_callback);

    number_input_reset(number_input);

    return number_input;
}

void number_input_free(NumberInput* number_input) {
    furi_assert(number_input);
    with_view_model(
        number_input->view,
        NumberInputModel * model,
        { furi_string_free(model->text_buffer); },
        true);
    view_free(number_input->view);
    free(number_input);
}

View* number_input_get_view(NumberInput* number_input) {
    furi_assert(number_input);
    return number_input->view;
}

void number_input_set_result_callback(
    NumberInput* number_input,
    NumberInputCallback callback,
    void* callback_context,
    FuriString* text_buffer,
    int32_t min_value,
    int32_t max_value,
    bool useSign,
    bool clear_default_text) {
    with_view_model(
        number_input->view,
        NumberInputModel * model,
        {
            model->callback = callback;
            model->callback_context = callback_context;
            model->text_buffer = text_buffer;
            model->clear_default_text = clear_default_text;
            model->min_value = min_value;
            model->max_value = max_value;
            model->useSign = useSign;
            model->sign = '+';
        },
        true);
}

void number_input_set_header_text(NumberInput* number_input, const char* text) {
    with_view_model(
        number_input->view, NumberInputModel * model, { model->header = text; }, true);
}
