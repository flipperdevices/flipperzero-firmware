#include "flipper.h"
#include "app_state.h"
#include "scenes/scenes.h"
#include "item.h"

App* app_alloc() {
    App* app = malloc(sizeof(App));
    app->scene_manager = scene_manager_alloc(&app_scene_handlers, app);
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, app_scene_custom_callback);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, app_back_event_callback);

    // Create our UI elements
    app->btn_menu = button_menu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, SR_ButtonMenu, button_menu_get_view(app->btn_menu));

    // Storage
    app->storage = furi_record_open(RECORD_STORAGE);

    // Notifications - for LED light access
    app->notifications = furi_record_open(RECORD_NOTIFICATION);

    // initialize device items list
    app->depth = 0;
    app->selected_item = -1;

    app->items_view = item_get_items_view_from_path(app, NULL);

    return app;
}

void app_free(App* app) {
    furi_assert(app);

    item_items_view_free(app->items_view);

    view_dispatcher_remove_view(app->view_dispatcher, SR_ButtonMenu);

    button_menu_free(app->btn_menu);
    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);

    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_NOTIFICATION);

    free(app);
}