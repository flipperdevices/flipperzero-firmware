#include "dtmf_dolphin_bluebox.h"

#include <gui/elements.h>

typedef struct DTMFDolphinBluebox {
    View* view;
    DTMFDolphinBlueboxOkCallback callback;
    void* context;
} DTMFDolphinBluebox;

typedef struct {
    uint8_t index;
} DTMFDolphinBlueboxModel;

static bool dtmf_dolphin_bluebox_process_left(DTMFDolphinBluebox* dtmf_dolphin_bluebox);
static bool dtmf_dolphin_bluebox_process_right(DTMFDolphinBluebox* dtmf_dolphin_bluebox);
static bool dtmf_dolphin_bluebox_process_ok(DTMFDolphinBluebox* dtmf_dolphin_bluebox, InputEvent* event);

static void dtmf_dolphin_bluebox_draw_callback(Canvas* canvas, void* _model) {
    DTMFDolphinBlueboxModel* model = _model;
    UNUSED(model);
    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, 64, 2, AlignCenter, AlignTop, "Bluebox Mode");
    canvas_set_font(canvas, FontSecondary);
    elements_multiline_text_aligned(
        canvas, 64, 16, AlignCenter, AlignTop, "Press < or > to select");
    // elements_multiline_text_aligned(
    //     canvas, 64, 32, AlignCenter, AlignTop, dtmf_dolphin_get_tone_name(model->index, DTMF_DOLPHIN_TONE_BLOCK_BLUEBOX));
}

static bool dtmf_dolphin_bluebox_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    DTMFDolphinBluebox* dtmf_dolphin_bluebox = context;
    bool consumed = false;

    if(event->type == InputTypeShort) {
        if(event->key == InputKeyRight) {
            consumed = dtmf_dolphin_bluebox_process_right(dtmf_dolphin_bluebox);
        } else if(event->key == InputKeyLeft) {
            consumed = dtmf_dolphin_bluebox_process_left(dtmf_dolphin_bluebox);
        }
    } else if(event->key == InputKeyOk) {
        consumed = dtmf_dolphin_bluebox_process_ok(dtmf_dolphin_bluebox, event);
    }

    return consumed;
}

static bool dtmf_dolphin_bluebox_process_left(DTMFDolphinBluebox* dtmf_dolphin_bluebox) {
    with_view_model(
        dtmf_dolphin_bluebox->view, (DTMFDolphinBlueboxModel * model) {
            if(model->index) {
                model->index--;
            }
            return true;
        });
    return true;
}

static bool dtmf_dolphin_bluebox_process_right(DTMFDolphinBluebox* dtmf_dolphin_bluebox) {
    with_view_model(
        dtmf_dolphin_bluebox->view, (DTMFDolphinBlueboxModel * model) {
            if(model->index < DTMF_DOLPHIN_BLUEBOX_TONE_COUNT) {
                model->index++;
            }
            return true;
        });
    return true;
}

static bool dtmf_dolphin_bluebox_process_ok(DTMFDolphinBluebox* dtmf_dolphin_bluebox, InputEvent* event) {
    bool consumed = false;

    with_view_model(
        dtmf_dolphin_bluebox->view, (DTMFDolphinBlueboxModel* model) {
            if(event->type == InputTypePress) {
                if(model->index < DTMF_DOLPHIN_BLUEBOX_TONE_COUNT) {
                    // TODO: Do the thing
                } else {
                    // TODO: Do the thing
                }
                consumed = true;
            } else if(event->type == InputTypeRelease) {
                if(model->index < DTMF_DOLPHIN_BLUEBOX_TONE_COUNT) {
                    // gpio_item_set_pin(Model->pin_idx, false);
                } else {
                    // gpio_item_set_all_pins(false);
                }
                consumed = true;
            }
            dtmf_dolphin_bluebox->callback(event->type, dtmf_dolphin_bluebox->context);
            return true;
        });

    return consumed;
}

DTMFDolphinBluebox* dtmf_dolphin_bluebox_alloc() {
    DTMFDolphinBluebox* dtmf_dolphin_bluebox = malloc(sizeof(DTMFDolphinBluebox));

    dtmf_dolphin_bluebox->view = view_alloc();
    view_allocate_model(dtmf_dolphin_bluebox->view, ViewModelTypeLocking, sizeof(DTMFDolphinBlueboxModel));
    view_set_context(dtmf_dolphin_bluebox->view, dtmf_dolphin_bluebox);
    view_set_draw_callback(dtmf_dolphin_bluebox->view, dtmf_dolphin_bluebox_draw_callback);
    view_set_input_callback(dtmf_dolphin_bluebox->view, dtmf_dolphin_bluebox_input_callback);

    return dtmf_dolphin_bluebox;
}

void dtmf_dolphin_bluebox_free(DTMFDolphinBluebox* dtmf_dolphin_bluebox) {
    furi_assert(dtmf_dolphin_bluebox);
    view_free(dtmf_dolphin_bluebox->view);
    free(dtmf_dolphin_bluebox);
}

View* dtmf_dolphin_bluebox_get_view(DTMFDolphinBluebox* dtmf_dolphin_bluebox) {
    furi_assert(dtmf_dolphin_bluebox);
    return dtmf_dolphin_bluebox->view;
}

void dtmf_dolphin_bluebox_set_ok_callback(DTMFDolphinBluebox* dtmf_dolphin_bluebox, DTMFDolphinBlueboxOkCallback callback, void* context) {
    furi_assert(dtmf_dolphin_bluebox);
    furi_assert(callback);
    with_view_model(
        dtmf_dolphin_bluebox->view, (DTMFDolphinBlueboxModel * model) {
            UNUSED(model);
            dtmf_dolphin_bluebox->callback = callback;
            dtmf_dolphin_bluebox->context = context;
            return false;
        });
}
