#include "nfc_relay.h"

#define TAG "NfcRelay"

bool nfc_relay_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    NfcRelay* nfc_relay = context;
    return scene_manager_handle_custom_event(nfc_relay->scene_manager, event);
}

bool nfc_relay_back_event_callback(void* context) {
    furi_assert(context);
    FURI_LOG_D(TAG, "back_event_callback");
    NfcRelay* nfc_relay = context;
    return scene_manager_handle_back_event(nfc_relay->scene_manager);
}

void nfc_relay_tick_event_callback(void* context) {
    furi_assert(context);
    NfcRelay* nfc_relay = context;
    scene_manager_handle_tick_event(nfc_relay->scene_manager);
}

void nfc_relay_show_loading_popup(void* context, bool show) {
    NfcRelay* nfc_relay = context;
    TaskHandle_t timer_task = xTaskGetHandle(configTIMER_SERVICE_TASK_NAME);

    if(show) {
        // Raise timer priority so that animations can play
        vTaskPrioritySet(timer_task, configMAX_PRIORITIES - 1);
        view_dispatcher_switch_to_view(nfc_relay->view_dispatcher, NfcRelayViewLoading);
    } else {
        // Restore default timer priority
        vTaskPrioritySet(timer_task, configTIMER_TASK_PRIORITY);
    }
}

NfcRelay* nfc_relay_alloc() {
    NfcRelay* nfc_relay = malloc(sizeof(NfcRelay));
    nfc_relay->view_dispatcher = view_dispatcher_alloc();
    nfc_relay->scene_manager = scene_manager_alloc(&nfc_relay_scene_handlers, nfc_relay);
    view_dispatcher_enable_queue(nfc_relay->view_dispatcher);
    view_dispatcher_set_event_callback_context(nfc_relay->view_dispatcher, nfc_relay);
    view_dispatcher_set_custom_event_callback(
        nfc_relay->view_dispatcher, nfc_relay_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        nfc_relay->view_dispatcher, nfc_relay_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        nfc_relay->view_dispatcher, nfc_relay_tick_event_callback, 100);

    view_dispatcher_set_navigation_event_callback(
        nfc_relay->view_dispatcher, nfc_relay_back_event_callback);

    nfc_relay->gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(
        nfc_relay->view_dispatcher, nfc_relay->gui, ViewDispatcherTypeFullscreen);
    // Open Notification record
    nfc_relay->notifications = furi_record_open(RECORD_NOTIFICATION);

    // Submenu
    nfc_relay->submenu = submenu_alloc();
    view_dispatcher_add_view(
        nfc_relay->view_dispatcher, NfcRelayViewMenu, submenu_get_view(nfc_relay->submenu));

    // Popup
    nfc_relay->popup = popup_alloc();
    view_dispatcher_add_view(
        nfc_relay->view_dispatcher, NfcRelayViewPopup, popup_get_view(nfc_relay->popup));

    // Loading
    nfc_relay->loading = loading_alloc();
    view_dispatcher_add_view(
        nfc_relay->view_dispatcher, NfcRelayViewLoading, loading_get_view(nfc_relay->loading));

    // Text Input
    nfc_relay->text_input = text_input_alloc();
    view_dispatcher_add_view(
        nfc_relay->view_dispatcher,
        NfcRelayViewTextInput,
        text_input_get_view(nfc_relay->text_input));

    nfc_relay->widget = widget_alloc();
    view_dispatcher_add_view(
        nfc_relay->view_dispatcher, NfcRelayViewWidget, widget_get_view(nfc_relay->widget));

    // TODO: not static config
    nfc_relay->config = malloc(sizeof(NfcRelayConfig));
    nfc_relay->config->mode = NfcRelayModeUart;
    nfc_relay->config->uart_config.baudrate = 38400; //115200;
    nfc_relay->config->uart_config.uartId = FuriHalUartIdLPUART1;

    return nfc_relay;
}

void nfc_relay_free(NfcRelay* nfc_relay) {
    furi_assert(nfc_relay);

    // view
    view_dispatcher_remove_view(nfc_relay->view_dispatcher, NfcRelayViewMenu);
    submenu_free(nfc_relay->submenu);

    view_dispatcher_remove_view(nfc_relay->view_dispatcher, NfcRelayViewPopup);
    popup_free(nfc_relay->popup);

    view_dispatcher_remove_view(nfc_relay->view_dispatcher, NfcRelayViewLoading);
    loading_free(nfc_relay->loading);

    view_dispatcher_remove_view(nfc_relay->view_dispatcher, NfcRelayViewTextInput);
    text_input_free(nfc_relay->text_input);

    view_dispatcher_remove_view(nfc_relay->view_dispatcher, NfcRelayViewWidget);
    widget_free(nfc_relay->widget);
    view_dispatcher_free(nfc_relay->view_dispatcher);

    scene_manager_free(nfc_relay->scene_manager);

    furi_record_close(RECORD_GUI);
    nfc_relay->gui = NULL;

    furi_record_close(RECORD_NOTIFICATION);
    nfc_relay->notifications = NULL;

    free(nfc_relay);
}

int32_t nfc_relay_app(void* p) {
    UNUSED(p);
    NfcRelay* nfc_relay = nfc_relay_alloc();

    scene_manager_next_scene(nfc_relay->scene_manager, NfcRelaySceneStart);

    view_dispatcher_run(nfc_relay->view_dispatcher);

    nfc_relay_free(nfc_relay);

    return 0;
}
