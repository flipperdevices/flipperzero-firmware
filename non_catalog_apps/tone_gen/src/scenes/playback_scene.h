#ifndef _PLAYBACK_SCENE_H_

#define _PLAYBACK_SCENE_H_

#include <gui/scene_manager.h>

void scene_on_enter_playback_scene(void* context);
bool scene_on_event_playback_scene(void* context, SceneManagerEvent event);
void scene_on_exit_playback_scene(void* context);

#endif
