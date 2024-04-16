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
#include <views/dict_attack.h>
#include <views/write_problems.h>

#include <input/input.h>

#include "scenes/nfc_magic_scene.h"

#include <storage/storage.h>
#include <dialogs/dialogs.h>
#include <lib/toolbox/path.h>

#include "nfc_magic_icons.h"

#include <nfc/nfc.h>
#include <nfc/nfc_device.h>
#include <nfc/nfc_poller.h>
#include <toolbox/keys_dict.h>

#include "magic/nfc_magic_scanner.h"
#include "magic/protocols/nfc_magic_protocols.h"
#include "magic/protocols/gen1a/gen1a_poller.h"
#include "magic/protocols/gen2/gen2_poller.h"
#include "magic/protocols/gen4/gen4_poller.h"

#include "lib/nfc/protocols/mf_classic/mf_classic_poller.h"

#define NFC_APP_FOLDER ANY_PATH("nfc")
#define NFC_APP_EXTENSION ".nfc"
#define NFC_APP_SHADOW_EXTENSION ".shd"

#define NFC_APP_MF_CLASSIC_DICT_USER_PATH (NFC_APP_FOLDER "/assets/mf_classic_dict_user.nfc")
#define NFC_APP_MF_CLASSIC_DICT_SYSTEM_PATH (NFC_APP_FOLDER "/assets/mf_classic_dict.nfc")

#define NFC_MAGIC_APP_BYTE_INPUT_STORE_SIZE (4)

enum NfcMagicAppCustomEvent {
    // Reserve first 100 events for button types and indexes, starting from 0
    NfcMagicAppCustomEventReserved = 100,

    NfcMagicAppCustomEventViewExit,
    NfcMagicAppCustomEventWorkerExit,
    NfcMagicAppCustomEventByteInputDone,
    NfcMagicAppCustomEventTextInputDone,
    NfcMagicAppCustomEventCardDetected,
    NfcMagicAppCustomEventCardLost,
    NfcMagicAppCustomEventDictAttackDataUpdate,
    NfcMagicAppCustomEventDictAttackComplete,
    NfcMagicAppCustomEventDictAttackSkip,
};

typedef struct {
    KeysDict* dict;
    uint8_t sectors_total;
    uint8_t sectors_read;
    uint8_t current_sector;
    uint8_t keys_found;
    size_t dict_keys_total;
    size_t dict_keys_current;
    bool is_key_attack;
    uint8_t key_attack_current_sector;
    bool is_card_present;
} NfcMagicAppMfClassicDictAttackContext;

typedef struct {
    uint8_t problem_index;
    uint8_t problem_index_abs;
    uint8_t problems_total;
    Gen2PollerWriteProblems problems;
} NfcMagicAppWriteProblemsContext;

struct NfcMagicApp {
    ViewDispatcher* view_dispatcher;
    Gui* gui;
    NotificationApp* notifications;
    DialogsApp* dialogs;
    Storage* storage;

    SceneManager* scene_manager;
    NfcDevice* source_dev;
    NfcDevice* target_dev;
    FuriString* file_name;
    FuriString* file_path;

    Nfc* nfc;
    NfcMagicProtocol protocol;
    NfcMagicScanner* scanner;
    NfcPoller* poller;
    Gen1aPoller* gen1a_poller;

    Gen2Poller* gen2_poller;
    bool gen2_poller_is_wipe_mode;

    Gen4Poller* gen4_poller;

    Gen4* gen4_data;

    Gen4Password gen4_password;
    Gen4Password gen4_password_new;

    NfcMagicAppMfClassicDictAttackContext nfc_dict_context;
    DictAttack* dict_attack;
    NfcMagicAppWriteProblemsContext write_problems_context;
    WriteProblems* write_problems;

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
    NfcMagicAppViewDictAttack,
    NfcMagicAppViewWriteProblems,
} NfcMagicAppView;

void nfc_magic_app_blink_start(NfcMagicApp* nfc_magic);

void nfc_magic_app_blink_stop(NfcMagicApp* nfc_magic);

void nfc_magic_app_show_loading_popup(void* context, bool show);

bool nfc_magic_load_from_file_select(NfcMagicApp* instance);
