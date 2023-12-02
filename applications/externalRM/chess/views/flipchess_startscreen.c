#include "../flipchess.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include "flipchess_icons.h"

struct FlipChessStartscreen {
    View* view;
    FlipChessStartscreenCallback callback;
    void* context;
};

typedef struct {
    int some_value;
} FlipChessStartscreenModel;

void flipchess_startscreen_set_callback(
    FlipChessStartscreen* instance,
    FlipChessStartscreenCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void flipchess_startscreen_draw(Canvas* canvas, FlipChessStartscreenModel* model) {
    UNUSED(model);
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    canvas_draw_icon(canvas, 0, 0, &I_FLIPR_128x64);

#ifdef CANVAS_HAS_FONT_SCUMM_ROMAN_OUTLINE
    const uint8_t text_x_pos = 2;
    const uint8_t text_y_pos = 12;
    canvas_set_font(canvas, FontScummRomanOutline);
#else
    const uint8_t text_x_pos = 4;
    const uint8_t text_y_pos = 11;
    canvas_set_font(canvas, FontPrimary);
#endif
    canvas_draw_str(canvas, text_x_pos, text_y_pos, "Chess");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 62, text_y_pos, FLIPCHESS_VERSION);

    //canvas_set_font(canvas, FontSecondary);
    //canvas_draw_str(canvas, 10, 11, "How about a nice game of...");
    //canvas_draw_str(canvas, 99, 40, FLIPCHESS_VERSION);

    //canvas_set_font(canvas, FontPrimary);
    //canvas_draw_str(canvas, 10, 23, "Chess");
    //canvas_draw_icon(canvas, 0, 40, &I_Background_128x11);
    //canvas_draw_str(canvas, 10, 61, "FLIPR");

    elements_button_left(canvas, "Sound");
    elements_button_right(canvas, "Silent");
}

static void flipchess_startscreen_model_init(FlipChessStartscreenModel* const model) {
    model->some_value = 1;
}

bool flipchess_startscreen_input(InputEvent* event, void* context) {
    furi_assert(context);
    FlipChessStartscreen* instance = context;
    FlipChess* app = instance->context;

    if(event->type == InputTypeRelease) {
        switch(event->key) {
        case InputKeyBack:
            with_view_model(
                instance->view,
                FlipChessStartscreenModel * model,
                {
                    UNUSED(model);
                    instance->callback(FlipChessCustomEventStartscreenBack, instance->context);
                },
                true);
            break;
        case InputKeyLeft:
            // sound on, haptic off
            app->sound = 1;
            app->haptic = FlipChessHapticOff;
            with_view_model(
                instance->view,
                FlipChessStartscreenModel * model,
                {
                    UNUSED(model);
                    instance->callback(FlipChessCustomEventStartscreenOk, instance->context);
                },
                true);
            break;
        case InputKeyRight:
            // sound off, haptic on
            app->sound = 0;
            app->haptic = FlipChessHapticOn;
            with_view_model(
                instance->view,
                FlipChessStartscreenModel * model,
                {
                    UNUSED(model);
                    instance->callback(FlipChessCustomEventStartscreenOk, instance->context);
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

void flipchess_startscreen_exit(void* context) {
    furi_assert(context);
}

void flipchess_startscreen_enter(void* context) {
    furi_assert(context);
    FlipChessStartscreen* instance = (FlipChessStartscreen*)context;
    with_view_model(
        instance->view,
        FlipChessStartscreenModel * model,
        { flipchess_startscreen_model_init(model); },
        true);
}

FlipChessStartscreen* flipchess_startscreen_alloc() {
    FlipChessStartscreen* instance = malloc(sizeof(FlipChessStartscreen));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(FlipChessStartscreenModel));
    view_set_context(instance->view, instance); // furi_assert crashes in events without this
    view_set_draw_callback(instance->view, (ViewDrawCallback)flipchess_startscreen_draw);
    view_set_input_callback(instance->view, flipchess_startscreen_input);
    //view_set_enter_callback(instance->view, flipchess_startscreen_enter);
    //view_set_exit_callback(instance->view, flipchess_startscreen_exit);

    with_view_model(
        instance->view,
        FlipChessStartscreenModel * model,
        { flipchess_startscreen_model_init(model); },
        true);

    return instance;
}

void flipchess_startscreen_free(FlipChessStartscreen* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view, FlipChessStartscreenModel * model, { UNUSED(model); }, true);
    view_free(instance->view);
    free(instance);
}

View* flipchess_startscreen_get_view(FlipChessStartscreen* instance) {
    furi_assert(instance);
    return instance->view;
}
