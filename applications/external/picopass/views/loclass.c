#include "loclass.h"

#include <picopass_device.h>

#include <gui/elements.h>

struct Loclass {
    View* view;
    LoclassCallback callback;
    void* context;
};

typedef struct {
    FuriString* header;
    uint8_t num_macs;
} LoclassViewModel;

static void loclass_draw_callback(Canvas* canvas, void* model) {
    LoclassViewModel* m = model;

    char draw_str[32] = {};
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, furi_string_get_cstr(m->header));

    if(m->num_macs == 255) {
        return;
    }

    float progress = m->num_macs == 0 ? 0 :
                                        (float)(m->num_macs) / (float)(LOCLASS_MACS_TO_COLLECT);

    if(progress > 1.0) {
        progress = 1.0;
    }

    snprintf(draw_str, sizeof(draw_str), "%d/%d", m->num_macs, LOCLASS_MACS_TO_COLLECT);

    elements_progress_bar_with_text(canvas, 0, 20, 128, progress, draw_str);

    elements_button_center(canvas, "Skip");
}

static bool loclass_input_callback(InputEvent* event, void* context) {
    Loclass* loclass = context;
    bool consumed = false;
    if(event->type == InputTypeShort && event->key == InputKeyOk) {
        if(loclass->callback) {
            loclass->callback(loclass->context);
        }
        consumed = true;
    }
    return consumed;
}

Loclass* loclass_alloc() {
    Loclass* loclass = malloc(sizeof(Loclass));
    loclass->view = view_alloc();
    view_allocate_model(loclass->view, ViewModelTypeLocking, sizeof(LoclassViewModel));
    view_set_draw_callback(loclass->view, loclass_draw_callback);
    view_set_input_callback(loclass->view, loclass_input_callback);
    view_set_context(loclass->view, loclass);
    with_view_model(
        loclass->view, LoclassViewModel * model, { model->header = furi_string_alloc(); }, false);
    return loclass;
}

void loclass_free(Loclass* loclass) {
    furi_assert(loclass);
    with_view_model(
        loclass->view, LoclassViewModel * model, { furi_string_free(model->header); }, false);
    view_free(loclass->view);
    free(loclass);
}

void loclass_reset(Loclass* loclass) {
    furi_assert(loclass);
    with_view_model(
        loclass->view,
        LoclassViewModel * model,
        {
            model->num_macs = 0;
            furi_string_reset(model->header);
        },
        false);
}

View* loclass_get_view(Loclass* loclass) {
    furi_assert(loclass);
    return loclass->view;
}

void loclass_set_callback(Loclass* loclass, LoclassCallback callback, void* context) {
    furi_assert(loclass);
    furi_assert(callback);
    loclass->callback = callback;
    loclass->context = context;
}

void loclass_set_header(Loclass* loclass, const char* header) {
    furi_assert(loclass);
    furi_assert(header);

    with_view_model(
        loclass->view, LoclassViewModel * model, { furi_string_set(model->header, header); }, true);
}

void loclass_set_num_macs(Loclass* loclass, uint16_t num_macs) {
    furi_assert(loclass);
    with_view_model(
        loclass->view, LoclassViewModel * model, { model->num_macs = num_macs; }, true);
}
