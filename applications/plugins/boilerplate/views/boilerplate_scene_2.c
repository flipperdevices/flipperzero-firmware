#include "../boilerplate.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include <dolphin/dolphin.h>
#include "../helpers/boilerplate_haptic.h"
#include "../helpers/boilerplate_speaker.h"
#include "../helpers/boilerplate_led.h"

struct BoilerplateScene2 {
    View* view;
    BoilerplateScene2Callback callback;
    void* context;
};

typedef struct {
    int screen_text;
} BoilerplateScene2Model;

char buttonText[11][14] = {
    "",
    "Press Up",
    "Press Down",
    "Press Left",
    "Press Right",
    "Press Ok",
    "Release Up",
    "Release Down",
    "Release Left",
    "Release Right",
    "Release Ok",
};

void boilerplate_scene_2_set_callback(
    BoilerplateScene2* instance,
    BoilerplateScene2Callback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void boilerplate_scene_2_draw(Canvas* canvas, BoilerplateScene2Model* model) {
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 0, 10, AlignLeft, AlignTop, "Scene 2: Input Examples");
    canvas_set_font(canvas, FontSecondary);
    char* strInput = malloc(15);
    strcpy(strInput, buttonText[model->screen_text]);
    canvas_draw_str_aligned(canvas, 0, 22, AlignLeft, AlignTop, strInput);
    free(strInput);
}

static void boilerplate_scene_2_model_init(BoilerplateScene2Model* const model) {
    model->screen_text = 0;
}

bool boilerplate_scene_2_input(InputEvent* event, void* context) {
    furi_assert(context);
    BoilerplateScene2* instance = context;
    if(event->type == InputTypeRelease) {
        switch(event->key) {
        case InputKeyBack:
            with_view_model(
                instance->view,
                BoilerplateScene2Model * model,
                {
                    UNUSED(model);
                    boilerplate_stop_all_sound(instance->context);
                    instance->callback(BoilerplateCustomEventScene2Back, instance->context);
                    boilerplate_play_long_bump(instance->context);
                },
                true);
            break;
        case InputKeyUp:
            with_view_model(
                instance->view,
                BoilerplateScene2Model * model,
                {
                    model->screen_text = 6;
                    boilerplate_play_bad_bump(instance->context);
                    boilerplate_stop_all_sound(instance->context);
                    boilerplate_led_set_rgb(instance->context, 255, 0, 255);
                },
                true);
            break;
        case InputKeyDown:
            with_view_model(
                instance->view,
                BoilerplateScene2Model * model,
                {
                    model->screen_text = 7;
                    boilerplate_play_bad_bump(instance->context);
                    boilerplate_stop_all_sound(instance->context);
                    boilerplate_led_set_rgb(instance->context, 255, 255, 0);
                },
                true);
            break;
        case InputKeyLeft:
            with_view_model(
                instance->view,
                BoilerplateScene2Model * model,
                {
                    model->screen_text = 8;
                    boilerplate_play_bad_bump(instance->context);
                    boilerplate_stop_all_sound(instance->context);
                    boilerplate_led_set_rgb(instance->context, 0, 255, 255);
                },
                true);
            break;
        case InputKeyRight:
            with_view_model(
                instance->view,
                BoilerplateScene2Model * model,
                {
                    model->screen_text = 9;
                    boilerplate_play_bad_bump(instance->context);
                    boilerplate_stop_all_sound(instance->context);
                    boilerplate_led_set_rgb(instance->context, 255, 0, 0);
                },
                true);
            break;
        case InputKeyOk:
            with_view_model(
                instance->view,
                BoilerplateScene2Model * model,
                {
                    model->screen_text = 10;
                    boilerplate_play_bad_bump(instance->context);
                    boilerplate_stop_all_sound(instance->context);
                    boilerplate_led_set_rgb(instance->context, 255, 255, 255);
                },
                true);
            break;
        case InputKeyMAX:
            break;
        }
    } else if(event->type == InputTypePress) {
        switch(event->key) {
        case InputKeyUp:
            with_view_model(
                instance->view,
                BoilerplateScene2Model * model,
                {
                    model->screen_text = 1;
                    boilerplate_play_happy_bump(instance->context);
                    boilerplate_play_input_sound(instance->context);
                },
                true);
            break;
        case InputKeyDown:
            with_view_model(
                instance->view,
                BoilerplateScene2Model * model,
                {
                    model->screen_text = 2;
                    boilerplate_play_happy_bump(instance->context);
                    boilerplate_play_input_sound(instance->context);
                },
                true);
            break;
        case InputKeyLeft:
            with_view_model(
                instance->view,
                BoilerplateScene2Model * model,
                {
                    model->screen_text = 3;
                    boilerplate_play_happy_bump(instance->context);
                    boilerplate_play_input_sound(instance->context);
                },
                true);
            break;
        case InputKeyRight:
            with_view_model(
                instance->view,
                BoilerplateScene2Model * model,
                {
                    model->screen_text = 4;
                    boilerplate_play_happy_bump(instance->context);
                    boilerplate_play_input_sound(instance->context);
                },
                true);
            break;
        case InputKeyOk:
            with_view_model(
                instance->view,
                BoilerplateScene2Model * model,
                {
                    model->screen_text = 5;
                    boilerplate_play_happy_bump(instance->context);
                    boilerplate_play_input_sound(instance->context);
                },
                true);
            break;
        case InputKeyBack:
        case InputKeyMAX:
            break;
        }
    }

    return true;
}

void boilerplate_scene_2_exit(void* context) {
    furi_assert(context);
    Boilerplate* app = context;
    boilerplate_stop_all_sound(app);
    //boilerplate_led_reset(app);
}

void boilerplate_scene_2_enter(void* context) {
    furi_assert(context);
    DOLPHIN_DEED(DolphinDeedPluginStart);
}

BoilerplateScene2* boilerplate_scene_2_alloc() {
    BoilerplateScene2* instance = malloc(sizeof(BoilerplateScene2));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(BoilerplateScene2Model));
    view_set_context(instance->view, instance);
    view_set_draw_callback(instance->view, (ViewDrawCallback)boilerplate_scene_2_draw);
    view_set_input_callback(instance->view, boilerplate_scene_2_input);
    //view_set_enter_callback(instance->view, boilerplate_scene_2_enter);
    view_set_exit_callback(instance->view, boilerplate_scene_2_exit);

    with_view_model(
        instance->view,
        BoilerplateScene2Model * model,
        { boilerplate_scene_2_model_init(model); },
        true);

    return instance;
}

void boilerplate_scene_2_free(BoilerplateScene2* instance) {
    furi_assert(instance);

    view_free(instance->view);
    free(instance);
}

View* boilerplate_scene_2_get_view(BoilerplateScene2* instance) {
    furi_assert(instance);

    return instance->view;
}
