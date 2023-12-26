#ifndef _MAIN_MENU_H_

#define _MAIN_MENU_H_

#include <gui/scene_manager.h>

void scene_on_enter_main_menu(void* context);
bool scene_on_event_main_menu(void* context, SceneManagerEvent event);
void scene_on_exit_main_menu(void* context);

#endif
