#include "xremote_infoscreen.h"
#include "../xremote.h"

struct XRemoteInfoscreen {
    View* view;
    XRemoteInfoscreenCallback callback;
    void* context;
};

typedef struct {
    int some_value;
} XRemoteInfoscreenModel;

void xremote_infoscreen_set_callback(
    XRemoteInfoscreen* instance,
    XRemoteInfoscreenCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void xremote_infoscreen_draw(Canvas* canvas, XRemoteInfoscreenModel* model) {
    UNUSED(model);
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 10, AlignCenter, AlignTop, "Cross Remote");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 22, AlignCenter, AlignTop, "Chain IR and SubGhz");
    canvas_draw_str_aligned(canvas, 64, 32, AlignCenter, AlignTop, "Commands");
    elements_button_center(canvas, "Back");
}

static void xremote_infoscreen_model_init(XRemoteInfoscreenModel* const model) {
    model->some_value = 1;
}

bool xremote_infoscreen_input(InputEvent* event, void* context) {
    furi_assert(context);
    XRemoteInfoscreen* instance = context;
    if(event->type == InputTypeRelease) {
        switch(event->key) {
        case InputKeyBack:
            with_view_model(
                instance->view,
                XRemoteInfoscreenModel * model,
                {
                    UNUSED(model);
                    instance->callback(XRemoteCustomEventInfoscreenBack, instance->context);
                },
                true);
            break;
        case InputKeyLeft:
        case InputKeyRight:
        case InputKeyUp:
        case InputKeyDown:
        case InputKeyOk:
            with_view_model(
                instance->view,
                XRemoteInfoscreenModel * model,
                {
                    UNUSED(model);
                    instance->callback(XRemoteCustomEventInfoscreenOk, instance->context);
                },
                true);
            break;
        case InputKeyMAX:
            break;
        }
    }
    return true;
}

void xremote_infoscreen_exit(void* context) {
    furi_assert(context);
}

void xremote_infoscreen_enter(void* context) {
    furi_assert(context);
    XRemoteInfoscreen* instance = (XRemoteInfoscreen*)context;
    with_view_model(
        instance->view,
        XRemoteInfoscreenModel * model,
        { xremote_infoscreen_model_init(model); },
        true);
}

XRemoteInfoscreen* xremote_infoscreen_alloc() {
    XRemoteInfoscreen* instance = malloc(sizeof(XRemoteInfoscreen));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(XRemoteInfoscreenModel));
    view_set_context(instance->view, instance); // furi_assert crashes in events without this
    view_set_draw_callback(instance->view, (ViewDrawCallback)xremote_infoscreen_draw);
    view_set_input_callback(instance->view, xremote_infoscreen_input);
    //view_set_enter_callback(instance->view, xremote_infoscreen_enter);
    //view_set_exit_callback(instance->view, xremote_infoscreen_exit);

    with_view_model(
        instance->view,
        XRemoteInfoscreenModel * model,
        { xremote_infoscreen_model_init(model); },
        true);

    return instance;
}

void xremote_infoscreen_free(XRemoteInfoscreen* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view, XRemoteInfoscreenModel * model, { UNUSED(model); }, true);
    view_free(instance->view);
    free(instance);
}

View* xremote_infoscreen_get_view(XRemoteInfoscreen* instance) {
    furi_assert(instance);
    return instance->view;
}
