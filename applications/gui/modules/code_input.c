#include "code_input.h"
#include <gui/elements.h>
#include <furi.h>

#define MAX_CODE_LEN 12

struct CodeInput {
    View* view;
};

typedef struct {
    const char* header;

    InputKey* ext_buffer;
    uint8_t ext_buffer_length;

    InputKey local_buffer[2][MAX_CODE_LEN];
    uint8_t input_length[2];
    uint8_t current;

    CodeInputCallback input_callback;
    CodeChangedCallback changed_callback;
    void* callback_context;

} CodeInputModel;

static const Icon* keys_assets[] = {
    [InputKeyUp] = &I_ButtonUp_7x4,
    [InputKeyDown] = &I_ButtonDown_7x4,
    [InputKeyRight] = &I_ButtonRight_4x7,
    [InputKeyLeft] = &I_ButtonLeft_4x7,
};

/**
 * @brief Draw input sequence
 * 
 * @param canvas 
 * @param buffer 
 * @param length 
 * @param x 
 * @param y 
 * @param active
 */
static void code_input_draw_sequence(
    Canvas* canvas,
    uint8_t* buffer,
    uint8_t length,
    uint8_t x,
    uint8_t y,
    bool active) {
    uint8_t pos_x = x + 8;
    uint8_t pos_y = y + 3;

    if(active) {
        canvas_draw_icon(canvas, x - 4, y + 5, &I_ButtonRightSmall_3x5);
    }

    elements_slightly_rounded_frame(canvas, x, y, 116, 15);

    for(size_t i = 0; i < length; i++) {
        uint8_t offset_y = buffer[i] < 2 ? 2 + (buffer[i] * 2) :
                                           1; // maybe symmetrical assets? :-/
        canvas_draw_icon(canvas, pos_x, pos_y + offset_y, keys_assets[buffer[i]]);
        pos_x += buffer[i] > 1 ? 9 : 11;
    }
}

/**
 * @brief Call input callback
 * 
 * @param model 
 */
static void code_input_call_input_callback(CodeInputModel* model) {
    if(model->input_callback != NULL) {
        model->input_callback(model->callback_context);
    }
}

/**
 * @brief Call changed callback
 * 
 * @param model 
 */
static void code_input_call_changed_callback(CodeInputModel* model) {
    if(model->changed_callback != NULL) {
        model->changed_callback(model->callback_context);
    }
}

/**
 * @brief Handle Back button
 * 
 * @param model 
 */
static bool code_input_handle_back(CodeInputModel* model) {
    if(model->current && !model->input_length[model->current]) {
        --model->current;
        return true;
    }

    if(model->input_length[model->current]) {
        model->input_length[model->current] = 0;
        return true;
    }

    code_input_call_changed_callback(model);
    return false;
}

/**
 * @brief Handle OK button
 * 
 * @param model 
 */
static void code_input_handle_ok(CodeInputModel* model) {
    if(model->current < 1 && model->input_length[model->current]) {
        model->current++;
    } else {
        bool match = false;

        match = (model->input_length[0] > 0 && model->input_length[1] > 0);

        match = (model->input_length[0] == model->input_length[1]);

        if(match) {
            for(size_t i = 0; i < model->input_length[0]; i++) {
                match = model->local_buffer[0][i] == model->local_buffer[1][i];
            }
        }

        if(match) {
            code_input_call_input_callback(model);
        }
    }
}

/**
 * @brief Draw callback
 * 
 * @param canvas 
 * @param _model 
 */
static void code_input_view_draw_callback(Canvas* canvas, void* _model) {
    CodeInputModel* model = _model;

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    canvas_draw_str(canvas, 2, 9, model->header);

    canvas_set_font(canvas, FontSecondary);

    code_input_draw_sequence(
        canvas, model->local_buffer[0], model->input_length[0], 6, 14, !model->current);

    if(model->current) canvas_draw_str(canvas, 2, 39, "Repeat code");

    code_input_draw_sequence(
        canvas, model->local_buffer[1], model->input_length[1], 6, 44, model->current);
}

