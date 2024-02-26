#include <furi.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/widget.h>
#include <cli/cli.h>

typedef struct {
    Cli* cli;
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    Widget* widget;
} NfcCliApp;

typedef enum {
    NfcCliAppViewWidget,
} NfcCliAppView;

uint32_t nfc_cli_app_exit(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

static NfcCliApp* nfc_cli_app_alloc() {
    NfcCliApp* instance = malloc(sizeof(NfcCliApp));

    // Records
    instance->cli = furi_record_open(RECORD_CLI);
    instance->gui = furi_record_open(RECORD_GUI);

    // View dispatcher
    instance->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(instance->view_dispatcher);
    view_dispatcher_attach_to_gui(
        instance->view_dispatcher, instance->gui, ViewDispatcherTypeFullscreen);

    // Widget view
    instance->widget = widget_alloc();
    widget_add_string_element(
        instance->widget, 64, 19, AlignCenter, AlignBottom, FontPrimary, "NFC CLI");
    view_dispatcher_add_view(
        instance->view_dispatcher, NfcCliAppViewWidget, widget_get_view(instance->widget));
    view_set_previous_callback(widget_get_view(instance->widget), nfc_cli_app_exit);

    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcCliAppViewWidget);

    return instance;
}

static void nfc_cli_app_free(NfcCliApp* instance) {
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