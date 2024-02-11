#include "tullave_i.h"

/**
 * LOG_TAG to identify TuLlave logs in the logstream
*/
#define LOG_TAG "TuLlave"

bool tullave_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    TuLlaveApp* t_llave = context;
    return scene_manager_handle_custom_event(t_llave->scene_manager, event);
}

bool tullave_back_event_callback(void* context) {
    furi_assert(context);
    TuLlaveApp* t_llave = context;
    return scene_manager_handle_back_event(t_llave->scene_manager);
}

void tullave_tick_event_callback(void* context) {
    furi_assert(context);
    TuLlaveApp* t_llave = context;

    scene_manager_handle_tick_event(t_llave->scene_manager);
}

TuLlaveApp* tullave_alloc() {
    FURI_LOG_D(LOG_TAG, "Allocating User Inferface required memory");

    TuLlaveApp* instance = malloc(sizeof(TuLlaveApp));

    instance->nfc = nfc_alloc();
    // Nfc device
    instance->nfc_device = nfc_device_alloc();

    // Allocate space for view dispatcher and scene manager
    instance->view_dispatcher = view_dispatcher_alloc();
    instance->scene_manager = scene_manager_alloc(&tullave_scene_handlers, instance);

    view_dispatcher_enable_queue(instance->view_dispatcher);

    //Set the events handler for GUI
    view_dispatcher_set_event_callback_context(instance->view_dispatcher, instance);

    view_dispatcher_set_custom_event_callback(
        instance->view_dispatcher, tullave_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        instance->view_dispatcher, tullave_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        instance->view_dispatcher, tullave_tick_event_callback, 100);

    // Open GUI record
    instance->gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(
        instance->view_dispatcher, instance->gui, ViewDispatcherTypeFullscreen);

    // Notifications
    instance->notifications = furi_record_open(RECORD_NOTIFICATION);

    // Main Menu
    instance->submenu = submenu_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher, TuLlaveViewMenu, submenu_get_view(instance->submenu));

    // Popup
    instance->popup = popup_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher, TuLlaveViewPopup, popup_get_view(instance->popup));

    // Custom widget
    instance->widget = widget_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher, TuLlaveViewWidget, widget_get_view(instance->widget));

    FURI_LOG_D(LOG_TAG, "Main Allocation Finished");

    return instance;
}

void tullave_free(TuLlaveApp* instance) {
    furi_assert(instance);

    FURI_LOG_D(LOG_TAG, "Main Free was invoked");

    // Nfc Objects
    nfc_free(instance->nfc);
    nfc_device_free(instance->nfc_device);

    // Submenu
    view_dispatcher_remove_view(instance->view_dispatcher, TuLlaveViewMenu);
    submenu_free(instance->submenu);

    // Popup
    view_dispatcher_remove_view(instance->view_dispatcher, TuLlaveViewPopup);
    popup_free(instance->popup);

    // Widget
    view_dispatcher_remove_view(instance->view_dispatcher, TuLlaveViewWidget);
    widget_free(instance->widget);

    // Notifications
    furi_record_close(RECORD_NOTIFICATION);
    instance->notifications = NULL;

    scene_manager_free(instance->scene_manager);
    view_dispatcher_free(instance->view_dispatcher);

    // GUI
    furi_record_close(RECORD_GUI);
    instance->gui = NULL;

    FURI_LOG_D(LOG_TAG, "Main Free Finished");

    free(instance);
}

int32_t tullave_main(void* p) {
    UNUSED(p);

    FURI_LOG_D(LOG_TAG, "Initializing TuLlave User Application");

    TuLlaveApp* instance = tullave_alloc();

    scene_manager_next_scene(instance->scene_manager, TuLlaveSceneStart);
    view_dispatcher_run(instance->view_dispatcher);

    tullave_free(instance);
    return 0;
}