#pragma once

#include <furi.h>
#include <gui/gui.h>

#include "dolphin/dolphin.h"
#include "dolphin/dolphin_i.h"
#include "dolphin/helpers/dolphin_state.h"
#include "dolphin/helpers/dolphin_deed.h"

#define TAG "Pet a Dolphin"

typedef struct {
    bool active;
    FuriThread* thread;
} PetFeeder;

typedef struct {
    Dolphin* dolphin;
    DolphinStats stats;
    PetFeeder pet_feeder;
    uint32_t counter;
} esAppData;

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} AppEvent;

typedef struct {
    bool run;

    FuriMutex** mutex;
    FuriMessageQueue* queue;
    FuriTimer* timer;

    ViewPort* view_port;
    Gui* gui;

    AppEvent event;

    esAppData* data;
} esApp;

esApp* es_app_alloc(void* render_callback, void* timer_callback);
void es_app_free(esApp* instance);