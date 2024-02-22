#pragma once

#include "nfc_magic_app.h"
#include "helpers/nfc_magic_custom_events.h"

#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <notification/notification_messages.h>

#include <gui/modules/submenu.h>
#include <gui/modules/popup.h>
#include <gui/modules/loading.h>
#include <gui/modules/text_input.h>
#include <gui/modules/byte_input.h>
#include <gui/modules/widget.h>

#include <input/input.h>

#include "scenes/nfc_magic_scene.h"

#include <storage/storage.h>
#include <dialogs/dialogs.h>
#include <lib/toolbox/path.h>

#include "nfc_magic_icons.h"

#include <assets_icons.h>

#include <nfc/nfc.h>
#include <nfc/nfc_device.h>

#include "lib/magic/nfc_magic_scanner.h"
#include "lib/magic/protocols/nfc_magic_protocols.h"
#include "lib/magic/protocols/gen1a/gen1a_poller.h"
#include "lib/magic/protocols/gen4/gen4_poller.h"

#define NFC_APP_FOLDER ANY_PATH("nfc")
#define NFC_APP_EXTENSION ".nfc"
#define NFC_APP_SHADOW_EXTENSION ".shd"

#define NFC_MAGIC_APP_BYTE_INPUT_STORE_SIZE (4)

enum NfcMagicAppCustomEvent {
    // Reserve first 100 events for button types and indexes, starting from 0
    NfcMagicAppCustomEventReserved = 100,

    NfcMagicAppCustomEventViewExit,
    NfcMagicAppCustomEventWorkerExit,
    NfcMagicAppCustomEventByteInputDone,
    NfcMagicAppCustomEventTextInputDone,
};

struct NfcMagicApp {
    ViewDispatcher* view_dispatcher;
    Gui* gui;
    NotificationApp* notifications;
    DialogsApp* dialogs;
    Storage* storage;

    SceneManager* scene_manager;
    NfcDevice* source_dev;
    FuriString* file_name;
    FuriString* file_path;

    Nfc* nfc;
    NfcMagicProtocol protocol;
    NfcMagicScanner* scanner;
    Gen1aPoller* gen1a_poller;
    Gen4Poller* gen4_poller;

    uint32_t gen4_password;
    uint32_t gen4_password_new;

    uint8_t gen4_config_display[32];
    uint8_t gen4_revision_display[5];

    FuriString* text_box_store;
    uint8_t byte_input_store[NFC_MAGIC_APP_BYTE_INPUT_STORE_SIZE];

    // Common Views
    Submenu* submenu;
    Popup* popup;
    Loading* loading;
    TextInput* text_input;
    ByteInput* byte_input;
    Widget* widget;
};

typedef enum {
    NfcMagicAppViewMenu,
    NfcMagicAppViewPopup,
    NfcMagicAppViewLoading,
    NfcMagicAppViewTextInput,
    NfcMagicAppViewByteInput,
    NfcMagicAppViewWidget,
} NfcMagicAppView;

void nfc_magic_app_blink_start(NfcMagicApp* nfc_magic);

void nfc_magic_app_blink_stop(NfcMagicApp* nfc_magic);

void nfc_magic_app_show_loading_popup(void* context, bool show);

bool nfc_magic_load_from_file_select(NfcMagicApp* instance);
