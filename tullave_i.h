#pragma once

#include "tullave.h"

#include "lib/tullave/protocols/tullave_data.h"

#include <furi.h>

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/popup.h>
#include <gui/modules/widget.h>

#include <notification/notification_messages.h>

#include <lib/nfc/nfc.h>
#include <lib/nfc/nfc_device.h>

#include <nfc/protocols/iso14443_4a/iso14443_4a_poller.h>

#include "scenes/tullave_scene.h"

// Requires to have fap_libs=["assets"] in the application.fam to read F0 asset icons.
#include <assets_icons.h>

typedef enum { TuLlaveViewMenu, TuLlaveViewWidget, TuLlaveViewPopup } TuLlaveView;

typedef enum { TuLlaveErrorNone = 0x100, TuLlaveErrorCardNotSupported } TuLlaveError;

// Main structure for the application
struct TuLlaveApp {
    NotificationApp* notifications;
    ViewDispatcher* view_dispatcher;
    Gui* gui;
    SceneManager* scene_manager;
    Widget* widget;
    Submenu* submenu;
    Popup* popup;
    Nfc* nfc;
    NfcDevice* nfc_device;
    NfcPoller* nfc_poller;
    TuLlaveData* card_data;
    TuLlaveError err_code;
};

// NfC Poller custom events
typedef enum {
    //Start enum in 100 to preserve first 100 events for GUI actions. Taken from Nfc Flipper App.
    NfcCustomEventReserved = 100,
    NfcPollerEventReadingData,
    NfcPollerEventSuccess,
    NfcPollerEventError
} NfcCustomEvent;

/**
 * Allocates and initializes TuLlaveApp main structure, required to initialize the application
*/
TuLlaveApp* tullave_alloc();

/**
 * Frees the space utilized by TuLlaveApp main structure
*/
void tullave_free(TuLlaveApp* t_llave);

/**
 * Application entrypoint
*/
int32_t tullave_main(void* p);