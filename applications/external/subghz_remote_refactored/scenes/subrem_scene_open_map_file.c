#include "../subghz_remote_app_i.h"

void subrem_scene_open_map_file_submenu_callback(void* context, uint32_t index) {
    furi_assert(context);
    SubGhzRemoteApp* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void subrem_scene_open_map_file_on_enter(void* context) {
    furi_assert(context);
    SubGhzRemoteApp* app = context;
    view_dispatcher_switch_to_view(app->view_dispatcher, SubRemViewIDSubmenu);
    SubRemLoadMapState load_state = subrem_load_from_file(app);

    if(load_state == SubRemLoadMapStateBack) {
        if(!scene_manager_previous_scene(app->scene_manager)) {
            scene_manager_stop(app->scene_manager);
            view_dispatcher_stop(app->view_dispatcher);
        } else {
            scene_manager_search_and_switch_to_previous_scene(
                app->scene_manager, SubRemSceneStart);
        }
    } else if((load_state == SubRemLoadMapStateNotAllOK) || (load_state == SubRemLoadMapStateOK)) {
        Submenu* submenu = app->submenu;

        submenu_add_item(
            submenu,
            "Run Map File",
            SubmenuIndexSubRemRunMapFile,
            subrem_scene_open_map_file_submenu_callback,
            app);
        submenu_add_item(
            submenu,
            "Edit Map File",
            SubmenuIndexSubRemEditMapFile,
            subrem_scene_open_map_file_submenu_callback,
            app);

        submenu_set_selected_item(
            submenu,
            scene_manager_get_scene_state(app->scene_manager, SubmenuIndexSubRemOpenMapFile));

        view_dispatcher_switch_to_view(app->view_dispatcher, SubRemViewIDSubmenu);
    } else {
#ifdef SUBREM_LIGHT
        dialog_message_show_storage_error(app->dialogs, "Can't load\nMap file");
#else
        DialogMessage* message = dialog_message_alloc();

        dialog_message_set_header(message, "Map File Error", 64, 8, AlignCenter, AlignCenter);
        dialog_message_set_text(message, "Can't load\nMap file", 64, 32, AlignCenter, AlignCenter);
        dialog_message_set_buttons(message, "Back", NULL, NULL);
        dialog_message_show(app->dialogs, message);

        dialog_message_free(message);
#endif
    }
}

bool subrem_scene_open_map_file_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);

    SubGhzRemoteApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexSubRemRunMapFile) {
            scene_manager_next_scene(app->scene_manager, SubRemSceneRemote);
            consumed = true;
        } else if(event.event == SubmenuIndexSubRemEditMapFile) {
            scene_manager_set_scene_state(
                app->scene_manager, SubRemSceneEditMenu, SubRemSubKeyNameUp);
            scene_manager_next_scene(app->scene_manager, SubRemSceneEditMenu);
            consumed = true;
        }
    }

    return consumed;
}

void subrem_scene_open_map_file_on_exit(void* context) {
    furi_assert(context);

    SubGhzRemoteApp* app = context;
    submenu_reset(app->submenu);
}
