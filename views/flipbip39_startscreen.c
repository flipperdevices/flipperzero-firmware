#include "../flipbip39.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>

struct FlipBip39Startscreen {
    View* view;
    FlipBip39StartscreenCallback callback;
    void* context;
};


typedef struct {
    int some_value;
} FlipBip39StartscreenModel;

void flipbip39_startscreen_set_callback(
    FlipBip39Startscreen* instance,
    FlipBip39StartscreenCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void flipbip39_startscreen_draw(Canvas* canvas, FlipBip39StartscreenModel* model) {
    UNUSED(model);
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 10, AlignCenter, AlignTop, "Flip-BIP39"); 
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 22, AlignCenter, AlignTop, "Crypto tools"); 
    canvas_draw_str_aligned(canvas, 64, 32, AlignCenter, AlignTop, "for Flipper");
    elements_button_center(canvas, "Start"); 
}

static void flipbip39_startscreen_model_init(FlipBip39StartscreenModel* const model) {
    model->some_value = 1;
}

bool flipbip39_startscreen_input(InputEvent* event, void* context) {
    furi_assert(context); 
    FlipBip39Startscreen* instance = context;
    if (event->type == InputTypeRelease) {
        switch(event->key) {
            case InputKeyBack:
                with_view_model(
                    instance->view,
                    FlipBip39StartscreenModel * model,
                    {
                        UNUSED(model);
                        instance->callback(FlipBip39CustomEventStartscreenBack, instance->context);
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
                    FlipBip39StartscreenModel* model,
                    {
                        UNUSED(model);
                        instance->callback(FlipBip39CustomEventStartscreenOk, instance->context);
                    },
                    true);
                break;
            case InputKeyMAX:
                break;
        }
    }
    return true;
}

void flipbip39_startscreen_exit(void* context) {
    furi_assert(context);
}

void flipbip39_startscreen_enter(void* context) {
    furi_assert(context);
    FlipBip39Startscreen* instance = (FlipBip39Startscreen*)context;
    with_view_model(
        instance->view,
        FlipBip39StartscreenModel * model,
        {
            flipbip39_startscreen_model_init(model);
        },
        true
    );
}

FlipBip39Startscreen* flipbip39_startscreen_alloc() {
    FlipBip39Startscreen* instance = malloc(sizeof(FlipBip39Startscreen));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(FlipBip39StartscreenModel));
    view_set_context(instance->view, instance); // furi_assert crashes in events without this
    view_set_draw_callback(instance->view, (ViewDrawCallback)flipbip39_startscreen_draw);
    view_set_input_callback(instance->view, flipbip39_startscreen_input);
    //view_set_enter_callback(instance->view, flipbip39_startscreen_enter);
    //view_set_exit_callback(instance->view, flipbip39_startscreen_exit);

    with_view_model(
        instance->view,
        FlipBip39StartscreenModel * model,
        {
            flipbip39_startscreen_model_init(model);
        },
        true
    );
    
    return instance;
}

void flipbip39_startscreen_free(FlipBip39Startscreen* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        FlipBip39StartscreenModel * model,
        {
            UNUSED(model);
        },
        true);
    view_free(instance->view);
    free(instance);
}

View* flipbip39_startscreen_get_view(FlipBip39Startscreen* instance) {
    furi_assert(instance);
    return instance->view;
}

