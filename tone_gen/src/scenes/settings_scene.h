#ifndef _SETTINGS_SCENE_H_

#define _SETTINGS_SCENE_H_

#include <gui/scene_manager.h>

void scene_on_enter_settings_scene(void* context);
bool scene_on_event_settings_scene(void* context, SceneManagerEvent event);
void scene_on_exit_settings_scene(void* context);

#endif
