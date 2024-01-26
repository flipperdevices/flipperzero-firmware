#include "../flippertag.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include <dolphin/dolphin.h>

struct FlipperTagScene1 {
    View* view;
    FlipperTagScene1Callback callback;
    void* context;
};

typedef struct {
    int some_value;
    void* context;
} FlipperTagScene1Model;

void flippertag_scene_1_set_callback(
    FlipperTagScene1* instance,
    FlipperTagScene1Callback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void flippertag_scene_1_draw(Canvas* canvas, void* context) {
    furi_assert(context);
    FlipperTagScene1Model* model = context;
    FlipperTag* app = model->context;

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    if(app->health <= 0) {
        canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "Game Over!");
    } else {
        canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "Game On!");
    }
    canvas_set_font(canvas, FontSecondary);

    FuriString* health_text = furi_string_alloc_printf("Health: %ld", app->health);
    FuriString* ammo_text = furi_string_alloc_printf("Ammo: %ld", app->ammo);

    canvas_draw_str_aligned(canvas, 0, 22, AlignLeft, AlignTop, furi_string_get_cstr(health_text));
    canvas_draw_str_aligned(canvas, 0, 32, AlignLeft, AlignTop, furi_string_get_cstr(ammo_text));

    if(app->is_shielded) {
        FuriString* shield_text = furi_string_alloc_printf("Shield Integrity: %ld", app->shield_status);
        canvas_draw_str_aligned(canvas, 0, 42, AlignLeft, AlignTop, furi_string_get_cstr(shield_text));
        furi_string_free(shield_text);
    }

    furi_string_free(health_text);
    furi_string_free(ammo_text);
}

static void flippertag_scene_1_model_init(FlipperTagScene1Model* const model, void* context) {
    model->some_value = 1;
    model->context = context;
}

bool flippertag_scene_1_input(InputEvent* event, void* context) {
    furi_assert(context);
    FlipperTagScene1* instance = context;
    FlipperTag* app = instance->context;

    if(event->type == InputTypeRelease) {
        switch(event->key) {
        case InputKeyBack:
            with_view_model(
                instance->view,
                FlipperTagScene1Model * model,
                {
                    UNUSED(model);
                    instance->callback(FlipperTagCustomEventScene1Back, instance->context);

                    app->health = DEFAULT_HEALTH;
                    app->ammo = DEFAULT_AMMO;
                    app->shield_status = DEFAULT_SHIELD;
                },
                true);
            break;
        case InputKeyUp:
            break;
        case InputKeyLeft:
        case InputKeyRight:
        case InputKeyDown:
        case InputKeyOk:
            with_view_model(
                instance->view, FlipperTagScene1Model * model, { UNUSED(model); }, true);
            break;
        case InputKeyMAX:
            break;
        }
    } else if(event->type == InputTypePress) {
        switch(event->key) {
            case InputKeyOk:
                with_view_model(
                    instance->view,
                    FlipperTagScene1Model * model,
                    {
                    UNUSED(model);
                    if(app->ammo > 0 && !(app->health <= 0 || app->is_shielded)) {
                        app->ammo -= 1;

                        infrared_worker_rx_stop(app->rx);

                        infrared_worker_tx_start(app->tx);
                        infrared_worker_tx_stop(app->tx);

                        infrared_worker_rx_start(app->rx);

                        flippertag_led_set_rgb(app, 194, 122, 8);
                        flippertag_play_happy_bump(app);
                        flippertag_led_reset(app);
                        // flippertag_play_sound(app, 2);
                    }
                },
                true);
            break;
        case InputKeyDown:
            with_view_model(
                instance->view,
                FlipperTagScene1Model * model,
                {
                    UNUSED(model);
                    FlipperTag* app = instance->context;
                    app->ammo = 10;
                    flippertag_led_set_rgb(app, 0, 0, 255);
                    flippertag_led_reset(app);
                },
                true);
            break;
        case InputKeyUp:
            with_view_model(
                instance->view,
                FlipperTagScene1Model * model,
                {
                    UNUSED(model);

                    if(app->shield_status > 0) {
                        app->is_shielded = !app->is_shielded;
                    }
                },
                true);
            break;
        case InputKeyLeft:
        case InputKeyRight:
        case InputKeyBack:
            with_view_model(
                instance->view, FlipperTagScene1Model * model, { UNUSED(model); }, true);
            break;
        case InputKeyMAX:
            break;
        }
    }

    if(app->health <= 0) {
        instance->callback(FlipperTagCustomEventScene1Back, instance->context);

        app->health = DEFAULT_HEALTH;
        app->ammo = DEFAULT_AMMO;
    }
    return true;
}

void flippertag_scene_1_exit(void* context) {
    furi_assert(context);
    FlipperTagScene1* instance = (FlipperTagScene1*)context;

    FlipperTag* app = instance->context;
    infrared_worker_rx_stop(app->rx);
}

void flippertag_scene_1_enter(void* context) {
    furi_assert(context);
    FlipperTagScene1* instance = (FlipperTagScene1*)context;

    FlipperTag* app = instance->context;
    infrared_worker_rx_start(app->rx);

    with_view_model(
        instance->view,
        FlipperTagScene1Model * model,
        { flippertag_scene_1_model_init(model, app); },
        true);
}

FlipperTagScene1* flippertag_scene_1_alloc() {
    FlipperTagScene1* instance = malloc(sizeof(FlipperTagScene1));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(FlipperTagScene1Model));
    view_set_context(instance->view, instance); // furi_assert crashes in events without this
    view_set_draw_callback(instance->view, flippertag_scene_1_draw);
    view_set_input_callback(instance->view, flippertag_scene_1_input);
    view_set_enter_callback(instance->view, flippertag_scene_1_enter);
    view_set_exit_callback(instance->view, flippertag_scene_1_exit);

    with_view_model(
        instance->view,
        FlipperTagScene1Model * model,
        { flippertag_scene_1_model_init(model, instance); },
        true);

    return instance;
}

void flippertag_scene_1_free(FlipperTagScene1* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view, FlipperTagScene1Model * model, { UNUSED(model); }, true);
    view_free(instance->view);
    free(instance);
}

View* flippertag_scene_1_get_view(FlipperTagScene1* instance) {
    furi_assert(instance);
    return instance->view;
}
