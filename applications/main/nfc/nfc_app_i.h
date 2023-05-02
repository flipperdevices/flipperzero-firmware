#pragma once

#include "nfc_app.h"

#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <gui/view.h>
#include <assets_icons.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <cli/cli.h>
#include <notification/notification_messages.h>

#include <gui/modules/submenu.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/popup.h>
#include <gui/modules/loading.h>
#include <gui/modules/text_input.h>
#include <gui/modules/byte_input.h>
#include <gui/modules/text_box.h>
#include <gui/modules/widget.h>

#include <nfc/scenes/nfc_scene.h>
#include <nfc/helpers/nfc_custom_event.h>
#include "helpers/mf_ultralight_auth.h"

#include <dialogs/dialogs.h>
#include <storage/storage.h>
#include <toolbox/path.h>

#include "rpc/rpc_app.h"

#include <m-array.h>

#include <lib/nfc/nfc.h>
#include <lib/nfc/nfc_poller.h>
#include <lib/nfc/protocols/nfca/nfca_poller.h>
#include <lib/nfc/protocols/nfca/nfca_listener.h>
#include <lib/nfc/protocols/mf_ultralight/mf_ultralight_poller.h>
#include <lib/nfc/protocols/mf_ultralight/mf_ultralight_listener.h>
#include <lib/nfc/protocols/nfcb/nfcb_poller.h>

#include <lib/nfc/nfc_dev.h>
#include <lib/nfc/helpers/nfc_data_generator.h>

#include <gui/modules/validators.h>
#include <toolbox/path.h>
#include <dolphin/dolphin.h>

#define NFC_NAME_SIZE 22
#define NFC_TEXT_STORE_SIZE 128
#define NFC_APP_FOLDER ANY_PATH("nfc")
#define NFC_APP_EXTENSION ".nfc"

typedef enum {
    NfcRpcStateIdle,
    NfcRpcStateEmulating,
    NfcRpcStateEmulated,
} NfcRpcState;

struct NfcApp {
    DialogsApp* dialogs;
    Storage* storage;
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    NotificationApp* notifications;
    SceneManager* scene_manager;

    char text_store[NFC_TEXT_STORE_SIZE + 1];
    FuriString* text_box_store;
    uint8_t byte_input_store[6];

    void* rpc_ctx;
    NfcRpcState rpc_state;

    // Common Views
    Submenu* submenu;
    DialogEx* dialog_ex;
    Popup* popup;
    Loading* loading;
    TextInput* text_input;
    ByteInput* byte_input;
    TextBox* text_box;
    Widget* widget;

    Nfc* nfc;
    NfcaPoller* nfca_poller;
    NfcaListener* nfca_listener;
    MfUltralightPoller* mf_ul_poller;
    MfUltralightListener* mf_ul_listener;
    NfcbPoller* nfcb_poller;
    NfcPoller* nfc_poller;

    MfUltralightAuth* mf_ul_auth;

    NfcDev* nfc_dev;
    NfcDevData nfc_dev_data;
    NfcaData nfca_edit_data;
    FuriString* file_path;
    FuriString* file_name;
};

typedef enum {
    NfcViewMenu,
    NfcViewDialogEx,
    NfcViewPopup,
    NfcViewLoading,
    NfcViewTextInput,
    NfcViewByteInput,
    NfcViewTextBox,
    NfcViewWidget,
    NfcViewDictAttack,
    NfcViewDetectReader,
} NfcView;

int32_t nfc_task(void* p);

void nfc_text_store_set(NfcApp* nfc, const char* text, ...);

void nfc_text_store_clear(NfcApp* nfc);

void nfc_blink_read_start(NfcApp* nfc);

void nfc_blink_emulate_start(NfcApp* nfc);

void nfc_blink_detect_start(NfcApp* nfc);

void nfc_blink_stop(NfcApp* nfc);

void nfc_show_loading_popup(void* context, bool show);

bool nfc_save(NfcApp* instance);

bool nfc_delete(NfcApp* instance);

bool nfc_load_from_file_select(NfcApp* instance);

bool nfc_load_file(NfcApp* instance, FuriString* path, bool show_dialog);

bool nfc_save_file(NfcApp* instance, FuriString* path);

void nfc_make_app_folder(NfcApp* instance);
