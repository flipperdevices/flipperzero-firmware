#pragma once

#include "tullave_co.h"

#include <core/log.h>

#include <furi.h>

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>

#include <gui/modules/submenu.h>
#include <gui/modules/popup.h>

#include <assets_icons.h>

#include <nfc/nfc.h>

#include "scenes/tullave_co_scene.h"

typedef enum { TuLlaveViewMenu, TuLlaveViewPopup } TuLlaveView;

struct TuLlave {
    ViewDispatcher* view_dispatcher;
    Gui* gui;
    SceneManager* scene_manager;
    Submenu* submenu;
    Popup* popup;
};

TuLlave* tullave_alloc();

void tullave_free(TuLlave* t_llave);

int32_t tullave_co_main(void* p);