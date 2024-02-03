#include "basic_scenes_split.h"

void basic_scenes_greeting_message_scene_on_enter(void* context) {
    App* app = context;
    widget_reset(app->widget);
    FuriString* message = furi_string_alloc();
    furi_string_printf(message, "Hello,\n%s!", app->user_name);
    widget_add_string_multiline_element(
        app->widget, 5, 15, AlignLeft, AlignCenter, FontPrimary, furi_string_get_cstr(message));
    furi_string_free(message);
    view_dispatcher_switch_to_view(app->view_dispatcher, BasicScenesWidgetView);
}

bool basic_scenes_greeting_message_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false; // event not handled.
}

void basic_scenes_greeting_message_scene_on_exit(void* context) {
    App* app = context;
    widget_reset(app->widget);
}