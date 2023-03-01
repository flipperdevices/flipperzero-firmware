#include "../flipbip39.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include <dolphin/dolphin.h>
#include "../helpers/flipbip39_haptic.h"
#include "../helpers/flipbip39_speaker.h"
#include "../helpers/flipbip39_led.h"

struct FlipBip39Scene2 {
    View* view;
    FlipBip39Scene2Callback callback;
    void* context;
};

typedef struct {
    int screen_text;
} FlipBip39Scene2Model;

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

void flipbip39_scene_2_set_callback(
    FlipBip39Scene2* instance,
    FlipBip39Scene2Callback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void flipbip39_scene_2_draw(Canvas* canvas, FlipBip39Scene2Model* model) {
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 0, 10, AlignLeft, AlignTop, "Scene 2: Input Examples"); 
    canvas_set_font(canvas, FontSecondary);
    char *strInput = malloc(15);
    strcpy(strInput, buttonText[model->screen_text]);
    canvas_draw_str_aligned(canvas, 0, 22, AlignLeft, AlignTop, strInput); 
    free(strInput);
}

static void flipbip39_scene_2_model_init(FlipBip39Scene2Model* const model) {
    model->screen_text = 0;
}

bool flipbip39_scene_2_input(InputEvent* event, void* context) {
    furi_assert(context);
    FlipBip39Scene2* instance = context;
    if (event->type == InputTypeRelease) {
        switch(event->key) {
            case InputKeyBack:
                with_view_model(
                    instance->view,
                    FlipBip39Scene2Model * model,
                    {
                        UNUSED(model);
                        flipbip39_stop_all_sound(instance->context);
                        instance->callback(FlipBip39CustomEventScene2Back, instance->context);
                        flipbip39_play_long_bump(instance->context);
                    },
                    true);
                break;
            case InputKeyUp:
                with_view_model(
                    instance->view,
                    FlipBip39Scene2Model * model,
                    {
                        model->screen_text = 6;
                        flipbip39_play_bad_bump(instance->context);
                        flipbip39_stop_all_sound(instance->context);
                        flipbip39_led_set_rgb(instance->context, 255, 0, 255);
                    },
                    true);
                break;
            case InputKeyDown:
                with_view_model(
                    instance->view,
                    FlipBip39Scene2Model * model,
                    {
                        model->screen_text = 7;
                        flipbip39_play_bad_bump(instance->context);
                        flipbip39_stop_all_sound(instance->context);
                        flipbip39_led_set_rgb(instance->context, 255, 255, 0);
                    },
                    true);
                break;
            case InputKeyLeft:
                with_view_model(
                    instance->view,
                    FlipBip39Scene2Model * model,
                    {
                        model->screen_text = 8;
                        flipbip39_play_bad_bump(instance->context);
                        flipbip39_stop_all_sound(instance->context);
                        flipbip39_led_set_rgb(instance->context, 0, 255, 255);
                    },
                    true);
                break;
            case InputKeyRight:
                with_view_model(
                    instance->view,
                    FlipBip39Scene2Model * model,
                    {
                        model->screen_text = 9;
                        flipbip39_play_bad_bump(instance->context);
                        flipbip39_stop_all_sound(instance->context);
                        flipbip39_led_set_rgb(instance->context, 255, 0, 0);
                    },
                    true);
                break;
            case InputKeyOk:
                with_view_model(
                    instance->view,
                    FlipBip39Scene2Model * model,
                    {
                        model->screen_text = 10;
                        flipbip39_play_bad_bump(instance->context);
                        flipbip39_stop_all_sound(instance->context);
                        flipbip39_led_set_rgb(instance->context, 255, 255, 255);
                    },
                    true);
                break;
            case InputKeyMAX:
                break;
        }
    } else if (event->type == InputTypePress) {
         switch(event->key) {
            case InputKeyUp:
                with_view_model(
                    instance->view,
                    FlipBip39Scene2Model * model,
                    {
                        model->screen_text = 1;
                        flipbip39_play_happy_bump(instance->context);
                        flipbip39_play_input_sound(instance->context);
                    },
                    true);
                break;
            case InputKeyDown:
                with_view_model(
                    instance->view,
                    FlipBip39Scene2Model * model,
                    {
                        model->screen_text = 2;
                        flipbip39_play_happy_bump(instance->context);
                        flipbip39_play_input_sound(instance->context);
                    },
                    true);
                break;
            case InputKeyLeft:
                with_view_model(
                    instance->view,
                    FlipBip39Scene2Model * model,
                    {
                        model->screen_text = 3;
                        flipbip39_play_happy_bump(instance->context);
                        flipbip39_play_input_sound(instance->context);
                    },
                    true);
                break;
            case InputKeyRight:
                with_view_model(
                    instance->view,
                    FlipBip39Scene2Model * model,
                    {
                        model->screen_text = 4;
                        flipbip39_play_happy_bump(instance->context);
                        flipbip39_play_input_sound(instance->context);
                    },
                    true);
                break;
            case InputKeyOk:
                with_view_model(
                    instance->view,
                    FlipBip39Scene2Model * model,
                    {
                        model->screen_text = 5;
                        flipbip39_play_happy_bump(instance->context);
                        flipbip39_play_input_sound(instance->context);
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

void flipbip39_scene_2_exit(void* context) {
    furi_assert(context);
    FlipBip39* app = context;
    flipbip39_stop_all_sound(app);
    //flipbip39_led_reset(app);
}

void flipbip39_scene_2_enter(void* context) {
    furi_assert(context);
    DOLPHIN_DEED(DolphinDeedPluginStart);
}

FlipBip39Scene2* flipbip39_scene_2_alloc() {
    FlipBip39Scene2* instance = malloc(sizeof(FlipBip39Scene2));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(FlipBip39Scene2Model));
    view_set_context(instance->view, instance);
    view_set_draw_callback(instance->view, (ViewDrawCallback)flipbip39_scene_2_draw);
    view_set_input_callback(instance->view, flipbip39_scene_2_input);
    //view_set_enter_callback(instance->view, flipbip39_scene_2_enter);
    view_set_exit_callback(instance->view, flipbip39_scene_2_exit);

    with_view_model(
        instance->view,
        FlipBip39Scene2Model * model,
        {
            flipbip39_scene_2_model_init(model);
        },
        true);
    
    return instance;
}

void flipbip39_scene_2_free(FlipBip39Scene2* instance) {
    furi_assert(instance);


    view_free(instance->view);
    free(instance);
}

View* flipbip39_scene_2_get_view(FlipBip39Scene2* instance) {
    furi_assert(instance);


    return instance->view;
}

