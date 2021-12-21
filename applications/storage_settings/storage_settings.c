#include "storage_settings.h"

static StorageSettings* storage_settings_alloc() {
    StorageSettings* app = malloc(sizeof(StorageSettings));

    app->gui = furi_record_open("gui");
    app->fs_api = furi_record_open("storage");
    app->notification = furi_record_open("notification");

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_allocate_scene_manager(
        app->view_dispatcher, &storage_settings_scene_handlers, app);
    view_dispatcher_set_start_scene(app->view_dispatcher, StorageSettingsStart);
    app->scene_manager = view_dispatcher_get_scene_manager(app->view_dispatcher);
    string_init(app->text_string);

    view_dispatcher_enable_queue(app->view_dispatcher);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, StorageSettingsViewSubmenu, submenu_get_view(app->submenu));

    app->dialog_ex = dialog_ex_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, StorageSettingsViewDialogEx, dialog_ex_get_view(app->dialog_ex));

    return app;
}

static void storage_settings_free(StorageSettings* app) {
    view_dispatcher_remove_view(app->view_dispatcher, StorageSettingsViewSubmenu);
    submenu_free(app->submenu);

    view_dispatcher_remove_view(app->view_dispatcher, StorageSettingsViewDialogEx);
    dialog_ex_free(app->dialog_ex);

    view_dispatcher_free(app->view_dispatcher);

    furi_record_close("gui");
    furi_record_close("storage");
    furi_record_close("notification");

    string_clear(app->text_string);

    free(app);
}

int32_t storage_settings_app(void* p) {
    StorageSettings* app = storage_settings_alloc();

    view_dispatcher_run(app->view_dispatcher);

    storage_settings_free(app);
    return 0;
}