#include "xremote_pause_set.h"

struct XRemotePauseSet {
    View* view;
    XRemotePauseSetCallback callback;
    void* context;
};

typedef struct {
    int type;
    const char* name;
    int time;
} XRemotePauseSetModel;

static void xremote_pause_set_model_init(XRemotePauseSetModel* const model) {
    model->type = XRemoteRemoteItemTypePause;
    model->time = 1;
}

void xremote_pause_set_set_callback(
    XRemotePauseSet* instance,
    XRemotePauseSetCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void xremote_pause_set_draw(Canvas* canvas, XRemotePauseSetModel* model) {
    char seconds[14];
    snprintf(seconds, SECONDS_LENGHT, SECONDS_FORMAT, model->time);
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 10, AlignCenter, AlignTop, "Pause duration");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_icon(canvas, 59, 22, &I_ButtonUp_10x5);
    canvas_draw_icon(canvas, 59, 42, &I_ButtonDown_10x5);
    canvas_draw_str_aligned(canvas, 64, 31, AlignCenter, AlignTop, seconds);
    elements_button_center(canvas, "Add");
}

bool xremote_pause_set_input(InputEvent* event, void* context) {
    furi_assert(context);
    XRemotePauseSet* instance = context;
    if(event->type == InputTypeRelease) {
        switch(event->key) {
        case InputKeyBack:
            instance->callback(XRemoteCustomEventPauseSetBack, instance->context);
            break;
        case InputKeyLeft:
        case InputKeyRight:
            break;
        case InputKeyUp:
            with_view_model(
                instance->view,
                XRemotePauseSetModel * model,
                {
                    model->time++;
                    if(model->time > 9) {
                        model->time = 0;
                    }
                },
                true);
            break;
        case InputKeyDown:
            with_view_model(
                instance->view,
                XRemotePauseSetModel * model,
                {
                    model->time--;
                    if(model->time < 0) {
                        model->time = 9;
                    }
                },
                true);
            break;
        case InputKeyOk:
            with_view_model(
                instance->view,
                XRemotePauseSetModel * model,
                {
                    XRemote* app = instance->context;
                    cross_remote_add_pause(app->cross_remote, model->time);
                },
                true);

            instance->callback(XRemoteCustomEventPauseSetOk, instance->context);
            break;
        case InputKeyMAX:
            break;
        }
    }
    return true;
}

XRemotePauseSet* xremote_pause_set_alloc() {
    XRemotePauseSet* instance = malloc(sizeof(XRemotePauseSet));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(XRemotePauseSetModel));
    view_set_context(instance->view, instance);
    view_set_draw_callback(instance->view, (ViewDrawCallback)xremote_pause_set_draw);
    view_set_enter_callback(instance->view, xremote_pause_set_enter);
    view_set_input_callback(instance->view, xremote_pause_set_input);

    with_view_model(
        instance->view,
        XRemotePauseSetModel * model,
        { xremote_pause_set_model_init(model); },
        true);

    return instance;
}

void xremote_pause_set_enter(void* context) {
    furi_assert(context);
    XRemotePauseSet* instance = (XRemotePauseSet*)context;
    with_view_model(
        instance->view,
        XRemotePauseSetModel * model,
        { xremote_pause_set_model_init(model); },
        true);
}

View* xremote_pause_set_get_view(XRemotePauseSet* instance) {
    furi_assert(instance);
    return instance->view;
}