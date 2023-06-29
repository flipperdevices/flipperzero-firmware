#pragma once

#include "bc_scanner_app.h"
#include "scenes/bc_scanner_scene.h"

#include <gui/gui.h>
#include <gui/icon_i.h>
#include <bc_scanner_icons.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <dialogs/dialogs.h>
#include <notification/notification_messages.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>
#include "views/bc_scanner_view.h"
#include "bc_scanner_icons.h"

#define RECORD_STORAGE "storage"
#define BC_SCANNER_APP_PATH_FOLDER ANY_PATH("bcscanner")
#define BC_SCANNER_APP_EXTENSION ".bc"

typedef enum {
    BarCodeAppErrorNoFiles,
    BarCodeAppErrorCloseRpc,
} BarCodeAppError;

struct BarCodeApp {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    NotificationApp* notifications;
    DialogsApp* dialogs;
    Widget* widget;

    BarCodeAppError error;
    FuriString* file_path;
    BarCodeView* bc_scanner_view;
    BarCodeScript* bc_scanner_script;
};

typedef enum {
    BarCodeAppViewError,
    BarCodeAppViewWork,
} BarCodeAppView;
