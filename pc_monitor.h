#pragma once

#include <furi.h>
#include <furi_hal_bt.h>
#include <furi_hal_bt_serial.h>
#include <bt/bt_service/bt.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <notification/notification_messages.h>
#include <input/input.h>

#define TAG "PCMonitor"
#define BT_SERIAL_BUFFER_SIZE 128

typedef enum {
    BtStateChecking,
    BtStateInactive,
    BtStateWaiting,
    BtStateRecieving,
    BtSateLost
} BtState;

typedef struct {
    uint8_t cpu_usage;
    uint8_t gpu_usage;
    uint16_t ram_max;
    uint16_t ram_used;
    char ram_unit[8];
} DataStruct;

typedef struct {
    Bt* bt;
    Gui* gui;
    ViewPort* view_port;
    FuriMutex* app_mutex;
    FuriMessageQueue* event_queue;
    NotificationApp* notification;

    BtState bt_state;
    DataStruct data;
} PcMonitorApp;