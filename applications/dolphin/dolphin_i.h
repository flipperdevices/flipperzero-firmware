#pragma once

#include "dolphin.h"

#include <flipper_v2.h>

#include <gui/gui.h>
#include <gui/widget.h>
#include <gui/canvas.h>
#include <menu/menu.h>

#include <assets_icons.h>

#include <stdint.h>

typedef struct {
    uint32_t ibutton;
    uint32_t nfc;
    uint32_t rfid;
    uint32_t ir;
} DolphinLimit;

typedef struct {
    uint32_t icounter;
    uint32_t butthurt;
} DolphinState;

typedef enum {
    DolphinEventTypeDeed,
} DolphinEventType;

typedef struct {

} DolphinEvent;

struct Dolphin {
    Icon* icon;
    Widget* widget;
    ValueMutex* menu_vm;

    DolphinLimit limit;
    DolphinState state;

    osMessageQueueId_t event_queue;
};

void dolphin_draw_callback(Canvas* canvas, void* context);

void dolphin_input_callback(InputEvent* event, void* context);

Dolphin* dolphin_alloc();
