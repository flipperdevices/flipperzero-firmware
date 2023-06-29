#include "../flipbip.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include "flipbip_icons.h"

struct FlipBipStartscreen {
    View* view;
    FlipBipStartscreenCallback callback;
    void* context;
};

typedef struct {
    int some_value;
} FlipBipStartscreenModel;

void flipbip_startscreen_set_callback(
    FlipBipStartscreen* instance,
    FlipBipStartscreenCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void flipbip_startscreen_draw(Canvas* canvas, FlipBipStartscreenModel* model) {
    UNUSED(model);
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    canvas_draw_icon(canvas, 1, 33, &I_Auth_62x31);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 18, 11, "FlipBIP - BIP32/39/44");

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 23, 22, "Crypto toolkit for Flipper");
    canvas_draw_str(canvas, 99, 34, FLIPBIP_VERSION);

    elements_button_right(canvas, "Start");
}

static void flipbip_startscreen_model_init(FlipBipStartscreenModel* const model) {
    model->some_value = 1;
}

bool flipbip_startscreen_input(InputEvent* event, void* context) {
    furi_assert(context);
    FlipBipStartscreen* instance = context;
    if(event->type == InputTypeRelease) {
        switch(event->key) {
        case InputKeyBack:
            with_view_model(
                instance->view,
                FlipBipStartscreenModel * model,
                {
                    UNUSED(model);
                    instance->callback(FlipBipCustomEventStartscreenBack, instance->context);
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
                FlipBipStartscreenModel * model,
                {
                    UNUSED(model);
                    instance->callback(FlipBipCustomEventStartscreenOk, instance->context);
                },
                true);
            break;
        case InputKeyMAX:
            break;
        }
    }
    return true;
}

void flipbip_startscreen_exit(void* context) {
    furi_assert(context);
}

void flipbip_startscreen_enter(void* context) {
    furi_assert(context);
    FlipBipStartscreen* instance = (FlipBipStartscreen*)context;
    with_view_model(
        instance->view,
        FlipBipStartscreenModel * model,
        { flipbip_startscreen_model_init(model); },
        true);
}

FlipBipStartscreen* flipbip_startscreen_alloc() {
    FlipBipStartscreen* instance = malloc(sizeof(FlipBipStartscreen));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(FlipBipStartscreenModel));
    view_set_context(instance->view, instance); // furi_assert crashes in events without this
    view_set_draw_callback(instance->view, (ViewDrawCallback)flipbip_startscreen_draw);
    view_set_input_callback(instance->view, flipbip_startscreen_input);
    //view_set_enter_callback(instance->view, flipbip_startscreen_enter);
    //view_set_exit_callback(instance->view, flipbip_startscreen_exit);

    with_view_model(
        instance->view,
        FlipBipStartscreenModel * model,
        { flipbip_startscreen_model_init(model); },
        true);

    return instance;
}

void flipbip_startscreen_free(FlipBipStartscreen* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view, FlipBipStartscreenModel * model, { UNUSED(model); }, true);
    view_free(instance->view);
    free(instance);
}

View* flipbip_startscreen_get_view(FlipBipStartscreen* instance) {
    furi_assert(instance);
    return instance->view;
}
