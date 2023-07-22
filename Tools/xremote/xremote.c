#include "xremote.h"

bool xremote_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    XRemote* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

void xremote_tick_event_callback(void* context) {
    furi_assert(context);
    XRemote* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

//leave app if back button pressed
bool xremote_navigation_event_callback(void* context) {
    furi_assert(context);
    XRemote* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

XRemote* xremote_app_alloc() {
    XRemote* app = malloc(sizeof(XRemote));
    app->gui = furi_record_open(RECORD_GUI);
    app->notification = furi_record_open(RECORD_NOTIFICATION);

    //Turn backlight on, believe me this makes testing your app easier
    notification_message(app->notification, &sequence_display_backlight_on);

    //Scene additions
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);

    app->scene_manager = scene_manager_alloc(&xremote_scene_handlers, app);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, xremote_navigation_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, xremote_tick_event_callback, 100);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, xremote_custom_event_callback);
    app->submenu = submenu_alloc();
    app->editmenu = submenu_alloc();

    // Set defaults, in case no config loaded
    app->haptic = 1;
    app->speaker = 1;
    app->led = 1;
    app->save_settings = 1;
    app->transmitting = 0;

    // Load configs
    xremote_read_settings(app);

    app->dialogs = furi_record_open(RECORD_DIALOGS);
    app->file_path = furi_string_alloc();

    app->ir_remote_buffer = xremote_ir_remote_alloc();
    app->ir_worker = infrared_worker_alloc();
    app->cross_remote = cross_remote_alloc();

    app->sg_remote_buffer = xremote_sg_remote_alloc();

    app->loading = loading_alloc();

    app->text_input = text_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, XRemoteViewIdTextInput, text_input_get_view(app->text_input));

    view_dispatcher_add_view(
        app->view_dispatcher, XRemoteViewIdMenu, submenu_get_view(app->submenu));
    view_dispatcher_add_view(
        app->view_dispatcher, XRemoteViewIdEditItem, submenu_get_view(app->editmenu));
    app->xremote_infoscreen = xremote_infoscreen_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        XRemoteViewIdInfoscreen,
        xremote_infoscreen_get_view(app->xremote_infoscreen));
    app->xremote_transmit = xremote_transmit_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        XRemoteViewIdTransmit,
        xremote_transmit_get_view(app->xremote_transmit));
    app->xremote_pause_set = xremote_pause_set_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        XRemoteViewIdPauseSet,
        xremote_pause_set_get_view(app->xremote_pause_set));
    app->button_menu_create = button_menu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, XRemoteViewIdCreate, button_menu_get_view(app->button_menu_create));
    app->button_menu_create_add = button_menu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        XRemoteViewIdCreateAdd,
        button_menu_get_view(app->button_menu_create_add));
    app->button_menu_ir = button_menu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, XRemoteViewIdIrRemote, button_menu_get_view(app->button_menu_ir));
    app->variable_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        XRemoteViewIdSettings,
        variable_item_list_get_view(app->variable_item_list));

    app->popup = popup_alloc();
    view_dispatcher_add_view(app->view_dispatcher, XRemoteViewIdWip, popup_get_view(app->popup));
    app->view_stack = view_stack_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, XRemoteViewIdStack, view_stack_get_view(app->view_stack));

    //End Scene Additions

    return app;
}

void xremote_show_loading_popup(XRemote* app, bool show) {
    TaskHandle_t timer_task = xTaskGetHandle(configTIMER_SERVICE_TASK_NAME);
    ViewStack* view_stack = app->view_stack;
    Loading* loading = app->loading;

    if(show) {
        // Raise timer priority so that animations can play
        vTaskPrioritySet(timer_task, configMAX_PRIORITIES - 1);
        view_stack_add_view(view_stack, loading_get_view(loading));
    } else {
        view_stack_remove_view(view_stack, loading_get_view(loading));
        // Restore default timer priority
        vTaskPrioritySet(timer_task, configTIMER_TASK_PRIORITY);
    }
}

void xremote_app_free(XRemote* app) {
    furi_assert(app);

    // Scene manager
    scene_manager_free(app->scene_manager);

    infrared_worker_free(app->ir_worker);

    // View Dispatcher
    view_dispatcher_remove_view(app->view_dispatcher, XRemoteViewIdMenu);
    view_dispatcher_remove_view(app->view_dispatcher, XRemoteViewIdCreate);
    view_dispatcher_remove_view(app->view_dispatcher, XRemoteViewIdCreateAdd);
    view_dispatcher_remove_view(app->view_dispatcher, XRemoteViewIdSettings);
    view_dispatcher_remove_view(app->view_dispatcher, XRemoteViewIdWip);
    view_dispatcher_remove_view(app->view_dispatcher, XRemoteViewIdStack);
    view_dispatcher_remove_view(app->view_dispatcher, XRemoteViewIdTextInput);
    view_dispatcher_remove_view(app->view_dispatcher, XRemoteViewIdTransmit);
    view_dispatcher_remove_view(app->view_dispatcher, XRemoteViewIdPauseSet);
    text_input_free(app->text_input);
    button_menu_free(app->button_menu_create);
    button_menu_free(app->button_menu_create_add);
    button_menu_free(app->button_menu_ir);
    view_stack_free(app->view_stack);
    popup_free(app->popup);
    submenu_free(app->submenu);

    view_dispatcher_free(app->view_dispatcher);
    furi_record_close(RECORD_GUI);

    app->gui = NULL;
    app->notification = NULL;

    //Remove whatever is left
    free(app);
}

void xremote_popup_closed_callback(void* context) {
    furi_assert(context);
    XRemote* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, XRemoteCustomEventTypePopupClosed);
}

void xremote_text_input_callback(void* context) {
    furi_assert(context);
    XRemote* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, XRemoteCustomEventTextInput);
}

int32_t xremote_app(void* p) {
    UNUSED(p);
    XRemote* app = xremote_app_alloc();

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    //scene_manager_next_scene(app->scene_manager, XRemoteSceneInfoscreen); //Start with start screen
    scene_manager_next_scene(
        app->scene_manager, XRemoteSceneMenu); //if you want to directly start with Menu

    furi_hal_power_suppress_charge_enter();

    view_dispatcher_run(app->view_dispatcher);

    xremote_save_settings(app);

    furi_hal_power_suppress_charge_exit();
    xremote_app_free(app);

    return 0;
}
