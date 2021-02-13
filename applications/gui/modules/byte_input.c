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
} ByteInputModel;

#define ENTER_KEY '\r'
#define BACKSPACE_KEY '\b'
#define MAX_DRAWABLE_BYTES 8

static const ByteInputKey keyboard_keys_row_1[] = {
    {'0', 1, 8},
    {'1', 10, 8},
    {'2', 19, 8},
    {'3', 28, 8},
    {'4', 37, 8},
    {'5', 46, 8},
    {'6', 55, 8},
    {'7', 64, 8},
    {BACKSPACE_KEY, 64, 8},
};

static const ByteInputKey keyboard_keys_row_2[] = {
    {'8', 1, 8},
    {'9', 10, 8},
    {'A', 19, 8},
    {'B', 28, 8},
    {'C', 37, 8},
    {'D', 46, 8},
    {'E', 55, 8},
    {'F', 64, 8},
    {ENTER_KEY, 64, 8},
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

static void byte_input_view_draw_callback(Canvas* canvas, void* _model) {
    ByteInputModel* model = _model;

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    canvas_draw_str(canvas, 2, 9, model->header);
    elements_slightly_rounded_frame(canvas, 1, 15, 126, 15);

    canvas_set_font(canvas, FontKeyboard);

    const uint8_t text_start_x = 2;

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

static bool byte_input_view_input_callback(InputEvent* event, void* context) {
    ByteInput* byte_input = context;
    furi_assert(byte_input);
    bool consumed = false;

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
