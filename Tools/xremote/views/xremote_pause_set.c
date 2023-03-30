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
    UNUSED(model);
    UNUSED(canvas);
    /*if(model->type == XRemoteRemoteItemTypeInfrared) {
        xremote_transmit_draw_ir(canvas, model);
    }*/
}

XRemotePauseSet* xremote_pause_set_alloc() {
    XRemotePauseSet* instance = malloc(sizeof(XRemotePauseSet));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(XRemotePauseSetModel));
    view_set_context(instance->view, instance);
    view_set_draw_callback(instance->view, (ViewDrawCallback)xremote_pause_set_draw);
    view_set_enter_callback(instance->view, xremote_pause_set_enter);

    with_view_model(
        instance->view,
        XRemotePauseSetModel * model,
        {
            xremote_pause_set_model_init(model);
        },
        true
    );

    return instance;
}

void xremote_pause_set_enter(void* context) {
    furi_assert(context);
    XRemotePauseSet* instance = (XRemotePauseSet*) context;
    with_view_model(
        instance->view,
        XRemotePauseSetModel * model,
        {
            xremote_pause_set_model_init(model);
        },
        true
    );
}

View* xremote_pause_set_get_view(XRemotePauseSet* instance) {
    furi_assert(instance);
    return instance->view;
}