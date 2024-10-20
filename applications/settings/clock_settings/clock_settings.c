#include "clock_settings.h"

#include <furi.h>
#include <furi_hal.h>

static bool clock_settings_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    ClockSettings* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool clock_settings_back_event_callback(void* context) {
    furi_assert(context);
    ClockSettings* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

ClockSettings* clock_settings_alloc() {
    ClockSettings* app = malloc(sizeof(ClockSettings));

    app->gui = furi_record_open(RECORD_GUI);

    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&clock_settings_scene_handlers, app);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, clock_settings_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, clock_settings_back_event_callback);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    app->pwm_view =
        clock_settings_module_alloc(view_dispatcher_get_event_loop(app->view_dispatcher));
    view_dispatcher_add_view(
        app->view_dispatcher, ClockSettingsViewPwm, clock_settings_module_get_view(app->pwm_view));

    scene_manager_next_scene(app->scene_manager, ClockSettingsSceneStart);

    return app;
}

void clock_settings_free(ClockSettings* app) {
    furi_assert(app);

    // Views
    view_dispatcher_remove_view(app->view_dispatcher, ClockSettingsViewPwm);

    clock_settings_module_free(app->pwm_view);

    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    // Close records
    furi_record_close(RECORD_GUI);

    free(app);
}

int32_t clock_settings(void* p) {
    UNUSED(p);
    ClockSettings* clock_settings = clock_settings_alloc();

    view_dispatcher_run(clock_settings->view_dispatcher);

    clock_settings_free(clock_settings);

    return 0;
}
