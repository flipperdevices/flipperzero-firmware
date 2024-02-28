#include "nfc_cli_app_i.h"

#include <furi.h>

#define NFC_CLI_APP_CLI_SESSION_UPDATE_TIME_MS (200)

static void nfc_cli_app_update_cli_status(NfcCliApp* instance) {
    if(instance->is_cli_connected != cli_is_connected(instance->cli)) {
        instance->is_cli_connected = cli_is_connected(instance->cli);
        NfcCliWidgetConfig new_config = instance->is_cli_connected ?
                                            NfcCliWidgetConfigCliSessionRunning :
                                            NfcCliWidgetConfigCliSessionWait;
        nfc_cli_config_widget(instance, new_config);
    }
}

static bool nfc_cli_app_view_dispatcher_navigation_callback(void* context) {
    bool consumed = false;
    NfcCliApp* instance = context;

    if(nfc_cli_is_command_in_progress(instance->nfc_cli)) {
        if(instance->widget_config != NfcCliWidgetConfigAbortConfirm) {
            nfc_cli_config_widget(instance, NfcCliWidgetConfigAbortConfirm);
        }
        consumed = true;
    }

    return consumed;
}

static bool nfc_cli_app_view_dispatcher_custom_event_callback(void* context, uint32_t event) {
    bool consumed = false;
    NfcCliApp* instance = context;

    // Process events from Abort Confirm view
    if(event == GuiButtonTypeLeft) {
        nfc_cli_abort_command(instance->nfc_cli);
        view_dispatcher_stop(instance->view_dispatcher);
        consumed = true;
    } else if(event == GuiButtonTypeRight) {
        NfcCliWidgetConfig new_config = instance->is_cli_connected ?
                                            NfcCliWidgetConfigCliSessionRunning :
                                            NfcCliWidgetConfigCliSessionWait;
        nfc_cli_config_widget(instance, new_config);
        consumed = true;
    }

    return consumed;
}

static void nfc_cli_app_view_dispatcher_tick_event_callback(void* context) {
    NfcCliApp* instance = context;

    if(instance->widget_config != NfcCliWidgetConfigAbortConfirm) {
        nfc_cli_app_update_cli_status(instance);
    }
}

static NfcCliApp* nfc_cli_app_alloc() {
    NfcCliApp* instance = malloc(sizeof(NfcCliApp));

    // Records
    instance->cli = furi_record_open(RECORD_CLI);
    instance->gui = furi_record_open(RECORD_GUI);

    // NFC CLI
    instance->nfc_cli = nfc_cli_alloc(instance->cli);

    // View dispatcher
    instance->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(instance->view_dispatcher);
    view_dispatcher_attach_to_gui(
        instance->view_dispatcher, instance->gui, ViewDispatcherTypeFullscreen);

    // Widget view
    instance->widget = widget_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher, NfcCliAppViewWidget, widget_get_view(instance->widget));
    view_dispatcher_set_event_callback_context(instance->view_dispatcher, instance);
    view_dispatcher_set_navigation_event_callback(
        instance->view_dispatcher, nfc_cli_app_view_dispatcher_navigation_callback);
    view_dispatcher_set_custom_event_callback(
        instance->view_dispatcher, nfc_cli_app_view_dispatcher_custom_event_callback);
    view_dispatcher_set_tick_event_callback(
        instance->view_dispatcher,
        nfc_cli_app_view_dispatcher_tick_event_callback,
        NFC_CLI_APP_CLI_SESSION_UPDATE_TIME_MS);

    instance->is_cli_connected = cli_is_connected(instance->cli);
    NfcCliWidgetConfig new_config = instance->is_cli_connected ?
                                        NfcCliWidgetConfigCliSessionRunning :
                                        NfcCliWidgetConfigCliSessionWait;
    nfc_cli_config_widget(instance, new_config);

    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcCliAppViewWidget);

    return instance;
}

static void nfc_cli_app_free(NfcCliApp* instance) {
    nfc_cli_free(instance->nfc_cli);

    view_dispatcher_remove_view(instance->view_dispatcher, NfcCliAppViewWidget);
    widget_free(instance->widget);

    view_dispatcher_free(instance->view_dispatcher);

    furi_record_close(RECORD_CLI);
    furi_record_close(RECORD_GUI);

    free(instance);
}

static void nfc_cli_app_run(NfcCliApp* instance) {
    view_dispatcher_run(instance->view_dispatcher);
}

int32_t nfc_cli_app(void* p) {
    UNUSED(p);

    NfcCliApp* instance = nfc_cli_app_alloc();
    nfc_cli_app_run(instance);
    nfc_cli_app_free(instance);

    return 0;
}
