#pragma once

#include "views/updater_main.h"
#include "util/update_task.h"

#include <furi.h>
#include <gui/gui.h>
#include <gui/view_stack.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/popup.h>
#include <gui/scene_manager.h>
#include <gui/modules/widget.h>
#include <storage/storage.h>
#include <update_util/update_hl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    UpdaterViewMain,
    UpdaterViewWidget,
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

    UpdaterManifestProcessingState* pending_update;
    UpdatePrepareResult preparation_result;

    UpdateTask* update_task;
    Widget* widget;
    string_t startup_arg;
} Updater;

Updater* updater_alloc(const char* arg);

void updater_free(Updater* updater);

#ifdef __cplusplus
}
#endif
