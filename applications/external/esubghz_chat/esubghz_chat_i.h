#pragma once

#include <furi.h>
#include <gui/view_dispatcher_i.h>
#include <gui/view_port_i.h>
#include <gui/scene_manager.h>
#include <gui/modules/byte_input.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/menu.h>
#include <gui/modules/popup.h>
#include <gui/modules/text_box.h>
#include <gui/modules/text_input.h>
#include <notification/notification_messages.h>
#include <lib/nfc/nfc_worker.h>
#include <lib/subghz/subghz_tx_rx_worker.h>
#include <toolbox/sha256.h>

#include "crypto_wrapper.h"
#include "scenes/esubghz_chat_scene.h"

#include "esubghz_chat_icons.h"

#define APPLICATION_NAME "ESubGhzChat"

#define DEFAULT_FREQ 433920000

#define KEY_READ_POPUP_MS 3000

#define RX_TX_BUFFER_SIZE 1024

#define CHAT_BOX_STORE_SIZE 4096
#define TEXT_INPUT_STORE_SIZE 256
#define MSG_PREVIEW_SIZE 32

#define KEY_HEX_STR_SIZE ((KEY_BITS / 8) * 3)

typedef struct {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    NotificationApp* notification;

    // UI elements
    Menu* menu;
    TextBox* chat_box;
    FuriString* chat_box_store;
    TextInput* text_input;
    char text_input_store[TEXT_INPUT_STORE_SIZE + 1];
    ByteInput* hex_key_input;
    uint8_t hex_key_input_store[KEY_BITS / 8];
    DialogEx* key_display;
    char key_hex_str[KEY_HEX_STR_SIZE + 1];
    Popup* nfc_popup;

    // for Sub-GHz
    uint32_t frequency;
    SubGhzTxRxWorker* subghz_worker;
    const SubGhzDevice* subghz_device;

    // for NFC
    NfcWorker* nfc_worker;
    NfcDeviceData* nfc_dev_data;

    // message assembly before TX
    FuriString* name_prefix;
    FuriString* msg_input;

    // message preview
    char msg_preview[MSG_PREVIEW_SIZE + 1];

    // encryption
    bool encrypted;
    ESubGhzChatCryptoCtx* crypto_ctx;

    // RX and TX buffers
    uint8_t rx_buffer[RX_TX_BUFFER_SIZE];
    uint8_t tx_buffer[RX_TX_BUFFER_SIZE];
    char rx_str_buffer[RX_TX_BUFFER_SIZE + 1];
    volatile uint32_t last_time_rx_data;

    // for locking
    ViewPortDrawCallback orig_draw_cb;
    ViewPortInputCallback orig_input_cb;
    bool kbd_locked;
    uint32_t kbd_lock_msg_ticks;
    uint8_t kbd_lock_count;

    // for ongoing inputs
    bool kbd_ok_input_ongoing;
    bool kbd_left_input_ongoing;
    bool kbd_right_input_ongoing;
} ESubGhzChatState;

typedef enum {
    ESubGhzChatEvent_FreqEntered,
    ESubGhzChatEvent_KeyMenuNoEncryption,
    ESubGhzChatEvent_KeyMenuPassword,
    ESubGhzChatEvent_KeyMenuHexKey,
    ESubGhzChatEvent_KeyMenuGenKey,
    ESubGhzChatEvent_KeyMenuReadKeyFromNfc,
    ESubGhzChatEvent_KeyReadPopupFailed,
    ESubGhzChatEvent_KeyReadPopupSucceeded,
    ESubGhzChatEvent_PassEntered,
    ESubGhzChatEvent_HexKeyEntered,
    ESubGhzChatEvent_MsgEntered,
    ESubGhzChatEvent_GotoMsgInput,
    ESubGhzChatEvent_GotoKeyDisplay,
    ESubGhzChatEvent_KeyDisplayBack,
    ESubGhzChatEvent_KeyDisplayShare,
} ESubGhzChatEvent;

typedef enum {
    ESubGhzChatView_Menu,
    ESubGhzChatView_Input,
    ESubGhzChatView_HexKeyInput,
    ESubGhzChatView_ChatBox,
    ESubGhzChatView_KeyDisplay,
    ESubGhzChatView_NfcPopup,
} ESubGhzChatView;

void set_chat_input_header(ESubGhzChatState* state);
void append_msg(ESubGhzChatState* state, const char* msg);
void tx_msg_input(ESubGhzChatState* state);
void enter_chat(ESubGhzChatState* state);
