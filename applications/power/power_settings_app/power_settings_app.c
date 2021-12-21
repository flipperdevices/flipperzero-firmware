#include "power_settings_app.h"

PowerSettingsApp* power_settings_app_alloc() {
    PowerSettingsApp* app = furi_alloc(sizeof(PowerSettingsApp));

    // Records
    app->gui = furi_record_open("gui");
    app->power = furi_record_open("power");

    // View dispatcher
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_allocate_scene_manager(
        app->view_dispatcher, &power_settings_scene_handlers, app);
    view_dispatcher_set_start_scene(app->view_dispatcher, PowerSettingsAppSceneStart);
    app->scene_manager = view_dispatcher_get_scene_manager(app->view_dispatcher);
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_tick_event_period(app->view_dispatcher, 2000);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    // Views
    app->batery_info = battery_info_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        PowerSettingsAppViewBatteryInfo,
        battery_info_get_view(app->batery_info));
    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, PowerSettingsAppViewSubmenu, submenu_get_view(app->submenu));
    app->dialog = dialog_ex_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, PowerSettingsAppViewDialog, dialog_ex_get_view(app->dialog));

    return app;
}

void power_settings_app_free(PowerSettingsApp* app) {
    furi_assert(app);
    // Views
    view_dispatcher_remove_view(app->view_dispatcher, PowerSettingsAppViewBatteryInfo);
    battery_info_free(app->batery_info);
    view_dispatcher_remove_view(app->view_dispatcher, PowerSettingsAppViewSubmenu);
    submenu_free(app->submenu);
    view_dispatcher_remove_view(app->view_dispatcher, PowerSettingsAppViewDialog);
    dialog_ex_free(app->dialog);
    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);
    // Records
    furi_record_close("power");
    furi_record_close("gui");
    free(app);
}

extern int32_t power_settings_app(void* p) {
    PowerSettingsApp* app = power_settings_app_alloc();
    view_dispatcher_run(app->view_dispatcher);
    power_settings_app_free(app);
    return 0;
}
