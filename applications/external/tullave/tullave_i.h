#pragma once

#include "tullave.h"

#include "lib/tullave/tullave_data.h"
#include "lib/tullave/protocols/tullave_poller.h"

#include <furi.h>

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/popup.h>
#include <gui/modules/widget.h>

#include <notification/notification_messages.h>

#include <nfc/nfc.h>
#include <nfc/nfc_device.h>

#include "scenes/tullave_scene.h"

// Requires to have fap_libs=["assets"] in the application.fam to read F0 asset icons.
#include <assets_icons.h>

typedef enum { TuLlaveViewMenu, TuLlaveViewWidget, TuLlaveViewPopup } TuLlaveView;

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
    TuLlavePoller* tullave_poller;
};

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