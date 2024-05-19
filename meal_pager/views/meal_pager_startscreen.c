#include "meal_pager_startscreen.h"

#include "../meal_pager_i.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>

struct Meal_PagerStartscreen {
    View* view;
    Meal_PagerStartscreenCallback callback;
    void* context;
};

typedef struct {
    int some_value;
} Meal_PagerStartscreenModel;

void meal_pager_startscreen_set_callback(
    Meal_PagerStartscreen* instance,
    Meal_PagerStartscreenCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void meal_pager_startscreen_draw(Canvas* canvas, Meal_PagerStartscreenModel* model) {
    UNUSED(model);
    char buffer[64];
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 10, AlignCenter, AlignTop, "Restaurant Pager");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 22, AlignCenter, AlignTop, "Trigger Tool");
    snprintf(buffer, sizeof(buffer), "Version: %s", MEAL_PAGER_VERSION);
    canvas_draw_str_aligned(canvas, 64, 42, AlignCenter, AlignTop, buffer);
    elements_button_center(canvas, "Start");
}

static void meal_pager_startscreen_model_init(Meal_PagerStartscreenModel* const model) {
    model->some_value = 1;
}

bool meal_pager_startscreen_input(InputEvent* event, void* context) {
    furi_assert(context);
    Meal_PagerStartscreen* instance = context;
    if(event->type == InputTypeRelease) {
        switch(event->key) {
        case InputKeyBack:
            with_view_model(
                instance->view,
                Meal_PagerStartscreenModel * model,
                {
                    UNUSED(model);
                    instance->callback(Meal_PagerCustomEventStartscreenBack, instance->context);
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
                Meal_PagerStartscreenModel * model,
                {
                    UNUSED(model);
                    instance->callback(Meal_PagerCustomEventStartscreenOk, instance->context);
                },
                true);
            break;
        case InputKeyMAX:
            break;
        }
    }
    return true;
}

void meal_pager_startscreen_exit(void* context) {
    furi_assert(context);
}

void meal_pager_startscreen_enter(void* context) {
    furi_assert(context);
    Meal_PagerStartscreen* instance = (Meal_PagerStartscreen*)context;
    with_view_model(
        instance->view,
        Meal_PagerStartscreenModel * model,
        { meal_pager_startscreen_model_init(model); },
        true);
}

Meal_PagerStartscreen* meal_pager_startscreen_alloc() {
    Meal_PagerStartscreen* instance = malloc(sizeof(Meal_PagerStartscreen));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(Meal_PagerStartscreenModel));
    view_set_context(instance->view, instance); // furi_assert crashes in events without this
    view_set_draw_callback(instance->view, (ViewDrawCallback)meal_pager_startscreen_draw);
    view_set_input_callback(instance->view, meal_pager_startscreen_input);
    //view_set_enter_callback(instance->view, meal_pager_startscreen_enter);
    //view_set_exit_callback(instance->view, meal_pager_startscreen_exit);

    with_view_model(
        instance->view,
        Meal_PagerStartscreenModel * model,
        { meal_pager_startscreen_model_init(model); },
        true);

    return instance;
}

void meal_pager_startscreen_free(Meal_PagerStartscreen* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view, Meal_PagerStartscreenModel * model, { UNUSED(model); }, true);
    view_free(instance->view);
    free(instance);
}

View* meal_pager_startscreen_get_view(Meal_PagerStartscreen* instance) {
    furi_assert(instance);
    return instance->view;
}
