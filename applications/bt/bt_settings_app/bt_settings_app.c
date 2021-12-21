#include "bt_settings_app.h"

BtSettingsApp* bt_settings_app_alloc() {
    BtSettingsApp* app = furi_alloc(sizeof(BtSettingsApp));

    // Load settings
    bt_settings_load(&app->settings);
    app->gui = furi_record_open("gui");

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_allocate_scene_manager(app->view_dispatcher, &bt_settings_scene_handlers, app);
    view_dispatcher_set_start_scene(app->view_dispatcher, BtSettingsAppSceneStart);
    app->scene_manager = view_dispatcher_get_scene_manager(app->view_dispatcher);
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    app->var_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        BtSettingsAppViewVarItemList,
        variable_item_list_get_view(app->var_item_list));

    return app;
}

void bt_settings_app_free(BtSettingsApp* app) {
    furi_assert(app);
    // Variable item list
    view_dispatcher_remove_view(app->view_dispatcher, BtSettingsAppViewVarItemList);
    variable_item_list_free(app->var_item_list);
    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);
    // Records
    furi_record_close("gui");
    free(app);
}

extern int32_t bt_settings_app(void* p) {
    BtSettingsApp* app = bt_settings_app_alloc();
    view_dispatcher_run(app->view_dispatcher);
    bt_settings_save(&app->settings);
    bt_settings_app_free(app);
    return 0;
}
