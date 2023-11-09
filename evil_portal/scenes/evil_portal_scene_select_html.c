#include "../evil_portal_app_i.h"
#include "../helpers/evil_portal_storage.h"

void evil_portal_show_loading_popup(Evil_PortalApp* app, bool show) {
    TaskHandle_t timer_task = xTaskGetHandle(configTIMER_SERVICE_TASK_NAME);
    ViewStack* view_stack = app->view_stack;
    Loading* loading = app->loading;
    if(show) {
        // Raise timer priority so that animations can play
        vTaskPrioritySet(timer_task, configMAX_PRIORITIES - 1);
        view_stack_add_view(view_stack, loading_get_view(loading));
    } else {
        view_stack_remove_view(view_stack, loading_get_view(loading));
        // Restore default timer priority
        vTaskPrioritySet(timer_task, configTIMER_TASK_PRIORITY);
    }
}

void evil_portal_scene_select_html_on_enter(void* context) {
    Evil_PortalApp* app = context;
    DialogsFileBrowserOptions browser_options;
    evil_portal_create_html_folder_if_not_exists();

    dialog_file_browser_set_basic_options(&browser_options, HTML_EXTENSION, &I_evil_portal_10px);
    browser_options.base_path = HTML_FOLDER;

    FuriString* path;
    path = furi_string_alloc();

    furi_string_set(path, HTML_FOLDER);

    bool success = dialog_file_browser_show(app->dialogs, app->file_path, path, &browser_options);
    furi_string_free(path);

    if(success) {
        //Replace HTML File
        evil_portal_show_loading_popup(app, true);
        evil_portal_replace_index_html(app->file_path);
        evil_portal_show_loading_popup(app, false);
    }

    if(success) {
        scene_manager_search_and_switch_to_previous_scene(
            app->scene_manager, Evil_PortalSceneStart);
    } else {
        scene_manager_previous_scene(app->scene_manager);
    }
}

bool evil_portal_scene_select_html_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    bool consumed = true;
    return consumed;
}

void evil_portal_scene_select_html_on_exit(void* context) {
    UNUSED(context);
}
