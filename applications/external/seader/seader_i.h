#pragma once

#include <stdlib.h> // malloc
#include <stdint.h> // uint32_t
#include <stdarg.h> // __VA_ARGS__
#include <string.h>
#include <stdio.h>

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <notification/notification_messages.h>
#include "seader_icons.h"

#include <gui/modules/submenu.h>
#include <gui/modules/popup.h>
#include <gui/modules/loading.h>
#include <gui/modules/text_input.h>
#include <gui/modules/widget.h>

#include <input/input.h>

// ASN1
#include <asn_system.h>
#include <asn_internal.h>
#include <asn_codecs_prim.h>
#include <asn_codecs.h>
#include <asn_application.h>

#include <Payload.h>
#include <FrameProtocol.h>

#include "scenes/seader_scene.h"
#include "views/seader_uart_view.h"

#include "seader_bridge.h"
#include "seader.h"
#include "ccid.h"
#include "uart.h"
#include "rfal_picopass.h"
#include "seader_worker.h"
#include "seader_credential.h"

#define WORKER_ALL_RX_EVENTS                                                      \
    (WorkerEvtStop | WorkerEvtRxDone | WorkerEvtCfgChange | WorkerEvtLineCfgSet | \
     WorkerEvtCtrlLineSet)
#define WORKER_ALL_TX_EVENTS (WorkerEvtTxStop | WorkerEvtSamRx)

#define SEADER_TEXT_STORE_SIZE 128

enum SeaderCustomEvent {
    // Reserve first 100 events for button types and indexes, starting from 0
    SeaderCustomEventReserved = 100,

    SeaderCustomEventViewExit,
    SeaderCustomEventWorkerExit,
    SeaderCustomEventByteInputDone,
    SeaderCustomEventTextInputDone,
};

typedef enum {
    WorkerEvtStop = (1 << 0),
    WorkerEvtRxDone = (1 << 1),

    WorkerEvtTxStop = (1 << 2),
    WorkerEvtSamRx = (1 << 3),

    WorkerEvtCfgChange = (1 << 4),

    WorkerEvtLineCfgSet = (1 << 5),
    WorkerEvtCtrlLineSet = (1 << 6),

} WorkerEvtFlags;

struct Seader {
    bool revert_power;
    bool is_debug_enabled;
    SeaderWorker* worker;
    ViewDispatcher* view_dispatcher;
    Gui* gui;
    NotificationApp* notifications;
    SceneManager* scene_manager;
    SeaderUartBridge* uart;
    SeaderCredential* credential;

    char text_store[SEADER_TEXT_STORE_SIZE + 1];
    FuriString* text_box_store;

    // Common Views
    Submenu* submenu;
    Popup* popup;
    Loading* loading;
    TextInput* text_input;
    Widget* widget;

    //Custom views
    SeaderUartView* seader_uart_view;
};

typedef enum {
    SeaderViewMenu,
    SeaderViewPopup,
    SeaderViewLoading,
    SeaderViewTextInput,
    SeaderViewWidget,
    SeaderViewUart,
} SeaderView;

void seader_text_store_set(Seader* seader, const char* text, ...);

void seader_text_store_clear(Seader* seader);

void seader_blink_start(Seader* seader);

void seader_blink_stop(Seader* seader);

void seader_show_loading_popup(void* context, bool show);
