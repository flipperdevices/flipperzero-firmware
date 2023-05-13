#include "ac_remote_app_i.h"

#include <furi.h>
#include <furi_hal.h>

bool ac_remote_app_load_settings(AppState* app_state) {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    storage_common_stat(storage, SAVING_DIRECTORY, NULL);
    File* file = storage_file_alloc(storage);
    uint16_t bytes_readed = 0;
    if(storage_file_open(file, SAVING_FILENAME, FSAM_READ, FSOM_OPEN_EXISTING)) {
        bytes_readed = storage_file_read(file, app_state, sizeof(AppState));
    }
    storage_file_close(file);
    storage_file_free(file);

    furi_record_close(RECORD_STORAGE);

    return bytes_readed == sizeof(AppState);
}

void ac_remote_app_save_settings(const AppState* app_state) {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    if(storage_common_stat(storage, SAVING_DIRECTORY, NULL) == FSE_NOT_EXIST) {
        if(!storage_simply_mkdir(storage, SAVING_DIRECTORY)) {
            return;
        }
    }

    File* file = storage_file_alloc(storage);
    if(storage_file_open(file, SAVING_FILENAME, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        storage_file_write(file, app_state, sizeof(AppState));
    }
    storage_file_close(file);
    storage_file_free(file);

    furi_record_close(RECORD_STORAGE);
}

static bool ac_remote_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    AC_RemoteApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool ac_remote_app_back_event_callback(void* context) {
    furi_assert(context);
    AC_RemoteApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static void ac_remote_app_tick_event_callback(void* context) {
    furi_assert(context);
    AC_RemoteApp* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

AC_RemoteApp* ac_remote_app_alloc() {
    AC_RemoteApp* app = malloc(sizeof(AC_RemoteApp));

    app->gui = furi_record_open(RECORD_GUI);

    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&ac_remote_scene_handlers, app);
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, ac_remote_app_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, ac_remote_app_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, ac_remote_app_tick_event_callback, 100);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    app->var_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        AC_RemoteAppViewVarItemList,
        variable_item_list_get_view(app->var_item_list));

    for(int i = 0; i < NUM_MENU_ITEMS; ++i) {
        app->app_state.selected_option_index[i] = 0;
    }

    ac_remote_app_load_settings(&app->app_state);

    scene_manager_next_scene(app->scene_manager, AC_RemoteSceneStart);
    return app;
}

void ac_remote_app_free(AC_RemoteApp* app) {
    furi_assert(app);
    ac_remote_app_save_settings(&app->app_state);

    // Views
    view_dispatcher_remove_view(app->view_dispatcher, AC_RemoteAppViewVarItemList);

    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);
    
    // Close records
    furi_record_close(RECORD_GUI);
    free(app);
}

int32_t ac_remote_app(void* p) {
    UNUSED(p);
    AC_RemoteApp* ac_remote_app = ac_remote_app_alloc();
    view_dispatcher_run(ac_remote_app->view_dispatcher);
    ac_remote_app_free(ac_remote_app);
    return 0;
}
