#pragma once

#include "dolphin.h"
#include "dolphin_state.h"
#include "dolphin_views.h"

#include <flipper_v2.h>

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/canvas.h>
#include <menu/menu.h>

#include <assets_icons.h>
#include <stdint.h>

typedef enum {
    DolphinEventTypeDeed,
} DolphinEventType;

typedef struct {
    DolphinEventType type;
    union {
        DolphinDeed deed;
    };
} DolphinEvent;

struct Dolphin {
    // Internal message queue
    osMessageQueueId_t event_queue;
    // State
    DolphinState* state;
    // Menu
    ValueMutex* menu_vm;
    // GUI
    ViewDispatcher* idle_view_dispatcher;
    View* idle_view_main;
    View* idle_view_stats;
    View* idle_view_debug;
    ViewDispatcher* main_view_dispatcher;
    
};

Dolphin* dolphin_alloc();

