#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include "scenes/spi_mem_scene.h"

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    Submenu* submenu;
} SPIMemApp;

typedef enum { SPIMemViewSubmenu } SPIMemView;
