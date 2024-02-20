/**
 * @file app_i.h
 * @brief Main application header file.
 *
 * Contains defines, structure definitions and function prototypes
 * used throughout the whole application.
 */
#pragma once

#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>

#include <gui/modules/widget.h>
#include <gui/modules/submenu.h>

#include <storage/storage.h>

#include <notification/notification.h>

#include "scenes/scene.h"
#include "views/progress.h"
#include "flasher/flasher.h"

#define VGM_TOOL_TAG "VgmTool"

// This can be set by the build system to avoid manual code editing
#ifndef VGM_FW_VERSION
#define VGM_FW_VERSION "0.1.0"
#endif
#define VGM_FW_FILE_EXTENSION ".uf2"
#define VGM_FW_FILE_NAME "vgm-fw-" VGM_FW_VERSION VGM_FW_FILE_EXTENSION

#define VGM_DEFAULT_FW_FILE APP_ASSETS_PATH(VGM_FW_FILE_NAME)
#define VGM_FW_DEFAULT_PATH EXT_PATH("")

typedef struct {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;

    Widget* widget;
    Submenu* submenu;
    Progress* progress;

    NotificationApp* notification;

    FuriString* file_path;
    FlasherError flasher_error;
} App;

typedef enum {
    ViewIdWidget,
    ViewIdSubmenu,
    ViewIdProgress,

    ViewIdMax,
} ViewId;

void submenu_item_common_callback(void* context, uint32_t index);
