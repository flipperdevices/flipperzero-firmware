#include "basic_scenes_split.h"

void basic_scenes_lottery_scene_on_enter(void* context) {
    App* app = context;
    widget_reset(app->widget);
    widget_add_string_element(
        app->widget, 25, 15, AlignLeft, AlignCenter, FontPrimary, "Lotto numbers:");
    widget_add_string_element(
        app->widget, 30, 35, AlignLeft, AlignCenter, FontBigNumbers, "0 4 2");
    view_dispatcher_switch_to_view(app->view_dispatcher, BasicScenesWidgetView);
}

bool basic_scenes_lottery_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false; // event not handled.
}

void basic_scenes_lottery_scene_on_exit(void* context) {
    UNUSED(context);
}