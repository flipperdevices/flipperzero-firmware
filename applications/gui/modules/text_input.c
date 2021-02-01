#include "text_input.h"
#include <furi.h>

struct TextInput {
    View* view;
};

typedef struct {
    const char text;
    const uint8_t x;
    const uint8_t y;
} TextInputKey;

typedef struct {
    const char* header;
    char* text;
    uint8_t max_text_length;

    TextInputCallback callback;
    void* callback_context;

    uint8_t selected_row;
    uint8_t selected_column;
} TextInputModel;

static const uint8_t keyboard_origin_x = 1;
static const uint8_t keyboard_origin_y = 29;
static const uint8_t keyboard_row_count = 3;

#define ENTER_KEY '\r'
#define BACKSPACE_KEY '\b'

static const TextInputKey keyboard_keys_row_1[] = {
    {'a', 1, 8},
    {'b', 10, 8},
    {'c', 19, 8},
    {'d', 28, 8},
    {'e', 37, 8},
    {'f', 46, 8},
    {'g', 55, 8},
    {'h', 64, 8},
    {'i', 73, 8},
    {'j', 82, 8},
    {'k', 91, 8},
    {'l', 100, 8},
    {'m', 109, 8},
    {'n', 118, 8},
};

static const TextInputKey keyboard_keys_row_2[] = {
    {'o', 1, 19},
    {'p', 10, 19},
    {'q', 19, 19},
    {'r', 28, 19},
    {'s', 37, 19},
    {'t', 46, 19},
    {'u', 55, 19},
    {'v', 64, 19},
    {'w', 73, 19},
    {'x', 82, 19},
    {'y', 91, 19},
    {'z', 100, 19},
    {BACKSPACE_KEY, 110, 12},
};

static const TextInputKey keyboard_keys_row_3[] = {
    {'1', 1, 32},
    {'2', 10, 32},
    {'3', 19, 32},
    {'4', 28, 32},
    {'5', 37, 32},
    {'6', 46, 32},
    {'7', 55, 32},
    {'8', 64, 32},
    {'9', 73, 32},
    {'0', 82, 32},
    {'_', 91, 32},
    {ENTER_KEY, 102, 23},
};

static uint8_t get_row_size(uint8_t row_index) {
    uint8_t row_size = 0;

    switch(row_index + 1) {
    case 1:
        row_size = sizeof(keyboard_keys_row_1) / sizeof(TextInputKey);
        break;
    case 2:
        row_size = sizeof(keyboard_keys_row_2) / sizeof(TextInputKey);
        break;
    case 3:
        row_size = sizeof(keyboard_keys_row_3) / sizeof(TextInputKey);
        break;
    }

    return row_size;
}

