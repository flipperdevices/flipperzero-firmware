#include "flipper.h"
#include "app_state.h"
#include "scenes.h"

App* app_alloc() {
    App* app = malloc(sizeof(App));
    app->scene_manager = scene_manager_alloc(&resistors_scene_manager_handlers, app);
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, resistors_custom_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, resistors_back_event_callback);

    app->submenu = submenu_alloc();
    app->widget = widget_alloc();

    view_dispatcher_add_view(
        app->view_dispatcher, ResistorsSubmenuView, submenu_get_view(app->submenu));
    view_dispatcher_add_view(
        app->view_dispatcher, ResistorsEditView, widget_get_view(app->widget));

    return app;
}

AppState* app_state_alloc() {
    AppState* state = malloc(sizeof(AppState));
    state->edit_selection = 0;
    return state;
}

void app_free(App* app) {
    furi_assert(app);

    free(app->state);

    view_dispatcher_remove_view(app->view_dispatcher, ResistorsSubmenuView);
    view_dispatcher_remove_view(app->view_dispatcher, ResistorsEditView);

    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);

    submenu_free(app->submenu);
    widget_free(app->widget);

    free(app);
}
