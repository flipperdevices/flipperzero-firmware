#include "byte_input.h"
#include <gui/elements.h>
#include <furi.h>

struct ByteInput {
    View* view;
};

typedef struct {
    const uint8_t value;
    const uint8_t x;
    const uint8_t y;
} ByteInputKey;

typedef struct {
    const char* header;
    uint8_t* bytes;
    uint8_t bytes_count;

    ByteInputCallback input_callback;
    ByteChangedCallback changed_callback;
    void* callback_context;

    bool selected_high_nibble;
    uint8_t selected_byte;
    int8_t selected_row;
    uint8_t selected_column;
} ByteInputModel;

#define ENTER_KEY '\r'
#define BACKSPACE_KEY '\b'
#define MAX_DRAWABLE_BYTES 8
static const uint8_t keyboard_origin_x = 7;
static const uint8_t keyboard_origin_y = 32;
static const uint8_t keyboard_row_count = 2;

static const ByteInputKey keyboard_keys_row_1[] = {
    {'0', 0, 12},
    {'1', 12, 12},
    {'2', 24, 12},
    {'3', 36, 12},
    {'4', 48, 12},
    {'5', 60, 12},
    {'6', 72, 12},
    {'7', 84, 12},
    {BACKSPACE_KEY, 103, 4},
};

static const ByteInputKey keyboard_keys_row_2[] = {
    {'8', 0, 25},
    {'9', 12, 25},
    {'A', 24, 25},
    {'B', 36, 25},
    {'C', 48, 25},
    {'D', 60, 25},
    {'E', 72, 25},
    {'F', 84, 25},
    {ENTER_KEY, 95, 16},
};

static uint8_t get_row_size(uint8_t row_index) {
    uint8_t row_size = 0;

    switch(row_index + 1) {
    case 1:
        row_size = sizeof(keyboard_keys_row_1) / sizeof(ByteInputKey);
        break;
    case 2:
        row_size = sizeof(keyboard_keys_row_2) / sizeof(ByteInputKey);
        break;
    }

    return row_size;
}

static const ByteInputKey* get_row(uint8_t row_index) {
    const ByteInputKey* row = NULL;

    switch(row_index + 1) {
    case 1:
        row = keyboard_keys_row_1;
        break;
    case 2:
        row = keyboard_keys_row_2;
        break;
    }

    return row;
}

static char get_nibble_text(uint8_t byte, bool high_nibble) {
    if(high_nibble) {
        byte = byte >> 4;
    }
    byte = byte & 0x0F;

    switch(byte & 0x0F) {
    case 0x0:
    case 0x1:
    case 0x2:
    case 0x3:
    case 0x4:
    case 0x5:
    case 0x6:
    case 0x7:
    case 0x8:
    case 0x9:
        byte = byte + '0';
        break;
    case 0xA:
    case 0xB:
    case 0xC:
    case 0xD:
    case 0xE:
    case 0xF:
        byte = byte - 0xA + 'A';
        break;
    default:
        byte = '!';
        break;
    }

    return byte;
}

static void set_nibble(uint8_t* data, uint8_t position, char value, bool high_nibble) {
    switch(value) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        value = value - '0';
        break;
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
        value = value - 'A' + 10;
        break;
    default:
        value = 0;
        break;
    }

    if(high_nibble) {
        data[position] &= 0x0F;
        data[position] |= value << 4;
    } else {
        data[position] &= 0xF0;
        data[position] |= value;
    }
}

static void byte_input_draw_input(Canvas* canvas, ByteInputModel* model) {
    const uint8_t text_start_x = 2;
    elements_slightly_rounded_frame(canvas, 1, 15, 126, 15);

    for(uint8_t i = 0; i < model->bytes_count; i++) {
        if(i == model->selected_byte) {
            canvas_draw_frame(canvas, text_start_x + i * 14, 17, 15, 11);

            if(model->selected_high_nibble) {
                canvas_draw_glyph(
                    canvas, text_start_x + 8 + i * 14, 26, get_nibble_text(model->bytes[i], false));
                canvas_draw_box(canvas, text_start_x + 1 + i * 14, 18, 7, 9);
                canvas_invert_color(canvas);
                canvas_draw_line(canvas, text_start_x + 14 + i * 14, 20, 16 + i * 14, 24);
                canvas_draw_glyph(
                    canvas, text_start_x + 2 + i * 14, 26, get_nibble_text(model->bytes[i], true));
                canvas_invert_color(canvas);
            } else {
                canvas_draw_box(canvas, text_start_x + 7 + i * 14, 18, 7, 9);
                canvas_draw_glyph(
                    canvas, text_start_x + 2 + i * 14, 26, get_nibble_text(model->bytes[i], true));
                canvas_invert_color(canvas);
                canvas_draw_line(canvas, text_start_x + i * 14, 20, 2 + i * 14, 24);
                canvas_draw_glyph(
                    canvas, text_start_x + 8 + i * 14, 26, get_nibble_text(model->bytes[i], false));
                canvas_invert_color(canvas);
            }
        } else {
            canvas_draw_glyph(canvas, 4 + i * 14, 26, get_nibble_text(model->bytes[i], true));
            canvas_draw_glyph(canvas, 10 + i * 14, 26, get_nibble_text(model->bytes[i], false));
        }
    }
}

