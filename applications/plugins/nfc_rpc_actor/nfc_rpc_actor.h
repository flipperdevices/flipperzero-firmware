#pragma once

#include "furi.h"
#include "furi_hal.h"
#include "rpc/rpc_app.h"
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <notification/notification_messages.h>
#include <gui/modules/widget.h>
#include <gui/modules/text_box.h>
#include "assets/compiled/nfc.pb.h"
#include "lib/nanopb/pb_decode.h"
#include "lib/nanopb/pb_encode.h"

typedef enum {
    NfcRpcActorEventDataExchange,
} NfcRpcActorEventType;

typedef struct {
    NfcRpcActorEventType type;
    const uint8_t* data;
} NfcRpcActorEvent;

typedef struct {
    bool ok;
    uint8_t uid[10];
    uint8_t sak[1];
    uint8_t atqa[2];
} NfcRpcActorAnticollisionResponse;

typedef struct {
    Gui* gui;
    RpcAppSystem* rpc;
    NotificationApp* notifications;
    ViewPort* view_port;
    FuriMessageQueue* queue;
    ViewDispatcher* view_dispatcher;
    View* view;

    TextBox* text_box;
} NfcRpcActor;
