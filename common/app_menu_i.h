#pragma once

#include "app_menu.h"
#include <gui/modules/submenu.h>
#include <gui/modules/widget.h>

#define MAX_MENU_ITEMS 5

struct AppMenu {
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    uint8_t view_id_count;
    uint32_t view_ids[MAX_MENU_ITEMS];
};