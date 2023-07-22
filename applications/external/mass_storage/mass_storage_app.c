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
    app->notifications = furi_record_open(RECORD_NOTIFICATION);
    app->dialogs = furi_record_open(RECORD_DIALOGS);

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

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    if(storage_file_exists(app->fs_api, furi_string_get_cstr(app->file_path))) {
        scene_manager_next_scene(app->scene_manager, MassStorageSceneWork);
    } else {
        scene_manager_next_scene(app->scene_manager, MassStorageSceneFileSelect);
    }

    return app;
}

void mass_storage_app_free(MassStorageApp* app) {
    furi_assert(app);

    // Views
    view_dispatcher_remove_view(app->view_dispatcher, MassStorageAppViewWork);
    mass_storage_free(app->mass_storage_view);

    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    furi_string_free(app->file_path);

    // Close records
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_DIALOGS);

    free(app);
}

int32_t mass_storage_app(void* p) {
    MassStorageApp* mass_storage_app = mass_storage_app_alloc((char*)p);
    view_dispatcher_run(mass_storage_app->view_dispatcher);
    mass_storage_app_free(mass_storage_app);
    return 0;
}
