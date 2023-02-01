#include "color_guess_color_set.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>

struct ColorGuessColorSet {
    View* view;
    void* context;
};

typedef struct {
    ColorGuessColorSetStatus status;
} ColorGuessColorSetModel;

bool color_guess_color_set_input(InputEvent* event, void* context) {
    furi_assert(context);
    UNUSED(event);
    //ColorGuessColorSet* instance = context;
    return false;
}

void color_guess_color_set_draw(Canvas* canvas, ColorGuessColorSetModel* model) {
    //uint8_t graphics_mode = 1;
    UNUSED(model);
    if (graphics_mode == 1) {
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 5, 7, "random Text");
    }
}

void color_guess_color_set_exit(void* context) {
    furi_assert(context);
    ColorGuessColorSet* instance = context;

    with_view_model(instance->view, ColorGuessColorSetModel * model, {
        if (model->status != 1) {
            model->status = 1;
        }
    }, true);
}

void color_guess_color_set_enter(void* context) {
    furi_assert(context);
    // ColorGuessColorSet* instance = context;
}

ColorGuessColorSet* color_guess_color_set_alloc() {
    ColorGuessColorSet* instance = malloc(sizeof(ColorGuessColorSet));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(ColorGuessColorSetModel));
    view_set_draw_callback(instance->view, (ViewDrawCallback)color_guess_color_set_draw);
    view_set_input_callback(instance->view, color_guess_color_set_input);
    view_set_enter_callback(instance->view, color_guess_color_set_enter);
    view_set_exit_callback(instance->view, color_guess_color_set_exit);

    return instance;
}

void color_guess_color_set_free(ColorGuessColorSet* instance) {
    furi_assert(instance);


    view_free(instance->view);
    free(instance);
}

View* color_guess_color_set_get_view(ColorGuessColorSet* instance) {
    furi_assert(instance);
    return instance->view;
}

