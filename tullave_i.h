#pragma once

#include "tullave.h"
#include "lib/tullave/tullave_worker.h"

#include "lib/tullave/protocols/tullave_apdu.h"

#include <furi.h>

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>

#include <notification/notification_messages.h>

#include <gui/modules/submenu.h>
#include <gui/modules/popup.h>
#include <gui/modules/widget.h>

#include "scenes/tullave_scene.h"

// Requires to have fap_libs=["assets"] in the application.fam to read F0 asset icons.
#include <assets_icons.h>

typedef enum { TuLlaveViewMenu, TuLlaveViewWidget, TuLlaveViewPopup } TuLlaveView;

struct TuLlave {
    TuLlaveWorker* worker;
    NotificationApp* notifications;
    ViewDispatcher* view_dispatcher;
    Gui* gui;
    SceneManager* scene_manager;
    Widget* widget;
    Submenu* submenu;
    Popup* popup;
};

TuLlave* tullave_alloc();

void tullave_free(TuLlave* t_llave);

int32_t tullave_co_main(void* p);

void tullave_blink_start(TuLlave* t_llave);

void tullave_blink_stop(TuLlave* t_llave);