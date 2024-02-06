#ifndef __STATUS_SCENE_H__
#define __STATUS_SCENE_H__

#include <gui/scene_manager.h>

void scene_status_on_enter(void*);
bool scene_status_on_event(void*, SceneManagerEvent);
void scene_status_on_exit(void*);

#endif
