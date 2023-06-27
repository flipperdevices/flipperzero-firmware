#include "../cfw_app.h"

enum FileBrowserResult {
    FileBrowserResultOk,
};

static bool cfw_app_scene_interface_mainmenu_add_file_browser_callback(
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

void cfw_app_scene_interface_mainmenu_add_on_enter(void* context) {
    CfwApp* app = context;
    FuriString* default_path = furi_string_alloc_set_str(EXT_PATH("apps"));
    FuriString* selected_path = furi_string_alloc();

    const DialogsFileBrowserOptions browser_options = {
        .extension = ".fap",
        .skip_assets = true,
        .hide_ext = true,
        .item_loader_callback = cfw_app_scene_interface_mainmenu_add_file_browser_callback,
        .item_loader_context = app,
        .base_path = EXT_PATH("apps"),
    };

    if(dialog_file_browser_show(app->dialogs, selected_path, default_path, &browser_options)) {
        Storage* storage = furi_record_open(RECORD_STORAGE);
        FuriString* filename = NULL;
        filename = furi_string_alloc();
        uint8_t* icon_buf = malloc(MENU_ICON_MAX_SIZE);

        path_extract_filename(selected_path, filename, true);
        if(!flipper_application_load_name_and_icon(selected_path, storage, &icon_buf, filename)) {
            free(icon_buf);
            icon_buf = NULL;
            app->save_mainmenu_apps = false;
            app->require_reboot = false;
        } else {
            free(icon_buf);
            icon_buf = NULL;
            CharList_push_back(
                app->mainmenu_app_paths, strdup(furi_string_get_cstr(selected_path)));
            CharList_push_back(app->mainmenu_app_names, strdup(furi_string_get_cstr(filename)));
            app->save_mainmenu_apps = true;
            app->require_reboot = true;
        }
        furi_record_close(RECORD_STORAGE);
        furi_string_free(filename);
    }

    furi_string_free(default_path);
    furi_string_free(selected_path);

    view_dispatcher_send_custom_event(app->view_dispatcher, FileBrowserResultOk);
}

bool cfw_app_scene_interface_mainmenu_add_on_event(void* context, SceneManagerEvent event) {
    CfwApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        switch(event.event) {
        case FileBrowserResultOk:
            scene_manager_previous_scene(app->scene_manager);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void cfw_app_scene_interface_mainmenu_add_on_exit(void* context) {
    UNUSED(context);
}
