#include "desktop_settings_app.h"

DesktopSettingsApp* desktop_settings_app_alloc() {
    DesktopSettingsApp* app = furi_alloc(sizeof(DesktopSettingsApp));

    app->gui = furi_record_open("gui");
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_allocate_scene_manager(
        app->view_dispatcher, &desktop_settings_scene_handlers, app);
    app->scene_manager = view_dispatcher_get_scene_manager(app->view_dispatcher);
    view_dispatcher_set_start_scene(app->view_dispatcher, DesktopSettingsAppSceneStart);
    view_dispatcher_enable_queue(app->view_dispatcher);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, DesktopSettingsAppViewMenu, submenu_get_view(app->submenu));

    app->code_input = code_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        DesktopSettingsAppViewPincodeInput,
        code_input_get_view(app->code_input));

    return app;
}

void desktop_settings_app_free(DesktopSettingsApp* app) {
    furi_assert(app);
    // Variable item list
    view_dispatcher_remove_view(app->view_dispatcher, DesktopSettingsAppViewMenu);
    submenu_free(app->submenu);
    view_dispatcher_remove_view(app->view_dispatcher, DesktopSettingsAppViewPincodeInput);
    code_input_free(app->code_input);
    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);
    // Records
    furi_record_close("gui");
    free(app);
}

extern int32_t desktop_settings_app(void* p) {
    DesktopSettingsApp* app = desktop_settings_app_alloc();
    LOAD_DESKTOP_SETTINGS(&app->settings);
    view_dispatcher_run(app->view_dispatcher);
    desktop_settings_app_free(app);
    return 0;
}
