#include "app_i.h"

#include <furi.h>

#include "video_game_module_tool_icons.h"

void scene_probe_on_enter(void* context) {
    App* app = context;

    if(flasher_init()) {
        scene_manager_next_scene(app->scene_manager, SceneStart);
    } else {
        widget_add_icon_element(app->widget, 1, 1, &I_Update_module_56x52);
        widget_add_string_multiline_element(
            app->widget,
            92,
            32,
            AlignCenter,
            AlignCenter,
            FontSecondary,
            "Install Video\nGame Module");
        view_dispatcher_switch_to_view(app->view_dispatcher, ViewIdWidget);
    }
}

bool scene_probe_on_event(void* context, SceneManagerEvent event) {
    App* app = context;

    bool consumed = false;
    if(event.type == SceneManagerEventTypeTick) {
        if(flasher_init()) {
            scene_manager_next_scene(app->scene_manager, SceneStart);
        }
        consumed = true;
    }
    return consumed;
}

void scene_probe_on_exit(void* context) {
    App* app = context;
    widget_reset(app->widget);
    flasher_deinit();
}
