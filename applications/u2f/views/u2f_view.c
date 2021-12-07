#include "u2f_view.h"
#include <gui/elements.h>

struct U2F {
    View* view;
    U2FOkCallback callback;
    void* context;
};

typedef struct {
    uint8_t unused;
} U2FModel;

static void u2f_draw_callback(Canvas* canvas, void* _model) {
    //U2FModel* model = _model;

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 0, 0, AlignLeft, AlignTop, "U2F");

    canvas_draw_str_aligned(canvas, 64, 25, AlignCenter, AlignCenter, "! Warning !");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 40, AlignCenter, AlignCenter, "Demo, not for secure use");
}

static bool u2f_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    U2F* u2f = context;
    bool consumed = false;

    if(event->type == InputTypeShort) {
        if(event->key == InputKeyOk) {
            consumed = true;
            furi_assert(u2f->callback);
            u2f->callback(InputTypeShort, u2f->context);
        }
    }

    return consumed;
}

U2F* u2f_alloc() {
    U2F* u2f = furi_alloc(sizeof(U2F));

    u2f->view = view_alloc();
    view_allocate_model(u2f->view, ViewModelTypeLocking, sizeof(U2FModel));
    view_set_context(u2f->view, u2f);
    view_set_draw_callback(u2f->view, u2f_draw_callback);
    view_set_input_callback(u2f->view, u2f_input_callback);

    return u2f;
}

void u2f_free(U2F* u2f) {
    furi_assert(u2f);
    view_free(u2f->view);
    free(u2f);
}

View* u2f_get_view(U2F* u2f) {
    furi_assert(u2f);
    return u2f->view;
}

void u2f_set_ok_callback(U2F* u2f, U2FOkCallback callback, void* context) {
    furi_assert(u2f);
    furi_assert(callback);
    with_view_model(
        u2f->view, (U2FModel * model) {
            u2f->callback = callback;
            u2f->context = context;
            return false;
        });
}

void u2f_set_state(U2F* u2f) { //}, U2FState* st) {
    //furi_assert(st);
    with_view_model(
        u2f->view, (U2FModel * model) { return false; });
}
