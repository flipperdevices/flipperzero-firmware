#include "../xremote.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include <dolphin/dolphin.h>
#include "../helpers/xremote_haptic.h"
#include "../helpers/xremote_speaker.h"
#include "../helpers/xremote_led.h"

struct XRemoteScene2 {
    View* view;
    XRemoteScene2Callback callback;
    void* context;
};

typedef struct {
    int screen_text;
} XRemoteScene2Model;

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

void xremote_scene_2_set_callback(
    XRemoteScene2* instance,
    XRemoteScene2Callback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void xremote_scene_2_draw(Canvas* canvas, XRemoteScene2Model* model) {
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 0, 10, AlignLeft, AlignTop, "In Development"); 
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 0, 20, AlignLeft, AlignTop, "Please check back later"); 
    char *strInput = malloc(15);
    strcpy(strInput, buttonText[model->screen_text]);
    canvas_draw_str_aligned(canvas, 0, 22, AlignLeft, AlignTop, strInput); 
    free(strInput);
}

static void xremote_scene_2_model_init(XRemoteScene2Model* const model) {
    model->screen_text = 0;
}

bool xremote_scene_2_input(InputEvent* event, void* context) {
    furi_assert(context);
    XRemoteScene2* instance = context;
    if (event->type == InputTypeRelease) {
        switch(event->key) {
            case InputKeyBack:
                with_view_model(
                    instance->view,
                    XRemoteScene2Model * model,
                    {
                        UNUSED(model);
                        xremote_stop_all_sound(instance->context);
                        instance->callback(XRemoteCustomEventScene2Back, instance->context);
                        xremote_play_long_bump(instance->context);
                    },
                    true);
                break;
            case InputKeyUp:
                with_view_model(
                    instance->view,
                    XRemoteScene2Model * model,
                    {
                        model->screen_text = 6;
                        xremote_play_bad_bump(instance->context);
                        xremote_stop_all_sound(instance->context);
                        xremote_led_set_rgb(instance->context, 255, 0, 255);
                    },
                    true);
                break;
            case InputKeyDown:
                with_view_model(
                    instance->view,
                    XRemoteScene2Model * model,
                    {
                        model->screen_text = 7;
                        xremote_play_bad_bump(instance->context);
                        xremote_stop_all_sound(instance->context);
                        xremote_led_set_rgb(instance->context, 255, 255, 0);
                    },
                    true);
                break;
            case InputKeyLeft:
                with_view_model(
                    instance->view,
                    XRemoteScene2Model * model,
                    {
                        model->screen_text = 8;
                        xremote_play_bad_bump(instance->context);
                        xremote_stop_all_sound(instance->context);
                        xremote_led_set_rgb(instance->context, 0, 255, 255);
                    },
                    true);
                break;
            case InputKeyRight:
                with_view_model(
                    instance->view,
                    XRemoteScene2Model * model,
                    {
                        model->screen_text = 9;
                        xremote_play_bad_bump(instance->context);
                        xremote_stop_all_sound(instance->context);
                        xremote_led_set_rgb(instance->context, 255, 0, 0);
                    },
                    true);
                break;
            case InputKeyOk:
                with_view_model(
                    instance->view,
                    XRemoteScene2Model * model,
                    {
                        model->screen_text = 10;
                        xremote_play_bad_bump(instance->context);
                        xremote_stop_all_sound(instance->context);
                        xremote_led_set_rgb(instance->context, 255, 255, 255);
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
                    XRemoteScene2Model * model,
                    {
                        model->screen_text = 1;
                        xremote_play_happy_bump(instance->context);
                        xremote_play_input_sound(instance->context);
                    },
                    true);
                break;
            case InputKeyDown:
                with_view_model(
                    instance->view,
                    XRemoteScene2Model * model,
                    {
                        model->screen_text = 2;
                        xremote_play_happy_bump(instance->context);
                        xremote_play_input_sound(instance->context);
                    },
                    true);
                break;
            case InputKeyLeft:
                with_view_model(
                    instance->view,
                    XRemoteScene2Model * model,
                    {
                        model->screen_text = 3;
                        xremote_play_happy_bump(instance->context);
                        xremote_play_input_sound(instance->context);
                    },
                    true);
                break;
            case InputKeyRight:
                with_view_model(
                    instance->view,
                    XRemoteScene2Model * model,
                    {
                        model->screen_text = 4;
                        xremote_play_happy_bump(instance->context);
                        xremote_play_input_sound(instance->context);
                    },
                    true);
                break;
            case InputKeyOk:
                with_view_model(
                    instance->view,
                    XRemoteScene2Model * model,
                    {
                        model->screen_text = 5;
                        xremote_play_happy_bump(instance->context);
                        xremote_play_input_sound(instance->context);
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

void xremote_scene_2_exit(void* context) {
    furi_assert(context);
    XRemote* app = context;
    xremote_stop_all_sound(app);
    //xremote_led_reset(app);
}

void xremote_scene_2_enter(void* context) {
    furi_assert(context);
    DOLPHIN_DEED(DolphinDeedPluginStart);
}

XRemoteScene2* xremote_scene_2_alloc() {
    XRemoteScene2* instance = malloc(sizeof(XRemoteScene2));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(XRemoteScene2Model));
    view_set_context(instance->view, instance);
    view_set_draw_callback(instance->view, (ViewDrawCallback)xremote_scene_2_draw);
    view_set_input_callback(instance->view, xremote_scene_2_input);
    //view_set_enter_callback(instance->view, xremote_scene_2_enter);
    view_set_exit_callback(instance->view, xremote_scene_2_exit);

    with_view_model(
        instance->view,
        XRemoteScene2Model * model,
        {
            xremote_scene_2_model_init(model);
        },
        true);
    
    return instance;
}

void xremote_scene_2_free(XRemoteScene2* instance) {
    furi_assert(instance);


    view_free(instance->view);
    free(instance);
}

View* xremote_scene_2_get_view(XRemoteScene2* instance) {
    furi_assert(instance);


    return instance->view;
}

