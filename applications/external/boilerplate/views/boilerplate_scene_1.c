#include "../boilerplate.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include <dolphin/dolphin.h>

struct BoilerplateScene1 {
    View* view;
    BoilerplateScene1Callback callback;
    void* context;
};

typedef struct {
    int some_value;
} BoilerplateScene1Model;

void boilerplate_scene_1_set_callback(
    BoilerplateScene1* instance,
    BoilerplateScene1Callback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void boilerplate_scene_1_draw(Canvas* canvas, BoilerplateScene1Model* model) {
    UNUSED(model);
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 0, 10, AlignLeft, AlignTop, "This is Scene 1");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 0, 22, AlignLeft, AlignTop, "An empty scene to be");
    canvas_draw_str_aligned(canvas, 0, 32, AlignLeft, AlignTop, "used as boilerplate");
}

static void boilerplate_scene_1_model_init(BoilerplateScene1Model* const model) {
    model->some_value = 1;
}

bool boilerplate_scene_1_input(InputEvent* event, void* context) {
    furi_assert(context);
    BoilerplateScene1* instance = context;
    if(event->type == InputTypeRelease) {
        switch(event->key) {
        case InputKeyBack:
            with_view_model(
                instance->view,
                BoilerplateScene1Model * model,
                {
                    UNUSED(model);
                    instance->callback(BoilerplateCustomEventScene1Back, instance->context);
                },
                true);
            break;
        case InputKeyLeft:
        case InputKeyRight:
        case InputKeyUp:
        case InputKeyDown:
        case InputKeyOk:
            with_view_model(
                instance->view, BoilerplateScene1Model * model, { UNUSED(model); }, true);
            break;
        case InputKeyMAX:
            break;
        }
    }
    return true;
}

void boilerplate_scene_1_exit(void* context) {
    furi_assert(context);
}

void boilerplate_scene_1_enter(void* context) {
    furi_assert(context);
    BoilerplateScene1* instance = (BoilerplateScene1*)context;
    with_view_model(
        instance->view,
        BoilerplateScene1Model * model,
        { boilerplate_scene_1_model_init(model); },
        true);
}

BoilerplateScene1* boilerplate_scene_1_alloc() {
    BoilerplateScene1* instance = malloc(sizeof(BoilerplateScene1));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(BoilerplateScene1Model));
    view_set_context(instance->view, instance); // furi_assert crashes in events without this
    view_set_draw_callback(instance->view, (ViewDrawCallback)boilerplate_scene_1_draw);
    view_set_input_callback(instance->view, boilerplate_scene_1_input);
    view_set_enter_callback(instance->view, boilerplate_scene_1_enter);
    view_set_exit_callback(instance->view, boilerplate_scene_1_exit);

    with_view_model(
        instance->view,
        BoilerplateScene1Model * model,
        { boilerplate_scene_1_model_init(model); },
        true);

    return instance;
}

void boilerplate_scene_1_free(BoilerplateScene1* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view, BoilerplateScene1Model * model, { UNUSED(model); }, true);
    view_free(instance->view);
    free(instance);
}

View* boilerplate_scene_1_get_view(BoilerplateScene1* instance) {
    furi_assert(instance);
    return instance->view;
}
