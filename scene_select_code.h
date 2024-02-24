/*
Opens a file browser to pick an fcom code file from the flipper file system
Reads that code, and then invokes the send code scene.
*/
#ifndef SCENE_SELECT_CODE_HEADERS
#define SCENE_SELECT_CODE_HEADERS

#include "flipper.h"

void fcom_select_code_scene_on_enter(void* context);
bool fcom_select_code_scene_on_event(void* context, SceneManagerEvent event);
void fcom_select_code_scene_on_exit(void* context);

#endif