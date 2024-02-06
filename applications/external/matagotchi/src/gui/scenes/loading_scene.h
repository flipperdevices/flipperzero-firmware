#ifndef __LOADING_SCENE_H__
#define __LOADING_SCENE_H__

#include <gui/scene_manager.h>

void scene_loading_on_enter(void*);
bool scene_loading_on_event(void*, SceneManagerEvent);
void scene_loading_on_exit(void*);

#endif
