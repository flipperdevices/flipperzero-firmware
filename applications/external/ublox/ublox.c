#include "ublox_i.h"

bool ublox_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    Ublox* ublox = context;
    return scene_manager_handle_custom_event(ublox->scene_manager, event);
}

bool ublox_back_event_callback(void* context) {
    furi_assert(context);
    Ublox* ublox = context;
    return scene_manager_handle_back_event(ublox->scene_manager);
}

Ublox* ublox_alloc() {
    Ublox* ublox = malloc(sizeof(Ublox));

    ublox->view_dispatcher = view_dispatcher_alloc();
    ublox->scene_manager = scene_manager_alloc(&ublox_scene_handlers, ublox);
    view_dispatcher_enable_queue(ublox->view_dispatcher);
    view_dispatcher_set_event_callback_context(ublox->view_dispatcher, ublox);
    view_dispatcher_set_custom_event_callback(ublox->view_dispatcher, ublox_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        ublox->view_dispatcher, ublox_back_event_callback);

    ublox->worker = ublox_worker_alloc();

    ublox->gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(
        ublox->view_dispatcher, ublox->gui, ViewDispatcherTypeFullscreen);

    ublox->submenu = submenu_alloc();
    view_dispatcher_add_view(
        ublox->view_dispatcher, UbloxViewMenu, submenu_get_view(ublox->submenu));

    ublox->widget = widget_alloc();
    view_dispatcher_add_view(
        ublox->view_dispatcher, UbloxViewWidget, widget_get_view(ublox->widget));

    ublox->variable_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        ublox->view_dispatcher,
        UbloxViewVariableItemList,
        variable_item_list_get_view(ublox->variable_item_list));

    ublox->text_input = text_input_alloc();
    view_dispatcher_add_view(
        ublox->view_dispatcher, UbloxViewTextInput, text_input_get_view(ublox->text_input));

    ublox->data_display = data_display_alloc();
    view_dispatcher_add_view(
        ublox->view_dispatcher, UbloxViewDataDisplay, data_display_get_view(ublox->data_display));

    ublox->notifications = furi_record_open(RECORD_NOTIFICATION);
    ublox->storage = furi_record_open(RECORD_STORAGE);

    ublox->log_state = UbloxLogStateNone;
    // Establish default data display state
    (ublox->data_display_state).view_mode = UbloxDataDisplayViewModeHandheld;
    (ublox->data_display_state).backlight_mode = UbloxDataDisplayBacklightDefault;
    (ublox->data_display_state).refresh_rate = 2;
    (ublox->data_display_state).notify_mode = UbloxDataDisplayNotifyOn;

    (ublox->device_state).odometer_mode = UbloxOdometerModeRunning;
    // "suitable for most applications" according to u-blox.
    (ublox->device_state).platform_model = UbloxPlatformModelPortable;
    ublox->gps_initted = false;
    return ublox;
}

void ublox_free(Ublox* ublox) {
    furi_assert(ublox);

    ublox_worker_stop(ublox->worker);
    ublox_worker_free(ublox->worker);

    view_dispatcher_remove_view(ublox->view_dispatcher, UbloxViewMenu);
    submenu_free(ublox->submenu);

    view_dispatcher_remove_view(ublox->view_dispatcher, UbloxViewWidget);
    widget_free(ublox->widget);

    view_dispatcher_remove_view(ublox->view_dispatcher, UbloxViewDataDisplay);
    data_display_free(ublox->data_display);

    view_dispatcher_remove_view(ublox->view_dispatcher, UbloxViewVariableItemList);
    variable_item_list_free(ublox->variable_item_list);

    view_dispatcher_free(ublox->view_dispatcher);

    scene_manager_free(ublox->scene_manager);

    furi_record_close(RECORD_GUI);
    ublox->gui = NULL;
    furi_record_close(RECORD_NOTIFICATION);
    ublox->notifications = NULL;
    furi_record_close(RECORD_STORAGE);
    ublox->storage = NULL;

    free(ublox);
}

int32_t ublox_app(void* p) {
    UNUSED(p);

    Ublox* ublox = ublox_alloc();

    scene_manager_next_scene(ublox->scene_manager, UbloxSceneStart);

    view_dispatcher_run(ublox->view_dispatcher);

    // force restore the default backlight on exit
    notification_message_block(ublox->notifications, &sequence_display_backlight_enforce_auto);

    ublox_free(ublox);

    return 0;
}
