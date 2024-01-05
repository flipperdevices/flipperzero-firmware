#pragma once

#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <gui/view.h>
#include "assets/assets_icons.h"
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <cli/cli.h>
#include <notification/notification_messages.h>

#include <gui/modules/submenu.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/popup.h>
#include <gui/modules/text_input.h>
#include <gui/modules/byte_input.h>
#include <gui/modules/widget.h>

#include "views/t5577_multiwriter_view_read.h"

#include <notification/notification_messages.h>
#include <dialogs/dialogs.h>
#include <storage/storage.h>
#include <flipper_format/flipper_format.h>

#include <rpc/rpc_app.h>

#include <toolbox/protocols/protocol_dict.h>
#include <toolbox/path.h>
#include <lfrfid/lfrfid_dict_file.h>
#include <lfrfid/protocols/lfrfid_protocols.h>
#include <lfrfid/lfrfid_worker.h>

#include "scenes/t5577_multiwriter_scene.h"

#define LFRFID_KEY_NAME_SIZE 22
#define LFRFID_TEXT_STORE_SIZE 40

#define LFRFID_APP_FOLDER ANY_PATH("lfrfid")
#define LFRFID_SD_FOLDER EXT_PATH("lfrfid")
#define LFRFID_APP_FILENAME_PREFIX "RFID"
#define LFRFID_APP_FILENAME_EXTENSION ".rfid"
#define LFRFID_APP_SHADOW_FILENAME_EXTENSION ".shd"

enum LfRfidCustomEvent {
    LfRfidEventNext = 100,
    LfRfidEventExit,
    LfRfidEventPopupClosed,
    LfRfidEventWriteOK,
    LfRfidEventWriteProtocolCannotBeWritten,
    LfRfidEventWriteFobCannotBeWritten,
    LfRfidEventWriteTooLongToWrite,
};

typedef enum {
    LfRfidRpcStateIdle,
    LfRfidRpcStateEmulating,
} LfRfidRpcState;

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

    ProtocolDict* dict;
    ProtocolId protocol_id;
    ProtocolId protocol_id_next;
    LFRFIDWorkerReadType read_type;

    uint8_t* old_key_data;
    uint8_t* new_key_data;

    // Common Views
    Submenu* submenu;
    DialogEx* dialog_ex;
    Popup* popup;
    TextInput* text_input;
    ByteInput* byte_input;

    // Custom views
    LfRfidReadView* read_view;
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

bool t5577_multiwriter_load_key_from_file_select(LfRfid* app);

bool t5577_multiwriter_load_key_data(LfRfid* app, FuriString* path, bool show_dialog);

void t5577_multiwriter_make_app_folder(LfRfid* app);

void t5577_multiwriter_text_store_set(LfRfid* app, const char* text, ...);

void t5577_multiwriter_text_store_clear(LfRfid* app);

void t5577_multiwriter_popup_timeout_callback(void* context);

void t5577_multiwriter_widget_callback(GuiButtonType result, InputType type, void* context);

void t5577_multiwriter_text_input_callback(void* context);
