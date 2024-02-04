#pragma once

#include <furi.h>
#include <gui/modules/submenu.h>
#include <gui/view.h>

typedef struct GenieSubmenu GenieSubmenu;
typedef struct GenieApp GenieApp;

uint32_t genie_navigation_submenu_callback(void* context);
GenieSubmenu* genie_submenu_alloc(GenieApp* app);
View* genie_submenu_get_view(GenieSubmenu* submenu);
void genie_submenu_free(GenieSubmenu* submenu);