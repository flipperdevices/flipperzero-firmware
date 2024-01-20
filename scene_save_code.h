#ifndef SCENE_HCSR04_HEADERS
#define SCENE_HCSR04_HEADERS

#include "flipper.h"

void fcom_save_code_scene_on_enter(void* context);
bool fcom_save_code_scene_on_event(void* context, SceneManagerEvent event);
void fcom_save_code_scene_on_exit(void* context);

#endif