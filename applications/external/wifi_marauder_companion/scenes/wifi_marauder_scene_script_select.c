#include "../wifi_marauder_app_i.h"

static void wifi_marauder_scene_script_select_script_list_enter_callback(void* context, uint32_t index) {
    furi_assert(context);
    WifiMarauderApp* app = context;
    if (app->script_list_count == 0) {
        return;
    }

    char script_path[256];
    snprintf(script_path, sizeof(script_path), "%s/%s.json", MARAUDER_APP_FOLDER_SCRIPTS, furi_string_get_cstr(app->script_list[index]));

    app->script = wifi_marauder_script_parse_file(script_path, app->storage);
    if (app->script) {
        scene_manager_next_scene(app->scene_manager, WifiMarauderSceneConsoleOutput);
    }
}

void wifi_marauder_scene_script_select_on_enter(void* context) {
    furi_assert(context);
    WifiMarauderApp* app = context;
    VariableItemList* script_var_item_list = app->script_var_item_list;
    variable_item_list_set_enter_callback(script_var_item_list, wifi_marauder_scene_script_select_script_list_enter_callback, app);

    File* dir_scripts = storage_file_alloc(app->storage);
    if(storage_dir_open(dir_scripts, MARAUDER_APP_FOLDER_SCRIPTS)) {
        FileInfo file_info;
        char file_path[255];
        app->script_list_count = 0;
        // Goes through the files in the folder counting the ones that end with the json extension
        while(storage_dir_read(dir_scripts, &file_info, file_path, 255)) {
            app->script_list_count++;
        }
        if (app->script_list_count > 0) {
            app->script_list = malloc(app->script_list_count * sizeof(FuriString*));
            storage_dir_close(dir_scripts);
            storage_dir_open(dir_scripts, MARAUDER_APP_FOLDER_SCRIPTS);
            // Read the files again from the beginning, adding the scripts in the list
            int script_index = 0;
            while(storage_dir_read(dir_scripts, &file_info, file_path, 255)) {
                app->script_list[script_index] = furi_string_alloc();
                path_extract_filename_no_ext(file_path, app->script_list[script_index]);
                variable_item_list_add(script_var_item_list, furi_string_get_cstr(app->script_list[script_index]), 1, NULL, app);
                script_index++;
            }
        } else {
            variable_item_list_add(script_var_item_list, "No script found", 1, NULL, app);
        }
        storage_dir_close(dir_scripts);
    }
    storage_file_free(dir_scripts);

    variable_item_list_set_selected_item(script_var_item_list, scene_manager_get_scene_state(app->scene_manager, WifiMarauderSceneScriptSelect));
    view_dispatcher_switch_to_view(app->view_dispatcher, WifiMarauderAppViewScriptSelect);
}

bool wifi_marauder_scene_script_select_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);
    WifiMarauderApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == WifiMarauderEventStartConsole) {
            scene_manager_set_scene_state(
                app->scene_manager, WifiMarauderSceneScriptSelect, app->selected_script_index);
            scene_manager_next_scene(app->scene_manager, WifiMarauderSceneConsoleOutput);
        }
        consumed = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        app->selected_script_index = variable_item_list_get_selected_item_index(app->var_item_list);
        consumed = true;
    }

    return consumed;
}

void wifi_marauder_scene_script_select_on_exit(void* context) {
    furi_assert(context);
    WifiMarauderApp* app = context;

    variable_item_list_reset(app->script_var_item_list);

    for (int i = 0; i < app->script_list_count; i++) {
        furi_string_free(app->script_list[i]);
    }
    free(app->script_list);
    
}
