#include "byte_input.h"
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
} ByteInputModel;

static void byte_input_view_draw_callback(Canvas* canvas, void* _model) {
    ByteInputModel* model = _model;
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
