#pragma once

#include <stdio.h>
#include <furi.h>
#include <furi_hal_bt.h>
#include <furi_hal_usb_hid.h>
#include <furi_hal_bt_hid.h>
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification_messages.h>
#include <bt/bt_service/bt.h>
#include <storage/storage.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/popup.h>
#include "bt_trigger_icons.h"

#define HID_BT_KEYS_STORAGE_PATH EXT_PATH("apps_data/hid_ble/.bt_hid.keys")
#define TAG "bt_trigger"

// Enum of allowed event types
typedef enum {
    EventTypeTick,
    EventTypeInput,

} EventType;

// Struct to store an event and its type
typedef struct {
    EventType type;
    InputEvent input;
} IosTriggerEvent;

typedef struct {
    Bt* bt;
    Gui* gui;
    NotificationApp* notifications;
    ViewPort* view_port;
    bool connected;
    bool running;
    bool shooting;
    int shots;
    int delay; // in ms
} AppStruct;

static void draw_callback(Canvas* canvas, void* ctx);
static void input_callback(InputEvent* input_event, void* ctx);
static void timer_callback(FuriMessageQueue* event_queue);
static void bt_hid_connection_status_changed_callback(BtStatus status, void* context);
AppStruct* appStructAlloc();
void cleanUpBeforeYouLeave(AppStruct* app);
