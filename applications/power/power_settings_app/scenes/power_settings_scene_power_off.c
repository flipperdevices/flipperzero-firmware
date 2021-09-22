#include "../power_settings_app.h"

void power_settings_scene_power_off_on_enter(void* context) {
    PowerSettingsApp* app = context;

    view_dispatcher_switch_to_view(app->view_dispatcher, PowerSettingsAppViewSubmenu);
}

bool power_settings_scene_power_off_on_event(void* context, SceneManagerEvent event) {
    // PowerSettingsApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
    }
    return consumed;
}

void power_settings_scene_power_off_on_exit(void* context) {
    // PowerSettingsApp* app = context;
}
