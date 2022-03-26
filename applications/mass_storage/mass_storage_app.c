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

static bool mass_storage_check_assets(Storage* fs_api) {
    File* dir = storage_file_alloc(fs_api);
    bool ret = false;

    if(storage_dir_open(dir, MASS_STORAGE_APP_PATH_FOLDER)) {
        ret = true;
    }

    storage_dir_close(dir);
    storage_file_free(dir);

    return ret;
}

MassStorageApp* mass_storage_app_alloc(char* arg) {
    MassStorageApp* app = malloc(sizeof(MassStorageApp));
    memset(app, 0, sizeof(MassStorageApp));

    if(arg != NULL) {
        string_t filename;
        string_init_set(filename, arg);
        if(string_start_with_str_p(filename, MASS_STORAGE_APP_PATH_FOLDER)) {
            string_right(filename, strlen(MASS_STORAGE_APP_PATH_FOLDER) + 1);
        }
        strncpy(app->file_name, string_get_cstr(filename), MASS_STORAGE_FILE_NAME_LEN);
        string_clear(filename);
    }

    app->gui = furi_record_open("gui");
    app->fs_api = furi_record_open("storage");
    app->notifications = furi_record_open("notification");
    app->dialogs = furi_record_open("dialogs");

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

    // Custom Widget
    app->widget = widget_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, MassStorageAppViewError, widget_get_view(app->widget));

    app->mass_storage_view = mass_storage_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        MassStorageAppViewWork,
        mass_storage_get_view(app->mass_storage_view));

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    if(*app->file_name != '\0') {
        scene_manager_next_scene(app->scene_manager, MassStorageSceneWork);
    } else if(mass_storage_check_assets(app->fs_api)) {
        scene_manager_next_scene(app->scene_manager, MassStorageSceneFileSelect);
    } else {
        scene_manager_next_scene(app->scene_manager, MassStorageSceneError);
    }

    return app;
}

void mass_storage_app_free(MassStorageApp* app) {
    furi_assert(app);

    // Views
    view_dispatcher_remove_view(app->view_dispatcher, MassStorageAppViewFileSelect);
    view_dispatcher_remove_view(app->view_dispatcher, MassStorageAppViewWork);
    mass_storage_free(app->mass_storage_view);

    // Custom Widget
    view_dispatcher_remove_view(app->view_dispatcher, MassStorageAppViewError);
    widget_free(app->widget);

    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    // Close records
    furi_record_close("gui");
    furi_record_close("storage");
    furi_record_close("notification");
    furi_record_close("dialogs");

    free(app);
}

int32_t mass_storage_app(void* p) {
    MassStorageApp* mass_storage_app = mass_storage_app_alloc((char*)p);
    view_dispatcher_run(mass_storage_app->view_dispatcher);
    mass_storage_app_free(mass_storage_app);
    return 0;
}