/**
 * @brief Input callback
 * 
 * @param event 
 * @param context 
 * @return true 
 * @return false 
 */
static bool code_input_view_input_callback(InputEvent* event, void* context) {
    CodeInput* code_input = context;
    furi_assert(code_input);
    bool consumed = false;

    if(event->type == InputTypeShort || event->type == InputTypeRepeat) {
        switch(event->key) {
        case InputKeyBack:
            with_view_model(
                code_input->view, (CodeInputModel * model) {
                    consumed = code_input_handle_back(model);
                    return true;
                });

            break;

        case InputKeyOk:
            with_view_model(
                code_input->view, (CodeInputModel * model) {
                    code_input_handle_ok(model);
                    return true;
                });
            consumed = true;
            break;
        default:

            with_view_model(
                code_input->view, (CodeInputModel * model) {
                    uint8_t at = model->current;
                    uint8_t idx = model->input_length[model->current];

                    model->local_buffer[at][idx] = event->key;
                    model->input_length[at] = CLAMP(idx + 1, model->ext_buffer_length, 0);

                    FURI_LOG_I(
                        "Code Input", "Key: %d At: %d Len: %d", event->key, model->current, idx);
                    return true;
                });
            consumed = true;

            break;
        }
    }

    return consumed;
}

/**
 * @brief Reset all input-related data in model
 * 
 * @param model CodeInputModel
 */
static void code_input_reset_model_input_data(CodeInputModel* model) {
    model->current = 0;
    model->input_length[0] = 0;
    model->input_length[1] = 0;
    model->ext_buffer = NULL;
    model->ext_buffer_length = 0;
}

/** 
 * @brief Allocate and initialize code input. This code input is used to enter codes.
 * 
 * @return CodeInput instance pointer
 */
CodeInput* code_input_alloc() {
    CodeInput* code_input = furi_alloc(sizeof(CodeInput));
    code_input->view = view_alloc();
    view_set_context(code_input->view, code_input);
    view_allocate_model(code_input->view, ViewModelTypeLocking, sizeof(CodeInputModel));
    view_set_draw_callback(code_input->view, code_input_view_draw_callback);
    view_set_input_callback(code_input->view, code_input_view_input_callback);

    with_view_model(
        code_input->view, (CodeInputModel * model) {
            model->header = "";
            model->input_callback = NULL;
            model->changed_callback = NULL;
            model->callback_context = NULL;
            code_input_reset_model_input_data(model);
            return true;
        });

    return code_input;
}

/** 
 * @brief Deinitialize and free code input
 * 
 * @param code_input Code input instance
 */
void code_input_free(CodeInput* code_input) {
    furi_assert(code_input);
    view_free(code_input->view);
    free(code_input);
}

/** 
 * @brief Get code input view
 * 
 * @param code_input code input instance
 * @return View instance that can be used for embedding
 */
View* code_input_get_view(CodeInput* code_input) {
    furi_assert(code_input);
    return code_input->view;
}

/** 
 * @brief Deinitialize and free code input
 * 
 * @param code_input code input instance
 * @param input_callback input callback fn
 * @param changed_callback changed callback fn
 * @param callback_context callback context
 * @param sequence buffer to use
 * @param length buffer length
 */
void code_input_set_result_callback(
    CodeInput* code_input,
    CodeInputCallback input_callback,
    CodeChangedCallback changed_callback,
    void* callback_context,
    uint8_t* buffer,
    uint8_t buffer_length) {
    with_view_model(
        code_input->view, (CodeInputModel * model) {
            code_input_reset_model_input_data(model);
            model->input_callback = input_callback;
            model->changed_callback = changed_callback;
            model->callback_context = callback_context;
            model->ext_buffer = buffer;
            model->ext_buffer_length = buffer_length;
            return true;
        });
}

/**
 * @brief Set code input header text
 * 
 * @param code_input code input instance
 * @param text text to be shown
 */
void code_input_set_header_text(CodeInput* code_input, const char* text) {
    with_view_model(
        code_input->view, (CodeInputModel * model) {
            model->header = text;
            return true;
        });
}
