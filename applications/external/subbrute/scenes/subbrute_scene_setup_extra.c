#include "../subbrute_i.h"
#include "subbrute_scene.h"

#define TAG "SubBruteSceneLoadFile"

void setup_extra_widget_callback(GuiButtonType result, InputType type, void* context);

static void setup_extra_widget_draw(void* context) {
    furi_assert(context);
    SubBruteState* instance = context;

    Widget* widget = instance->widget;

    widget_add_button_element(
        widget, GuiButtonTypeLeft, "-TD", setup_extra_widget_callback, instance);
    widget_add_button_element(
        widget, GuiButtonTypeRight, "TD+", setup_extra_widget_callback, instance);

    char str[20];
    snprintf(&str[0], 20, "%d", subbrute_worker_get_timeout(instance->worker));

    widget_add_string_element(
        instance->widget, 64, 15, AlignCenter, AlignCenter, FontPrimary, "Time Delay");

    widget_add_string_element(
        instance->widget, 64, 32, AlignCenter, AlignCenter, FontBigNumbers, &str[0]);
}

void setup_extra_widget_callback(GuiButtonType result, InputType type, void* context) {
    furi_assert(context);
    SubBruteState* instance = context;

    if((result == GuiButtonTypeLeft) && ((type == InputTypeShort) || (type == InputTypeRepeat))) {
        widget_reset(instance->widget);
        subbrute_worker_timeout_dec(instance->worker);
        setup_extra_widget_draw(instance);
    } else if(
        (result == GuiButtonTypeRight) &&
        ((type == InputTypeShort) || (type == InputTypeRepeat))) {
        widget_reset(instance->widget);
        subbrute_worker_timeout_inc(instance->worker);
        setup_extra_widget_draw(instance);
    }
}

void subbrute_scene_setup_extra_on_enter(void* context) {
    furi_assert(context);
    SubBruteState* instance = context;

    setup_extra_widget_draw(instance);

    view_dispatcher_switch_to_view(instance->view_dispatcher, SubBruteViewWidget);
}

void subbrute_scene_setup_extra_on_exit(void* context) {
    furi_assert(context);
    SubBruteState* instance = context;

    widget_reset(instance->widget);
}

bool subbrute_scene_setup_extra_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}