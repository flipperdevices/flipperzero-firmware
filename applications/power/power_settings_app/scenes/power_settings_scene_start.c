#include "../power_settings_app.h"

enum PowerSettingsSubmenuIndex {
    PowerSettingsSubmenuIndexBatteryInfo,
    PowerSettingsSubmenuIndexReboot,
    PowerSettingsSubmenuIndexOff,
};

static void power_settings_scene_start_submenu_callback(void* context, uint32_t index) {
    furi_assert(context);
    PowerSettingsApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void power_settings_scene_start_on_enter(void* context) {
    PowerSettingsApp* app = context;
    Submenu* submenu = app->submenu;

    submenu_add_item(
        submenu,
        "Battery info",
        PowerSettingsSubmenuIndexBatteryInfo,
        power_settings_scene_start_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "Reboot",
        PowerSettingsSubmenuIndexReboot,
        power_settings_scene_start_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "Power OFF",
        PowerSettingsSubmenuIndexOff,
        power_settings_scene_start_submenu_callback,
        app);
    submenu_set_selected_item(
        submenu,
        view_dispatcher_get_scene_state(app->view_dispatcher, PowerSettingsAppSceneStart));

    view_dispatcher_switch_to_view(app->view_dispatcher, PowerSettingsAppViewSubmenu);
}

bool power_settings_scene_start_on_event(void* context, SceneManagerEvent event) {
    PowerSettingsApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == PowerSettingsSubmenuIndexBatteryInfo) {
            view_dispatcher_next_scene(app->view_dispatcher, PowerSettingsAppSceneBatteryInfo);
        } else if(event.event == PowerSettingsSubmenuIndexReboot) {
            view_dispatcher_next_scene(app->view_dispatcher, PowerSettingsAppSceneReboot);
        } else if(event.event == PowerSettingsSubmenuIndexOff) {
            view_dispatcher_next_scene(app->view_dispatcher, PowerSettingsAppScenePowerOff);
        }
        view_dispatcher_set_scene_state(
            app->view_dispatcher, PowerSettingsAppSceneStart, event.event);
        consumed = true;
    }
    return consumed;
}

void power_settings_scene_start_on_exit(void* context) {
    PowerSettingsApp* app = context;
    submenu_clean(app->submenu);
}
