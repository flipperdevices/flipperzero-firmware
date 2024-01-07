#include "int_input.h"

#include <gui/elements.h>
#include <furi.h>
#include <assets_icons.h>

/** IntInput type */
struct IntInput {
    View* view;
};

typedef struct {
    const char text;
    const uint8_t x;
    const uint8_t y;
} IntInputKey;

typedef struct {
    const char* header;
    char* text_buffer;
    size_t text_buffer_size;
    bool clear_default_text;
    
    IntInputCallback callback;
    //IntChangedCallback changed_callback;
    void* callback_context;

    int8_t selected_row; 
    uint8_t selected_column;
} IntInputModel;

static const uint8_t keyboard_origin_x = 7;
static const uint8_t keyboard_origin_y = 31;
static const uint8_t keyboard_row_count = 2;
static const uint8_t enter_symbol = '\r';
static const uint8_t backspace_symbol = '\b';
//static const uint8_t max_drawable_digits = 4;

static const IntInputKey keyboard_keys_row_1[] = {
    {'0', 0, 12},
    {'1', 11, 12},
    {'2', 22, 12},
    {'3', 33, 12},
    {'4', 44, 12},
    {backspace_symbol, 103, 4},
};

static const IntInputKey keyboard_keys_row_2[] = {
    {'5', 0, 26},
    {'6', 11, 26},
    {'7', 22, 26},
    {'8', 33, 26},
    {'9', 44, 26},
    {enter_symbol, 95, 17},
};

/** Get row size
 *
 * @param      row_index  Index of row
 *
 * @return     uint8_t Row size
 */
