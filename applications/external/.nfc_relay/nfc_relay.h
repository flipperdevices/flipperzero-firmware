#pragma once
#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <notification/notification_messages.h>

#include <gui/modules/submenu.h>
#include <gui/modules/popup.h>
#include <gui/modules/loading.h>
#include <gui/modules/text_input.h>
#include <gui/modules/widget.h>
#include "scenes/nfc_relay_scene.h"
#include "nfc_relay_icons.h"

#include <furi_hal.h>

#include <lib/nfc/nfc_device.h>

typedef enum {
    NfcRelayModeUart,
} NfcRelayMode;

typedef struct NfcRelayConfigUart {
    FuriHalUartId uartId;
    uint32_t baudrate;
} NfcRelayConfigUart;

typedef struct NfcRelayConfig {
    NfcRelayMode mode;
    NfcRelayConfigUart uart_config;
} NfcRelayConfig;

typedef struct AsReaderWorker AsReaderWorker;
typedef struct AsCardWorker AsCardWorker;
typedef struct NfcRelay {
    ViewDispatcher* view_dispatcher;
    Gui* gui;
    NotificationApp* notifications;
    SceneManager* scene_manager;
    // Common Views
    Submenu* submenu;
    Popup* popup;
    Loading* loading;
    TextInput* text_input;
    Widget* widget;

    NfcRelayConfig* config;
    AsReaderWorker* as_reader_worker;
    AsCardWorker* as_card_worker;
} NfcRelay;

typedef enum {
    NfcRelayViewMenu,
    NfcRelayViewPopup,
    NfcRelayViewLoading,
    NfcRelayViewTextInput,
    NfcRelayViewWidget,
} NfcRelayView;