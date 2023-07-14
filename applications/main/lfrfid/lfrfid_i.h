#pragma once

#include <dialogs/dialogs.h>
#include <flipper_format/flipper_format.h>
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/popup.h>
#include <gui/modules/text_input.h>
#include <gui/modules/byte_input.h>
#include <gui/modules/widget.h>
#include <notification/notification_messages.h>
#include <rpc/rpc_app.h>
#include <storage/storage.h>

#include <toolbox/protocols/protocol_dict.h>
#include <toolbox/path.h>
#include <lfrfid/lfrfid_dict_file.h>
#include <lfrfid/protocols/lfrfid_protocols.h>
#include <lfrfid/lfrfid_worker.h>

#include "views/lfrfid_view_read.h"
#include "scenes/lfrfid_scene.h"

#include "lfrfid_icons.h"

#define LFRFID_KEY_NAME_SIZE 22
#define LFRFID_TEXT_STORE_SIZE 40

#define LFRFID_APP_FOLDER ANY_PATH("lfrfid")
#define LFRFID_SD_FOLDER EXT_PATH("lfrfid")
#define LFRFID_APP_EXTENSION ".rfid"
#define LFRFID_APP_SHADOW_EXTENSION ".shd"

#define LFRFID_APP_RAW_ASK_EXTENSION ".ask.raw"
#define LFRFID_APP_RAW_PSK_EXTENSION ".psk.raw"

#define SCREEN_WIDTH (128)
#define SCREEN_HEIGHT (64)
#define SCREEN_WIDTH_CENTER (SCREEN_WIDTH >> 1)
#define SCREEN_HEIGHT_CENTER (SCREEN_HEIGHT >> 1)

enum LfRfidCustomEvent {
    LfRfidEventNext = 100,
    LfRfidEventExit,
    LfRfidEventPopupClosed,
    LfRfidEventReadSenseStart,
    LfRfidEventReadSenseEnd,
    LfRfidEventReadSenseCardStart,
    LfRfidEventReadSenseCardEnd,
    LfRfidEventReadStartASK,
    LfRfidEventReadStartPSK,
    // LfRfidEventReadStartRTF,
    // LfRfidEventReadSenseHitag, //TODO combine with sensecardstart?
    LfRfidEventReadDone,
    LfRfidEventReadOverrun,
    LfRfidEventReadError,
    LfRfidEventWriteOK,
    LfRfidEventWriteProtocolCannotBeWritten,
    LfRfidEventWriteFobCannotBeWritten,
    LfRfidEventWriteTooLongToWrite,
    LfRfidEventRpcLoadFile,
    LfRfidEventRpcSessionClose,
};

typedef enum {
    LfRfidRpcStateIdle,
    LfRfidRpcStateEmulating,
} LfRfidRpcState;

typedef enum {
    LfRfidUsePassword = 1 << 0,
    LfRfidSetConfigurationLockBit = 1 << 1,
    LfRfidSetPasswordLockBit = 1 << 2,
    LfRfidSetMasterKeyDisableTestMode = 1 << 3,
    LfRfidDisablePasswordMode = 1 << 4,
    LfRfidWriteBlockMode = 1 << 5,
    LfRfidWriteBlockLockBit = 1 << 6,
    LfRfidReadBlockMode = 1 << 7,
    LfRfidTestModeAccess = 1 << 8
} LfRfidExtraOptions;

typedef enum {
    LfRfidSettingHexGeneric = 1 << 0,
    LfRfidSettingCurrentPassword = 1 << 1,
    LfRfidSettingNewPassword = 1 << 2,
} LfRfidSettingHex;

typedef struct LfRfid LfRfid;

struct LfRfid {
    LFRFIDWorker* lfworker;
    ViewDispatcher* view_dispatcher;
    Gui* gui;
    NotificationApp* notifications;
    SceneManager* scene_manager;
    Storage* storage;
    DialogsApp* dialogs;
    Widget* widget;

    char text_store[LFRFID_TEXT_STORE_SIZE + 1];
    FuriString* file_path;
    FuriString* file_name;
    FuriString* raw_file_name;

    ProtocolDict* dict;
    ProtocolId protocol_id;
    ProtocolId protocol_id_next;
    LFRFIDWorkerReadType read_type;

    uint8_t* old_key_data;
    uint8_t* new_key_data;

    RpcAppSystem* rpc_ctx;
    LfRfidRpcState rpc_state;

    // Common Views
    Submenu* submenu;
    DialogEx* dialog_ex;
    Popup* popup;
    TextInput* text_input;
    ByteInput* byte_input;

    // Custom views
    LfRfidReadView* read_view;

    LfRfidExtraOptions extra_options;
    uint8_t* password;
    uint8_t* new_password;
    LfRfidSettingHex setting_hex;
    uint8_t write_page;
    uint8_t write_block;
    //uint8_t read_page;
    //uint8_t read_block;
};

typedef enum {
    LfRfidViewSubmenu,
    LfRfidViewDialogEx,
    LfRfidViewPopup,
    LfRfidViewWidget,
    LfRfidViewTextInput,
    LfRfidViewByteInput,
    LfRfidViewRead,
} LfRfidView;

bool lfrfid_save_key(LfRfid* app);

bool lfrfid_load_key_from_file_select(LfRfid* app);

bool lfrfid_load_raw_key_from_file_select(LfRfid* app);

bool lfrfid_delete_key(LfRfid* app);

bool lfrfid_load_key_data(LfRfid* app, FuriString* path, bool show_dialog);

bool lfrfid_save_key_data(LfRfid* app, FuriString* path);

void lfrfid_make_app_folder(LfRfid* app);

void lfrfid_text_store_set(LfRfid* app, const char* text, ...);

void lfrfid_text_store_clear(LfRfid* app);

void lfrfid_popup_timeout_callback(void* context);

void lfrfid_widget_callback(GuiButtonType result, InputType type, void* context);

void lfrfid_text_input_callback(void* context);
