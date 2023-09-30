#include "../hex_viewer.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>

struct HexViewerStartscreen {
    View* view;
    HexViewerStartscreenCallback callback;
    void* context;
};


typedef struct {
    int some_value;
} HexViewerStartscreenModel;

void hex_viewer_startscreen_set_callback(
    HexViewerStartscreen* instance,
    HexViewerStartscreenCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void hex_viewer_startscreen_draw(Canvas* canvas, HexViewerStartscreenModel* model) {
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

static void hex_viewer_startscreen_model_init(HexViewerStartscreenModel* const model) {
    model->some_value = 1;
}

bool hex_viewer_startscreen_input(InputEvent* event, void* context) {
    furi_assert(context); 
    HexViewerStartscreen* instance = context;
    if (event->type == InputTypeRelease) {
        switch(event->key) {
            case InputKeyBack:
                with_view_model(
                    instance->view,
                    HexViewerStartscreenModel * model,
                    {
                        UNUSED(model);
                        instance->callback(HexViewerCustomEventStartscreenBack, instance->context);
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
                    HexViewerStartscreenModel* model,
                    {
                        UNUSED(model);
                        instance->callback(HexViewerCustomEventStartscreenOk, instance->context);
                    },
                    true);
                break;
            case InputKeyMAX:
                break;
        }
    }
    return true;
}

void hex_viewer_startscreen_exit(void* context) {
    furi_assert(context);
}

void hex_viewer_startscreen_enter(void* context) {
    furi_assert(context);
    HexViewerStartscreen* instance = (HexViewerStartscreen*)context;
    with_view_model(
        instance->view,
        HexViewerStartscreenModel * model,
        {
            hex_viewer_startscreen_model_init(model);
        },
        true
    );
}

HexViewerStartscreen* hex_viewer_startscreen_alloc() {
    HexViewerStartscreen* instance = malloc(sizeof(HexViewerStartscreen));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(HexViewerStartscreenModel));
    view_set_context(instance->view, instance); // furi_assert crashes in events without this
    view_set_draw_callback(instance->view, (ViewDrawCallback)hex_viewer_startscreen_draw);
    view_set_input_callback(instance->view, hex_viewer_startscreen_input);
    //view_set_enter_callback(instance->view, hex_viewer_startscreen_enter);
    //view_set_exit_callback(instance->view, hex_viewer_startscreen_exit);

    with_view_model(
        instance->view,
        HexViewerStartscreenModel * model,
        {
            hex_viewer_startscreen_model_init(model);
        },
        true
    );
    
    return instance;
}

void hex_viewer_startscreen_free(HexViewerStartscreen* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        HexViewerStartscreenModel * model,
        {
            UNUSED(model);
        },
        true);
    view_free(instance->view);
    free(instance);
}

View* hex_viewer_startscreen_get_view(HexViewerStartscreen* instance) {
    furi_assert(instance);
    return instance->view;
}