static uint8_t int_input_get_row_size(uint8_t row_index) {
    uint8_t row_size = 0;

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
 * @return     const IntInputKey* Row pointer
 */
static const IntInputKey* int_input_get_row(uint8_t row_index) {
    const IntInputKey* row = NULL;

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
static void int_input_draw_input(Canvas* canvas, IntInputModel* model) {
    //const uint8_t text_x = 8;
    //const uint8_t text_y = 25;
    //const uint8_t text_y2 = 40;
    UNUSED(model);
    
    elements_slightly_rounded_frame(canvas, 6, 14, 116, 15);

    //canvas_draw_icon(canvas, 2, 19, &I_ButtonLeftSmall_3x5);
    //canvas_draw_icon(canvas, 123, 19, &I_ButtonRightSmall_3x5);

    /*for(uint8_t i = model->first_visible_byte;
        i < model->first_visible_byte + MIN(model->bytes_count, max_drawable_bytes);
        i++) {
        uint8_t byte_position = i - model->first_visible_byte;

        if(i == model->selected_byte) {
            canvas_draw_frame(canvas, text_x + byte_position * 14, text_y - 9, 15, 11);
            if(model->selected_row == -2) {
                canvas_draw_icon(
                    canvas, text_x + 6 + byte_position * 14, text_y - 14, &I_arrow_nano_up);
                canvas_draw_icon(
                    canvas, text_x + 6 + byte_position * 14, text_y + 5, &I_arrow_nano_down);
            }

            if(model->selected_high_nibble) {
                canvas_draw_glyph(
                    canvas,
                    text_x + 8 + byte_position * 14,
                    text_y,
                    int_input_get_nibble_text(model->bytes[i], false));
                canvas_draw_box(canvas, text_x + 1 + byte_position * 14, text_y - 8, 7, 9);
                canvas_invert_color(canvas);
                canvas_draw_line(
                    canvas,
                    text_x + 14 + byte_position * 14,
                    text_y - 6,
                    text_x + 14 + byte_position * 14,
                    text_y - 2);
                canvas_draw_glyph(
                    canvas,
                    text_x + 2 + byte_position * 14,
                    text_y,
                    int_input_get_nibble_text(model->bytes[i], true));
                canvas_invert_color(canvas);
            } else {
                canvas_draw_box(canvas, text_x + 7 + byte_position * 14, text_y - 8, 7, 9);
                canvas_draw_glyph(
                    canvas,
                    text_x + 2 + byte_position * 14,
                    text_y,
                    int_input_get_nibble_text(model->bytes[i], true));
                canvas_invert_color(canvas);
                canvas_draw_line(
                    canvas,
                    text_x + byte_position * 14,
                    text_y - 6,
                    text_x + byte_position * 14,
                    text_y - 2);
                canvas_draw_glyph(
                    canvas,
                    text_x + 8 + byte_position * 14,
                    text_y,
                    int_input_get_nibble_text(model->bytes[i], false));
                canvas_invert_color(canvas);
            }
        } else {
            if(model->first_visible_byte > 0 && i == model->first_visible_byte) {
                canvas_draw_icon(
                    canvas,
                    text_x + 2 + byte_position * 14,
                    text_y - 7,
                    &I_More_data_placeholder_5x7);
            } else {
                canvas_draw_glyph(
                    canvas,
                    text_x + 2 + byte_position * 14,
                    text_y,
                    int_input_get_nibble_text(model->bytes[i], true));
            }
            if(model->bytes_count - model->first_visible_byte > max_drawable_bytes &&
               i == model->first_visible_byte + MIN(model->bytes_count, max_drawable_bytes) - 1) {
                canvas_draw_icon(
                    canvas,
                    text_x + 8 + byte_position * 14,
                    text_y - 7,
                    &I_More_data_placeholder_5x7);
            } else {
                canvas_draw_glyph(
                    canvas,
                    text_x + 8 + byte_position * 14,
                    text_y,
                    int_input_get_nibble_text(model->bytes[i], false));
            }
        }
    }*/
}

/** Handle up button
 *
 * @param      model  The model
 */
static void int_input_handle_up(IntInputModel* model) {
    if(model->selected_row > 0) {
        model->selected_row--;
    }
}

/** Handle down button
 *
 * @param      model  The model
 */
static void int_input_handle_down(IntInputModel* model) {
    if(model->selected_row < keyboard_row_count - 1) {
        model->selected_row += 1;
    }
}

/** Handle left button
 *
 * @param      model  The model
 */
static void int_input_handle_left(IntInputModel* model) {
    if(model->selected_column > 0) {
        model->selected_column--;
    } else {
        model->selected_column = int_input_get_row_size(model->selected_row) - 1;
    }
}

/** Handle right button
 *
 * @param      model  The model
 */
static void int_input_handle_right(IntInputModel* model) {
    if(model->selected_column < int_input_get_row_size(model->selected_row) - 1) {
        model->selected_column++;
    } else {
        model->selected_column = 0;
    }
}

/** Handle OK button
 *
 * @param      model  The model
 */
static void int_input_handle_ok(IntInputModel* model) {
    char selected = int_input_get_row(model->selected_row)[model->selected_column].text;
    size_t text_length = strlen(model->text_buffer);
UNUSED(text_length);
    if(selected == enter_symbol) {
    //    int_input_call_input_callback(model);
    } else if(selected == backspace_symbol) {
        //int_input_clear_selected_byte(model);
    } else {
        if (model->clear_default_text) {
            text_length = 0;
        }
        //if(text_length < (model->text_buffer_size - 1)) {
            //model->text_buffer[text_length] = selected;
            //model->text_buffer[text_length + 1] = 0;
            FURI_LOG_D("INT_INPUT", model->text_buffer);
            //FURI_LOG_D("INT_INPUT", "%u", text_length);
            //FURI_LOG_D("INT_INPUT", "%u", model->text_buffer_size);
            FURI_LOG_D("INT_INPUT", "%d", selected);
        //}
    }
    model->clear_default_text = false;
}

/** Draw callback
 *
 * @param      canvas  The canvas
 * @param      _model  The model
 */
static void int_input_view_draw_callback(Canvas* canvas, void* _model) {
    IntInputModel* model = _model;
    uint8_t text_length = model->text_buffer ? strlen(model->text_buffer) : 0;
    UNUSED(text_length);
    //uint8_t needed_string_width = canvas_width(canvas) - 8;
    //uint8_t start_pos = 4;

    //const char* text = model->text_buffer;

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    
    int_input_draw_input(canvas, model);
    
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 9, model->header);
    canvas_set_font(canvas, FontKeyboard);
    // Draw keyboard
    for(uint8_t row = 0; row < keyboard_row_count; row++) {
        const uint8_t column_count = int_input_get_row_size(row);
        const IntInputKey* keys = int_input_get_row(row);

        for(size_t column = 0; column < column_count; column++) {
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
            } else {
                if(model->selected_row == row && model->selected_column == column) {
                    canvas_set_color(canvas, ColorBlack);
                    canvas_draw_box(
                        canvas,
                        keyboard_origin_x + keys[column].x - 3,
                        keyboard_origin_y + keys[column].y - 10,
                        11,
                        13);
                    canvas_set_color(canvas, ColorWhite);
                } else if(
                    model->selected_row == -1 && row == 0 &&
                    model->selected_column == column) {
                    canvas_set_color(canvas, ColorBlack);
                    canvas_draw_frame(
                        canvas,
                        keyboard_origin_x + keys[column].x - 3,
                        keyboard_origin_y + keys[column].y - 10,
                        11,
                        13);
                } else {
                    canvas_set_color(canvas, ColorBlack);
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
static bool int_input_view_input_callback(InputEvent* event, void* context) {
    IntInput* int_input = context;
    furi_assert(int_input);

    bool consumed = false;
    
    // Fetch the model
    IntInputModel* model = view_get_model(int_input->view);

    if(event->type == InputTypeShort || event->type == InputTypeLong || event->type == InputTypeRepeat) {
        consumed = true;
        switch(event->key) {
        case InputKeyLeft:
            int_input_handle_left(model);
            break;
        case InputKeyRight:
            int_input_handle_right(model);
            break;
        case InputKeyUp:
            int_input_handle_up(model);
            break;
        case InputKeyDown:
            int_input_handle_down(model);
            break;
        case InputKeyOk:
            int_input_handle_ok(model);
            break;
        default:
            consumed = false;
            break;
        }
    }

    // commit view
    view_commit_model(int_input->view, consumed);

    return consumed;
}

void int_input_reset(IntInput* int_input) {
    FURI_LOG_D("INT_INPUT", "Resetting Model");
    furi_assert(int_input);
    with_view_model(
        int_input->view,
        IntInputModel * model,
        {
            model->header = "";
            model->selected_row = 0;
            model->selected_column = 0;
            model->clear_default_text = false;
            model->text_buffer = "";
            model->text_buffer_size = 0;
            model->callback = NULL;
            model->callback_context = NULL;
        },
        true);
}


IntInput* int_input_alloc() {
    IntInput* int_input = malloc(sizeof(IntInput));
    int_input->view = view_alloc();
    view_set_context(int_input->view, int_input);
    view_allocate_model(int_input->view, ViewModelTypeLocking, sizeof(IntInputModel));
    view_set_draw_callback(int_input->view, int_input_view_draw_callback);
    view_set_input_callback(int_input->view, int_input_view_input_callback);

    int_input_reset(int_input);

    return int_input;
}

void int_input_free(IntInput* int_input) {
    furi_assert(int_input);
    view_free(int_input->view);
    free(int_input);
}

View* int_input_get_view(IntInput* int_input) {
    furi_assert(int_input);
    return int_input->view;
}

void int_input_set_result_callback(
    IntInput* int_input,
    IntInputCallback callback,
    void* callback_context,
    char* text_buffer,
    size_t text_buffer_size,
    bool clear_default_text) {
    with_view_model(
        int_input->view,
        IntInputModel * model,
        {
            model->callback = callback;
            model->callback_context = callback_context;
            model->text_buffer = text_buffer;
            model->text_buffer_size = text_buffer_size;
            model->clear_default_text = clear_default_text;
        },
        true);
}

void int_input_set_header_text(IntInput* int_input, const char* text) {
    with_view_model(
        int_input->view, 
        IntInputModel * model, 
        { 
            model->header = text; 
        }, 
        true);
}
