#include "dtmf_dolphin_dialer.h"

#include <gui/elements.h>

typedef struct DTMFDolphinDialer {
    View* view;
    DTMFDolphinDialerOkCallback callback;
    void* context;
} DTMFDolphinDialer;

typedef struct {
    uint8_t row;
    uint8_t col;
    float *freq;
} DTMFDolphinDialerModel;

static bool dtmf_dolphin_dialer_process_up(DTMFDolphinDialer* dtmf_dolphin_dialer);
static bool dtmf_dolphin_dialer_process_down(DTMFDolphinDialer* dtmf_dolphin_dialer);
static bool dtmf_dolphin_dialer_process_left(DTMFDolphinDialer* dtmf_dolphin_dialer);
static bool dtmf_dolphin_dialer_process_right(DTMFDolphinDialer* dtmf_dolphin_dialer);
static bool dtmf_dolphin_dialer_process_ok(DTMFDolphinDialer* dtmf_dolphin_dialer, InputEvent* event);

void draw_button(Canvas* canvas, uint8_t row, uint8_t col, bool invert) {

    uint8_t left = DTMF_DOLPHIN_NUMPAD_X + \
        // ((col + 1) * DTMF_DOLPHIN_BUTTON_PADDING) + 
        (col * DTMF_DOLPHIN_BUTTON_WIDTH);
        // (col * DTMF_DOLPHIN_BUTTON_PADDING);
    uint8_t top = DTMF_DOLPHIN_NUMPAD_Y + \
        // ((row + 1) * DTMF_DOLPHIN_BUTTON_PADDING) + 
        (row * DTMF_DOLPHIN_BUTTON_HEIGHT);
        // (row * DTMF_DOLPHIN_BUTTON_PADDING);

    uint8_t span = dtmf_dolphin_get_tone_span(row, col, DTMF_DOLPHIN_TONE_BLOCK_DIALER);

    canvas_set_color(canvas, ColorBlack);
    
    if (invert)
        canvas_draw_rbox(canvas, left, top,
            (DTMF_DOLPHIN_BUTTON_WIDTH * span) - (DTMF_DOLPHIN_BUTTON_PADDING * 2),
            DTMF_DOLPHIN_BUTTON_HEIGHT - (DTMF_DOLPHIN_BUTTON_PADDING * 2),
            2);
    else
        canvas_draw_rframe(canvas, left, top,
            (DTMF_DOLPHIN_BUTTON_WIDTH * span) - (DTMF_DOLPHIN_BUTTON_PADDING * 2),
            DTMF_DOLPHIN_BUTTON_HEIGHT- (DTMF_DOLPHIN_BUTTON_PADDING * 2),
            2);

    if (invert)
        canvas_invert_color(canvas);


    canvas_set_font(canvas, FontSecondary);
    // canvas_set_color(canvas, invert ? ColorWhite : ColorBlack);
    canvas_draw_str_aligned(canvas,
        left - 1 + (int) ((DTMF_DOLPHIN_BUTTON_WIDTH * span) / 2),
        top + (int) (DTMF_DOLPHIN_BUTTON_HEIGHT / 2),
        AlignCenter,
        AlignCenter,
        dtmf_dolphin_get_tone_name(row, col, DTMF_DOLPHIN_TONE_BLOCK_DIALER));

    if (invert)
        canvas_invert_color(canvas);
}

void draw_dialer(Canvas* canvas, void* _model) {
    DTMFDolphinDialerModel* model = _model;
    uint8_t max_rows;
    uint8_t max_cols;
    uint8_t max_span;
    dtmf_dolphin_tone_get_max_pos(&max_rows, &max_cols, &max_span, DTMF_DOLPHIN_TONE_BLOCK_DIALER);

    canvas_set_font(canvas, FontSecondary);

    for (int r = 0; r < max_rows; r++) {
        for (int c = 0; c < max_cols; c++) {
            if (model->row == r && model->col == c)
                draw_button(canvas, r, c, true);
            else
                draw_button(canvas, r, c, false);
        }
    }
}

