#include "tullave_co_i.h"

#define LOG_TAG "TuLlaveCO"

static const NotificationSequence tullave_seq_blink_start = {
    &message_blink_start_10,
    &message_blink_set_color_blue,
    &message_do_not_reset,
    NULL,
};

static const NotificationSequence tullave_seq_blink_stop = {
    &message_blink_stop,
    NULL,
};

void tullave_blink_start(TuLlave* t_llave) {
    notification_message(t_llave->notifications, &tullave_seq_blink_start);
}

void tullave_blink_stop(TuLlave* t_llave) {
    notification_message(t_llave->notifications, &tullave_seq_blink_stop);
}

bool tullave_co_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    TuLlave* t_llave = context;
    return scene_manager_handle_custom_event(t_llave->scene_manager, event);
}

bool tullave_co_back_event_callback(void* context) {
    furi_assert(context);
    TuLlave* t_llave = context;
    return scene_manager_handle_back_event(t_llave->scene_manager);
}

void tullave_co_tick_event_callback(void* context) {
    furi_assert(context);
    TuLlave* t_llave = context;
    scene_manager_handle_tick_event(t_llave->scene_manager);
}

TuLlave* tullave_alloc() {
    FURI_LOG_D(LOG_TAG, "Allocating User Inferface required memory");

    TuLlave* t_llave = malloc(sizeof(TuLlave));

    t_llave->worker = tullave_worker_alloc();

    // Allocate space for view dispatcher and scene manager
    t_llave->view_dispatcher = view_dispatcher_alloc();
    t_llave->scene_manager = scene_manager_alloc(&tullave_co_scene_handlers, t_llave);

    view_dispatcher_enable_queue(t_llave->view_dispatcher);

    //Set the events handler for GUI
    view_dispatcher_set_event_callback_context(t_llave->view_dispatcher, t_llave);

    view_dispatcher_set_custom_event_callback(
        t_llave->view_dispatcher, tullave_co_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        t_llave->view_dispatcher, tullave_co_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        t_llave->view_dispatcher, tullave_co_tick_event_callback, 100);

    // Open GUI record
    t_llave->gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(
        t_llave->view_dispatcher, t_llave->gui, ViewDispatcherTypeFullscreen);

    // Notifications
    t_llave->notifications = furi_record_open(RECORD_NOTIFICATION);

    // Main Menu
    t_llave->submenu = submenu_alloc();
    view_dispatcher_add_view(
        t_llave->view_dispatcher, TuLlaveViewMenu, submenu_get_view(t_llave->submenu));

    // Popup
    t_llave->popup = popup_alloc();
    view_dispatcher_add_view(
        t_llave->view_dispatcher, TuLlaveViewPopup, popup_get_view(t_llave->popup));

    FURI_LOG_D(LOG_TAG, "Main Allocation Finished");

    return t_llave;
}

void tullave_free(TuLlave* t_llave) {
    furi_assert(t_llave);

    FURI_LOG_D(LOG_TAG, "Freeing User Interface Memory");

    // Submenu
    view_dispatcher_remove_view(t_llave->view_dispatcher, TuLlaveViewMenu);
    submenu_free(t_llave->submenu);

    // Popup
    view_dispatcher_remove_view(t_llave->view_dispatcher, TuLlaveViewPopup);
    popup_free(t_llave->popup);

    // Notifications7
    furi_record_close(RECORD_NOTIFICATION);
    t_llave->notifications = NULL;

    // Stop and free the worker
    tullave_worker_stop(t_llave->worker);
    tullave_worker_free(t_llave->worker);

    scene_manager_free(t_llave->scene_manager);
    view_dispatcher_free(t_llave->view_dispatcher);

    // GUI
    furi_record_close(RECORD_GUI);
    t_llave->gui = NULL;

    FURI_LOG_D(LOG_TAG, "Main Free Finished");

    free(t_llave);
}

int32_t tullave_co_main(void* p) {
    UNUSED(p);

    FURI_LOG_D(LOG_TAG, "Initializing TuLlave CO User Application");

    TuLlave* t_llave = tullave_alloc();

    scene_manager_next_scene(t_llave->scene_manager, TuLlaveSceneStart);
    view_dispatcher_run(t_llave->view_dispatcher);

    tullave_free(t_llave);
    return 0;
}