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
    app->dialog = dialog_ex_alloc();
    app->text_input = text_input_alloc();
    app->file_path = furi_string_alloc();
    app->file_browser = file_browser_alloc(app->file_path);
    app->text_box = text_box_alloc();
    app->text_box_store = furi_string_alloc();

    file_browser_configure(app->file_browser, "*", NULL, true, false, &I_badusb_10px, true);

    view_dispatcher_add_view(
        app->view_dispatcher, FcomMainMenuView, submenu_get_view(app->submenu));

    view_dispatcher_add_view(
        app->view_dispatcher, FcomReadCodeView, dialog_ex_get_view(app->dialog));

    view_dispatcher_add_view(
        app->view_dispatcher, FcomSendCodeView, dialog_ex_get_view(app->dialog));

    view_dispatcher_add_view(
        app->view_dispatcher, FcomKeyboardView, text_input_get_view(app->text_input));

    view_dispatcher_add_view(
        app->view_dispatcher, FcomSerialView, text_box_get_view(app->text_box));

    view_dispatcher_add_view(
        app->view_dispatcher, FcomFileSelectView, file_browser_get_view(app->file_browser));

    return app;
}

AppState* app_state_alloc() {
    AppState* state = malloc(sizeof(AppState));

    // Allocate and start dcomm
 
    return state;
}

void app_quit(App* app) {
    scene_manager_stop(app->scene_manager);
}

void app_free(App* app) {
    furi_assert(app);

    // stop and deallocate usb serial if enabled
    // Stop and deallocate dcomm

    app->notification = NULL;

    free(app->state);

    view_dispatcher_remove_view(app->view_dispatcher, FcomMainMenuView);
    view_dispatcher_remove_view(app->view_dispatcher, FcomReadCodeView);
    view_dispatcher_remove_view(app->view_dispatcher, FcomSendCodeView);
    view_dispatcher_remove_view(app->view_dispatcher, FcomKeyboardView);
    view_dispatcher_remove_view(app->view_dispatcher, FcomSerialView);
    view_dispatcher_remove_view(app->view_dispatcher, FcomFileSelectView);
    submenu_free(app->submenu);
    widget_free(app->widget);
    

    dialog_ex_free(app->dialog);
    text_box_free(app->text_box);
    furi_string_free(app->text_box_store);

    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);


    free(app);
}
