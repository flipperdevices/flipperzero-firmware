#include "../flipenigma.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include "flipenigma_icons.h"
#include "helpers/flipenigma_fonts.h"

// #include <assets_icons.h>

struct FlipEnigmaStartscreen {
    View* view;
    FlipEnigmaStartscreenCallback callback;
    void* context;
};

typedef struct {
    int some_value;
} FlipEnigmaStartscreenModel;

void flipenigma_startscreen_set_callback(
    FlipEnigmaStartscreen* instance,
    FlipEnigmaStartscreenCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void flipenigma_startscreen_draw(Canvas* canvas, FlipEnigmaStartscreenModel* model) {
    UNUSED(model);
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    canvas_draw_icon(canvas, 0, 0, &I_FLIPR_128x64);

    const uint8_t text_x_pos = 2;
    const uint8_t text_y_pos = 12;
    canvas_set_custom_u8g2_font(canvas, _u8g2_font_lucasarts_scumm_subtitle_o_tr);
    canvas_draw_str(canvas, text_x_pos, text_y_pos, "Enigma");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 62, text_y_pos, FLIPENIGMA_VERSION);

    //canvas_set_font(canvas, FontSecondary);
    //canvas_draw_str(canvas, 10, 11, "How about a nice game of...");
    //canvas_draw_str(canvas, 99, 40, FLIPENIGMA_VERSION);

    //canvas_set_font(canvas, FontPrimary);
    //canvas_draw_str(canvas, 10, 23, "Enigma");
    //canvas_draw_icon(canvas, 0, 40, &I_Background_128x11);
    //canvas_draw_str(canvas, 10, 61, "FLIPR");

    elements_button_left(canvas, "Sound");
    elements_button_right(canvas, "Silent");
}

static void flipenigma_startscreen_model_init(FlipEnigmaStartscreenModel* const model) {
    model->some_value = 1;
}

bool flipenigma_startscreen_input(InputEvent* event, void* context) {
    furi_assert(context);
    FlipEnigmaStartscreen* instance = context;
    FlipEnigma* app = instance->context;

    if(event->type == InputTypeRelease) {
        switch(event->key) {
        case InputKeyBack:
            with_view_model(
                instance->view,
                FlipEnigmaStartscreenModel * model,
                {
                    UNUSED(model);
                    instance->callback(FlipEnigmaCustomEventStartscreenBack, instance->context);
                },
                true);
            break;
        case InputKeyLeft:
            // sound on, haptic off
            app->sound = 1;
            app->haptic = FlipEnigmaHapticOff;
            with_view_model(
                instance->view,
                FlipEnigmaStartscreenModel * model,
                {
                    UNUSED(model);
                    instance->callback(FlipEnigmaCustomEventStartscreenOk, instance->context);
                },
                true);
            break;
        case InputKeyRight:
            // sound off, haptic on
            app->sound = 0;
            app->haptic = FlipEnigmaHapticOn;
            with_view_model(
                instance->view,
                FlipEnigmaStartscreenModel * model,
                {
                    UNUSED(model);
                    instance->callback(FlipEnigmaCustomEventStartscreenOk, instance->context);
                },
                true);
            break;
        case InputKeyUp:
        case InputKeyDown:
        case InputKeyOk:
        case InputKeyMAX:
            break;
        }
    }
    return true;
}

void flipenigma_startscreen_exit(void* context) {
    furi_assert(context);
}

void flipenigma_startscreen_enter(void* context) {
    furi_assert(context);
    FlipEnigmaStartscreen* instance = (FlipEnigmaStartscreen*)context;
    with_view_model(
        instance->view,
        FlipEnigmaStartscreenModel * model,
        { flipenigma_startscreen_model_init(model); },
        true);
}

FlipEnigmaStartscreen* flipenigma_startscreen_alloc() {
    FlipEnigmaStartscreen* instance = malloc(sizeof(FlipEnigmaStartscreen));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(FlipEnigmaStartscreenModel));
    view_set_context(instance->view, instance); // furi_assert crashes in events without this
    view_set_draw_callback(instance->view, (ViewDrawCallback)flipenigma_startscreen_draw);
    view_set_input_callback(instance->view, flipenigma_startscreen_input);
    //view_set_enter_callback(instance->view, flipenigma_startscreen_enter);
    //view_set_exit_callback(instance->view, flipenigma_startscreen_exit);

    with_view_model(
        instance->view,
        FlipEnigmaStartscreenModel * model,
        { flipenigma_startscreen_model_init(model); },
        true);

    return instance;
}

void flipenigma_startscreen_free(FlipEnigmaStartscreen* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view, FlipEnigmaStartscreenModel * model, { UNUSED(model); }, true);
    view_free(instance->view);
    free(instance);
}

View* flipenigma_startscreen_get_view(FlipEnigmaStartscreen* instance) {
    furi_assert(instance);
    return instance->view;
}
