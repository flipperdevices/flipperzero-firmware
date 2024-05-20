#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <notification/notification_messages.h>
#include <power/power_service/power.h>

#include "eth_worker.h"
#include "finik_eth_icons.h"

typedef enum {
    DRAW_ALL,
    DRAW_ONLY_TEXT,
    DRAW_ONLY_PICTURES,
    TOTAL_DRAW_MODES = 3,
} DrawMode;

typedef enum {
    PROCESS_INIT,
    PROCESS_DHCP,
    PROCESS_STATIC,
    PROCESS_PING,
    PROCESS_RESET,
} DrawProcess;

typedef enum {
    CURSOR_CHOOSE_PROCESS,
    CURSOR_CLICK_PROCESS,
    CURSOR_INSIDE_PROCESS,
    CURSOR_EXIT_ICON,
    CURSOR_EXIT,
} CursorPosition;

struct FinikEthApp {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    NotificationApp* notifications;
    EthWorker* eth_worker;

    Power* power;
    PowerInfo info;

    DrawMode draw_mode;
    DrawProcess draw_process;
    CursorPosition cursor_position;
};

typedef struct FinikEthApp FinikEthApp;
