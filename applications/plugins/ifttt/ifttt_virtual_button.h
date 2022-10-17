#pragma once

#include <furi.h>
#include <power/power_service/power.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>

#include "views/send_view.h"
#include "views/module_view.h"
#include "views/reboot_view.h"
#include "views/reset_view.h"
#include <gui/modules/submenu.h>
#include <gui/modules/dialog_ex.h>

#include "scenes/virtual_button_scene.h"

typedef struct {
    Power* power;
    Gui* gui;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    SendView* sen_view;
    ModuleView* modul_view;
    RebootView* reboo_view;
    ResetView* rese_view;
    Submenu* submenu;
    DialogEx* dialog;
    PowerInfo info;
} VirtualButtonApp;

typedef enum {
    VirtualButtonAppViewSendView,
    VirtualButtonAppViewModuleView,
    VirtualButtonAppViewRebootView,
    VirtualButtonAppViewResetView,
    VirtualButtonAppViewSubmenu,
    VirtualButtonAppViewDialog,
} VirtualButtonAppView;
