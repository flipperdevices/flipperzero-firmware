#pragma once

#include <gui/gui.h>
#include "nfc_cli_icons.h"
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/widget.h>
#include "cli/nfc_cli.h"

typedef enum {
    NfcCliWidgetConfigCliSessionWait,
    NfcCliWidgetConfigCliSessionRunning,
    NfcCliWidgetConfigAbortConfirm,

    NfcCliWidgetConfigNum,
} NfcCliWidgetConfig;

typedef struct {
    Cli* cli;
    Gui* gui;
    NfcCli* nfc_cli;
    ViewDispatcher* view_dispatcher;
    Widget* widget;
    NfcCliWidgetConfig widget_config;
    bool is_cli_connected;
} NfcCliApp;

typedef enum {
    NfcCliAppViewWidget,
} NfcCliAppView;

void nfc_cli_config_widget(NfcCliApp* instance, NfcCliWidgetConfig config);
