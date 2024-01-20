#ifndef SCENE_READ_CODE_HEADERS
#define SCENE_READ_CODE_HEADERS

#include "flipper.h"

void fcom_read_code_scene_on_enter(void* context);
bool fcom_read_code_scene_on_event(void* context, SceneManagerEvent event);
void fcom_read_code_scene_on_exit(void* context);

#endif