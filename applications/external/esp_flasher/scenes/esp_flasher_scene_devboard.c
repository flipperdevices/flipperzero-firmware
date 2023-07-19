#include "../esp_flasher_app_i.h"

enum SubmenuIndex {
    SubmenuIndexBlackMagic,
    SubmenuIndexEvilPortal,
    SubmenuIndexMarauder,
};

void esp_flasher_scene_devboard_submenu_callback(void* context, uint32_t index) {
    furi_assert(context);
    EspFlasherApp* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void esp_flasher_scene_devboard_on_enter(void* context) {
    furi_assert(context);

    EspFlasherApp* app = context;
    Submenu* submenu = app->submenu;
    submenu_set_header(submenu, "Select Firmware to flash");
    submenu_add_item(
        submenu,
        "Black Magic",
        SubmenuIndexBlackMagic,
        esp_flasher_scene_devboard_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "Evil Portal",
        SubmenuIndexEvilPortal,
        esp_flasher_scene_devboard_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "Marauder",
        SubmenuIndexMarauder,
        esp_flasher_scene_devboard_submenu_callback,
        app);

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(app->scene_manager, EspFlasherSceneDevboard));

    view_dispatcher_switch_to_view(app->view_dispatcher, EspFlasherAppViewSubmenu);
}

bool esp_flasher_scene_devboard_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);

    EspFlasherApp* app = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        const char* path = NULL;
        consumed = true;
        switch(event.event) {
        case SubmenuIndexBlackMagic:
            path = APP_DATA_PATH("Black_Magic.bin");
            break;
        case SubmenuIndexEvilPortal:
            path = APP_DATA_PATH("Evil_Portal.bin");
            break;
        case SubmenuIndexMarauder:
            path = APP_DATA_PATH("Marauder.bin");
            break;
        default:
            consumed = false;
            break;
        }

        if(consumed) {
            app->selected_flash_options[SelectedFlashBoot] = true;
            strncpy(app->bin_file_path_boot, path, sizeof(app->bin_file_path_boot));
            scene_manager_next_scene(app->scene_manager, EspFlasherSceneConsoleOutput);
        }
        scene_manager_set_scene_state(app->scene_manager, EspFlasherSceneDevboard, event.event);
    }

    return consumed;
}

void esp_flasher_scene_devboard_on_exit(void* context) {
    furi_assert(context);

    EspFlasherApp* app = context;
    submenu_reset(app->submenu);
}