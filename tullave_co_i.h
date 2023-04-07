#pragma once

#include "tullave_co.h"
#include "worker/tullave_worker.h"

#include "drv/tullave_drv/tullave_drv.h"

#include <furi.h>

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>

#include <notification/notification_messages.h>

#include <gui/modules/submenu.h>
#include <gui/modules/popup.h>

#include <assets_icons.h>

#include "scenes/tullave_co_scene.h"

typedef enum { TuLlaveViewMenu, TuLlaveViewPopup } TuLlaveView;

struct TuLlave {
    TuLlaveWorker* worker;
    NotificationApp* notifications;
    ViewDispatcher* view_dispatcher;
    Gui* gui;
    SceneManager* scene_manager;
    Submenu* submenu;
    Popup* popup;
};

TuLlave* tullave_alloc();

void tullave_free(TuLlave* t_llave);

int32_t tullave_co_main(void* p);

void tullave_blink_start(TuLlave* t_llave);

void tullave_blink_stop(TuLlave* t_llave);