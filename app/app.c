#include "app.h"
#include "../scenes/scenes.h"
#include "../scenes/scene_manager.h"
#include "../callbacks/callbacks.h"

App* app_alloc() {
    App* app = calloc(1, sizeof(App));
    app->scene_manager = scene_manager_alloc(&book_scene_manager_handlers, app);
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, book_back_event_callback);

    app->submenu = submenu_alloc();
    view_dispatcher_add_view(app->view_dispatcher, BookSubmenuView, submenu_get_view(app->submenu));
    
    app->widget = widget_alloc();
    view_dispatcher_add_view(app->view_dispatcher, BookWidgetView, widget_get_view(app->widget));
    return app;
}

void app_free(App* app) {
    furi_assert(app);
    view_dispatcher_remove_view(app->view_dispatcher, BookSubmenuView);
    view_dispatcher_remove_view(app->view_dispatcher, BookWidgetView);
    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);
    submenu_free(app->submenu);
    widget_free(app->widget);
    free(app);
}

