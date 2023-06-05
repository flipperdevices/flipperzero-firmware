#include "../subghz_remote_app_i.h"

void subrem_scene_open_map_file_submenu_callback(void* context, uint32_t index) {
    furi_assert(context);
    SubGhzRemoteApp* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void subrem_scene_open_map_file_on_enter(void* context) {
    furi_assert(context);
    SubGhzRemoteApp* app = context;
    FURI_LOG_I("sub", "here1");
    SubRemLoadMapState load_state = subrem_load_from_file(app);

    FURI_LOG_I("sub", "here2");
    if(load_state == SubRemLoadMapStateBack) {
        FURI_LOG_I("sub", "here3");
        if(!scene_manager_previous_scene(app->scene_manager)) {
            FURI_LOG_I("sub", "here4");
            scene_manager_stop(app->scene_manager);
            view_dispatcher_stop(app->view_dispatcher);
        } else {
            FURI_LOG_I("sub", "here5");
            scene_manager_search_and_switch_to_previous_scene(
                app->scene_manager, SubRemSceneStart);
        }
    } else if((load_state == SubRemLoadMapStateNotAllOK) || (load_state == SubRemLoadMapStateOK)) {
        FURI_LOG_I("sub", "here6");
        Submenu* submenu = app->submenu;

        FURI_LOG_I("sub", "here6a");
        submenu_add_item(
            submenu,
            "Run Map File",
            SubmenuIndexSubRemRunMapFile,
            subrem_scene_open_map_file_submenu_callback,
            app);
        FURI_LOG_I("sub", "here6b");
        submenu_add_item(
            submenu,
            "Edit Map File",
            SubmenuIndexSubRemEditMapFile,
            subrem_scene_open_map_file_submenu_callback,
            app);

        FURI_LOG_I("sub", "here6c");
        submenu_set_selected_item(
            submenu,
            scene_manager_get_scene_state(app->scene_manager, SubmenuIndexSubRemOpenMapFile));

        FURI_LOG_I("sub", "here6d");
        view_dispatcher_switch_to_view(app->view_dispatcher, SubRemViewIDSubmenu);
        FURI_LOG_I("sub", "here7");
    } else {
        FURI_LOG_I("sub", "here8");
#ifdef SUBREM_LIGHT
        dialog_message_show_storage_error(app->dialogs, "Can't load\nMap file");
#else
        DialogMessage* message = dialog_message_alloc();

        dialog_message_set_header(message, "Map File Error", 64, 8, AlignCenter, AlignCenter);
        dialog_message_set_text(message, "Can't load\nMap file", 64, 32, AlignCenter, AlignCenter);
        dialog_message_set_buttons(message, "Back", NULL, NULL);
        dialog_message_show(app->dialogs, message);

        dialog_message_free(message);

        FURI_LOG_I("sub", "here9");
#endif
    }
}

bool subrem_scene_open_map_file_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);

    FURI_LOG_I("sub", "here10");
    SubGhzRemoteApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        FURI_LOG_I("sub", "here11");
        FURI_LOG_I("sub", "event.event: %lu", event.event);
        if(event.event == SubmenuIndexSubRemRunMapFile) {
            FURI_LOG_I("sub", "here12");
            scene_manager_next_scene(app->scene_manager, SubRemSceneRemote);
            consumed = true;
        } else if(event.event == SubmenuIndexSubRemEditMapFile) {
            FURI_LOG_I("sub", "here13");
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
