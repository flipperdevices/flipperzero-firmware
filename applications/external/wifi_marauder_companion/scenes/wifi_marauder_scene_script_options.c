#include "../wifi_marauder_app_i.h"

enum SubmenuIndex {
    SubmenuIndexRun,
    SubmenuIndexEdit,
    SubmenuIndexDelete
};

static void wifi_marauder_scene_script_options_callback(void* context, uint32_t index) {
    WifiMarauderApp* app = context;

    switch (index) {
        case SubmenuIndexRun:
            scene_manager_set_scene_state(app->scene_manager, WifiMarauderSceneScriptOptions, index);
            scene_manager_next_scene(app->scene_manager, WifiMarauderSceneConsoleOutput);
            break;
        case SubmenuIndexEdit:
            scene_manager_set_scene_state(app->scene_manager, WifiMarauderSceneScriptOptions, index);
            scene_manager_next_scene(app->scene_manager, WifiMarauderSceneScriptEdit);
            break;
        case SubmenuIndexDelete:
            scene_manager_set_scene_state(app->scene_manager, WifiMarauderSceneScriptOptions, index);
            scene_manager_next_scene(app->scene_manager, WifiMarauderSceneScriptConfirmDelete);
            break;
        default:
            break;
    }
}

void wifi_marauder_scene_script_options_on_enter(void* context) {
    WifiMarauderApp* app = context;

    // If returning after confirming script deletion
    if (app->script == NULL) {
        scene_manager_previous_scene(app->scene_manager);
        return;
    }

    Submenu* script_options_submenu = app->script_options_submenu;

    submenu_set_header(script_options_submenu, app->script->name);
    submenu_add_item(script_options_submenu, "[>] RUN", SubmenuIndexRun, wifi_marauder_scene_script_options_callback, app);
    submenu_add_item(script_options_submenu, "[*] EDIT", SubmenuIndexEdit, wifi_marauder_scene_script_options_callback, app);
    submenu_add_item(script_options_submenu, "[X] DELETE", SubmenuIndexDelete, wifi_marauder_scene_script_options_callback, app);

    submenu_set_selected_item(script_options_submenu, scene_manager_get_scene_state(app->scene_manager, WifiMarauderSceneScriptOptions));
    view_dispatcher_switch_to_view(app->view_dispatcher, WifiMarauderAppViewScriptOptions);
}

bool wifi_marauder_scene_script_options_on_event(void* context, SceneManagerEvent event) {
    WifiMarauderApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeBack) {
        wifi_marauder_script_free(app->script);
        app->script = NULL;
    }

    return consumed;
}

void wifi_marauder_scene_script_options_on_exit(void* context) {
    WifiMarauderApp* app = context;
    submenu_reset(app->script_options_submenu);
}
