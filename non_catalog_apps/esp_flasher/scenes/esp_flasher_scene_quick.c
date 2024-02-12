#include "../esp_flasher_app_i.h"

enum QuickState {
    QuickStart,
    QuickBootS2,
    QuickBootS2_Marauder,
    QuickBootS2_Blackmagic,
    QuickBootWROOM,
    QuickBootWROOM_Marauder,
    QuickWROOM,
    QuickWROOM_Marauder,
    QuickS2,
    QuickS2_Marauder,
    QuickS2_Blackmagic,
};

void esp_flasher_scene_quick_submenu_callback(void* context, uint32_t index) {
    furi_assert(context);
    EspFlasherApp* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void esp_flasher_scene_quick_on_enter(void* context) {
    furi_assert(context);
    EspFlasherApp* app = context;
    Submenu* submenu = app->submenu;
    uint32_t state = scene_manager_get_scene_state(app->scene_manager, EspFlasherSceneQuick);

    switch(state) {
    case QuickStart:
    case QuickBootS2:
    case QuickBootWROOM:
    case QuickWROOM:
    case QuickS2:
        submenu_set_header(submenu, "Choose Board:");
        submenu_add_item(
            submenu,
            "Flipper WiFi Board / Xeon",
            QuickBootS2,
            esp_flasher_scene_quick_submenu_callback,
            app);
        submenu_add_item(
            submenu, "Multi-Fucc", QuickBootWROOM, esp_flasher_scene_quick_submenu_callback, app);
        submenu_add_item(
            submenu,
            "Other ESP32-WROOM",
            QuickWROOM,
            esp_flasher_scene_quick_submenu_callback,
            app);
        submenu_add_item(
            submenu, "Other ESP32-S2", QuickS2, esp_flasher_scene_quick_submenu_callback, app);
        break;
    case QuickBootS2_Marauder:
    case QuickBootS2_Blackmagic:
    case QuickS2_Marauder:
    case QuickS2_Blackmagic:
        submenu_set_header(submenu, "Choose Firmware:");
        submenu_add_item(
            submenu,
            "Marauder (has Evil Portal)",
            state > QuickS2 ? QuickS2_Marauder : QuickBootS2_Marauder,
            esp_flasher_scene_quick_submenu_callback,
            app);
        submenu_add_item(
            submenu,
            "Black Magic",
            state > QuickS2 ? QuickS2_Blackmagic : QuickBootS2_Blackmagic,
            esp_flasher_scene_quick_submenu_callback,
            app);
        break;
    case QuickBootWROOM_Marauder:
    case QuickWROOM_Marauder:
        submenu_set_header(submenu, "Choose Firmware:");
        submenu_add_item(
            submenu,
            "Marauder (has Evil Portal)",
            state > QuickWROOM ? QuickWROOM_Marauder : QuickBootWROOM_Marauder,
            esp_flasher_scene_quick_submenu_callback,
            app);
        break;
    default:
        break;
    }

    submenu_set_selected_item(submenu, state);

    view_dispatcher_switch_to_view(app->view_dispatcher, EspFlasherAppViewSubmenu);
}

bool esp_flasher_scene_quick_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);

    EspFlasherApp* app = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;

        bool enter_bootloader = false;
        const char* boot = NULL; // 0x1000
        const char* part = NULL; // 0x8000
        const char* app0 = NULL; // 0xE000
        const char* firm = NULL; // 0x10000

        switch(event.event) {
        case QuickBootS2:
        case QuickBootWROOM:
        case QuickWROOM:
        case QuickS2:
            scene_manager_set_scene_state(
                app->scene_manager, EspFlasherSceneQuick, event.event + 1);
            scene_manager_next_scene(app->scene_manager, EspFlasherSceneQuick);
            return consumed;

        case QuickBootS2_Marauder:
            enter_bootloader = true;
            /* fallthrough */
        case QuickS2_Marauder:
            boot = APP_ASSETS_PATH("marauder/S2/esp32_marauder.ino.bootloader.bin");
            part = APP_ASSETS_PATH("marauder/esp32_marauder.ino.partitions.bin");
            app0 = APP_ASSETS_PATH("marauder/boot_app0.bin");
            firm = APP_ASSETS_PATH("marauder/S2/esp32_marauder.flipper.bin");
            break;

        case QuickBootS2_Blackmagic:
            enter_bootloader = true;
            /* fallthrough */
        case QuickS2_Blackmagic:
            boot = APP_ASSETS_PATH("blackmagic/bootloader.bin");
            part = APP_ASSETS_PATH("blackmagic/partition-table.bin");
            firm = APP_ASSETS_PATH("blackmagic/blackmagic.bin");
            break;

        case QuickBootWROOM_Marauder:
            enter_bootloader = true;
            /* fallthrough */
        case QuickWROOM_Marauder:
            boot = APP_ASSETS_PATH("marauder/WROOM/esp32_marauder.ino.bootloader.bin");
            part = APP_ASSETS_PATH("marauder/esp32_marauder.ino.partitions.bin");
            app0 = APP_ASSETS_PATH("marauder/boot_app0.bin");
            firm =
                APP_ASSETS_PATH("marauder/WROOM/esp32_marauder.dev_board_pro.bin");
            break;

        default:
            consumed = false;
            return consumed;
        }

        scene_manager_set_scene_state(app->scene_manager, EspFlasherSceneQuick, event.event);
        memset(app->selected_flash_options, 0, sizeof(app->selected_flash_options));
        app->bin_file_path_boot[0] = '\0';
        app->bin_file_path_part[0] = '\0';
        app->bin_file_path_nvs[0] = '\0';
        app->bin_file_path_boot_app0[0] = '\0';
        app->bin_file_path_app_a[0] = '\0';
        app->bin_file_path_app_b[0] = '\0';
        app->bin_file_path_custom[0] = '\0';

        if(boot) {
            app->selected_flash_options[SelectedFlashBoot] = true;
            strncpy(app->bin_file_path_boot, boot, sizeof(app->bin_file_path_boot));
        }
        if(part) {
            app->selected_flash_options[SelectedFlashPart] = true;
            strncpy(app->bin_file_path_part, part, sizeof(app->bin_file_path_part));
        }
        if(app0) {
            app->selected_flash_options[SelectedFlashBootApp0] = true;
            strncpy(app->bin_file_path_boot_app0, app0, sizeof(app->bin_file_path_boot_app0));
        }
        if(firm) {
            app->selected_flash_options[SelectedFlashAppA] = true;
            strncpy(app->bin_file_path_app_a, firm, sizeof(app->bin_file_path_app_a));
        }

        app->reset = false;
        app->quickflash = true;
        app->turbospeed = true;
        app->boot = enter_bootloader;
        scene_manager_next_scene(app->scene_manager, EspFlasherSceneConsoleOutput);
    } else if(event.type == SceneManagerEventTypeBack) {
        uint32_t state = scene_manager_get_scene_state(app->scene_manager, EspFlasherSceneQuick);
        if(state > QuickS2)
            state = QuickS2;
        else if(state > QuickWROOM)
            state = QuickWROOM;
        else if(state > QuickBootWROOM)
            state = QuickBootWROOM;
        else if(state > QuickBootS2)
            state = QuickBootS2;
        scene_manager_set_scene_state(app->scene_manager, EspFlasherSceneQuick, state);
    }

    return consumed;
}

void esp_flasher_scene_quick_on_exit(void* context) {
    furi_assert(context);

    EspFlasherApp* app = context;
    submenu_reset(app->submenu);
}
