#pragma once

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>
#include <notification/notification_messages.h>
#include <lib/subghz/subghz_setting.h>
#include <lib/subghz/subghz_worker.h>
#include <lib/subghz/receiver.h>
#include <lib/subghz/transmitter.h>
#include <lib/subghz/registry.h>

#define TAG "ProtoView"

typedef struct ProtoViewApp ProtoViewApp;

/* Subghz system state */
typedef enum {
    TxRxStateIDLE,
    TxRxStateRx,
    TxRxStateSleep,
} TxRxState;

/* This is the context of our subghz worker and associated thread.
 * It receives data and we get our protocol "feed" callback called
 * with the level (1 or 0) and duration. */
struct ProtoViewTxRx {
    SubGhzWorker* worker;
    SubGhzEnvironment* environment;
    SubGhzReceiver* receiver;
    SubGhzRadioPreset* preset;
    TxRxState txrx_state; /* Receiving, idle or sleeping? */
};

typedef struct ProtoViewTxRx ProtoViewTxRx;

struct ProtoViewApp {
    Gui *gui;
    ViewPort *view_port;
    FuriMessageQueue *event_queue;
    ProtoViewTxRx *txrx;     /* Radio state. */
    SubGhzSetting *setting;
    int running;             /* Once false exists the app. */
    uint32_t signal_bestlen; /* Longest coherent signal observed so far. */
    uint32_t us_scale;       /* microseconds per pixel. */
};

void radio_begin(ProtoViewApp* app);
uint32_t radio_rx(ProtoViewApp* app, uint32_t frequency);
void radio_idle(ProtoViewApp* app);
void radio_rx_end(ProtoViewApp* app);
void radio_sleep(ProtoViewApp* app);
