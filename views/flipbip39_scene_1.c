#include "../flipbip39.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include <dolphin/dolphin.h>

//#include "../crypto/bip32.h"
#include "../crypto/bip39.h"
//#include "../crypto/ecdsa.h"
//#include "../crypto/curves.h"

struct FlipBip39Scene1 {
    View* view;
    FlipBip39Scene1Callback callback;
    void* context;
};


typedef struct {
    int some_value;
} FlipBip39Scene1Model;

void flipbip39_scene_1_set_callback(
    FlipBip39Scene1* instance,
    FlipBip39Scene1Callback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void flipbip39_scene_1_draw(Canvas* canvas, FlipBip39Scene1Model* model) {
    UNUSED(model);
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 0, 10, AlignLeft, AlignTop, "This is Scene 1"); 
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 0, 22, AlignLeft, AlignTop, mnemonic_generate(128)); 
    //canvas_draw_str_aligned(canvas, 0, 32, AlignLeft, AlignTop, "used as flipbip39"); 
}

static void flipbip39_scene_1_model_init(FlipBip39Scene1Model* const model) {
    model->some_value = 1;
}

bool flipbip39_scene_1_input(InputEvent* event, void* context) {
    furi_assert(context); 
    FlipBip39Scene1* instance = context;
    if (event->type == InputTypeRelease) {
        switch(event->key) {
            case InputKeyBack:
                with_view_model(
                    instance->view,
                    FlipBip39Scene1Model * model,
                    {
                        UNUSED(model);
                        instance->callback(FlipBip39CustomEventScene1Back, instance->context);
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
                    FlipBip39Scene1Model* model,
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

void flipbip39_scene_1_exit(void* context) {
    furi_assert(context);
}

void flipbip39_scene_1_enter(void* context) {
    furi_assert(context);
    FlipBip39Scene1* instance = (FlipBip39Scene1*)context;
    with_view_model(
        instance->view,
        FlipBip39Scene1Model * model,
        {
            flipbip39_scene_1_model_init(model);
        },
        true
    );
}

FlipBip39Scene1* flipbip39_scene_1_alloc() {
    FlipBip39Scene1* instance = malloc(sizeof(FlipBip39Scene1));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(FlipBip39Scene1Model));
    view_set_context(instance->view, instance); // furi_assert crashes in events without this
    view_set_draw_callback(instance->view, (ViewDrawCallback)flipbip39_scene_1_draw);
    view_set_input_callback(instance->view, flipbip39_scene_1_input);
    view_set_enter_callback(instance->view, flipbip39_scene_1_enter);
    view_set_exit_callback(instance->view, flipbip39_scene_1_exit);

    with_view_model(
        instance->view,
        FlipBip39Scene1Model * model,
        {
            flipbip39_scene_1_model_init(model);
        },
        true
    );
    
    return instance;
}

void flipbip39_scene_1_free(FlipBip39Scene1* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        FlipBip39Scene1Model * model,
        {
            UNUSED(model);
        },
        true);
    view_free(instance->view);
    free(instance);
}

View* flipbip39_scene_1_get_view(FlipBip39Scene1* instance) {
    furi_assert(instance);
    return instance->view;
}

