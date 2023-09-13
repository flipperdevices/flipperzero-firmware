#pragma once

#include <gui/view.h>
#include <gui/view_dispatcher.h>

#define FLIPBOARD_APP_MENU_VIEW_ID 0

typedef struct AppMenu AppMenu;

AppMenu* app_menu_alloc(ViewDispatcher* view_dispatcher);
View* app_menu_get_view(AppMenu* menu);
void app_menu_add_config(AppMenu* menu, View* config_view, uint32_t view_id);
void app_menu_add_item(AppMenu* menu, char* name, View* view, uint32_t view_id);
void app_menu_add_about(AppMenu* menu, char* about_text, uint32_t view_id);
void app_menu_show(AppMenu* menu);
void app_menu_free(AppMenu* menu);
