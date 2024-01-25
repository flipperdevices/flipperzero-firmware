#include "../flippertag.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>

struct FlipperTagStartscreen {
    View* view;
    FlipperTagStartscreenCallback callback;
    void* context;
};

typedef struct {
    int some_value;
} FlipperTagStartscreenModel;

void flippertag_startscreen_set_callback(
    FlipperTagStartscreen* instance,
    FlipperTagStartscreenCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void flippertag_startscreen_draw(Canvas* canvas, void* model) {
    UNUSED(model);
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 10, AlignCenter, AlignTop, "Start Game");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 28, AlignCenter, AlignTop, "Left for Solo");
    canvas_draw_str_aligned(canvas, 64, 40, AlignCenter, AlignTop, "Ok for Team 1");
    canvas_draw_str_aligned(canvas, 64, 52, AlignCenter, AlignTop, "Right for Team 2");
}

static void flippertag_startscreen_model_init(void* const context) {
    FlipperTagStartscreenModel* model = context;
    model->some_value = 1;
}

bool flippertag_startscreen_input(InputEvent* event, void* context) {
    furi_assert(context);
    FlipperTagStartscreen* instance = context;
    if(event->type == InputTypeRelease) {
        switch(event->key) {
        case InputKeyBack:
            with_view_model(
                instance->view,
                FlipperTagStartscreenModel * model,
                {
                    UNUSED(model);
                    instance->callback(FlipperTagCustomEventStartscreenBack, instance->context);
                },
                true);
            break;
        case InputKeyLeft:
            with_view_model(
                instance->view,
                FlipperTagStartscreenModel * model,
                {
                    UNUSED(model);
                    instance->callback(FlipperTagCustomEventStartscreenLeft, instance->context);
                },
                true);
            break;
        case InputKeyRight:
            with_view_model(
                instance->view,
                FlipperTagStartscreenModel * model,
                {
                    UNUSED(model);
                    instance->callback(FlipperTagCustomEventStartscreenRight, instance->context);
                },
                true);
            break;
        case InputKeyUp:
        case InputKeyDown:
        case InputKeyOk:
            with_view_model(
                instance->view,
                FlipperTagStartscreenModel * model,
                {
                    UNUSED(model);
                    instance->callback(FlipperTagCustomEventStartscreenOk, instance->context);
                },
                true);
            break;
        case InputKeyMAX:
            break;
        }
    }
    return true;
}

void flippertag_startscreen_exit(void* context) {
    furi_assert(context);
}

void flippertag_startscreen_enter(void* context) {
    furi_assert(context);
    FlipperTagStartscreen* instance = context;
    with_view_model(
        instance->view,
        FlipperTagStartscreenModel * model,
        { flippertag_startscreen_model_init(model); },
        true);
}

FlipperTagStartscreen* flippertag_startscreen_alloc() {
    FlipperTagStartscreen* instance = malloc(sizeof(FlipperTagStartscreen));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(FlipperTagStartscreenModel));
    view_set_context(instance->view, instance); // furi_assert crashes in events without this
    view_set_draw_callback(instance->view, flippertag_startscreen_draw);
    view_set_input_callback(instance->view, flippertag_startscreen_input);
    //view_set_enter_callback(instance->view, FlipperTag_startscreen_enter);
    //view_set_exit_callback(instance->view, FlipperTag_startscreen_exit);

    with_view_model(
        instance->view,
        FlipperTagStartscreenModel * model,
        { flippertag_startscreen_model_init(model); },
        true);

    return instance;
}

void flippertag_startscreen_free(FlipperTagStartscreen* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view, FlipperTagStartscreenModel * model, { UNUSED(model); }, true);
    view_free(instance->view);
    free(instance);
}

View* flippertag_startscreen_get_view(FlipperTagStartscreen* instance) {
    furi_assert(instance);
    return instance->view;
}