void update_frequencies(DTMFDolphinDialerModel *model) {
    dtmf_dolphin_get_tone_frequencies(model->freq, model->row, model->col, DTMF_DOLPHIN_TONE_BLOCK_DIALER);
}

static void dtmf_dolphin_dialer_draw_callback(Canvas* canvas, void* _model) {
    DTMFDolphinDialerModel* model = _model;
    uint8_t max_rows;
    uint8_t max_cols;
    uint8_t max_span;
    dtmf_dolphin_tone_get_max_pos(&max_rows, &max_cols, &max_span, DTMF_DOLPHIN_TONE_BLOCK_DIALER);

    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text(canvas, 2, 10, "Dialer");
    canvas_draw_line(canvas,
        (max_span * DTMF_DOLPHIN_BUTTON_WIDTH) + 1, 0,
        (max_span * DTMF_DOLPHIN_BUTTON_WIDTH) + 1, canvas_height(canvas));
    elements_multiline_text(canvas, (max_span * DTMF_DOLPHIN_BUTTON_WIDTH) + 4, 10, "Detail");
    canvas_draw_line(canvas, 0, DTMF_DOLPHIN_NUMPAD_Y - 3, canvas_width(canvas), DTMF_DOLPHIN_NUMPAD_Y - 3);
    // elements_multiline_text_aligned(canvas, 64, 2, AlignCenter, AlignTop, "Dialer Mode");

    draw_dialer(canvas, model);

    string_t output;
    string_init(output);

    string_cat_printf(
        output,
        "F1: %u Hz\nF2: %u Hz",
        model->freq[0] ? (unsigned int) model->freq[0] : 0,
        model->freq[1] ? (unsigned int) model->freq[1] : 0);

    canvas_set_font(canvas, FontSecondary);
    canvas_set_color(canvas, ColorBlack);
    elements_multiline_text(canvas, (max_span * DTMF_DOLPHIN_BUTTON_WIDTH) + 4, 21, string_get_cstr(output));

    string_clear(output);
}

static bool dtmf_dolphin_dialer_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    DTMFDolphinDialer* dtmf_dolphin_dialer = context;
    bool consumed = false;

    if(event->type == InputTypeShort) {
        if(event->key == InputKeyRight) {
            consumed = dtmf_dolphin_dialer_process_right(dtmf_dolphin_dialer);
        } else if(event->key == InputKeyLeft) {
            consumed = dtmf_dolphin_dialer_process_left(dtmf_dolphin_dialer);
        } else if(event->key == InputKeyUp) {
            consumed = dtmf_dolphin_dialer_process_up(dtmf_dolphin_dialer);
        } else if(event->key == InputKeyDown) {
            consumed = dtmf_dolphin_dialer_process_down(dtmf_dolphin_dialer);
        }

    } else if(event->key == InputKeyOk) {
        consumed = dtmf_dolphin_dialer_process_ok(dtmf_dolphin_dialer, event);
    }

    return consumed;
}

static bool dtmf_dolphin_dialer_process_up(DTMFDolphinDialer* dtmf_dolphin_dialer) {
    with_view_model(
        dtmf_dolphin_dialer->view, (DTMFDolphinDialerModel * model) {
            if(model->row > 0) {
                model->row--;
                update_frequencies(model);
            }
            return true;
        });
    return true;
}

static bool dtmf_dolphin_dialer_process_down(DTMFDolphinDialer* dtmf_dolphin_dialer) {
    uint8_t max_rows = 0;
    uint8_t max_cols = 0;
    uint8_t max_span = 0;
    dtmf_dolphin_tone_get_max_pos(&max_rows, &max_cols, &max_span, DTMF_DOLPHIN_TONE_BLOCK_DIALER);

    with_view_model(
        dtmf_dolphin_dialer->view, (DTMFDolphinDialerModel * model) {
            if(model->row < max_rows - 1) {
                model->row++;
                update_frequencies(model);
            }
            return true;
        });
    return true;
}

