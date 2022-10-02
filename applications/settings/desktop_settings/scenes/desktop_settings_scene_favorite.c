#include "../desktop_settings_app.h"
#include "applications.h"
#include "desktop_settings_scene.h"
#include <storage/storage.h>
#include <dialogs/dialogs.h>

static void desktop_settings_scene_favorite_submenu_callback(void* context, uint32_t index) {
    DesktopSettingsApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void desktop_settings_scene_favorite_on_enter(void* context) {
    DesktopSettingsApp* app = context;
    Submenu* submenu = app->submenu;
    submenu_reset(submenu);

    for(size_t i = 0; i < FLIPPER_APPS_COUNT; i++) {
        submenu_add_item(
            submenu,
            FLIPPER_APPS[i].name,
            i,
            desktop_settings_scene_favorite_submenu_callback,
            app);
    }

    uint32_t primary_favorite =
        scene_manager_get_scene_state(app->scene_manager, DesktopSettingsAppSceneFavorite);

    submenu_set_header(
        app->submenu, primary_favorite ? "Primary favorite app:" : "Secondary favorite app:");

    // if(primary_favorite) {
    //     submenu_set_selected_item(app->submenu, app->settings.favorite_primary);
    // } else {
    //     submenu_set_selected_item(app->submenu, app->settings.favorite_secondary);
    // }
    view_dispatcher_switch_to_view(app->view_dispatcher, DesktopSettingsAppViewMenu);
}

bool desktop_settings_scene_favorite_on_event(void* context, SceneManagerEvent event) {
    DesktopSettingsApp* app = context;
    bool consumed = false;
    string_t temp_path;
    string_init_set_str(temp_path, EXT_PATH("apps"));

    uint32_t primary_favorite =
        scene_manager_get_scene_state(app->scene_manager, DesktopSettingsAppSceneFavorite);

    if(event.type == SceneManagerEventTypeCustom) {
        if(strcmp(FLIPPER_APPS[event.event].name, FAP_LOADER_APP_NAME)) {
            if(primary_favorite) {
                app->settings.favorite_primary.is_external = false;
                strncpy(
                    app->settings.favorite_primary.name_or_path,
                    FLIPPER_APPS[event.event].name,
                    MAX_APP_LENGTH);
            } else {
                app->settings.favorite_secondary.is_external = false;
                strncpy(
                    app->settings.favorite_secondary.name_or_path,
                    FLIPPER_APPS[event.event].name,
                    MAX_APP_LENGTH);
            }
        } else {
            const DialogsFileBrowserOptions browser_options = {
                .extension = ".fap",
                .skip_assets = true,
                .hide_ext = true,
            };

            while(dialog_file_browser_show(app->dialogs, temp_path, temp_path, &browser_options)) {
                if(primary_favorite) {
                    app->settings.favorite_primary.is_external = true;
                    strncpy(
                        app->settings.favorite_primary.name_or_path,
                        string_get_cstr(temp_path),
                        MAX_APP_LENGTH);
                } else {
                    app->settings.favorite_secondary.is_external = true;
                    strncpy(
                        app->settings.favorite_secondary.name_or_path,
                        string_get_cstr(temp_path),
                        MAX_APP_LENGTH);
                }
            }
        }
        scene_manager_previous_scene(app->scene_manager);
        consumed = true;
    }

    string_clear(temp_path);
    return consumed;
}

void desktop_settings_scene_favorite_on_exit(void* context) {
    DesktopSettingsApp* app = context;
    DESKTOP_SETTINGS_SAVE(&app->settings);
    submenu_reset(app->submenu);
}
