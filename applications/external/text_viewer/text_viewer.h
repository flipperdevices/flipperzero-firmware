#pragma once

#include <gui/gui.h>
#include <storage/storage.h>
#include <dialogs/dialogs.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/widget.h>
#include "text_viewer_icons.h"
#include "scenes/text_viewer_scene.h"

#define TEXT_VIEWER_PATH STORAGE_EXT_PATH_PREFIX
#define TEXT_VIEWER_EXTENSION "*"

typedef struct {
    Gui* gui;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Widget* widget;

    FuriString* path;
} TextViewer;

typedef enum {
    TextViewerViewWidget,
} TextViewerView;
