#include "../esp_flasher_app_i.h"

enum SubmenuIndex {
    SubmenuIndexEspFlasherFlash,
    SubmenuIndexEspFlasherSwitchA,
    SubmenuIndexEspFlasherSwitchB,
    SubmenuIndexEspFlasherAbout,
    SubmenuIndexEspFlasherReset,
    SubmenuIndexEspFlasherBootloader,
};

void esp_flasher_scene_start_submenu_callback(void* context, uint32_t index) {
    furi_assert(context);
    EspFlasherApp* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void esp_flasher_scene_start_on_enter(void* context) {
    furi_assert(context);

    EspFlasherApp* app = context;
    Submenu* submenu = app->submenu;
    submenu_add_item(
        submenu,
        "Flash ESP",
        SubmenuIndexEspFlasherFlash,
        esp_flasher_scene_start_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "Switch to Firmware A",
        SubmenuIndexEspFlasherSwitchA,
        esp_flasher_scene_start_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "Switch to Firmware B",
        SubmenuIndexEspFlasherSwitchB,
        esp_flasher_scene_start_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "Reset Board",
        SubmenuIndexEspFlasherReset,
        esp_flasher_scene_start_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "Enter Bootloader",
        SubmenuIndexEspFlasherBootloader,
        esp_flasher_scene_start_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "About",
        SubmenuIndexEspFlasherAbout,
        esp_flasher_scene_start_submenu_callback,
        app);

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(app->scene_manager, EspFlasherSceneStart));

    view_dispatcher_switch_to_view(app->view_dispatcher, EspFlasherAppViewSubmenu);
}

bool esp_flasher_scene_start_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);

    EspFlasherApp* app = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexEspFlasherAbout) {
            scene_manager_next_scene(app->scene_manager, EspFlasherSceneAbout);
            consumed = true;
        } else if(event.event == SubmenuIndexEspFlasherFlash) {
            scene_manager_next_scene(app->scene_manager, EspFlasherSceneBrowse);
            consumed = true;
        } else if(event.event == SubmenuIndexEspFlasherSwitchA) {
            app->switch_fw = SwitchToFirmwareA;
            scene_manager_next_scene(app->scene_manager, EspFlasherSceneConsoleOutput);
            consumed = true;
        } else if(event.event == SubmenuIndexEspFlasherSwitchB) {
            app->switch_fw = SwitchToFirmwareB;
            scene_manager_next_scene(app->scene_manager, EspFlasherSceneConsoleOutput);
            consumed = true;
        } else if(event.event == SubmenuIndexEspFlasherReset) {
            app->reset = true;
            scene_manager_next_scene(app->scene_manager, EspFlasherSceneConsoleOutput);
            consumed = true;
        } else if(event.event == SubmenuIndexEspFlasherBootloader) {
            app->boot = true;
            scene_manager_next_scene(app->scene_manager, EspFlasherSceneConsoleOutput);
            consumed = true;
        }
        scene_manager_set_scene_state(app->scene_manager, EspFlasherSceneStart, event.event);
    }

    return consumed;
}

void esp_flasher_scene_start_on_exit(void* context) {
    furi_assert(context);

    EspFlasherApp* app = context;
    submenu_reset(app->submenu);
}