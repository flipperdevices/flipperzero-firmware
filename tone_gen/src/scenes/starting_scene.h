#ifndef _STARTING_SCENE_H_

#define _STARTING_SCENE_H_

#include <gui/scene_manager.h>

void scene_on_enter_starting_scene(void* context);
bool scene_on_event_starting_scene(void* context, SceneManagerEvent event);
void scene_on_exit_starting_scene(void* context);

#endif
