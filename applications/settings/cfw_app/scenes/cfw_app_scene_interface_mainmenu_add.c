#include "../cfw_app.h"

enum SubmenuIndex {
    SubmenuIndexMainApp,
    SubmenuIndexExternalApp,
};

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

static void cfw_app_scene_interface_mainmenu_add_submenu_callback(void* context, uint32_t index) {
    CfwApp* app = context;
    scene_manager_set_scene_state(app->scene_manager, CfwAppSceneInterfaceMainmenuAdd, index);

    switch(index) {
    case SubmenuIndexMainApp:
        scene_manager_next_scene(app->scene_manager, CfwAppSceneInterfaceMainmenuAddMain);
        break;
    case SubmenuIndexExternalApp: {
        const DialogsFileBrowserOptions browser_options = {
            .extension = ".fap",
            .icon = &I_unknown_10px,
            .skip_assets = true,
            .hide_ext = true,
            .item_loader_callback = fap_selector_item_callback,
            .item_loader_context = app,
            .base_path = EXT_PATH("apps"),
        };
        FuriString* temp_path = furi_string_alloc_set_str(EXT_PATH("apps"));
        FuriString* filename = furi_string_alloc();
        if(dialog_file_browser_show(app->dialogs, temp_path, temp_path, &browser_options)) {
            Storage* storage = furi_record_open(RECORD_STORAGE);
            uint8_t* icon_buf = malloc(MENU_ICON_MAX_SIZE);

            if(!flipper_application_load_name_and_icon(temp_path, storage, &icon_buf, filename)) {
                free(icon_buf);
            } else {
                free(icon_buf);
                CharList_push_back(
                    app->mainmenu_app_names, strdup(furi_string_get_cstr(filename)));
                CharList_push_back(
                    app->mainmenu_app_paths, strdup(furi_string_get_cstr(temp_path)));
                app->mainmenu_app_index = CharList_size(app->mainmenu_app_names) - 1;
                app->save_mainmenu_apps = true;
                app->require_reboot = true;
            }
            furi_record_close(RECORD_STORAGE);
            furi_string_free(temp_path);
            furi_string_free(filename);
        }
        scene_manager_search_and_switch_to_previous_scene(
            app->scene_manager, CfwAppSceneInterfaceMainmenu);
        break;
    }
    default:
        break;
    }
}

void cfw_app_scene_interface_mainmenu_add_on_enter(void* context) {
    CfwApp* app = context;
    Submenu* submenu = app->submenu;

    submenu_add_item(
        submenu,
        "Main App",
        SubmenuIndexMainApp,
        cfw_app_scene_interface_mainmenu_add_submenu_callback,
        app);

    submenu_add_item(
        submenu,
        "External App",
        SubmenuIndexExternalApp,
        cfw_app_scene_interface_mainmenu_add_submenu_callback,
        app);

    submenu_set_header(submenu, "Add Menu App:");

    view_dispatcher_switch_to_view(app->view_dispatcher, CfwAppViewSubmenu);
}

bool cfw_app_scene_interface_mainmenu_add_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
    }

    return consumed;
}

void cfw_app_scene_interface_mainmenu_add_on_exit(void* context) {
    CfwApp* app = context;
    submenu_reset(app->submenu);
}
