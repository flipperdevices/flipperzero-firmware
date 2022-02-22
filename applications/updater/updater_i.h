#pragma once

//#include "updater.h"
#include "views/updater_main.h"
#include "util/update_task.h"

#include <furi.h>
#include <gui/gui.h>
#include <gui/view_stack.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/popup.h>
#include <gui/modules/code_input.h>
#include <gui/scene_manager.h>
#include <storage/storage.h>

typedef enum {
    UpdaterViewMain,
    UpdaterViewComplete,
    UpdaterViewTotal,
} UpdaterViewEnum;

typedef enum {
    UpdaterEventUnknown,
    UpdaterEventSdMounted,
    UpdaterEventSdUnmounted,
    UpdaterEventProgressUpdate,
} UpdaterEvent;

typedef struct {
    // Scene
    FuriThread* updater_thread;
    // GUI
    Gui* gui;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Storage* storage;
    //NotificationApp* notifications;

    //Popup* hw_mismatch_popup;

    //ViewStack* main_view_stack;
    UpdaterMainView* main_view;
    UpdateTask* update_task;
    //FuriPubSubSubscription* app_start_stop_subscription;
} Updater;

Updater* updater_alloc();

void updater_free(Updater* updater);