static const TextInputKey* get_row(uint8_t row_index) {
    const TextInputKey* row = NULL;

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

static const char get_selected_char(TextInputModel* model) {
    return get_row(model->selected_row)[model->selected_column].text;
}

static const bool char_is_lowercase(char letter) {
    return (letter >= 0x61 && letter <= 0x7A);
}

static const char char_uppercase(const char letter) {
    return (letter - 0x20);
}

static void text_input_view_draw_callback(Canvas* canvas, void* _model) {
    TextInputModel* model = _model;
    uint8_t text_length = strlen(model->text);

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    canvas_draw_str(canvas, 2, 8, model->header);
    canvas_draw_line(canvas, 2, 12, canvas_width(canvas) - 7, 12);
    canvas_draw_line(canvas, 1, 13, 1, 25);
    canvas_draw_line(canvas, canvas_width(canvas) - 6, 13, canvas_width(canvas) - 6, 25);
    canvas_draw_line(canvas, 2, 26, canvas_width(canvas) - 7, 26);

    canvas_draw_str(canvas, 4, 22, model->text);
    canvas_draw_str(canvas, 4 + canvas_string_width(canvas, model->text) + 1, 22, "|");

    canvas_set_font(canvas, FontKeyboard);

    for(uint8_t row = 0; row <= keyboard_row_count; row++) {
        uint8_t volatile column_count = get_row_size(row);
        const TextInputKey* keys = get_row(row);

        for(size_t column = 0; column < column_count; column++) {
            if(keys[column].text == ENTER_KEY) {
                canvas_set_color(canvas, ColorBlack);
                if(model->selected_row == row && model->selected_column == column) {
                    canvas_draw_icon_name(
                        canvas,
                        keyboard_origin_x + keys[column].x,
                        keyboard_origin_y + keys[column].y,
                        I_KeySaveSelected_24x11);
                } else {
                    canvas_draw_icon_name(
                        canvas,
                        keyboard_origin_x + keys[column].x,
                        keyboard_origin_y + keys[column].y,
                        I_KeySave_24x11);
                }
            } else if(keys[column].text == BACKSPACE_KEY) {
                canvas_set_color(canvas, ColorBlack);
                if(model->selected_row == row && model->selected_column == column) {
                    canvas_draw_icon_name(
                        canvas,
                        keyboard_origin_x + keys[column].x,
                        keyboard_origin_y + keys[column].y,
                        I_KeyBackspaceSelected_16x9);
                } else {
                    canvas_draw_icon_name(
                        canvas,
                        keyboard_origin_x + keys[column].x,
                        keyboard_origin_y + keys[column].y,
                        I_KeyBackspace_16x9);
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

                if(text_length == 0 && char_is_lowercase(keys[column].text)) {
                    canvas_draw_glyph(
                        canvas,
                        keyboard_origin_x + keys[column].x,
                        keyboard_origin_y + keys[column].y,
                        char_uppercase(keys[column].text));
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
}

static bool text_input_handle_up(TextInput* text_input) {
    with_view_model(
        text_input->view, (TextInputModel * model) {
            if(model->selected_row > 0) {
                model->selected_row--;
            }
        });
}

static bool text_input_handle_down(TextInput* text_input) {
    with_view_model(
        text_input->view, (TextInputModel * model) {
            if(model->selected_row < keyboard_row_count - 1) {
                model->selected_row++;
                if(model->selected_column > get_row_size(model->selected_row) - 1) {
                    model->selected_column = get_row_size(model->selected_row) - 1;
                }
            }
        });
}

static bool text_input_handle_left(TextInput* text_input) {
    with_view_model(
        text_input->view, (TextInputModel * model) {
            if(model->selected_column > 0) {
                model->selected_column--;
            } else {
                model->selected_column = get_row_size(model->selected_row) - 1;
            }
        });
}

static bool text_input_handle_right(TextInput* text_input) {
    with_view_model(
        text_input->view, (TextInputModel * model) {
            if(model->selected_column < get_row_size(model->selected_row) - 1) {
                model->selected_column++;
            } else {
                model->selected_column = 0;
            }
        });
}

static bool text_input_handle_ok(TextInput* text_input) {
    with_view_model(
        text_input->view, (TextInputModel * model) {
            char selected = get_selected_char(model);
            uint8_t text_length = strlen(model->text);

            if(selected == ENTER_KEY) {
                if(model->callback != 0) {
                    model->callback(model->callback_context, model->text);
                }
            } else if(selected == BACKSPACE_KEY) {
                if(text_length > 0) {
                    model->text[text_length - 1] = 0;
                }
            } else if(text_length < model->max_text_length) {
                if(text_length == 0 && char_is_lowercase(selected)) {
                    selected = char_uppercase(selected);
                }
                model->text[text_length] = selected;
                model->text[text_length + 1] = 0;
            }
        });
}

static bool text_input_view_input_callback(InputEvent* event, void* context) {
    TextInput* text_input = context;
    furi_assert(text_input);
    bool consumed = false;

    if(event->state) {
        switch(event->input) {
        case InputUp:
            text_input_handle_up(text_input);
            consumed = true;
            break;
        case InputDown:
            text_input_handle_down(text_input);
            consumed = true;
            break;
        case InputLeft:
            text_input_handle_left(text_input);
            consumed = true;
            break;
        case InputRight:
            text_input_handle_right(text_input);
            consumed = true;
            break;
        case InputOk:
            text_input_handle_ok(text_input);
            consumed = true;
            break;
        }
    }

    return consumed;
}

TextInput* text_input_alloc() {
    TextInput* text_input = furi_alloc(sizeof(TextInput));
    text_input->view = view_alloc();
    view_set_context(text_input->view, text_input);
    view_allocate_model(text_input->view, ViewModelTypeLocking, sizeof(TextInputModel));
    view_set_draw_callback(text_input->view, text_input_view_draw_callback);
    view_set_input_callback(text_input->view, text_input_view_input_callback);

    with_view_model(
        text_input->view, (TextInputModel * model) {
            model->max_text_length = 0;
            model->header = "";
            model->selected_row = 0;
            model->selected_column = 0;
        });

    return text_input;
}

void text_input_free(TextInput* text_input) {
    furi_assert(text_input);
    view_free(text_input->view);
    free(text_input);
}

View* text_input_get_view(TextInput* text_input) {
    furi_assert(text_input);
    return text_input->view;
}

void text_input_set_result_callback(
    TextInput* text_input,
    TextInputCallback callback,
    void* callback_context,
    char* text,
    uint8_t max_text_length) {
    with_view_model(
        text_input->view, (TextInputModel * model) {
            model->callback = callback;
            model->callback_context = callback_context;
            model->text = text;
            model->max_text_length = max_text_length;
        });
}

void text_input_set_header_text(TextInput* text_input, const char* text) {
    with_view_model(
        text_input->view, (TextInputModel * model) { model->header = text; });
}