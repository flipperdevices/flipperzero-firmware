#include "../desktop_settings_app.h"
#include "applications.h"

static void desktop_settings_scene_pincode_menu_submenu_callback(void* context, uint32_t index) {
    DesktopSettingsApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}
typedef enum {
    PincodeSubmenuSetPin,
    PincodeSubmenuResetPin,
    PincodeSubmenuDisable,
    PincodeSubmenuTotal,
} PincodeSubmenuIndex;

void desktop_settings_scene_pincode_menu_on_enter(void* context) {
    DesktopSettingsApp* app = context;
    Submenu* submenu = app->submenu;
    submenu_clean(submenu);

    if(!app->settings.pincode.length) {
        submenu_add_item(
            submenu,
            "Set Pin",
            PincodeSubmenuSetPin,
            desktop_settings_scene_pincode_menu_submenu_callback,
            app);

    } else {
        submenu_add_item(
            submenu,
            "Reset Pin",
            PincodeSubmenuResetPin,
            desktop_settings_scene_pincode_menu_submenu_callback,
            app);

        submenu_add_item(
            submenu,
            "Disable",
            PincodeSubmenuDisable,
            desktop_settings_scene_pincode_menu_submenu_callback,
            app);
    }

    submenu_set_header(app->submenu, "Pin code settings:");
    submenu_set_selected_item(app->submenu, app->menu_idx);
    view_dispatcher_switch_to_view(app->view_dispatcher, DesktopSettingsAppViewPincodeMenu);
}

bool desktop_settings_scene_pincode_menu_on_event(void* context, SceneManagerEvent event) {
    DesktopSettingsApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case PincodeSubmenuSetPin:
            scene_manager_next_scene(app->scene_manager, DesktopSettingsAppViewPincodeInput);
            consumed = true;
            break;
        case PincodeSubmenuResetPin:
            scene_manager_next_scene(app->scene_manager, DesktopSettingsAppViewPincodeInput);
            consumed = true;
            break;
        case PincodeSubmenuDisable:
            scene_manager_next_scene(app->scene_manager, DesktopSettingsAppViewPincodeInput);
            consumed = true;
            break;
        default:
            consumed = true;
            break;
        }
    }
    return consumed;
}

void desktop_settings_scene_pincode_menu_on_exit(void* context) {
    DesktopSettingsApp* app = context;
    submenu_clean(app->submenu);
}
