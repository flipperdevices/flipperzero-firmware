#include "../boilerplate.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include <dolphin/dolphin.h>

struct BoilerplateScene2 {
    View* view;
    BoilerplateScene2Callback callback;
    void* context;
};

typedef struct {
    int custom_value;
} BoilerplateScene2Model;

void boilerplate_scene_2_set_callback(
    BoilerplateScene2* instance,
    BoilerplateScene2Callback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void boilerplate_scene_2_draw(Canvas* canvas, BoilerplateScene2Model* model) {
    UNUSED(model);
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 0, 10, AlignLeft, AlignTop, "This is Scene 2"); 
}

static void boilerplate_scene_2_model_init(BoilerplateScene2Model* const model) {
    model->custom_value = 1;
}

bool boilerplate_scene_2_input(InputEvent* event, void* context) {
    furi_assert(context);
    BoilerplateScene2* instance = context;
    if (event->type == InputTypeRelease) {
        switch(event->key) {
            case InputKeyBack:
                with_view_model(
                    instance->view,
                    BoilerplateScene2Model * model,
                    {
                        UNUSED(model);
                        instance->callback(BoilerplateCustomEventScene2Back, instance->context);
                    },
                    true);
                break;
            case InputKeyLeft:
            case InputKeyRight:
            case InputKeyUp:
            case InputKeyDown:
            case InputKeyOk:
            case InputKeyMAX:
                break;
        }
    }
    
    return true;
}

void boilerplate_scene_2_exit(void* context) {
    furi_assert(context);
}

void boilerplate_scene_2_enter(void* context) {
    furi_assert(context);
    DOLPHIN_DEED(DolphinDeedPluginStart);
}

BoilerplateScene2* boilerplate_scene_2_alloc() {
    BoilerplateScene2* instance = malloc(sizeof(BoilerplateScene2));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(BoilerplateScene2Model));
    view_set_context(instance->view, instance);
    view_set_draw_callback(instance->view, (ViewDrawCallback)boilerplate_scene_2_draw);
    view_set_input_callback(instance->view, boilerplate_scene_2_input);
    //view_set_enter_callback(instance->view, boilerplate_scene_2_enter);
    //view_set_exit_callback(instance->view, boilerplate_scene_2_exit);

    with_view_model(
        instance->view,
        BoilerplateScene2Model * model,
        {
            boilerplate_scene_2_model_init(model);
        },
        true);
    
    return instance;
}

void boilerplate_scene_2_free(BoilerplateScene2* instance) {
    furi_assert(instance);


    view_free(instance->view);
    free(instance);
}

View* boilerplate_scene_2_get_view(BoilerplateScene2* instance) {
    furi_assert(instance);


    return instance->view;
}

