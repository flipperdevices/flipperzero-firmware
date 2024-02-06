#ifndef __RESET_SCENE_H__
#define __RESET_SCENE_H__

#include <gui/scene_manager.h>

void scene_reset_on_enter(void*);
bool scene_reset_on_event(void*, SceneManagerEvent);
void scene_reset_on_exit(void*);

#endif