static bool dtmf_dolphin_dialer_process_left(DTMFDolphinDialer* dtmf_dolphin_dialer) {
    with_view_model(
        dtmf_dolphin_dialer->view, (DTMFDolphinDialerModel * model) {
            if(model->col > 0) {
                model->col--;
                update_frequencies(model);
            }
            return true;
        });
    return true;
}

static bool dtmf_dolphin_dialer_process_right(DTMFDolphinDialer* dtmf_dolphin_dialer) {
    uint8_t max_rows = 0;
    uint8_t max_cols = 0;
    uint8_t max_span = 0;
    dtmf_dolphin_tone_get_max_pos(&max_rows, &max_cols, &max_span, DTMF_DOLPHIN_TONE_BLOCK_DIALER);

    with_view_model(
        dtmf_dolphin_dialer->view, (DTMFDolphinDialerModel * model) {
            if(model->col < max_cols - 1) {
                model->col++;
                update_frequencies(model);
            }
            return true;
        });
    return true;
}

static bool dtmf_dolphin_dialer_process_ok(DTMFDolphinDialer* dtmf_dolphin_dialer, InputEvent* event) {
    bool consumed = false;

    with_view_model(
        dtmf_dolphin_dialer->view, (DTMFDolphinDialerModel * model) {
            if (event->type == InputTypePress) {
                dtmf_dolphin_player_play_tones(model->freq);
            } else if (event->type == InputTypeRelease) {
                dtmf_dolphin_player_stop_tones();
            }

            return true;
        });

    return consumed;
}

DTMFDolphinDialer* dtmf_dolphin_dialer_alloc() {
    DTMFDolphinDialer* dtmf_dolphin_dialer = malloc(sizeof(DTMFDolphinDialer));

    dtmf_dolphin_dialer->view = view_alloc();
    view_allocate_model(dtmf_dolphin_dialer->view, ViewModelTypeLocking, sizeof(DTMFDolphinDialerModel));

    with_view_model(
        dtmf_dolphin_dialer->view, (DTMFDolphinDialerModel * model) {
            model->col = 0;
            model->row = 0;
            model->freq = malloc(sizeof(float) * 2);
            update_frequencies(model);
            return true;
        }
    );

    view_set_context(dtmf_dolphin_dialer->view, dtmf_dolphin_dialer);
    view_set_draw_callback(dtmf_dolphin_dialer->view, dtmf_dolphin_dialer_draw_callback);
    view_set_input_callback(dtmf_dolphin_dialer->view, dtmf_dolphin_dialer_input_callback);

    return dtmf_dolphin_dialer;
}

void dtmf_dolphin_dialer_free(DTMFDolphinDialer* dtmf_dolphin_dialer) {
    furi_assert(dtmf_dolphin_dialer);
    with_view_model(
        dtmf_dolphin_dialer->view, (DTMFDolphinDialerModel * model) {
            free(model->freq);
            return true;
        }
    );
    view_free(dtmf_dolphin_dialer->view);
    free(dtmf_dolphin_dialer);
}

View* dtmf_dolphin_dialer_get_view(DTMFDolphinDialer* dtmf_dolphin_dialer) {
    furi_assert(dtmf_dolphin_dialer);
    return dtmf_dolphin_dialer->view;
}

// void dtmf_dolphin_dialer_set_ok_callback(DTMFDolphinDialer* dtmf_dolphin_dialer, DTMFDolphinDialerOkCallback callback, void* context) {
//     furi_assert(dtmf_dolphin_dialer);
//     furi_assert(callback);
//     with_view_model(
//         dtmf_dolphin_dialer->view, (DTMFDolphinDialerModel * model) {
//             UNUSED(model);
//             dtmf_dolphin_dialer->callback = callback;
//             dtmf_dolphin_dialer->context = context;
//             return false;
//         });
// }