static void byte_input_draw_input_selected(Canvas* canvas, ByteInputModel* model) {
    const uint8_t text_start_x = 2;
    canvas_draw_box(canvas, 0, 13, 128, 19);
    canvas_invert_color(canvas);
    elements_slightly_rounded_frame(canvas, 1, 15, 126, 15);

    for(uint8_t i = 0; i < model->bytes_count; i++) {
        if(i == model->selected_byte) {
            canvas_draw_box(canvas, text_start_x + i * 14, 17, 15, 11);
            canvas_invert_color(canvas);
            canvas_draw_glyph(canvas, 4 + i * 14, 26, get_nibble_text(model->bytes[i], true));
            canvas_draw_glyph(canvas, 10 + i * 14, 26, get_nibble_text(model->bytes[i], false));
            canvas_invert_color(canvas);
        } else {
            canvas_draw_glyph(canvas, 4 + i * 14, 26, get_nibble_text(model->bytes[i], true));
            canvas_draw_glyph(canvas, 10 + i * 14, 26, get_nibble_text(model->bytes[i], false));
        }
    }

    canvas_invert_color(canvas);
}

static void byte_input_view_draw_callback(Canvas* canvas, void* _model) {
    ByteInputModel* model = _model;

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    canvas_draw_str(canvas, 2, 9, model->header);

    canvas_set_font(canvas, FontKeyboard);

    if(model->selected_row == -1) {
        byte_input_draw_input_selected(canvas, model);
    } else {
        byte_input_draw_input(canvas, model);
    }

    for(uint8_t row = 0; row < keyboard_row_count; row++) {
        const uint8_t column_count = get_row_size(row);
        const ByteInputKey* keys = get_row(row);

        for(size_t column = 0; column < column_count; column++) {
            if(keys[column].value == ENTER_KEY) {
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
            } else if(keys[column].value == BACKSPACE_KEY) {
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
                        keyboard_origin_x + keys[column].x - 3,
                        keyboard_origin_y + keys[column].y - 10,
                        11,
                        13);
                    canvas_set_color(canvas, ColorWhite);
                } else if(model->selected_row == -1 && row == 0 && model->selected_column == column) {
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
                    keys[column].value);
            }
        }
    }
}

static bool byte_input_view_input_callback(InputEvent* event, void* context) {
    ByteInput* byte_input = context;
    furi_assert(byte_input);
    bool consumed = false;

    if(event->type == InputTypeShort) {
        switch(event->key) {
        case InputKeyLeft:
            with_view_model(
                byte_input->view, (ByteInputModel * model) {
                    if(model->selected_row == -1) {
                        model->selected_byte -= 1;
                    } else {
                        model->selected_column -= 1;
                    }

                    return true;
                });
            consumed = true;
            break;
        case InputKeyRight:
            with_view_model(
                byte_input->view, (ByteInputModel * model) {
                    if(model->selected_row == -1) {
                        model->selected_byte += 1;
                    } else {
                        model->selected_column += 1;
                    }
                    return true;
                });
            consumed = true;
            break;
        case InputKeyUp:
            with_view_model(
                byte_input->view, (ByteInputModel * model) {
                    model->selected_row -= 1;
                    return true;
                });
            consumed = true;
            break;
        case InputKeyDown:
            with_view_model(
                byte_input->view, (ByteInputModel * model) {
                    model->selected_row += 1;
                    return true;
                });
            consumed = true;
            break;
        case InputKeyOk:
            with_view_model(
                byte_input->view, (ByteInputModel * model) {
                    char value = get_row(model->selected_row)[model->selected_column].value;
                    set_nibble(
                        model->bytes, model->selected_byte, value, model->selected_high_nibble);
                    if(model->selected_high_nibble == true) {
                        model->selected_high_nibble = false;
                    } else {
                        model->selected_byte++;
                        model->selected_high_nibble = true;
                    }
                    return true;
                });
            consumed = true;
            break;
        default:
            break;
        }
    }

    return consumed;
}

ByteInput* byte_input_alloc() {
    ByteInput* byte_input = furi_alloc(sizeof(ByteInput));
    byte_input->view = view_alloc();
    view_set_context(byte_input->view, byte_input);
    view_allocate_model(byte_input->view, ViewModelTypeLocking, sizeof(ByteInputModel));
    view_set_draw_callback(byte_input->view, byte_input_view_draw_callback);
    view_set_input_callback(byte_input->view, byte_input_view_input_callback);

    with_view_model(
        byte_input->view, (ByteInputModel * model) {
            model->header = "";
            model->bytes = NULL;
            model->bytes_count = 0;
            model->input_callback = NULL;
            model->changed_callback = NULL;
            model->callback_context = NULL;
            model->selected_high_nibble = true;
            model->selected_byte = 0;
            return true;
        });

    return byte_input;
}

void byte_input_free(ByteInput* byte_input) {
    furi_assert(byte_input);
    view_free(byte_input->view);
    free(byte_input);
}

View* byte_input_get_view(ByteInput* byte_input) {
    furi_assert(byte_input);
    return byte_input->view;
}

void byte_input_set_result_callback(
    ByteInput* byte_input,
    ByteInputCallback input_callback,
    ByteChangedCallback changed_callback,
    void* callback_context,
    uint8_t* bytes,
    uint8_t bytes_count) {
    with_view_model(
        byte_input->view, (ByteInputModel * model) {
            model->input_callback = input_callback;
            model->changed_callback = changed_callback;
            model->callback_context = callback_context;
            model->bytes = bytes;
            model->bytes_count = bytes_count;
            model->selected_high_nibble = true;
            model->selected_byte = 0;
            return true;
        });
}

void byte_input_set_header_text(ByteInput* byte_input, const char* text) {
    with_view_model(
        byte_input->view, (ByteInputModel * model) {
            model->header = text;
            return true;
        });
}
