#include "ac_remote_app_i.h"

#include <furi.h>
#include <furi_hal.h>

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

    app->view_stack = view_stack_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, AC_RemoteAppViewStack, view_stack_get_view(app->view_stack));

    app->ac_remote_panel = ac_remote_panel_alloc();

    scene_manager_next_scene(app->scene_manager, AC_RemoteSceneMitsubishi);
    return app;
}

void ac_remote_app_free(AC_RemoteApp* app) {
    furi_assert(app);

    // Views
    view_dispatcher_remove_view(app->view_dispatcher, AC_RemoteAppViewStack);

    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);
    view_stack_free(app->view_stack);
    ac_remote_panel_free(app->ac_remote_panel);
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
