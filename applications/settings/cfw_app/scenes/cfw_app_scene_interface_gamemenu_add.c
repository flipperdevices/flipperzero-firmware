#include "../cfw_app.h"

#define GAMES_FOLDER EXT_PATH("apps/Games")

static bool fap_selector_item_callback(
    FuriString* file_path,
    void* context,
    uint8_t** icon_ptr,
    FuriString* item_name) {
    UNUSED(context);
    Storage* storage = furi_record_open(RECORD_STORAGE);
    bool success = flipper_application_load_name_and_icon(file_path, storage, icon_ptr, item_name);
    furi_record_close(RECORD_STORAGE);
    return success;
}

void cfw_app_scene_interface_gamemenu_add_on_enter(void* context) {
    CfwApp* app = context;
    const DialogsFileBrowserOptions browser_options = {
        .extension = ".fap",
        .icon = &I_unknown_10px,
        .skip_assets = true,
        .hide_ext = true,
        .hide_dot_files = true,
        .item_loader_callback = fap_selector_item_callback,
        .item_loader_context = app,
        .base_path = GAMES_FOLDER,
    };
    FuriString* temp_path = furi_string_alloc_set_str(GAMES_FOLDER);
    FuriString* filename = furi_string_alloc();
    if(dialog_file_browser_show(app->dialogs, temp_path, temp_path, &browser_options)) {
        Storage* storage = furi_record_open(RECORD_STORAGE);
        uint8_t* icon_buf = malloc(MENU_ICON_MAX_SIZE);

        if(flipper_application_load_name_and_icon(temp_path, storage, &icon_buf, filename)) {
            CharList_push_back(app->gamemenu_app_names, strdup(furi_string_get_cstr(filename)));
            CharList_push_back(app->gamemenu_app_paths, strdup(furi_string_get_cstr(temp_path)));
            app->gamemenu_app_index = CharList_size(app->gamemenu_app_names) - 1;
            app->save_gamemenu_apps = true;
            app->require_reboot = true;
        }
        free(icon_buf);
        furi_record_close(RECORD_STORAGE);
        furi_string_free(temp_path);
        furi_string_free(filename);
    }
    scene_manager_search_and_switch_to_previous_scene(
        app->scene_manager, CfwAppSceneInterfaceGamemenu);
}

bool cfw_app_scene_interface_gamemenu_add_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
    }

    return consumed;
}

void cfw_app_scene_interface_gamemenu_add_on_exit(void* context) {
    UNUSED(context);
}
