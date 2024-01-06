#include "app_state.h"

App* app_alloc() {
    App* app = malloc(sizeof(App));
    app->scene_manager = scene_manager_alloc(&fcom_scene_manager_handlers, app);
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, fcom_custom_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, fcom_back_event_callback);

    app->submenu = submenu_alloc();
    app->widget = widget_alloc();

    view_dispatcher_add_view(
        app->view_dispatcher, FcomSubmenuView, submenu_get_view(app->submenu));

    return app;
}

AppState* app_state_alloc() {
    AppState* state = malloc(sizeof(AppState));
    return state;
}

void app_quit(App* app) {
    scene_manager_stop(app->scene_manager);
}

void app_free(App* app) {
    furi_assert(app);

    free(app->state);

    view_dispatcher_remove_view(app->view_dispatcher, FcomSubmenuView);

    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);

    submenu_free(app->submenu);
    widget_free(app->widget);

    free(app);
}
