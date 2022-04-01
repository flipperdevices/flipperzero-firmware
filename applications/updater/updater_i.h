#pragma once

//#include "updater.h"
#include "views/updater_main.h"
//#include "views/updater_loadcfg.h"
#include "util/update_task.h"

#include <furi.h>
#include <gui/gui.h>
#include <gui/view_stack.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/popup.h>
#include <gui/scene_manager.h>
#include <gui/modules/widget.h>
#include <storage/storage.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    UpdaterViewMain,
    //UpdaterViewLoadCfg,
    UpdaterViewWidget,
    //UpdaterViewTotal,
} UpdaterViewEnum;

typedef enum {
    UpdaterCustomEventUnknown,
    UpdaterCustomEventSdMounted,
    UpdaterCustomEventSdUnmounted,
    UpdaterCustomEventLoadManifest,
    UpdaterCustomEventApplyUpdate,
    UpdaterCustomEventCancelUpdate,
} UpdaterCustomEvent;

typedef struct UpdaterManifestProcessingState {
    //const char* manifest_path;
    UpdateManifest* manifest;
    string_t message;
    bool ready_to_be_applied;
} UpdaterManifestProcessingState;

typedef struct {
    // GUI
    Gui* gui;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Storage* storage;

    UpdaterMainView* main_view;

    //UpdaterLoadCfgView* loadcfg_view;
    UpdaterManifestProcessingState* pending_update;

    UpdateTask* update_task;
    Widget* widget;
    const char* startup_arg;
} Updater;

Updater* updater_alloc(const char* arg);

void updater_free(Updater* updater);

#ifdef __cplusplus
}
#endif
