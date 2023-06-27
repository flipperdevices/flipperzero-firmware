#include "../desktop_settings_app.h"
#include "desktop_settings_scene.h"
#include "../helpers/desktop_settings_applications.h"
#include <storage/storage.h>
#include <dialogs/dialogs.h>
#include <flipper_application/flipper_application.h>

#define NONE_APPLICATION_NAME ("None (disable)")
#define NONE_APPLICATION_INDEX (FLIPPER_APPS2_COUNT + 1)

static bool favorite_fap_selector_item_callback(
    FuriString* file_path,
    void* context,
    uint8_t** icon_ptr,
    FuriString* item_name) {
    UNUSED(context);
#ifdef APP_FAP_LOADER
    Storage* storage = furi_record_open(RECORD_STORAGE);
    bool success = flipper_application_load_name_and_icon(file_path, storage, icon_ptr, item_name);
    furi_record_close(RECORD_STORAGE);
#else
    UNUSED(file_path);
    UNUSED(icon_ptr);
    UNUSED(item_name);
    bool success = false;
#endif
    return success;
}

static bool favorite_fap_selector_file_exists(char* file_path) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    bool exists = storage_file_exists(storage, file_path);
    furi_record_close(RECORD_STORAGE);
    return exists;
}

static void desktop_settings_scene_favorite_submenu_callback(void* context, uint32_t index) {
    DesktopSettingsApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void desktop_settings_scene_favorite_on_enter(void* context) {
    DesktopSettingsApp* app = context;
    Submenu* submenu = app->submenu;
    submenu_reset(submenu);

    uint32_t primary_favorite =
        scene_manager_get_scene_state(app->scene_manager, DesktopSettingsAppSceneFavorite);
    uint32_t pre_select_item = 0;
    FavoriteApp* curr_favorite_app = NULL;

    switch(primary_favorite) {
    case 0:
        curr_favorite_app = &app->settings.favorite_primary;
        break;
    case 1:
        curr_favorite_app = &app->settings.favorite_secondary;
        break;
    case 2:
        curr_favorite_app = &app->settings.favorite_tertiary;
        break;
    case 3:
        curr_favorite_app = &app->settings.favorite_quaternary;
        break;
    }

    if(curr_favorite_app == NULL) {
        // This should not happen!
        return;
    }

    for(size_t i = 0; i < FLIPPER_APPS2_COUNT; i++) {
        submenu_add_item(
            submenu,
            FLIPPER_APPS2[i].name,
            i,
            desktop_settings_scene_favorite_submenu_callback,
            app);

        // Select favorite item in submenu
        if(!curr_favorite_app->is_external &&
           !strcmp(FLIPPER_APPS2[i].name, curr_favorite_app->name_or_path)) {
            pre_select_item = i;
        } else if(!curr_favorite_app->is_external && strcmp(FLIPPER_APPS2[i].appid, "NULL")) {
            if(!strcmp(FLIPPER_APPS2[i].appid, curr_favorite_app->name_or_path)) {
                pre_select_item = i;
            }
        } else if(curr_favorite_app->is_external && !strcmp(FLIPPER_APPS2[i].name, "Applications")) {
            pre_select_item = i;
        }
    }

    submenu_add_item(
        submenu,
        NONE_APPLICATION_NAME,
        NONE_APPLICATION_INDEX,
        desktop_settings_scene_favorite_submenu_callback,
        app);

    if(strcmp(curr_favorite_app->name_or_path, "None (disable)") == 0) {
        pre_select_item = NONE_APPLICATION_INDEX;
    }

    switch(primary_favorite) {
    case 0:
        submenu_set_header(submenu, "Primary favorite app:");
        break;
    case 1:
        submenu_set_header(submenu, "Secondary favorite app:");
        break;
    case 2:
        submenu_set_header(submenu, "Tertiary favorite app:");
        break;
    case 3:
        submenu_set_header(submenu, "Quaternary favorite app:");
        break;
    default:
        submenu_set_header(submenu, "Primary favorite app:");
        break;
    }

    submenu_set_selected_item(submenu, pre_select_item); // If set during loop, visual glitch.

    view_dispatcher_switch_to_view(app->view_dispatcher, DesktopSettingsAppViewMenu);
}

bool desktop_settings_scene_favorite_on_event(void* context, SceneManagerEvent event) {
    DesktopSettingsApp* app = context;
    bool consumed = false;
    FuriString* temp_path = furi_string_alloc_set_str(EXT_PATH("apps"));

    uint32_t primary_favorite =
        scene_manager_get_scene_state(app->scene_manager, DesktopSettingsAppSceneFavorite);
    FavoriteApp* curr_favorite_app = NULL;

    switch(primary_favorite) {
    case 0:
        curr_favorite_app = &app->settings.favorite_primary;
        break;
    case 1:
        curr_favorite_app = &app->settings.favorite_secondary;
        break;
    case 2:
        curr_favorite_app = &app->settings.favorite_tertiary;
        break;
    case 3:
        curr_favorite_app = &app->settings.favorite_quaternary;
        break;
    }

    if(curr_favorite_app == NULL) {
        // This should not happen!
        furi_string_free(temp_path);
        return consumed;
    }

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NONE_APPLICATION_INDEX) {
            curr_favorite_app->is_external = false;
            strncpy(curr_favorite_app->name_or_path, "None (disable)", MAX_APP_LENGTH);
            consumed = true;
        } else if(strcmp(FLIPPER_APPS2[event.event].name, "Applications") == 0) {
            const DialogsFileBrowserOptions browser_options = {
                .extension = ".fap",
                .icon = &I_unknown_10px,
                .skip_assets = true,
                .hide_ext = true,
                .item_loader_callback = favorite_fap_selector_item_callback,
                .item_loader_context = app,
                .base_path = EXT_PATH("apps"),
            };

            // Select favorite fap in file browser
            if(favorite_fap_selector_file_exists(curr_favorite_app->name_or_path)) {
                furi_string_set_str(temp_path, curr_favorite_app->name_or_path);
            }

            if(dialog_file_browser_show(app->dialogs, temp_path, temp_path, &browser_options)) {
                submenu_reset(app->submenu); // Prevent menu from being shown when we exiting scene
                curr_favorite_app->is_external = true;
                strncpy(
                    curr_favorite_app->name_or_path,
                    furi_string_get_cstr(temp_path),
                    MAX_APP_LENGTH);
                consumed = true;
            }
        } else if(strcmp(FLIPPER_APPS2[event.event].appid, "NULL") != 0) {
            if(strstr(FLIPPER_APPS2[event.event].appid, ".fap")) {
                curr_favorite_app->is_external = false;
                strncpy(
                    curr_favorite_app->name_or_path,
                    FLIPPER_APPS2[event.event].appid,
                    MAX_APP_LENGTH);
            }
            consumed = true;
        } else {
            curr_favorite_app->is_external = false;
            strncpy(
                curr_favorite_app->name_or_path, FLIPPER_APPS2[event.event].name, MAX_APP_LENGTH);
            consumed = true;
        }
        if(consumed) {
            scene_manager_previous_scene(app->scene_manager);
        }
        consumed = true;
    }

    furi_string_free(temp_path);
    return consumed;
}

void desktop_settings_scene_favorite_on_exit(void* context) {
    DesktopSettingsApp* app = context;
    DESKTOP_SETTINGS_SAVE(&app->settings);
    submenu_reset(app->submenu);
}
