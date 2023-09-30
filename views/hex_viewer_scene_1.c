#include "../hex_viewer.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include <dolphin/dolphin.h>

struct HexViewerScene1 {
    View* view;
    HexViewerScene1Callback callback;
    void* context;
};


typedef struct {
    int some_value;
} HexViewerScene1Model;

void hex_viewer_scene_1_set_callback(
    HexViewerScene1* instance,
    HexViewerScene1Callback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void hex_viewer_scene_1_draw(Canvas* canvas, HexViewerScene1Model* model) {
    UNUSED(model);
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 0, 10, AlignLeft, AlignTop, "This is Scene 1"); 
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 0, 22, AlignLeft, AlignTop, "An empty scene to be"); 
    canvas_draw_str_aligned(canvas, 0, 32, AlignLeft, AlignTop, "used as hex_viewer"); 
}

static void hex_viewer_scene_1_model_init(HexViewerScene1Model* const model) {
    model->some_value = 1;
}

bool hex_viewer_scene_1_input(InputEvent* event, void* context) {
    furi_assert(context); 
    HexViewerScene1* instance = context;
    if (event->type == InputTypeRelease) {
        switch(event->key) {
            case InputKeyBack:
                with_view_model(
                    instance->view,
                    HexViewerScene1Model * model,
                    {
                        UNUSED(model);
                        instance->callback(HexViewerCustomEventScene1Back, instance->context);
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
                    HexViewerScene1Model* model,
                    {
                        UNUSED(model);
                    },
                    true);
                break;
            case InputKeyMAX:
                break;
        }
    }
    return true;
}

void hex_viewer_scene_1_exit(void* context) {
    furi_assert(context);
}

void hex_viewer_scene_1_enter(void* context) {
    furi_assert(context);
    HexViewerScene1* instance = (HexViewerScene1*)context;
    with_view_model(
        instance->view,
        HexViewerScene1Model * model,
        {
            hex_viewer_scene_1_model_init(model);
        },
        true
    );
}

HexViewerScene1* hex_viewer_scene_1_alloc() {
    HexViewerScene1* instance = malloc(sizeof(HexViewerScene1));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(HexViewerScene1Model));
    view_set_context(instance->view, instance); // furi_assert crashes in events without this
    view_set_draw_callback(instance->view, (ViewDrawCallback)hex_viewer_scene_1_draw);
    view_set_input_callback(instance->view, hex_viewer_scene_1_input);
    view_set_enter_callback(instance->view, hex_viewer_scene_1_enter);
    view_set_exit_callback(instance->view, hex_viewer_scene_1_exit);

    with_view_model(
        instance->view,
        HexViewerScene1Model * model,
        {
            hex_viewer_scene_1_model_init(model);
        },
        true
    );
    
    return instance;
}

void hex_viewer_scene_1_free(HexViewerScene1* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        HexViewerScene1Model * model,
        {
            UNUSED(model);
        },
        true);
    view_free(instance->view);
    free(instance);
}

View* hex_viewer_scene_1_get_view(HexViewerScene1* instance) {
    furi_assert(instance);
    return instance->view;
}

