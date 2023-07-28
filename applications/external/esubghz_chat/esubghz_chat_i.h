#pragma once

#include <furi.h>
#include <gui/view_dispatcher_i.h>
#include <gui/view_port_i.h>
#include <gui/scene_manager.h>
#include <gui/modules/menu.h>
#include <gui/modules/text_box.h>
#include <gui/modules/text_input.h>
#include <notification/notification_messages.h>
#include <lib/subghz/subghz_tx_rx_worker.h>
#include <toolbox/sha256.h>

#include "crypto_wrapper.h"
#include "scenes/esubghz_chat_scene.h"

#define APPLICATION_NAME "ESubGhzChat"

#define DEFAULT_FREQ 433920000

#define RX_TX_BUFFER_SIZE 1024

#define CHAT_BOX_STORE_SIZE 4096
#define TEXT_INPUT_STORE_SIZE 256

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

    // for Sub-GHz
    uint32_t frequency;
    SubGhzTxRxWorker* subghz_worker;
    const SubGhzDevice* subghz_device;

    // message assembly before TX
    FuriString* name_prefix;
    FuriString* msg_input;

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
    bool kbd_ok_input_ongoing;
} ESubGhzChatState;

typedef enum {
    ESubGhzChatEvent_FreqEntered,
    ESubGhzChatEvent_KeyMenuNoEncryption,
    ESubGhzChatEvent_KeyMenuPassword,
    ESubGhzChatEvent_PassEntered,
    ESubGhzChatEvent_MsgEntered
} ESubGhzChatEvent;

typedef enum {
    ESubGhzChatView_Menu,
    ESubGhzChatView_Input,
    ESubGhzChatView_ChatBox,
} ESubGhzChatView;

void tx_msg_input(ESubGhzChatState* state);
void enter_chat(ESubGhzChatState* state);
