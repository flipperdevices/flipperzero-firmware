#include "color_guess_play.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>

struct ColorGuessPlay {
    View* view;
    void* context;
};

typedef struct {
    ColorGuessPlayStatus status;
} ColorGuessPlayModel;

void color_guess_play_draw(Canvas* canvas, ColorGuessPlayModel* model) {
    //uint8_t graphics_mode = 1;
    UNUSED(model);
    //if (graphics_mode == 1) {
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 5, 7, "In Development");
    //}
}

bool color_guess_play_input(InputEvent* event, void* context) {
    furi_assert(context);
    UNUSED(event);
    return true;
}

void color_guess_play_exit(void* context) {
    furi_assert(context);
    
}

void color_guess_play_enter(void* context) {
    furi_assert(context);
    
}

ColorGuessPlay* color_guess_play_alloc() {
    ColorGuessPlay* instance = malloc(sizeof(ColorGuessPlay));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(ColorGuessPlayModel));
    view_set_draw_callback(instance->view, (ViewDrawCallback)color_guess_play_draw);
    //view_set_input_callback(instance->view, color_guess_play_input);
    //view_set_enter_callback(instance->view, color_guess_play_enter);
    //view_set_exit_callback(instance->view, color_guess_play_exit);

    with_view_model(
        instance->view,
        ColorGuessPlayModel * model,
        {
            model->status = ColorGuessPlayStatusIDLE;
        },
        true
    );
    
    return instance;
}

void color_guess_play_free(ColorGuessPlay* instance) {
    furi_assert(instance);


    view_free(instance->view);
    free(instance);
}

View* color_guess_play_get_view(ColorGuessPlay* instance) {
    furi_assert(instance);
    return instance->view;
}

