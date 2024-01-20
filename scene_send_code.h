#ifndef SCENE_SEND_CODE_HEADERS
#define SCENE_SEND_CODE_HEADERS

#include "flipper.h"

void fcom_send_code_scene_on_enter(void* context);
bool fcom_send_code_scene_on_event(void* context, SceneManagerEvent event);
void fcom_send_code_scene_on_exit(void* context);

#endif