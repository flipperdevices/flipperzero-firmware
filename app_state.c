#include "app_state.h"

App* app_alloc() {
    App* app = malloc(sizeof(App));

    app->notification = furi_record_open(RECORD_NOTIFICATION);
    
    //Turn backlight on, believe me this makes testing your app easier
    notification_message(app->notification, &sequence_display_backlight_on);

    app->scene_manager = scene_manager_alloc(&fcom_scene_manager_handlers, app);
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, fcom_custom_callback);
    
    // Wire back button to scene manager
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, fcom_back_event_callback);

    //Allocate our submenu and add the view
    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, FcomSubmenuView, submenu_get_view(app->submenu));

    //Allocate our dialog and add the view
    app->dialog = dialog_ex_alloc();
    app->widget = widget_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, FcomHCSR04View, dialog_ex_get_view(app->dialog));

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

    app->notification = NULL;

    free(app->state);

    view_dispatcher_remove_view(app->view_dispatcher, FcomSubmenuView);
    submenu_free(app->submenu);

    view_dispatcher_remove_view(app->view_dispatcher, FcomHCSR04View);
    widget_free(app->widget);

    dialog_ex_free(app->dialog);

    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);


    free(app);
}
