#include "mass_storage_app_i.h"
#include <furi.h>
#include <storage/storage.h>
#include <lib/toolbox/path.h>

static bool mass_storage_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    MassStorageApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool mass_storage_app_back_event_callback(void* context) {
    furi_assert(context);
    MassStorageApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static void mass_storage_app_tick_event_callback(void* context) {
    furi_assert(context);
    MassStorageApp* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

void mass_storage_app_show_loading_popup(MassStorageApp* app, bool show) {
    TaskHandle_t timer_task = xTaskGetHandle(configTIMER_SERVICE_TASK_NAME);

    if(show) {
        // Raise timer priority so that animations can play
        vTaskPrioritySet(timer_task, configMAX_PRIORITIES - 1);
        view_dispatcher_switch_to_view(app->view_dispatcher, MassStorageAppViewLoading);
    } else {
        // Restore default timer priority
        vTaskPrioritySet(timer_task, configTIMER_TASK_PRIORITY);
    }
}

MassStorageApp* mass_storage_app_alloc(char* arg) {
    MassStorageApp* app = malloc(sizeof(MassStorageApp));
    app->file_path = furi_string_alloc();

    if(arg != NULL) {
        furi_string_set_str(app->file_path, arg);
    } else {
        furi_string_set_str(app->file_path, MASS_STORAGE_APP_PATH_FOLDER);
    }

    app->gui = furi_record_open(RECORD_GUI);
    app->fs_api = furi_record_open(RECORD_STORAGE);
    app->dialogs = furi_record_open(RECORD_DIALOGS);

    app->create_image_size = (uint8_t)-1;
    SDInfo sd_info;
    if(storage_sd_info(app->fs_api, &sd_info) == FSE_OK) {
        switch(sd_info.fs_type) {
        case FST_FAT12:
            app->create_image_max = 16LL * 1024 * 1024;
            break;
        case FST_FAT16:
            app->create_image_max = 2LL * 1024 * 1024 * 1024;
            break;
        case FST_FAT32:
            app->create_image_max = 4LL * 1024 * 1024 * 1024;
            break;
        default:
            app->create_image_max = 0;
            break;
        }
    }

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);

    app->scene_manager = scene_manager_alloc(&mass_storage_scene_handlers, app);

    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, mass_storage_app_tick_event_callback, 500);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, mass_storage_app_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, mass_storage_app_back_event_callback);

    app->mass_storage_view = mass_storage_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        MassStorageAppViewWork,
        mass_storage_get_view(app->mass_storage_view));

    app->var_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        MassStorageAppViewVarItemList,
        variable_item_list_get_view(app->var_item_list));

    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, MassStorageAppViewSubmenu, submenu_get_view(app->submenu));

    app->text_input = text_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, MassStorageAppViewTextInput, text_input_get_view(app->text_input));

    app->popup = popup_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, MassStorageAppViewPopup, popup_get_view(app->popup));

    app->loading = loading_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, MassStorageAppViewLoading, loading_get_view(app->loading));

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    scene_manager_set_scene_state(
        app->scene_manager, MassStorageSceneStart, MassStorageSceneFileSelect);
    if(storage_file_exists(app->fs_api, furi_string_get_cstr(app->file_path))) {
        scene_manager_next_scene(app->scene_manager, MassStorageSceneWork);
    } else {
        scene_manager_next_scene(app->scene_manager, MassStorageSceneStart);
    }

    return app;
}

void mass_storage_app_free(MassStorageApp* app) {
    furi_assert(app);

    // Views
    view_dispatcher_remove_view(app->view_dispatcher, MassStorageAppViewWork);
    view_dispatcher_remove_view(app->view_dispatcher, MassStorageAppViewVarItemList);
    view_dispatcher_remove_view(app->view_dispatcher, MassStorageAppViewSubmenu);
    view_dispatcher_remove_view(app->view_dispatcher, MassStorageAppViewTextInput);
    view_dispatcher_remove_view(app->view_dispatcher, MassStorageAppViewPopup);
    view_dispatcher_remove_view(app->view_dispatcher, MassStorageAppViewLoading);

    mass_storage_free(app->mass_storage_view);
    variable_item_list_free(app->var_item_list);
    submenu_free(app->submenu);
    text_input_free(app->text_input);
    popup_free(app->popup);
    loading_free(app->loading);

    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    furi_string_free(app->file_path);

    // Close records
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_DIALOGS);

    free(app);
}

int32_t mass_storage_app(void* p) {
    MassStorageApp* mass_storage_app = mass_storage_app_alloc((char*)p);
    view_dispatcher_run(mass_storage_app->view_dispatcher);
    mass_storage_app_free(mass_storage_app);
    return 0;
}
