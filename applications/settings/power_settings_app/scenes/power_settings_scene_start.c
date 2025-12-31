#include "../power_settings_app.h"

void power_settings_scene_start_on_enter(void* context) {
    PowerSettingsApp* app = context;
    submenu_settings_helpers_scene_enter(app->settings_helper);
}

bool power_settings_scene_start_on_event(void* context, SceneManagerEvent event) {
    PowerSettingsApp* app = context;
    return submenu_settings_helpers_scene_event(app->settings_helper, event);
}

void power_settings_scene_start_on_exit(void* context) {
    PowerSettingsApp* app = context;
    submenu_settings_helpers_scene_exit(app->settings_helper);
}
