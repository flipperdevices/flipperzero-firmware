#include "xremote_transmit.h"

struct XRemoteTransmit {
    View* view;
    XRemoteTransmitCallback callback;
    void* context;
};

typedef struct {
    int type;
    const char* name;
    int time;
} XRemoteTransmitModel;

static void xremote_transmit_model_init(XRemoteTransmitModel* const model) {
    model->type = XRemoteRemoteItemTypeInfrared;
    model->time = 1;
}

void xremote_transmit_model_set_name(XRemoteTransmit* instance, const char* name) {
    furi_assert(instance);
    XRemoteTransmitModel* model = view_get_model(instance->view);
    model->name = name;
    view_commit_model(instance->view, false);
}

void xremote_transmit_model_set_type(XRemoteTransmit* instance, int type) {
    furi_assert(instance);
    XRemoteTransmitModel* model = view_get_model(instance->view);
    model->time = 1;
    model->type = type;
    view_commit_model(instance->view, false);
}

void xremote_transmit_set_callback(
    XRemoteTransmit* instance,
    XRemoteTransmitCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void xremote_transmit_draw_ir(Canvas* canvas, XRemoteTransmitModel* model) {
    model->time++;
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_icon(canvas, 0, 0, &I_ir_transmit_128x64);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 74, 5, AlignLeft, AlignTop, "Sending");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 74, 20, AlignLeft, AlignTop, "Infrared");
    canvas_draw_str_aligned(canvas, 74, 30, AlignLeft, AlignTop, model->name);

    char temp_str[18];
    snprintf(temp_str, 18, "%u", model->time);
    canvas_draw_str_aligned(canvas, 74, 40, AlignLeft, AlignTop, temp_str);
}

void xremote_transmit_draw_pause(Canvas* canvas, XRemoteTransmitModel* model) {
    model->time++;
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_icon(canvas, 0, 0, &I_pause_128x64);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 74, 5, AlignLeft, AlignTop, "Waiting");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 74, 20, AlignLeft, AlignTop, "Sequence");
    canvas_draw_str_aligned(canvas, 74, 30, AlignLeft, AlignTop, model->name);

    char temp_str[18];
    snprintf(temp_str, 18, "%u", model->time);
    canvas_draw_str_aligned(canvas, 74, 40, AlignLeft, AlignTop, temp_str);
}

void xremote_transmit_draw_subghz(Canvas* canvas, XRemoteTransmitModel* model) {
    model->time++;
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_icon(canvas, 0, 0, &I_sg_transmit_128x64);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 74, 5, AlignLeft, AlignTop, "Sending");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 74, 20, AlignLeft, AlignTop, "SubGhz");
    canvas_draw_str_aligned(canvas, 74, 30, AlignLeft, AlignTop, model->name);

    char temp_str[18];
    snprintf(temp_str, 18, "%u", model->time);
    canvas_draw_str_aligned(canvas, 74, 40, AlignLeft, AlignTop, temp_str);
}

void xremote_transmit_draw(Canvas* canvas, XRemoteTransmitModel* model) {
    if(model->type == XRemoteRemoteItemTypeInfrared) {
        xremote_transmit_draw_ir(canvas, model);
    } else if(model->type == XRemoteRemoteItemTypePause) {
        xremote_transmit_draw_pause(canvas, model);
    }
}

XRemoteTransmit* xremote_transmit_alloc() {
    XRemoteTransmit* instance = malloc(sizeof(XRemoteTransmit));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(XRemoteTransmitModel));
    view_set_context(instance->view, instance);
    view_set_draw_callback(instance->view, (ViewDrawCallback)xremote_transmit_draw);
    view_set_enter_callback(instance->view, xremote_transmit_enter);

    with_view_model(
        instance->view,
        XRemoteTransmitModel * model,
        { xremote_transmit_model_init(model); },
        true);

    return instance;
}

void xremote_transmit_enter(void* context) {
    furi_assert(context);
    XRemoteTransmit* instance = (XRemoteTransmit*)context;
    with_view_model(
        instance->view,
        XRemoteTransmitModel * model,
        { xremote_transmit_model_init(model); },
        true);
}

void xremote_transmit_free(XRemoteTransmit* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view, XRemoteTransmitModel * model, { UNUSED(model); }, true);
    view_free(instance->view);
    free(instance);
}

View* xremote_transmit_get_view(XRemoteTransmit* instance) {
    furi_assert(instance);
    return instance->view;
}