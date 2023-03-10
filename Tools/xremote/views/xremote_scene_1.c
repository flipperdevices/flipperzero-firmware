#include "../xremote.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include <dolphin/dolphin.h>

struct XRemoteScene1 {
    View* view;
    XRemoteScene1Callback callback;
    void* context;
};


typedef struct {
    int some_value;
} XRemoteScene1Model;

void xremote_scene_1_set_callback(
    XRemoteScene1* instance,
    XRemoteScene1Callback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void xremote_scene_1_draw(Canvas* canvas, XRemoteScene1Model* model) {
    UNUSED(model);
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 0, 10, AlignLeft, AlignTop, "Cross Remote"); 
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 0, 22, AlignLeft, AlignTop, "Combine IR & SubGhz Commands"); 
    canvas_draw_str_aligned(canvas, 0, 32, AlignLeft, AlignTop, "Run all with one press of a Button"); 
}

static void xremote_scene_1_model_init(XRemoteScene1Model* const model) {
    model->some_value = 1;
}

bool xremote_scene_1_input(InputEvent* event, void* context) {
    furi_assert(context); 
    XRemoteScene1* instance = context;
    if (event->type == InputTypeRelease) {
        switch(event->key) {
            case InputKeyBack:
                with_view_model(
                    instance->view,
                    XRemoteScene1Model * model,
                    {
                        UNUSED(model);
                        instance->callback(XRemoteCustomEventScene1Back, instance->context);
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
                    XRemoteScene1Model* model,
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

void xremote_scene_1_exit(void* context) {
    furi_assert(context);
}

void xremote_scene_1_enter(void* context) {
    furi_assert(context);
    XRemoteScene1* instance = (XRemoteScene1*)context;
    with_view_model(
        instance->view,
        XRemoteScene1Model * model,
        {
            xremote_scene_1_model_init(model);
        },
        true
    );
}

XRemoteScene1* xremote_scene_1_alloc() {
    XRemoteScene1* instance = malloc(sizeof(XRemoteScene1));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(XRemoteScene1Model));
    view_set_context(instance->view, instance); // furi_assert crashes in events without this
    view_set_draw_callback(instance->view, (ViewDrawCallback)xremote_scene_1_draw);
    view_set_input_callback(instance->view, xremote_scene_1_input);
    view_set_enter_callback(instance->view, xremote_scene_1_enter);
    view_set_exit_callback(instance->view, xremote_scene_1_exit);

    with_view_model(
        instance->view,
        XRemoteScene1Model * model,
        {
            xremote_scene_1_model_init(model);
        },
        true
    );
    
    return instance;
}

void xremote_scene_1_free(XRemoteScene1* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        XRemoteScene1Model * model,
        {
            UNUSED(model);
        },
        true);
    view_free(instance->view);
    free(instance);
}

View* xremote_scene_1_get_view(XRemoteScene1* instance) {
    furi_assert(instance);
    return instance->view;
}

