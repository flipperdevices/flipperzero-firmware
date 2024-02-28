#include "nfc_cli_app_i.h"

#include <furi/furi.h>

typedef void (*NfcCliAppWidgetSetup)(NfcCliApp* instance);

static void nfc_cli_app_widget_setup_cli_session_wait(NfcCliApp* instance) {
    widget_add_string_multiline_element(
        instance->widget,
        64,
        0,
        AlignCenter,
        AlignTop,
        FontSecondary,
        "Connect to USB to use\nCLI commands");
    widget_add_icon_element(instance->widget, 20, 30, &I_PC_connect_84x21);
}

static void nfc_cli_app_widget_setup_cli_session_running(NfcCliApp* instance) {
    widget_add_string_element(
        instance->widget, 96, 0, AlignCenter, AlignTop, FontPrimary, "Connected!");
    widget_add_icon_element(instance->widget, 0, 0, &I_Connected_cyberhelmet_64x64);
    widget_add_text_box_element(
        instance->widget,
        64,
        18,
        64,
        32,
        AlignLeft,
        AlignTop,
        "Additional NFC CLI commands are available",
        false);
    widget_add_string_element(
        instance->widget, 64, 64, AlignLeft, AlignBottom, FontSecondary, "Exit = ");
    widget_add_icon_element(instance->widget, 95, 57, &I_back_btn_10x8);
}

static void nfc_cli_app_widget_setup_abort_confirm_callback(
    GuiButtonType result,
    InputType type,
    void* context) {
    NfcCliApp* instance = context;

    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(instance->view_dispatcher, result);
    }
}

static void nfc_cli_app_widget_setup_abort_confirm(NfcCliApp* instance) {
    widget_add_string_multiline_element(
        instance->widget, 64, 0, AlignCenter, AlignTop, FontPrimary, "Exit the NFC CLI?");
    widget_add_string_multiline_element(
        instance->widget,
        64,
        13,
        AlignCenter,
        AlignTop,
        FontSecondary,
        "Connection will be interrupted\nand unsaved data will be lost!");
    widget_add_button_element(
        instance->widget,
        GuiButtonTypeLeft,
        "Exit",
        nfc_cli_app_widget_setup_abort_confirm_callback,
        instance);
    widget_add_button_element(
        instance->widget,
        GuiButtonTypeRight,
        "Stay",
        nfc_cli_app_widget_setup_abort_confirm_callback,
        instance);
}

static const NfcCliAppWidgetSetup nfc_cli_app_widget_setup[NfcCliWidgetConfigNum] = {
    [NfcCliWidgetConfigCliSessionWait] = nfc_cli_app_widget_setup_cli_session_wait,
    [NfcCliWidgetConfigCliSessionRunning] = nfc_cli_app_widget_setup_cli_session_running,
    [NfcCliWidgetConfigAbortConfirm] = nfc_cli_app_widget_setup_abort_confirm,
};

void nfc_cli_config_widget(NfcCliApp* instance, NfcCliWidgetConfig config) {
    furi_assert(instance);
    furi_assert(config < NfcCliWidgetConfigNum);

    widget_reset(instance->widget);
    nfc_cli_app_widget_setup[config](instance);
    instance->widget_config = config;
}
