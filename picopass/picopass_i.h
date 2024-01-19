#pragma once

#include "picopass.h"
#include "picopass_device.h"

#include "rfal_picopass.h"

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
#include <gui/modules/text_box.h>
#include <gui/modules/widget.h>

#include <input/input.h>

#include "scenes/picopass_scene.h"
#include "views/dict_attack.h"
#include "views/loclass.h"

#include <storage/storage.h>
#include <lib/toolbox/path.h>
#include <picopass_icons.h>

#include <assets_icons.h>

#include <nfc/nfc.h>
#include <toolbox/keys_dict.h>
#include "protocol/picopass_poller.h"
#include "protocol/picopass_listener.h"

#define PICOPASS_TEXT_STORE_SIZE 129

#define PICOPASS_ICLASS_ELITE_DICT_FLIPPER_NAME APP_ASSETS_PATH("iclass_elite_dict.txt")
#define PICOPASS_ICLASS_STANDARD_DICT_FLIPPER_NAME APP_ASSETS_PATH("iclass_standard_dict.txt")
#define PICOPASS_ICLASS_ELITE_DICT_USER_NAME APP_DATA_PATH("assets/iclass_elite_dict_user.txt")

enum PicopassCustomEvent {
    // Reserve first 100 events for button types and indexes, starting from 0
    PicopassCustomEventReserved = 100,

    PicopassCustomEventViewExit,
    PicopassCustomEventWorkerExit,
    PicopassCustomEventByteInputDone,
    PicopassCustomEventTextInputDone,
    PicopassCustomEventDictAttackSkip,
    PicopassCustomEventDictAttackUpdateView,
    PicopassCustomEventLoclassGotMac,
    PicopassCustomEventLoclassGotStandardKey,
    PicopassCustomEventNrMacSaved,

    PicopassCustomEventPollerSuccess,
    PicopassCustomEventPollerFail,
};

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    const char* name;
    uint16_t total_keys;
    uint16_t current_key;
    bool card_detected;
} PicopassDictAttackContext;

typedef struct {
    uint8_t key_to_write[PICOPASS_BLOCK_LEN];
    bool is_elite;
} PicopassWriteKeyContext;

typedef struct {
    size_t macs_collected;
} PicopassLoclassContext;

struct Picopass {
    ViewDispatcher* view_dispatcher;
    Gui* gui;
    NotificationApp* notifications;
    SceneManager* scene_manager;
    PicopassDevice* dev;

    Nfc* nfc;
    PicopassPoller* poller;
    PicopassListener* listener;
    KeysDict* dict;
    uint32_t last_error_notify_ticks;

    char text_store[PICOPASS_TEXT_STORE_SIZE];
    FuriString* text_box_store;
    uint8_t byte_input_store[PICOPASS_BLOCK_LEN];

    // Common Views
    Submenu* submenu;
    Popup* popup;
    Loading* loading;
    TextInput* text_input;
    ByteInput* byte_input;
    TextBox* text_box;
    Widget* widget;
    DictAttack* dict_attack;
    Loclass* loclass;

    PicopassDictAttackContext dict_attack_ctx;
    PicopassWriteKeyContext write_key_context;
    PicopassLoclassContext loclass_context;
};

typedef enum {
    PicopassViewMenu,
    PicopassViewPopup,
    PicopassViewLoading,
    PicopassViewTextInput,
    PicopassViewByteInput,
    PicopassViewTextBox,
    PicopassViewWidget,
    PicopassViewDictAttack,
    PicopassViewLoclass,
} PicopassView;

Picopass* picopass_alloc();

void picopass_text_store_set(Picopass* picopass, const char* text, ...);

void picopass_text_store_clear(Picopass* picopass);

void picopass_blink_start(Picopass* picopass);

void picopass_blink_emulate_start(Picopass* picopass);

void picopass_blink_stop(Picopass* picopass);

void picopass_show_loading_popup(void* context, bool show);

/** Check if memory is set to pattern
 *
 * @warning    zero size will return false
 *
 * @param[in]  data     Pointer to the byte array
 * @param[in]  pattern  The pattern
 * @param[in]  size     The byte array size
 *
 * @return     True if memory is set to pattern, false otherwise
 */
bool picopass_is_memset(const uint8_t* data, const uint8_t pattern, size_t size);
