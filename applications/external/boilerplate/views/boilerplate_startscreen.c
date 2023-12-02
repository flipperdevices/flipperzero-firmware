#include "../boilerplate.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>

struct BoilerplateStartscreen {
    View* view;
    BoilerplateStartscreenCallback callback;
    void* context;
};

typedef struct {
    int some_value;
} BoilerplateStartscreenModel;

void boilerplate_startscreen_set_callback(
    BoilerplateStartscreen* instance,
    BoilerplateStartscreenCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void boilerplate_startscreen_draw(Canvas* canvas, BoilerplateStartscreenModel* model) {
    UNUSED(model);
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 10, AlignCenter, AlignTop, "Start Screen");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 22, AlignCenter, AlignTop, "Explain your app");
    canvas_draw_str_aligned(canvas, 64, 32, AlignCenter, AlignTop, "on this screen");
    elements_button_center(canvas, "Start");
}

static void boilerplate_startscreen_model_init(BoilerplateStartscreenModel* const model) {
    model->some_value = 1;
}

bool boilerplate_startscreen_input(InputEvent* event, void* context) {
    furi_assert(context);
    BoilerplateStartscreen* instance = context;
    if(event->type == InputTypeRelease) {
        switch(event->key) {
        case InputKeyBack:
            with_view_model(
                instance->view,
                BoilerplateStartscreenModel * model,
                {
                    UNUSED(model);
                    instance->callback(BoilerplateCustomEventStartscreenBack, instance->context);
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
                BoilerplateStartscreenModel * model,
                {
                    UNUSED(model);
                    instance->callback(BoilerplateCustomEventStartscreenOk, instance->context);
                },
                true);
            break;
        case InputKeyMAX:
            break;
        }
    }
    return true;
}

void boilerplate_startscreen_exit(void* context) {
    furi_assert(context);
}

void boilerplate_startscreen_enter(void* context) {
    furi_assert(context);
    BoilerplateStartscreen* instance = (BoilerplateStartscreen*)context;
    with_view_model(
        instance->view,
        BoilerplateStartscreenModel * model,
        { boilerplate_startscreen_model_init(model); },
        true);
}

BoilerplateStartscreen* boilerplate_startscreen_alloc() {
    BoilerplateStartscreen* instance = malloc(sizeof(BoilerplateStartscreen));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(BoilerplateStartscreenModel));
    view_set_context(instance->view, instance); // furi_assert crashes in events without this
    view_set_draw_callback(instance->view, (ViewDrawCallback)boilerplate_startscreen_draw);
    view_set_input_callback(instance->view, boilerplate_startscreen_input);
    //view_set_enter_callback(instance->view, boilerplate_startscreen_enter);
    //view_set_exit_callback(instance->view, boilerplate_startscreen_exit);

    with_view_model(
        instance->view,
        BoilerplateStartscreenModel * model,
        { boilerplate_startscreen_model_init(model); },
        true);

    return instance;
}

void boilerplate_startscreen_free(BoilerplateStartscreen* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view, BoilerplateStartscreenModel * model, { UNUSED(model); }, true);
    view_free(instance->view);
    free(instance);
}

View* boilerplate_startscreen_get_view(BoilerplateStartscreen* instance) {
    furi_assert(instance);
    return instance->view;
}
