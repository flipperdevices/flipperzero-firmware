#include "app_state.h"

/*
TODO:
- convert dmcomm loop into a furi thread
- replace serial read/write with furi notification messages

- create and start USB serial thread in serial bit
- create thread to link USB serial to dmcomm

*/

App* app_alloc() {
    App* app = malloc(sizeof(App));

    app->storage = furi_record_open(RECORD_STORAGE);
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

    app->dmcomm_mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    app->dmcomm_stream_buffer = furi_stream_buffer_alloc(128, 1);
    app->dmcomm_output_buffer = furi_string_alloc();

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


    setApp(app);

    app->dmcomm_run = true;
    app->dcomm_thread = furi_thread_alloc();
    furi_thread_set_context(app->dcomm_thread, app);
    furi_thread_set_priority(app->dcomm_thread, FuriThreadPriorityHigh);
    furi_thread_set_stack_size(app->dcomm_thread, 8 * 1024);
    furi_thread_set_name(app->dcomm_thread, "DMCOMMWorker");
    furi_thread_set_callback(app->dcomm_thread, dmcomm_reader);
    furi_thread_start(app->dcomm_thread);

    return app;
}

AppState* app_state_alloc() {
    AppState* state = malloc(sizeof(AppState));

    // Allocate and start dcomm
    state->usbSerialEnabled = false;
    state->current_code[0] = 0;
    state->file_name_tmp[0] = 0;
 
    return state;
}

void app_quit(App* app) {
    scene_manager_stop(app->scene_manager);
}

void app_free(App* app) {
    furi_assert(app);

    // Stop and deallocate usb serial if enabled
    // Stop and deallocate dcomm

    app->dmcomm_run = false;
    furi_thread_join(app->dcomm_thread);
    furi_thread_free(app->dcomm_thread);

    furi_mutex_free(app->dmcomm_mutex);
    furi_stream_buffer_free(app->dmcomm_stream_buffer);
    furi_string_free(app->dmcomm_output_buffer);

    furi_record_close(RECORD_NOTIFICATION);
    app->notification = NULL;
    furi_record_close(RECORD_STORAGE);
    app->storage = NULL;

    free(app->state);

    view_dispatcher_remove_view(app->view_dispatcher, FcomMainMenuView);
    view_dispatcher_remove_view(app->view_dispatcher, FcomReadCodeView);
    view_dispatcher_remove_view(app->view_dispatcher, FcomSendCodeView);
    view_dispatcher_remove_view(app->view_dispatcher, FcomKeyboardView);
    view_dispatcher_remove_view(app->view_dispatcher, FcomSerialView);
    view_dispatcher_remove_view(app->view_dispatcher, FcomFileSelectView);

    submenu_free(app->submenu);
    dialog_ex_free(app->dialog);
    text_input_free(app->text_input);
    furi_string_free(app->file_path);
    file_browser_free(app->file_browser);
    text_box_free(app->text_box);
    furi_string_free(app->text_box_store);

    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);

    free(app);
}
