#ifndef __ARHA_FLIPPERAPP_DEMO
#define __ARHA_FLIPPERAPP_DEMO

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <furi.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <dialogs/dialogs.h>
#include <input/input.h>
#include <storage/storage.h>
#include <stdlib.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

#include "sump.h"
#include "usb_uart.h"

#define TAG "LogicAnalyzer"

#define TIMER_HZ 50
#define TIMEOUT 3
#define QUEUE_SIZE 32

typedef enum { KeyNone, KeyUp, KeyRight, KeyDown, KeyLeft, KeyOK } KeyCode;

typedef enum { EventKeyPress, EventBufferFilled } EventType;

typedef struct {
    EventType type;
    InputEvent input;
} AppEvent;

typedef struct {
    FuriMessageQueue* event_queue;
    NotificationApp* notification;
    Storage* storage;
    ViewPort* view_port;
    Gui* gui;
    DialogsApp* dialogs;
    UsbUart* uart;
    Sump* sump;

    FuriMutex* mutex;
    bool triggered;
    bool processing;

    FuriThread* capture_thread;
    uint8_t* capture_buffer;
    size_t capture_pos;
    uint8_t current_levels;

    char state_string[64];
} AppFSM;

#endif