#include "../color_guess.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include <dolphin/dolphin.h>

struct ColorGuessStartscreen {
    View* view;
    ColorGuessStartscreenCallback callback;
    void* context;
};

typedef struct {
    int some_value;
} ColorGuessStartscreenModel;

void color_guess_startscreen_set_callback(
    ColorGuessStartscreen* instance,
    ColorGuessStartscreenCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void color_guess_startscreen_draw(Canvas* canvas, ColorGuessStartscreenModel* model) {
    UNUSED(model);
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 10, AlignCenter, AlignTop, "Color Guess");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 22, AlignCenter, AlignTop, "Guess the color");
    canvas_draw_str_aligned(canvas, 64, 32, AlignCenter, AlignTop, "on Flipper's LED");
    elements_button_center(canvas, "Start");
}

static void color_guess_startscreen_model_init(ColorGuessStartscreenModel* const model) {
    model->some_value = 1;
}

bool color_guess_startscreen_input(InputEvent* event, void* context) {
    furi_assert(context);
    ColorGuessStartscreen* instance = context;
    if(event->type == InputTypeRelease) {
        switch(event->key) {
        case InputKeyBack:
            with_view_model(
                instance->view,
                ColorGuessStartscreenModel * model,
                {
                    UNUSED(model);
                    instance->callback(ColorGuessCustomEventStartscreenBack, instance->context);
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
                ColorGuessStartscreenModel * model,
                {
                    UNUSED(model);
                    instance->callback(ColorGuessCustomEventStartscreenOk, instance->context);
                },
                true);
            break;
        case InputKeyMAX:
            break;
        }
    }
    return true;
}

void color_guess_startscreen_exit(void* context) {
    furi_assert(context);
}

void color_guess_startscreen_enter(void* context) {
    furi_assert(context);
    ColorGuessStartscreen* instance = (ColorGuessStartscreen*)context;
    with_view_model(
        instance->view,
        ColorGuessStartscreenModel * model,
        { color_guess_startscreen_model_init(model); },
        true);
}

ColorGuessStartscreen* color_guess_startscreen_alloc() {
    ColorGuessStartscreen* instance = malloc(sizeof(ColorGuessStartscreen));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(ColorGuessStartscreenModel));
    view_set_context(instance->view, instance); // furi_assert crashes in events without this
    view_set_draw_callback(instance->view, (ViewDrawCallback)color_guess_startscreen_draw);
    view_set_input_callback(instance->view, color_guess_startscreen_input);
    //view_set_enter_callback(instance->view, color_guess_startscreen_enter);
    //view_set_exit_callback(instance->view, color_guess_startscreen_exit);

    with_view_model(
        instance->view,
        ColorGuessStartscreenModel * model,
        { color_guess_startscreen_model_init(model); },
        true);

    return instance;
}

void color_guess_startscreen_free(ColorGuessStartscreen* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view, ColorGuessStartscreenModel * model, { UNUSED(model); }, true);
    view_free(instance->view);
    free(instance);
}

View* color_guess_startscreen_get_view(ColorGuessStartscreen* instance) {
    furi_assert(instance);
    return instance->view;
}
